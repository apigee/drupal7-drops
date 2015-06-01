<?php
/**
 * @file
 * Adds SmartDocs to the list of drush-clearable caches.
 */

/**
 * Implements hook_drush_cache_clear().
 *
 * @param array $types
 */
function smartdocs_drush_cache_clear(&$types) {
  $types['smartdocs'] = 'drush_cache_clear_smartdocs';
}

/**
 * Cache-clear callback for Developer App cache
 */
function drush_cache_clear_smartdocs() {
  drupal_load('module', 'smartdocs');
  smartdocs_rebuild_cache_table(FALSE);
}
