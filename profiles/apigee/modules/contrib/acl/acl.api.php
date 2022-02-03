<?php

/**
 * @file
 * API documentation for ACL.
 */

/**
 * Explain what your ACL grant records mean.
 */
function hook_acl_explain($acl_id, $name, $number, $users = NULL) {
  if (empty($users)) {
    return "ACL (id=$acl_id) would grant access to $name/$number.";
  }
  return "ACL (id=$acl_id) grants access to $name/$number to the listed user(s).";
}

/**
 * Inform ACL module that the client module is enabled.
 *
 * ACL will not return its NA records for your module if your module does not
 * confirm that it's active.
 *
 * If you use the example below, you can disable ACL on hook_disable using:
 * @code
 * function MYMODULE_disable() {
 *   MYMODULE_enabled(FALSE);
 * }
 * @endcode
 */
function hook_enabled($set = NULL) {
  static $enabled = TRUE; // not drupal_static!

  if ($set !== NULL) {
    $enabled = $set;
  }
  return $enabled;
}
