<?php


class xautoload_NamespaceHandler_DrupalExtensionPSR0 extends xautoload_NamespaceHandler_DrupalExtensionLib {

  protected function _extensionClassesDir($name, $extension_dir, $path_prefix_symbolic) {
    return $extension_dir . DIRECTORY_SEPARATOR .
      $path_prefix_symbolic . $name . DIRECTORY_SEPARATOR;
  }
}
