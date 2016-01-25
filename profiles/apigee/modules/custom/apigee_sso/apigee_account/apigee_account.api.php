<?php
/**
 * @file
 * Example implementations of hooks exposed by the apigee_account module.
 */

/**
 * Allows modules to alter a user object or postlogin destination when the user
 * is logging in via accounts.apigee.com.
 *
 * @param array $session_info
 *   Array of string values returned from Apigee login.
 * @param stdClass $account
 *   The newly-logged-in user.
 * @param string $destination
 *   URL (relative to DRUPAL_ROOT) to which the user should be redirected after
 *   login.
 */
function hook_apigee_account_response(array $session_info, stdClass &$account, &$destination) {
  if ($session_info['name'] == 'admin') {
    $destination = 'user/' . $account->uid . '/edit';
  }
}