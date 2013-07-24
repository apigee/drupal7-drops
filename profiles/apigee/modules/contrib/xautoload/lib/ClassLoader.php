<?php


/**
 * Behaves like the Symfony ClassLoader classes.
 */
class xautoload_ClassLoader {

  protected $finder;

  /**
   * @param xautoload_ClassFinder $finder
   *   The object that does the actual class finding.
   */
  function __construct($finder) {
    $this->finder = $finder;
  }

  /**
   * Registers this instance as an autoloader.
   *
   * @param boolean $prepend
   *   If TRUE, the loader will be prepended. Otherwise, it will be appended.
   */
  function register($prepend = false) {
    // http://www.php.net/manual/de/function.spl-autoload-register.php#107362
    // "when specifying the third parameter (prepend), the function will fail badly in PHP 5.2"
    if (version_compare(PHP_VERSION, '5.3.0') >= 0) {
      spl_autoload_register(array($this, 'loadClass'), TRUE, $prepend);
    }
    elseif ($prepend) {
      $loaders = spl_autoload_functions();
      spl_autoload_register(array($this, 'loadClass'));
      foreach ($loaders as $loader) {
        spl_autoload_unregister($loader);
        spl_autoload_register($loader);
      }
    }
    else {
      spl_autoload_register(array($this, 'loadClass'));
    }
  }

  /**
   * Callback for class loading. This will include ("require") the file found.
   *
   * @param string $class
   *   The class to load.
   */
  function loadClass($class) {
    $api = new xautoload_InjectedAPI_findFile($class);
    if ($this->finder->findFile($api, $class)) {
      require $api->getFile();
    }
    elseif (preg_match('#^xautoload_#', $class)) {
      // die("Failed to load '$class'.");
    }
  }

  /**
   * For compatibility, it is possible to use the class loader as a finder.
   *
   * @param string $class
   *   The class to find.
   *
   * @return string
   *   File where the class is assumed to be.
   */
  function findFile($class) {
    $api = new xautoload_InjectedAPI_findFile($class);
    if ($this->finder->findFile($api, $class)) {
      return $api->getFile();
    }
  }
}
