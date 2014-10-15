<?php

/**
 * Implements hook_drush_cache_clear().
 *
 * @param array $types
 */
function devconnect_developer_apps_drush_cache_clear(&$types) {
  $types += array(
    'developer-apps' => 'drush_cache_clear_developer_apps',
    'api-products' => 'drush_cache_clear_api_products'
  );
}

/**
 * Cache-clear callback for Developer App cache
 */
function drush_cache_clear_developer_apps() {
  cache_clear_all(NULL, 'developer_app');
  drupal_load('module', 'devconnect_developer_apps');
  devconnect_developer_apps_sync_apps('default', TRUE, FALSE);
}

/**
 * Cache-clear callback for API Products cache
 */
function drush_cache_clear_api_products() {
  cache_clear_all(NULL, 'api_product');
  drupal_load('module', 'devconnect_developer_apps');
  devconnect_developer_apps_sync_products('default', TRUE, FALSE);
}