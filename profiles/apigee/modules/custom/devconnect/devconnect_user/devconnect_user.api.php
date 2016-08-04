<?php

/**
 * Allows modules to do something immediately after a user is saved to Edge.
 *
 * @param array $results
 *   Fields coming back from Edge
 * @param array $edit
 *   Fields that were sent to user_save
 * @param stdClass|null $account
 *   If this is a user edit, contains the user object. If it is a user save,
 *   it is NULL.
 */
function hook_devconnect_user_save(array $results, array &$edit, $account) {
  $action = ($account->uid ? 'created' : 'updated');
  $user_name = $edit['data']['name'];
  drupal_set_message("User $user_name was $action in Edge.");
}

/**
 * Allows modules to set a customized terms-and-conditions URL.
 *
 * @param stdClass $user
 *   Fully-loaded user object.
 *
 * @return string
 *   URL (relative to Drupal root) of T&C page.
 */
function hook_tnc_url($user) {
  if (user_access('administer site configuration', $user)) {
    return 'terms-and-conditions-administrator';
  }
  return 'terms-and-conditions';
}

/**
 * Determines if a user is a developer and should thus be synced with Edge.
 *
 * Note that this hook will NOT exclude developers from developer-sync.
 * To do that, you will need to implement hook_query_TAG_alter() for the tag
 * devconnect_user_sync.
 *
 * @param stdClass $account
 *   Fully-loaded user object
 *
 * @return bool
 *   TRUE if user should be synced with Edge, FALSE otherwise.
 */
function hook_devconnect_user_is_developer($user) {
  return array_key_exists(DRUPAL_AUTHENTICATED_RID, $user->roles);
}
