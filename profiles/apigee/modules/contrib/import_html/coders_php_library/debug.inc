<?php
/**
 * @file
 * Small library of diagnostic feedback stuff.
 *
 * Maybe should be tracing to a file instead but...
 * Set DEBUGLEVEL > 0 to see what's going on
 * Set DEBUGSYSLOG to send messages to syslog also
 * Set DEBUGBACKTRACE for performance and callstack logging
 *
 * @package coders
 * @author Dan Morrison http://coders.co.nz/
 */


// The number set here (if not 0) will be the number of function parents displayed. Normally 0
define('DEBUGBACKTRACE', 0);

/**
 * Wrapper to watchdog
 *
 * WATCHDOG_DEBUG level alerts are discarded unless debug_level is set to that
 * level.
 * Otherwise everything (both info and notices) does get logged, it's up to the
 * review screen to filter out what we don't want to see.
 */
function import_html_debug($message, array $variables = array(), $severity = WATCHDOG_DEBUG) {
  $debuglevel = variable_get('import_html_debug_level', WATCHDOG_WARNING);
  if (empty($debuglevel) && ($severity >= WATCHDOG_NOTICE)) {
    return;
  }
  if (! is_array($variables)) {
    trigger_error("Incorrect code call to import_html_debug ($message). Second param should be an array.");
  }
  if (DEBUGBACKTRACE) {
    $message = debug_get_trace() . $message;
  }

  watchdog('Import HTML', $message, $variables, $severity);
}

/**
 * @param string $message
 * @param mixed $data
 *   Either a string, array or object. Whichever,
 *   it will be dumbed as code.
 * @param int $severity
 */
function import_html_debug_code($message, $data = array(), $severity = WATCHDOG_DEBUG) {
  $printed_value = "<textarea rows=20 style=\"width: 100%;\">\n " . print_r($data, 1) . " </textarea>";
  #$printed_value = "<div style=\"width: 100%; overflow:scroll; height:10em\"><pre>\n ". print_r($data, 1) ." </pre></div>";
  import_html_debug($message . $printed_value, $variables = array(), $severity);
}


/**
 * Returns a summary of the place in the code the debug message was called from
 */
function debug_get_trace() {
  $trace = debug_backtrace();
  $prev = array_shift($trace);
  $prev = array_shift($trace);
  if ($prev['function'] == 'import_html_debug_code') {
    $prev = array_shift($trace);
  }
  $cap = DEBUGBACKTRACE;
  foreach ($trace as $key => $value) {
    $rich_trace[$value['function'] . '():' . $prev['line']] = $value;
    $prev = $value;
    if (--$cap <= 0) {
      break;
    }
  }
  return join(' - ', array_keys($rich_trace)) . ' | ';
}
