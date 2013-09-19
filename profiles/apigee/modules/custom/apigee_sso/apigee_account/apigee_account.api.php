<?php

/**
 * Allows modules to alter a user object or postlogin destination when the user
 * is logging in via accounts.apigee.com.
 *
 * @param array $session_info
 * @param stdClass $account
 * @param string $destination
 */
function hook_apigee_account_response(array $session_info, stdClass &$account, &$destination) {
  if ($session_info['name'] == 'stovak') {
    $destination = 'user/' . $account->uid . '/edit';
  }
}