<?php


/**
 * This is the only class that needs to be included explicitly using
 * module_load_include().
 *
 * The thing is called ClassFinder, but it also works for interfaces.
 */
class xautoload_ClassFinder {

  protected $modules = array();

  function __construct($modules) {
    $this->modules = $modules;
  }

  /**
   * Find the file that contains a class or interface definition.
   *
   * @param $class :string
   *   Name of a class or interface to load.
   * @return :string
   *   The file that defines the class or interface, or NULL if not found.
   */
  function findFile($api, $class) {
    if (preg_match('/^([a-z0-9_]+)_([A-Z].*)$/', $class, $m)) {
      list(,$module,$name) = $m;
      if (isset($this->modules[$module])) {
        $path = strtr($name, '_', '/');
        $path = $this->modules[$module] . $path . '.php';
        if ($api->suggestFile($path)) {
          return TRUE;
        }
      }
    }
  }
}
