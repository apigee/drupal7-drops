<?php

/**
 * @file
 * Monetization cache code.
 */

// @TODO Find a better way to register the autoloader
require_once drupal_get_path('profile', 'apigee') . '/libraries/mgmt-api-php-sdk/vendor/autoload.php';

class MintCacheManager extends Apigee\Util\CacheManager {

  public function __construct() {
  }

  public function setup($config) {
  }

  public function getConfig() {
    return array();
  }

  /**
   * Cache a value given $data and identify it by $cid
   *
   * @param string $cid
   * @param mixed $data
   */
  public function set($cid, $data) {
    cache_set($cid, $data, 'cache_mint', variable_get('devconnect_monetization_cache_expires', CACHE_TEMPORARY));
  }

  /**
   * Attempt to get a value from cache given the id specified by $cid
   * if no value is found in cache, then value specified by $data is
   * returned. if no $data is specified it will return NULL
   *
   * @param string $cid
   * @param mixed $data
   */
  public function get($cid, $data = NULL) {
    $cache = cache_get($cid, 'cache_mint');
    if ($cache !== FALSE) {
      $data = $cache->data;
    }
    return $data;
  }

  /**
   * Expires data from the cache.
   *
   *  @param string $cid
   *    If set, the cache ID or an array of cache IDs. Otherwise,
   *    all cache entries that can expire are deleted. The $wildcard argument will be ignored if set to NULL.
   *  @param bool $wildcard
   *    If TRUE, the $cid argument must contain a string value and cache IDs
   *    starting with $cid are deleted in addition to the exact cache ID specified by $cid. If $wildcard is TRUE and $cid is '*', the entire cache is emptied.(non-PHPdoc)
   */
  public function clear($cid = NULL, $wildcard = FALSE) {
    cache_clear_all($cid, 'cache_mint', $wildcard);
  }
}
