<?php


class xautoload_ClassFinder_Prefix {

  protected $prefixMap;
  protected $classes = array();

  function __construct() {
    $this->prefixMap = new xautoload_ClassFinder_Helper_RecursiveMapEvaluator();
  }

  /**
   *
   */
  function registerPrefixRoot($prefix, $root_path, $lazy_check = TRUE) {
    $subdir = str_replace('_', DIRECTORY_SEPARATOR, $prefix);
    $deep_path
      = strlen($root_path)
      ? ($root_path . DIRECTORY_SEPARATOR . $subdir)
      : $subdir
    ;
    $this->registerPrefixDeepLocation($prefix, $deep_path, $lazy_check);
    // We assume that the class named $prefix is also found at this path.
    $this->registerClass($prefix, $deep_path . '.php');
  }

  function registerPrefixDeep($prefix, $deep_path, $lazy_check = TRUE) {
    $this->registerPrefixDeepLocation($prefix, $deep_path, $lazy_check);
  }

  function registerClass($class, $deep_path) {
    $this->classes[$class] = $deep_path;
  }

  /**
   * Register a filesystem location for a given class prefix.
   *
   * @param string $prefix
   *   The prefix, without trailing underscore.
   * @param string $path
   *   The filesystem location.
   * @param boolean $lazy_check
   *   If TRUE, then we are not sure if the directory at $path actually exists.
   *   If during the process we find the directory to be nonexistent, we
   *   unregister the path.
   */
  function registerPrefixDeepLocation($prefix, $path, $lazy_check = FALSE) {
    $path_prefix_symbolic =
      strlen($prefix)
      ? str_replace('_', DIRECTORY_SEPARATOR, $prefix . '_')
      : ''
    ;
    $this->prefixMap->registerDeepPath($path_prefix_symbolic, $path . '/', $lazy_check);
  }

  function registerPrefixHandler($prefix, $handler) {
    $path_prefix_symbolic =
      strlen($prefix)
      ? str_replace('_', DIRECTORY_SEPARATOR, $prefix . '_')
      : ''
    ;
    $this->prefixMap->registerNamespaceHandler($path_prefix_symbolic, $handler);
  }

  function findFile($api, $class) {

    // First check if the literal class name is registered.
    if (isset($this->classes[$class])) {
      if ($api->suggestFile($this->classes[$class])) {
        return TRUE;
      }
    }

    if ($class{0} === '_') {
      // We don't autoload classes that begin with '_'.
      return;
    }

    if (FALSE !== $pos = strrpos($class, '_')) {

      // Class does contain one or more '_' symbols.
      // Determine the canonical path.
      $path = str_replace('_', DIRECTORY_SEPARATOR, $class) . '.php';

      // Loop through all '/', backwards.
      do {
        $first_part = substr($path, 0, $pos + 1);
        $second_part = substr($path, $pos + 1);
        if ($this->prefixMap->findFile_nested($api, $first_part, $second_part)) {
          return TRUE;
        }
        $pos = strrpos($first_part, DIRECTORY_SEPARATOR, -2);
      } while (FALSE !== $pos);

      // Check if anything is registered for '' prefix.
      if ($this->prefixMap->findFile_nested($api, '', $path)) {
        return TRUE;
      }
    }
  }
}
