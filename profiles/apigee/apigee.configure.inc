<?php
/**
 * @file
 * Contains batch callbacks which perform configuration steps.
 */

/**
 * Configure variables across the environment properly.
 *
 * @param array $context
 *   Current state of installer.
 */
function apigee_install_configure_variables(array &$context) {
  watchdog('apigee_install', 'Config Vars', array(), WATCHDOG_INFO);
  $conf = (array_key_exists('conf', $GLOBALS) ? $GLOBALS['conf'] : array());
  if (array_key_exists('PRESSFLOW_SETTINGS', $_SERVER)) {
    $pressflow = json_decode($_SERVER['PRESSFLOW_SETTINGS'], TRUE);
    if (is_array($pressflow) && array_key_exists('conf', $pressflow)) {
      // Prefer PRESSFLOW_SETTINGS over anything configured in settings.php.
      $conf = array_merge($conf, $pressflow['conf']);
    }
  }
  else {
    // Fill in any defaults that weren't defined in settings.php.
    $conf += array(
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

  // Detect private dir if it is configured already.
  if (array_key_exists('apigee_credential_dir', $conf)) {
    $abs_private_dir = $configured_private_dir = $conf['apigee_credential_dir'];
  }
  else {
    $abs_private_dir = $configured_private_dir = $conf['file_private_path'];
  }
  if (!empty($abs_private_dir) && substr($abs_private_dir, 0, 1) != '/') {
    $abs_private_dir = DRUPAL_ROOT . '/' . $abs_private_dir;
  }
  // Make sure private dir exists and is writable.
  if (!empty($abs_private_dir)) {
    if (!is_dir($abs_private_dir) || !is_writable($abs_private_dir)) {
      unset($configured_private_dir, $abs_private_dir);
    }
  }
  // Do we need to guess where to store?
  if (!isset($abs_private_dir)) {
    foreach (array('sites/default/files/private', 'sites/default/private') as $private_dir) {
      if (is_dir(DRUPAL_ROOT . '/' . $private_dir) && is_writable(DRUPAL_ROOT . '/' . $private_dir)) {
        $configured_private_dir = $private_dir;
        $abs_private_dir = DRUPAL_ROOT . '/' . $private_dir;
        break;
      }
    }
  }
  if (!isset($configured_private_dir)) {
    $configured_private_dir = $conf['file_public_path'] . '/private';
    $abs_private_dir = DRUPAL_ROOT . '/' . $configured_private_dir;
    if (!file_exists($abs_private_dir)) {
      mkdir($abs_private_dir);
    }
  }
  variable_set('apigee_credential_dir', $configured_private_dir);
  $key_file = $abs_private_dir . '/.apigee.key';
  if (!file_exists($key_file) || filesize($key_file) < 32) {
    $crypt_key = drupal_random_bytes(32);
    file_put_contents($key_file, $crypt_key);
  }

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
  variable_set('jquery_update_compression_type', 'min');
  variable_set('jquery_update_jquery_cdn', 'google');
  variable_set('jquery_update_jquery_version', '1.10');
  variable_set('jquery_update_jquery_migrate_enable', 1);
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
  $context['results'][] = 'variables';
  $context['message'] = st('Default variables set.');
}

/**
 * Search config batch item.
 *
 * @param array $context
 *   Current state of installer.
 */
function apigee_install_configure_search(array &$context) {
  watchdog('apigee_install', 'Configuring search', array(), WATCHDOG_INFO);
  $search_active_modules = array(
    'apachesolr_search' => 0,
    'user' => 'user',
    'node' => 'node',
  );
  $search_default_module = 'node';
  user_role_grant_permissions(DRUPAL_AUTHENTICATED_RID, array('search content'));
  user_role_grant_permissions(DRUPAL_ANONYMOUS_RID, array('search content'));
  variable_set('search_active_modules', $search_active_modules);
  variable_set('search_default_module', $search_default_module);
  $context['results'][] = 'search_config';
  $context['message'] = st('Search configured.');
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
    'pass' => user_password(15),
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
  $context['message'] = st('Admin user created.');
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
    'admin_theme' => 'adminimal',
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
      'description' => st('Use <em>basic pages</em> for your static content, such as an ‘About us’ page.'),
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
  $context['message'] = st('Blog content types enabled.');
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
  $context['message'] = st('Content permissions rebuilt.');
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
  if (!array_key_exists('filtered_html', $filters)) {
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
  if (!array_key_exists('full_html', $filters)) {
    // Exported format: Full HTML.
    filter_format_save((object) array(
        'format' => 'full_html',
        'name' => 'Full HTML',
        'cache' => 1,
        'status' => 1,
        'weight' => -10,
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
    'tableresize', 'toolbarswitch', 'widget', 'wysiwygarea',
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

  $plugins['media']['path'] = '%base_path%profiles/apigee/modules/contrib/media_ckeditor/js/plugins/media/';
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
  $plugins_default_true = array(
    'drupalbreaks',
    'tableresize',
    'toolbarswitch',
    'widget',
    'wysiwygarea',
  );

  foreach ($plugins_default_true as $plugin_name) {
    $plugins[$plugin_name]['default'] = 't';
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
    ['InsertPre']]";

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
    ->condition('name', array('filtered', 'full', 'CKEditor Global Profile'))
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
    ->condition('name', array('filtered', 'full'))
    ->execute();
  db_insert('ckeditor_input_format')->fields(array('name' => 'filtered', 'format' => 'filtered_html'))->execute();
  db_insert('ckeditor_input_format')->fields(array('name' => 'full', 'format' => 'full_html'))->execute();
  $context['results'][] = 'ckeditor_settings';
  $context['message'] = st('CKEditor settings built.');
}

/**
 * Batch process callback to create the environmental indicators.
 *
 * @param array $context
 *   Current state of installer.
 */
function apigee_install_create_environmental_indicators(array &$context) {
  if (defined('PANTHEON_ENVIRONMENT')) {
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
    $context['message'] = st('Created environment indicators');
  }
}
