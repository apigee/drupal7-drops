<?php

/**
 * @file
 * Utility class for cryptological functionality.
 *
 * All methods are declared static.
 *
 * @author djohnson
 */
namespace Drupal\devconnect;

/**
 * Utility class for cryptological functionality.
 *
 * All methods are declared static.
 *
 * @author djohnson
 */
class Crypto {

  private static $cryptoKey;

  private static $encryptedStrings;

  const MAX_KEY_LENGTH = 32;

  /**
   * Sets the encryption key.
   *
   * Set the key before you call encrypt(). You must call decrypt() with the
   * same value as used for encrypt(). If the key length exceeds the maximum
   * (defined in the class constant MAX_KEY_LENGTH) it will be truncated.
   *
   * @static
   * @param string $key
   */
  public static function setKey($key) {
    if (strlen($key) > self::MAX_KEY_LENGTH) {
      $key = substr($key, 0, self::MAX_KEY_LENGTH);
    }
    self::$cryptoKey = $key;
    self::$encryptedStrings = array();
  }

  /**
   * Encrypts a string using the specified key. Caches the result so that
   * expensive encryption does not need to happen more often than necessary.
   *
   * @static
   * @param string $string
   * @return string
   */
  public static function encrypt($string) {
    if (!isset(self::$encryptedStrings)) {
      self::$encryptedStrings = array();
    }
    if (!array_key_exists($string, self::$encryptedStrings)) {
      $iv = drupal_random_bytes(16);
      $iv_base64 = rtrim(base64_encode($iv), '='); // Guaranteed to be 22 char long
      $encrypted = openssl_encrypt($string, 'aes-256-cbc', self::$cryptoKey, 1, $iv);
      self::$encryptedStrings[$string] = '$' . $iv_base64 . base64_encode($encrypted);
    }
    return self::$encryptedStrings[$string];
  }

  /**
   * Decrypts a string which was encrypted with Crypto::encrypt().
   *
   * @static
   * @param string $scrambled
   * @return string
   *
   * @throws \Exception
   */
  public static function decrypt($scrambled) {
    $use_openssl = FALSE;
    if (substr($scrambled, 0, 1) == '$') {
      $use_openssl = TRUE;
      $scrambled = substr($scrambled, 1);
    }
    if (!$use_openssl && !extension_loaded('mcrypt')) {
      throw new \Exception('Cannot decrypt legacy data; mcrypt extension not found.');
    }

    // If $scrambled is not valid base64, abort.
    if (!preg_match('!^[A-Za-z0-9+/]+={0,2}$!', $scrambled)) {
      throw new \Exception('Encrypted string is not properly formed.');
    }
    $iv_base64 = substr($scrambled, 0, 22) . '==';
    $string_encrypted = substr($scrambled, 22);

    $iv = base64_decode($iv_base64);
    if ($iv === FALSE || strlen($iv) < 16) {
      throw new \Exception('Unable to parse encrypted string.');
    }
    $string_unmangled = @base64_decode($string_encrypted);
    if ($string_unmangled === FALSE) {
      throw new \Exception('Unable to decode encrypted string.');
    }
    if ($use_openssl) {
      $decrypted = openssl_decrypt($string_unmangled, 'aes-256-cbc', self::$cryptoKey, 1, $iv);
    }
    else {
      $decrypted = mcrypt_decrypt(MCRYPT_RIJNDAEL_128, self::$cryptoKey, $string_unmangled, MCRYPT_MODE_CBC, $iv);
      if (strpos($decrypted, "\n") === FALSE) {
        throw new \Exception('Unable to decrypt encrypted string.');
      }
      list ($length, $password) = explode("\n", $decrypted, 2);
      $decrypted = substr($password, 0, intval($length));
    }
    return $decrypted;
  }
}
