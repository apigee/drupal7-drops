<?php
/**
 * @file
 * Class to read/write encrypted bearer-tokens to the private filesystem.
 */
namespace Drupal\devconnect;

use Apigee\Util\CredentialStorageInterface;

class EncryptedCredentialStorage implements CredentialStorageInterface {

  /**
   * {@inheritdoc}
   */
  public function write($identifier, $credential_data) {
    $dir = self::getTokenCacheDir();
    $data = Crypto::encrypt($credential_data);
    file_put_contents("$dir/$identifier", $data);
  }

  /**
   * {@inheritdoc}
   */
  public function clear() {
    $cache_dir = self::getTokenCacheDir();
    if ($dh = opendir($cache_dir)) {
      while (($file = readdir($dh)) !== false) {
        if (is_file($file) && substr($file, 0, 1) != '.') {
          @unlink($file);
        }
      }
      closedir($dh);
    }
  }

    /**
     * {@inheritdoc}
     */
  public function read($identifier) {
    $dir = self::getTokenCacheDir();
    if (!file_exists("$dir/$identifier")) {
      return false;
    }
    return Crypto::decrypt(file_get_contents("$dir/$identifier"));
  }

  /**
   * Returns the private dir where access tokens are cached.
   *
   * @return string
   */
  private static function getTokenCacheDir() {
    $dir = variable_get('apigee_credential_dir', 'sites/default/files/private') . '/tokens';
    if (!file_exists($dir)) {
      mkdir($dir, 0777, TRUE);
    }
    return $dir;
  }
}