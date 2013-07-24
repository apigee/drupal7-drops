<?php


/**
 * Adapted from Symfony\Component\ClassLoader\UniversalClassLoader
 *
 * We do not make this a subclass, because
 * - almost every method gets an override.
 * - attributes are declared private in Symfony, instead of protected (why?)
 * - we are now independent of Symfony, which allows to use the same code for
 *   D6, D7 and D8.
 *
 * Still, the interface is pretty much the same.
 */
class xautoload_ClassFinder_SymfonyExtended extends xautoload_ClassFinder_NamespaceOrPrefix {

  // Attributes as they are used in Symfony.
  // Just, we make them protected instead of private.
  protected $namespaces = array();
  protected $prefixes = array();
  protected $namespaceFallbacks = array();
  protected $prefixFallbacks = array();

  /**
   * Gets the configured namespaces.
   *
   * @return array
   *   A hash with namespaces as keys and directories as values
   */
  function getNamespaces() {
    return $this->namespaces;
  }

  /**
   * Gets the configured class prefixes.
   *
   * @return array
   *   A hash with class prefixes as keys and directories as values
   */
  function getPrefixes() {
    return $this->prefixes;
  }

  /**
   * Gets the directory(ies) to use as a fallback for namespaces.
   *
   * @return array
   *   An array of directories
   */
  function getNamespaceFallbacks() {
    return $this->namespaceFallbacks;
  }

  /**
   * Gets the directory(ies) to use as a fallback for class prefixes.
   *
   * @return array
   *   An array of directories
   */
  function getPrefixFallbacks() {
    return $this->prefixFallbacks;
  }

  /**
   * Registers the directory to use as a fallback for namespaces.
   *
   * @param array $dirs
   *   An array of directories
   */
  function registerNamespaceFallbacks(array $dirs) {
    $this->namespaceFallbacks = $dirs;
    foreach ($dirs as $dir) {
      $this->registerNamespaceDeepLocation('', $dir, FALSE);
    }
  }

  /**
   * Registers the directory to use as a fallback for class prefixes.
   *
   * @param array $dirs
   *   An array of directories
   */
  function registerPrefixFallbacks(array $dirs) {
    $this->prefixFallbacks = $dirs;
    foreach ($dirs as $dir) {
      $this->registerPrefixDeepLocation('', $dir, FALSE);
    }
  }

  /**
   * Registers an array of namespaces
   *
   * @param array $namespaces
   *   An array of namespaces (namespaces as keys and locations as values)
   */
  function registerNamespaces(array $namespaces) {
    foreach ($namespaces as $namespace => $locations) {
      $this->registerNamespace($namespace, $locations);
    }
  }

  /**
   * Registers a namespace.
   *
   * @param string       $namespace
   *   The namespace
   * @param array|string $paths
   *   The location(s) of the namespace
   *
   * @api
   */
  function registerNamespace($namespace, $paths) {
    $this->namespaces[$namespace] = (array) $paths;
    $subdir = str_replace('\\', DIRECTORY_SEPARATOR, $namespace);
    foreach ($paths as $path) {
      $this->registerNamespaceDeepLocation($namespace, $path . DIRECTORY_SEPARATOR . $subdir);
    }
  }

  /**
   * Registers an array of classes using the PEAR naming convention.
   *
   * @param array $classes
   *   An array of classes (prefixes as keys and locations as values)
   */
  function registerPrefixes(array $classes) {
    foreach ($classes as $prefix => $locations) {
      $this->registerPrefix($prefix, $locations);
    }
  }

  /**
   * Registers a set of classes using the PEAR naming convention.
   *
   * @param string       $prefix
   *   The classes prefix
   * @param array|string $paths
   *   The location(s) of the classes
   */
  function registerPrefix($prefix, $paths) {
    $this->prefixes[$prefix] = (array) $paths;
    $subdir = str_replace('_', DIRECTORY_SEPARATOR, $prefix);
    foreach ($locations as $path) {
      $this->registerPrefixDeepLocation($prefix, $path . DIRECTORY_SEPARATOR . $subdir);
    }
  }
}
