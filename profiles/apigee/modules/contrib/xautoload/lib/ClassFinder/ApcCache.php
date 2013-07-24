<?php


class xautoload_ClassFinder_ApcCache {

  protected $prefix;
  protected $finder;

  /**
   * @param object $finder
   *   Another ClassFinder to delegate to, if the class is not in the cache.
   * @param string $prefix
   *   A prefix for the storage key in APC.
   */
  function __construct($finder, $prefix) {
    if (!extension_loaded('apc') || !function_exists('apc_store')) {
      throw new Exception('Unable to use xautoload_ClassFinder_ApcCache, as APC is not enabled.');
    }
    $this->finder = $finder;
    $this->prefix = $prefix;
  }

  /**
   * @param string $class
   *   The class that wants to be autoloaded.
   */
  function findFile($api, $class) {

    $key = $this->prefix . ':' . $class;

    $file = apc_fetch($key);
    if (!empty($file)) {
      if ($api->suggestFile($file)) {
        return TRUE;
      }
      else {
        // Value in cache no longer valid.
        apc_delete($key);
      }
    }

    // Class was not found in cache, or cache is no longer valid,
    // so we ask the finder.
    if ($this->finder->findFile($api, $class)) {
      apc_store($key, $api->getFile());
      return TRUE;
    }
  }
}
