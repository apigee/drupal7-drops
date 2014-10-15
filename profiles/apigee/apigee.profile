<?php

require_once(dirname(__FILE__) . '/libraries/mgmt-api-php-sdk/Apigee/Util/Crypto.php');
// Make Crypto work properly with R23
if (method_exists('Apigee\Util\Crypto', 'setKey')) {
  Apigee\Util\Crypto::setKey(hash('SHA256', 'w3-Love_ap|s', TRUE));
}

/**
 * Selects the Apigee Profile
 *
 * @param $install_state
 */
function apigee_install_select_profile(&$install_state) {
  $install_state['parameters']['profile'] = 'apigee';
}

/**
 * Ensure the locale is set to English
 *
 * @param $install_state
 */
function apigee_install_select_locale(&$install_state) {
  $install_state['parameters']['locale'] = 'en';
}

/**
 * Task handler to load our install profile and enhance the dependency information
 *
 * @param $install_state
 */
function apigee_install_load_profile(&$install_state) {
  // Loading the install profile normally
  install_load_profile($install_state);
  if (array_key_exists('PANTHEON_ENVIRONMENT', $_SERVER)) {
    $install_state['profile_info']['dependencies'][] = "pantheon_api";
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
 * Create batch items for apigee install
 *
 * @param $install_state
 * @return array
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
      array('apigee_install_configure_autologout', array()),
    ),
    'finished' => '_apigee_install_configure_task_finished',
  );
}

/**
 * Ensures the given task is completed, if not, skip
 *
 * @param $success
 * @param $results
 * @param $operations
 */
function _apigee_install_configure_task_finished($success, $results, $operations) {
  watchdog(__FUNCTION__, 'Configure Task Finished', array(), WATCHDOG_INFO);

  $GLOBALS['install_state']['batch_configure_complete'] = install_verify_completed_task();
}

/**
 * Configure variables across the environment properly
 *
 * @param $context
 */
function apigee_install_configure_variables(&$context) {
  watchdog(__FUNCTION__, 'Config Vars', array(), WATCHDOG_INFO);
  if (array_key_exists('PRESSFLOW_SETTINGS', $_SERVER)) {
    $pressflow = json_decode($_SERVER['PRESSFLOW_SETTINGS'], TRUE);
    $conf = $pressflow['conf'];
  }
  else {
    $conf = array(
      'file_public_path' => 'sites/default/files',
      'file_private_path' => 'sites/default/private',
      'file_temporary_path' => 'sites/default/tmp'
    );
  }
  try {
    file_prepare_directory($conf['file_public_path'], FILE_CREATE_DIRECTORY);
    file_prepare_directory($conf['file_temporary_path'], FILE_CREATE_DIRECTORY);
    file_prepare_directory($conf['file_private_path'], FILE_CREATE_DIRECTORY);
  } catch (Exception $e) {
    drupal_set_message(t('unable to create the directories necessary for Drupal to write files: :error', array(
      ':error' => $e->getMessage()
    )));
  }
  $crypt_key = drupal_random_bytes(64);
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
  variable_set('date_default_timezone', 'America/Los_Angeles'); // Designed by Apigee in California
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
  variable_set('logintoboggan_login_with_email', 1); // Set use email for login flag
  variable_set('logintoboggan_immediate_login_on_register', 0); // Set immediate login to false by default
  variable_set('logintoboggan_override_destination_parameter', 0); // Set immediate login to false by default
  variable_set('apigee_crypt_key', $crypt_key);
  $context['results'][] = 'variables';
  $context['message'] = st('Default variables set.');
}

/**
 * Pushes apachesolr xml to Pantheon server
 *
 * @param $context
 */
function apigee_install_pantheon_push_solr(&$context) {
  if (array_key_exists('PANTHEON_ENVIRONMENT', $_SERVER) && module_exists('pantheon_apachesolr')) {
    watchdog(__FUNCTION__, 'Pushing Solr', array(), WATCHDOG_INFO);
    module_load_include('module', 'pantheon_apachesolr');
    pantheon_apachesolr_post_schema_exec('profiles/apigee/modules/contrib/apachesolr/solr-conf/solr-3.x/schema.xml');
    $context['results'][] = 'solr_push';
    $context['message'] = st('Solr config pushed to pantheon solr server.');
  }
  else {
    watchdog(__FUNCTION__, 'Solr not enabled.', array(), WATCHDOG_NOTICE);
    $context['results'][] = 'solr_push';
    $context['message'] = st('Solr is not enabled, no need to push to Solr server.');
  }
}

/**
 * Solr config batch item
 *
 * @param $context
 */
function apigee_install_configure_solr(&$context) {
  watchdog(__FUNCTION__, 'Configuring Solr', array(), WATCHDOG_INFO);
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
 * Function that configures standard user functionality across the platform
 *
 * @param $context
 */
function apigee_install_configure_users(&$context) {
  watchdog(__FUNCTION__, 'Configuring Default Users', array(), WATCHDOG_INFO);
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
  //Anonymous user permissions
  $permissions = array('access comments', 'access content', 'view faq page');
  user_role_grant_permissions(1, $permissions);
  //Authenticated user permissions
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
 * Configures default themes batch item
 *
 * @param $context
 */
function apigee_install_configure_themes(&$context) {
  watchdog(__FUNCTION__, 'Configuring themes', array(), WATCHDOG_INFO);
  // activate admin theme when editing a node
  variable_set('node_admin_theme', '1');
  db_update('system')
    ->fields(array('status' => 0))
    ->condition('type', 'theme')
    ->execute();
  $enable = array(
    'theme_default' => 'apigee_responsive',
    'admin_theme' => 'rubik',
    'apigee_base'
  );
  try {
    theme_enable($enable);
    foreach ($enable as $var => $theme) {
      if (!is_numeric($var)) {
        variable_set($var, $theme);
      }
    }
    db_update('block')->fields(array('status' => 0, 'region' => -1))->condition('delta', 'main', '!=')->execute();
  } catch (Exception $e) {
    watchdog_exception(__FUNCTION__, $e, 'ERROR CONFIGURING THEMES %message', array('%message' => $e->getMessage()), WATCHDOG_ERROR);
  }
  $context['results'][] = 'themes';
  $context['message'] = st('Default Apigee theme configured.');
}

/**
 * Installs the various content types that the profile will use.
 *
 * @param $context
 */
function apigee_install_content_types(&$context) {
  watchdog(__FUNCTION__, 'Creating default content types', array(), WATCHDOG_INFO);
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
 * Enables the devconnect_blog_content_types module
 */
function apigee_install_enable_blog_content_types() {
  //Needs to be done here not in the .info file
  module_enable(array('devconnect_blog_content_types'));
  $context['results'][] = 'blog_content_types';
  $context['message'] = st('Enabling DevConnect Blog Content Types!');
}

/**
 * Installs the default homepage
 */
function apigee_install_create_homepage() {
  watchdog(__FUNCTION__, 'Generating Homepage', array(), WATCHDOG_INFO);
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
  } catch (Exception $e) {
    watchdog_exception(__FUNCTION__, $e, 'Error generating home page: %message', array('%message' => $e->getMessage()), WATCHDOG_ERROR);
    $context['results'][] = 'homepage_created';
    $context['message'] = st('No need to generate default homepage...');
  }
}

/**
 * Creates the CKEditor settings for the portal
 *
 * @param $context
 */
function apigee_install_base_ckeditor_settings(&$context) {
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
        ),
      )
    );
    db_insert('role_permission')->fields(array(
      'rid' => 2,
      'permission' => 'use text format filtered_html',
      'module' => 'filter'
    ))->execute();
    db_insert('role_permission')->fields(array(
      'rid' => 3,
      'permission' => 'use text format filtered_html',
      'module' => 'filter'
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
      'module' => 'filter'
    ))->execute();
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
    'loadPlugins' => array(
      'a11yhelp' => array(
        'name' => 'a11yhelp',
        'desc' => 'Plugin file: a11yhelp',
        'path' => '%plugin_dir_extra%a11yhelp/',
        'buttons' => '',
        'default' => 'f',
      ),
      'about' => array(
        'name' => 'about',
        'desc' => 'Plugin file: about',
        'path' => '%plugin_dir_extra%about/',
        'buttons' => '',
        'default' => 'f',
      ),
      'basicstyles' => array(
        'name' => 'basicstyles',
        'desc' => 'Plugin file: basicstyles',
        'path' => '%plugin_dir_extra%basicstyles/',
        'buttons' => '',
        'default' => 'f',
      ),
      'blockquote' => array(
        'name' => 'blockquote',
        'desc' => 'Plugin file: blockquote',
        'path' => '%plugin_dir_extra%blockquote/',
        'buttons' => '',
        'default' => 'f',
      ),
      'button' => array(
        'name' => 'button',
        'desc' => 'Plugin file: button',
        'path' => '%plugin_dir_extra%button/',
        'buttons' => '',
        'default' => 'f',
      ),
      'ckeditor_link' => array(
        'name' => 'drupal_path',
        'desc' => 'CKEditor Link - A plugin to easily create links to Drupal internal paths',
        'path' => '%base_path%profiles/apigee/modules/contrib/ckeditor_link/plugins/link/',
        'buttons' => '',
      ),
      'clipboard' => array(
        'name' => 'clipboard',
        'desc' => 'Plugin file: clipboard',
        'path' => '%plugin_dir_extra%clipboard/',
        'buttons' => '',
        'default' => 'f',
      ),
      'contextmenu' => array(
        'name' => 'contextmenu',
        'desc' => 'Plugin file: contextmenu',
        'path' => '%plugin_dir_extra%contextmenu/',
        'buttons' => '',
        'default' => 'f',
      ),
      'counter' => array(
        'name' => 'contextmenu',
        'desc' => 'Plugin file: counter',
        'path' => '%plugin_dir_extra%counter/',
        'buttons' => '',
        'default' => 'f',
      ),
      'dialog' => array(
        'name' => 'dialog',
        'desc' => 'Plugin file: dialog',
        'path' => '%plugin_dir_extra%dialog/',
        'buttons' => '',
        'default' => 'f',
      ),
      'dialogui' => array(
        'name' => 'dialog',
        'desc' => 'Plugin file: dialogui',
        'path' => '%plugin_dir_extra%dialogui/',
        'buttons' => '',
        'default' => 'f',
      ),
      'drupalbreaks' => array(
        'name' => 'drupalbreaks',
        'desc' => 'Plugin for inserting Drupal teaser and page breaks.',
        'path' => '%plugin_dir%drupalbreaks/',
        'buttons' => array(
          'DrupalBreak' => array(
            'label' => 'DrupalBreak',
            'icon' => 'images/drupalbreak.png',
          ),
        ),
        'default' => 't',
      ),
      'elementspath' => array(
        'name' => 'elementspath',
        'desc' => 'Plugin file: elementspath',
        'path' => '%plugin_dir_extra%elementspath/',
        'buttons' => '',
        'default' => 'f',
      ),
      'enterkey' => array(
        'name' => 'enterkey',
        'desc' => 'Plugin file: enterkey',
        'path' => '%plugin_dir_extra%enterkey/',
        'buttons' => '',
        'default' => 'f',
      ),
      'entities' => array(
        'name' => 'entities',
        'desc' => 'Plugin file: entities',
        'path' => '%plugin_dir_extra%entities/',
        'buttons' => '',
        'default' => 'f',
      ),
      'fakeobjects' => array(
        'name' => 'fakeobjects',
        'desc' => 'Plugin file: fakeobjects',
        'path' => '%plugin_dir_extra%fakeobjects/',
        'buttons' => '',
        'default' => 'f',
      ),
      'filebrowser' => array(
        'name' => 'filebrowser',
        'desc' => 'Plugin file: filebrowser',
        'path' => '%plugin_dir_extra%filebrowser/',
        'buttons' => '',
        'default' => 'f',
      ),
      'floatingspace' => array(
        'name' => 'floatingspace',
        'desc' => 'Plugin file: floatingspace',
        'path' => '%plugin_dir_extra%floatingspace/',
        'buttons' => '',
        'default' => 'f',
      ),
      'floatpanel' => array(
        'name' => 'floatpanel',
        'desc' => 'Plugin file: floatpanel',
        'path' => '%plugin_dir_extra%floatpanel/',
        'buttons' => '',
        'default' => 'f',
      ),
      'horizontalrule' => array(
        'name' => 'horizontalrule',
        'desc' => 'Plugin file: horizontalrule',
        'path' => '%plugin_dir_extra%horizontalrule/',
        'buttons' => '',
        'default' => 'f',
      ),
      'htmlwriter' => array(
        'name' => 'htmlwriter',
        'desc' => 'Plugin file: htmlwriter',
        'path' => '%plugin_dir_extra%htmlwriter/',
        'buttons' => '',
        'default' => 'f',
      ),
      'iframe' => array(
        'name' => 'iframe',
        'desc' => 'Plugin file: iframe',
        'path' => '%plugin_dir_extra%iframe/',
        'buttons' => '',
        'default' => 'f',
      ),
      'image' => array(
        'name' => 'image',
        'desc' => 'Plugin file: image',
        'path' => '%plugin_dir_extra%image/',
        'buttons' => '',
        'default' => 'f',
      ),
      'insertpre' => array(
        'name' => 'insertpre',
        'desc' => 'Plugin file: insertpre',
        'path' => '%plugin_dir_extra%insertpre/',
        'buttons' => '',
        'default' => 'f'
      ),
      'indent' => array(
        'name' => 'indent',
        'desc' => 'Plugin file: indent',
        'path' => '%plugin_dir_extra%indent/',
        'buttons' => '',
        'default' => 'f',
      ),
      'indentlist' => array(
        'name' => 'indentlist',
        'desc' => 'Plugin file: indentlist',
        'path' => '%plugin_dir_extra%indentlist/',
        'buttons' => '',
        'default' => 'f',
      ),
      'lineutils' => array(
        'name' => 'lineutils',
        'desc' => 'Plugin file: lineutils',
        'path' => '%plugin_dir_extra%lineutils/',
        'buttons' => '',
        'default' => 'f',
      ),
      'list' => array(
        'name' => 'list',
        'desc' => 'Plugin file: list',
        'path' => '%plugin_dir_extra%list/',
        'buttons' => '',
        'default' => 'f',
      ),
      'magicline' => array(
        'name' => 'magicline',
        'desc' => 'Plugin file: magicline',
        'path' => '%plugin_dir_extra%magicline/',
        'buttons' => '',
        'default' => 'f',
      ),
      'media' => array(
        'name' => 'media',
        'desc' => 'Plugin for inserting images from Drupal media module',
        'path' => '%plugin_dir%media/',
        'buttons' => array(
          'Media' => array(
            'label' => 'Media',
            'icon' => 'images/icon.gif',
          ),
        ),
        'default' => 'f',
      ),
      'mediaembed' => array(
        'name' => 'mediaembed',
        'desc' => 'Plugin for inserting Drupal embeded media',
        'path' => '%plugin_dir%mediaembed/',
        'buttons' => array(
          'MediaEmbed' => array(
            'label' => 'MediaEmbed',
            'icon' => 'images/icon.png',
          )
        ),
        'default' => 'f',
      ),
      'menu' => array(
        'name' => 'menu',
        'desc' => 'Plugin file: menu',
        'path' => '%plugin_dir_extra%menu/',
        'buttons' => '',
        'default' => 'f',
      ),
      'menubutton' => array(
        'name' => 'menubutton',
        'desc' => 'Plugin file: menubutton',
        'path' => '%plugin_dir_extra%menubutton/',
        'buttons' => '',
        'default' => 'f',
      ),
      'panel' => array(
        'name' => 'panel',
        'desc' => 'Plugin file: panel',
        'path' => '%plugin_dir_extra%panel/',
        'buttons' => '',
        'default' => 'f',
      ),
      'pastefromword' => array(
        'name' => 'pastefromword',
        'desc' => 'Plugin file: pastefromword',
        'path' => '%plugin_dir_extra%pastefromword/',
        'buttons' => '',
        'default' => 'f',
      ),
      'pastetext' => array(
        'name' => 'pastetext',
        'desc' => 'Plugin file: pastetext',
        'path' => '%plugin_dir_extra%pastetext/',
        'buttons' => '',
        'default' => 'f',
      ),
      'popup' => array(
        'name' => 'popup',
        'desc' => 'Plugin file: popup',
        'path' => '%plugin_dir_extra%popup/',
        'buttons' => '',
        'default' => 'f',
      ),
      'removeformat' => array(
        'name' => 'removeformat',
        'desc' => 'Plugin file: removeformat',
        'path' => '%plugin_dir_extra%removeformat/',
        'buttons' => '',
        'default' => 'f',
      ),
      'richcombo' => array(
        'name' => 'richcombo',
        'desc' => 'Plugin file: richcombo',
        'path' => '%plugin_dir_extra%richcombo/',
        'buttons' => '',
        'default' => 'f',
      ),
      'scayt' => array(
        'name' => 'scayt',
        'desc' => 'Plugin file: scayt',
        'path' => '%plugin_dir_extra%scayt/',
        'buttons' => '',
        'default' => 'f',
      ),
      'sharedspace' => array(
        'name' => 'sharedspace',
        'desc' => 'Plugin file: sharedspace',
        'path' => '%plugin_dir_extra%sharedspace/',
        'buttons' => '',
        'default' => 'f',
      ),
      'sourcearea' => array(
        'name' => 'sourcearea',
        'desc' => 'Plugin file: sourcearea',
        'path' => '%plugin_dir_extra%sourcearea/',
        'buttons' => '',
        'default' => 'f',
      ),
      'sourcedialog' => array(
        'name' => 'sourcedialog',
        'desc' => 'Plugin file: sourcedialog',
        'path' => '%plugin_dir_extra%sourcedialog/',
        'buttons' => '',
        'default' => 'f',
      ),
      'specialchar' => array(
        'name' => 'specialchar',
        'desc' => 'Plugin file: specialchar',
        'path' => '%plugin_dir_extra%specialchar/',
        'buttons' => '',
        'default' => 'f',
      ),
      'stylescombo' => array(
        'name' => 'stylescombo',
        'desc' => 'Plugin file: stylescombo',
        'path' => '%plugin_dir_extra%stylescombo/',
        'buttons' => '',
        'default' => 'f',
      ),
      'tab' => array(
        'name' => 'tab',
        'desc' => 'Plugin file: tab',
        'path' => '%plugin_dir_extra%tab/',
        'buttons' => '',
        'default' => 'f',
      ),
      'tableresize' => array(
        'name' => 'tableresize',
        'desc' => 'Plugin file: tableresize',
        'path' => '%plugin_dir_extra%tableresize/',
        'buttons' => '',
        'default' => 't',
      ),
      'toolbarswitch' => array(
        'name' => 'toolbarswitch',
        'desc' => 'Plugin file: toolbarswitch',
        'path' => '%plugin_dir_extra%toolbarswitch/',
        'buttons' => '',
        'default' => 't',
      ),
      'widget' => array(
        'name' => 'widget',
        'desc' => 'Plugin file: widget',
        'path' => '%plugin_dir_extra%widget/',
        'buttons' => '',
        'default' => 't',
      ),
      'wysiwygarea' => array(
        'name' => 'wysiwygarea',
        'desc' => 'Plugin file: wysiwygarea',
        'path' => '%plugin_dir_extra%wysiwygarea/',
        'buttons' => '',
        'default' => 't',
      ),
      'trifold' => array(
        'name' => 'trifold',
        'desc' => 'Plugin file: trifold',
        'path' => '%base_path%profiles/apigee/modules/contrib/ckeditor_bootstrap/plugins/trifold/',
        'buttons' => '',
        'default' => 't',
      ),
      'featurette' => array(
        'name' => 'featurette',
        'desc' => 'Plugin file: featurette',
        'path' => '%base_path%profiles/apigee/modules/contrib/ckeditor_bootstrap/plugins/featurette/',
        'buttons' => '',
        'default' => 't',
      ),
      'jumbotron' => array(
        'name' => 'jumbotron',
        'desc' => 'Plugin file: jumbotron',
        'path' => '%base_path%profiles/apigee/modules/contrib/ckeditor_bootstrap/plugins/jumbotron/',
        'buttons' => '',
        'default' => 't',
      ),
      'carousel' => array(
        'name' => 'carousel',
        'desc' => 'Plugin file: carousel',
        'path' => '%base_path%profiles/apigee/modules/contrib/ckeditor_bootstrap/plugins/carousel/',
        'buttons' => '',
        'default' => 't',
      ),
    ),
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
    ->condition('name', 'Full')
    ->execute();
  db_delete('ckeditor_settings')
    ->condition('name', 'Advanced')
    ->execute();
  db_delete('ckeditor_settings')
    ->condition('name', 'CKEditor Global Profile')
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
 * Creates dummy taxonomy terms
 *
 * @param $context
 */
function apigee_install_create_taxonomy_terms(&$context) {
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
 * Creates example tutorial content
 *
 * @param $context
 */
function apigee_install_create_tutorial_content(&$context) {
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
 * Creates default content for the install
 *
 * @param $context
 */
function apigee_install_create_forum_content(&$context) {
  // 10 forum posts
  for ($i = 0; $i <= 7; $i++) {
    $body = array();
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
 * Creates default content for the install
 *
 * @param $context
 */
function apigee_install_create_page_content(&$context) {
  // 5 pages
  for ($i = 0; $i <= 5; $i++) {
    $body = array();
    $body['post'] = _apigee_install_generate_greek(mt_rand(2, 300), TRUE);
    _apigee_install_generate_node('page', $body);
  }
  $context['results'][] = 'content_created';
  $context['message'] = st('5 Example Pages Created!');
}

/**
 * Creates default content for the install
 *
 * @param $context
 */
function apigee_install_create_audio_content(&$context) {
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
          'format' => 'full_html'
        )
      )
    ),
    'field_audio' => array(
      LANGUAGE_NONE => array(
        array(
          'fid' => $fid,
          'display' => 1,
        )
      )
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
 * Creates default content for the install
 *
 * @param $context
 */
function apigee_install_create_video_content(&$context) {
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
          'format' => 'full_html'
        )
      )
    ),
    'field_content_tag' => array(LANGUAGE_NONE => array(array('tid' => $blog_tid))),
    'field_video' => array(
      LANGUAGE_NONE => array(
        array(
          'fid' => $fid,
          'display' => 1,
          'description' => ''
        )
      )
    ),
  );
  node_save($node);
  _apigee_content_types_set_file_usage($fid, $node->nid);
  // Now create path alias
  if (!module_exists('pathauto')) {
    $path = array('source' => 'node/' . $node->nid, 'alias' => 'blog/your-api-sucks');
    path_save($path);
  }
  $context['results'][] = 'content_created';
  $context['message'] = st('Video Content Created!');
}

/**
 * Creates default content for the install
 *
 * @param $context
 */
function apigee_install_create_faq_content(&$context) {
  $type = 'faq';
  for ($i = 0; $i <= 3; $i++) {
    _apigee_install_generate_node($type, $body = NULL, $fields = NULL);
  }
  $context['results'][] = 'content_created';
  $context['message'] = st('FAQ Example Content Created!');
}

/**
 * Helper function that sets the file usage
 *
 * @param $fid
 * @param $nid
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
        'count' => 1
      ))
      ->execute();
  }
}

/**
 * Helper function to find (or create) an entry in the file_managed table.
 *
 * @param string $uri
 * @param string $filename
 * @param string $filemime
 * @param string $type
 * @return integer
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
      'title' => ''
    );
    $file = file_save($file);
    $fid = $file->fid;
  }
  return $fid;
}

/**
 * Gets the tid for a Given Blog
 *
 * @param null $blog_tid
 * @return null
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
 * Function that generates nodes
 *
 * @param $type
 * @param null $body
 * @param null $fields
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
 * Generates filler content for generated nodes
 * Mimics Devel's devel_creating_greeking, but makes this profile not rely on it
 *
 * @param $word_count
 * @param bool $title
 * @return string
 */
function _apigee_install_generate_greek($word_count, $title = FALSE) {
  static $greek_flipped = NULL;
  if (!isset($greek_flipped)) {
    $greek = file(dirname(__FILE__) . '/greek.txt', FILE_IGNORE_NEW_LINES | FILE_SKIP_EMPTY_LINES);
    $greek_flipped = array_flip($greek);
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
 * Rebuilds permissions
 *
 * @param $context
 */
function apigee_install_rebuild_permissions(&$context) {
  watchdog(__FUNCTION__, 'rebuilding permissions', array(), WATCHDOG_INFO);
  try {
    node_access_rebuild(TRUE);
  } catch (Exception $e) {
    watchdog_exception(__FUNCTION__, $e, 'Error rebuilding node access: %message', array('%message' => $e->getMessage()), WATCHDOG_ERROR);
  }
  $context['results'][] = 'content_permissions';
  $context['message'] = st('Content Permissions Rebuilt');
}

/**
 * Flushes caches for the JS/CSS cache
 *
 * @param $context
 */
function apigee_install_clear_caches_flush(&$context) {
  watchdog(__FUNCTION__, 'Flushing CSS/JS', array(), WATCHDOG_INFO);
  _drupal_flush_css_js();
  $context['results'][] = 'cache_flush';
  $context['message'] = st('CSS & JS flushed');
}

/**
 * Rebuilds the registry for the site
 *
 * @param $context
 */
function apigee_install_rebuild_registry(&$context) {
  watchdog(__FUNCTION__, 'Rebuilding Registry', array(), WATCHDOG_INFO);
  registry_rebuild();
  $context['results'][] = 'cache_registry';
  $context['message'] = st('Registry Rebuilt');
}

/**
 * Clears the CSS Cache
 *
 * @param $context
 */
function apigee_install_clear_caches_css(&$context) {
  watchdog(__FUNCTION__, 'Clearing CSS Cache', array(), WATCHDOG_INFO);
  drupal_clear_css_cache();
  $context['results'][] = 'cache_css';
  $context['message'] = st('CSS Caches Cleared');
}

/**
 * Clears the JS Cache
 *
 * @param $context
 */
function apigee_install_clear_caches_js(&$context) {
  watchdog(__FUNCTION__, 'Clearing JS Cache', array(), WATCHDOG_INFO);
  drupal_clear_js_cache();
  $context['results'][] = 'cache_js';
  $context['message'] = st('JS Caches Cleared');
}

/**
 * Clears the theme cache
 *
 * @param $context
 */
function apigee_install_clear_caches_theme(&$context) {
  watchdog(__FUNCTION__, 'Rebuilding themes...', array(), WATCHDOG_INFO);
  system_rebuild_theme_data();
  drupal_theme_rebuild();
  $context['results'][] = 'cache_theme';
  $context['message'] = st('Theme Caches Cleared');
}

/**
 * Clears the entity cache
 *
 * @param $context
 */
function apigee_install_clear_caches_entity(&$context) {
  watchdog(__FUNCTION__, 'Clearing Entity Cache...', array(), WATCHDOG_INFO);
  entity_info_cache_clear();
  $context['results'][] = 'cache_entity';
  $context['message'] = st('Entity Caches Cleared');
}

/**
 * Rebuilds the node types cache
 *
 * @param $context
 */
function apigee_install_clear_caches_nodes(&$context) {
  watchdog(__FUNCTION__, 'Rebuilding Node Types...', array(), WATCHDOG_INFO);
  node_types_rebuild();
  $context['results'][] = 'cache_node';
  $context['message'] = st('Node Caches Cleared');
}

/**
 * Rebuilds the menu
 *
 * @param $context
 */
function apigee_install_clear_caches_menu(&$context) {
  watchdog(__FUNCTION__, 'Rebuilding Menu...', array(), WATCHDOG_INFO);
  menu_rebuild();
  $context['results'][] = 'cache_menu';
  $context['message'] = st('Menu Caches Cleared');
}

/**
 * Synchronizes Actions
 *
 * @param $context
 */
function apigee_install_clear_caches_actions(&$context) {
  watchdog(__FUNCTION__, 'Synchronizing Actions...', array(), WATCHDOG_INFO);
  actions_synchronize();
  $context['results'][] = 'cache_action';
  $context['message'] = st('Action Caches Cleared');
}

/**
 * Worker to flush a supplied cache
 *
 * @param $table
 * @param $results_label
 * @param $message_label
 * @param $context
 */
function _apigee_install_clear_cache($table, $results_label, $message_label, &$context) {
  static $cache_tables;
  if (!isset($cache_tables)) {
    $cache_tables = module_invoke_all('flush_caches');
  }
  watchdog(__FUNCTION__, "Flushing $results_label caches...", array(), WATCHDOG_INFO);
  $my_cache_tables = array_merge($cache_tables, array($table));
  foreach ($my_cache_tables as $my_table) {
    cache_clear_all('*', $my_table, TRUE);
  }
  $context['results'][] = $results_label;
  $context['message'] = st("$message_label caches cleared");
}

/**
 * Clears core cache
 *
 * @param $context
 */
function apigee_install_clear_caches_core(&$context) {
  _apigee_install_clear_cache('cache', 'cache_core', 'Core', $context);
}

/**
 * Clears core path cache
 *
 * @param $context
 */
function apigee_install_clear_caches_core_path(&$context) {
  _apigee_install_clear_cache('cache_path', 'cache_path', 'Path', $context);
}

/**
 * Clears cache filter
 *
 * @param $context
 */
function apigee_install_clear_caches_core_filter(&$context) {
  _apigee_install_clear_cache('cache_filter', 'cache_filter', 'Filter', $context);
}

/**
 * Clears bootstrap cache
 *
 * @param $context
 */
function apigee_install_clear_caches_core_bootstrap(&$context) {
  _apigee_install_clear_cache('cache_bootstrap', 'cache_bootstrap', 'Bootstrap', $context);
}

/**
 * Clears cache page
 *
 * @param $context
 */
function apigee_install_clear_caches_core_page(&$context) {
  _apigee_install_clear_cache('cache_page', 'cache_page', 'Page', $context);
}

/**
 * Updates the bootstrap Status
 *
 * @param $context
 */
function apigee_install_bootstrap_status(&$context) {
  watchdog(__FUNCTION__, 'Updating bootstrap status...', array(), WATCHDOG_INFO);
  _system_update_bootstrap_status();
  drupal_get_messages();
  $context['results'][] = 'bootstrap_status';
  $context['message'] = st('Bootstrap Status Reset.');
}

/**
 * Set the apigee endpoint configuration vars
 *
 * @param $form
 * @param $form_state
 * @return array
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
    'spellcheck' => 'false'
  );
  $form = array();
  $form['org'] = array(
    '#type' => 'textfield',
    '#title' => t('Management API Organization'),
    '#required' => TRUE,
    '#default_value' => $org,
    '#description' => t('The v4 product organization name. Changing this value could make your site not work.'),
    '#attributes' => $attributes
  );
  $form['endpoint'] = array(
    '#type' => 'textfield',
    '#title' => t('Management API Endpoint URL'),
    '#required' => TRUE,
    '#default_value' => $endpoint,
    '#description' => t('URL to which to make Edge REST calls. For on-prem installs you will need to change this value.'),
    '#attributes' => $attributes
  );
  $form['user'] = array(
    '#type' => 'textfield',
    '#title' => t('Endpoint Authenticated User'),
    '#required' => TRUE,
    '#default_value' => '',
    '#description' => t('User name used when authenticating with the endpoint. Generally this takes the form of an email address.'),
    '#attributes' => $attributes + array('placeholder' => 'username')
  );
  $form['pass'] = array(
    '#type' => 'textfield',
    '#title' => t('Authenticated User’s Password'),
    '#required' => TRUE,
    '#default_value' => '',
    '#description' => t('Password used when authenticating with the endpoint.'),
    '#attributes' => $attributes,
    '#post_render' => array('apigee_password_post_render')
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
 * Validates if the connection is successful
 *
 * @param $form
 * @param $form_state
 */
function apigee_install_api_endpoint_validate($form, &$form_state) {
  $org = $form_state['values']['org'];
  $endpoint = $form_state['values']['endpoint'];
  $user = $form_state['values']['user'];
  $pass = $form_state['values']['pass'];
  module_load_include('inc', 'devconnect', 'devconnect.admin');
  $return = _devconnect_test_kms_connection($org, $endpoint, $user, $pass);
  if (strpos($return, t('Connection Successful')) === FALSE) { //If connection is not successful
    form_set_error('form', $return);
  }
}

/**
 * Turns a text field into a password field.
 *
 * @param $content
 * @param $element
 * @return mixed
 */
function apigee_password_post_render($content, $element) {
  return str_replace('type="text"', 'type="password"', $content);
}

/**
 * Custom function that skips the devconnect installation piece
 *
 * @param $form
 * @param $form_state
 */
function apigee_skip_api_endpoint($form, &$form_state) {
  $GLOBALS['apigee_api_endpoint_configured'] = FALSE;
  $GLOBALS['install_state']['completed_task'] = install_verify_completed_task();
}

/**
 * Installs the endpoint credentials for the management server
 *
 * @param $form
 * @param $form_state
 */
function apigee_install_api_endpoint_submit($form, &$form_state) {
  drupal_load('module', 'devconnect');
  $config = devconnect_get_org_settings();
  foreach (array('org', 'endpoint', 'user', 'pass') as $key) {
    $value = $form_state['values'][$key];
    if ($key == 'pass') {
      $value = Apigee\Util\Crypto::encrypt($value);
    }
    $config[$key] = $value;
  }
  $config_copy = $config;
  unset($config_copy['org_settings']);
  $config['org_settings'] = array($config_copy);
  $config['connection_timeout'] = 16;
  $config['request_timeout'] = 16;
  variable_set('devconnect_org_settings', $config);
  $GLOBALS['apigee_api_endpoint_configured'] = TRUE;
  $GLOBALS['install_state']['completed_task'] = install_verify_completed_task();
}

/**
 * Install settings form
 *
 * @param $form
 * @param $form_state
 * @param $install_state
 * @return mixed
 */
function apigee_install_settings_form($form, &$form_state, &$install_state) {
  $attributes = array(
    'autocomplete' => 'off',
    'autocorrect' => 'off',
    'autocapitalize' => 'off',
    'spellcheck' => 'false'
  );
  $form = install_settings_form($form, $form_state, $install_state);
  $form['settings']['mysql']['database']['#default_value'] = 'drops7';
  $form['settings']['mysql']['username']['#default_value'] = 'root';
  $form['settings']['mysql']['database']['#attributes'] = $attributes;
  $form['settings']['mysql']['username']['#attributes'] = $attributes;
  $form['settings']['mysql']['password']['#attributes'] = $attributes;
  $form['actions']['save']['#validate'][] = 'install_settings_form_validate';
  return $form;
}

/**
 * Form for choosing to generate SmartDocs content
 *
 * @param $form
 * @param $form_state
 * @return array
 */
function apigee_generate_make_smartdocs_model($form, &$form_state) {
  // Configure SmartDocs API Proxy URL
  $form = array();
  $form['smartdocs_api_proxy_url'] = array(
    '#markup' => t('Generate sample SmartDocs content.'),
  );
  $form['apigee_api_endpoint_configured'] = array(
    '#type' => 'hidden',
    '#value' => (array_key_exists('apigee_api_endpoint_configured', $GLOBALS) ? intval($GLOBALS['apigee_api_endpoint_configured']) : 0),
  );
  $form['actions'] = array(
    '#weight' => 100,
    '#attributes' => array(
      'class' => array('container-inline'),
    ),
  );
  $form['actions']['save'] = array(
    '#type' => 'submit',
    '#value' => t('Generate sample SmartDocs Content'),
    '#attributes' => array(
      'style' => 'float:left;',
    ),
  );
  $form['actions']['skip'] = array(
    '#type' => 'submit',
    '#limit_validation_errors' => array(),
    '#value' => t('Skip this config'),
    '#submit' => array('apigee_skip_generate_make_smartdocs_model'),
    '#attributes' => array(
      'style' => 'float:left;',
    ),
  );
  $form['#submit'][] = 'apigee_generate_make_smartdocs_model_submit';
  return $form;
}


/**
 * Custom function that skips the Smartdocs installation piece
 *
 * @param $form
 * @param $form_state
 */
function apigee_skip_generate_make_smartdocs_model($form, &$form_state) {
  $GLOBALS['apigee_smartdocs_skip'] = TRUE;
  $GLOBALS['install_state']['completed_task'] = install_verify_completed_task();
}

/**
 * Reapplies apigee_api_endpoint_configured status
 *
 * @param $form
 * @param $form_state
 */
function apigee_generate_make_smartdocs_model_submit($form, &$form_state) {
  // re-apply apigee_api_endpoint_configured status
  $GLOBALS['apigee_api_endpoint_configured'] = $form_state['input']['apigee_api_endpoint_configured'];
  $GLOBALS['install_state']['completed_task'] = install_verify_completed_task();
}

/**
 * Creates model if it doesn't exist
 */
function apigee_generate_import_smartdocs_model_content() {

  _apigee_manage_memory();

  if (isset($GLOBALS['apigee_smartdocs_skip'])) {
    $endpoint_configured = (array_key_exists('apigee_api_endpoint_configured', $GLOBALS) ? $GLOBALS['apigee_api_endpoint_configured'] : FALSE);
    if ($endpoint_configured || $GLOBALS['apigee_smartdocs_skip'] == TRUE) {
      return;
    }
  }
  // Enable SmartDocs Module
  if (!module_exists('smartdocs')) {
    module_enable(array('smartdocs'), TRUE);
  }
  // Create sample SmartDocs Weather Model
  $model_name = 'weather';
  $payload = array(
    'model_name' => $model_name,
    'display_name' => 'Weather Model',
    'model_description' => 'Weather Model (Apigee sample)',
  );
  /** @var SmartDocsModelController $controller */
  $controller = entity_get_controller('smartdocs_model');
  $model = $controller->loadSingle($payload['model_name']);
  if (empty($model)) {
    $model = $controller->create($payload);
  }
  $GLOBALS['smartdocs_latest_revision_number'] = $model['latestRevisionNumber'];
  if (empty($model['latestRevisionNumber'])) {
    $entity = array();
    $entity['apiId'] = $model_name;
    $entity['xml'] = file_get_contents(drupal_get_path('profile', 'apigee') . '/samples/smartdocs/weather.xml');
    $test = $controller->import($entity, 'wadl');
    if (is_array($test)) {
      drupal_set_message($test['message'], 'error');
    }
    else {
      drupal_set_message('The WADL XML has been imported into the model.', 'status');
    }
  }
  else {
    $display = (empty($model['displayName'])) ? $model['name'] : $model['displayName'];
    drupal_set_message($display . ' currently has a revision.  No need to import data.', 'status');
  }
  return;
}

/**
 * Renders content if it doesn't exist
 *
 * @return mixed
 */
function apigee_generate_render_smartdocs_model_template() {

  _apigee_manage_memory();

  if (isset($GLOBALS['apigee_smartdocs_skip'])) {
    if ((!$GLOBALS['apigee_api_endpoint_configured']) || ($GLOBALS['apigee_smartdocs_skip'] == TRUE)) {
      return NULL;
    }
  }

  $context['message'] = t('Ensuring correct model template');
  $html = file_get_contents(drupal_get_path('module', 'smartdocs') . '/templates/smartdocs.hbr');
  entity_get_controller('smartdocs_template')->updateTemplate('weather', 'method', $html);
}

/**
 * Renders content if it doesn't exist
 *
 * @return mixed
 */
function apigee_generate_render_smartdocs_model_content() {

  _apigee_manage_memory();

  if (isset($GLOBALS['apigee_smartdocs_skip'])) {
    if ((!$GLOBALS['apigee_api_endpoint_configured']) || ($GLOBALS['apigee_smartdocs_skip'] == TRUE)) {
      return NULL;
    }
  }
  $model_name = 'weather';
  $model = array(
    'name' => $model_name,
    'displayName' => 'Weather Model',
  );
  /** @var SmartDocsRevisionController $controller */
  $controller = entity_get_controller('smartdocs_revision');
  $display = $model['displayName'];
  if (isset($GLOBALS['smartdocs_latest_revision_number'])) {
    $entity = $controller->loadVerbose($model_name, $GLOBALS['smartdocs_latest_revision_number']);
    drupal_set_message($display . ' is preparing to render revision #' . $GLOBALS['smartdocs_latest_revision_number'], 'status');
  }
  else {
    $entity = $controller->loadVerbose($model_name, 1);
    drupal_set_message($display . ' is preparing to render revision #1', 'status');
  }
  $selected = array();
  foreach ($entity['resources'] as $revision) {
    foreach ($revision['methods'] as $method) {
      $selected[$method['id']] = $method['id'];
    }
  }
  require drupal_get_path('module', 'smartdocs') . '/batch/smartdocs.render.inc';
  $entity['displayName'] = $model['displayName'];
  $entity['name'] = $model['name'];
  $verbose = $entity;
  $selected = array();
  foreach ($entity['resources'] as $revision) {
    foreach ($revision['methods'] as $method) {
      $selected[$method['id']] = $method['id'];
    }
  }
  $batch = smartdocs_render($model, $verbose, $selected, array('publish' => 'publish'), FALSE);
  unset($batch['finished']);
  return $batch;
}

/**
 * Create a Drupal Admin User
 *
 * @param array $form
 * @param array $form_state
 * @return array
 * @author Cesar Galindo
 */
function apigee_install_create_admin_user($form, &$form_state) {
  $attributes = array(
    'autocomplete' => 'off',
    'autocorrect' => 'off',
    'autocapitalize' => 'off',
    'spellcheck' => 'false'
  );
  $form = array();
  $form['firstname'] = array(
    '#type' => 'textfield',
    '#title' => t('Developer First Name'),
    '#required' => TRUE,
    '#default_value' => '',
    '#description' => t('The first name of the administrator.'),
    '#attributes' => $attributes
  );
  $form['lastname'] = array(
    '#type' => 'textfield',
    '#title' => t('Developer Last Name'),
    '#required' => TRUE,
    '#default_value' => '',
    '#description' => t('The last name of the administrator.'),
    '#attributes' => $attributes
  );
  $form['username'] = array(
    '#type' => 'textfield',
    '#title' => t('Developer Portal Username'),
    '#required' => TRUE,
    '#default_value' => '',
    '#description' => t('An admin username used when logging into the Developer Portal.'),
    '#attributes' => $attributes
  );
  $form['pass'] = array(
    '#type' => 'textfield',
    '#title' => t('Developer Portal Password'),
    '#required' => TRUE,
    '#default_value' => '',
    '#description' => t('An admin password used when logging into the Developer Portal.'),
    '#attributes' => $attributes,
    '#post_render' => array('apigee_password_post_render')
  );
  $form['emailaddress'] = array(
    '#type' => 'textfield',
    '#title' => t('Developer Portal Email'),
    '#required' => TRUE,
    '#default_value' => '',
    '#description' => t('Email address to associate with this account.'),
    '#attributes' => $attributes
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
 * Defines the SMTP credentials for the ORG
 *
 * @param $form
 * @param $form_state
 * @return mixed
 */
function apigee_install_smtp_credentials($form, &$form_state) {
  $attributes = array(
    "autocomplete" => "off",
    "autocorrect" => "off",
    "autocapitalize" => "off",
    "spellcheck" => "false"
  );
  $form['server'] = array(
    '#type' => 'fieldset',
    '#title' => t('SMTP server settings'),
  );
  $form['server']['smtp_host'] = array(
    '#type' => 'textfield',
    '#title' => t('SMTP server'),
    '#default_value' => variable_get('smtp_host', ''),
    '#description' => t('The address of your outgoing SMTP server.'),
    '#attributes' => $attributes
  );
  $form['server']['smtp_hostbackup'] = array(
    '#type' => 'textfield',
    '#title' => t('SMTP backup server'),
    '#default_value' => variable_get('smtp_hostbackup', ''),
    '#description' => t('The address of your outgoing SMTP backup server. If the primary server can\'t be found this one will be tried. This is optional.'),
    '#attributes' => $attributes
  );
  $form['server']['smtp_port'] = array(
    '#type' => 'textfield',
    '#title' => t('SMTP port'),
    '#size' => 6,
    '#maxlength' => 6,
    '#default_value' => variable_get('smtp_port', '25'),
    '#description' => t('The default SMTP port is 25, if that is being blocked try 80. Gmail uses 465. See !url for more information on configuring for use with Gmail.', array('!url' => l(t('this page'), 'http://gmail.google.com/support/bin/answer.py?answer=13287'))),
    '#attributes' => $attributes
  );
  // Only display the option if openssl is installed.
  if (function_exists('openssl_open')) {
    $encryption_options = array(
      'standard' => t('No'),
      'ssl' => t('Use SSL'),
      'tls' => t('Use TLS'),
    );
    $encryption_description = t('This allows connection to an SMTP server that requires SSL encryption such as Gmail.');
  }
  // If openssl is not installed, use normal protocol.
  else {
    variable_set('smtp_protocol', 'standard');
    $encryption_options = array('standard' => t('No'));
    $encryption_description = t('Your PHP installation does not have SSL enabled. See the !url page on php.net for more information. Gmail requires SSL.', array('!url' => l(t('OpenSSL Functions'), 'http://php.net/openssl')));
  }
  $form['server']['smtp_protocol'] = array(
    '#type' => 'select',
    '#title' => t('Use encrypted protocol'),
    '#default_value' => variable_get('smtp_protocol', 'standard'),
    '#options' => $encryption_options,
    '#description' => $encryption_description,
  );

  $form['auth'] = array(
    '#type' => 'fieldset',
    '#title' => t('SMTP Authentication'),
    '#description' => t('Leave blank if your SMTP server does not require authentication.'),
  );
  $form['auth']['smtp_username'] = array(
    '#type' => 'textfield',
    '#title' => t('Username'),
    '#default_value' => variable_get('smtp_username', ''),
    '#description' => t('SMTP Username.'),
    '#attributes' => $attributes
  );
  $form['auth']['smtp_password'] = array(
    '#type' => 'password',
    '#title' => t('Password'),
    '#default_value' => variable_get('smtp_password', ''),
    '#description' => t('SMTP password. If you have already entered your password before, you should leave this field blank, unless you want to change the stored password.'),
    '#attributes' => $attributes
  );
  $form['email_options'] = array(
    '#type' => 'fieldset',
    '#title' => t('E-mail options'),
  );
  $form['email_options']['smtp_from'] = array(
    '#type' => 'textfield',
    '#title' => t('E-mail from address'),
    '#default_value' => variable_get('smtp_from', ''),
    '#description' => t('The e-mail address that all e-mails will be from.'),
    '#attributes' => $attributes
  );
  $form['email_options']['smtp_fromname'] = array(
    '#type' => 'textfield',
    '#title' => t('E-mail from name'),
    '#default_value' => variable_get('smtp_fromname', ''),
    '#description' => t('The name that all e-mails will be from. If left blank will use the site name of:') . ' ' . variable_get('site_name', 'Drupal powered site'),
  );
  $form['email_options']['smtp_allowhtml'] = array(
    '#type' => 'checkbox',
    '#title' => t('Allow to send e-mails formated as Html'),
    '#default_value' => variable_get('smtp_allowhtml', 0),
    '#description' => t('Checking this box will allow Html formated e-mails to be sent with the SMTP protocol.'),
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
    '#submit' => array('apigee_skip_smtp_details'),
    '#attributes' => array(
      'style' => 'float:left;',
    ),
  );
  return $form;
}

/**
 * Submit for SMTP credentials
 *
 * @param $form
 * @param $form_state
 */
function apigee_install_smtp_credentials_submit($form, &$form_state) {
  if (!module_exists('smtp')) {
    module_enable(array('smtp'), TRUE);
  }
  $values = $form_state['values'];
  // Make the site use SMTP
  variable_set('smtp_on', 1);
  // SMTP credentials vars
  variable_set('smtp_host', $values['smtp_host']);
  variable_set('smtp_hostbackup', $values['smtp_hostbackup']);
  variable_set('smtp_port', $values['smtp_port']);
  variable_set('smtp_protocol', $values['smtp_protocol']);
  // SMTP authentication vars
  variable_set('smtp_username', $values['smtp_username']);
  variable_set('smtp_password', $values['smtp_password']);
  // SMTP email settings vars
  variable_set('smtp_from', $values['smtp_from']);
  variable_set('smtp_fromname', $values['smtp_fromname']);
  variable_set('smtp_allowhtml', $values['smtp_allowhtml']);
  $GLOBALS['install_state']['completed_task'] = install_verify_completed_task();
}

/**
 * Skips SMTP piece
 *
 * @param $form
 * @param $form_state
 */
function apigee_skip_smtp_details($form, &$form_state) {
  $GLOBALS['install_state']['completed_task'] = install_verify_completed_task();
}

/**
 * Custom function that skips the create admin user installation piece
 *
 * @param $form
 * @param $form_state
 */
function apigee_skip_create_admin_user($form, &$form_state) {
  // skips the config, nothing left to do
  $GLOBALS['install_state']['completed_task'] = install_verify_completed_task();
}

/**
 * hook validate for create admin user form
 *
 * @param string $form
 * @param string $form_state
 * @return void
 * @author Cesar Galindo
 */

function apigee_install_create_admin_user_validate($form, &$form_state) {
  if ($form_state['values']['username'] == 'admin') {
    form_set_error('username', 'Please select a different username.');
  }
  if (!valid_email_address($form_state['values']['emailaddress'])) {
    form_set_error('emailaddress', 'Please select a valid email address.');
  }
}

/**
 * hook submit for create admin user form
 *
 * @param string $form
 * @param string $form_state
 * @return void
 * @author Cesar Galindo
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
 * @param $context
 */
function apigee_install_create_environmental_indicators(&$context) {
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
        'fixed' => 0
      )
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
        'fixed' => 0
      )
    );
    ctools_export_crud_save('environment_indicator_environment', $environment_test);
    $context['message'] = st('Created environmental indicators');
  }
}

function _apigee_manage_memory() {
  ini_set('memory_limit', '1024M');
  ini_set('max_execution_time', 300);
}

/**
 * Batch process callback to enable the autologout module and set the timeout.
 * @param array $context
 */
function apigee_install_configure_autologout(&$context){
  if(!module_exists('autologout')){
    module_enable(array('autologout'), TRUE);
  }
  
  variable_set('autologout_timeout', 300);
  
  $context['message'] = st('Installed and configured autologout module');
}
