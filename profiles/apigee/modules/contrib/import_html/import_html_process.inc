<?php
/**
 * @file Actual routines for importing files.
 *
 *
 * @ingroup import_html Import HTML
 * @author Dan Morrison http://coders.co.nz/
 *
 */

module_load_include('inc', 'import_html', 'coders_php_library/debug');
module_load_include('inc', 'import_html', 'coders_php_library/xml-transform');
module_load_include('inc', 'import_html', 'coders_php_library/file-routines');

/**
 * Files have been selected, set them up for processing.
 *
 * @param $file_list
 *   an  array of simple file paths, probably selected from the file_list form.
 *   Relative to the import context settings.
 * @param $context
 *   A set of parameters, similar to the import_html profile, possibly from
 *   the list_filesystem form. Should contain the base path that the
 *   submitted files are relative to.
 * Note that context is NOT a full profile.
 *
 * @return A result set of nodes.
 */
function import_html_import_files($file_list, $context) {
  drupal_set_message(t('Processing %count files for import. Using %profile_id configuration settings.', array('%count' => count($file_list), '%profile_id' => $context['profile_id'])));

  if ( empty($file_list)) {
    drupal_set_message(__FUNCTION__ . ' ' . t("No Files Selected. Nothing to import"), 'error');
    return;
  }

  // Validate the profile, early exit if something is wrong.
  $profile = import_html_profile_load($context['profile_id']);
  if (! $profile) {
    drupal_set_message(t("Invalid profile_id '%profile_id' specified to %func. Aborting process.", array('%func' => __FUNCTION__, '%profile_id' => $context['profile_id'])), 'error');
    return NULL;
  }
  if (!import_html_profile_validate($profile)) {
    drupal_set_message(t("Invalid setting in '%profile_id' settings. Aborting process. See the reports log for details.", array('%profile_id' => $context['profile_id'])), 'error');
    return NULL;
  }


  // TODO see what we can do about clearing out our memory.

  $results = array();
  foreach ($file_list as $list_index => $rel_path) {
    // The $list_index is usually numeric.

    // $rel_path may still be empty
    // - representing the root item of the base_path.
    $file_results = import_html_import_file($rel_path, $context);

    // The results may be empty if the path scanned was just a directory
    // that had no immediate index page. The child items may have been queued
    // recursively, but no results are immediately returned in that case.
    if (! $file_results) {
      import_html_debug('
        Failed to get any results from the attempted analysis of %rel_path.
        The source file path was probably unavailable or not HTML.',
        array('%rel_path' => $rel_path),
        WATCHDOG_ERROR
      );
      continue;
    }
    // Result of importing a file MAY be more than one node,
    // unlikely as it may be for XHTML, but is possible for XML extension.
    import_html_debug_code("Result of processing file $rel_path", $file_results, WATCHDOG_DEBUG);

    foreach ($file_results as $node) {
      // Discard debug logs, try to save space.
      unset($node->file_data);
      $results[] = $node;
    }
  }

  if (! empty($results)) {
    // This isn't happening correctly until I visit admin?
    menu_rebuild();
    $profile = import_html_profile_load($context['profile_id']);
    list($menu_name, $mid) = explode(":", $profile['menu_parent_id']);
    import_html_debug(
      'Processed %filecount files for %nodecount nodes! The new pages may be found at !menu_link . or in your content management.',
      array(
        '!menu_link' => l($menu_name, 'admin/structure/menu-customize/' . $menu_name),
        '%filecount' => count($file_list),
        '%nodecount' => count($results),
      ),
      WATCHDOG_NOTICE
    );
  }
  else {
    import_html_debug('No results! Failed to extract any new nodes', array(), WATCHDOG_WARNING);
  }

  return $results;
}


/**
 * Given a html file, prepare all the node info we can get out of it.
 *
 * This func mainly prepares the paths and relative links.
 * Data extraction happens in _import_html_process_html_page()
 *
 * It does submit and save the node to the database.
 *
 * @param $context
 *   describes the context this function was called in. It should contain
 * 'profile_id' and optionally 'source_siteroot' or any other setting that you
 * want to override from the profile settings array. $context may be a full
 * import_html profile.
 *
 * profile_id MUST be valid or it will return NULL.
 *
 * Also 'form_id' if it's being used to just run a demo.
 *
 * Maybe a number of other import_html settings overrides.
 *
 * @return Array that may contain information about more than one node (in
 * extreme cases), but usually an array of 1. FALSE on error.
 *
 * NOTE - to try and
 * keep the footprint light, ONLY A SMALL AMOUNT of the created node object is
 * returned. Enough for auditing (nid and title), but not enough to analyze.
 */
function import_html_import_file($rel_path, $context) {
  // Read the profile id and use that as a context for all settings.
  $profile = import_html_profile_load($context['profile_id']);

  // Overlay our settings on to that profile.
  $profile = $context + $profile;

  // Sometimes the processed page needs to know where it's REALLY from.
  // - may support mirroring.
  $profile['source_siteroot'] = ensure_trailing_slash($profile['source_siteroot']);

  $dest_root = ensure_trailing_slash($profile['file_storage_path']);
  $is_remote = valid_url($profile['source_siteroot'], TRUE);

  $strings = array('%rel_path' => $rel_path);
  import_html_debug(
    "<strong>Importing</strong> '%rel_path'",
    $strings,
    WATCHDOG_NOTICE
  );
  // Return value is always an array of summaries of the results.
  $files = array();

  $source_path = $profile['source_siteroot'] . $rel_path;
  $save_as = safe_filepath_from_url($rel_path);
  $dest_path = preg_replace("|/+|", "/", $dest_root . $save_as);

  $strings['%source_path'] = $source_path;

  // Handle URLS/Folders with training slash

  // Handle trailing slashes differently at home and away.
  if ($is_remote) {
    // It's remote,
    if (preg_match("|/$|", $rel_path)) {
      // Need a dummy filename if retrieving default docs.
      $dest_path .= reset($profile['default_documents']);
    }
  }
  else {
    // It's local, but if it's a dir, need to handle it completely differently
    // Either recurse or batch the contents.
    if (is_dir($source_path)) {
      return import_html_import_directory($rel_path, $context);
    }
  }

  $strings['%dest_path'] = $dest_path;

  $file = array(
    'source' => $source_path,
    'dest' => $dest_path,
    'rel_path' => $rel_path,
  );

  // Handle files that are resources.
  // Copy them into the files folder and return.
  $checkfile = is_local($source_path) ? $source_path : $dest_path;
  // Can't use mime detection on remote lookups yet.
  // TODO - do some mime guessing on remote URL based just on URL? Only for demo, but also for spider later.
  if (is_local($source_path) && import_html_guess_file_class($checkfile) != 'html') {
    // Non-page resource - what sort of processing can I do here?

    import_html_debug(
      "I think (due to file suffix '%doctype')
        that '%source_path' is not a html page I can process.<br/>
        It's just been copied into '!dest'.",
      $strings + array(
      '!dest' => l($dest_path, $dest_path),
      '%doctype' => import_html_guess_file_class($checkfile),
    ),
      WATCHDOG_NOTICE
    );

    import_html_get_raw_file_local($source_path, $dest_path, $is_remote);
    $file['type'] = 'resource';
    $file['path'] = $dest_path;
    $files[] = $file;
    return $files;
  }


  // Compare the alias path of this new page with what we've already got.
  $new_path = _import_html_calc_path($rel_path, $profile);
  if (($normal_path = drupal_get_normal_path($new_path)) != $new_path) {
    $strings += array(
      '%new_path' => $new_path,
      '%normal_path' => $normal_path,
    );
    // We recognise that alias, thus an item already exists in that path.
    if ($profile['handle_duplicates'] == IMPORT_HTML_SKIP) {

      import_html_debug(
        "We already have '%new_path' in the system as '%normal_path'.
          According to import_html settings, this import is being skipped",
        $strings,
        WATCHDOG_INFO
      );

      return;
    }

    import_html_debug(
      "We already have '%new_path' in the system as '%normal_path'.
        Overwriting/updating it with the new import",
      $strings,
      WATCHDOG_INFO
    );
  }

  // Minor clean-up.
  // Helps recover from crashes and prevents temp files
  // getting renamed into file-01.etc
  if (is_file($dest_path) && ! $profile['keep_temp_files']) {
    unlink($dest_path);
  }

  // TODO
  // Resolve issue when there is a file called the same name as a folder
  // eg spidering off another Drupal site, /node is a PAGE with content,
  // but /node/1 is also.
  // ??
  // Forcably add a suffix to otherwise unsuffixed stuff?
  // is it a problem from wget?

  if (! is_file($dest_path) || ! $profile['keep_temp_files']) {
    // If getting raw_file from an http request, the $source and $dest paths
    // may get updated by ref
    // - eg to change 'about_us' to 'about_us/index.htm'.
    if (!import_html_get_raw_file_local($source_path, $dest_path, $is_remote)) {
      import_html_debug(
        "Failed to fetch a copy of %source_path into %dest_path",
        $strings,
        WATCHDOG_ERROR
      );
      return FALSE;
    }

    $strings['%persistant'] = ($profile['keep_temp_files'] ? 'persistant' : 'temporary');
    import_html_debug(
      "Fetched a %persistant local copy to %dest_path",
      $strings
    );

  }
  else {
    import_html_debug(
      "Local copy exists at %dest_path",
      $strings
    );
  }

  if ($is_remote) {
    // Importing a remote file - as for demo.
    // Relinking will happen to point back at where it came from, not here.
    // TODO need yet another parameter to indicate this, the path to neighbours.
    import_html_debug("Relinking this source will point back to the original URL context!");
  }

  // We have a local copy now.
  // $node initialized and processed HERE. Produces a node OBJECT.

  $nodes = import_html_process_html_page($dest_path, $rel_path, $profile);

  // At this point, the node(s) are full of data, but not yet saved.

  // On rare occasions, (using xt:document) the processing can produce
  // an ARRAY of nodes that need saving.
  // So we always work with $nodes as an array of nodes.
  // Almost always just one.

  if (empty($nodes)) {
    import_html_debug(
      "Failed to process any node out of file '%rel_path'",
      $strings,
      WATCHDOG_ERROR
    );
    return array();
  }

  // We can immediately discard the source file -
  // it should have been a temp copy made by import_html_get_raw_file_local() above
  if ( file_exists($dest_path) && ! $profile['keep_temp_files']) {
    unlink($dest_path);
  }

  // Almost trivial loop (probably over 1 item).
  foreach ($nodes as $node) {
    $strings = array(
      '%title' => $node->title,
      '!node' => l($node->title, $node->path_alias),
      '%path' => $node->path_alias,
      '%type' => $node->type,
      '%source_path' => $source_path,
    );

    // The node data object has been initialized
    // It may contain heaps of extra junk set in via a random absorbtion of elements in the XML import.
    // They will get ignored if not recognised.
    import_html_debug("Processed page to extract content. Title: <strong>'%title'</strong> ", $strings, WATCHDOG_INFO );
    import_html_debug_code("Ready to save node", $node, WATCHDOG_DEBUG);

    // If file logging was enabled, save the intermediate steps for heavy debugging.
    // Debug trace data.
    if ($profile['keep_temp_files']) {
      foreach ((array) @$node->file_data as $stage => $raw_data) {
        file_put_contents($dest_path . ".{$stage}.html", $raw_data);
      }
    }

    // If it's overwriting an existing path, merge values.
    $node = import_html_merge_over_existing_node($node, $profile);

    // Last-minute abort.

    if (!empty($node->import_html_delete)) {
      node_delete($node->nid);
      import_html_debug('DELETING unwanted node! %title, %path ' . $node->import_html_delete, $strings, WATCHDOG_WARNING);
      // Early exit
      continue;
    }

    // Contrib processes (especially the merge ones)
    // are allowed to say 'don't do this'
    // by setting the following flag.
    if (!empty($node->import_html_exclude)) {
      $strings['%exclude'] = $node->import_html_exclude;
      import_html_debug("Node %path will NOT be saved, as it has been flagged to be excluded [%exclude]", $strings, WATCHDOG_NOTICE);
      $files['rel_path'] = array(
        'type' => 'import_html_exclude',
        'rel_path' => $node->rel_path,
      );

      // Early exit.
      continue;
    }

    if (empty($node->body)) {
      import_html_debug("No body content found in this node. !node", $strings, WATCHDOG_WARNING);
    }

    // Validate that the node type is valid.
    // Import may have made some incorrect assumptions, require an unavailable
    // dependency, have made a typo or just set $node->type wrong.
    // Saving them in the database is hell to clean out again.
    $types = node_type_get_names();
    if (! in_array($node->type, array_keys($types))) {
      import_html_debug("Node type [%type] is invalid. %path will NOT be saved. Either there is a mistake somewhere or a required dependency or content type definition is unavailable", $strings, WATCHDOG_CRITICAL);
      // Early exit.
      continue;
    }

    // Finished prep, now save.

    // 'prepare' occasionally ensures that some required fields are filled in
    // depending on enabled modules. Maybe.
    // node_invoke_nodeapi($node, 'prepare');

    // I really should VALIDATE now!
    // but what to do with errors?
    // path_nodeapi complains if I try to validate before I know my nid.
    // Is that correct?
    // node_invoke_nodeapi($node, 'validate');

    // node_submit() doesn't actually save, it just fills in extra fields.
    // node_submit() 'tidies' $node->created for us. Work around that.
    // It's probably best if the preprocessors just set $node->date = string instead.
    if (isset($node->created) && empty($node->date)) {
      $node->date = format_date($node->created, 'custom', 'Y-m-d\TH:i:sP');
    }
    $node = node_submit($node);

    if (isset($context['form_id']) && $context['form_id'] == 'import_html_demo_form') {
      // DO NOT actually save stuff to the database
      $file['node'] = $node;
    }
    else {

      if ($errors = form_get_errors()) {

        import_html_debug(
          "Import of '%rel_path' did not quite validate. I'm not sure how to recover from that problem. <br/>!errors",
          array(
          '%rel_path' => $file['rel_path'],
          '!errors' => join(',<br/> ', $errors),
        ),
          WATCHDOG_ERROR
        );
        // TODO This is not very helpful in bulk mode.
        // What can I do now?
      }

      else {
        if (! empty($node->nid)) {
          import_html_debug(
            "!node Exists, updating it with content from %source_path.",
            $strings,
            WATCHDOG_INFO
          );
        }
        else {
          import_html_debug(
            "Inserting New Node !node with content from %source_path",
            $strings,
            WATCHDOG_INFO
          );
          #dpm($node);
          // Note, navigation items only gets set up on first import.
          // After that you are on your own.
        }

        node_save($node);

        // Call after_save hook.
        // Had to wait until I had an ID to do this.
        // These callbacks add the aliases and menus.

        //module_invoke_all('import_html_after_save', $profile, $node);
        // That fails due to pass-by-ref limitations in invoke_all
        // WORKAROUND for php 5.3
        foreach (module_implements('import_html_after_save') as $module) {
          $func = $module . '_import_html_after_save';
          $func($profile, $node);
        }

        // CORE BUG(?) ODDNESS
        // node_save will respect my $node->uid settings as far as node OWNERSHIP
        // but it also enters a revision entry that uses the global $user.
        // Boo.
        // Force it to get it right.
        // FAIL: _node_save_revision($node, $node->uid, TRUE);
        // INSTEAD:
        drupal_write_record('node_revisions', $node, 'vid');
      } // Finished updating database.

      import_html_debug_code("Saved node", $node, WATCHDOG_DEBUG);

      // Keep a mini copy for auditing (maybe not if memory gets heavy)
      // NOTE this mini-node is incomplete and only for auditing!
      $mini_node = (object) array();
      foreach (array('title', 'nid', 'path') as $att) {
        $mini_node->$att = $node->$att;
      }
      $file['node'] = $mini_node;

      $strings['%memory'] = format_size(memory_get_usage()) . '/' . format_size(memory_get_peak_usage());
      import_html_debug(
        "<strong>Imported Node</strong> !node with content from %source_path .\n  [mem: %memory]",
        $strings,
        WATCHDOG_NOTICE
      );

    } // Looped over all nodes from file.
    $files[] = $file;
  } // Looped over all files.

  return $files;
}

/**
 * Big brother to import_html_import_file
 *
 * Recursively imports ALL FILES in a given folder and returns a result array.
 *
 * This can be called directly as an API utility if you like.
 *
 * This does this immediately in normal flow, and really should be done in
 * batch. Try not to do this directly a lot.
 *
 * @param $context an array of settings. Must contain at least 'profile_id' and
 * 'source_siteroot'. MAY contain any import_html overrides.
 *
 * @return an array of summaries of the imported nodes.
 */
function import_html_import_directory($rel_path, $context) {
  // Read the profile id and use that as a context for all settings
  // Override those loaded settings with those given by $context.

  $profile = $context + import_html_profile_load($context['profile_id']);
  $source_siteroot = $profile['source_siteroot'];
  import_html_debug(
    "<strong>Importing Directory</strong> '%rel_path'",
    array('%rel_path' => $rel_path),
    WATCHDOG_INFO
  );
  $source_path = $source_siteroot . $rel_path;
  $results = array();
  if (is_dir($source_path)) {
    // Will scan and queue ALL sub-contents of this folder
    // List the immediate children non-recursive, don't get out of control
    $file_scan_options = array(
      'nomask' => import_html_file_exclusions_to_regexp($profile['file_exclusions']),
      'recurse' => FALSE
    );
    $dir_structure = import_html_file_scan_directory(trim_trailing_slash($source_path), "@.*@", $file_scan_options);
    $dir_structure = import_html_sort_directory($dir_structure, $profile);

    // If we are running under a batch process,
    // All sub-processes should be handed back to that to handle, in its own time.
    // Otherwise we may recurse without end.

    $path_list = array();
    foreach ($dir_structure as $filepath => $file) {
      // We have the full path, but need to knock it back to the rel path
      // before handing off to the normal file import
      $child_rel_path = str_replace($source_siteroot, '', $filepath);

      // If it's an index file, may as well do it immediately,
      // but otherwise queue most of the child items.
      if (in_array(basename($filepath), $profile['default_documents'])) {
        $result = import_html_import_file($child_rel_path, $context);
        $results = array_merge($results, (array) $result);
      }
      else {
        $path_list[$child_rel_path] = $child_rel_path;
      }
    }
    if (empty($path_list)) {
      import_html_debug('No sub-paths found when scanning %source_path (maybe just had a single index file)', array('%source_path' => $source_path), WATCHDOG_INFO);
    }

    // Invoke the appropriate batch queue operation.
    if (!empty($context['recursion_method'])) {
      import_html_debug(
        "Deferring processing of sub-pages, due to the recursion_method setting (%recursion_method). Directory:[%rel_path]'",
        array('%rel_path' => $rel_path, '%recursion_method' => $context['recursion_method']),
        WATCHDOG_INFO
      );
      $context['batch_id'] = $context['task_id'] ? $context['task_id'] : NULL;
      $callback_function = $context['recursion_method'] . '_add_list_to_queue';
      if (function_exists($callback_function)) {
        $result = $callback_function($path_list, $context);
        $results = array_merge($results, (array) $result);
      }
      else {
        import_html_debug(
          "A recursion method '%recursion_method' is defined, but no recusion func for that method was available. Child items are NOT being queued correctly.'",
          array('%recursion_method' => $context['recursion_method']),
          WATCHDOG_WARNING
        );
      }
    }
    else {
      // If *not* in a batch, just do it all now
      foreach ($path_list as $child_rel_path) {
        $result = import_html_import_file($child_rel_path, $context);
        $results = array_merge($results, $result);
      }
    }
  }
  else {
    import_html_debug('%source_path is not a directory. Incorrect argument or incomplete context given to %func', array('%source_path' => $source_path, '%func' => __FUNCTION__), WATCHDOG_WARNING);
  }
  return $results;
}


/**
 * Carefully fetch a (potentially remote?) file and save it nearby.
 *
 * If it was a directory of some description, we need to update the $dest_path
 * by ref.
 * If it was a remote http request that triggered a redirect (from dirname to
 * dirname/) then something needs to be changed.
 */
function import_html_get_raw_file_local(&$source_path, &$dest_path, $host) {
  mkdirs(dirname($dest_path), FILE_CREATE_DIRECTORY);
  if (is_dir($dest_path) || preg_match('|/$|', $source_path)) {
    // local copy needs to be inside a directory, not a directory itself
    $dest_path = "$dest_path/index.html";
    $source_path = ensure_trailing_slash($source_path);
  }
  if (! mkdirs(dirname($dest_path)) ) {
    trigger_error("Failed to create directory for $dest_path Might be permissions.", E_USER_ERROR);
  }

  $strings = array(
    '%source_path' => $source_path,
    '%dest_path' => $dest_path,
    '%location' => empty($host) ? 'local' : $host,
    '!realpath' => (empty($host) ? 'file://' : '') . realpath($source_path),
  );

  import_html_debug(
    "Fetching content from %location '<a href='!realpath'>%source_path</a>' now. Saving temp file locally as %dest_path",
    $strings,
    WATCHDOG_INFO
  );

  $orig_path = $source_path;

  if ($host) {
    // It's remote. Trust PHP5 and allow_url_fopen is available.
    if (!copy($source_path, $dest_path)) {
      import_html_debug(
        "Remote file copy from %source_path to %dest_path failed",
        $strings,
        WATCHDOG_ERROR
      );
      return FALSE;
    }
  }
  else {
    // local copy
    if (! is_file($source_path) ) {
      import_html_debug(
        "Source file %source_path is not a normal file. Returning a failure.",
        $strings,
        WATCHDOG_ERROR
      );
      return FALSE;
    }
    if (! is_writable(dirname($dest_path)) ) {
      import_html_debug(
        "Cannot copy file to %dest_path, destination not writable.",
        $strings,
        WATCHDOG_ERROR
      );
      return FALSE;
    }

    if (file_exists($source_path) && realpath($source_path) == realpath($dest_path)) {
      import_html_debug(
        "Copying between identical source and destination, %source_path = %dest_path , importing file in-place.",
        $strings,
        WATCHDOG_INFO
      );
    }

    if (! copy($source_path, $dest_path) ) {
      import_html_debug(
        'Local file copy failed (%source_path to %dest_path).',
        $strings,
        WATCHDOG_ERROR
      );
    }
  }
  // Check the actual result, not the return from copy().
  if (! is_file($dest_path)) {
    import_html_debug(
      'Local file copy failed (%source_path to %dest_path).
      Source %source_path is <pre>!source_stat</pre>
      Dest folder %dest_path is <pre>!dest_stat</pre>
      ',
      $strings + array(
      '!source_stat' => print_r(stat($source_path), 1),
      '!dest_stat' => print_r(stat(dirname($dest_path)), 1),
    ),
      WATCHDOG_ERROR
    );

    return FALSE;
  }

  import_html_debug(
    "Copied import file from %source_path to %dest_path",
    $strings,
    WATCHDOG_INFO
  );
  return TRUE;
}


/**
 * Analyse a source page and create a node definition from it.
 *
 * Most of the processing magic is in here.
 * The $node handle may be provided initialized with some pre-set values.
 * The $node may come in as an array or an object.
 * Internally we should continue using the object methods.
 *
 * This processing is still in the 'validate' phase, so should
 * not cause anything to happen, just configure the node object. NO SAVING
 * happens yet.
 *
 * @param $path/$node
 *   the file (or object) to read the data from. If it's a string, it's taken
 * to be the filename, if an object, it's the node. A node should contain a -
 * >body (or ->raw_html) and a - >path at least.
 * @param $rel_path
 *   Where this html page was found, relative to its own server root. This is
 * used to rewrite its urls. If the path is a directory, it should end with a
 * slash. ( /a/path/ == /a/path/index.html != /a/path )
 * @param $profile
 *   The  settings for this import process.
 *
 * @return array containing the new node object as the first item. Some
 * processes may return multiple nodes, eg an XML document that gets processed
 * into multiple items.
 */
function import_html_process_html_page($path, $rel_path, $profile) {
  if (!init_xsl()) {
    trigger_error("Sorry, with no XML support there will be no content scanning AT ALL. Aborting process. See the import_html_help.htm for info on enabling XML under PHP.", E_USER_ERROR);
    return;
  }
  $strings = array(
    '!rel_path' => l($rel_path, IMPORT_HTML_ADMIN_PATH . '/file_preview/' . $rel_path),
    '%path' => $path,
  );
  import_html_debug_code("Starting to process a file. The import profile settings being used to import_html_process_html_page($rel_path)", $profile);

  if (is_string($path)) {
    // Read from file
    import_html_debug(
      "Processing file as HTML page.
        Full file path: %path , will be imported as a relative path
        under the current section.
        Relative-path is: !rel_path",
        $strings,
        WATCHDOG_INFO
    );

    if (! file_exists($path)) {
      trigger_error(t("
        Path '%path' was not found.
        This should have been a local copy of the file being imported,
        but the paths may be wrong somehow. Abject failure processing !rel_path"
      , $strings));
    }

    $raw_source = file_get_contents($path);

    // Truly terrible input can sometimes only be repaired with string
    // manipulation before we even set the tools on it
    // (like the worst of MSWord save as HTML)
    // So run pure string-fix callbacks over our input, as the very first thing.

    foreach (module_implements('import_html_preprocess') as $module) {
      $func = $module . '_import_html_preprocess';
      $raw_source = $func($profile, $raw_source);
    }

    if (empty($raw_source)) {
      // Well, something is very wrong here. Give up immediately.
      import_html_debug("
        Import_HTML failed to load or process initial HTML string input from
        !rel_path.
        Either file was empty, unreadable, or the preprocess functions
        returned NULL.
        Aborting import on this file (%path)",
        $strings,
        WATCHDOG_ERROR
      );
      return FALSE;
    }

    /*
     * Trying to parse pure XML first is causing problems
     * Either I want everything to be html, (always tidy)
     * or I allow for exsl:document blocks (which can't be tidied)
     * Option for now is try to parse, and only tidy if that fails.
     *
     */
    // temporarily ignore parser errors (catch?)
    set_error_handler('stfu');
    $xmldoc = parse_in_xml_string($raw_source, $profile['force_tidy']);
    restore_error_handler();

    if (! $xmldoc && $profile['force_tidy'] ) {
      import_html_debug(
        "%path was not tidy enough - running tidy over it now so I can parse it.",
        $strings
      );
      // If a raw XML parse failed,
      // tell parse_in_xml_file() to use htmlTidy before it begins.
      // TODO - add a flag to skip this double-processing, (parsing twice)
      // It may be a bit slow if it's not often used.
      $xmldoc = parse_in_xml_string($raw_source, TRUE);
    }
    #import_html_debug_code("Finished reading from file:", xml_tostring($xmldoc));
    $source_node = new stdClass();
  }
  else {
    // Argument is NOT a filepath.
    // We may have passed in a source-node object where the path was expected
    // instead.
    // A bit of a sneak. The given node has the source HTML in $node->raw_html
    if (is_object($path)) {
      $source_node = $path;
      $path = $source_node->path_alias;
      if (! $source_node->raw_html) {
        trigger_error(t("import_html_process_html_page called with no HTML source to analyse"), E_USER_ERROR);
      }
      import_html_debug("Processing page source, " . strlen($source_node->raw_html) . " chars long", array());
      $xmldoc = parse_in_xml_string($source_node->raw_html, $profile['force_tidy']);
    }
  }

  // Debug trace data.
  if ($profile['keep_temp_files']) {
    $source_node->file_data['1-after_preprocessing'] = $raw_source;
  }

  if (!$xmldoc) {
    // Parsing failed.
    import_html_debug("
      Import_HTML failed to initialize or parse XMLdoc input.
      !rel_path seems like bad news.",
      $strings, WATCHDOG_ERROR
    );
    return FALSE;
  }

  import_html_debug_code("PARSED XML $path . XHTML", xml_tostring($xmldoc));

  // Start massaging the XML.

  if ($profile['rewrite_links']) {
    // use XSL to rewrite links to fit into Drupal
    $xmldoc = import_html_rewrite_links($xmldoc, $rel_path, $profile);
  }
  if ($profile['strip_tables']) {
    $xmldoc = import_html_strip_tables($xmldoc);
  }
  if ($profile['cleanup_namespaces']) {
    // use XSL to rewrite links to fit into Drupal
    $xmldoc = import_html_cleanup_namespaces($xmldoc);
  }

  // Debug trace data.
  if ($profile['keep_temp_files']) {
    $source_node->file_data['2-after_rewriting'] = xml_tostring($xmldoc);
  }

  // Import content as node.
  // Translate the source text to the known tidy simple, tagged HTML structure.
  $parameters = array(
    'xmlid' => TRUE,
    'xsl_path' => $profile['translation_template'],
  );
  if ( !empty($profile['content_tag_id'])) {
    $parameters['contentid'] = $profile['content_tag_id'];
  }

  if ($xsldoc = _import_html_get_xsl_doc($profile['translation_template'])) {
    // For info only:
    $xml_top = $xmldoc->firstChild;
    $xsl_top = $xsldoc->firstChild;
    import_html_debug("
      Using XSL translation template to extract semantic content.
      Will search for body content labelled '" . @$parameters['contentid']
      . "' in the source.
      Active XML Namespaces are
      {$xml_top->nodeName} : {$xml_top->namespaceURI} -
      {$xsl_top->nodeName} : {$xsl_top->namespaceURI}  \n"
      , array(), WATCHDOG_DEBUG);

    $importxml = xmldoc_plus_xsldoc($xmldoc, $xsldoc, $parameters);

    if ($profile['keep_temp_files']) {
      $source_node->file_data['3-after_templating'] = $importxml;
    }

    import_html_debug("
      Transform Successful.
      TRANSLATED %path from messy source into a pure xhtml page to import",
      $strings
    );
  }
  else {
    // get_xsl_doc failed.
    trigger_error(t("
      Failed to initialize XSLdoc '%translation_template'. Aborting.",
      array('%translation_template' => $profile['translation_template'])),
      E_USER_WARNING
    );
    // Abort.
    return NULL;
  }

  if (! $importxml) {
    trigger_error("Nothing useful extracted via XML from that content", E_USER_WARNING);
    return FALSE;
  }

  $xmldoc = parse_in_xml_string($importxml, FALSE);

  // Allow add-on modules to manipulate the DOM before XSL parsing.
  foreach (module_implements('import_html_preparse') as $module) {
    $func = $module . '_import_html_preparse';
    $func($profile, $xmldoc);
  }

  if ($profile['keep_temp_files']) {
    $source_node->file_data['4-after_preparse'] = xml_tostring($xmldoc);
  }

  // D7 requires that all nodes have a language set. Fix that before we proceed.
  // Requred for body and path (and everything) to be set right.
  // Nope, this meant nothing saved successfully. How AM I expected to save lanuage tagged content?
  // $source_node->language = language_default('language');
  $source_node->language = LANGUAGE_NONE;
  #import_html_debug("Setting node language to %language", array('%language' => $source_node->language), WATCHDOG_DEBUG);

  // Allow one source document to produce multiple nodes.
  $nodes = array();
  // If the process has resulted in xt:document blocks, each block
  // is a new item.
  // Either there is a html element in the input ... or many of them.

  $html_elements = xml_getelementsbytagname($xmldoc, 'html');
  import_html_debug("Found " . count($html_elements) . " html elements in source doc", array(), WATCHDOG_DEBUG);

  // Probably only one, but we'll iterate over an array of one then.
  foreach ($html_elements as $html_element) {

    import_html_debug_code('XML simple XHTML source ready to have its semantics extracted', xml_toString($xmldoc));
    $source_node->rel_path = $rel_path;

    // path.module is not always available (doh!) so set our path here,
    // not in path_import_html hook.
    if (empty($node->path_alias)) {
      $source_node->path_alias = _import_html_calc_path($source_node->rel_path, $profile, FALSE);
      $source_node->old_path = _import_html_calc_path($source_node->rel_path, $profile, TRUE);
    }
    // May need extra care when creating multiples.
    // Invent new paths for the new documents if the exsl:document didn't define them
    if (isset($nodes[$source_node->path_alias])) {
      // already using this path, extend a new one
      $source_node->path_alias .= '/' . import_html_check_name(!empty($source_node->label) ? $source_node->label : $source_node->title);
    }


    ////////////////////////////////////
    // Do most of the content logic:
    // This will also invoke the per-module hooks.
    //
    $node = import_html_xhtml_to_node($html_element, $source_node, $profile);
    //
    ////////////////////////////////////

    // The rest is housekeeping or generic.
    // Most core node properties (type, modified) are set in node_import_html()

    $node->title = import_html_guess_document_title($node, $profile['handle_no_title']);
    $node->status = $profile['import_status'];
    $node->promote = $profile['import_promote'];
    if (! $node->type) {
      import_html_debug("Node wasn't even given a type, this means something is wrong.", array(), WATCHDOG_WARNING);
      $node->type = $profile['content_type'];
    }

    // Debug trace data
    if ($profile['keep_temp_files']) {
      $node->file_data['5-data_structure'] = print_r($node, 1);
    }
    $nodes[$node->path_alias] = $node;

    import_html_debug("Path to save this page as is %path", array('%path' => $path), WATCHDOG_INFO);
  }

  return $nodes;
}



/**
 * From a given XML document, create a node structure with all useful
 * parameters set.
 *
 * A shell node object may be passed in with some values already set. The data
 * extracted from the XHTML structure will be layered onto that.
 *
 * Here is where we map HTML info to node data, like H1 -> $node->title
 * TODO tidy this up with a lookup table or something
 *
 * Node may have defined its own $node->type even.
 *
 * Called by
 * @see import_html_process_html_page()
 *
 * THIS IS THE ENGINE OF IMPORT_HTML
 *
 * @param $datadoc
 *   An XML document containing the whole source data
 * @param $node
 *   A partial node object, ready to have other bits added to it. It should be
 * already filled out with the most important information - like $node->path_alias
 * @param $profile
 *   A set of settings and preferences for the import_html process currently
 * underway. May include some context information like paths.
 *
 */
function import_html_xhtml_to_node($datadoc, $node, $profile) {
  import_html_debug("Importing from XML object to node object");

  // Remember this?. TODO - check if it hurts performance.
  // $node->datadoc = $datadoc;

  $node = $node ? $node : new stdClass();
  // Need to set a default valid type before import, although it could get redefined
  // based on page parameters.
  $node->type = (!empty($node->type) && is_string($node->type)) ? $node->type : $profile['content_type'];

  // Debug notes/trace logs. Can be removed.
  if ($profile['keep_temp_files']) {
    $node->file_data['raw_xhtml'] = xml_toString($datadoc);
  }

  // Now read the input into node structure.
  //
  // Absorb the most generic bits first. Later processes may overwrite them
  // more accurately.

  // This initial import is a totally generic catch-all.
  // Optional - this can get madly messy on complex modern sites.
  // TODO - separate ids (few) and classes (hundreds)
  if ($profile['absorb_all_tagged_elements']) {
    import_html_absorb_all_tagged_elements($node, $datadoc);
  }

  //
  // Get all metas as properties.
  //
  $head_element = xml_getelementsbytagname($datadoc, 'head', TRUE);

  // Allow ALL values I find (some may get lost later)
  import_html_absorb_metas($node, $head_element, 'meta', 'name', 'content');
  import_html_absorb_metas($node, $head_element, 'link', 'rel', 'href');

  // Most of the simple data is now loaded into a keyed array in $node->import_html.

  // This import all may have discovered a preferred $node->type
  // from meta (which is good) or css ids (which are possibly accidental)
  // We may have an array, which needs flattening and validation.
  // Do this asap as most other functions like to know the node type.

  if (isset($node->import_html['type'])) {
    // Scan the values, ensure the named type is valid.
    // Use the LAST valid one found.
    $possible_types = is_array($node->type) ? $node->type : array($node->type);
    foreach ($possible_types as $possible_type) {
      if (node_type_get_type($possible_type)) {
        $node->type = $possible_type;
        import_html_debug("Overriding type to %possible_type", array('%possible_type' => $possible_type), WATCHDOG_INFO);
      }
    }
  }

  // If there are any other things to come from HTML into $node,
  // let me know now!
  // Loop over a buch of hook-like per-module extensions.
  // MENU, PATH, TAXONOMY, CCK all add values in their own callbacks in
  // import_html_modules.inc
  // Also the core node elements - body, title, teaser get set in a 'core'
  // node callback.

  import_html_include_add_on_module_handlers();
  import_html_debug_code(
    "Invoking hook to let all modules change this node as I'm creating it",
    module_implements('import_html')
  );

  // Call anything that provides HOOK_import_html implimentations.
  // module_invoke_all seems to be failing badly.
  // module_invoke_all('import_html', $profile, $node, $datadoc);
  // PROBABLY due to a pass-by-reference limitation in call_user_func_array()
  // http://drupal.org/node/321160
  // Need to test over PHP versions? Failed on 5.3.2, so we do it by hand.
  // WORKAROUND for php 5.3
  foreach (module_implements('import_html') as $module) {
    $func = $module . '_import_html';
    import_html_debug("Invoking $func", array(), WATCHDOG_DEBUG);
    $func($profile, $node, $datadoc);
  }

  import_html_debug_code(
      "After absorbing all the tagged items found in the raw source, the
       available data array has the following keys:
      ",
      array_keys($node->import_html),
      WATCHDOG_DEBUG
  );

  // Clean this up for a clearer debug message
  $debug_node = clone $node;
  unset($debug_node->file_data);
  import_html_debug_code(
    "After absorbing absolutely everything I could find,
    the node object now contains the following blocks and bits:",
    $debug_node,
    WATCHDOG_DEBUG
  );
  unset($debug_node);

  return $node;
}

/**
 * Import ALL tagged classes and IDs as node attributes.
 *
 * If the input has ANY id or classes at all, grab that info and apply it to
 * this object. Assume anything important enough to have a label is important
 * enough to remember.
 *
 * This will probably produce a very cloggy node, filled with trash, Possibly
 * even some arrays where there shouldn't be. But any unrecognised property
 * names will be discarded on save, leaving only the serializable values. This
 * approach will allow arbitrary data to come and go in the future.
 *
 * @param $node Handle on the node object being updated.
 * @param $datadoc XML DOM Document being processed.
 * @return void Updates $node by reference.
 */
function import_html_absorb_all_tagged_elements(&$node, $datadoc) {

  // Scrape the whole doc for anything that looks faintly semantic
  foreach (array('class', 'property', 'typeof', 'id') as $attribute_label) {

    import_html_debug(
      "Absorbing all elements with an %attribute_label
      as incidental data blobs (possibly html) into node structure",
      array('%attribute_label' => $attribute_label)
    );
    $found_elements = xml_query($datadoc, './/*[@' . $attribute_label . ']');

    // I now have a collection of tagged nodes.
    foreach ($found_elements as $found_element) {

      $attribute_value = xml_getattribute($found_element, $attribute_label);
      // If it was a class, it may be multiple!
      // Usually just one however...
      $keys = explode(' ', $attribute_value);

      foreach ($keys as $key) {
        // Found 'something' labelled 'something'.
        if (! trim($key)) {
          continue;
        }

        // Allow HTML though. Sometimes this will not be right...
        // TODO, figure it out?
        $value = xml_tostring($found_element, TRUE);
        if (! trim($value)) {
          continue;
        }

        // The value just gets absorbed.
        import_html_debug(
          "Found a tagged %attribute_label value - %key ,
            Absorbing it into the node as a default text/html value",
          array('%attribute_label' => $attribute_label, '%key' => $key),
          WATCHDOG_DEBUG
        );

        // Set it onto the node data list.
        // Treat anything we find as though it were potentially multiple.
        // So always stick it in an array, even if just an array of one.
        $node->import_html[$key][] = $value;
        // There can be only one ID.
        // Ensure this - this will protect us from detecting
        // <div class="teaser" id="teaser"> as two entries
        // It does happen like this sometimes.
        if ($attribute_label == 'id') {
          $node->import_html[$key] = array($value);
        }


      } // Each multiple key.
    } // Each found element.
  } // Each attribute type.
}

/**
 * Scan a given dom object for metas of a certain persuasion, and add all found
 * key-values to the $node.
 *
 * Supports different metas, like
 * <meta name="key" content="value" />
 * or
 * <rel type="top" href="url" />
 *
 * import_html_absorb_metas($node, $htmlnode, 'meta', 'name', 'content');
 * import_html_absorb_metas($node, $htmlnode, 'rel', 'type', 'href');
 *
 * ...
 * .. would result in :
 *
 * $node->key='value';
 * $node->top='url';
 *
 *
 */
function import_html_absorb_metas(&$node, $xml_element, $tagname, $keyname, $valname) {

  import_html_debug(
    "Absorbing the '%valname' of '%tagname's with a '%keyname'
      from source doc into node structure",
    array(
    '%valname' => $valname,
    '%tagname' => $tagname,
    '%keyname' => $keyname,
  ),
    WATCHDOG_DEBUG
  );

  $metas = xml_getelementsbytagname($xml_element, $tagname);
  foreach ($metas as $meta) {
    if (empty($meta)) {
      continue;
    }
    $key = xml_getattribute($meta, $keyname);
    $value = xml_getattribute($meta, $valname);
    if ($key && $value) {
      // Metas may be name="created" or name="DC.Date.Created"
      $keys = array($key);
      // Squash them as well, I can't always be bothered with namespaces
      $bits = explode('.', $key);
      if (count($bits) > 1) {
        $keys[] = strtolower(array_pop($bits));
      }
      foreach ($keys as $key) {
        import_html_absorb_properties($node, $key, $value);
        import_html_debug(
          "Absorbed '%tagname':'%valname' from source doc,
          (%key='%value') .",
          array(
            '%valname' => $valname,
            '%tagname' => $tagname,
            '%keyname' => $keyname,
            '%key' => $key,
            '%value' => $value,
          ),
          WATCHDOG_INFO
        );
      }

    }
    else {
      import_html_debug(
        "When absorbing '%valname' from '%tagname's with a '%keyname' from source doc,
        (%key='%value') had a null value. Not a great problem, just letting you know.",
        array(
        '%valname' => $valname,
        '%tagname' => $tagname,
        '%keyname' => $keyname,
        '%key' => $key,
        '%value' => $value,
      ),
        WATCHDOG_INFO
      );
    }
  }
}


/**
 * Set the given property on the given object,
 * allowing multiple values to expand into arrays.
 *
 * Happens automatically IFF more than one key match is found. Deal with that
 * yourself.
 */
function import_html_absorb_properties(&$node, $key, $value) {
  if (!$key) {
    import_html_debug("Odd, when absorbing properties, value:'$value' is a value for what key? The calling function passed a null key to be absorbed.", array(), WATCHDOG_NOTICE);
    return;
  }
  if (!$value) {
    import_html_debug("Odd, when absorbing properties, '$key' had a null value. This is probably not an error.", array(), WATCHDOG_NOTICE);
    return;
  }

  // ALWAYS store this data as an array for consistency
  $node->import_html[$key][] = $value;

  /*
REMOVE
  // Auto-expand into arrays - most metas can legally have duplicates
  if ( ! isset($node->import_html[$key]) ) {
    $node->import_html[$key] = $value;
  }
  elseif ( is_array($node->import_html[$key]) ) {
    $a = $node->import_html[$key];
    $a[] = $value;
    $node->import_html[$key] = $a;
  }
  else {
    $node->import_html[$key] = array($node->import_html[$key], $value);
  }
  */
}

/**
 * Include what we can find in the /modules directory.
 * Only once.
 *
 * Due to D7 hook registry caching, this can no longer be lazy-loaded.
 */
function import_html_include_add_on_module_handlers() {
  static $done;
  if ($done) {
    return;
  }
  // Scan add-on dir and include all bits found there
  $inc_files = file_scan_directory(drupal_get_path('module', 'import_html') . '/modules', "/.*.inc/");
  foreach ($inc_files as $inc_path) {
    include_once DRUPAL_ROOT . '/' . $inc_path->uri;
  }
  $done = TRUE;
}

/**
 * Ensure the node has a valid title. It's really important.
 *
 * If none is set, fill in a default.
 */
function import_html_guess_document_title($node, $handle_no_title = IMPORT_HTML_GUESS) {
  if (empty($node->title) ) {
    import_html_debug(
      "Failed to extract a useful title for this node, falling back to a default value.",
      array(),
      WATCHDOG_NOTICE
    );
    switch ($handle_no_title) {
      case IMPORT_HTML_GUESS:
        return import_html_guess_label('', $node->path_alias);
        break;
      case IMPORT_HTML_DEFAULT:
        return 'Untitled Document';
        break;
    }
  }
  return $node->title;
}

/**
 * Create a human-readable string from a filename or similar, by replacing
 * underscores with spaces, removing suffix etc.
 */
function import_html_guess_label($title, $path) {
  $label = $title;
  // Need to beware of stupid long titles
  // - they can't fit in breadcrumbs and menus
  // If your TITLE is long, prefer to drop back to filename,
  // which may be more concise and meaningful.
  // Truncating a long title often loses any info it had.
  if (strlen($title) > IMPORT_HTML_MAX_LABEL_LENGTH || empty($title) ) {
    if (! empty($path)) { // Yeah. No path.module, no path.
      $path_bits = explode('/', $path);
      $label = array_pop($path_bits);
      if (!$label) {
        // it had a trailing slash
        $label = array_pop($path_bits);
      }
      $label = preg_replace('/\?.*$/', '?', $label); // messiness from mirrored URLs with args in
      // TODO maybe adjust this title-munging algoritm to make better guesses
      $label = str_replace('_', ' ', $label);
      $label = (strstr($label, '.')) ? substr($label, 0, strrpos($label, ".")) : $label;
    }
    else {
      // Long title but no path. OK, truncate the title.
      // Should I user node_teaser($title, NULL, IMPORT_HTML_MAX_LABEL_LENGTH)
      // Or my own one-liner to break on spaces.
      $label = preg_replace('/\s+?(\S+)?$/', '', substr($title, 0, IMPORT_HTML_MAX_LABEL_LENGTH));
      if (empty($label)) {
        // A really long word? chop then
        $label = substr($title, 0, IMPORT_HTML_MAX_LABEL_LENGTH);
      }
    }
  }
  return $label;
}

/**
 * Trims the path if the path is a default document - one listed in the profiles
 * 'default_documents' array.
 *
 * Returns if that happened.
 */
function import_html_check_default_document(&$path, $default_documents = array('index.htm')) {
  // Default documents are linked to their parent directories.
  // Trailing slash is tricky.
  // /this/path is a whole navigation level above
  // /this/path/ and will resolve relative links differently!
  // Do we need to actually redirect, not just alias any links like that?
  $filename = basename($path);
  if (in_array($filename, $default_documents)) {
    $trimmed_path = dirname($path);
    if ($trimmed_path == '.') {
      $trimmed_path = 'home';
    }
    import_html_debug(
      "'%path' is an index page, so we will chop it back to %trimmed_path",
      array('%path' => $path, '%trimmed_path' => $trimmed_path),
      WATCHDOG_INFO
    );
    $path = $trimmed_path;
    return TRUE;
  }
  import_html_debug(
    "'%path' is not an index page",
    array('%path' => $path),
    WATCHDOG_INFO
  );
  return FALSE;
}

/**
 * Return the nice path alias of an imported page.
 *
 * Simplify a legacy URL path into something better looking.
 */
function _import_html_calc_path($rel_path, $profile, $leave_suffix = FALSE) {
  $path = ( isset($profile['import_site_prefix']) ? $profile['import_site_prefix'] : '' ) . preg_replace('|^/|', '', $rel_path);
  $path = urldecode($path);

  if ($leave_suffix) {
    return $path;
  }

  if (!empty($profile['trim_suffixes'])) {
    // Simplify the URL if possible by trimming the suffix and 'index'
    // but remember the original path somewhere, we'ill need to link it forward
    // once the new node is established.

    if (import_html_check_default_document($path, $profile['default_documents'])) {
      // The check will have trimmed blah/index.html back to blah for us.
      // Carry on.
    }
    else {
      // No change, Chop suffix instead.
      // Take care - don't break a path like
      // /path/site-mirror/drupal.org/about
      // or path/contact-us/index.aspx?cat=feedback'
      // incorrectly.
      // So make sure that we split off the basename,
      // chop its suffix, then glue it back onto the dirname.
      // $path = (! empty($path) ? dirname($path) .'/' : '') . preg_replace('|\.[^\.]+$|', "", basename($path));
      // Alternatively - avoid matching back up the tree.
      $path = preg_replace('|([^/]*)\.[^\.\?]+$|', '$1', $path);

      // If the input came from wget, we may get "/folder/sectionname.1.html"
      // Drop any trailing .number
      $path = preg_replace('|\.\d$|', '', $path);


      // Or should use real URL parsing?
      // TODO Shall we support ? and # in URLS when trimming suffix?
    }
  }

  return $path;
}

/**
 * Find and initialize the transformation template.
 *
 * Includes caching retrieval for a bit of speed-up over bulks.
 *
 * @return XML Document
 */
function _import_html_get_xsl_doc($xslfile) {
  static $xsldocs;
  if (isset($xsldocs[$xslfile])) {
    return $xsldocs[$xslfile];
  }
  if (empty($xslfile)) {
    import_html_debug("Null XSL stylesheet? Cannot proceed", array(), WATCHDOG_ERROR);
  }

  // Check if and where filepath can be found.
  // Search first under full path, then module dir, then under files dir.
  $xslfilepath = $xslfile;
  if (!is_file($xslfilepath)) {
    $xslfilepath = drupal_get_path('module', 'import_html') . "/$xslfile";
  }
  if (!is_file($xslfilepath)) {
    // TODO - update to D7 filesystem
    $xslfilepath = variable_get('file_public_path', conf_path() . '/files') . "/$xslfile";
  }

  if (is_file($xslfilepath)) {
    import_html_debug("Loading Transformation Stylesheet from $xslfilepath");
    $xsldoc = parse_in_xml_file($xslfilepath, FALSE);
    $xsldocs[$xslfile] = $xsldoc;
  }
  else {
    trigger_error("Unable to locate the Transformation Stylesheet '$xslfile' ", E_USER_WARNING);
    $xsldocs[$xslfile] = FALSE;
    return FALSE;
  }
  return $xsldoc;
}

/**
 * Run the url-rewrite XSL over the source document
 * TODO allow for the non-base version of Drupal links
 *
 * The relative links need to be converted into path-to- top and back down
 * again. Relative references just cannot be maintained.
 *
 * @return an XML doc again
 */
function import_html_rewrite_links($xmldoc, $rel_path, $profile) {
  // Memo this to speed up bulk imports.
  static $rewrite_xsldoc;
  static $xslfilepath;
  if (!$rewrite_xsldoc) {
    $xslfilepath = drupal_get_path('module', 'import_html') . "/rewrite_href_and_src.xsl";
    $rewrite_xsldoc = parse_in_xml_file($xslfilepath, FALSE);
  }

  import_html_debug("Rewriting links for a file called '$rel_path'. dirname($rel_path) is " . dirname($rel_path));
  import_html_debug_code("import_html profile settings used for rewriting", $profile);

  // dirname('/ok.htm') returns '\';
  // No idea why, may only happen at root level on Win
  // !! B-X

  // $rel_base is the path from the import root to the current page dir.
  // I want a trailing slash, but not a leading one for the next concatenation.
  // dirname('/a/dir/') returns '/a' - which is not what I want.
  $rel_dir = preg_match('|/$|', $rel_path) ? $rel_path : dirname($rel_path);
  $rel_base = ensure_trailing_slash($rel_dir);

  $site_root = isset($profile['site_root']) ? $profile['site_root'] : url('');
  $path_to_import_top = url( ensure_trailing_slash($profile['import_site_prefix']) );
  $site_root = $path_to_import_top;

  // If we are re-writing thing/index.htm to thing - our links will resolve differently!
  // Either too high for thing, or too low for the thing/index.htm alias.
  $href_base = url( ensure_trailing_slash($profile['import_site_prefix']) . $rel_base);

  // Create the prefix for resource sources.
  // Is url() OK for files with unclean urls? - NO. Neither is file_create_url.
  $src_root = $site_root . ensure_trailing_slash($profile['file_storage_path']);

  $src_base = ensure_trailing_slash($src_root) . (($rel_base == '/') ? '' : $rel_base);

  // Or not, if we are still linking to full URLs (demo or partial import).
  if (valid_url($rel_path, TRUE)) {
    // It's remote!
    $url_parts = parse_url($rel_path);
    $path_to_import_top = $rel_path;
    $site_root = 'http://' . $url_parts['host'] . '/';
    $src_root = $site_root;
    $src_base = $rel_path;
  }
  $src_base = str_replace('/./', '/', $src_base);
  $href_base = str_replace('/./', '/', $href_base);
  // XSL will only support one default doc at a time.
  $default_document = reset($profile['default_documents']);

  import_html_debug("
    <b>Rewrite patterns:</b>
    Path to the top of this (relative) server is $site_root .
    Path to top of the prefixed section
    ({$profile['import_site_prefix']})
    from here ($rel_path)
    to our import base
    ({$profile['import_site_prefix']})
    would be '$path_to_import_top'.
    Path to a relative <em>neighbour</em> of this page would be
    ($href_base)
    or to find the base for <em>relative</em> resource files over in
    the file storage area
    ({$profile['file_storage_path']})
    would be '$src_base' ",
    array(),
    WATCHDOG_DEBUG
  );


  $parameters = array(
    // These parameters tell the rewriter what to prepend to the links.
    // They are instructions how this page will find its missing bretheren
    // when we put it where we put it.
    // Images and Pages may end up in different places.
    'site_root' => $site_root,
    'src_root' => $src_root,
    'src_base' => $src_base,
    'href_base' => $href_base,
    'replace_suffix' => $profile['relink_files'],
    'new_suffix' => '',
    'xsl_path' => $xslfilepath,
    'default_document' => $default_document,
  );
  import_html_debug("
    XSL for URL rewrites loaded OK.
    HTML links for files that were under '$rel_base' will be made relative to '"
    . $parameters['href_base'] . "' (for pages) and '" . $parameters['src_base'] . "' (for resources) "
    ,
    array(),
    WATCHDOG_DEBUG
  );

  $rewritten = xmldoc_plus_xsldoc($xmldoc, $rewrite_xsldoc, $parameters);

  // Collapse dir-up "../" paths.
  // To tricky for XSL so regexp it. Hope it doesn't break anything.
  // Beware danger of infinite loop here. Use maxout as a sanity limiter.
  $maxout = 5;
  while (preg_match('|/([^\.][^/\s"\'>]*/\.\./)|', $rewritten, $matches) && $maxout) {
    $rewritten = preg_replace('|/[^\.][^/\s"\'>]*/\.\./|', '/', $rewritten);
    $maxout --;
  }

  import_html_debug_code("The source after URL rewriting . XHTML (string)", $rewritten);

  $xmldoc = parse_in_xml_string($rewritten, FALSE);
  if (empty($xmldoc)) {
    trigger_error("Failed to rewrite links into a valid XML file", E_USER_WARNING);
    return FALSE;
  }

  return $xmldoc;
}

/**
 * Run the strip_tables XSL over the source document.
 *
 * @return an XML doc again
 */
function import_html_strip_tables($xmldoc) {
  // Memo this to speed up bulk imports.
  static $strip_tables_xsldoc;
  if (!$strip_tables_xsldoc) {
    $xslfilepath = drupal_get_path('module', 'import_html') . "/strip_tables.xsl";
    $strip_tables_xsldoc = parse_in_xml_file($xslfilepath, FALSE);
  }

  $parameters = array();
  $rewritten = xmldoc_plus_xsldoc($xmldoc, $strip_tables_xsldoc, $parameters);

  // Normalize space to clean up the gaps.
  $rewritten = preg_replace("/\\s*\\n\\s*/", "\n", $rewritten);

  #debug_code( $rewritten , 3, "The source after stripping tables . XHTML (string)");
  $xmldoc = parse_in_xml_string($rewritten, FALSE);
  if (!$xmldoc) {
    trigger_error("Failed to strip tables and end up with a valid XML file", E_USER_WARNING);
    return FALSE;
  }

  return $xmldoc;
}

/**
 * Run the cleanup_namespaces XSL over the source document.
 *
 * @return an XML doc again
 */
function import_html_cleanup_namespaces($xmldoc) {
  import_html_debug("Cleaning up unused or deep namespaces in the XHTML");

   $xslfilepath = drupal_get_path('module', 'import_html') . "/cleanup_namespaces.xsl";
  // Memo this to speed up bulk imports.
  static $cleanup_namespaces_xsldoc;
  if (!$cleanup_namespaces_xsldoc) {
    $cleanup_namespaces_xsldoc = parse_in_xml_file($xslfilepath, FALSE);
  }

  $parameters = array('xsl_path' => $xslfilepath);
  $rewritten = xmldoc_plus_xsldoc($xmldoc, $cleanup_namespaces_xsldoc, $parameters);

  // Normalize space to clean up the gaps.
  $rewritten = preg_replace("/\\s*\\n\\s*/", "\n", $rewritten);

  $xmldoc = parse_in_xml_string($rewritten, FALSE);
  if (!$xmldoc) {
    trigger_error("Failed to cleanup namespaces and end up with a valid XML file", E_USER_WARNING);
    return FALSE;
  }

  return $xmldoc;
}


/**
 * Ensure a string is able to be used as an XML, CSS or Javascript ID. Basically
 * strip out all non-alpha-numerics.
 *
 * @see http://www.w3.org/TR/REC-xml/#NT-Name
 *
 * @see form_clean_id() - which should have done this
 */
function import_html_check_name($name) {
  return preg_replace('|[^a-zA-Z0-9_]+|', '_', $name);
}


/**
 * Avoid double-ups, if the path already exists, UPDATE the existing node.
 * Can't have two content nodes claiming the same path or it won't validate.
 * Plus, we want to retain any info that's been added via drupal. Probably.
 *
 *
 * @param $node
 *   partially  created node from import. Key lookup on $node->path_alias
 * @param $profile
 *  May  contain some rules for conflict resolution - which values to keep,
 * which to over-write.
 *
 * @return $node
 *   possibly with pre-existing values blended in. Importantly - the nid
 */
function import_html_merge_over_existing_node($node, $profile) {
  // Deal with funny characters a bit.
  // This should already be done by now.
  // $node->path_alias = urldecode($node->path_alias);

  $internal_link = drupal_get_normal_path($node->path_alias);

  // path_redirect.module also may identify the node that is being overwritten. Check its lookup table
  if (($internal_link == $node->path_alias) && module_exists('path_redirect')) {
    $path_redirect = path_redirect_load_by_source($node->path_alias);
    if ($path_redirect['redirect']) {
      $internal_link = $path_redirect['redirect'];
    }
  }

  if ($internal_link != $node->path_alias) {
    // Found an internal match, the alias is already asigned to a node.
    // Merge info to avoid losing any Drupal-only info.

    $path_parts = explode("/", $internal_link);
    $probable_nid = array_pop($path_parts);
    if (! is_numeric($probable_nid)) {
      // This may happen if the menu builder has created a placeholder alias
      // pseudo-page, or the alias conflicts with an already-created system path.
      import_html_debug("
        When looking for an alias to '%nodepath',
        Found some pre-existing (non-node) content (or callback handler) there.
        the internal link
        '%internal_link' - was expected to return a nid. Therefore, not attempting a content merge",
        array(
        '%nodepath' => $node->path_alias,
        '%internal_link' => $internal_link,
      ),
        WATCHDOG_NOTICE
      );
      return $node;
    }
    $node->nid = $probable_nid;

    import_html_debug("
        Page path alias '%nodepath' already exists,
        It's already linked to node id '%nodenid'.
        This data import will <em>replace</em> that content,
        but try to keep and merge any other values.
      ",
      array(
        '%nodepath' => $node->path_alias,
        '%nodenid' => $node->nid,
      ),
      WATCHDOG_INFO
    );

    // Load existing item, layer changes on top of it.
    $old_node = node_load($node->nid );

    // Paranoia.
    if (empty($old_node)) {
      // There was an alias found pointing at what looked like a node path
      // But that node was not found.
      // This means the old alias is redundant and probably very wrong.
      trigger_error("Something is seriously wrong, database may be out of sync. I found a reference to node '{$node->nid}' when looking up existing aliases, but that is not loaded as a valid node. We'll proceed by making a new node, cannot merge with the allegedly pre-existing one. I'm going to delete that for you.", E_USER_WARNING);
      // Delete it.
      path_set_alias($internal_link);
      return $node;
    }

    // It's tricky to handle merges for some complex modules.
    // Invoke a callback to let (for example) taxonomy figure out
    // how to blend an old node term list with a new node term list.

    // module_invoke_all('import_html_node_merge', $old_node, $node, $profile);
    // WORKAROUND for php 5.3
    foreach (module_implements('import_html_node_merge') as $module) {
      $func = $module . '_import_html_node_merge';
      $func($profile, $node, $old_node);
    }
    // Now do the rest by copying values as best we can.
    foreach ($node as $key => $value) {
      // Do a deepish merge.
      if (is_array($value)) {
        // Merge deeper sets, like taxonomy.
        if (!@is_array($old_node->$key)) {
          $old_node->$key = array();
        }
        foreach ($value as $k => $v) {
          $old_node->{$key}[$k] = $v;
        }
      }
      else {
        $old_node->$key = $value;
      }
    }
    $node = $old_node;
  }
  else {
    import_html_debug(
      "No previous node at found at the path %nodepath.",
      array(
      '%nodepath' => $node->path_alias,
    ),
      WATCHDOG_INFO
    );
  }
  return $node;
}

/**
 * Our own hook, called by import_html_merge_over_existing_node.
 *
 * -- on track to become its own plugin module, but not yet.
 */
function import_html_import_html_node_merge($profile, &$node, &$old_node) {
  if (empty($old_node)) {
    return;
  }
  $strings = array(
    '!title' => l($node->title, $node->path_alias),
    '%import_html_user' => $node->name,
  );
  if ($profile['handle_duplicates'] == IMPORT_HTML_CAREFUL) {
    import_html_debug('
      We need to check if the node we are replacing
      has been modified locally.
      Recently-modified pages will not be overwritten',
      array(), WATCHDOG_INFO
    );

    if ($node->uid == $old_node->revision_uid) {
      // same owner, alright!
      return;
    }
    // else

    $revision_user = user_load($old_node->revision_uid);
    $strings['%revision_user'] = $revision_user->name;
    import_html_debug('
      This node [!title] has recently been edited by %revision_user.
      When running as %import_html_user, we will not overwrite those changes,
      so we will not import this item.
      ',
      $strings, WATCHDOG_WARNING
    );
    $node->import_html_exclude = 'Local edits by other user detected.';

  }

}


/**
 * Scan directory for files, with a max_depth limitation.
 *
 * Utility function
 *
 * file_scan_directory() does not support max_depth.
 * I need it so my folder listings don't go insane when recursing.
 *
 * This is a version of file_scan_directory that does respect max_depth
 * when recursing.
 * It also adds a filecount value to the returned item to assist feedback.
 *
 * @see file_scan_directory.
 */
function import_html_file_scan_directory($dir, $mask, $options = array(), $depth = 0) {
  // D7 changed the func signature. Unpack the options array.
  // Also note, nomask is now a regexp string, not an array.
  // Which is good.
  // Merge in defaults.
  $options += array(
    'nomask' => '/(\.\.?|CVS)$/',
    'callback' => 0,
    'recurse' => TRUE,
    'key' => 'uri',
    'min_depth' => 0,
  );

  // If no max_depth is set, the normal recursed version is OK
  // Note, set to 0 is not the same as 'not set'
  if (empty($options['max_depth'])) {
    $files = file_scan_directory($dir, $mask, $options, $depth);
  }
  else {
    $files = array();
    // Use file_scan_directory - non-recursive
    $options['recurse'] = FALSE;
    $files = file_scan_directory($dir, $mask, $options);

    foreach ($files as $filepath => $file) {
      if (is_dir($filepath)) {
        // Now do our own recursion, but only up to max_depth.
        if ($depth < $options['max_depth']) {
          $files = array_merge(import_html_file_scan_directory($filepath, $mask, $options, $depth + 1), $files);
        }

        // This may be intensive, but will help debugging
        $count_files = file_scan_directory($filepath, $mask, $options);
        $files[$filepath]->child_count = count($count_files);
      }
    }
  }
  return $files;
}


/**
 * Convert the newline-separated file exclusions in the UI
 * into the regexp that is used by file_scan_directory.
 *
 * @param string $file_exclusions
 * @return a regular expression for file matching.
 */
function import_html_file_exclusions_to_regexp($file_exclusions) {
  $parts = explode("\n",$file_exclusions);
  // Always exclude '..'
  $parts[] = '\.\.';
  // I want to use nomask to also exclude based on regexp.
  $parts = array_map('trim', $parts);
  $escaped_patterns = str_replace('@', '\@', join('|', $parts));
  $regexp = '@(' . $escaped_patterns . ')@';
  return $regexp;
}

/**
 * Sort items in a directory so that index files are at the top, followed by
 * files, followed by folders.
 *
 * This assists recursive operations and displays.
 *
 * @param $files an array, as from file_scan_directory
 * @param $profile import_html_profile preferences
 */
function import_html_sort_directory($files, $profile) {
  $chunks = array(
    'index' => array(),
    'files' => array(),
    'folders' => array(),
  );
  foreach ($files as $filepath => $file) {
    if (in_array($file->filename, $profile['default_documents'])) {
      $chunks['index'][$filepath] = $file;
    }
    elseif (isset($file->child_count)) {
      $chunks['folders'][$filepath] = $file;
    }
    else {
      $chunks['files'][$filepath] = $file;
    }
  }
  return array_merge($chunks['index'], $chunks['files'], $chunks['folders']);
}

/**
 * Given a list of directories (relative to the working path)
 * Return a list of all files in them (relative to the working path).
 */
function import_html_scan_rel_dir($selected_dirs, $context) {
  $source_siteroot = $context['source_siteroot'];
  $files = array();
  foreach ($selected_dirs as $dir) {
    $working_path = $source_siteroot . $dir;
    $found_files = file_scan_directory($working_path, "/.*/");
    foreach ($found_files as $filepath => $file_data) {
      // strip them back to relative
      $files[] = str_replace($source_siteroot, '', $filepath);
    }
  }
  return $files;
}

/**
 * Strip "../" sort of paths out of a given path if possible.
 *
 * Turns "path/long/../target.htm"
 * into "path/target.htm"
 *
 */
function import_html_collapse_parent_directories($path) {
  while (preg_match('@/[^/]+/../@', $path)) {
    $path = preg_replace('@/[^/]+/../@', '/', $path);
  }
  return $path;
}


/**
 * Tidy URLs before saving locally - for URL imports.
 *
 * Squash/hash query strings, but don't discard them.
 * Do discard fragment ids.
 *
 * Replace spaces and non-alphanumerics with underscore.
 */
function safe_filepath_from_url($rel_path, $allow_bad_urls = FALSE) {
  if (empty($rel_path)) {
    $rel_path = "index.html";
  }
  $save_as = preg_replace("|\?|", "%3f", $rel_path);
  $save_as = preg_replace("|\&|", "%26", $save_as);
  $save_as = preg_replace("|#.*|", "", $save_as);
  if ($allow_bad_urls) {
    return $save_as;
  }
  $save_as = preg_replace("|[^A-Za-z0-9_\-~\./%]+|", "_", $save_as);
  return $save_as;
}

/**
 * http://nz2.php.net/manual/en/function.utf8-decode.php#85034 .
 */
function charset_decode_utf_8($string) {
  /* Only do the slow convert if there are 8-bit characters */
  /* avoid using 0xA0 (\240) in ereg ranges. RH73 does not like that */
  #if (! ereg("[\200-\237]", $string) and ! ereg("[\241-\377]", $string)) {
  if (! preg_match("@[\200-\237]@", $string) and ! preg_match("@[\241-\377]@", $string)) {
    return $string;
  }

  // Decode three byte unicode characters.
  $string = preg_replace(
    "/([\340-\357])([\200-\277])([\200-\277])/e",
    "'&#'.((ord('\\1')-224)*4096 + (ord('\\2')-128)*64 + (ord('\\3')-128)).';'",
    $string
  );

  // Decode two byte unicode characters.
  $string = preg_replace(
    "/([\300-\337])([\200-\277])/e",
    "'&#'.((ord('\\1')-192)*64+(ord('\\2')-128)).';'",
    $string
  );

  return $string;
}


/**
 * Dummy error handler.
 *
 * Used to shush DOM errors when we know the doc is probably invalid.
 */
function stfu($err, $str) {
}


/**
 * Resolve a URL relative to a base path. This happens to work with POSIX
 * filenames as well. This is based on RFC 2396 section 5.2.
 *
 * http://nz2.php.net/manual/en/function.parse-url.php#76682
 *
 * @param $base
 * @param $url
 */
function resolve_url($base, $url) {
  if (!strlen($base)) {
    return $url;
  }
  // Step 2
  if (!strlen($url)) {
    return $base;
  }
  // Step 3
  if (preg_match('!^[a-z]+:!i', $url)) {
    return $url;
  }
  $base = parse_url($base);
  if ($url{0} == "#") {
    // Step 2 (fragment)
    $base['fragment'] = substr($url, 1);
    return glue_url($base);
  }
  unset($base['fragment']);
  unset($base['query']);
  if (substr($url, 0, 2) == "//") {
    // Step 4
    return unparse_url(array(
      'scheme' => $base['scheme'],
      'path' => $url,
    ));
  }
  elseif ($url{0} == "/") {
    // Step 5
    $base['path'] = $url;
  }
  else {
    // Step 6
    $path = explode('/', $base['path']);
    $url_path = explode('/', $url);
    // Step 6a: drop file from base
    array_pop($path);
    // Step 6b, 6c, 6e: append url while removing "." and ".." from
    // the directory portion
    $end = array_pop($url_path);
    foreach ($url_path as $segment) {
      if ($segment == '.') {
        // skip
      }
      elseif ($segment == '..' && $path && $path[sizeof($path) -1] != '..') {
        array_pop($path);
      }
      else {
        $path[] = $segment;
      }
    }
    // Step 6d, 6f: remove "." and ".." from file portion
    if ($end == '.') {
      $path[] = '';
    }
    elseif ($end == '..' && $path && $path[sizeof($path) -1] != '..') {
      $path[sizeof($path) -1] = '';
    }
    else {
      $path[] = $end;
    }
    // Step 6h
    $base['path'] = join('/', $path);
  }
  // Step 7
  return glue_url($base);
}

/**
 * Constructs and URL string from an array.
 *
 * Inverse of parse_url().
 *
 * @param array $parsed Keyed array of url_parts as produced by parse_url()
 */
function glue_url($parsed) {
  if (!is_array($parsed)) {
    return FALSE;
  }

  $uri = isset($parsed['scheme']) ? $parsed['scheme'] . ':' . ((strtolower($parsed['scheme']) == 'mailto') ? '' : '//') : '';
  $uri .= isset($parsed['user']) ? $parsed['user'] . (isset($parsed['pass']) ? ':' . $parsed['pass'] : '') . '@' : '';
  $uri .= isset($parsed['host']) ? $parsed['host'] : '';
  $uri .= isset($parsed['port']) ? ':' . $parsed['port'] : '';

  if (isset($parsed['path'])) {
    $uri .= (substr($parsed['path'], 0, 1) == '/') ?
        $parsed['path'] : ((!empty($uri) ? '/' : '' ) . $parsed['path']);
  }

  $uri .= isset($parsed['query']) ? '?' . $parsed['query'] : '';
  $uri .= isset($parsed['fragment']) ? '#' . $parsed['fragment'] : '';

  return $uri;
}
