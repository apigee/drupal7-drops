<?php


class xautoload_NamespaceHandler_Simpletest implements xautoload_NamespaceHandler_Interface {

  protected $extensionsRaw;
  protected $extensions = array();

  /**
   * Expect a class Drupal\(module)\Tests\SomeTest
   * to be in (module dir)/lib/Drupal/(module)/Tests/SomeTest.php,
   * but consider the PHP include_path setting.
   *
   * @param object $api
   *   The InjectedAPI object.
   * @param string $path_prefix_symbolic
   *   First part of the path, typically "Drupal/".
   * @param string $path_suffix
   *   Second part of the path, e.g. "$module/Tests/SomeTest.php".
   */
  function findFile($api, $prefix, $suffix) {
    $pos = strpos($suffix, '/');
    if (
      $pos !== FALSE &&
      substr($suffix, $pos, 7) === '/Tests/'
    ) {
      $extension = substr($suffix, 0, $pos);
      $extension_dir = $this->_getExtensionPath($extension);
      if (!empty($extension_dir)) {
        $path = $extension_dir . '/lib/Drupal/' . $suffix;
        if ($api->suggestFile($path)) {
          return TRUE;
        }
      }
    }
  }

  protected function _getExtensionPath($extension) {
    if (!isset($this->extensionsRaw)) {
      // The filename can be e.g.
      //   (module path)/(module name).module,  OR
      //   (theme path)/(theme name).info
      $this->extensionsRaw = db_query("SELECT name, filename FROM {system}")->fetchAllKeyed();
    }
    if (!isset($this->extensions[$extension])) {
      if (!isset($this->extensionsRaw[$extension])) {
        return;
      }
      $filepath = $this->extensionsRaw[$extension];
      $slashpos = strrpos($filepath, '/');
      if ($slashpos === FALSE) {
        return;
      }
      $path = substr($filepath, 0, $slashpos);
      $this->extensions[$extension] = $path;
    }
    return $this->extensions[$extension];
  }
}
