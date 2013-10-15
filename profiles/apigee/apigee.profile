<?php

require_once(dirname(__FILE__) . '/modules/custom/devconnect/lib/Apigee/Exceptions/InstallException.php');
require_once(dirname(__FILE__) . '/modules/custom/devconnect/lib/Apigee/Util/Crypto.php');
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
    $install_state['profile_info']['dependencies'][] = "pantheon_api";
    $install_state['profile_info']['dependencies'][] = "pantheon_apachesolr";
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
  // Do not enable apachesolr modules for OPDK builds.
  if (getenv('OPDK_BUILD') == 'yes') {
    foreach (array_keys($dependencies) as $i) {
      if (substr($dependencies[$i], 0, 10) == 'apachesolr') {
        unset($dependencies[$i]);
      }
    }
  }

  $install_state['profile_info']['dependencies'] = array_unique($dependencies);
  //variable_set("install_profile_modules", $install_state['profile_info']['dependencies']);
  //$install_state['profiles'] = array("apigee");  
  //drupal_get_messages();
}

/**
 * Settings install
 *
 * @param string $install_state
 * @return void
 * @author Tom Stovall
 */


function apigee_install_settings(&$install_state) {

  drupal_static_reset('conf_path');
  $conf_path = './' . conf_path(FALSE);
  $settings_file = $conf_path . '/settings.php';

  //if (!is_pantheon()) {
  try {
    $mysqli = new mysqli("localhost", "root");
    /* check connection */
    if ($mysqli->connect_errno) {
      throw new Exception($mysqli->connect_error);
    }
    if ($mysqli->query("create database if not exists drops7") === TRUE) {
      $mysqli->close();
    }
    else {
      throw new Exception($mysqli->error);
    }
  } catch (Exception $e) {
    throw new \Apigee\Exceptions\InstallException($e->getMessage());
  }
  //}

  $settings = array(
    "host" => "localhost",
    "driver" => "mysql",
    "username" => "root",
    "password" => NULL,
    'database' => 'drops7',
    'prefix' => NULL
  );

  $errors = install_database_errors($settings, $settings_file);
  if (count($errors) >= 1) {
    throw new \Apigee\Exceptions\InstallException($errors[0]);
  }
  else {
    $form_state = array('values' => $settings);
    install_settings_form_submit(array(), $form_state);
  }

  $install_state['settings_verified'] = TRUE;
  $install_state['completed_task'] = install_verify_completed_task();
}


/**
 * Create batch items for apigee install
 *
 * @param string $install_state
 * @return void
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
      array("apigee_install_rebuild_permissions", array()),
      array("apigee_install_create_homepage", array()),
      array("apigee_install_create_default_content", array()),
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

  global $install_state;
  $install_state['batch_configure_complete'] = install_verify_completed_task();
}

/**
 * Variables batch item
 *
 * @param string $install_state
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

  $context['results'][] = "variables";
  $context['message'] = st('Defautl variables set.');
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

  watchdog(__FUNCTION__, "Pushing Solr", array(), WATCHDOG_INFO);

  if (array_key_exists('PANTHEON_ENVIRONMENT', $_SERVER) && module_exists("pantheon_apachesolr")) {
    module_load_include("module", "pantheon_apachesolr");
    pantheon_apachesolr_post_schema_exec("profiles/apigee/modules/contrib/apachesolr/solr-conf/solr-3.x/schema.xml");
    $context['results'][] = "solr_push";
    $context['message'] = st('Solr config pushed to pantheon solr server.');
  }
  elseif (getenv('OPDK_BUILD') == 'yes') {
    // If apachesolr crept into the build tree, disable it if this is an OPDK build.
    $to_disable = array();
    foreach (module_list(TRUE) as $module) {
      if (substr($module, 0, 10) == 'apachesolr') {
        $to_disable[] = $module;
      }
    }
    if (count($to_disable) > 0) {
      module_disable($to_disable);
      drupal_uninstall_modules($to_disable);
    }
  }
  else {
    watchdog(__FUNCTION__, "SOLR NOT ENABLED!!!", array(), WATCHDOG_ERROR);
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
  $search_active_modules = array(
    'apachesolr_search' => 'apachesolr_search',
    'user' => 'user',
    'node' => 0
  );

  variable_set('search_active_modules', $search_active_modules);
  variable_set('search_default_module', 'apachesolr_search');
  $context['results'][] = "solr_push";
  $context['message'] = st('Solr Configured.');

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

  $default_theme = "apigee_devconnect";
  $admin_theme = "rubik";
  // activate admin theme when editing a node
  variable_set('node_admin_theme', '1');

  db_update('system')
    ->fields(array('status' => 0))
    ->condition('type', 'theme')
    ->execute();
  $enable = array(
    'theme_default' => 'apigee_devconnect',
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

function apigee_install_create_default_content(&$context) {
  watchdog(__FUNCTION__, "Generating default content nodes", array(), WATCHDOG_INFO);
  $gen = array();
  $gen['values'] = array(
    'node_types' => array(
      'blog' => 'blog',
      'page' => 'page',
      'forum' => 'forum'
    ),
    'title_length' => 6,
    'num_nodes' => 20,
    'max_comments' => 0,
    'time_range' => 604800
  );
  try {
    module_load_include('inc', 'devel_generate');
    devel_generate_content($gen);
  } catch (Exception $e) {
    watchdog_exception(__FUNCTION__, $e, "Error generating default content: %message", array("%message" => $e->getMessage()), WATCHDOG_ERROR);
  }
  $context['results'][] = "content_created";
  $context['message'] = st('Default Content Generated!');
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
  $context['message'] = st("$results_label caches cleared");
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
 *  Set the apigee endpoint configuration vars
 *
 * @param string $form
 * @param string $form_state
 * @return void
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
  $form['devconnect_org'] = array(
    '#type' => 'textfield',
    '#title' => t("Devconnect Organization"),
    '#default_value' => $org,
    '#description' => t('The v4 product organization name. Changing this value could make your site not work.'),
    '#required' => TRUE,
    '#attributes' => $attributes
  );
  $form['devconnect_endpoint'] = array(
    '#type' => 'textfield',
    '#title' => t("Devconnect Endpoint"),
    '#default_value' => $endpoint,
    '#description' => t('URL to which to make Apigee Management UI REST calls. For on-prem installs you will need to change this value.'),
    '#required' => TRUE,
    '#attributes' => $attributes
  );
  $form['devconnect_curlauth'] = array(
    '#type' => 'textfield',
    '#title' => t("Authentication for the Endpoint"),
    '#default_value' => "<USERNAME>:<PASSWORD>",
    '#description' => t('These values be used to authenticate with the endpoint. Separate the Username and Password with a colon (e.g. "guest:secret").'),
    '#required' => TRUE,
    '#attributes' => $attributes
  );

  /**
   * Test connection functionality uses ajax, therefore we need to be on the same form where we supply the info to test.

  $test_org = $form['devconnect_org']['#default_value'];
  $test_endpoint = $form['devconnect_endpoint']['#default_value'];
  $test_curl_auth = $form['devconnect_curlauth']['#default_value'];

  if (array_key_exists('storage', $form_state) && array_key_exists('connection_status', $form_state['storage'])) {
    $status = $form_state['storage']['connection_status'];
  }
  else {
    $status = '';
  }

  $form['connect_test'] = array(
    '#type' => 'fieldset',
    '#title' => t('Connection Configuration'),
    '#weight' => 100,
  );
  $form['connect_test']['test_connection'] = array(
    '#type' => 'submit',
    '#value' => t('Test Connection'),
    '#submit' => array('_apigee_install_test_management_connection_submit'),
    '#ajax' => array(
      'callback' => '_apigee_install_test_management_connection_ajax',
      'wrapper' => 'test-connect-result',
      'method' => 'replace',
      'effect' => 'fade',
    ),
  );
  $form['connect_test']['test_connection_status'] = array(
    '#type' => 'item',
    '#prefix' => '<div id="test-connect-result">',
    '#suffix' => '</div>',
    '#markup' => (isset($status)) ? '<br/>' . $status : '<br/>' .
       _apigee_install_test_kms_connection($test_org, $test_endpoint, $test_curl_auth),
  );
   *
   *
   */

  $form['actions'] = array(
    '#weight' => 100,
  );
  $form['actions']['save'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  $form['#submit'][] = "apigee_install_api_endpoint_submit";

  // @todo, you should test the connection in a hook validate too - force a form_set_error

  return $form;
}

/**
 * Ajax Callback for Testing the KMS Connection
 *
 * @param $form
 * @param $form_state
 * @return mixed
 * @author Brian Hasselbeck
 */
function _apigee_install_test_management_connection_ajax($form, &$form_state) {
  return $form['connect_test']['test_connection_status'];
}

/**
 * Submit Callback for testing the KMS Connection
 *
 * @param $form
 * @param $form_state
 * @author Brian Hasselbeck
 */
function _apigee_install_test_management_connection_submit($form, &$form_state) {
  // provide the currently user supplied credentials
  $org = $form_state['input']['devconnect_org'];
  $endpoint = $form_state['input']['devconnect_endpoint'];
  $curl_auth = $form_state['input']['devconnect_curlauth'];

  // make the form states rebuild and store important variables
  $form_state['rebuild'] = TRUE;
  $form_state['storage']['ajaxed'] = TRUE;
  $form_state['storage']['connection_status'] = _apigee_install_test_kms_connection($org, $endpoint, $curl_auth);
}

/**
 * Test Function for connectivity to the KMS
 *
 * @param $org
 * @param $endpoint
 * @param $curl_auth
 * @return string
 * @author Brian Hasselbeck, Daniel Johnson
 */
function _apigee_install_test_kms_connection($org, $endpoint, $curl_auth) {
  if (defined('APIGEE_DEVCONNECT_DEFAULT_ORG') && $org == APIGEE_DEVCONNECT_DEFAULT_ORG) {
    return '<span style="color:red">' . t('Invalid org') . '</span>';
  }
  if (!valid_url($endpoint)) {
    return '<span style="color:red">' . t('Invalid endpoint') . '</span>';
  }
  list($user, $pass) = explode(':', $curl_auth, 2);

  // Register the autoloader
  devconnect_boot();
  $client = new Apigee\Util\APIClient($endpoint, $org, $user, $pass);
  $dev_app = new Apigee\ManagementAPI\DeveloperAppAnalytics($client);
  try {
    $dev_app->queryEnvironments(); // uses R23 camelCase in methods
    return '<span style="color:green">' . t('Connection Successful') . '</span>';
  }
  catch (Exception $e) {}
  return '<span style="color:red">' . t('Connection Unsuccessful') . '</span>';
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


  $raw_auth = $form_state['values']['devconnect_curlauth'];
  list($username, $raw_pass) = explode(':', $raw_auth, 2);
  $pass = Apigee\Util\Crypto::encrypt($raw_pass);
  $form_state['values']['devconnect_curlauth'] = "{$username}:{$pass}";

  $values = $form_state['values'];
  foreach ($values as $key => $value) {
    if (substr($key, 0, 10) == "devconnect") {
      variable_set($key, $value);
    }
  }

  $install_state['completed_task'] = install_verify_completed_task();
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



