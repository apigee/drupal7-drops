<?php
/**
 * @file
 * devconnect_default_structure.context.inc
 */

/**
 * Implements hook_context_default_contexts().
 */
function devconnect_context_context_default_contexts() {
  $export = array();
  $files = file_scan_directory(drupal_get_path('module', 'devconnect_context') . "/context", '!\.php$!');
  foreach ($files as $info) {
    require_once($info->uri);
    $export[$context->name] = $context;
  }
  return $export;
}
