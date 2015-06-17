<?php
/**
 * @file
 * Contains install steps for the Apigee profile.
 */

use Drupal\devconnect\Crypto;

require_once DRUPAL_ROOT . '/profiles/apigee/modules/custom/devconnect/lib/Crypto.php';


define('SMARTDOCS_SAMPLE_PETSTORE_MODEL', 'petstore_example');
define('SMARTDOCS_SAMPLE_WEATHER_MODEL', 'weather_example');

/**
 * Selects the Apigee Profile.
 *
 * @param array $install_state
 *   Current state of installer.
 */
function apigee_install_select_profile(&$install_state) {
  $install_state['parameters']['profile'] = 'apigee';
}

/**
 * Ensure the locale is set to English.
 *
 * @param array $install_state
 *   Current state of installer.
 */
function apigee_install_select_locale(&$install_state) {
  $install_state['parameters']['locale'] = 'en';
}

/**
 * Loads our install profile and enhances the dependency information.
 *
 * @param array $install_state
 *   Current state of installer.
 */
function apigee_install_load_profile(&$install_state) {
  // Loading the install profile normally.
  install_load_profile($install_state);
  if (array_key_exists('PANTHEON_ENVIRONMENT', $_SERVER)) {
    $install_state['profile_info']['dependencies'][] = 'pantheon_api';
  }
  // Include any dependencies that we might have missed...
  $dependencies = $install_state['profile_info']['dependencies'];
  foreach ($dependencies as $module) {
    $module_info = drupal_parse_info_file(drupal_get_path('module', $module) . '/' . $module . '.info');
    if (!empty($module_info['dependencies'])) {
      foreach ($module_info['dependencies'] as $dependency) {
        $parts = explode(' (', $dependency, 2);
        $dependencies[] = $parts[0];
      }
    }
  }
  $install_state['profile_info']['dependencies'] = array_unique($dependencies);
}

/**
 * Create batch items for apigee install.
 *
 * @param array $install_state
 *   Current state of installer.
 *
 * @return array
 *   Batch items.
 */
function apigee_install_configure_batch(&$install_state) {

  _apigee_manage_memory();

  return array(
    'title' => t('Configuring your install...'),
    'operations' => array(
      array('apigee_install_configure_variables', array()),
      array('apigee_install_pantheon_push_solr', array()),
      array('apigee_install_configure_solr', array()),
      array('apigee_install_configure_users', array()),
      array('apigee_install_configure_themes', array()),
      array('apigee_install_content_types', array()),
      array('apigee_install_enable_blog_content_types', array()),
      array('apigee_install_rebuild_permissions', array()),
      array('apigee_install_create_homepage', array()),
      array('apigee_install_base_ckeditor_settings', array()),
      array('apigee_install_create_taxonomy_terms', array()),
      array('apigee_install_create_tutorial_content', array()),
      array('apigee_install_create_forum_content', array()),
      array('apigee_install_create_page_content', array()),
      array('apigee_install_create_audio_content', array()),
      array('apigee_install_create_video_content', array()),
      array('apigee_install_create_faq_content', array()),
      array('apigee_install_create_environmental_indicators', array()),
      array('apigee_install_clear_caches_flush', array()),
      array('apigee_install_clear_caches_css', array()),
      array('apigee_install_clear_caches_js', array()),
      array('apigee_install_clear_caches_theme', array()),
      array('apigee_install_clear_caches_entity', array()),
      array('apigee_install_clear_caches_nodes', array()),
      array('apigee_install_clear_caches_menu', array()),
      array('apigee_install_clear_caches_actions', array()),
      array('apigee_install_clear_caches_core_path', array()),
      array('apigee_install_clear_caches_core_filter', array()),
      array('apigee_install_clear_caches_core_bootstrap', array()),
      array('apigee_install_clear_caches_core_page', array()),
      array('apigee_install_clear_caches_core', array()),
      array('apigee_install_bootstrap_status', array()),
    ),
    'finished' => '_apigee_install_configure_task_finished',
  );
}

/**
 * Implements callback_batch_finished().
 *
 * Ensures the given task is completed.
 */
function _apigee_install_configure_task_finished($success, $results, $operations) {
  watchdog('apigee_install', 'Configure Task Finished', array(), WATCHDOG_INFO);
  $GLOBALS['install_state']['batch_configure_complete'] = install_verify_completed_task();
}

/**
 * Configure variables across the environment properly.
 *
 * @param array $context
 *   Current state of installer.
 */
function apigee_install_configure_variables(array &$context) {
  watchdog('apigee_install', 'Config Vars', array(), WATCHDOG_INFO);
  if (array_key_exists('PRESSFLOW_SETTINGS', $_SERVER)) {
    $pressflow = json_decode($_SERVER['PRESSFLOW_SETTINGS'], TRUE);
    $conf = $pressflow['conf'];
  }
  else {
    $conf = array(
      'file_public_path' => 'sites/default/files',
      'file_private_path' => 'sites/default/private',
      'file_temporary_path' => 'sites/default/tmp',
    );
  }
  try {
    file_prepare_directory($conf['file_public_path'], FILE_CREATE_DIRECTORY);
    file_prepare_directory($conf['file_temporary_path'], FILE_CREATE_DIRECTORY);
    file_prepare_directory($conf['file_private_path'], FILE_CREATE_DIRECTORY);
  }
  catch (Exception $e) {
    drupal_set_message(t('unable to create the directories necessary for Drupal to write files: :error', array(
      ':error' => $e->getMessage(),
    )));
  }

  $crypt_key = drupal_random_bytes(32);

  variable_set('file_public_path', $conf['file_public_path']);
  variable_set('file_temporary_path', $conf['file_temporary_path']);
  variable_set('file_private_path', $conf['file_private_path']);
  variable_set('cache', 1);
  variable_set('block_cache', 1);
  variable_set('cache_lifetime', '0');
  variable_set('page_cache_maximum_age', '900');
  variable_set('page_compression', 0);
  variable_set('preprocess_css', 0);
  variable_set('preprocess_js', 0);
  variable_set('clean_url', TRUE);
  variable_set('site_name', 'New Apigee Site');
  variable_set('site_mail', 'noreply@apigee.com');
  // Designed by Apigee in California.
  variable_set('date_default_timezone', 'America/Los_Angeles');
  variable_set('site_default_country', 'US');
  variable_set('jquery_update_compression_type', 'none');
  variable_set('jquery_update_jquery_cdn', 'google');
  variable_set('jquery_update_jquery_version', '1.7');
  variable_set('user_email_verification', FALSE);
  variable_set('error_level', 0);
  variable_set('devconnect_api_product_handling', 'single_required');
  variable_set('devconnect_callback_handling', 'require');
  variable_set('devconnect_developer_apps_apiproduct_widget', 'checkboxes');
  variable_set('bootstrap_version', '3');
  variable_set('bootstrap_modal_forms_login', '1');
  variable_set('bootstrap_modal_forms_register', '1');
  // Set use email for login flag.
  variable_set('logintoboggan_login_with_email', 1);
  // Set immediate login to false by default.
  variable_set('logintoboggan_immediate_login_on_register', 0);
  // Set immediate login to false by default.
  variable_set('logintoboggan_override_destination_parameter', 0);

  // Detect private dir if it is configured already.
  $configured_private_dir = $conf['file_private_path'];
  // Make sure private dir exists and is writable.
  if (!empty($configured_private_dir)) {
    if (!is_dir(DRUPAL_ROOT . '/' . $configured_private_dir) || !is_writable(DRUPAL_ROOT . '/' . $configured_private_dir)) {
      unset($configured_private_dir);
    }
  }
  // Do we need to guess where to store?
  if (!isset($configured_private_dir)) {
    foreach (array('sites/default/files/private', 'sites/default/private') as $private_dir) {
      if (is_dir(DRUPAL_ROOT . '/' . $private_dir) && is_writable(DRUPAL_ROOT . '/' . $private_dir)) {
        $configured_private_dir = $private_dir;
        break;
      }
    }
  }
  if (!isset($configured_private_dir)) {
    $configured_private_dir = $conf['file_public_path'] . '/private';
    if (!file_exists(DRUPAL_ROOT . '/' . $configured_private_dir)) {
      mkdir($configured_private_dir);
    }
  }
  variable_set('apigee_credential_dir', $configured_private_dir);
  file_put_contents($configured_private_dir . '/.apigee.key', $crypt_key);

  $context['results'][] = 'variables';
  $context['message'] = st('Default variables set.');
}

/**
 * Pushes apachesolr XML to Pantheon server.
 *
 * @param array $context
 *   Current state of installer.
 */
function apigee_install_pantheon_push_solr(array &$context) {
  if (array_key_exists('PANTHEON_ENVIRONMENT', $_SERVER) && module_exists('pantheon_apachesolr')) {
    watchdog('apigee_install', 'Pushing Solr', array(), WATCHDOG_INFO);
    module_load_include('module', 'pantheon_apachesolr');
    pantheon_apachesolr_post_schema_exec('profiles/apigee/modules/contrib/apachesolr/solr-conf/solr-3.x/schema.xml');
    $context['results'][] = 'solr_push';
    $context['message'] = st('Solr config pushed to pantheon solr server.');
  }
  else {
    watchdog('apigee_install', 'Solr not enabled.', array(), WATCHDOG_NOTICE);
    $context['results'][] = 'solr_push';
    $context['message'] = st('Solr is not enabled, no need to push to Solr server.');
  }
}

/**
 * Solr config batch item.
 *
 * @param array $context
 *   Current state of installer.
 */
function apigee_install_configure_solr(array &$context) {
  watchdog('apigee_install', 'Configuring Solr', array(), WATCHDOG_INFO);
  $search_default_module = 'apachesolr_search';
  if (module_exists('apachesolr')) {
    $search_active_modules = array(
      'apachesolr_search' => 'apachesolr_search',
      'user' => 'user',
      'node' => 0,
    );
  }
  else {
    $search_active_modules = array(
      'apachesolr_search' => 0,
      'user' => 'user',
      'node' => 'node',
    );
    $search_default_module = 'node';
  }
  user_role_grant_permissions(DRUPAL_AUTHENTICATED_RID, array('search content'));
  user_role_grant_permissions(DRUPAL_ANONYMOUS_RID, array('search content'));
  variable_set('search_active_modules', $search_active_modules);
  variable_set('search_default_module', $search_default_module);
  $context['results'][] = 'solr_push';
  $context['message'] = st('Search Configured.');
}

/**
 * Configures standard user functionality across the platform.
 *
 * @param array $context
 *   Current state of installer.
 */
function apigee_install_configure_users(array &$context) {
  watchdog('apigee_install', 'Configuring Default Users', array(), WATCHDOG_INFO);
  $admin_role = new stdClass();
  $admin_role->name = 'administrator';
  $admin_role->weight = 10;
  user_role_save($admin_role);
  db_insert('users_roles')
    ->fields(array('uid' => 1, 'rid' => $admin_role->rid))
    ->execute();
  $roles = array_keys(user_roles());
  foreach ($roles as $role) {
    user_role_grant_permissions($role, array('node' => 'access content'));
  }
  variable_set('user_admin_role', $admin_role->rid);
  user_role_grant_permissions($admin_role->rid, array_keys(module_invoke_all('permission')));
  // Grant anonymous user permissions.
  $permissions = array('access comments', 'access content', 'view faq page');
  user_role_grant_permissions(1, $permissions);
  // Grant authenticated user permissions.
  $permissions[] = 'post comments';
  user_role_grant_permissions(2, $permissions);
  $roles = array(3 => TRUE, 4 => TRUE);
  $user = (object) array(
    'uid' => 1,
    'name' => 'admin',
    'pass' => md5(time()),
    'mail' => 'noreply@apigee.com',
    'field_first_name' => array(LANGUAGE_NONE => array(array('value' => 'drupal'))),
    'field_last_name' => array(LANGUAGE_NONE => array(array('value' => 'admin'))),
    'status' => 1,
    'access' => REQUEST_TIME,
    'roles' => $roles,
  );
  $results = user_save($user);
  if ($results) {
    drupal_set_message(t('Admin user created. Use password recovery or drush to set the password.'));
  }
  else {
    drupal_set_message(t('Unable to create admin user.'));
  }
  $context['results'][] = 'admin_user';
  $context['message'] = st('Admin User Created.');
}

/**
 * Configures default themes batch item.
 *
 * @param array $context
 *   Current state of installer.
 */
function apigee_install_configure_themes(array &$context) {
  watchdog('apigee_install', 'Configuring themes', array(), WATCHDOG_INFO);
  // Activate admin theme when editing a node.
  variable_set('node_admin_theme', '1');
  db_update('system')
    ->fields(array('status' => 0))
    ->condition('type', 'theme')
    ->execute();
  $enable = array(
    'theme_default' => 'apigee_responsive',
    'admin_theme' => 'rubik',
  );
  try {
    theme_enable($enable);
    foreach ($enable as $var => $theme) {
      if (!is_numeric($var)) {
        variable_set($var, $theme);
      }
    }
    db_update('block')->fields(array('status' => 0, 'region' => -1))->condition('delta', 'main', '!=')->execute();
  }
  catch (Exception $e) {
    watchdog_exception('apigee_install', $e, 'ERROR CONFIGURING THEMES %message', array('%message' => $e->getMessage()), WATCHDOG_ERROR);
  }
  $context['results'][] = 'themes';
  $context['message'] = st('Default Apigee theme configured.');
}

/**
 * Installs the various content types that the profile will use.
 *
 * @param array $context
 *   Current state of installer.
 */
function apigee_install_content_types(array &$context) {
  watchdog('apigee_install', 'Creating default content types', array(), WATCHDOG_INFO);
  $types = array(
    array(
      'type' => 'page',
      'name' => st('Basic page'),
      'base' => 'node_content',
      'description' => st('Use <em>basic pages</em> for your static content, such as an \'About us\' page.'),
      'custom' => 1,
      'modified' => 1,
      'locked' => 0,
    ),
    array(
      'type' => 'article',
      'name' => st('Article'),
      'base' => 'node_content',
      'description' => st('Use <em>articles</em> for time-sensitive content like news, press releases or blog posts.'),
      'custom' => 1,
      'modified' => 1,
      'locked' => 0,
    ),
  );

  foreach ($types as $type) {
    $type = node_type_set_defaults($type);
    node_type_save($type);
    node_add_body_field($type);
  }
  variable_set('pathauto_node_page_pattern', '[node:title]');
  variable_set('pathauto_node_blog_pattern', 'blog/[node:title]');
  variable_set('pathauto_node_faq_pattern', 'faqs/[node:title]');
  $context['results'][] = 'content_types';
  $context['message'] = st('Default content types created.');
}

/**
 * Enables the devconnect_blog_content_types module.
 *
 * @param array $context
 *   Current state of installer.
 */
function apigee_install_enable_blog_content_types(array &$context) {
  // Needs to be done here not in the .info file.
  module_enable(array('devconnect_blog_content_types'));
  $context['results'][] = 'blog_content_types';
  $context['message'] = st('Enabling DevConnect Blog Content Types!');
}

/**
 * Installs the default homepage.
 *
 * @param array $context
 *   Current state of installer.
 */
function apigee_install_create_homepage(array &$context) {
  watchdog('apigee_install', 'Generating Homepage', array(), WATCHDOG_INFO);
  $homepage = (object) array(
    'title' => 'home',
    'body' => array(),
    'type' => 'page',
    'status' => TRUE,
    'comment' => FALSE,
    'promote' => FALSE,
    'sticky' => FALSE,
  );
  try {
    node_save($homepage);
    variable_set("site_frontpage", "node/{$homepage->nid}");
    $context['results'][] = 'homepage_created';
    $context['message'] = st('Default Homepage Generated!');
  }
  catch (Exception $e) {
    watchdog_exception('apigee_install', $e, 'Error generating home page: %message', array('%message' => $e->getMessage()), WATCHDOG_ERROR);
    $context['results'][] = 'homepage_created';
    $context['message'] = st('No need to generate default homepage...');
  }
}

/**
 * Creates the CKEditor settings for the portal.
 *
 * @param array $context
 *   Current state of installer.
 */
function apigee_install_base_ckeditor_settings(array &$context) {
  if (!module_exists('filter')) {
    module_enable(array('filter'), TRUE);
  }
  module_load_include('module', 'filter');
  $filters = filter_get_filters();
  if (!in_array('filtered_html', array_keys($filters))) {
    filter_format_save((object) array(
        'format' => 'filtered_html',
        'name' => 'Filtered HTML',
        'cache' => 1,
        'status' => 1,
        'weight' => 0,
        'filters' => array(
          'filter_html' => array(
            'weight' => -10,
            'status' => 1,
            'settings' => array(
              'allowed_html' => '<a> <em> <strong> <cite> <blockquote> <code> <ul> <ol> <li> <dl> <dt> <dd>',
              'filter_html_help' => 1,
              'filter_html_nofollow' => 0,
            ),
          ),
          'ckeditor_link_filter' => array(
            'weight' => 0,
            'status' => 1,
            'settings' => array(),
          ),
          'wysiwyg' => array(
            'weight' => 0,
            'status' => 1,
            'settings' => array(
              'valid_elements' => 'a[!href|target<_blank|title],
div[align<center?justify?left?right],
p[align<center?justify?left?right],
br,span,em,strong,cite,code,blockquote,ul,ol,li,dl,dt,dd',
              'allow_comments' => 0,
              'nofollow_policy' => 'whitelist',
              'nofollow_domains' => array(),
              'style_color' => array(
                'color' => 0,
                'background' => 0,
                'background-color' => 0,
                'background-image' => 0,
                'background-repeat' => 0,
                'background-attachment' => 0,
                'background-position' => 0,
              ),
              'style_font' => array(
                'font' => 0,
                'font-family' => 0,
                'font-size' => 0,
                'font-size-adjust' => 0,
                'font-stretch' => 0,
                'font-style' => 0,
                'font-variant' => 0,
                'font-weight' => 0,
              ),
              'style_text' => array(
                'text-align' => 0,
                'text-decoration' => 0,
                'text-indent' => 0,
                'text-transform' => 0,
                'letter-spacing' => 0,
                'word-spacing' => 0,
                'white-space' => 0,
                'direction' => 0,
                'unicode-bidi' => 0,
              ),
              'style_box' => array(
                'margin' => 0,
                'margin-top' => 0,
                'margin-right' => 0,
                'margin-bottom' => 0,
                'margin-left' => 0,
                'padding' => 0,
                'padding-top' => 0,
                'padding-right' => 0,
                'padding-bottom' => 0,
                'padding-left' => 0,
              ),
              'style_border-1' => array(
                'border' => 0,
                'border-top' => 0,
                'border-right' => 0,
                'border-bottom' => 0,
                'border-left' => 0,
                'border-width' => 0,
                'border-top-width' => 0,
                'border-right-width' => 0,
                'border-bottom-width' => 0,
                'border-left-width' => 0,
              ),
              'style_border-2' => array(
                'border-color' => 0,
                'border-top-color' => 0,
                'border-right-color' => 0,
                'border-bottom-color' => 0,
                'border-left-color' => 0,
                'border-style' => 0,
                'border-top-style' => 0,
                'border-right-style' => 0,
                'border-bottom-style' => 0,
                'border-left-style' => 0,
              ),
              'style_dimension' => array(
                'height' => 0,
                'line-height' => 0,
                'max-height' => 0,
                'max-width' => 0,
                'min-height' => 0,
                'min-width' => 0,
                'width' => 0,
              ),
              'style_positioning' => array(
                'bottom' => 0,
                'clip' => 0,
                'left' => 0,
                'overflow' => 0,
                'right' => 0,
                'top' => 0,
                'vertical-align' => 0,
                'z-index' => 0,
              ),
              'style_layout' => array(
                'clear' => 0,
                'display' => 0,
                'float' => 0,
                'position' => 0,
                'visibility' => 0,
              ),
              'style_list' => array(
                'list-style' => 0,
                'list-style-image' => 0,
                'list-style-position' => 0,
                'list-style-type' => 0,
              ),
              'style_table' => array(
                'border-collapse' => 0,
                'border-spacing' => 0,
                'caption-side' => 0,
                'empty-cells' => 0,
                'table-layout' => 0,
              ),
              'style_user' => array(
                'cursor' => 0,
                'outline' => 0,
                'outline-width' => 0,
                'outline-style' => 0,
                'outline-color' => 0,
                'zoom' => 0,
              ),
              'rule_valid_classes' => array(),
              'rule_valid_ids' => array(),
              'rule_style_urls' => array(),
            ),
          ),
        ),
      )
    );
    db_insert('role_permission')->fields(array(
      'rid' => 2,
      'permission' => 'use text format filtered_html',
      'module' => 'filter',
    ))->execute();
    db_insert('role_permission')->fields(array(
      'rid' => 3,
      'permission' => 'use text format filtered_html',
      'module' => 'filter',
    ))->execute();
  }
  if (!in_array('full_html', array_keys($filters))) {
    // Exported format: Full HTML.
    filter_format_save((object) array(
        'format' => 'full_html',
        'name' => 'Full HTML',
        'cache' => 1,
        'status' => 1,
        'weight' => 0,
        'filters' => array(
          'ckeditor_link_filter' => array(
            'weight' => 0,
            'status' => 1,
            'settings' => array(),
          ),
          'filter_autop' => array(
            'weight' => 0,
            'status' => 1,
            'settings' => array(),
          ),
        ),
      )
    );
    db_insert('role_permission')->fields(array(
      'rid' => 3,
      'permission' => 'use text format full_html',
      'module' => 'filter',
    ))->execute();
  }

  $plugin_names = array(
    'a11yhelp', 'about', 'basicstyles', 'blockquote', 'button',
    'ckeditor_link', 'clipboard', 'contextmenu', 'dialog', 'dialogui',
    'drupalbreaks', 'elementspath', 'enterkey', 'entities', 'fakeobjects',
    'filebrowser', 'floatingspace', 'floatpanel', 'horizontalrule',
    'htmlwriter', 'iframe', 'image', 'insertpre', 'indent', 'indentlist',
    'lineutils', 'list', 'magicline', 'media', 'mediaembed', 'menu',
    'menubutton', 'panel', 'pastefromword', 'pastetext', 'popup',
    'removeformat', 'richcombo', 'scayt', 'sharedspace', 'sourcearea',
    'sourcedialog', 'specialchar', 'stylescombo', 'tab',
    'tableresize', 'toolbarswitch', 'widget',
    'wysiwygarea', 'trifold', 'featurette', 'jumbotron', 'carousel',
  );
  $plugins = array();
  foreach ($plugin_names as $plugin_name) {
    $plugins[$plugin_name] = array(
      'name' => $plugin_name,
      'desc' => 'Plugin file: ' . $plugin_name,
      'path' => '%plugin_dir_extra%' . $plugin_name . '/',
      'buttons' => '',
      'default' => 'f',
    );
  }
  // Now handle exceptions to the rule.
  $plugins['ckeditor_link']['path'] = '%base_path%profiles/apigee/modules/contrib/ckeditor_link/plugins/link/';
  $plugins['ckeditor_link']['name'] = 'drupal_path';
  // Is this a bug?
  unset($plugins['ckeditor_link']['default']);

  $plugins['drupalbreaks']['path'] = '%plugin_dir%drupalbreaks/';
  $plugins['drupalbreaks']['buttons'] = array(
    'DrupalBreak' => array(
      'label' => 'DrupalBreak',
      'icon' => 'images/drupalbreak.png',
    ),
  );

  $plugins['media']['path'] = '%plugin_dir%media/';
  $plugins['media']['buttons'] = array(
    'Media' => array(
      'label' => 'Media',
      'icon' => 'images/icon.gif',
    ),
  );

  $plugins['mediaembed']['path'] = '%plugin_dir%mediaembed/';
  $plugins['mediaembed']['buttons'] = array(
    'MediaEmbed' => array(
      'label' => 'MediaEmbed',
      'icon' => 'images/icon.png',
    ),
  );

  // Set default-true plugins.
  foreach (array('drupalbreaks', 'tableresize', 'toolbarswitch', 'widget', 'wysiwygarea', 'trifold', 'featurette', 'jumbotron', 'carousel') as $plugin_name) {
    $plugins[$plugin_name]['default'] = 't';
  }
  // Set paths for bootstrap plugins.
  foreach (array('trifold', 'featurette', 'jumbotron', 'carousel') as $plugin_name) {
    $plugins[$plugin_name]['path'] = '%base_path%profiles/apigee/modules/contrib/ckeditor_bootstrap/plugins/' . $plugin_name . '/';
  }

  $ckeditor_filtered = array(
    'ss' => 2,
    'default' => 't',
    'show_toggle' => 't',
    'uicolor' => 'default',
    'uicolor_user' => 'default',
    'toolbar' => "
    [['Source'],
    ['Cut','Copy','Paste','PasteText','PasteFromWord','-','SpellChecker','Scayt'],
    ['Undo','Redo','Find','Replace','-','SelectAll','RemoveFormat'],
    ['Media','Table','HorizontalRule','Smiley','SpecialChar','Iframe'],
    '/',
    ['Bold','Italic','Underline','Strike','-','Subscript','Superscript'],
    ['NumberedList','BulletedList','-','Outdent','Indent','Blockquote','CreateDiv'],
    ['JustifyLeft','JustifyCenter','JustifyRight','JustifyBlock','-','BidiLtr','BidiRtl'],
    ['Link','Unlink','Anchor','MediaEmbed'],
    ['DrupalBreak'],
    '/',
    ['Format','Font','FontSize'],
    ['TextColor','BGColor'],
    ['Maximize','ShowBlocks']]",
    'expand' => 't',
    'width' => '100%',
    'lang' => 'en',
    'auto_lang' => 't',
    'language_direction' => 'default',
    'enter_mode' => 'p',
    'shift_enter_mode' => 'br',
    'font_format' => 'p;div;pre;address;h1;h2;h3;h4;h5;h6',
    'custom_formatting' => 'f',
    'formatting' => array(
      'custom_formatting_options' => array(
        'indent' => 'indent',
        'breakBeforeOpen' => 'breakBeforeOpen',
        'breakAfterOpen' => 'breakAfterOpen',
        'breakAfterClose' => 'breakAfterClose',
        'breakBeforeClose' => '0',
        'pre_indent' => '0',
      ),
    ),
    'css_mode' => 'self',
    'css_path' => '%tcss/bootstrap.min.css',
    'css_style' => 'theme',
    'styles_path' => '',
    'filebrowser' => 'none',
    'filebrowser_image' => '',
    'filebrowser_flash' => '',
    'UserFilesPath' => '%b%f/',
    'UserFilesAbsolutePath' => '%d%b%f/',
    'forcePasteAsPlainText' => 'f',
    'html_entities' => 't',
    'scayt_autoStartup' => 'f',
    'theme_config_js' => 'f',
    'js_conf' => 'config.allowedContent = true;',
    'loadPlugins' => $plugins,
  );
  $ckeditor_full = $ckeditor_filtered;
  $ckeditor_full['toolbar'] = "
    [['Source'],
    ['Cut','Copy','Paste','PasteText','PasteFromWord','-','SpellChecker','Scayt'],
    ['Undo','Redo','Find','Replace','-','SelectAll','RemoveFormat'],
    ['Media','Table','HorizontalRule','Smiley','SpecialChar','Iframe'],
    '/',
    ['Bold','Italic','Underline','Strike','-','Subscript','Superscript'],
    ['NumberedList','BulletedList','-','Outdent','Indent','Blockquote','CreateDiv'],
    ['JustifyLeft','JustifyCenter','JustifyRight','JustifyBlock','-','BidiLtr','BidiRtl'],
    ['Link','Unlink','Anchor','MediaEmbed'],
    ['DrupalBreak'],
    '/',
    ['Format','Font','FontSize'],
    ['TextColor','BGColor'],
    ['Maximize','ShowBlocks'],
    ['Carousel','Featurette','Jumbotron','Trifold','InsertPre']]";

  $ckeditor_global_settings = array(
    'skin' => 'apigee',
    'ckeditor_path' => '%l/ckeditor',
    'ckeditor_local_path' => '',
    'ckeditor_plugins_path' => '%l/ckeditor/plugins',
    'ckeditor_plugins_local_path' => '',
    'ckfinder_path' => '%m/ckfinder',
    'ckfinder_local_path' => '',
    'ckeditor_aggregate' => 't',
    'toolbar_wizard' => 't',
    'loadPlugins' => array(),
  );
  db_delete('ckeditor_settings')
    ->condition('name', array('Full', 'Advanced', 'CKEditor Global Profile'))
    ->execute();
  db_insert('ckeditor_settings')
    ->fields(array(
      'name' => 'filtered',
      'settings' => serialize($ckeditor_filtered),
    ))
    ->execute();
  db_insert('ckeditor_settings')
    ->fields(array(
      'name' => 'full',
      'settings' => serialize($ckeditor_full),
    ))
    ->execute();
  db_insert('ckeditor_settings')
    ->fields(array(
      'name' => 'CKEditor Global Profile',
      'settings' => serialize($ckeditor_global_settings),
    ))
    ->execute();
  db_delete('ckeditor_input_format')
    ->condition('name', 'Full')
    ->execute();
  db_delete('ckeditor_input_format')
    ->condition('name', 'Advanced')
    ->execute();
  db_insert('ckeditor_input_format')->fields(array('name' => 'filtered', 'format' => 'filtered_html'))->execute();
  db_insert('ckeditor_input_format')->fields(array('name' => 'full', 'format' => 'full_html'))->execute();
  $context['results'][] = 'ckeditor_settings';
  $context['message'] = st('CKEditor Settings Built');
}

/**
 * Creates dummy taxonomy terms.
 *
 * @param array $context
 *   Current state of installer.
 */
function apigee_install_create_taxonomy_terms(array &$context) {
  for ($i = 0; $i <= 5; $i++) {
    $term = new stdClass();
    $term->name = _apigee_install_generate_greek(1, TRUE);
    $term->vid = taxonomy_vocabulary_machine_name_load('forums')->vid;
    taxonomy_term_save($term);
  }
  $context['results'][] = 'content_created';
  $context['message'] = st('Example Taxonomy Terms Created!');
}

/**
 * Creates example tutorial content.
 *
 * @param array $context
 *   Current state of installer.
 */
function apigee_install_create_tutorial_content(array &$context) {
  $posts = array(
    array(
      'title' => 'Portal Start Up Guide',
      'body' => '<p>The&nbsp;<em>developer portal</em>&nbsp;is a template portal, designed as a base that you can easily' .
        ' customize to meet your specific requirements</p>
        <p>Your customized developer portal should educate developers about your API&mdash;what it is and how it\'s used. It' .
        ' should also enable you to manage developer use of your API. This could include authorizing developers to use your API,' .
        ' giving developers an easy way to create apps that use your API products, assigning developers specific roles and permissions' .
        ' related to the API, or revoking developer access to the API as necessary. Beyond that, your developer portal can serve' .
        ' as the focal point for community activity, where developers can contribute API-related content to social media repositories' .
        ' such as blogs and forums.</p>' .
        ' <p>View more information about developer portals at ' .
        l('apigee.com', 'http://apigee.com/docs/developer-channel/content/what-developer-portal') . '</p>',
      'keyword' => 'Portal',
    ),
    array(
      'title' => 'Customizing your portal',
      'body' => '<p>You can customize the appearance of the developer portal to match your company theme, to add new content areas' .
        ' to the portal, or to change the layout of any page on the portal. Much of this configuration requires a working knowledge' .
        ' of ' . l('Drupal', 'https://drupal.org') . '. However, there is documentation that describes some of the basic tasks that you might want to' .
        ' perform to customize your portal.</p>' .
        ' <p>View more information about customizing your developer portals at ' .
        l('www.apigee.com', 'http://apigee.com/docs/developer-channel/content/customize-appearance') . '.</p>',
      'keyword' => 'Tutorials',
    ),
  );
  foreach ($posts as $post) {
    $body = array();
    $body['post'] = $post['body'];
    $body['title'] = $post['title'];
    $fields = array();
    $fields['type'] = 'tutorial';
    $fields['keyword'] = $post['keyword'];
    $fields['vid'] = taxonomy_vocabulary_machine_name_load('blog')->vid;
    _apigee_install_generate_node('article', $body, $fields);
  }
  $context['results'][] = 'content_created';
  $context['message'] = st('Tutorial Content Generated!');
}

/**
 * Creates default forum content for the install.
 *
 * @param array $context
 *   Current state of installer.
 */
function apigee_install_create_forum_content(array &$context) {
  // Create 10 forum posts.
  for ($i = 0; $i <= 7; $i++) {
    $body = array();
    $body['title'] = _apigee_install_generate_greek(mt_rand(3, 10));
    $body['post'] = _apigee_install_generate_greek(mt_rand(2, 300), TRUE);
    $fields = array();
    $fields['type'] = 'forum';
    $fields['keyword'] = _apigee_install_generate_greek(mt_rand(2, 5), TRUE);
    $fields['vid'] = taxonomy_vocabulary_machine_name_load('forums')->vid;
    _apigee_install_generate_node('forum', $body, $fields);
  }
  user_role_grant_permissions(DRUPAL_AUTHENTICATED_RID, array('create forum content', 'edit own forum content'));
  $context['results'][] = 'content_created';
  $context['message'] = st('10 Example Forum Posts Created!');
}

/**
 * Creates default page content for the install.
 *
 * @param array $context
 *   Current state of installer.
 */
function apigee_install_create_page_content(array &$context) {
  // Create five pages.
  for ($i = 0; $i <= 5; $i++) {
    $body = array();
    $body['title'] = _apigee_install_generate_greek(mt_rand(3, 10));
    $body['post'] = _apigee_install_generate_greek(mt_rand(2, 300), TRUE);
    _apigee_install_generate_node('page', $body);
  }
  $context['results'][] = 'content_created';
  $context['message'] = st('5 Example Pages Created!');
}

/**
 * Creates default audio content for the install.
 *
 * @param array $context
 *   Current state of installer.
 */
function apigee_install_create_audio_content(array &$context) {
  $fid = _apigee_content_types_get_fid('http://www.lullabot.com/sites/default/files/podcasts/drupalizeme_033.mp3', 'drupalizeme_033.mp3', 'audio/mpeg', 'audio');
  $blog_tid = _apigee_get_blog_tid();
  $t = get_t();
  $node = (object) array(
    'title' => $t('Drupalize.Me Podcast #33'),
    'status' => 1,
    'comment' => 2,
    'promote' => 1,
    'sticky' => 0,
    'type' => 'blog_audio',
    'language' => LANGUAGE_NONE,
    'translate' => 0,
    'field_content_tag' => array(LANGUAGE_NONE => array(array('tid' => $blog_tid))),
    'body' => array(
      LANGUAGE_NONE => array(
        array(
          'value' => '',
          'summary' => '',
          'format' => 'full_html',
        ),
      ),
    ),
    'field_audio' => array(
      LANGUAGE_NONE => array(
        array(
          'fid' => $fid,
          'display' => 1,
        ),
      ),
    ),
  );
  node_save($node);
  _apigee_content_types_set_file_usage($fid, $node->nid);
  if (!module_exists('pathauto')) {
    $path = array('source' => 'node/' . $node->nid, 'alias' => 'blog/drupalizeme-podcast-33');
    path_save($path);
  }
  $context['results'][] = 'content_created';
  $context['message'] = st('Audio Content Created!');
}

/**
 * Creates default video content for the install.
 *
 * @param array $context
 *   Current state of installer.
 */
function apigee_install_create_video_content(array &$context) {
  $fid = _apigee_content_types_get_fid('youtube://v/twWnGnQG_1s', 'twWnGnQG_1s', 'video/youtube', 'video');
  $blog_tid = _apigee_get_blog_tid();
  $t = get_t();
  $node = (object) array(
    'title' => $t('Your API Made Better'),
    'status' => 1,
    'comment' => 2,
    'promote' => 1,
    'sticky' => 0,
    'type' => 'blog_video',
    'language' => LANGUAGE_NONE,
    'translate' => 0,
    'body' => array(
      LANGUAGE_NONE => array(
        array(
          'value' => '<p>' . $t('Marsh and Brian from Apigee help your API Program take it to the next level!') . '</p>',
          'summary' => '',
          'format' => 'full_html',
        ),
      ),
    ),
    'field_content_tag' => array(LANGUAGE_NONE => array(array('tid' => $blog_tid))),
    'field_video' => array(
      LANGUAGE_NONE => array(
        array(
          'fid' => $fid,
          'display' => 1,
          'description' => '',
        ),
      ),
    ),
  );
  node_save($node);
  _apigee_content_types_set_file_usage($fid, $node->nid);
  // Now create path alias.
  if (!module_exists('pathauto')) {
    $path = array('source' => 'node/' . $node->nid, 'alias' => 'blog/your-api-sucks');
    path_save($path);
  }
  $context['results'][] = 'content_created';
  $context['message'] = st('Video Content Created!');
}

/**
 * Creates default FAQ content for the install.
 *
 * @param array $context
 *   Current state of installer.
 */
function apigee_install_create_faq_content(array &$context) {
  $type = 'faq';
  for ($i = 0; $i <= 3; $i++) {
    _apigee_install_generate_node($type, $body = NULL, $fields = NULL);
  }
  $context['results'][] = 'content_created';
  $context['message'] = st('FAQ Example Content Created!');
}

/**
 * Helper function that sets the file usage.
 *
 * @param int $fid
 *   File identifier
 * @param int $nid
 *   Node identifier
 */
function _apigee_content_types_set_file_usage($fid, $nid) {
  $fid_exists = db_select('file_usage', 'fu')
    ->fields('fu', array('fid'))
    ->condition('type', 'node')
    ->condition('id', $nid)
    ->condition('fid', $fid)
    ->execute()
    ->fetchField();
  if (!$fid_exists) {
    db_insert('file_usage')
      ->fields(array(
        'fid' => $fid,
        'module' => 'file',
        'type' => 'node',
        'id' => $nid,
        'count' => 1,
      ))
      ->execute();
  }
}

/**
 * Helper function to find (or create) an entry in the file_managed table.
 *
 * @param string $uri
 *   URI of the managed file
 * @param string $filename
 *   Filename of the managed file
 * @param string $filemime
 *   MIME type of the managed file
 * @param string $type
 *   Content-type to which file is attached.
 *
 * @return int
 *   File ID of the managed file.
 */
function _apigee_content_types_get_fid($uri, $filename, $filemime, $type) {
  $fid = db_select('file_managed', 'fm')
    ->fields('fm', array('fid'))
    ->condition('uri', $uri)
    ->execute()
    ->fetchField();
  if (!$fid) {
    $file = (object) array(
      'uid' => 0,
      'filename' => $filename,
      'uri' => $uri,
      'filemime' => $filemime,
      'status' => 1,
      'type' => $type,
      'alt' => '',
      'title' => '',
    );
    $file = file_save($file);
    $fid = $file->fid;
  }
  return $fid;
}

/**
 * Gets the tid for the 'blog' term in the 'content_type_tag' vocabulary.
 *
 * @param int|null $blog_tid
 *   If $blog_tid is passed in, it is set as the tid for future use.
 *
 * @return int
 *   The tid of the blog term.
 *
 * @TODO Why aren't we using taxonomy_get_term_by_name()?
 */
function _apigee_get_blog_tid($blog_tid = NULL) {
  static $tid;
  if (isset($blog_tid)) {
    $tid = $blog_tid;
  }
  if (!isset($tid)) {
    $query = db_select('taxonomy_vocabulary', 'v');
    $query->innerJoin('taxonomy_term_data', 't', 'v.vid = t.vid');
    $tid = $query->condition('v.machine_name', 'content_type_tag')
      ->condition('t.name', 'blog')
      ->fields('t', array('tid'))
      ->execute()
      ->fetchField();
  }
  return $tid;
}

/**
 * Helper function to generate a node.
 *
 * @param string $type
 *   Node content-type to be created.
 * @param string|null $body
 *   Body of the node.
 * @param null|array $fields
 *   Abbreviated descriptors of fields to be created on the node.
 */
function _apigee_install_generate_node($type, $body = NULL, $fields = NULL) {
  $node = new stdClass();
  $node->nid = NULL;
  $node->type = $type;
  $users = array();

  $result = db_select('users', 'u')
    ->fields('u', array('uid'))
    ->range(0, 50)
    ->execute();
  foreach ($result as $record) {
    $users[] = $record->uid;
  }
  $users = array_merge($users, array('0'));
  node_object_prepare($node);
  $node->uid = $users[array_rand($users)];
  $node->title = (isset($body['title'])) ? $body['title'] : _apigee_install_generate_greek(mt_rand(2, 7), TRUE);
  $node->language = LANGUAGE_NONE;
  if (isset($body['post'])) {
    $node->body[LANGUAGE_NONE][0]['value'] = $body['post'];
    $node->body[LANGUAGE_NONE][0]['summary'] = $body['post'];
    $node->body[LANGUAGE_NONE][0]['format'] = 'filtered_html';
  }
  else {
    $nparas = mt_rand(1, 12);
    $output = '';
    for ($i = 1; $i <= $nparas; $i++) {
      $output .= "<p>" . _apigee_install_generate_greek(mt_rand(10, 60)) . "</p>" . "\n\n";
    }
    $node->body[LANGUAGE_NONE][0]['value'] = $output;
    $node->body[LANGUAGE_NONE][0]['summary'] = $output;
    $node->body[LANGUAGE_NONE][0]['format'] = 'filtered_html';
  }
  $node->comment = 1;
  $node->status = 1;
  $node->created = REQUEST_TIME - mt_rand(0, 604800);
  if (!is_null($fields)) {
    switch ($fields['type']) {
      case 'tutorial':
        if (isset($fields['vid'])) {
          $vid = $fields['vid'];
          $term = taxonomy_get_term_by_name($fields['keyword'], 'blog');
          if (empty($term)) {
            if ($vid) {
              taxonomy_term_save((object) array(
                'name' => $fields['keyword'],
                'vid' => $vid,
              ));
              $keyword = taxonomy_get_term_by_name($fields['keyword'], 'blog');
            }
          }
          else {
            if ($vid) {
              $keyword = taxonomy_get_term_by_name($fields['keyword'], 'blog');
            }
          }
          if (isset($keyword)) {
            foreach ($keyword as $obj) {
              $node->field_keywords[LANGUAGE_NONE][]['tid'] = $obj->tid;
            }
          }
          foreach (taxonomy_get_term_by_name('blog', 'content_type_tag') as $obj) {
            $node->field_content_tag[LANGUAGE_NONE][]['tid'] = $obj->tid;
          }
        }
        break;

      case 'forum':
        if (isset($fields['vid'])) {
          $node->comment = 2;
          $rand = array_rand(taxonomy_get_tree($fields['vid']));
          $tree = taxonomy_get_tree($fields['vid']);
          $node->taxonomy_forums[LANGUAGE_NONE][]['tid'] = $tree[$rand]->tid;
        }
        break;

      default:
        break;
    }
  }
  if ($node->type == 'faq') {
    $node->detailed_question = $node->title;
  }
  node_save($node);
}

/**
 * Generates filler content for generated nodes.
 *
 * Mimics Devel's devel_creating_greeking, but makes this profile not rely on
 * that module.
 *
 * @param int $word_count
 *   How many words are to be created?
 * @param bool $title
 *   Are we creating a node title rather than node body?
 *
 * @return string
 *   String of greeked "words".
 */
function _apigee_install_generate_greek($word_count, $title = FALSE) {
  static $greek_flipped = NULL;
  if (!isset($greek_flipped)) {
    $greek = file(dirname(__FILE__) . '/greek.txt', FILE_IGNORE_NEW_LINES | FILE_SKIP_EMPTY_LINES);
    $greek_flipped = array_flip($greek);
  }
  if ($word_count > count($greek_flipped)) {
    $word_count = count($greek_flipped);
  }

  $greek_text = '';
  if (!$title) {
    $words_remaining = $word_count;
    while ($words_remaining > 0) {
      $sentence_length = mt_rand(3, 10);
      $words = array_rand($greek_flipped, $sentence_length);
      $sentence = implode(' ', $words);
      $greek_text .= ucfirst($sentence) . '. ';
      $words_remaining -= $sentence_length;
    }
  }
  else {
    // Use slightly different method for titles.
    $words = array_rand($greek_flipped, $word_count);
    $words = is_array($words) ? implode(' ', $words) : $words;
    $greek_text = ucwords($words);
  }
  return trim($greek_text);
}

/**
 * Rebuilds permissions.
 *
 * @param array $context
 *   Current state of installer.
 */
function apigee_install_rebuild_permissions(array &$context) {
  watchdog('apigee_install', 'rebuilding permissions', array(), WATCHDOG_INFO);
  try {
    node_access_rebuild(TRUE);
  }
  catch (Exception $e) {
    watchdog_exception('apigee_install', $e, 'Error rebuilding node access: %message', array('%message' => $e->getMessage()), WATCHDOG_ERROR);
  }
  $context['results'][] = 'content_permissions';
  $context['message'] = st('Content Permissions Rebuilt');
}

/**
 * Flushes caches for the JS/CSS cache.
 *
 * @param array $context
 *   Current state of installer.
 */
function apigee_install_clear_caches_flush(array &$context) {
  watchdog('apigee_install', 'Flushing CSS/JS', array(), WATCHDOG_INFO);
  _drupal_flush_css_js();
  $context['results'][] = 'cache_flush';
  $context['message'] = st('CSS & JS flushed');
}

/**
 * Rebuilds the registry for the site.
 *
 * @param array $context
 *   Current state of installer.
 */
function apigee_install_rebuild_registry(array &$context) {
  watchdog('apigee_install', 'Rebuilding Registry', array(), WATCHDOG_INFO);
  registry_rebuild();
  $context['results'][] = 'cache_registry';
  $context['message'] = st('Registry Rebuilt');
}

/**
 * Clears the CSS Cache.
 *
 * @param array $context
 *   Current state of installer.
 */
function apigee_install_clear_caches_css(array &$context) {
  watchdog('apigee_install', 'Clearing CSS Cache', array(), WATCHDOG_INFO);
  drupal_clear_css_cache();
  $context['results'][] = 'cache_css';
  $context['message'] = st('CSS Caches Cleared');
}

/**
 * Clears the JS Cache.
 *
 * @param array $context
 *   Current state of installer.
 */
function apigee_install_clear_caches_js(array &$context) {
  watchdog('apigee_install', 'Clearing JS Cache', array(), WATCHDOG_INFO);
  drupal_clear_js_cache();
  $context['results'][] = 'cache_js';
  $context['message'] = st('JS Caches Cleared');
}

/**
 * Clears the theme cache.
 *
 * @param array $context
 *   Current state of installer.
 */
function apigee_install_clear_caches_theme(array &$context) {
  watchdog('apigee_install', 'Rebuilding themes', array(), WATCHDOG_INFO);
  system_rebuild_theme_data();
  drupal_theme_rebuild();
  $context['results'][] = 'cache_theme';
  $context['message'] = st('Theme Caches Cleared');
}

/**
 * Clears the entity cache.
 *
 * @param array $context
 *   Current state of installer.
 */
function apigee_install_clear_caches_entity(array &$context) {
  watchdog('apigee_install', 'Clearing Entity Cache', array(), WATCHDOG_INFO);
  entity_info_cache_clear();
  $context['results'][] = 'cache_entity';
  $context['message'] = st('Entity Caches Cleared');
}

/**
 * Rebuilds the node types cache.
 *
 * @param array $context
 *   Current state of installer.
 */
function apigee_install_clear_caches_nodes(array &$context) {
  watchdog('apigee_install', 'Rebuilding Node Types', array(), WATCHDOG_INFO);
  node_types_rebuild();
  $context['results'][] = 'cache_node';
  $context['message'] = st('Node Caches Cleared');
}

/**
 * Rebuilds the menu.
 *
 * @param array $context
 *   Current state of installer.
 */
function apigee_install_clear_caches_menu(array &$context) {
  watchdog('apigee_install', 'Rebuilding Menu', array(), WATCHDOG_INFO);
  menu_rebuild();
  $context['results'][] = 'cache_menu';
  $context['message'] = st('Menu Caches Cleared');
}

/**
 * Synchronizes Actions.
 *
 * @param array $context
 *   Current state of installer.
 */
function apigee_install_clear_caches_actions(array &$context) {
  watchdog('apigee_install', 'Synchronizing Actions...', array(), WATCHDOG_INFO);
  actions_synchronize();
  $context['results'][] = 'cache_action';
  $context['message'] = st('Action Caches Cleared');
}

/**
 * Helper function to flush a supplied cache.
 *
 * @param string $table
 *   Name of cache table to clear.
 * @param string $results_label
 *   Human-readable name of cache for logs.
 * @param string $message_label
 *   Human-readable name of cache for UI.
 * @param array $context
 *   Current state of installer.
 */
function _apigee_install_clear_cache($table, $results_label, $message_label, &$context) {
  static $cache_tables;
  if (!isset($cache_tables)) {
    $cache_tables = module_invoke_all('flush_caches');
  }
  watchdog('apigee_install', "Flushing $results_label caches...", array(), WATCHDOG_INFO);
  $my_cache_tables = array_merge($cache_tables, array($table));
  foreach ($my_cache_tables as $my_table) {
    cache_clear_all('*', $my_table, TRUE);
  }
  $context['results'][] = $results_label;
  $context['message'] = st("$message_label caches cleared");
}

/**
 * Clears core cache.
 *
 * @param array $context
 *   Current state of installer.
 */
function apigee_install_clear_caches_core(array &$context) {
  _apigee_install_clear_cache('cache', 'cache_core', 'Core', $context);
}

/**
 * Clears core path cache.
 *
 * @param array $context
 *   Current state of installer.
 */
function apigee_install_clear_caches_core_path(array &$context) {
  _apigee_install_clear_cache('cache_path', 'cache_path', 'Path', $context);
}

/**
 * Clears cache filter.
 *
 * @param array $context
 *   Current state of installer.
 */
function apigee_install_clear_caches_core_filter(array &$context) {
  _apigee_install_clear_cache('cache_filter', 'cache_filter', 'Filter', $context);
}

/**
 * Clears bootstrap cache.
 *
 * @param array $context
 *   Current state of installer.
 */
function apigee_install_clear_caches_core_bootstrap(array &$context) {
  _apigee_install_clear_cache('cache_bootstrap', 'cache_bootstrap', 'Bootstrap', $context);
}

/**
 * Clears page cache.
 *
 * @param array $context
 *   Current state of installer.
 */
function apigee_install_clear_caches_core_page(array &$context) {
  _apigee_install_clear_cache('cache_page', 'cache_page', 'Page', $context);
}

/**
 * Updates the bootstrap status.
 *
 * @param array $context
 *   Current state of installer.
 */
function apigee_install_bootstrap_status(array &$context) {
  watchdog('apigee_install', 'Updating bootstrap status...', array(), WATCHDOG_INFO);
  _system_update_bootstrap_status();
  drupal_get_messages();
  $context['results'][] = 'bootstrap_status';
  $context['message'] = st('Bootstrap Status Reset.');
}

/**
 * Form constructor for setting Apigee endpoint configuration vars.
 *
 * @param array $form
 *   The form being constructed.
 * @param array $form_state
 *   State of the form being constructed.
 *
 * @return array
 *   The newly-created form.
 */
function apigee_install_api_endpoint($form, &$form_state) {
  if (isset($_REQUEST['devconnect_org'])) {
    $org = $_REQUEST['devconnect_org'];
  }
  else {
    if (array_key_exists('PANTHEON_ENVIRONMENT', $_SERVER)) {
      $org = str_replace($_SERVER['PANTHEON_ENVIRONMENT'] . '-', '', $_SERVER['HTTP_HOST']);
      $org = str_replace('.devportal.apigee.com', '', $org);
    }
    else {
      $org = '';
    }
  }
  if (isset($_REQUEST['devconnect_endpoint'])) {
    $endpoint = $_REQUEST['devconnect_endpoint'];
  }
  else {
    $endpoint = 'https://api.enterprise.apigee.com/v1';
  }
  $attributes = array(
    'autocomplete' => 'off',
    'autocorrect' => 'off',
    'autocapitalize' => 'off',
    'spellcheck' => 'false',
  );
  $form = array();

  $form['script'] = array(
    '#type' => 'markup',
    '#markup' => "<script>function togglePass(a) {
  var i = a.parentNode.getElementsByTagName('input')[0];
  if (i.type == 'password') {
    a.innerHTML = 'Hide password';
    i.type = 'text';
  } else {
    a.innerHTML = 'Show password';
    i.type = 'password';
  }
}</script>",
  );

  $form['org'] = array(
    '#type' => 'textfield',
    '#title' => t('Management API Organization'),
    '#required' => TRUE,
    '#default_value' => $org,
    '#description' => t('The v4 product organization name. Changing this value could make your site not work.'),
    '#attributes' => $attributes,
  );
  $form['endpoint'] = array(
    '#type' => 'textfield',
    '#title' => t('Management API Endpoint URL'),
    '#required' => TRUE,
    '#default_value' => $endpoint,
    '#description' => t('URL to which to make Edge REST calls. For on-prem installs you will need to change this value.'),
    '#attributes' => $attributes,
  );
  $form['user'] = array(
    '#type' => 'textfield',
    '#title' => t('Endpoint Authenticated User'),
    '#required' => TRUE,
    '#default_value' => '',
    '#description' => t('User name used when authenticating with the endpoint. Generally this takes the form of an email address.'),
    '#attributes' => $attributes + array('placeholder' => 'username'),
  );
  $form['pass'] = array(
    '#type' => 'textfield',
    '#title' => t('Authenticated User’s Password'),
    '#required' => TRUE,
    '#default_value' => '',
    '#description' => t('Password used when authenticating with the endpoint.'),
    '#attributes' => $attributes,
    '#post_render' => array('apigee_endpoint_password_post_render'),
  );

  $form['actions'] = array(
    '#weight' => 100,
    '#attributes' => array(
      'class' => array('container-inline'),
    ),
  );
  $form['actions']['save'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#attributes' => array(
      'style' => 'float:left;',
    ),
  );
  $form['actions']['skip'] = array(
    '#type' => 'submit',
    '#limit_validation_errors' => array(),
    '#value' => t('Skip this config'),
    '#submit' => array('apigee_skip_api_endpoint'),
    '#attributes' => array(
      'style' => 'float:left;',
    ),

  );
  $form['#submit'][] = 'apigee_install_api_endpoint_submit';
  $form['#validate'][] = 'apigee_install_api_endpoint_validate';
  return $form;
}

/**
 * Validates if the connection is successful.
 *
 * @param array $form
 *   The form being validated.
 * @param array $form_state
 *   The state of the form being validated.
 */
function apigee_install_api_endpoint_validate($form, &$form_state) {
  $org = trim($form_state['values']['org']);
  $endpoint = trim($form_state['values']['endpoint']);
  $user = trim($form_state['values']['user']);
  $pass = trim($form_state['values']['pass']);
  module_load_include('inc', 'devconnect', 'devconnect.admin');
  $return = _devconnect_test_kms_connection($org, $endpoint, $user, $pass);
  // Was connection successful?
  if (strpos($return, t('Connection Successful')) === FALSE) {
    form_set_error('form', $return);
  }
}

/**
 * Pre-render callback for password fields.
 *
 * @param array $element
 *   The password element about to be rendered.
 *
 * @return array
 *   Password element descriptor to be rendered.
 */
function apigee_password_pre_render($element) {
  unset($element['pass1']['#title']);
  unset($element['pass1']['#attributes']['class']);
  unset($element['pass2']['#attributes']['class']);
  return $element;

}

/**
 * Post-render callback for a password field.
 *
 * @param string $content
 *   The rendered output of the form element.
 * @param array $element
 *   The form element being rendered.
 *
 * @return string
 *   HTML for the password element.
 */
function apigee_endpoint_password_post_render($content, $element) {
  $content = str_replace('type="text"', 'type="password"', $content);
  $toggle = '<a style="font-size:0.75em" href="#" onclick="togglePass(this); return false">Show password</a>';
  $search = '<div class="description">';
  $content = str_replace($search, $toggle . $search, $content);
  return $content;
}

/**
 * Form submit handler that skips the devconnect installation piece.
 *
 * @param array $form
 *   The form being submitted
 * @param array $form_state
 *   State of the form being submitted.
 */
function apigee_skip_api_endpoint($form, &$form_state) {
  global $install_state;
  $install_state['parameters']['edge_configured'] = FALSE;
  $install_state['completed_task'] = install_verify_completed_task();
}

/**
 * Installs the endpoint credentials for the management server.
 *
 * @param array $form
 *   The form being submitted
 * @param array $form_state
 *   State of the form being submitted.
 */
function apigee_install_api_endpoint_submit($form, &$form_state) {
  global $install_state;

  drupal_load('module', 'devconnect');
  $config = devconnect_get_org_settings();
  foreach (array('org', 'endpoint', 'user', 'pass') as $key) {
    $value = $form_state['values'][$key];
    $config[$key] = $value;
  }
  $config['connection_timeout'] = 16;
  $config['request_timeout'] = 16;

  $private_dir = variable_get('apigee_credential_dir', NULL);
  $key = devconnect_get_crypt_key();
  Crypto::setKey($key);
  file_put_contents(DRUPAL_ROOT . '/' . $private_dir . '/.apigee', Crypto::encrypt(serialize($config)));

  $install_state['parameters']['edge_configured'] = TRUE;
  $install_state['completed_task'] = install_verify_completed_task();
}

/**
 * Database settings form constructor.
 *
 * @param array $form
 *   The form being constructed.
 * @param array $form_state
 *   The state of the form being constructed.
 * @param array $install_state
 *   An array of information about the current installation state.
 *
 * @return array
 *   Newly-created form.
 */
function apigee_install_settings_form($form, &$form_state, &$install_state) {
  $attributes = array(
    'autocomplete' => 'off',
    'autocorrect' => 'off',
    'autocapitalize' => 'off',
    'spellcheck' => 'false',
  );
  $form = install_settings_form($form, $form_state, $install_state);
  $form['settings']['mysql']['database']['#attributes'] = $attributes;
  $form['settings']['mysql']['username']['#attributes'] = $attributes;
  $form['settings']['mysql']['password']['#attributes'] = $attributes;
  $form['actions']['save']['#validate'][] = 'install_settings_form_validate';
  return $form;
}

/**
 * Create model and import Swagger/WADL file to load content.
 */
function apigee_smartdocs_import_model_content() {
  global $install_state;

  _apigee_manage_memory();

  if (!$install_state['parameters']['edge_configured']) {
    return NULL;
  }

  // Create sample SmartDocs  model.
  $model_display_name = 'Pet Store Example API';
  $model_description = 'Manage inventory and users through an example REST API patterned after the classic pet store demo.';
  $operations[] = array('apigee_batch_smartdocs_create_model', array(SMARTDOCS_SAMPLE_PETSTORE_MODEL, $model_display_name, $model_description));

  // Import pet store Swagger file into pet store model.
  $model_import_file = __DIR__ . '/modules/custom/devconnect/smartdocs/samples/petstore.swagger.json';
  $operations[] = array('apigee_batch_smartdocs_import_model', array(SMARTDOCS_SAMPLE_PETSTORE_MODEL, $model_import_file, 'swagger', 'application/json'));

  // Create sample SmartDocs  model.
  $model_display_name = 'Weather Example API';
  $model_description = 'Get weather reports for any location using the Yahoo Weather API.';
  $operations[] = array('apigee_batch_smartdocs_create_model', array(SMARTDOCS_SAMPLE_WEATHER_MODEL, $model_display_name, $model_description));

  // Import pet store Swagger file into pet store model.
  $model_import_file = __DIR__ . '/modules/custom/devconnect/smartdocs/samples/weather.xml';
  $operations[] = array('apigee_batch_smartdocs_import_model', array(SMARTDOCS_SAMPLE_WEATHER_MODEL, $model_import_file, 'wadl', 'application/xml'));

  $batch = array(
    'operations' => $operations,
    'title' => t('Creating and importing SmartDocs example models'),
    'init_message' => t('SmartDocs model creation has started...'),
    'file' => drupal_get_path('profile', 'apigee') . '/apigee.install_callbacks.inc',
  );

  return $batch;
}

/**
 * Renders and publishes pet store and weather SmartDocs nodes.
 *
 * @return mixed
 */
function apigee_smartdocs_render_model_content() {
  global $install_state;

  _apigee_manage_memory();

  if (!$install_state['parameters']['edge_configured']) {
    return NULL;
  }

  drupal_set_message('Rendering SmartDocs example documentation pages.', 'status');
  require_once drupal_get_path('module', 'smartdocs') . '/batch/smartdocs.render.inc';

  // Render weather model node.
  $model = new Apigee\SmartDocs\Model(devconnect_default_org_config());
  $model->load(SMARTDOCS_SAMPLE_WEATHER_MODEL);
  $revision = new Apigee\SmartDocs\Revision($model->getConfig(), $model->getUuid());
  $rev = max($model->getLatestRevisionNumber(), 1);
  $revision->load($rev);

  $selected = array();
  foreach ($revision->getResources() as $resource) {
    foreach ($resource->getMethods() as $method) {
      $selected[$method->getUuid()] = $method->getUuid();
    }
  }
  $batch_weather = smartdocs_render($model, $revision, $selected, array('publish' => 'publish'), TRUE);

  // Render pet store model nodes.
  $model = new Apigee\SmartDocs\Model(devconnect_default_org_config());
  $model->load(SMARTDOCS_SAMPLE_PETSTORE_MODEL);
  $revision = new Apigee\SmartDocs\Revision($model->getConfig(), $model->getUuid());
  $rev = max($model->getLatestRevisionNumber(), 1);
  $revision->load($rev);

  $selected = array();

  foreach ($revision->getResources() as $resource) {
    foreach ($resource->getMethods() as $method) {
      $selected[$method->getUuid()] = $method->getUuid();
    }
  }
  $batch_pet = smartdocs_render($model, $revision, $selected, array('publish' => 'publish'), TRUE);

  // Merge the pet and weather batch to return.
  $batch = array(
    'operations' => array_merge($batch_weather['operations'], $batch_pet['operations']),
    'title' => t('Rendering SmartDocs documentation pages'),
    'init_message' => t('SmartDocs rendering has started...'),
    'progress_message' => t('Processed @current out of @total.'),
    'error_message' => t('Rendering SmartDocs nodes has encountered an error.'),
    'file' => drupal_get_path('module', 'smartdocs') . '/batch/smartdocs.render.inc',
  );

  return $batch;
}

/**
 * Form constructor to create a Drupal Admin User.
 *
 * @param array $form
 *   The form being constructed.
 * @param array $form_state
 *   The state of the form being constructed.
 *
 * @return array
 *   The newly-constructed form.
 */
function apigee_install_create_admin_user($form, &$form_state) {
  $attributes = array(
    'autocomplete' => 'off',
    'autocorrect' => 'off',
    'autocapitalize' => 'off',
    'spellcheck' => 'false',
  );
  $form['firstname'] = array(
    '#type' => 'textfield',
    '#title' => t('Developer First Name'),
    '#required' => TRUE,
    '#default_value' => '',
    '#description' => t('The first name of the administrator.'),
    '#attributes' => $attributes,
  );
  $form['lastname'] = array(
    '#type' => 'textfield',
    '#title' => t('Developer Last Name'),
    '#required' => TRUE,
    '#default_value' => '',
    '#description' => t('The last name of the administrator.'),
    '#attributes' => $attributes,
  );
  $form['username'] = array(
    '#type' => 'textfield',
    '#title' => t('Developer Portal Username'),
    '#required' => TRUE,
    '#default_value' => '',
    '#description' => t('An admin username used when logging into the Developer Portal.'),
    '#attributes' => $attributes,
  );
  $form['pass'] = array(
    '#type' => 'password_confirm',
    '#title' => t('Developer Portal Password'),
    '#required' => TRUE,
    '#description' => t('An admin password used when logging into the Developer Portal.'),
    '#attributes' => $attributes,
    '#pre_render' => array('apigee_password_pre_render'),
  );
  $form['emailaddress'] = array(
    '#type' => 'textfield',
    '#title' => t('Developer Portal Email'),
    '#required' => TRUE,
    '#default_value' => '',
    '#description' => t('Email address to associate with this account.'),
    '#attributes' => $attributes,
  );
  $form['actions'] = array(
    '#weight' => 100,
    '#attributes' => array(
      'class' => array('container-inline'),
    ),
  );
  $form['actions']['save'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#attributes' => array(
      'style' => 'float:left;',
    ),
  );
  $form['actions']['skip'] = array(
    '#type' => 'submit',
    '#limit_validation_errors' => array(),
    '#value' => t('Skip this config'),
    '#submit' => array('apigee_skip_create_admin_user'),
    '#attributes' => array(
      'style' => 'float:left;',
    ),

  );
  $form['#submit'][] = 'apigee_install_create_admin_user_submit';
  return $form;
}

/**
 * Submit handler to skip the create admin user installation piece.
 *
 * @param array $form
 *   The form being submitted.
 * @param array $form_state
 *   State of the form being submitted.
 */
function apigee_skip_create_admin_user($form, &$form_state) {
  // Skips the config, nothing left to do.
  $GLOBALS['install_state']['completed_task'] = install_verify_completed_task();
}

/**
 * Validates create admin user form.
 *
 * @param string $form
 *   The form being validated.
 * @param string $form_state
 *   State of the form being validated.
 */
function apigee_install_create_admin_user_validate($form, &$form_state) {
  if ($form_state['values']['username'] == 'admin') {
    form_set_error('username', 'Please select a different username.');
  }
  if (!valid_email_address($form_state['values']['emailaddress'])) {
    form_set_error('emailaddress', 'Please select a valid email address.');
  }
  if (apigee_install_create_admin_user_is_sdn_match($form_state['values']['firstname'], $form_state['values']['lastname'])) {
    form_set_error('', 'This name cannot be used as an administrator account. Please contact Apigee support for more details.');
  }
}

/**
 * Specially Designated Nationals List (SDN) Validation check.
 *
 * @param string $first_name
 *   First name of user to be validated.
 * @param string $last_name
 *   Last name of user to be validated.
 *
 * @return bool
 *   TRUE if person is on the SDN List.
 */
function apigee_install_create_admin_user_is_sdn_match($first_name, $last_name) {
  // Do not validate unless this is a cloud installation.
  if (!array_key_exists('PANTHEON_ENVIRONMENT', $_SERVER)) {
    return FALSE;
  }

  $endpoint = 'https://api.usergrid.com/devportalbuild/ofac-sdn-validation/individuals';

  $url = $endpoint . "?ql=" . urlencode("firstName='" . $first_name . "' AND lastName='" . $last_name . "'");
  $ch = curl_init();

  curl_setopt($ch, CURLOPT_URL, $url);
  curl_setopt($ch, CURLOPT_RETURNTRANSFER, TRUE);
  // The number of seconds to wait while trying to connect. Use 0
  // to wait indefinitely.
  curl_setopt($ch, CURLOPT_CONNECTTIMEOUT, 10);
  // The maximum number of seconds to allow cURL functions to execute.
  curl_setopt($ch, CURLOPT_TIMEOUT, 10);
  $response_json = curl_exec($ch);
  $http_status = curl_getinfo($ch, CURLINFO_HTTP_CODE);
  $curl_errno = curl_errno($ch);
  curl_close($ch);
  $response = json_decode($response_json, TRUE);

  if (array_key_exists('count', $response)) {
    if ($response['count'] != 0) {
      return TRUE;
    }
  }
  else {
    // The system could not check the SDN list, let Dev Portal team know.
    $my_module = 'sdn_check_error';
    $my_mail_token = 'apigee_profile';
    $from = variable_get('system_mail', 'noreply@apigee.com');

    $http_response_string = '';
    foreach ($response as $response_key => $response_value) {
      $http_response_string .= "$response_key => $response_value ";
    }

    $pantheon_site_name = '';
    if (array_key_exists('PANTHEON_SITE_NAME', $_SERVER) && array_key_exists('PANTHEON_ENVIRONMENT', $_SERVER)) {
      $pantheon_site_name = $_SERVER['PANTHEON_SITE_NAME'] . '.' . $_SERVER['PANTHEON_ENVIRONMENT'];
    }
    $pantheon_site_uuid = '';
    if (array_key_exists('PANTHEON_SITE', $_SERVER)) {
      $pantheon_site_uuid = $_SERVER['PANTHEON_SITE'];
    }

    $message_body = array(
      'The Specially Designated Nationals List (SDN) Validation check failed during a Dev Portal Pantheon site install.',
      'URL: ' . $url,
      'cURL Error Number: ' . $curl_errno,
      'HTTP Status: ' . $http_status,
      'HTTP Response: ' . $http_response_string,
      'Pantheon Site Name: ' . $pantheon_site_name,
      'Pantheon Site UUID: ' . $pantheon_site_uuid,
    );

    $message = array(
      'id' => $my_module . '_' . $my_mail_token,
      'to' => 'devportalbuild@apigee.com',
      'subject' => 'Dev Portal Specially Designated Nationals List (SDN) Validation check failure',
      'body' => $message_body,
      'headers' => array(
        'From' => $from,
        'Sender' => $from,
        'Return-Path' => $from,
      ),
    );
    $system = drupal_mail_system($my_module, $my_mail_token);

    // The format function must be called before calling the mail function.
    $message = $system->format($message);

    if (!$system->mail($message)) {
      watchdog('apigee_install', "SDN Validation error email NOT sent." . implode(" ", $message_body), WATCHDOG_WARNING);
    }
  }
  return FALSE;
}


/**
 * Submit handler for create admin user form.
 *
 * @param array $form
 *   The form being submitted.
 * @param array $form_state
 *   State of the form being submitted.
 */
function apigee_install_create_admin_user_submit($form, &$form_state) {
  require_once DRUPAL_ROOT . '/' . variable_get('password_inc', 'includes/password.inc');
  $account = new stdClass();
  $account->is_new = TRUE;
  $account->status = TRUE;
  $account->name = $form_state['values']['username'];
  $account->pass = user_hash_password($form_state['values']['pass']);
  $account->mail = $form_state['values']['emailaddress'];
  $account->init = $form_state['values']['emailaddress'];
  $role = user_role_load_by_name('administrator');
  $rid = $role->rid;
  $account->roles[$rid] = 'administrator';
  $account->field_first_name[LANGUAGE_NONE][0]['value'] = $form_state['values']['firstname'];
  $account->field_last_name[LANGUAGE_NONE][0]['value'] = $form_state['values']['lastname'];
  user_save($account);
  $GLOBALS['install_state']['completed_task'] = install_verify_completed_task();
}

/**
 * Batch process callback to create the environmental indicators.
 *
 * @param array $context
 *   Current state of installer.
 */
function apigee_install_create_environmental_indicators(array &$context) {
  if (array_key_exists('PANTHEON_ENVIRONMENT', $_SERVER)) {
    $environment_dev = (object) array(
      'disabled' => FALSE,
      'api_version' => 1,
      'machine' => 'development_environment',
      'name' => 'Development Environment',
      'regexurl' => 'dev-*',
      'settings' => array(
        'color' => '#fd7272',
        'text_color' => '#ffffff',
        'weight' => '',
        'position' => 'top',
        'fixed' => 0,
      ),
    );
    ctools_export_crud_save('environment_indicator_environment', $environment_dev);
    $environment_test = (object) array(
      'disabled' => FALSE,
      'api_version' => 1,
      'machine' => 'testing_environment',
      'name' => 'Testing Environment',
      'regexurl' => 'test-*',
      'settings' => array(
        'color' => '#fbf479',
        'text_color' => '#ffffff',
        'weight' => '',
        'position' => 'top',
        'fixed' => 0,
      ),
    );
    ctools_export_crud_save('environment_indicator_environment', $environment_test);
    $context['message'] = st('Created environmental indicators');
  }
}

/**
 * Boosts PHP's memory and execution time for large-capacity batch processes.
 */
function _apigee_manage_memory() {
  ini_set('memory_limit', '1024M');
  ini_set('max_execution_time', 300);
}
