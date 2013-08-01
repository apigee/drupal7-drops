<?php
/**
 * @file
 * devconnect_content_authoring.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function devconnect_content_authoring_ctools_plugin_api() {
  list($module, $api) = func_get_args();
  if ($module == "strongarm" && $api == "strongarm") {
    return array("version" => "1");
  }
}
