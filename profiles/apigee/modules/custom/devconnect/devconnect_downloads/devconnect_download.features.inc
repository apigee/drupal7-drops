<?php
/**
 * @file
 * devconnect_download.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function devconnect_download_ctools_plugin_api() {
  list($module, $api) = func_get_args();
  if ($module == "ds" && $api == "ds") {
    return array("version" => "1");
  }
}

/**
 * Implements hook_views_api().
 */
function devconnect_download_views_api() {
  return array("version" => "3.0");
}

/**
 * Implements hook_eck_bundle_info().
 */
function devconnect_download_eck_bundle_info() {
  $items = array(
  'devconnect_download_devconnect_download' => array(
  'machine_name' => 'devconnect_download_devconnect_download',
  'entity_type' => 'devconnect_download',
  'name' => 'devconnect_download',
  'label' => 'Devconnect Download',
),
  );
  return $items;
}

/**
 * Implements hook_eck_entity_type_info().
 */
function devconnect_download_eck_entity_type_info() {
$items = array(
       'devconnect_download' => array(
  'name' => 'devconnect_download',
  'label' => 'Devconnect Download',
  'properties' => array(
    'title' => array(
      'label' => 'Title',
      'type' => 'text',
      'behavior' => 'title',
    ),
    'uid' => array(
      'label' => 'Author',
      'type' => 'integer',
      'behavior' => 'author',
    ),
    'created' => array(
      'label' => 'Created',
      'type' => 'integer',
      'behavior' => 'created',
    ),
    'changed' => array(
      'label' => 'Changed',
      'type' => 'integer',
      'behavior' => 'changed',
    ),
  ),
),
  );
  return $items;
}
