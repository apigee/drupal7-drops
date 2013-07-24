<?php


class xautoload_NamespaceHandler_DrupalExtensionLibPSR0 extends xautoload_NamespaceHandler_DrupalExtensionLib {

  protected function _extensionClassesDir($name, $extension_dir, $path_prefix_symbolic) {
    return $extension_dir . DIRECTORY_SEPARATOR . 'lib' . DIRECTORY_SEPARATOR .
      $path_prefix_symbolic . $name . DIRECTORY_SEPARATOR;
  }
}
