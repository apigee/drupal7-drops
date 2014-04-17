<?php

require_once(dirname(__FILE__) . '/libraries/mgmt-api-php-sdk/Apigee/Util/Crypto.php');
require_once(dirname(__FILE__) . '/modules/custom/d8cmi/lib/Drupal.php');
require_once(dirname(__FILE__) . '/modules/custom/d8cmi/lib/Config.php');
// Make Crypto work properly with R23
if (method_exists('Apigee\Util\Crypto', 'setKey')) {
  Apigee\Util\Crypto::setKey(hash('SHA256', 'w3-Love_ap|s', TRUE));
}


function apigee_install_select_profile(&$install_state) {
  $install_state['parameters']['profile'] = 'apigee';
}

function apigee_install_select_locale(&$install_state) {
  $install_state['parameters']['locale'] = 'en';
}

/**
 * Task handler to load our install profile and enhance the dependency information
 */
function apigee_install_load_profile(&$install_state) {

  // Loading the install profile normally
  install_load_profile($install_state);

  if (array_key_exists('PANTHEON_ENVIRONMENT', $_SERVER)) {
    $install_state['profile_info']['dependencies'][] = "apachesolr";
    $install_state['profile_info']['dependencies'][] = "apachesolr_search";
    $install_state['profile_info']['dependencies'][] = "pantheon_api";
    $install_state['profile_info']['dependencies'][] = "pantheon_apachesolr";
    $install_state['profile_info']['dependencies'][] = "environment_indicator";
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
  //variable_set("install_profile_modules", $install_state['profile_info']['dependencies']);
  //$install_state['profiles'] = array("apigee");
  //drupal_get_messages();
}

/**
 * Create batch items for apigee install
 *
 * @param string $install_state
 * @return array
 * @author Tom Stovall
 */
function apigee_install_configure_batch(&$install_state) {

  return array(
    "title" => t("Configuring your install..."),
    "operations" => array(
      array("apigee_install_configure_variables", array()),
      array("apigee_install_pantheon_push_solr", array()),
      array("apigee_install_configure_solr", array()),
      array("apigee_install_configure_users", array()),
      array("apigee_install_configure_themes", array()),
      array("apigee_install_content_types", array()),
      array("apigee_install_enable_blog_content_types", array()),
      array("apigee_install_rebuild_permissions", array()),
      array("apigee_install_create_homepage", array()),
      array("apigee_install_base_ckeditor_settings", array()),
      array("apigee_install_create_taxonomy_terms", array()),
      array("apigee_install_create_tutorial_content", array()),
      array("apigee_install_create_forum_content", array()),
      array("apigee_install_create_page_content", array()),
      array("apigee_install_create_audio_content", array()),
      array("apigee_install_create_video_content", array()),
      array("apigee_install_create_faq_content", array()),
      array("apigee_install_clear_caches_flush", array()),
      array("apigee_install_clear_caches_css", array()),
      array("apigee_install_clear_caches_js", array()),
      array("apigee_install_clear_caches_theme", array()),
      array("apigee_install_clear_caches_entity", array()),
      array("apigee_install_clear_caches_nodes", array()),
      array("apigee_install_clear_caches_menu", array()),
      array("apigee_install_clear_caches_actions", array()),
      array("apigee_install_clear_caches_core_path", array()),
      array("apigee_install_clear_caches_core_filter", array()),
      array("apigee_install_clear_caches_core_bootstrap", array()),
      array("apigee_install_clear_caches_core_page", array()),
      array("apigee_install_clear_caches_core", array()),
      array("apigee_install_bootstrap_status", array()),
    ),
    'finished' => '_apigee_install_configure_task_finished',
  );
}

function _apigee_install_configure_task_finished($success, $results, $operations) {
  watchdog(__FUNCTION__, "Configure Task Finished", array(), WATCHDOG_INFO);

  $GLOBALS['install_state']['batch_configure_complete'] = install_verify_completed_task();
}

/**
 * Variables batch item
 *
 * @param string $context
 * @return void
 * @author Tom Stovall
 */

function apigee_install_configure_variables(&$context) {
  watchdog(__FUNCTION__, "Config Vars", array(), WATCHDOG_INFO);


  variable_set('cache', 1);
  variable_set('block_cache', 1);
  variable_set('cache_lifetime', '0');
  variable_set('page_cache_maximum_age', '900');
  variable_set('page_compression', 0);
  if (array_key_exists("PRESSFLOW_SETTINGS", $_SERVER)) {
    $pressflow = json_decode($_SERVER['PRESSFLOW_SETTINGS'], TRUE);
    $conf = $pressflow['conf'];
  }
  else {
    $conf = array(
      "file_public_path" => "sites/default/files",
      "file_private_path" => "sites/default/private",
      "file_temporary_path" => "sites/default/tmp"
    );
  }
  variable_set("file_public_path", $conf['file_public_path']);
  variable_set("file_temporary_path", $conf['file_temporary_path']);
  variable_set("file_private_path", $conf['file_private_path']);
  try {
    file_prepare_directory($conf['file_public_path'], FILE_CREATE_DIRECTORY);
    file_prepare_directory($conf['file_temporary_path'], FILE_CREATE_DIRECTORY);
    file_prepare_directory($conf['file_private_path'], FILE_CREATE_DIRECTORY);
  } catch (Exception $e) {
    drupal_set_message(t('unable to create the directories necessary for Drupal to write files: :error', array(":error" => $e->getMessage())));
  }

  variable_set('preprocess_css', 0);
  variable_set('preprocess_js', 0);
  variable_set('clean_url', TRUE);
  variable_set('site_name', "New Apigee Site");
  variable_set('site_mail', "noreply@apigee.com");
  variable_set('date_default_timezone', "America/Los_Angeles"); // Designed by Apigee in California
  variable_set('site_default_country', "US");

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

  $crypt_key = drupal_random_bytes(64);
  variable_set('apigee_crypt_key', $crypt_key);

  $context['results'][] = "variables";
  $context['message'] = st('Default variables set.');
}

/**
 * Push solr xml to Pantheon server
 *
 * @param string $install_state
 * @param string $context
 * @return void
 * @author Tom Stovall
 */


function apigee_install_pantheon_push_solr(&$context) {

  if (array_key_exists('PANTHEON_ENVIRONMENT', $_SERVER) && module_exists("pantheon_apachesolr")) {
    watchdog(__FUNCTION__, "Pushing Solr", array(), WATCHDOG_INFO);
    module_load_include("module", "pantheon_apachesolr");
    pantheon_apachesolr_post_schema_exec("profiles/apigee/modules/contrib/apachesolr/solr-conf/solr-3.x/schema.xml");
    $context['results'][] = "solr_push";
    $context['message'] = st('Solr config pushed to pantheon solr server.');
  }
  else {
    watchdog(__FUNCTION__, "SOLR not enabled.", array(), WATCHDOG_NOTICE);
  }

}

/**
 * Solr config batch item
 *
 * @param string $install_state
 * @param string $context
 * @return void
 * @author Tom Stovall
 */

function apigee_install_configure_solr(&$context) {
  watchdog(__FUNCTION__, "Configuring Solr", array(), WATCHDOG_INFO);
    $search_default_module = 'apachesolr_search';
    if(module_exists('apachesolr')) {
        $search_active_modules = array(
            'apachesolr_search' => 'apachesolr_search',
            'user' => 'user',
            'node' => 0,
        );
    } else {
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
  $context['results'][] = "solr_push";
  $context['message'] = st('Search Configured.');

}

/**
 * Users batch item
 *
 * @param string $install_state
 * @param string $context
 * @return void
 * @author Tom Stovall
 */

function apigee_install_configure_users(&$context) {
  watchdog(__FUNCTION__, "Configuring Default Users", array(), WATCHDOG_INFO);

  $admin_role = new stdClass();
  $admin_role->name = 'administrator';
  $admin_role->weight = 10;
  user_role_save($admin_role);
  db_insert('users_roles')
    ->fields(array('uid' => 1, 'rid' => $admin_role->rid))
    ->execute();

  $roles = array_keys(user_roles());
  foreach ($roles as $role) {
    user_role_grant_permissions($role, array("node" => "access content"));
  }
  variable_set('user_admin_role', $admin_role->rid);
  user_role_grant_permissions($admin_role->rid, array_keys(module_invoke_all('permission')));

  //Anonymous user permissions
  $permissions = array( 'access comments', 'access content','view faq page');
  user_role_grant_permissions(1, $permissions);
  //Authenticated user permissions
  $permissions[] = 'post comments';
  user_role_grant_permissions(2, $permissions);

  $roles = array(3 => TRUE, 4 => TRUE);
  $user = (object) array(
    "uid" => 1,
    "name" => "admin",
    "pass" => md5(mktime()),
    "mail" => "noreply@apigee.com",
    'field_first_name' => array(LANGUAGE_NONE => array(array('value' => "drupal"))),
    'field_last_name' => array(LANGUAGE_NONE => array(array('value' => "admin"))),
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
  $context['results'][] = "admin_user";
  $context['message'] = st('Admin User Created.');

}

/**
 * Themes batch Item
 *
 * @param string $install_state
 * @param string $context
 * @return void
 * @author Tom Stovall
 */

function apigee_install_configure_themes(&$context) {
  watchdog(__FUNCTION__, "Configuring themes", array(), WATCHDOG_INFO);

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
    db_query("update block set status = 0 where delta != 'main'");
    db_query("update block set region = -1 where delta != 'main'");
  } catch (Exception $e) {
    watchdog_exception(__FUNCTION__, $e, "ERROR CONFIGURING THEMES %message", array("%message" => $e->getMessage()), WATCHDOG_ERROR);
  }


  $context['results'][] = "themes";
  $context['message'] = st('Default Apigee theme configured.');

}

function apigee_install_content_types(&$context) {
  watchdog(__FUNCTION__, "Creating default content types", array(), WATCHDOG_INFO);
  $types = array(
    array(
      'type' => 'page',
      'name' => st('Basic page'),
      'base' => 'node_content',
      'description' => st("Use <em>basic pages</em> for your static content, such as an 'About us' page."),
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
  variable_set("pathauto_node_page_pattern", "[node:title]");
  variable_set("pathauto_node_blog_pattern", "blog/[node:title]");
  variable_set("pathauto_node_faq_pattern", "faqs/[node:title]");

  $context['results'][] = "content_types";
  $context['message'] = st('Default content types created.');

}

function apigee_install_enable_blog_content_types() {
  //Needs to be done here not in the .info file
  module_enable(array("devconnect_blog_content_types"));
}


function apigee_install_create_homepage() {
  watchdog(__FUNCTION__, "Generating Homepage", array(), WATCHDOG_INFO);

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
  } catch (Exception $e) {
    watchdog_exception(__FUNCTION__, $e, "Error generating home page: %message", array("%message" => $e->getMessage()), WATCHDOG_ERROR);
  }
  $context['results'][] = "homepage_created";
  $context['message'] = st('Default Homepage Generated!');
}



/**
 * Creates the CKEditor settings for the portal
 *
 * @param $context
 */
function apigee_install_base_ckeditor_settings(&$context) {
  if (!module_exists("filter")) {
    module_enable("filter");
  }
  module_load_include("module", "filter");
  $filters = filter_get_filters();

  if (!in_array("filtered_html", array_keys($filters))) {
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
    db_insert("role_permission")->fields(array("rid" => 2, "permission" => "use text format filtered_html","module"=>"filter"))->execute();
    db_insert("role_permission")->fields(array("rid" => 3, "permission" => "use text format filtered_html","module"=>"filter"))->execute();
  }

  if (!in_array("full_html", array_keys($filters))) {

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
    db_insert("role_permission")->fields(array("rid" => 3, "permission" => "use text format full_html","module"=>"filter"))->execute();
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
    'expand' => "t",
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
    ['Carousel','Featurette','Jumbotron','Trifold']]";

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
      "name" => 'filtered',
      "settings" => serialize($ckeditor_filtered),
    ))
    ->execute();
  db_insert('ckeditor_settings')
    ->fields(array(
      "name" => 'full',
      "settings" => serialize($ckeditor_full),
    ))
    ->execute();
  db_insert('ckeditor_settings')
    ->fields(array(
      "name" => 'CKEditor Global Profile',
      "settings" => serialize($ckeditor_global_settings),
    ))
    ->execute();

  db_delete('ckeditor_input_format')
    ->condition('name', 'Full')
    ->execute();
  db_delete('ckeditor_input_format')
    ->condition('name', 'Advanced')
    ->execute();
  db_insert('ckeditor_input_format')->fields(array("name" => 'filtered', "format" => 'filtered_html'))->execute();
  db_insert('ckeditor_input_format')->fields(array("name" => 'full', "format" => 'full_html'))->execute();

  $context['results'][] = "ckeditor_settings";
  $context['message'] = st('CKEditor Settings Built');
}

/**
 * Creates dummy taxonomy terms
 */
function apigee_install_create_taxonomy_terms(&$context) {
  for ($i = 0; $i <= 5; $i++) {
    $term = new stdClass();
    $term->name = _apigee_install_generate_greek(1, TRUE);
    $term->vid = taxonomy_vocabulary_machine_name_load('forums')->vid;
    taxonomy_term_save($term);
  }
  $context['results'][] = "content_created";
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
  $context['results'][] = "content_created";
  $context['message'] = st('Tutorial Content Generated!');
}

/**
 * Creates default content for the install
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
  $context['results'][] = "content_created";
  $context['message'] = st('10 Example Forum Posts Created!');
}

/**
 * Creates default content for the install
 * @param $context
 */
function apigee_install_create_page_content(&$context) {
  // 5 pages
  for ($i = 0; $i <= 5; $i++) {
    $body = array();
    $body['post'] = _apigee_install_generate_greek(mt_rand(2, 300), TRUE);
    _apigee_install_generate_node('page', $body);
  }
  $context['results'][] = "content_created";
  $context['message'] = st('5 Example Pages Created!');
}

/**
 * Creates default content for the install
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

  $context['results'][] = "content_created";
  $context['message'] = st('Audio Content Created!');
}

/**
 * Creates default content for the install
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
  $context['results'][] = "content_created";
  $context['message'] = st('Video Content Created!');
}

/**
 * Creates default content for the install
 * @param $context
 */
function apigee_install_create_faq_content(&$context) {
  $type = 'faq';
  for ($i = 0; $i <= 3; $i++) {
    _apigee_install_generate_node($type, $body = NULL, $fields = NULL);
  }
  $context['results'][] = "content_created";
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
 * @return integer|boolean
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
 * Generate node function
 */
function _apigee_install_generate_node($type, $body = NULL, $fields = NULL) {
  $node = new stdClass();
  $node->nid = NULL;
  $node->type = $type;
  $users = array();
  $result = db_query_range("SELECT uid FROM {users}", 0, 50);
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
  } else {
    $nparas = mt_rand(1,12);
    $output = '';
    for ($i = 1; $i <= $nparas; $i++) {
      $output .= "<p>" . _apigee_install_generate_greek(mt_rand(10,60)) . "</p>" ."\n\n";
    }
    $node->body[LANGUAGE_NONE][0]['value'] = $output;
    $node->body[LANGUAGE_NONE][0]['summary'] = $output;
    $node->body[LANGUAGE_NONE][0]['format'] = 'filtered_html';
  }
  $node->comment = 1;
  $node->status = 1;
  $node->created = REQUEST_TIME - mt_rand(0, 604800);
  if (!is_null($fields)) {
    switch($fields['type']) {
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
          } else {
            if ($vid) {
              $keyword = taxonomy_get_term_by_name($fields['keyword'], 'blog');
            }
          }
          if (isset($keyword)) {
            foreach ($keyword as $obj) {
              $node->field_keywords[LANGUAGE_NONE][]['tid'] = $obj->tid;
            }
          }
          foreach(taxonomy_get_term_by_name('blog', 'content_type_tag') as $obj){
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
  node_save($node);
}

/**
 * Generates filler content for generated nodes
 * Mimics Devel's devel_creating_greeking, but makes this profile not rely on it
 *
 * @param $context
 */
function _apigee_install_generate_greek($word_count, $title = FALSE) {
  $greek = array("abbas", "abdo", "abico", "abigo", "abluo", "accumsan",
    "acsi", "ad", "adipiscing", "aliquam", "aliquip", "amet", "antehabeo",
    "appellatio", "aptent", "at", "augue", "autem", "bene", "blandit",
    "brevitas", "caecus", "camur", "capto", "causa", "cogo", "comis",
    "commodo", "commoveo", "consectetuer", "consequat", "conventio", "cui",
    "damnum", "decet", "defui", "diam", "dignissim", "distineo", "dolor",
    "dolore", "dolus", "duis", "ea", "eligo", "elit", "enim", "erat",
    "eros", "esca", "esse", "et", "eu", "euismod", "eum", "ex", "exerci",
    "exputo", "facilisi", "facilisis", "fere", "feugiat", "gemino",
    "genitus", "gilvus", "gravis", "haero", "hendrerit", "hos", "huic",
    "humo", "iaceo", "ibidem", "ideo", "ille", "illum", "immitto",
    "importunus", "imputo", "in", "incassum", "inhibeo", "interdico",
    "iriure", "iusto", "iustum", "jugis", "jumentum", "jus", "laoreet",
    "lenis", "letalis", "lobortis", "loquor", "lucidus", "luctus", "ludus",
    "luptatum", "macto", "magna", "mauris", "melior", "metuo", "meus",
    "minim", "modo", "molior", "mos", "natu", "neo", "neque", "nibh",
    "nimis", "nisl", "nobis", "nostrud", "nulla", "nunc", "nutus", "obruo",
    "occuro", "odio", "olim", "oppeto", "os", "pagus", "pala", "paratus",
    "patria", "paulatim", "pecus", "persto", "pertineo", "plaga", "pneum",
    "populus", "praemitto", "praesent", "premo", "probo", "proprius",
    "quadrum", "quae", "qui", "quia", "quibus", "quidem", "quidne", "quis",
    "ratis", "refero", "refoveo", "roto", "rusticus", "saepius",
    "sagaciter", "saluto", "scisco", "secundum", "sed", "si", "similis",
    "singularis", "sino", "sit", "sudo", "suscipere", "suscipit", "tamen",
    "tation", "te", "tego", "tincidunt", "torqueo", "tum", "turpis",
    "typicus", "ulciscor", "ullamcorper", "usitas", "ut", "utinam",
    "utrum", "uxor", "valde", "valetudo", "validus", "vel", "velit",
    "veniam", "venio", "vereor", "vero", "verto", "vicis", "vindico",
    "virtus", "voco", "volutpat", "vulpes", "vulputate", "wisi", "ymo",
    "zelus");
  $greek_flipped = array_flip($greek);

  $greeking = '';

  if (!$title) {
    $words_remaining = $word_count;
    while ($words_remaining > 0) {
      $sentence_length = mt_rand(3, 10);
      $words = array_rand($greek_flipped, $sentence_length);
      $sentence = implode(' ', $words);
      $greeking .= ucfirst($sentence) . '. ';
      $words_remaining -= $sentence_length;
    }
  }
  else {
    // Use slightly different method for titles.
    $words = array_rand($greek_flipped, $word_count);
    $words = is_array($words) ? implode(' ', $words) : $words;
    $greeking = ucwords($words);
  }

  // Work around possible php garbage collection bug. Without an unset(), this
  // function gets very expensive over many calls (php 5.2.11).
  unset($dictionary, $dictionary_flipped);
  return trim($greeking);
}


function apigee_install_rebuild_permissions(&$context) {
  watchdog(__FUNCTION__, "rebuilding permissions", array(), WATCHDOG_INFO);
  try {
    node_access_rebuild(TRUE);
  } catch (Exception $e) {
    watchdog_exception(__FUNCTION__, $e, "Error rebuilding node access: %message", array("%message" => $e->getMessage()), WATCHDOG_ERROR);
  }
  $context['results'][] = "content_permissions";
  $context['message'] = st('Content Permissions Rebuilt');
}

function apigee_install_revert_features(&$context) {
  module_enable(array("curate"));
  features_revert(array('module' => array('curate')));
  $context['results'][] = "features";
  $context['message'] = st('Features reverted');
}


function apigee_install_clear_caches_flush(&$context) {
  watchdog(__FUNCTION__, "Flushing CSS/JS", array(), WATCHDOG_INFO);
  _drupal_flush_css_js();
  $context['results'][] = "cache_flush";
  $context['message'] = st('CSS & JS flushed');
}

function apigee_install_rebuild_registry(&$context) {
  watchdog(__FUNCTION__, "Rebuilding Registry", array(), WATCHDOG_INFO);
  registry_rebuild();
  $context['results'][] = "cache_registry";
  $context['message'] = st('Registry Rebuilt');
}

function apigee_install_clear_caches_css(&$context) {
  watchdog(__FUNCTION__, "Clearing CSS Cache", array(), WATCHDOG_INFO);
  drupal_clear_css_cache();
  $context['results'][] = "cache_css";
  $context['message'] = st('CSS Caches Cleared');
}

function apigee_install_clear_caches_js(&$context) {
  watchdog(__FUNCTION__, "Clearing JS Cache", array(), WATCHDOG_INFO);
  drupal_clear_js_cache();
  $context['results'][] = "cache_js";
  $context['message'] = st('JS Caches Cleared');
}

function apigee_install_clear_caches_theme(&$context) {
  watchdog(__FUNCTION__, "Rebuilding themes...", array(), WATCHDOG_INFO);
  system_rebuild_theme_data();
  drupal_theme_rebuild();
  $context['results'][] = "cache_theme";
  $context['message'] = st('Theme Caches Cleared');
}

function apigee_install_clear_caches_entity(&$context) {
  watchdog(__FUNCTION__, "Clearing Entity Cache...", array(), WATCHDOG_INFO);
  entity_info_cache_clear();
  $context['results'][] = "cache_entity";
  $context['message'] = st('Entity Caches Cleared');
}

function apigee_install_clear_caches_nodes(&$context) {
  watchdog(__FUNCTION__, "Rebuilding Node Types...", array(), WATCHDOG_INFO);
  node_types_rebuild();
  $context['results'][] = "cache_node";
  $context['message'] = st('Node Caches Cleared');
}

function apigee_install_clear_caches_menu(&$context) {
  watchdog(__FUNCTION__, "Rebuilding Menu...", array(), WATCHDOG_INFO);
  menu_rebuild();
  $context['results'][] = "cache_menu";
  $context['message'] = st('Menu Caches Cleared');
}

function apigee_install_clear_caches_actions(&$context) {
  watchdog(__FUNCTION__, "Synchronizing Actions...", array(), WATCHDOG_INFO);
  actions_synchronize();
  $context['results'][] = "cache_action";
  $context['message'] = st('Action Caches Cleared');
}

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

function apigee_install_clear_caches_core(&$context) {
  _apigee_install_clear_cache('cache', 'cache_core', 'Core', $context);
}

function apigee_install_clear_caches_core_path(&$context) {
  _apigee_install_clear_cache('cache_path', 'cache_path', 'Path', $context);
}

function apigee_install_clear_caches_core_filter(&$context) {
  _apigee_install_clear_cache('cache_filter', 'cache_filter', 'Filter', $context);
}

function apigee_install_clear_caches_core_bootstrap(&$context) {
  _apigee_install_clear_cache('cache_bootstrap', 'cache_bootstrap', 'Bootstrap', $context);
}

function apigee_install_clear_caches_core_page(&$context) {
  _apigee_install_clear_cache('cache_page', 'cache_page', 'Page', $context);
}

function apigee_install_bootstrap_status(&$context) {
  watchdog(__FUNCTION__, "Updating bootstrap status...", array(), WATCHDOG_INFO);
  _system_update_bootstrap_status();
  drupal_get_messages();
  $context['results'][] = "bootstrap_status";
  $context['message'] = st('Bootstrap Status Reset.');
}

/**
 * Set the apigee endpoint configuration vars
 *
 * @param array $form
 * @param array $form_state
 * @return array
 * @author Tom Stovall
 */

function apigee_install_api_endpoint($form, &$form_state) {


  if (isset($_REQUEST['devconnect_org'])) {
    $org = $_REQUEST['devconnect_org'];
  }
  else {
    if (array_key_exists('PANTHEON_ENVIRONMENT', $_SERVER)) {
      $org = str_replace($_SERVER['PANTHEON_ENVIRONMENT'] . "-", "", $_SERVER['HTTP_HOST']);
      $org = str_replace(".devportal.apigee.com", "", $org);
    }
    else {
      $org = "";
    }
  }
  if (isset($_REQUEST['devconnect_endpoint'])) {
    $endpoint = $_REQUEST['devconnect_endpoint'];
  }
  else {
    $endpoint = "https://api.enterprise.apigee.com/v1";
  }
  $attributes = array(
    "autocomplete" => "off",
    "autocorrect" => "off",
    "autocapitalize" => "off",
    "spellcheck" => "false"
  );
  $form = array();

  $form['org'] = array(
    '#type' => 'textfield',
    '#title' => t('Dev Portal Organization'),
    '#required' => TRUE,
    '#default_value' => $org,
    '#description' => t('The v4 product organization name. Changing this value could make your site not work.'),
    '#attributes' => $attributes
  );

  $form['endpoint'] = array(
    '#type' => 'textfield',
    '#title' => t('Dev Portal Endpoint URL'),
    '#required' => TRUE,
    '#default_value' => $endpoint,
    '#description' => t('URL to which to make Apigee REST calls. For on-prem installs you will need to change this value.'),
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
  $form['#submit'][] = "apigee_install_api_endpoint_submit";
  $form['#validate'][] = "apigee_install_api_endpoint_validate";

  return $form;
}

/**
 * Validate if the connection is successful
 */
function apigee_install_api_endpoint_validate($form, &$form_state){
  $org = $form_state['values']['org'];
  $endpoint = $form_state['values']['endpoint'];
  $user = $form_state['values']['user'];
  $pass = $form_state['values']['pass'];
  $return = _devconnect_test_kms_connection($org, $endpoint, $user, $pass);
  if (strpos($return, t('Connection Successful')) === FALSE) { //If connection is not successful
    form_set_error('form', $return);
  }
}
/**
 * Turns a text field into a password field.
 *
 * @param string $content
 * @param array $element
 * @return string
 */
function apigee_password_post_render($content, $element) {
  return str_replace('type="text"', 'type="password"', $content);
}

/**
 * Custom function that skips the devconnect installation piece
 */
function apigee_skip_api_endpoint($form, &$form_state) {
  // skips the config, so let's turn off the devconnect modules
  $modules = module_list();
  $disable = array();
  foreach ($modules as $name => $module) {
    if ($name) {
      if (strpos($name, 'devconnect') !== FALSE) {
        $disable[] = $name;
      }
    }
  }
  // module_disable($disable);
  $GLOBALS['apigee_api_endpoint_configured'] = FALSE;
  $GLOBALS['install_state']['completed_task'] = install_verify_completed_task();
}

/**
 * hook submit for endpoint vars form
 *
 * @param string $form
 * @param string $form_state
 * @return void
 * @author Tom Stovall
 */

function apigee_install_api_endpoint_submit($form, &$form_state) {

  $config = Drupal::config('devconnect.settings');
  foreach (array('org', 'endpoint', 'user', 'pass') as $key) {
    $value = $form_state['values'][$key];
    if ($key == 'pass') {
      $value = Apigee\Util\Crypto::encrypt($value);
    }
    $config->set($key, $value);
  }
  $config->save();
  $GLOBALS['apigee_api_endpoint_configured'] = TRUE;
  $GLOBALS['install_state']['completed_task'] = install_verify_completed_task();
}

function apigee_install_settings_form($form, &$form_state, &$install_state) {
  $attributes = array(
    "autocomplete" => "off",
    "autocorrect" => "off",
    "autocapitalize" => "off",
    "spellcheck" => "false"
  );
  $form = install_settings_form($form, $form_state, $install_state);
  $form['settings']['mysql']['database']["#default_value"] = "drops7";
  $form['settings']['mysql']['username']["#default_value"] = "root";
  $form['settings']['mysql']['database']["#attributes"] = $attributes;
  $form['settings']['mysql']['username']["#attributes"] = $attributes;
  $form['settings']['mysql']['password']["#attributes"] = $attributes;
  $form['actions']['save']["#validate"][] = "install_settings_form_validate";
  return $form;
}

function apigee_install_settings_form_submit($form, &$form_state, &$install_state) {


}

function apigee_generate_make_smartdocs_model() {
  if ($GLOBALS['apigee_api_endpoint_configured']!==TRUE) {
     return;
  }
  $css = 'https://smartdocs.apigee.com/static/css/main_cms.css
https://smartdocs.apigee.com/static/css/codemirror.css
https://smartdocs.apigee.com/static/css/prism.css';

  $js = 'https://smartdocs.apigee.com/static/js/codemirror.js
https://smartdocs.apigee.com/static/js/codemirror_javascript.js
https://smartdocs.apigee.com/static/js/codemirror_xml.js
https://smartdocs.apigee.com/static/js/prism.js
https://smartdocs.apigee.com/static/js/base64_min.js
https://smartdocs.apigee.com/static/js/model_cms.js
https://smartdocs.apigee.com/static/js/controller_cms.js';

  $model_name = 'weather';
  $payload = array(
    'model_name' => $model_name,
    'display_name' => 'Weather Model',
    'model_description' => 'Weather Model (Apigee sample)',
  );
  $model = entity_get_controller('docgen_model')->loadSingle($payload['model_name']);
  if (empty($model)) {
    if (entity_get_controller('docgen_model')->create($payload)) {
      variable_set(_devconnect_docgen_model_name($payload['model_name']), $payload['model_name']);
      _devconnect_docgen_render_operation_template($payload['model_name'], '3');
    }
  }
  variable_set(_devconnect_docgen_model_name($model_name) . '_bootstrap_ver', '3');
  variable_set(_devconnect_docgen_model_name($model_name) . '_css', $css);
  variable_set(_devconnect_docgen_model_name($model_name) . '_js', $js);
}

function apigee_generate_import_smartdocs_model() {
  if ($GLOBALS['apigee_api_endpoint_configured']!==TRUE) {
      return;
  }
  $model_name = 'weather';
  $entity = array();
  $entity['apiId'] = $model_name;
  $entity['xml'] = file_get_contents(drupal_get_path('profile', 'apigee') . "/samples/smartdocs/weather.xml");
  $test = entity_get_controller('docgen_model')->import($entity, 'wadl');
  if (is_array($test)) {
    drupal_set_message($test['message'], 'error');
  } else {
    drupal_set_message('The WADL XML has been imported into the model.', 'status');
  }
}

function apigee_generate_smartdocs_content() {
  if ($GLOBALS['apigee_api_endpoint_configured']!==TRUE) {
      return;
  }
  $model_name = 'weather';
  $payload = array(
    'model_name' => $model_name,
    'display_name' => 'Weather Model',
    'model_description' => 'Weather Model (Apigee sample)',
  );
  $model = entity_get_controller('docgen_model')->loadSingle($model_name);
  $entity = entity_get_controller('docgen_revision')->loadVerbose($model_name, $model['latestRevisionNumber']);
  $selected = array();
  foreach($entity['resources'] as $revision){
    foreach($revision['methods'] as $method)
      $selected[$method['id']] = $method['id'];
  }
  $entity['displayName'] = $payload['display_name'];
  $entity['name'] = $payload['model_name'];
  $verbose = $entity;
  //Publish the Nodes
  require drupal_get_path('module', 'devconnect_docgen') .'/includes/devconnect_docgen.batch_import.inc';
  $batch = _devconnect_docgen_import_nodes($model_name, $verbose, $selected, array('publish'=>'publish'), '3');
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
    "autocomplete" => "off",
    "autocorrect" => "off",
    "autocapitalize" => "off",
    "spellcheck" => "false"
  );
  $form = array();

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
  $form['#submit'][] = "apigee_install_create_admin_user_submit";

  return $form;
}

/**
 * Custom function that skips the create admin user installation piece
 */
function apigee_skip_create_admin_user($form, &$form_state) {
  // skips the config, nothing left to do
  $GLOBALS['install_state']['completed_task'] = install_verify_completed_task();
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

  $account = new StdClass();
  $account->is_new = TRUE;
  $account->status = TRUE;
  $account->name = $form_state['values']['username'];
  $account->pass = user_hash_password($form_state['values']['pass']);
  $account->mail = $form_state['values']['emailaddress'];
  $account->init = $form_state['values']['emailaddress'];
  $role = user_role_load_by_name('administrator');
  $rid = $role->rid;
  $account->roles[$rid] = 'administrator';
  $account->field_first_name[LANGUAGE_NONE][0]['value'] = 'FirstName';
  $account->field_last_name[LANGUAGE_NONE][0]['value'] = 'LastName';
  user_save($account);

  $GLOBALS['install_state']['completed_task'] = install_verify_completed_task();
}
