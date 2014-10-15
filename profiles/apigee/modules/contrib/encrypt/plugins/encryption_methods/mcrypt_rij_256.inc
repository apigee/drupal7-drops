<?php

/**
 * @file
 * Plugin definition for the mcrypt extension and AES-256 encryption method.
 */

$plugin = array(
  'title' => t('Mcrypt AES 256'),
  'description' => t('This uses PHPs mcrypt extension and <a href="http://en.wikipedia.org/wiki/Advanced_Encryption_Standard">AES-256</a>.'),
  'encrypt callback' => '_encrypt_encryption_methods_mcrypt_rij_256',
  'dependency callback' => '_mcrypt_extension_is_present',
);

/**
 * Callback for Encrypt implementation: Mcrypt.
 *
 * This method uses PHP's mcrypt extension and AES-256. Base64 encoding is
 * used by default, unless disabled by setting 'base64' to FALSE in $options.
 */
function _encrypt_encryption_methods_mcrypt_rij_256($op, $text, $key, $options = array()) {
  $processed_text = '';

  // Key cannot be too long for this encryption.
  $key = drupal_substr($key, 0, 32);

  // Define iv cipher.
  $iv_size = mcrypt_get_iv_size(MCRYPT_RIJNDAEL_256, MCRYPT_MODE_ECB);
  $iv = mcrypt_create_iv($iv_size, MCRYPT_RAND);
  $disable_base64 = array_key_exists('base64', $options) && $options['base64'] == FALSE;

  // Check op.
  if ($op == 'decrypt') {
    // Check if we are disabling base64 encoding
    if (!$disable_base64) {
      $text = base64_decode($text);
    }

    // Decrypt text.
    return trim(mcrypt_decrypt(MCRYPT_RIJNDAEL_256, $key, $text, MCRYPT_MODE_ECB, $iv));
  }
  else {
    // Encrypt.
    $processed_text = mcrypt_encrypt(MCRYPT_RIJNDAEL_256, $key, $text, MCRYPT_MODE_ECB, $iv);

    // Check if we are disabling base64 encoding
    if (!$disable_base64) {
      $processed_text = base64_encode($processed_text);
    }

    return $processed_text;
  }
}

/**
 * Callback to see if the MCrypt library is present.
 */
function _mcrypt_extension_is_present() {
  $errors = array();

  if (!function_exists('mcrypt_encrypt')) {
    $errors[] = t('MCrypt library not installed.');
  }

  return $errors;
}
