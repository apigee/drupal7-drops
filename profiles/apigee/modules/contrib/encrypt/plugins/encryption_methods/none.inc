<?php

/**
 * @file
 * Plugin definition for the "none" encryption method.
 */

$plugin = array(
  'title' => t('None'),
  'description' => t('This uses no encryption.  It is not suggested to use this.'),
  'encrypt callback' => 'encrypt_encryption_methods_none',
);

/**
 * Callback for Encrypt implementation: none.
 */
function encrypt_encryption_methods_none($op, $text, $key, $options = array()) {
  return $text;
}
