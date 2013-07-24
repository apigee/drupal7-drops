<?php


/**
 * Example:
 *   If this handler is registered for the namespace "Drupal\\Module",
 *   and we look for a class "Drupal\\Module\\$module\\SomeClass"
 *   then the $path_suffix argument in findFile() would be
 *   "$module/SomeClass.php".
 *   The handler can extract the first fragment of the path suffix, and use it
 *   to determine the location.
 */
interface xautoload_NamespaceHandler_Interface {

  /**
   * Find the file for a class that in PSR-0 or PEAR would be in
   * $psr_0_root . '/' . $path_prefix_symbolic . $path_suffix
   *
   * @param xautoload_InjectedAPI_findFile $api
   *   Wraps the file_exists() call, and replaces the return value.
   * @param string $path_prefix_symbolic
   *   The key that this handler was registered with.
   *   With trailing DIRECTORY_SEPARATOR.
   * @param string $path_suffix
   *   Second part of the canonical path, ending with '.php'.
   * @param object $filesystem
   *   Allows to mock file_exists() calls for easier testing.
   *
   * @return string
   *   The file that defines the class or interface, or NULL if not found.
   */
  function findFile($api, $path_prefix_symbolic, $path_suffix);
}
