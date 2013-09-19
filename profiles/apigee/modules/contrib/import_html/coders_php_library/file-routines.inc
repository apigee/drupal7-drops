<?php
/**
 * @file File Routines : Common read/write library.
 * The actual reading and writing between remote URL, local URL and local filepath.
 *
 * PHP5 doesn't need these too badly, partially PHP4 compatability patches,
 * partially win/*nix fixes.
 *
 *
 * @package coders
 * @author Dan Morrison http://coders.co.nz/
 */


/**
 * Converts all forward slashes to backslashes for filepaths.
 * Note, if quoting this result in a commandline command, ensure
 * you don't have to double-backslash it.
 *
 * @see foreslash
 * @see system_path
 * @parameter string $path
 * @returns string path.
 */
function backslash($path) {
  return preg_replace('|\/+|', '\\', $path);
}

/**
 * Converts backslashes to all forward slashes for filepaths.
 * @see backslash
 * @see system_path
 * @parameter string $path
 * @returns string path.
 */
function foreslash($path) {
  return preg_replace('|\\\\+|', '/', $path);
}

/**
 * Return either a foreslash or backslash filepath, as appropriate for the system
 * @see foreslash
 * @see backslash
 * @parameter string $path
 * @returns string path.
 */
function system_path($path) {
  return (strpos($_ENV['OS'], 'Windows') === FALSE) ? foreslash($path) : backslash($path);
}

/**
 * Adds a slash to a directory path if it wasn't already set
 * Unless input was an empty string.
 *
 * @parameter string $path
 * @returns string path.
 */
function ensure_trailing_slash($path) {
  return preg_replace( '|(.)/?$|', '$1/', $path );
}

/**
 * Removes last slash from a directory path
 * @parameter string $path
 * @returns string path.
 */
function trim_trailing_slash($path) {
  return rtrim($path, '/');
}

/**
 * Collapses paths with parent/path/../../other/path.
 *
 * @parameter string $path
 * @returns string path.
 */
function collapse_parents($path) {
  $pat = '|/[^/]+/\.\./|';
  while (preg_match($pat, $path)) {
    $path = preg_replace($pat, '/', $path);
  }
  return $path;
}

/**
 * Returns true if the given path/url is on the current filesystem
 * Returns false for Urls and stuff
 * @parameter string $url
 * @returns boolean
 */
function is_local($url) {
  $url_parts = parse_url($url);
  if (! empty($url_parts['host']) ) {
    return FALSE;
  }
  else {
    return TRUE;
  }
}


if (! function_exists('mkdirs')) {
  /**
   * Recursive directory creation.
   * Very common task, so it may have been declared by other libraries.
   * @param string $path
   *
   * TODO is there a Drupal version?
   */
  function mkdirs($strpath, $mode = 0777) {
    //creates directory tree recursively
    return is_dir($strpath) || ( mkdirs(dirname($strpath), $mode) and mkdir($strpath, $mode) and @chmod($strpath, 0777) );
  }
}
