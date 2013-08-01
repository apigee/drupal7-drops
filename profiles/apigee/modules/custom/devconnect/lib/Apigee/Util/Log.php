<?php
/**
 * @file
 * Provides a common logging mechanism.
 *
 * @author djohnson
 */

namespace Apigee\Util;

class Log {

  // NOTE: these constants are the same values as Drupal's corresponding
  // WATCHDOG_* constants.
  const LOGLEVEL_DEBUG = 7;
  const LOGLEVEL_NOTICE = 5;
  const LOGLEVEL_WARNING = 4;
  const LOGLEVEL_ERROR = 3;
  const LOGLEVEL_CRITICAL = 2;

  /**
   * @static
   * @param string $source
   * @param int $level
   * @param mixed $message [... $message] ...
   */
  public static function write($source, $level = self::LOGLEVEL_NOTICE, $message) {
    $log_threshold = Cache::get('apigee_log_threshold', self::LOGLEVEL_WARNING);
    if ($level > $log_threshold) {
      return;
    }
    $args = func_get_args();
    // strip off first two arguments
    array_shift($args);
    array_shift($args);

    if (count($args) > 1 || !is_string($message)) {
      ob_start();
      var_dump($args);
      $message = ob_get_clean();
    }
    if (function_exists('watchdog')) {
      watchdog($source, $message, array(), $level);
    }
    //TODO: What do we do when running outside of Drupal?
  }

}