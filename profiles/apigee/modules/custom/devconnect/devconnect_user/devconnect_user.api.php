<?php

/**
 * Allows modules to do something immediately after a user is saved to Edge.
 *
 * @param array $results
 *        Fields coming back from Edge
 * @param array $edit
 *        Fields that were sent to user_save
 * @param stdClass|null $account
 *        If this is a user edit, contains the user object. If it is a user
 *        save, it is NULL.
 */
function hook_devconnect_user_save(array $results, array &$edit, $account) {
  $action = ($account->uid ? 'created' : 'updated');
  $user_name = $edit['data']['name'];
  drupal_set_message("User $user_name was $action in KMS.");
}

/**
 * Allows modules to set a customized terms-and-conditions URL
 *
 * @param stdClass $user
 * @return string
 */
function hook_tnc_url($user) {
  if (user_access('administer site configuration', $user)) {
    return 'terms-and-conditions-administrator';
  }
  return 'terms-and-conditions';
}
