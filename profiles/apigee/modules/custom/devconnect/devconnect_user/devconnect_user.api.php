<?php

/**
 * Allows modules to do something immediately after a user is saved to KMS.
 *
 * @param $results
 * @param $edit
 * @param $account
 */
function hook_devconnect_user_save($results, &$edit, $account) {
  $action = ($account->uid ? 'created' : 'updated');
  $user_name = $edit['name'];
  drupal_set_message("User $user_name was $action in KMS.");
}