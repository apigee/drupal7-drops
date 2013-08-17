<?php
/**
 * @file
 * devconnect_default_content.features.user_permission.inc
 */

/**
 * Implements hook_user_default_permissions().
 */
function devconnect_default_content_user_default_permissions() {
  $permissions = array();

  // Exported permission: access ckeditor link.
  $permissions['access ckeditor link'] = array(
    'name' => 'access ckeditor link',
    'roles' => array(
      0 => 'administrator',
    ),
    'module' => 'ckeditor_link',
  );

  // Exported permission: access comments.
  $permissions['access comments'] = array(
    'name' => 'access comments',
    'roles' => array(
      0 => 'anonymous user',
      1 => 'authenticated user',
    ),
    'module' => 'comment',
  );

  // Exported permission: access content.
  $permissions['access content'] = array(
    'name' => 'access content',
    'roles' => array(
      0 => 'anonymous user',
      1 => 'authenticated user',
    ),
    'module' => 'node',
  );

  // Exported permission: administer ckeditor.
  $permissions['administer ckeditor'] = array(
    'name' => 'administer ckeditor',
    'roles' => array(
      0 => 'administrator',
    ),
    'module' => 'ckeditor',
  );

  // Exported permission: administer ckeditor link.
  $permissions['administer ckeditor link'] = array(
    'name' => 'administer ckeditor link',
    'roles' => array(
      0 => 'administrator',
    ),
    'module' => 'ckeditor_link',
  );

  // Exported permission: customize ckeditor.
  $permissions['customize ckeditor'] = array(
    'name' => 'customize ckeditor',
    'roles' => array(
      0 => 'administrator',
    ),
    'module' => 'ckeditor',
  );

  // Exported permission: use PHP for me alias paths.
  $permissions['use PHP for me alias paths'] = array(
    'name' => 'use PHP for me alias paths',
    'roles' => array(),
    'module' => 'me',
  );

  // Exported permission: view faq page.
  $permissions['view faq page'] = array(
    'name' => 'view faq page',
    'roles' => array(
      0 => 'anonymous user',
      1 => 'authenticated user',
    ),
    'module' => 'faq',
  );

  // Exported permission: view media.
  $permissions['view media'] = array(
    'name' => 'view media',
    'roles' => array(
      0 => 'anonymous user',
      1 => 'authenticated user',
    ),
    'module' => 'media',
  );

  return $permissions;
}
