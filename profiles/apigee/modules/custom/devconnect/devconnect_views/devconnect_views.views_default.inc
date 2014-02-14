<?php
/**
 * @file
 * devconnect_views.views_default.inc
 */

/**
 * Implements hook_views_default_views().
 */
function devconnect_views_views_default_views() {
  $export = array();
  $files = file_scan_directory(drupal_get_path('module', 'devconnect_views') . "/views", '!\.php$!');
  foreach ($files as $info) {
    require_once($info->uri);
    $export[$view->name] = $view;
  }
  return $export;
}
