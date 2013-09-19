<?php
/**
 * @file helper to import fields provided by CCK (Content Creation Kit)
 */

/**
 * Try and import any CCK fields into the node in the format expected
 *
 * Implementation of HOOK_import_html()
 *
 * @TODO this is probably broken in the upgrade. Revisit.
 * Need to re-aquire the DOM element, nnot just the text string maybe. Maybe
 * should ignore the previous text scan import altogether.
 */
function content_import_html($profile, &$node, $datadoc = NULL) {
  if (function_exists('content_types') && ($content_def = content_types($node->type)) && ! empty($content_def['fields'])) {
    // This content type has fields we may want

    import_html_debug("Scanning import document for CCK fields to import into this $node->type", array(), WATCHDOG_INFO);
  }
  else {
    import_html_debug("Content type {$node->type} has no CCK fields to scan for", array(), WATCHDOG_INFO);
    return; // no valid CCK stuff for me to work on

  }

  // Look for ids or classes in the doc matching an expected CCK field name

  foreach ($content_def['fields'] as $field_id => $field_def) {

    // allow both fieldname_field and just fieldname.

    $field_short_id = substr($field_id, 6);
    // look for versions with both _ and - in the field names,
    // depending on how valid the original was.

    $safe_field_id = str_replace('_', '-', $field_id);
    $safe_field_short_id = str_replace('_', '-', $field_short_id);

    // We will put them all into an array like CCK expects

    if (! is_array($node->{$field_def['field_name']})) {
      $node->{$field_def['field_name']} = array();
    }
    // Note that other add-on modules may already be trying to set a value here.
    // They may have already set up an array of values.

    // A tag may have found itself set on the node object directly -
    // - a dc: meta tag that Drupal doesn't handle natively for example
    // may get picked up in the metas, and therefore the node.
    // If we have a content type with field_dc_location
    // and <meta name="DC:Location" content="Here" />
    // then we get $node->'dc_location'='Here'
    // The following code captures that
    // Loop through possible values on the node

    foreach (array($field_id, $field_short_id, $safe_field_id, $safe_field_short_id) as $alt_field_id) {
      if (! empty($node->$alt_field_id)) {
        import_html_debug("Found a value for '%alt_field_id' to save as a CCK field value", array('%alt_field_id' => $alt_field_id), WATCHDOG_INFO);
        // The found value may or may not be an array. Ensure it is, then loop over the list.

        $field_val = is_array($node->$alt_field_id) ? $node->$alt_field_id : array($node->$alt_field_id);
        foreach ($field_val as $val) {
          if (is_array($val)) {
            // This means that a cck $node->fieldname[]['value'] was probably already in place

            continue;
          }
          // Create a CCK value entry for this value
          // if the field is 'textfield' etc, it's easy

          // Other ones need work

          switch ($field_def['type']) {

            case 'filefield':
              // Currently can only import a pure filepath (or URL)


              content_import_html_attach_file($node, $profile, $field_def, $val, $description = '');

              break;

            case 'date':
              /*
               // The date widget may be expecting any sort of format for the value.
               // check what it expects, and give it that.
               $input_format = $field_def['widget']['input_format_custom'] ? $field_def['widget']['input_format_custom'] : $field_def['widget']['input_format'];
               $date_date = date($input_format, strtotime(strip_tags($val)));
               */
              $date_timestamp = strtotime(strip_tags($val));
              $date_save_as = date('Y-m-d\TH:i:sP', $date_timestamp);
              import_html_debug("Saving found date string (%val) [%date_timestamp] as %date_save_as in '%alt_field_id' .", array('%alt_field_id' => $alt_field_id, '%val' => $val, '%date_timestamp' => $date_timestamp, '%date_save_as' => $date_save_as), WATCHDOG_INFO);
              $node->{$field_def['field_name']}[] = array(
                'value' => $date_save_as,
              );

              break;

              // Many field types just set the 'value'
              // They are easy.

            case 'text':
              $node->{$field_def['field_name']}[] = array(
                'value' => $val,
                'format' => import_html_get_preferred_filter($profile),
              );
              break;

            default:
              import_html_debug("Unsupported field type :( . '%alt_field_id' is a %type CCK field and I don't know exactly how to save that yet.", array('%alt_field_id' => $alt_field_id, '%type' => $field_def['type']), WATCHDOG_WARNING);
          }

          #dpm(get_defined_vars());

          // TODO other content field types!!



        }
      }
      else {
        import_html_debug("No value for '%alt_field_id' to save as a CCK field value", array('%alt_field_id' => $alt_field_id), WATCHDOG_DEBUG);
      }
    }

    // That's imported the easy ones (meta strings named after field names),
    // Now do the proper DOM.


    // ...
    // Actually, you knwo what? don't bother.

    // The bits I expect to find were already in the node object as
    // named attributes. The code above should have done them OK.

    // ... I'll look at the DOM method again when I find extra need for it, but for now


    if ($field_def['type'] == 'text') {
      continue;
    }

    // TODO support for other CCK field type - other than text!

  }
}

/**
 * Utility function to detect if a filename has already been imported or used as
 * an attaqchment
 */
function content_load_file_by_filepath($filepath) {
  $result = db_query('SELECT * FROM {files} WHERE filepath = :filepath', array(':filepath' => $filepath));
  while ($file = db_fetch_array($result)) {
    // just find the last one. Really don't expect more than one?
    return $file;
  }
}

/**
 * Utility to take a give file and add it to the CCK filefield and drupal files table
 *
 * If filefield_paths is enabled, it moves the file from where you say it is into another destination with - FILE_EXISTS_RENAME
 * So if you re-attach the same file (normally) it doesn't make double-ups, but if you re-import with filefield_paths on, you get duplicates.
 * - best to check the your node to see if the file is already attached once?
 *
 *
 * This function is currently a collection of three different attempts at the
 * same ide. Needs rationalizing and review. dman2011-04
 */
function content_import_html_attach_file(&$node, $profile, $field_def, $val, $description = '') {
  // If the caller is lazy and just entered the field_id as the field_def, fine.

  if (is_string($field_def)) {
    $content_def = content_types($node->type);
    $field_def = $content_def['fields'][$field_def];
  }
  if (empty($field_def['field_name'])) {
    // Total bad parameter given.

    import_html_debug("Bad parameter given to " . __FUNCTION__ . " - Invalid field definition when processing %path", array('%path' => $node->path_alias), WATCHDOG_ERROR);
    return FALSE;
  }
  $alt_field_id = $field_def['field_name'];

  // Support filefield paths ? I don't think I can

  // Also need to do extra if the matched field $val was not just a pure path.

  // Often our extraction process will find an image or href tag.
  // Supporting that will be better long-term


  // Is so, maybe we can extract the src and continue with that.
  // just a regexp for now. think harder later.

  if (preg_match('|<img.*src=[\'"](.*?)[\'"].*>|', $val, $matches)) {
    $val = $matches[1];
    if (preg_match('|<img.*alt=[\'"](.*?)[\'"].*>|', $val, $matches)) {
      $description = $matches[1];
    }
  }
  if (preg_match('|<a.*href=[\'"](.*?)[\'"].*>|', $val, $matches)) {
    $val = $matches[1];
  }


  $rel_filepath = resolve_url($node->rel_path, $val);
  // The filepath handed to us here may already be absolute or good enough to find.
  // Otherwise we will check inside the current context source.

  if (file_exists($rel_filepath)) {
    $source_filepath = $rel_filepath;
  }
  else {
    // Add the source location, remove our files folder,
    // By this stage, the link has probably already been rewritten to include sites/default/files. (or whatever)

    // we need to chop that off and go back to the input source to really find it.

    $source_filepath = $profile['source_siteroot'] . str_replace($profile['file_storage_path'], '', $rel_filepath);
  }
  // First check if it's already local. Otherwise try to import it

  // Before we can save, need to know where the file will end up.

  // Use the import_html prefs (not filefield_paths prefs)

  $dest_root = ensure_trailing_slash($profile['file_storage_path']);
  $save_as = safe_filepath_from_url($rel_filepath, $profile['allow_bad_urls']);
  $dest_path = preg_replace("|/+|", "/", $dest_root . $save_as);
  //

  $strings = array(
    '%alt_field_id' => $alt_field_id,
    '%type' => $field_def['type'],
    '%val' => $val,
    '%rel_filepath' => $rel_filepath,
    '%dest_path' => $dest_path,
    '%source_filepath' => $source_filepath,
  );

  // However, our import may have already done that, imported file
  // may already exist. In THAT case, just refer to it.

  if (file_exists($dest_path)) {
    // File is in our local folder, but does a files object already exist?
    if ($file_object = content_load_file_by_filepath($dest_path)) {
      // So it's already in the system. All we need to do is attach it to this node.

      $file = (array) $file_object;
      $strings['%fid'] = $file['fid'];
      import_html_debug("Found existing file %dest_path already available. File table entry [fid:%fid] exists also. '%alt_field_id' is a %type CCK field.", $strings, WATCHDOG_INFO);
    }
    else {
      // Need to create a db entry
      // filefield really needs to start exposing some CRUD.

      $file = new stdClass();
      $file->uid = $account->uid;
      $file->filename = basename($dest_path);
      $file->filepath = $dest_path;
      $file->filemime = module_exists('mimedetect') ? mimedetect_mime($file) : file_get_mimetype($file->filename);
      $file->timestamp = REQUEST_TIME;
      // Insert new record to the database.

      drupal_write_record('files', $file);
      import_html_debug("Found existing file %dest_path already available. Making a file table reference to it in order to attach it as a filefield attachment. '%alt_field_id' is a %type CCK field.", $strings, WATCHDOG_INFO);
    }
  }
  else {
    // It wasn't local. try and Import it from the remote source, THEN import it

    if (file_exists($source_filepath)) {
      // Groovy, we CAN get it.
      //
      // Create a file entry (save includes import to new location, but the dest dir needs to be created first),

      $base_dir = dirname($dest_path);
      mkdirs($base_dir);
      file_prepare_directory($base_dir, FILE_CREATE_DIRECTORY);
      import_html_debug("Moving file from import source %val into %dest_path as a filefield attachment. '%alt_field_id' is a %type CCK field.", $strings, WATCHDOG_INFO);
      $file = field_file_save_file($source_filepath, NULL, $dest_path, NULL);
    }
    else {
      import_html_debug("Failed to import file field, file [%val] [%rel_filepath] [%source_filepath] not found :( . '%alt_field_id' is a %type CCK field.", $strings, WATCHDOG_WARNING);
    }
  }
  // Those three attempts may have produced a working file

  if ($file) {
    $file = (array) $file;
    if (!empty($description)) {
      $file['data']['description'] = $description;
    }
    // cck sometimes has this populated with an array containing NULL. stupid, means I can't tack a new one onto the end if limited to 1

    $node->{$field_def['field_name']} = array_filter((array) $node->{$field_def['field_name']});
    $node->{$field_def['field_name']}[] = $file;
    return $file;
  }

}
