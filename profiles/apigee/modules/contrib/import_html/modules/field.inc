<?php
/**
 * @file helper to import fields.
 *
 * Next-generation rewrite of the previous content.inc that did this for CCK fields.
 */

/**
 * Try and import any fields into the node in the format expected
 *
 * Implementation of HOOK_import_html()
 *
 */
function field_import_html($profile, &$node, $datadoc = NULL) {
  // Use entity like syntax as much as possible, do
  // not assume 'node'.
  // TODO - get some examples of other things.

  $entity_type = 'node';

  list($id, $vid, $bundle) = entity_extract_ids($entity_type, $node);
  $strings = array(
    '%bundle' => $bundle
  );

  // RDF mapping is a good place to start when matching fields to keys.
  $rdf_mapping = array();
  if (module_exists('rdf') && $profile['rdf_mapping']) {
    $rdf_mapping = rdf_mapping_load($entity_type, $bundle);
  }

  // We have our own explicit manual mapping also.
  // Uses the same format.
  $field_mapping = array();
  if ($profile['manual_mapping']) {
    $field_mapping = $profile['manual_field_mappings'];
  }

  // Actual field instances on this type - used for auto-mapping
  $instances = field_info_instances($entity_type, $bundle);
  #$field_types = field_info_field_types();
  #$extra_fields = field_info_extra_fields($entity_type, $bundle, 'form');

  if (empty($instances)) {
    import_html_debug("Content type %bundle has no fields to scan for", $strings, WATCHDOG_INFO);
    return;
  }

  import_html_debug("Scanning import document for fields to import into this %bundle", $strings, WATCHDOG_INFO);

  // Temp place to put found data so it doesn't overwrite the data we are reading.
  $found = (object)array();

  // Look for ids or classes in the doc matching each expected field name.
  // Expectations are explicit (from the settings) or automatically generated.
  foreach ($instances as $field_id => $field_instance) {
    // $field_instance is the field instance & widget settings
    // $field is the more hardcore field definition info.
    $field = field_info_field($field_id);

    $possible_ids = array();
    // Start by getting the list of possible_ids from our manual
    // and built-in mappings.
    // Manual mappings:
    if (isset($field_mapping[$field_id]) && isset($field_mapping[$field_id]['predicates'])) {
      $possible_ids = array_merge($possible_ids, $field_mapping[$field_id]['predicates']);
    }
    // System mappings:
    if (isset($rdf_mapping[$field_id]) && isset($rdf_mapping[$field_id]['predicates'])) {
      $possible_ids = array_merge($possible_ids, $rdf_mapping[$field_id]['predicates']);
    }

    // Auto mappings: Generate the classes and ids to look for.
    $auto_possible_ids = array();
    if ($profile['auto_mapping']) {
      // Generate a list of a few alternative naming conventions for the
      // field we will look for.
      // If we are looking for 'field_my_caption',
      // then HTML classes we will accept are
      // 'field_my_caption', 'my_caption', 'field-my-caption', 'my-caption'
      $auto_possible_ids[] = $field_id;

      // Allow both field_fieldname and just fieldname.
      // A user-defined field gets 'field_' prefixed to it.
      if (substr($field_id, 0, 6) == 'field_') {
        $auto_possible_ids[] = substr($field_id, 6);
      }

      // Look for versions with both _ and - in the field names,
      // depending on how valid the original code was.
      foreach ($possible_ids as $tag => $id) {
        $auto_possible_ids[] = str_replace('_', '-', $id);
      }
    }
    $possible_ids = array_unique(array_merge($possible_ids, $auto_possible_ids));

    // When storing them on a node,
    // We will always put them all into an array like CCK expects.
    if (! isset($node->language)) {
      import_html_debug("Node should have had a language set by now. It doesn't so what's up?", array(), WATCHDOG_NOTICE);
    }
    $lang = isset($node->language) ? $node->language : language_default('language');

    // Start to scan for each of the alternative ids.
    foreach ($possible_ids as $alt_field_id) {
      if (! empty($node->import_html[$alt_field_id])) {
        $strings = array(
          '%alt_field_id' => $alt_field_id,
          '%field_id' => $field_id,
          '%type' => $field['type'],
          '%instance_type' => $field_instance['widget']['type'],
        );
        import_html_debug("Found a value for '%alt_field_id' to save as a field value in %field_id as a %type (%instance_type)", $strings, WATCHDOG_INFO);
        // The found value is expected to always be an array.

        $field_vals = $node->import_html[$alt_field_id];
        foreach ($field_vals as $val) {
          $strings['%val'] = $val;
          // Create a CCK value entry for this value
          // if the field is 'textfield' etc, it's easy
          // Other ones need work

          // Arg - it's not the widget type to flip on, it's the
          // field type - which I didn't see until I loaded $field (not $field_instance)

          switch ($field['type']) {

            case 'text':
            case 'text_long':
            case 'text_with_summary':
            //  @see text_field_info()
              import_html_debug("Saving found string (%alt_field_id) as %field_id .", $strings, WATCHDOG_INFO);
              $row = array(
                'value' => $val,
                'format' => import_html_get_preferred_filter($profile),
              );

              if ($field['type'] == 'text_with_summary') {
                #$summary = text_summary($val, $field_instance['settings']['text_processing'] ? import_html_get_preferred_filter($profile) : NULL, $field_instance['display']['teaser']['settings']['trim_length']);
                $summary = "";
                $row['summary'] = $summary;
              }

              $found->{$field_id}[$lang][] = $row;


              break;

            case 'filefield':
              // Currently can only import a pure filepath (or URL)
              field_import_html_attach_file($node, $profile, $field_instance, $val, $description = '');
              break;

            case 'taxonomy_term_reference':
              import_html_debug("Saving found term value %val (%alt_field_id) as %field_id .", $strings, WATCHDOG_INFO);
              // Expect a string here. Lookup term by name.
              // This feels like the wrong way to get the vocab machine name
              $vocab_name = $field['settings']['allowed_values'][0]['vocabulary'];
              $vocabulary = taxonomy_vocabulary_machine_name_load($vocab_name);

              $term = NULL;
              $terms = taxonomy_get_term_by_name($val, $vocab_name);
              // This returns a LIST of matches all the time. Pop the winner.
              if ($terms) {
                $term = reset($terms);
              }

              // If not found and freetagging is allowed, make the term.
              // For now, Skip the freetagging check, just make it.
              if (empty($term)) {
                $term = (object) array(
                  'name' => $val,
                  'vid' => $vocabulary->vid,
                );
                import_html_debug("Making a new term %val in the vocab.", $strings, WATCHDOG_INFO);
                taxonomy_term_save($term);
              }
              if (!empty($term)) {
                // We should have a term by now, but be paranoid anyway.
                $row = array(
                  'tid' => $term->tid,
                );
                $found->{$field_id}[$lang][] = $row;
                $strings['%term'] = $term->name;
                import_html_debug("Tagged %field_id with value %term.", $strings, WATCHDOG_DEBUG);
              }

              break;

            case 'date':
              $date_timestamp = strtotime(strip_tags($val));
              if (! $date_timestamp) {
                import_html_debug("Looks like found date string (%val) found in '%alt_field_id' was invalid. NOT saving it in  %field_id.", $strings, WATCHDOG_WARNING);
                break;
              }
              // Format this like "2012-06-03T15:32:07"
              // TODO - timezones?
              $storable_date = date(DATE_FORMAT_ISO, $date_timestamp);
              $strings['%date_timestamp'] = $date_timestamp;
              $strings['%date_save_as'] = $storable_date;
              import_html_debug("Saving found date string (%val) [%date_timestamp] as %date_save_as in '%alt_field_id' .", $strings, WATCHDOG_INFO);
              $found->{$field_id}[$lang][]['value'] = $storable_date;
              break;

            case 'datetime':
              $date_timestamp = strtotime(strip_tags($val));
              if (! $date_timestamp) {
                import_html_debug("Looks like found date string (%val) found in '%alt_field_id' was invalid. NOT saving it in  %field_id.", $strings, WATCHDOG_WARNING);
                break;
              }
              // Format this like "2012-06-03 15:32:07"
              // TODO - timezones?
              $storable_date = date(DATE_FORMAT_DATETIME, $date_timestamp);
              $strings['%date_timestamp'] = $date_timestamp;
              $strings['%date_save_as'] = $storable_date;
              import_html_debug("Saving found date string (%val) [%date_timestamp] as %date_save_as in '%alt_field_id' .", $strings, WATCHDOG_INFO);
              $found->{$field_id}[$lang][]['value'] = $storable_date;
              break;

              // TODO other content field types!!

            default:
              import_html_debug("Unsupported field type :( . '%alt_field_id' is a %type field (%instance_type) and I don't know exactly how to save that yet.", $strings, WATCHDOG_WARNING);
          } // Switch on field type.
        } // Multiple values for field.
      } // Value was found on the node.
      else {
        import_html_debug("No value for '%alt_field_id' to save as a field value", array('%alt_field_id' => $alt_field_id), WATCHDOG_DEBUG);
      }
    } // All possible field ids.
  } // All attached fields.

  // Problem:
  // This will currently keep adding copies of the same values to a multivalued
  // field on re-import.
  // TODO - filter for uniques on a hook_import_html_merge.

  foreach ($found as $field_id => $field_data) {
    $node->{$field_id} = $field_data;
  }

  return;
}


/**
 * Publish some extra settings to display on the import_html config page
 *
 * This is a version of {module}_form_{formname}_alter
 *
 * Used to get field.module to modify our import_html_profile_settings form
 * and add bits to it.
 */
function field_form_import_html_profile_settings_alter(&$form, $form_state) {
  $profile = $form['#profile'];

  $form['replication']['mapping_methods'] = array(
    '#type' => 'fieldset',
    '#title' => t('Data mapping rules'),
    '#description' => t('How found content is stored in the local content types.'),
  );

  // Toggle auto mapping option.
  $form['replication']['mapping_methods']['auto_mapping'] = array(
    '#title' => t('Automatic id mappping'),
    '#type' => 'checkbox',
    '#default_value' => $profile['auto_mapping'],
    '#description' => t('
      <em>If</em> there is a direct match between the CSS class or ID
      of the found element and the id of your storage field,
      it will be automatically mapped.
      A source element called <code>&lt;div id="extra_credits"&gt;</code>
      should map directly to your content types
      <code>field_extra_credits</code>.
      The source document may have IDs or classes named "extra_credits",
      "extra-credits", "field_extra_credits" or "field-extra-credits"
      - any of those, if found, can be used.
      This enables you to just build a content type with fields
      named after your source data and things should fall into place.
    '),
  );

  // RDF mapping option and info.
  if (module_exists('rdf')) {
    // Prepare a summary of rdf mappings.
    $rdf_mapping = rdf_mapping_load('node');
    foreach ($rdf_mapping as $field_id => $field_mapping) {
      if (isset($field_mapping['predicates'])) {
        foreach($field_mapping['predicates'] as $predicate) {
          $rows[] = array('source' => $predicate, 'field' => $field_id);
        }
      }
    }
    $form['replication']['mapping_methods']['rdf_mapping'] = array(
      '#title' => t('Built-in RDF mappping'),
      '#type' => 'checkbox',
      '#default_value' => $profile['rdf_mapping'],
      '#description' => t('
        Drupal7 supports a built-in mapping for RDFa values.
        It\'s handy to re-use that method of field mapping.
        Use the RDFx module for interesting, semantic control of what
        your data can be scraped as.
        If your data source has RDFa-annotated data fields, they can be mapped
        directly to the Drupal equivalent field.
      '),
    );
    $form['replication']['mapping_methods']['rdf_mapping_info'] = array(
      '#type' => 'fieldset',
      '#title' => t("Built in RDF data mapping"),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#description' => theme('table', array('rows' => $rows, 'header' => array('data source', 'data target'))),
    );
  }

  // Manual mapping option and settings.
  $form['replication']['mapping_methods']['manual_mapping'] = array(
    '#title' => t('Manual field mappping'),
    '#type' => 'checkbox',
    '#default_value' => $profile['manual_mapping'],
    '#description' => t('
      The table below lets you set any direct relationships between
      the scraped IDs and the local storage fields.
      Use this in association with the automatic mapping, or use it
      exclusively for full control if there may be conflicts.
    '),
  );

  // Build a subform in the shape of a table to represent the mapping rules.
  $form['replication']['mapping_methods']['manual_field_mappings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Field mapping'),
    '#description' => t("
      If you need extra control, add rules here. You will probably need
      to inspect the parsed content to find the usable ids of found content.
      Enable debug to see that, then run a test import of a single page.
      The debug log will have an entry reading:
      <em>the available data array has the following keys</em>.
      These are your available source IDs you can use here.
    "),
    '#theme' => 'import_html_form_table',
    '#tree' => TRUE,
    '#element_validate' => array('field_import_html_settings_form_validate'),
  );

  // Get a list of all fields that can be mapped to (any content type)
  $fields = field_info_fields();
  $field_options = array();
  foreach ($fields as $field_id => $field_def) {
    $used = array();
    foreach ($field_def['bundles'] as $bundle_type => $entity_list) {
      foreach ($entity_list as $entity_name) {
        $used[] = $bundle_type . ":" . $entity_name;
      }
    }
    $description = $field_def['field_name'] . " (" . join(', ', $used) . ')';
    $field_options[$field_id] = $description;
  }

  $rdf_mappings = $profile['manual_field_mappings'];
  // Internally, we will store the mappings in a way identical to rdf_mapping
  // This is more work in the UI right here,
  // but hopefully will be easier to make a transition.
  $field_mapping = field_import_html_rdf_mapping_to_keyed_array($rdf_mappings);

  $field_mapping[] = array(
    'source' => '',
    'destination' => FALSE,
  );
  foreach ($field_mapping as $i => $rule) {
    $form['replication']['mapping_methods']['manual_field_mappings'][$i] = array(
      'source' => array(
        '#title' => t('Source element identifier'),
        '#type' => 'textfield',
        '#default_value' => $rule['source'],
        '#description' => t("ID or class or meta:name of the element that is expected to be extracted."),
      ),
      'destination' => array(
        '#title' => t('Destination field or node property to map to'),
        '#type' => 'select',
        '#options' => $field_options,
        '#default_value' => $rule['destination'],
        '#description' => t("This is a list of node values that can have data inserted into them."),
      ),
    );
  }

  #dpm(get_defined_vars());
}

/**
 * Utility function for swapping between a UI table that shows mapping rules
 * and the way it's stored internally - internally it's identical to rdf_mapping.
 *
 * This function is jsut to make the UI easier to deal with.
 *
 * Used when unserializing the form settings.
 *
 * translates between
 *
 * keyed_mapping
 * array(
 *   array('source' => 'content:encoded', 'destination' => 'body')
 * );
 *
 * rdf_mapping
 * array(
 *   'body' => array('predicates' => array('content:encoded')
 * );
 *
 * @param array $rdf_mapping
 * @return array $keyed_mapping
 */
function field_import_html_rdf_mapping_to_keyed_array($rdf_mapping) {
  $keyed_mapping = array();
  foreach ($rdf_mapping as $field_id => $rdf_def) {
    if (! isset($rdf_def['predicates'])) {
      $rdf_def['predicates'] = array();
    }
    foreach ($rdf_def['predicates'] as $source) {
      $keyed_mapping[] = array(
        'source' => $source,
        'destination' => $field_id,
      );
    }
  }
  return $keyed_mapping;
}

/**
 * Inverse of field_import_html_rdf_mapping_to_keyed_array
 *
 * This function is jsut to make the UI easier to deal with.
 *
 * Used when serializing the form settings.
 *
 * @param array $keyed_mapping array style
 * @return array $rdf_mapping
 */
function field_import_html_keyed_array_to_rdf_mapping($keyed_mapping) {
  $rdf_mapping = array();
  foreach ($keyed_mapping as $rule) {
    $rdf_mapping[$rule['destination']]['predicates'][] = $rule['source'];
  }
  return $rdf_mapping;
}


/**
 * Small validate function runs when settings are being saved
 *
 * Ensure we don't save EMPTY rules. They need to be ignored later if we leave
 * them in, and they multiply on the settings form
 * (which always adds a blank for us) if we don't clean up.
 * And it's just tidier.
 */
function field_import_html_settings_form_validate($element, &$form_state, $form) {
  $values = $form_state['values']['manual_field_mappings'];
  // Throw out any submitted rules that have blanks.
  foreach ($values as $delta => $directive) {
    if (empty($directive['source'])) {
      unset($values[$delta]);
    }
  }
  // Also cook/convert to our preferred rdf mapping style before serializing
  $values = field_import_html_keyed_array_to_rdf_mapping($values);
  form_set_value($element, $values, $form_state);
}


/**
 * hook_import_html_default_settings()
 *
 * Return an array defining the keys that settings need to save.
 * If we define a profile setting of our own, it needs to be listed here to be
 * serialized.
 */
function field_import_html_default_settings() {
  // Basically, almost always we map 'content' to 'body'
  return array(
    'auto_mapping' => TRUE,
    'rdf_mapping' => TRUE,
    'manual_mapping' => TRUE,
    'manual_field_mappings' => array(
      'body' => array(
        'predicates' => array('content', 'bodycopy', 'pagecontent'),
      ),
    ),
  );
}

/////// below here needs work


/**
 * Utility function to detect if a filename has already been imported or used as
 * an attachment
 */
function field_load_file_by_filepath($filepath) {
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
function field_import_html_attach_file(&$node, $profile, $field_def, $val, $description = '') {
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
