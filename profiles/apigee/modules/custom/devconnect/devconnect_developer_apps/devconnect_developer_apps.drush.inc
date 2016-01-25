<?php

/**
 * Implements hook_drush_cache_clear().
 */
function devconnect_developer_apps_drush_cache_clear(&$types) {
  $types += array(
    'api-products' => 'drush_cache_clear_api_products'
  );
}

/**
 * Cache-clear callback for API Products cache
 */
function drush_cache_clear_api_products() {
  cache_clear_all(NULL, 'api_product');
  drupal_load('module', 'devconnect_developer_apps');
  devconnect_developer_apps_sync_products();
}
