<?php
/**
 * @file xautoload.emulate.inc
 *
 * This file contains code to copy+paste and adapt, if you want to use
 * xautoload-style autoloading, without making xautoload a dependency.
 */


/**
 * Include basic files, and get the autoload to work.
 *
 * You want to rename this function as something like mymodule_include(),
 * and call it any time to initialize xautoloading for a specific module.
 */
function _MYMODULE_include() {
  static $_first_run = TRUE;
  if ($_first_run) {
    if (!module_exists('xautoload')) {
      spl_autoload_register('_MYMODULE_autoload');
    }
    // This could be the place for some procedural includes.
    // ..
    $_first_run = FALSE;
  }
}


/**
 * The autoload callback for this specific module, used if xautoload is not
 * present. You want to replace any "MYMODULE" with the module name.
 */
function _MYMODULE_autoload($name) {
  if (preg_match('#^MYMODULE_(.*)$#', $name, $m)) {
    $path = strtr($m[1], '_', '/');
    module_load_include('php', 'MYMODULE', "lib/$path");
  }
}
