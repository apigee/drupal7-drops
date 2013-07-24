<?php


/**
 * Helper class for the class finder.
 */
class xautoload_ClassFinder_Helper_RecursiveMapEvaluator {

  protected $nsPaths = array();
  protected $nsHandlers = array();

  /**
   * If a class file would be in
   *   $psr0_root . '/' . $first_part . $second_part
   * then instead, we look in
   *   $root_path . '/' . $first_part . $second_part
   */
  function registerRootPath($first_part, $root_path) {
    $deep_path = $root_path . DIRECTORY_SEPARATOR . $first_part;
    $this->registerDeepPath($first_part, $deep_path);
  }

  /**
   * If a class file would be in
   *   $psr0_root . '/' . $first_part . $second_part
   * then instead, we look in
   *   $deep_path . $second_part
   *
   * @param string $first_part
   *   The would-be namespace path relative to PSR-0 root.
   *   That is, the namespace with '\\' replaced by DIRECTORY_SEPARATOR.
   * @param string $path
   *   The filesystem location of the (PSR-0) subfolder for the given namespace.
   * @param boolean $lazy_check
   *   If TRUE, then it is yet unknown whether the directory exists. If during
   *   the process we find that it does not exist, we unregister it.
   */
  function registerDeepPath($first_part, $deep_path, $lazy_check = TRUE) {
    $this->nsPaths[$first_part][$deep_path] = $lazy_check;
  }

  function registerNamespaceHandler($first_part, $handler) {
    $this->nsHandlers[$first_part][] = $handler;
  }

  /**
   * Find the file for a class that in PSR-0 or PEAR would be in
   * $psr_0_root . '/' . $first_part . $second_part
   *
   * @param string $first_part
   *   First part of the canonical path, with trailing DIRECTORY_SEPARATOR.
   * @param string $second_part
   *   Second part of the canonical path, ending with '.php'.
   */
  function findFile_nested($api, $first_part, $second_part) {

    // Check any paths registered for this namespace.
    if (isset($this->nsPaths[$first_part])) {
      $lazy_remove = FALSE;
      foreach ($this->nsPaths[$first_part] as $dir => $lazy_check) {
        $file = $dir . $second_part;
        if ($api->suggestFile($file)) {
          return TRUE;
        }
        if ($lazy_check && !$api->is_dir($dir)) {
          // This is the best place to lazy-check whether a directory exists.
          unset($this->nsPaths[$first_part][$dir]);
          $lazy_remove = TRUE;
        }
      }
      if ($lazy_remove && empty($this->nsPaths[$first_part])) {
        unset($this->nsPaths[$first_part]);
      }
    }

    // Check any handlers registered for this namespace.
    if (isset($this->nsHandlers[$first_part])) {
      foreach ($this->nsHandlers[$first_part] as $handler) {
        if ($handler->findFile($api, $first_part, $second_part)) {
          return TRUE;
        }
      }
    }
  }
}
