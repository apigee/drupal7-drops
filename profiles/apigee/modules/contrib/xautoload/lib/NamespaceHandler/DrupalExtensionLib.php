<?php


class xautoload_NamespaceHandler_DrupalExtensionLib implements xautoload_NamespaceHandler_Interface {

  protected $extensions = array();
  protected $system;

  function __construct($system) {
    $this->system = $system;
  }

  /**
   * Find the file for a class that in PSR-0 or PEAR would be in
   * $psr_0_root . '/' . $path_prefix_symbolic . $path_suffix
   */
  function findFile($api, $path_prefix_symbolic, $path_suffix) {
    $pos = 0;
    while (TRUE) {
      $pos = strpos($path_suffix, DIRECTORY_SEPARATOR);
      if (FALSE === $pos) {
        return FALSE;
      }
      $char = @$path_suffix{$pos + 1};
      if (isset($char) && "$char" === strtoupper($char)) {
        // We found a '_' followed by an uppercase character.
        break;
      }
      // We hit a normal '_' within an extension name.
      $path_suffix[$pos] = '_';
    }
    if (FALSE !== $pos) {
      $extension = substr($path_suffix, 0, $pos);
      $this->_initExtension($extension, $path_prefix_symbolic);
      if (!empty($this->extensions[$extension])) {
        $path = $this->extensions[$extension] . substr($path_suffix, $pos + 1);
        if ($api->suggestFile($path)) {
          return TRUE;
        }
      }
    }
  }

  protected function _initExtension($extension, $path_prefix_symbolic) {
    if (!isset($this->extensions[$extension])) {
      if ($this->system->extensionExists($extension)) {
        $extension_dir = $this->system->getExtensionPath($extension);
        $this->extensions[$extension] = $this->_extensionClassesDir($extension, $extension_dir, $path_prefix_symbolic);
      }
      else {
        $this->extensions[$extension] = FALSE;
      }
    }
  }

  protected function _extensionClassesDir($name, $extension_dir, $path_prefix_symbolic) {
    return $extension_dir . DIRECTORY_SEPARATOR . 'lib' . DIRECTORY_SEPARATOR;
  }
}
