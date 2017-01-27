<?php
/**
 * @file
 * Contains routines to clear caches.
 */

/**
 * Helper function to flush a supplied cache.
 *
 * @param string $table
 *   Name of cache table to clear.
 * @param string $results_label
 *   Human-readable name of cache for logs.
 * @param string $message_label
 *   Human-readable name of cache for UI.
 * @param array $context
 *   Current state of installer.
 */
function _apigee_install_clear_cache($table, $results_label, $message_label, &$context) {
  static $cache_tables;
  if (!isset($cache_tables)) {
    $cache_tables = module_invoke_all('flush_caches');
  }
  watchdog('apigee_install', "Flushing $results_label caches...", array(), WATCHDOG_INFO);
  $my_cache_tables = array_merge($cache_tables, array($table));
  foreach ($my_cache_tables as $my_table) {
    cache_clear_all('*', $my_table, TRUE);
  }
  $context['results'][] = $results_label;
  $context['message'] = st("$message_label caches cleared");
}

/**
 * Flushes caches for the JS/CSS cache.
 *
 * @param array $context
 *   Current state of installer.
 */
function apigee_install_clear_caches_flush(array &$context) {
  watchdog('apigee_install', 'Flushing CSS/JS', array(), WATCHDOG_INFO);
  _drupal_flush_css_js();
  $context['results'][] = 'cache_flush';
  $context['message'] = st('CSS & JS flushed');
}

/**
 * Clears the CSS Cache.
 *
 * @param array $context
 *   Current state of installer.
 */
function apigee_install_clear_caches_css(array &$context) {
  watchdog('apigee_install', 'Clearing CSS Cache', array(), WATCHDOG_INFO);
  drupal_clear_css_cache();
  $context['results'][] = 'cache_css';
  $context['message'] = st('CSS caches cleared');
}

/**
 * Clears the JS Cache.
 *
 * @param array $context
 *   Current state of installer.
 */
function apigee_install_clear_caches_js(array &$context) {
  watchdog('apigee_install', 'Clearing JS Cache', array(), WATCHDOG_INFO);
  drupal_clear_js_cache();
  $context['results'][] = 'cache_js';
  $context['message'] = st('JS caches cleared');
}

/**
 * Clears the theme cache.
 *
 * @param array $context
 *   Current state of installer.
 */
function apigee_install_clear_caches_theme(array &$context) {
  watchdog('apigee_install', 'Rebuilding themes', array(), WATCHDOG_INFO);
  system_rebuild_theme_data();
  drupal_theme_rebuild();
  $context['results'][] = 'cache_theme';
  $context['message'] = st('Theme caches cleared');
}

/**
 * Clears the entity cache.
 *
 * @param array $context
 *   Current state of installer.
 */
function apigee_install_clear_caches_entity(array &$context) {
  watchdog('apigee_install', 'Clearing Entity Cache', array(), WATCHDOG_INFO);
  entity_info_cache_clear();
  $context['results'][] = 'cache_entity';
  $context['message'] = st('Entity caches cleared');
}

/**
 * Rebuilds the node types cache.
 *
 * @param array $context
 *   Current state of installer.
 */
function apigee_install_clear_caches_nodes(array &$context) {
  watchdog('apigee_install', 'Rebuilding Node Types', array(), WATCHDOG_INFO);
  node_types_rebuild();
  $context['results'][] = 'cache_node';
  $context['message'] = st('Node caches cleared');
}

/**
 * Rebuilds the menu.
 *
 * @param array $context
 *   Current state of installer.
 */
function apigee_install_clear_caches_menu(array &$context) {
  watchdog('apigee_install', 'Rebuilding Menu', array(), WATCHDOG_INFO);
  menu_rebuild();
  $context['results'][] = 'cache_menu';
  $context['message'] = st('Menu caches cleared');
}

/**
 * Synchronizes Actions.
 *
 * @param array $context
 *   Current state of installer.
 */
function apigee_install_clear_caches_actions(array &$context) {
  watchdog('apigee_install', 'Synchronizing Actions...', array(), WATCHDOG_INFO);
  actions_synchronize();
  $context['results'][] = 'cache_action';
  $context['message'] = st('Action caches cleared');
}

/**
 * Clears core path cache.
 *
 * @param array $context
 *   Current state of installer.
 */
function apigee_install_clear_caches_core_path(array &$context) {
  _apigee_install_clear_cache('cache_path', 'cache_path', 'Path', $context);
}

/**
 * Clears cache filter.
 *
 * @param array $context
 *   Current state of installer.
 */
function apigee_install_clear_caches_core_filter(array &$context) {
  _apigee_install_clear_cache('cache_filter', 'cache_filter', 'Filter', $context);
}

/**
 * Clears bootstrap cache.
 *
 * @param array $context
 *   Current state of installer.
 */
function apigee_install_clear_caches_core_bootstrap(array &$context) {
  _apigee_install_clear_cache('cache_bootstrap', 'cache_bootstrap', 'Bootstrap', $context);
}

/**
 * Clears page cache.
 *
 * @param array $context
 *   Current state of installer.
 */
function apigee_install_clear_caches_core_page(array &$context) {
  _apigee_install_clear_cache('cache_page', 'cache_page', 'Page', $context);
}

/**
 * Clears core cache.
 *
 * @param array $context
 *   Current state of installer.
 */
function apigee_install_clear_caches_core(array &$context) {
  _apigee_install_clear_cache('cache', 'cache_core', 'Core', $context);
}

/**
 * Updates the bootstrap status.
 *
 * @param array $context
 *   Current state of installer.
 */
function apigee_install_bootstrap_status(array &$context) {
  watchdog('apigee_install', 'Updating bootstrap status...', array(), WATCHDOG_INFO);
  _system_update_bootstrap_status();
  drupal_get_messages();
  $context['results'][] = 'bootstrap_status';
  $context['message'] = st('Bootstrap status reset.');
}
