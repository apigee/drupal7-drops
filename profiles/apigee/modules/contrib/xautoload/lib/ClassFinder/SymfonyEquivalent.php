<?php


/**
 * Equivalent to Symfony\Component\ClassLoader\UniversalClassLoader, for
 * performance and behavioral testing relative to the xautoload class finder.
 *
 * Differences to the Symfony class loader:
 * - This thing does only find classes, whereas the Symfony one can load them.
 * - The findFile() method uses an injected API instead of a return value, and
 *   instead of the explicit file_exists() calls.
 * - Some of the namespace registration methods stripped for simplicity.
 * - Getter methods all stripped for simplicity.
 * - Code style (indentation etc) "the Drupal way".
 *
 * Differences to the xautoload class finder:
 * - Can only load PSR-0 and PEAR conformal code, not the "pseudo-PSR-0" that
 *   xautoload does support.
 * - Namespace registration is different.
 */
class xautoload_ClassFinder_SymfonyEquivalent {

  private $namespaces = array();
  private $prefixes = array();
  private $namespaceFallbacks = array();
  private $prefixFallbacks = array();

  /**
   * Registers the directory to use as a fallback for namespaces.
   *
   * @param array $dirs An array of directories
   *
   * @api
   */
  public function registerNamespaceFallbacks(array $dirs) {
    $this->namespaceFallbacks = $dirs;
  }

  /**
   * Registers the directory to use as a fallback for class prefixes.
   *
   * @param array $dirs An array of directories
   *
   * @api
   */
  public function registerPrefixFallbacks(array $dirs) {
    $this->prefixFallbacks = $dirs;
  }

  /**
   * Registers a namespace.
   *
   * @param string     $namespace The namespace
   * @param array|string $paths   The location(s) of the namespace
   *
   * @api
   */
  public function registerNamespace($namespace, $paths) {
    $this->namespaces[$namespace] = (array) $paths;
  }

  /**
   * Registers a set of classes using the PEAR naming convention.
   *
   * @param string     $prefix  The classes prefix
   * @param array|string $paths   The location(s) of the classes
   *
   * @api
   */
  public function registerPrefix($prefix, $paths) {
    $this->prefixes[$prefix] = (array) $paths;
  }

  /**
   * Finds the path to the file where the class is defined.
   *
   * @param string $class The name of the class
   *
   * @return string|null The path, if found
   */
  public function findFile($class, $api) {
    if ('\\' == $class[0]) {
      $class = substr($class, 1);
    }

    if (false !== $pos = strrpos($class, '\\')) {
      // namespaced class name
      $namespace = substr($class, 0, $pos);
      foreach ($this->namespaces as $ns => $dirs) {
        if (0 !== strpos($namespace, $ns)) {
          continue;
        }

        foreach ($dirs as $dir) {
          $className = substr($class, $pos + 1);
          $file = $dir . DIRECTORY_SEPARATOR . str_replace('\\', DIRECTORY_SEPARATOR, $namespace) . DIRECTORY_SEPARATOR . str_replace('_', DIRECTORY_SEPARATOR, $className) . '.php';
          if ($api->suggestFile($file)) {
            return TRUE;
          }
        }
      }

      foreach ($this->namespaceFallbacks as $dir) {
        $file = $dir . DIRECTORY_SEPARATOR . str_replace('\\', DIRECTORY_SEPARATOR, $class) . '.php';
        if ($api->suggestFile($file)) {
          return TRUE;
        }
      }
    }
    else {
      // PEAR-like class name
      foreach ($this->prefixes as $prefix => $dirs) {
        if (0 !== strpos($class, $prefix)) {
          continue;
        }

        foreach ($dirs as $dir) {
          $file = $dir . DIRECTORY_SEPARATOR . str_replace('_', DIRECTORY_SEPARATOR, $class) . '.php';
          if ($api->suggestFile($file)) {
            return TRUE;
          }
        }
      }

      foreach ($this->prefixFallbacks as $dir) {
        $file = $dir . DIRECTORY_SEPARATOR . str_replace('_', DIRECTORY_SEPARATOR, $class) . '.php';
        if ($api->suggestFile($file)) {
          return TRUE;
        }
      }
    }
  }
}
