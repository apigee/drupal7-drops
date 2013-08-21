<?php

require_once (dirname(__FILE__)."/modules/custom/devconnect/lib/Apigee/Exceptions/InstallException.php");
require_once (dirname(__FILE__)."/modules/custom/devconnect/lib/Apigee/Util/Crypto.php");




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

  // Include any dependencies that we might have missed...
  $dependencies = $install_state['profile_info']['dependencies'];
  foreach ($dependencies as $module) {
    $module_info = drupal_parse_info_file(drupal_get_path('module', $module) . '/' . $module . '.info');
    if (!empty($module_info['dependencies'])) {
      foreach ($module_info['dependencies'] as $dependency) {
        $parts = explode(' (', $dependency, 2);
        $dependencies[] = array_shift($parts);
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
      if ($mysqli->query("create database if not exists drops7") === TRUE){
        $mysqli->close();
      } else {
        throw new Exception($mysqli->error);
      }
    } catch(Exception $e) {
      throw new \Apigee\Exceptions\InstallException($e->getMessage());
    }
  //}
  
  $settings = array(
      "host" => "localhost",
      "driver" => "mysql",
      "username" => "root",
      "password" => null,
      'database' => 'drops7',
      'prefix' => null
    );

  $errors = install_database_errors($settings, $settings_file);
  if (count($errors) >= 1) {
    throw new \Apigee\Exceptions\InstallException($errors[0]);
  } else {
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

function apigee_install_configure_batch(&$install_state){
  
  return array(
    "title" => t("Configuring your install..."),
    "operations" => array(
      array("apigee_install_configure_variables", array()),
      array("apigee_install_pantheon_push_solr", array()),
      array("apigee_install_configure_solr", array()),      
      array("apigee_install_configure_users", array()),      
      array("apigee_install_configure_themes", array()),      
      array("apigee_install_revert_features", array()),
      array("apigee_feature_install_revert", array("devconnect_user")),
      array('apigee_feature_install_revert', array('devconnect_default_structure')),
      array("apigee_feature_install_revert", array("devconnect_default_content")),
      array("apigee_feature_rebuild_permissions", array()),
      array("apigee_install_clear_caches", array()),
    ),
    'finished' => '_apigee_install_configure_task_finished',    
  );
}

function _apigee_install_configure_task_finished($success, $results, $operations) {
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

function apigee_install_configure_variables( &$context) {
  
  
  variable_set('cache', 1);
  variable_set('block_cache', 1);
  variable_set('cache_lifetime', '0');
  variable_set('page_cache_maximum_age', '900');
  variable_set('page_compression', 0);
  if (array_key_exists("PRESSFLOW_SETTINGS", $_SERVER)) {
    $pressflow = json_decode($_SERVER['PRESSFLOW_SETTINGS'], true);
    $conf = $pressflow['conf'];
  } else {
    $conf = array(
      "file_public_path" => "sites/default/files",
      "file_private_path" => "sites/default/private",
      "file_temporary_path" => "sites/default/tmp"
    );
  }
  variable_set("file_public_path", $conf['file_public_path']);
  variable_set("file_temporary_path", $conf['file_temporary_path']);
  variable_set("file_private_path", $conf['file_private_path']);
  try{
    file_prepare_directory($conf['file_public_path'], FILE_CREATE_DIRECTORY);
    file_prepare_directory($conf['file_temporary_path'], FILE_CREATE_DIRECTORY);
    file_prepare_directory($conf['file_private_path'], FILE_CREATE_DIRECTORY);
  } catch(Exception $e) {
    drupal_set_message(t('unable to create the directories necessary for Drupal to write files: :error', array(":error" => $e->getMessage())));
  }
  
  variable_set('preprocess_css', 1);
  variable_set('preprocess_js', 1);
  variable_set('clean_url', true);
  variable_set('site_name', "New Apigee Site");
  variable_set('site_mail', "noreply@apigee.com");
  variable_set('date_default_timezone', "America/Los_Angeles"); // Designed by Apigee in California
  variable_set('site_default_country', "US");
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


function apigee_install_pantheon_push_solr( &$context) {
  
  
  if (array_key_exists("PRESSFLOW_SETTINGS", $_SERVER)){
    module_enable(array("pantheon_api", "pantheon_apachesolr"), TRUE);
    module_load_include("module", "pantheon_apachesolr");
    pantheon_apachesolr_update_schema("profiles/apigee/modules/contrib/apachesolr/solr-conf/solr-3.x/schema.xml");
    $context['results'][] = "solr_push";
    $context['message'] = st('Solr config pushed to pantheon solr server.');
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

function apigee_install_configure_users( &$context) {
  
  
  $admin_role = new stdClass();
  $admin_role->name = 'administrator';
  $admin_role->weight = 10;
  user_role_save($admin_role);
  db_insert('users_roles')
    ->fields(array('uid' => 1, 'rid' => $admin_role->rid))
    ->execute();
  
  $roles = array_keys(user_roles());
  foreach($roles as $role) {
    user_role_grant_permissions($role, array("node" => "access content"));
  }
  variable_set('user_admin_role', $admin_role->rid);
  user_role_grant_permissions($admin_role->rid, array_keys(module_invoke_all('permission')));
  
  $roles = array(3 => true, 4 => true);
  $user = (object)array(
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
  if ($results){
    drupal_set_message(t('Admin user created. Use password recovery or drush to set the password.'));
  } else {
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

function apigee_install_configure_themes( &$context) {
  
  
    $default_theme = "apigee_devconnect";
    $admin_theme = "rubik";
    // activate admin theme when editing a node
    variable_set('node_admin_theme', '1');
    variable_set("site_frontpage", "home");
    
    db_update('system')
      ->fields(array('status' => 0))
      ->condition('type', 'theme')
      ->execute();
    $enable = array(
        'theme_default' => 'apigee_devconnect',
        'admin_theme' => 'rubik',
        'apigee_base'
      );
    theme_enable($enable);
    
    foreach ($enable as $var => $theme) {
      if (!is_numeric($var)) {
        variable_set($var, $theme);
      }
    }
  drupal_flush_all_caches();
  $context['results'][] = "themes";
  $context['message'] = st('Default Apigee theme configured.');
  
}

/**
 * Features batch item
 *
 * @param string $install_state 
 * @param string $context 
 * @return void
 * @author Tom Stovall
 */

function apigee_feature_install_revert($feature, &$context) {
    features_install_modules(array($feature));
    if (module_exists($feature)) {
      features_revert(array($feature));
      $context['results'][] = "features_".$feature;
      $context['message'] = st('Feature: %feature enabled & reverted.', array("%feature" => $feature));
    } else {
      drupal_set_message("Feature not enabled: %feature", array("%feature" => $feature), "error");
      $context['results'][] = "features_".$feature;
      $context['message'] = st('Feature: %feature <b style="color:red;">NOT</b> enabled and reverted.', array("%feature" => $feature));
    }
    drupal_get_messages("status");

}


function apigee_feature_rebuild_permissions(&$context) {
  node_access_rebuild(TRUE);
  $context['results'][] = "content_permissions";
  $context['message'] = st('Content Permissions Rebuilt');
}


function apigee_install_clear_caches(&$context){
  drupal_flush_all_caches();
  $context['results'][] = "cache";
  $context['message'] = st('Caches Cleared');
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
  } else {
    if (isset($_SERVER['PANTHEON_ENVIRONMENT'])){
      $org = str_replace($_SERVER['PANTHEON_ENVIRONMENT']."-", "", $_SERVER['HTTP_HOST']);
      $org = str_replace(".devportal.apigee.com", "", $org);
    } else {
      $org = "";
    }
  }
  if (isset($_REQUEST['devconnect_endpoint'])) {
    $endpoint = $_REQUEST['devconnect_endpoint'];
  } else {
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
  $form['actions'] = array(
    '#weight' => 100,
  );
  $form['actions']['save'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  $form['#submit'][] = "apigee_install_api_endpoint_submit";
  return $form;
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
  foreach($values as $key => $value) {
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



