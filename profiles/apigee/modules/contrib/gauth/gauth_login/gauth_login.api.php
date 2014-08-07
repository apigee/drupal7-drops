<?php

/**
 * @file
 * Hooks provided by the GAuth Login module.
 */

/**
 * @addtogroup hooks
 * @{
 */

/**
 * Find an existing user based on info from Google.
 *
 * @param array $info
 *   The 'userinfo' array from OAuth.
 *
 * @return object|NULL
 *   An existing Drupal user object if found; otherwise NULL.
 */
function hook_gauth_login_find_existing_user($info) {
  // Check to see if the user exists in a 3rd party system, ex. LDAP.
  if ($remote_user_object = remote_find_user($info['email'])) {
    // If so, we jumpstart creation of the local Drupal user and return it.
    return remote_create_drupal_user($remote_user_object);
  }
}

/**
 * @} End of "addtogroup hooks".
 */
