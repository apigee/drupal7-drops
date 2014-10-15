<?php

/**
 * @file
 * Plugin definition for the Drupal Private Key key provider.
 */

$plugin = array(
  'title' => t('Drupal encrypt_drupal_variable_key variable'),
  'description' => t('Use a variable called encrypt_drupal_variable_key, preferably set in your settings.php $conf array.'),
  'key callback' => 'encrypt_get_encrypt_drupal_variable_key',
  'settings form' => NULL,
  'dependency callback' => '_encrypt_drupal_variable_is_present',
  'static key' => TRUE,
);

/**
 * Callback function to return a variable.
 */
function encrypt_get_encrypt_drupal_variable_key() {
  $key = variable_get('encrypt_drupal_variable_key', NULL);
  if (empty($key)) {
    watchdog('encrypt', 'You need to set the encrypt_drupal_variable_key variable, preferably in $conf in your settings.php.', array(), WATCHDOG_EMERGENCY);
    drupal_set_message("Encryption settings are insufficient. See your site log for more information.", 'error');
  }
  return $key;
}

/**
 * Callback function to validate the variable is present.
 */
function _encrypt_drupal_variable_is_present() {
  $errors = array();

  $key = variable_get('encrypt_drupal_variable_key', NULL);
  if (empty($key)) {
    $errors[] = t('The encrypt_drupal_variable_key is currently null. You should set it, preferably in $conf in your settings.php.');
  }

  return $errors;
}
