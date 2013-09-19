<?php
/**
 * @file Support for features.
 * Import and export the presets.
 */

/**
 * Implements hook_features_export_options().
 *
 * Provide a selector to define which of the configurations to use.
 */
function import_html_profile_features_export_options() {
  $options = array();
  foreach (import_html_profile_load_all() as $profile) {
    $options[$profile['profile_id']] = $profile['profile_id'];
  }
  return $options;
}


/**
 * Implements hook_features_export().
 *
 * Sets info-file options for exporting an import_html profile settings array.
 *
 * @param array $data
 * An array of machine names for the component in question
 * to be exported.
 * @param array &$export
 * By reference. An array of all components to be exported with a given
 * feature. Component objects that should be exported should be added to   this
 * array.
 */
function import_html_profile_features_export($data, &$export) {
  $profiles = import_html_profile_load_all();
  $export['dependencies']['import_html'] = 'import_html';

  // Dump all profiles if not defined
  if (empty($data)) {
    $data = array_keys($profiles);
  }
  foreach ($data as $component) {
    $export['features']['import_html_profile'][$component] = $component;
  }

  return array();
}


/**
 * Implements hook_features_export_render().
 *
 * Generates the PHP dump representing the settings
 *
 * @param array $data
 * An array of machine names for the component in question
 * to be exported.
 */
function import_html_profile_features_export_render($module, $data, $export = NULL) {
  $code = array();
  $code[] = '  $import_html_profiles = array();';
  $code[] = '';

  $profiles = import_html_profile_load_all();
  // Dump all profiles if not defined
  if (empty($data)) {
    $data = array_keys($profiles);
  }

  foreach ($data as $profile_id) {
    if (! isset($profiles[$profile_id])) {
      // Probably the very first run - nothing is loaded yet, so can't compare.
      import_html_debug("Invalid profile_id %profile_id specified to %func. Aborting this import. This is probably just the very first render of a profile that hasn't been initialized yet.", array('%func' => __FUNCTION__, '%profile_id' => $profile_id), WATCHDOG_NOTICE);
      continue;
    }

    $profile = $profiles[$profile_id];
    import_html_profile_sanitize($profile, $module);

    $setting_export = features_var_export($profile, '  ');
    $code[] = "  // Exported setting (only the non-default values): {$profile_id}";
    $code[] = "  \$import_html_profiles['{$profile_id}'] = {$setting_export};";
    $code[] = "";
  }

  $code[] = '  return $import_html_profiles;';
  $code = implode("\n", $code);

  return array('import_html_profile_defaults' => $code);
}


/**
 * Implements hook_features_revert().
 */
function import_html_profile_features_revert($module_name) {
  return import_html_profile_features_rebuild($module_name);
}


/**
 * Rebuild all component objects for a given feature module. That is, re-
 * import the exported feature settings as defined in
 * import_html_profile_features_export_render
 *
 *
 * @param string $module_name
 *   The name of the feature module whose components should be rebuilt.
 */
function import_html_profile_features_rebuild($module_name) {
  $component_type = 'import_html_profile';
  // This call will return the data structure serialized in the feature module
  $components = features_get_default($component_type, $module_name);
  // Merge the imported named settings into the array of settings
  watchdog('import_html', 'Initializing rebuild of feature %module_name. Importing profile settings %components', array('%module_name' => $module_name, '%components' => join(', ', array_keys($components))));
  if (!empty($components)) {
    foreach ($components as $component_id => $component) {
      // It's common a feature would want to deploy the files along with itself
      // Swap the !module path for the token, if it's used in a path.
      $component['translation_template'] = strtr($component['translation_template'], array('!module' => drupal_get_path('module', $module_name)));
      import_html_profile_save($component);
      drupal_set_message(t("Updated %module_name component %component_id as <a href='!link'>an import_html setting</a>.", array('%module_name' => $module_name, '%component_id' => $component_id, '!link' => url(IMPORT_HTML_ADMIN_PATH . '/profiles/' . $component_id))), 'info');
    }
    // The profiles are shown as menu local items, so bump the menu cache.
    menu_cache_clear('management');
  }
}


///////////////////////////////////////////////////////////////////////////////

/**
 * Import content from defined locations
 */

/**
 * Implements hook_features_export_options().
 *
 * List all config settings currently available for export. This adds each of
 * the configurations to the features UI where they can be chosen for bundling.
 *
 * @return array A keyed array of items, suitable for use with a FormAPI select
 * or checkboxes element.
 *
 **/
function import_html_content_features_export_options() {
  // We cannot tell whether the imported content has been changed (that's OK)
  // but we make a note of the import history, and return a list indicating
  // what we think we've done. This then gets checked to see if the imported
  // paths still exist.
  // If so, we guess we are up to date.
  $already_imported = variable_get('import_html_content_imported', array());
  $options = array();
  foreach ($already_imported as $component_id => $service) {
    $options[$component_id] = is_array($service) ? $service['name'] : $component_id;
  }
  return $options;
}


/**
 * Implements hook_features_export().
 */
function import_html_content_features_export($data, &$export) {
  $pipe = array();
  $export['dependencies']['import_html'] = 'import_html';

  // Original variable-based version
  $content_imported = variable_get('import_html_content_imported', array());
  foreach ($data as $component_id) {
    if ($setting = $content_imported[$component_id]) {
      $export['features']['import_html_content'][$component_id] = $component_id;
    }
  }

  return $pipe;
}


/**
 * Return the PHP code that represents a dump of the component
 * and its settings. The components to export are listed in $data.
 *
 * Implements {COMPONENT}_features_export_render().
 *
 * @param string $module_name
 *   The name of the feature module to be exported.
 * @param array $data
 *   An array of machine name identifiers for the objects to be rendered.
 * @param array $export
 *   The full export array of the current feature being exported. This is only
 *   passed when hook_features_export_render() is invoked for an actual feature
 *   update or recreate, not during state checks or other operations.
 * @return array
 *   An associative array of rendered PHP code where the key is the name of the
 *   hook that should wrap the PHP code. The hook should not include the name
 *   of the module, e.g. the key for `hook_example` should simply be `example`.
 *
 */
function import_html_content_features_export_render($module_name, $data, $export = NULL) {
  watchdog('import_html', 'Rendering features-based components. Features module:%module_name', array('%module_name' => $module_name), WATCHDOG_INFO );
  $code = array();
  $code[] = '  $settings = array();';
  $content_imported = variable_get('import_html_content_imported', array());
  $translatables = array();
  foreach ($data as $component_id) {
    if (!empty($content_imported[$component_id])) {
      $item = $content_imported[$component_id];
      import_html_content_sanitize($item, $module_name);

      // Now, we could just reflect the previous imported list back,
      // and that would mean that no changes are ever detected.
      // BUT, to ensure things really are in sync, we will check that the list
      // of files we say we imported really are still here.
      // Deleting an imported node will now prompt us to remake it, but
      // modified nodes will remain modified.
      foreach ($item['paths'] as $i => $path) {
        // Check that content still exists on this path
        // Need to use the url calculation in case the imported path had a prefix.
        $expected_path = _import_html_calc_path($path, $item);

        // TODO also allow files that have loaded to either be checked or get flagged for replacement
        $normal_path = drupal_get_normal_path($expected_path);
        if ($normal_path == $expected_path) {
          // This returns the same path if it failed.
          // Unsetting this is an indicator to the Features override-checker
          // that this node has gone AWOL
          unset($item['paths'][$i]);
          drupal_set_message(t("Failed to find expected imported path %path when checking the status of the import_html_content feature. (Originally provided by features from %module) It may have been deleted.", array('%path' => $expected_path, '%module' => $module_name)), 'warning');
        }
      }
      // The returned/modified settings array will contain all paths that are still here
      // - an exact match -
      // and NOT return paths that are missing.
      // A diff on that will show what's changed.
      $code[] = "  // Exported {$component_id}";
      $export = features_var_export($item, '  ');
      $code[] = "  \$settings['{$component_id}'] = {$export};";
    }
    else {
      // The thing we were expecting to see was not here.
      watchdog('import_html', 'The component %component_id was not present in the import_html_content_imported table. This means it\'s not imported locally yet.', array('%component_id' => $component_id) );
    }
  }
  $code[] = '  return $settings;';
  $code = implode("\n", $code);
  return array('import_html_content_defaults' => $code);
}


/**
 * Implements hook_features_enable().
 *
 * // Or does it? is this a ghost hook?
 */
function import_html_content_features_enable($module_name) {
  watchdog('import_html', '%nunction called', array('%function' => __FUNCTION__), WATCHDOG_INFO);
  import_html_content_features_rebuild($module_name);
}


/**
 * Revert all component objects for a given feature module.
 *
 * Implements {COMPONENT}_features_revert().
 *
 * @param string $module_name
 *   The name of the feature module whose components should be reverted.
 * @return boolean
 *   TRUE or FALSE for whether the components were successfully reverted.
 */
function import_html_content_features_revert($module_name) {
  watchdog('import_html', '%nunction called', array('%function' => __FUNCTION__), WATCHDOG_INFO);
  // If told to revert, deliberately forget if anything else was imported.
  // A re-import is a fine idea.
  variable_del('import_html_content_imported');
  import_html_content_features_rebuild($module_name);
}


/**
 * Rebuild all component objects for a given feature module. That is, re-
 * import the exported feature settings as defined in
 * import_html_profile_features_export_render
 *
 * Implements {COMPONENT}_features_rebuild().
 *
 * @param string $module_name
 *   The name of the feature module whose components should be rebuilt.
 */
function import_html_content_features_rebuild($module_name) {
  watchdog('import_html', '%nunction called', array('%function' => __FUNCTION__), WATCHDOG_INFO);
  $content_imported = variable_get('import_html_content_imported', array());

  $component_type = 'import_html_content';
  // This call will return the data structure serialized in the feature module
  $components = features_get_default($component_type, $module_name);

  watchdog('import_html', 'Initializing rebuild of feature %module_name. Importing content from %components', array('%module_name' => $module_name, '%components' => join(', ', array_keys($components))));

  // If we are running for the very first time, and this feature is also
  // deploying a custom profile within the same deployment,
  // * the profile may not be loaded yet *
  // I think it happens in alphabetical order.
  // To avoid this, run the profile import script first, now.
  import_html_profile_features_rebuild($module_name);

  // Merge the imported named settings into the array of settings
  if (!empty($components)) {
    variable_set('import_html_last_import_timestamp', REQUEST_TIME);
    foreach ($components as $component_id => $component) {
      // Initiate the import based on the instructions

      // It's common a feature would want to deploy the files along with itself
      // Swap the !module path for the token, if it's used in a path.
      if (isset($component['source_siteroot'])) {
        $component['source_siteroot'] = strtr($component['source_siteroot'], array('!module' => drupal_get_path('module', $module_name)));
      }
      // Deal with ../.. paths
      $component['source_siteroot'] = import_html_collapse_parent_directories($component['source_siteroot']);

      $paths = $component['paths'];

      // Ready to go. Start the import
      import_html_import_files($paths, $component);
      drupal_set_message(t("Import_HTML %module_name feature imported the %component_id (%name) content.", array('%module_name' => $module_name, '%component_id' => $component_id, '%name' => $component['name'])), 'info');
      // Keep an note of what we THINK we've imported.
      $content_imported[$component_id] = $component;
    }
  }
  variable_set('import_html_content_imported', $content_imported);
}

/**
 * Remove any localizations (instance-specific settings) before exporting.
 */
function import_html_content_sanitize(&$settings, $module_name) {
  // If the path appears to be linking to the local module folder, tokenize that
  $settings['source_siteroot'] = strtr($settings['source_siteroot'], array(drupal_get_path('module', $module_name) => '!module'));
}



// ============================================================

/**
 * BELOW is just SANDBOX code - verifying the features API in D&, and helping
 * move some sample content around.
 * This solution to exporting nodes is impractical and SHOULD NOT BE USED.
 */

/**
 * Tells the features system that we can export nodes. Any nodes.
 *
 * {COMPONENT}_features_export_options()
 */
function import_html_node_features_export_options() {
  // ANY NODE?
  $nodes = node_load_multiple(NULL, array('type' => 'page'));
  $options = array();
  foreach ($nodes as $node) {
    $options[$node->nid] = $node->title;
  }
  return $options;
}


/**
 * Sets up the features configs for an exported info file.
 *
 * Implements {COMPONENT}_features_export().
 */
function import_html_node_features_export($data, &$export) {
  $pipe = array();
  $export['dependencies']['import_html'] = 'import_html';
  // Raw node content export
  foreach ($data as $component_id) {
    if (is_numeric($component_id)) {
      $node = node_load($component_id);
    }
    $export['features']['import_html_node'][$component_id] = $component_id;
  }
  return $pipe;
}


/**
 * Return the PHP code that represents a dump of the component
 * and its settings. The components to export are listed in $data.
 *
 * Implements {COMPONENT}_features_export_render().
 */
function import_html_node_features_export_render($module, $data) {
  $code = array();
  $code[] = '  $settings = array();';
  foreach ($data as $component_id) {
    if (is_numeric($component_id)) {
      $item = node_load($component_id);
      import_html_node_sanitize($item, $module);
      $code[] = "  // Exported {$component_id}";
      $export = features_var_export($item, '  ');
      $code[] = "  \$settings['{$component_id}'] = {$export};";
    }
  }
  $code[] = '  return $settings;';
  $code = implode("\n", $code);
  return array('import_html_node_defaults' => $code);
}


/**
 * Called when reverting or installing the feature.
 *
 * Implements {COMPONENT}_features_revert().
 */
function import_html_node_features_revert($module_name) {
  import_html_node_features_rebuild($module_name);
}


/**
 * Rebuild all component objects for a given feature module. That is, re-
 * import the exported feature settings as defined in
 * import_html_profile_features_export_render
 *
 * Implements {COMPONENT}_features_rebuild().
 *
 * @param string $module_name
 *   The name of the feature module whose components should be rebuilt.
 */
function import_html_node_features_rebuild($module_name) {
  $component_type = 'import_html_node';
  // This call will return the data structure serialized in the feature module
  $components = features_get_default($component_type, $module_name);

  // Merge the imported named settings into the array of settings
  if (!empty($components)) {

    foreach ($components as $component_id => $component) {
      // Initiate the import based on the instructions
      $new_node = (object)$component;
      node_save($new_node);
    }
  }
}


/**
 * Helper function for {COMPONENT}_features_export_render()
 * Removes non-portable or unwanted data from a component before it's serialized.
 *
 *
 * @param unknown_type $node
 * @param unknown_type $module_name
 */
function import_html_node_sanitize(&$node, $module_name) {
  $unset = array(
      'vid',
      'uid',
      'log',
      'nid',
      'rdf_mapping',
  );
  foreach ($unset as $key) {
    unset($node->$key);
  }
}

