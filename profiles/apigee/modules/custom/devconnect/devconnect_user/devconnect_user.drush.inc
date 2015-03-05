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
  devconnect_user_sync();
}
