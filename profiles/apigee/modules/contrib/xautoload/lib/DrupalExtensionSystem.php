<?php


class xautoload_DrupalExtensionSystem {

  function extensionExists($name) {
    if (module_exists($name)) {
      return TRUE;
    }
    if (!function_exists('list_themes')) {
      return;
    }
    $themes = list_themes();
    if (isset($themes[$name])) {
      return TRUE;
    }
  }

  function getExtensionPath($name) {
    foreach (array('module', 'theme') as $type) {
      $candidate = drupal_get_path($type, $name);
      if (!empty($candidate)) {
        return $candidate;
      }
    }
  }
}
