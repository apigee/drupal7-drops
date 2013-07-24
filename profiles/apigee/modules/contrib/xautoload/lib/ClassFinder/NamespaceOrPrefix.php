<?php


class xautoload_ClassFinder_NamespaceOrPrefix extends xautoload_ClassFinder_Prefix {

  protected $namespaceMap;

  function __construct() {
    parent::__construct();
    $this->namespaceMap = new xautoload_ClassFinder_Helper_RecursiveMapEvaluator();
  }

  function registerNamespaceRoot($namespace, $root_path, $lazy_check = TRUE) {
    $subdir = str_replace('\\', DIRECTORY_SEPARATOR, $namespace);
    $deep_path
      = !strlen($root_path)
      ? $subdir
      : !strlen($subdir)
      ? $root_path
      : ($root_path . DIRECTORY_SEPARATOR . $subdir)
    ;
    $this->registerNamespaceDeepLocation($namespace, $deep_path, $lazy_check);
  }

  function registerNamespaceDeep($namespace, $path, $lazy_check = TRUE) {
    $this->registerNamespaceDeepLocation($namespace, $path, $lazy_check);
  }

  /**
   * Register a filesystem location for a given namespace.
   */
  function registerNamespaceDeepLocation($namespace, $path, $lazy_check = TRUE) {
    $path_prefix_symbolic = str_replace('\\', DIRECTORY_SEPARATOR, $namespace . '\\');
    $this->namespaceMap->registerDeepPath($path_prefix_symbolic, $path . '/', $lazy_check);
  }

  function registerNamespaceHandler($namespace, $handler) {
    $path_prefix_symbolic =
      strlen($namespace)
      ? str_replace('\\', DIRECTORY_SEPARATOR, $namespace . '\\')
      : ''
    ;
    $this->namespaceMap->registerNamespaceHandler($path_prefix_symbolic, $handler);
  }

  /**
   * Finds the path to the file where the class is defined.
   *
   * @param string $class
   *   The name of the class
   * @return string|null
   *   The path, if found
   */
  function findFile($api, $class) {

    if ('\\' == $class[0]) {
      $class = substr($class, 1);
    }

    if (FALSE !== $pos = strrpos($class, '\\')) {

      // The class is within a namespace.
      if ($class{$pos + 1} === '_') {
        // We do not autoload classes where the class name begins with '_'.
        return;
      }

      // Loop through positions of '\\', backwards.
      $first_part = str_replace('\\', DIRECTORY_SEPARATOR, substr($class, 0, $pos + 1));
      $second_part = str_replace('_', DIRECTORY_SEPARATOR, substr($class, $pos + 1)) . '.php';
      $path = $first_part . $second_part;
      while (TRUE) {
        if ($this->namespaceMap->findFile_nested($api, $first_part, $second_part)) {
          return TRUE;
        }
        $pos = strrpos($first_part, DIRECTORY_SEPARATOR, -2);
        if (FALSE === $pos) break;
        $first_part = substr($path, 0, $pos + 1);
        $second_part = substr($path, $pos + 1);
      }

      // Check if anything is registered for the root namespace.
      if ($this->namespaceMap->findFile_nested($api, '', $path)) {
        return TRUE;
      }
    }
    else {

      // The class is not within a namespace.
      // Fall back to the prefix-based finder.
      return parent::findFile($api, $class);
    }
  }
}
