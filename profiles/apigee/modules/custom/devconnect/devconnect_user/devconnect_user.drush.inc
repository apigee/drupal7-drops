<?php
/**
 * @file
 * Contains drush functionality for devconnect_user module
 */

/**
 * Implements hook_drush_command().
 */
function devconnect_user_drush_command() {
  $items = array();

  $items['devconnect-user-sync'] = array(
    'callback' => 'devconnect_user_drush_user_sync',
    'description' => dt('Synchronizes Drupal users and Edge developers.'),
    'arguments' => array(),
    'aliases' => array('dc-sync'),
  );

  $items['devconnect-user-process-queue'] = array(
    'callback' => 'devconnect_user_process_queue',
    'description' => dt('Processes the user-save queue.'),
    'arguments' => array(),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_FULL,
  );

  return $items;
}


/**
 * Implements hook_drush_help().
 */
function devconnect_user_drush_help($section) {
  switch ($section) {
    case 'drush:devconnect-user-sync':
      return dt('This command synchronizes Drupal users and Edge developers.
        In situations in which a developer exists in Edge with no corresponding
        Drupal account, the Drupal account is created. In situations in which
        a Drupal account exists with no corresponding developer in Edge, the
        developer entry is created.');
  }
}

/**
 * Drush command callback to perform user-developer sync.
 */
function devconnect_user_drush_user_sync() {
  module_load_include('inc', 'devconnect_user', 'devconnect_user.sync');
  $items = devconnect_user_sync();
  if (count($items) == 0) {
    drush_log(dt('All users are in sync.'), 'success');
  }
  else {
    foreach ($items as $item) {
      $type = 'success';
      if (strpos($item, 'not created') !== FALSE) {
        $type = 'warning';
      }
      drush_log($item, $type);
    }
  }
}
