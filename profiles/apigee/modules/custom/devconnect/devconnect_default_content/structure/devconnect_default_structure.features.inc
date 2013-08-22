<?php
/**
 * @file
 * devconnect_default_structure.features.inc
 */

/**
 * Implements hook_views_api().
 *
 * @return array
 */
function devconnect_default_structure_views_api() {
  return array("version" => "3.0");
}

/**
 * Implements hook_ctools_plugin_api().
 *
 * @return array
 */
function devconnect_default_structure_ctools_plugin_api() {
  list($module, $api) = func_get_args();
  if ($module == "context" && $api == "context") {
    return array("version" => "3");
  }
}