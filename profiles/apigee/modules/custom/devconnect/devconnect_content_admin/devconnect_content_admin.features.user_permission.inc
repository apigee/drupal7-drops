<?php
/**
 * @file
 * devconnect_content_admin.features.user_permission.inc
 */

/**
 * Implements hook_user_default_permissions().
 */
function devconnect_content_admin_user_default_permissions() {
  $permissions = array();

  // Exported permission: access administration menu.
  $permissions['access administration menu'] = array(
    'name' => 'access administration menu',
    'roles' => array(
      0 => 'Content Admin',
      1 => 'Drupal Administrator',
    ),
    'module' => 'admin_menu',
  );

  // Exported permission: access administration pages.
  $permissions['access administration pages'] = array(
    'name' => 'access administration pages',
    'roles' => array(
      0 => 'Content Admin',
      1 => 'Drupal Administrator',
    ),
    'module' => 'system',
  );

  // Exported permission: access broken links report.
  $permissions['access broken links report'] = array(
    'name' => 'access broken links report',
    'roles' => array(
      0 => 'Content Admin',
      1 => 'Drupal Administrator',
    ),
    'module' => 'linkchecker',
  );

  // Exported permission: access comments.
  $permissions['access comments'] = array(
    'name' => 'access comments',
    'roles' => array(
      0 => 'Content Admin',
      1 => 'Drupal Administrator',
      2 => 'anonymous user',
      3 => 'authenticated user',
    ),
    'module' => 'comment',
  );

  // Exported permission: access content.
  $permissions['access content'] = array(
    'name' => 'access content',
    'roles' => array(
      0 => 'Content Admin',
      1 => 'Drupal Administrator',
      2 => 'anonymous user',
      3 => 'authenticated user',
    ),
    'module' => 'node',
  );

  // Exported permission: access content overview.
  $permissions['access content overview'] = array(
    'name' => 'access content overview',
    'roles' => array(
      0 => 'Content Admin',
      1 => 'Drupal Administrator',
    ),
    'module' => 'node',
  );

  // Exported permission: access contextual links.
  $permissions['access contextual links'] = array(
    'name' => 'access contextual links',
    'roles' => array(
      0 => 'Content Admin',
      1 => 'Drupal Administrator',
    ),
    'module' => 'contextual',
  );

  // Exported permission: access own broken links report.
  $permissions['access own broken links report'] = array(
    'name' => 'access own broken links report',
    'roles' => array(
      0 => 'Content Admin',
      1 => 'Drupal Administrator',
    ),
    'module' => 'linkchecker',
  );

  // Exported permission: access printer-friendly version.
  $permissions['access printer-friendly version'] = array(
    'name' => 'access printer-friendly version',
    'roles' => array(
      0 => 'Content Admin',
      1 => 'Drupal Administrator',
    ),
    'module' => 'book',
  );

  // Exported permission: access statistics.
  $permissions['access statistics'] = array(
    'name' => 'access statistics',
    'roles' => array(
      0 => 'Content Admin',
      1 => 'Drupal Administrator',
    ),
    'module' => 'statistics',
  );

  // Exported permission: access user profiles.
  $permissions['access user profiles'] = array(
    'name' => 'access user profiles',
    'roles' => array(
      0 => 'Content Admin',
      1 => 'Drupal Administrator',
    ),
    'module' => 'user',
  );

  // Exported permission: add content to books.
  $permissions['add content to books'] = array(
    'name' => 'add content to books',
    'roles' => array(
      0 => 'Content Admin',
      1 => 'Drupal Administrator',
    ),
    'module' => 'book',
  );

  // Exported permission: add media from remote sources.
  $permissions['add media from remote sources'] = array(
    'name' => 'add media from remote sources',
    'roles' => array(
      0 => 'Content Admin',
      1 => 'Drupal Administrator',
    ),
    'module' => 'media_internet',
  );

  // Exported permission: administer blocks.
  $permissions['administer blocks'] = array(
    'name' => 'administer blocks',
    'roles' => array(
      0 => 'Content Admin',
      1 => 'Drupal Administrator',
    ),
    'module' => 'block',
  );

  // Exported permission: administer book outlines.
  $permissions['administer book outlines'] = array(
    'name' => 'administer book outlines',
    'roles' => array(
      0 => 'Content Admin',
      1 => 'Drupal Administrator',
    ),
    'module' => 'book',
  );

  // Exported permission: administer comments.
  $permissions['administer comments'] = array(
    'name' => 'administer comments',
    'roles' => array(
      0 => 'Content Admin',
      1 => 'Drupal Administrator',
    ),
    'module' => 'comment',
  );

  // Exported permission: administer faq.
  $permissions['administer faq'] = array(
    'name' => 'administer faq',
    'roles' => array(
      0 => 'Content Admin',
      1 => 'Drupal Administrator',
    ),
    'module' => 'faq',
  );

  // Exported permission: administer faq order.
  $permissions['administer faq order'] = array(
    'name' => 'administer faq order',
    'roles' => array(
      0 => 'Content Admin',
      1 => 'Drupal Administrator',
    ),
    'module' => 'faq',
  );

  // Exported permission: administer forums.
  $permissions['administer forums'] = array(
    'name' => 'administer forums',
    'roles' => array(
      0 => 'Content Admin',
      1 => 'Drupal Administrator',
    ),
    'module' => 'forum',
  );

  // Exported permission: administer google analytics.
  $permissions['administer google analytics'] = array(
    'name' => 'administer google analytics',
    'roles' => array(
      0 => 'Content Admin',
      1 => 'Drupal Administrator',
    ),
    'module' => 'googleanalytics',
  );

  // Exported permission: administer linkchecker.
  $permissions['administer linkchecker'] = array(
    'name' => 'administer linkchecker',
    'roles' => array(
      0 => 'Content Admin',
      1 => 'Drupal Administrator',
    ),
    'module' => 'linkchecker',
  );

  // Exported permission: administer media.
  $permissions['administer media'] = array(
    'name' => 'administer media',
    'roles' => array(
      0 => 'Content Admin',
      1 => 'Drupal Administrator',
    ),
    'module' => 'media',
  );

  // Exported permission: administer menu.
  $permissions['administer menu'] = array(
    'name' => 'administer menu',
    'roles' => array(
      0 => 'Content Admin',
      1 => 'Drupal Administrator',
    ),
    'module' => 'menu',
  );

  // Exported permission: administer meta tags.
  $permissions['administer meta tags'] = array(
    'name' => 'administer meta tags',
    'roles' => array(
      0 => 'Content Admin',
      1 => 'Drupal Administrator',
    ),
    'module' => 'metatag',
  );

  // Exported permission: administer modules.
  $permissions['administer modules'] = array(
    'name' => 'administer modules',
    'roles' => array(
      0 => 'Content Admin',
      1 => 'Drupal Administrator',
    ),
    'module' => 'system',
  );

  // Exported permission: administer nodes.
  $permissions['administer nodes'] = array(
    'name' => 'administer nodes',
    'roles' => array(
      0 => 'Content Admin',
      1 => 'Drupal Administrator',
    ),
    'module' => 'node',
  );

  // Exported permission: administer permissions.
  $permissions['administer permissions'] = array(
    'name' => 'administer permissions',
    'roles' => array(
      0 => 'Content Admin',
      1 => 'Drupal Administrator',
    ),
    'module' => 'user',
  );

  // Exported permission: administer search.
  $permissions['administer search'] = array(
    'name' => 'administer search',
    'roles' => array(
      0 => 'Content Admin',
      1 => 'Drupal Administrator',
    ),
    'module' => 'search',
  );

  // Exported permission: administer smtp module.
  $permissions['administer smtp module'] = array(
    'name' => 'administer smtp module',
    'roles' => array(
      0 => 'Content Admin',
      1 => 'Drupal Administrator',
    ),
    'module' => 'smtp',
  );

  // Exported permission: administer taxonomy.
  $permissions['administer taxonomy'] = array(
    'name' => 'administer taxonomy',
    'roles' => array(
      0 => 'Drupal Administrator',
    ),
    'module' => 'taxonomy',
  );

  // Exported permission: administer themes.
  $permissions['administer themes'] = array(
    'name' => 'administer themes',
    'roles' => array(
      0 => 'Content Admin',
      1 => 'Drupal Administrator',
    ),
    'module' => 'system',
  );

  // Exported permission: administer users.
  $permissions['administer users'] = array(
    'name' => 'administer users',
    'roles' => array(
      0 => 'Content Admin',
      1 => 'Drupal Administrator',
    ),
    'module' => 'user',
  );

  // Exported permission: administer views.
  $permissions['administer views'] = array(
    'name' => 'administer views',
    'roles' => array(
      0 => 'Content Admin',
      1 => 'Drupal Administrator',
    ),
    'module' => 'views',
  );

  // Exported permission: create article content.
  $permissions['create article content'] = array(
    'name' => 'create article content',
    'roles' => array(
      0 => 'Content Admin',
      1 => 'Drupal Administrator',
    ),
    'module' => 'node',
  );

  // Exported permission: create blog content.
  $permissions['create blog content'] = array(
    'name' => 'create blog content',
    'roles' => array(
      0 => 'Content Admin',
      1 => 'Drupal Administrator',
    ),
    'module' => 'node',
  );

  // Exported permission: create book content.
  $permissions['create book content'] = array(
    'name' => 'create book content',
    'roles' => array(
      0 => 'Content Admin',
    ),
    'module' => 'node',
  );

  // Exported permission: create faq content.
  $permissions['create faq content'] = array(
    'name' => 'create faq content',
    'roles' => array(
      0 => 'Content Admin',
      1 => 'Drupal Administrator',
    ),
    'module' => 'node',
  );

  // Exported permission: create forum content.
  $permissions['create forum content'] = array(
    'name' => 'create forum content',
    'roles' => array(
      0 => 'Content Admin',
      1 => 'authenticated user',
    ),
    'module' => 'node',
  );

  // Exported permission: create new books.
  $permissions['create new books'] = array(
    'name' => 'create new books',
    'roles' => array(
      0 => 'Content Admin',
      1 => 'Drupal Administrator',
    ),
    'module' => 'book',
  );

  // Exported permission: create page content.
  $permissions['create page content'] = array(
    'name' => 'create page content',
    'roles' => array(
      0 => 'Content Admin',
      1 => 'Drupal Administrator',
    ),
    'module' => 'node',
  );

  // Exported permission: delete any article content.
  $permissions['delete any article content'] = array(
    'name' => 'delete any article content',
    'roles' => array(
      0 => 'Content Admin',
      1 => 'Drupal Administrator',
    ),
    'module' => 'node',
  );

  // Exported permission: delete any blog content.
  $permissions['delete any blog content'] = array(
    'name' => 'delete any blog content',
    'roles' => array(
      0 => 'Content Admin',
      1 => 'Drupal Administrator',
    ),
    'module' => 'node',
  );

  // Exported permission: delete any book content.
  $permissions['delete any book content'] = array(
    'name' => 'delete any book content',
    'roles' => array(
      0 => 'Content Admin',
    ),
    'module' => 'node',
  );

  // Exported permission: delete any faq content.
  $permissions['delete any faq content'] = array(
    'name' => 'delete any faq content',
    'roles' => array(
      0 => 'Content Admin',
      1 => 'Drupal Administrator',
    ),
    'module' => 'node',
  );

  // Exported permission: delete any forum content.
  $permissions['delete any forum content'] = array(
    'name' => 'delete any forum content',
    'roles' => array(
      0 => 'Content Admin',
      1 => 'Drupal Administrator',
    ),
    'module' => 'node',
  );

  // Exported permission: delete any page content.
  $permissions['delete any page content'] = array(
    'name' => 'delete any page content',
    'roles' => array(
      0 => 'Content Admin',
      1 => 'Drupal Administrator',
    ),
    'module' => 'node',
  );

  // Exported permission: delete own article content.
  $permissions['delete own article content'] = array(
    'name' => 'delete own article content',
    'roles' => array(
      0 => 'Content Admin',
      1 => 'Drupal Administrator',
    ),
    'module' => 'node',
  );

  // Exported permission: delete own blog content.
  $permissions['delete own blog content'] = array(
    'name' => 'delete own blog content',
    'roles' => array(
      0 => 'Content Admin',
      1 => 'Drupal Administrator',
    ),
    'module' => 'node',
  );

  // Exported permission: delete own book content.
  $permissions['delete own book content'] = array(
    'name' => 'delete own book content',
    'roles' => array(
      0 => 'Content Admin',
    ),
    'module' => 'node',
  );

  // Exported permission: delete own faq content.
  $permissions['delete own faq content'] = array(
    'name' => 'delete own faq content',
    'roles' => array(
      0 => 'Content Admin',
      1 => 'Drupal Administrator',
    ),
    'module' => 'node',
  );

  // Exported permission: delete own forum content.
  $permissions['delete own forum content'] = array(
    'name' => 'delete own forum content',
    'roles' => array(
      0 => 'Content Admin',
      1 => 'Drupal Administrator',
    ),
    'module' => 'node',
  );

  // Exported permission: delete own page content.
  $permissions['delete own page content'] = array(
    'name' => 'delete own page content',
    'roles' => array(
      0 => 'Content Admin',
      1 => 'Drupal Administrator',
    ),
    'module' => 'node',
  );

  // Exported permission: delete revisions.
  $permissions['delete revisions'] = array(
    'name' => 'delete revisions',
    'roles' => array(
      0 => 'Content Admin',
      1 => 'Drupal Administrator',
    ),
    'module' => 'node',
  );

  // Exported permission: delete terms in 1.
  $permissions['delete terms in 1'] = array(
    'name' => 'delete terms in 1',
    'roles' => array(
      0 => 'Drupal Administrator',
    ),
    'module' => 'taxonomy',
  );

  // Exported permission: delete terms in 2.
  $permissions['delete terms in 2'] = array(
    'name' => 'delete terms in 2',
    'roles' => array(
      0 => 'Drupal Administrator',
    ),
    'module' => 'taxonomy',
  );

  // Exported permission: delete terms in 3.
  $permissions['delete terms in 3'] = array(
    'name' => 'delete terms in 3',
    'roles' => array(),
    'module' => 'taxonomy',
  );

  // Exported permission: display drupal links.
  $permissions['display drupal links'] = array(
    'name' => 'display drupal links',
    'roles' => array(
      0 => 'Content Admin',
      1 => 'Drupal Administrator',
    ),
    'module' => 'admin_menu',
  );

  // Exported permission: edit any article content.
  $permissions['edit any article content'] = array(
    'name' => 'edit any article content',
    'roles' => array(
      0 => 'Content Admin',
      1 => 'Drupal Administrator',
    ),
    'module' => 'node',
  );

  // Exported permission: edit any blog content.
  $permissions['edit any blog content'] = array(
    'name' => 'edit any blog content',
    'roles' => array(
      0 => 'Content Admin',
      1 => 'Drupal Administrator',
    ),
    'module' => 'node',
  );

  // Exported permission: edit any book content.
  $permissions['edit any book content'] = array(
    'name' => 'edit any book content',
    'roles' => array(
      0 => 'Content Admin',
    ),
    'module' => 'node',
  );

  // Exported permission: edit any faq content.
  $permissions['edit any faq content'] = array(
    'name' => 'edit any faq content',
    'roles' => array(
      0 => 'Content Admin',
      1 => 'Drupal Administrator',
    ),
    'module' => 'node',
  );

  // Exported permission: edit any forum content.
  $permissions['edit any forum content'] = array(
    'name' => 'edit any forum content',
    'roles' => array(
      0 => 'Content Admin',
      1 => 'Drupal Administrator',
    ),
    'module' => 'node',
  );

  // Exported permission: edit any page content.
  $permissions['edit any page content'] = array(
    'name' => 'edit any page content',
    'roles' => array(
      0 => 'Content Admin',
      1 => 'Drupal Administrator',
    ),
    'module' => 'node',
  );

  // Exported permission: edit link settings.
  $permissions['edit link settings'] = array(
    'name' => 'edit link settings',
    'roles' => array(
      0 => 'Content Admin',
      1 => 'Drupal Administrator',
    ),
    'module' => 'linkchecker',
  );

  // Exported permission: edit media.
  $permissions['edit media'] = array(
    'name' => 'edit media',
    'roles' => array(
      0 => 'Content Admin',
      1 => 'Drupal Administrator',
    ),
    'module' => 'media',
  );

  // Exported permission: edit meta tags.
  $permissions['edit meta tags'] = array(
    'name' => 'edit meta tags',
    'roles' => array(
      0 => 'Content Admin',
      1 => 'Drupal Administrator',
    ),
    'module' => 'metatag',
  );

  // Exported permission: edit own article content.
  $permissions['edit own article content'] = array(
    'name' => 'edit own article content',
    'roles' => array(
      0 => 'Content Admin',
      1 => 'Drupal Administrator',
    ),
    'module' => 'node',
  );

  // Exported permission: edit own blog content.
  $permissions['edit own blog content'] = array(
    'name' => 'edit own blog content',
    'roles' => array(
      0 => 'Content Admin',
      1 => 'Drupal Administrator',
    ),
    'module' => 'node',
  );

  // Exported permission: edit own book content.
  $permissions['edit own book content'] = array(
    'name' => 'edit own book content',
    'roles' => array(
      0 => 'Content Admin',
    ),
    'module' => 'node',
  );

  // Exported permission: edit own comments.
  $permissions['edit own comments'] = array(
    'name' => 'edit own comments',
    'roles' => array(
      0 => 'Content Admin',
      1 => 'Drupal Administrator',
    ),
    'module' => 'comment',
  );

  // Exported permission: edit own faq content.
  $permissions['edit own faq content'] = array(
    'name' => 'edit own faq content',
    'roles' => array(
      0 => 'Content Admin',
      1 => 'Drupal Administrator',
    ),
    'module' => 'node',
  );

  // Exported permission: edit own forum content.
  $permissions['edit own forum content'] = array(
    'name' => 'edit own forum content',
    'roles' => array(
      0 => 'Content Admin',
      1 => 'Drupal Administrator',
    ),
    'module' => 'node',
  );

  // Exported permission: edit own page content.
  $permissions['edit own page content'] = array(
    'name' => 'edit own page content',
    'roles' => array(
      0 => 'Content Admin',
      1 => 'Drupal Administrator',
    ),
    'module' => 'node',
  );

  // Exported permission: edit terms in 1.
  $permissions['edit terms in 1'] = array(
    'name' => 'edit terms in 1',
    'roles' => array(
      0 => 'Drupal Administrator',
    ),
    'module' => 'taxonomy',
  );

  // Exported permission: edit terms in 2.
  $permissions['edit terms in 2'] = array(
    'name' => 'edit terms in 2',
    'roles' => array(
      0 => 'Drupal Administrator',
    ),
    'module' => 'taxonomy',
  );

  // Exported permission: edit terms in 3.
  $permissions['edit terms in 3'] = array(
    'name' => 'edit terms in 3',
    'roles' => array(),
    'module' => 'taxonomy',
  );

  // Exported permission: export nodes.
  $permissions['export nodes'] = array(
    'name' => 'export nodes',
    'roles' => array(
      0 => 'Drupal Administrator',
    ),
    'module' => 'node_export',
  );

  // Exported permission: export own nodes.
  $permissions['export own nodes'] = array(
    'name' => 'export own nodes',
    'roles' => array(
      0 => 'Drupal Administrator',
    ),
    'module' => 'node_export',
  );

  // Exported permission: flush caches.
  $permissions['flush caches'] = array(
    'name' => 'flush caches',
    'roles' => array(
      0 => 'Content Admin',
      1 => 'Drupal Administrator',
    ),
    'module' => 'admin_menu',
  );

  // Exported permission: grant content access.
  $permissions['grant content access'] = array(
    'name' => 'grant content access',
    'roles' => array(
      0 => 'Content Admin',
      1 => 'Drupal Administrator',
    ),
    'module' => 'content_access',
  );

  // Exported permission: grant own content access.
  $permissions['grant own content access'] = array(
    'name' => 'grant own content access',
    'roles' => array(
      0 => 'Content Admin',
      1 => 'Drupal Administrator',
    ),
    'module' => 'content_access',
  );

  // Exported permission: opt-in or out of tracking.
  $permissions['opt-in or out of tracking'] = array(
    'name' => 'opt-in or out of tracking',
    'roles' => array(
      0 => 'Drupal Administrator',
    ),
    'module' => 'googleanalytics',
  );

  // Exported permission: post comments.
  $permissions['post comments'] = array(
    'name' => 'post comments',
    'roles' => array(
      0 => 'Content Admin',
      1 => 'Drupal Administrator',
      2 => 'authenticated user',
    ),
    'module' => 'comment',
  );

  // Exported permission: revert revisions.
  $permissions['revert revisions'] = array(
    'name' => 'revert revisions',
    'roles' => array(
      0 => 'Content Admin',
      1 => 'Drupal Administrator',
    ),
    'module' => 'node',
  );

  // Exported permission: search content.
  $permissions['search content'] = array(
    'name' => 'search content',
    'roles' => array(
      0 => 'Content Admin',
      1 => 'Drupal Administrator',
      2 => 'anonymous user',
      3 => 'authenticated user',
    ),
    'module' => 'search',
  );

  // Exported permission: skip comment approval.
  $permissions['skip comment approval'] = array(
    'name' => 'skip comment approval',
    'roles' => array(
      0 => 'Content Admin',
      1 => 'Drupal Administrator',
      2 => 'authenticated user',
    ),
    'module' => 'comment',
  );

  // Exported permission: use advanced search.
  $permissions['use advanced search'] = array(
    'name' => 'use advanced search',
    'roles' => array(
      0 => 'Content Admin',
      1 => 'Drupal Administrator',
      2 => 'anonymous user',
      3 => 'authenticated user',
    ),
    'module' => 'search',
  );

  // Exported permission: use text format filtered_html.
  $permissions['use text format filtered_html'] = array(
    'name' => 'use text format filtered_html',
    'roles' => array(
      0 => 'Content Admin',
      1 => 'Drupal Administrator',
    ),
    'module' => 'filter',
  );

  // Exported permission: use text format full_html.
  $permissions['use text format full_html'] = array(
    'name' => 'use text format full_html',
    'roles' => array(
      0 => 'Content Admin',
      1 => 'Drupal Administrator',
    ),
    'module' => 'filter',
  );

  // Exported permission: use text format markdown.
  $permissions['use text format markdown'] = array(
    'name' => 'use text format markdown',
    'roles' => array(
      0 => 'Content Admin',
      1 => 'Drupal Administrator',
    ),
    'module' => 'filter',
  );

  // Exported permission: view faq page.
  $permissions['view faq page'] = array(
    'name' => 'view faq page',
    'roles' => array(
      0 => 'Content Admin',
      1 => 'Drupal Administrator',
    ),
    'module' => 'faq',
  );

  // Exported permission: view media.
  $permissions['view media'] = array(
    'name' => 'view media',
    'roles' => array(
      0 => 'Content Admin',
      1 => 'Drupal Administrator',
      2 => 'anonymous user',
      3 => 'authenticated user',
    ),
    'module' => 'media',
  );

  // Exported permission: view own unpublished content.
  $permissions['view own unpublished content'] = array(
    'name' => 'view own unpublished content',
    'roles' => array(
      0 => 'Content Admin',
      1 => 'Drupal Administrator',
    ),
    'module' => 'node',
  );

  // Exported permission: view revisions.
  $permissions['view revisions'] = array(
    'name' => 'view revisions',
    'roles' => array(
      0 => 'Content Admin',
      1 => 'Drupal Administrator',
    ),
    'module' => 'node',
  );

  // Exported permission: view the administration theme.
  $permissions['view the administration theme'] = array(
    'name' => 'view the administration theme',
    'roles' => array(
      0 => 'Content Admin',
      1 => 'Drupal Administrator',
    ),
    'module' => 'system',
  );

  return $permissions;
}
