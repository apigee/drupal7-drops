<?php

/**
 * @file
 * Plugin definition for the Drupal Private Key key provider.
 */

$plugin = array(
  'title' => t('Drupal Private Key'),
  'description' => t("Use Drupal's private key from the database."),
  'key callback' => 'encrypt_get_drupal_private_key',
  'settings form' => NULL,
  'static key' => TRUE,
);

/**
 * Callback method to return Drupal's private key.
 */
function encrypt_get_drupal_private_key() {
  return drupal_get_private_key();
}
