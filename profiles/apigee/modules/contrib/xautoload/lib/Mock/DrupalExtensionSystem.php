<?php


class xautoload_Mock_DrupalExtensionSystem {

  protected $extensions;

  function addExtension($type, $name, $path) {
    $this->extensions[$name] = array(
      'type' => $type,
      'path' => $path,
    );
  }

  function addModule($name, $path) {
    $this->addExtension('module', $name, $path);
  }

  function addTheme($name, $path) {
    $this->addExtension('theme', $name, $path);
  }

  function extensionExists($name) {
    return isset($this->extensions[$name]);
  }

  function getExtensionPath($name) {
    $info = @$this->extensions[$name];
    if ($info) {
      return $info['path'];
    }
  }
}
