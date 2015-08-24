<?php
/**
 * @file
 * Cache implementation for Developer Apps.
 *
 * @deprecated
 * The functionality in this class has been stripped out. It remains here so
 * we won't whitescreen if update rules have not yet been run.
 */
namespace Drupal\devconnect_developer_apps;

class DeveloperAppCache implements \DrupalCacheInterface {

  // $cid is orgName:appId
  public function get($cid) {
    return FALSE;
  }

  public function getMultiple(&$cids) {
    return FALSE;
  }

  public function set($cid, $entity, $expire = CACHE_PERMANENT) {
    return;
  }

  public function clear($cid = NULL, $wildcard = FALSE) {
    return;
  }

  public function isEmpty() {
    return TRUE;
  }
}