<?php
/**
 * @file
 * devconnect_content_admin.features.user_role.inc
 */

/**
 * Implements hook_user_default_roles().
 */
function devconnect_content_admin_user_default_roles() {
  $roles = array();

  // Exported role: Content Admin.
  $roles['Content Admin'] = array(
    'name' => 'Content Admin',
    'weight' => '2',
  );

  return $roles;
}
