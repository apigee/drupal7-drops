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
 * Set up base config
 */
function apigee_install_configure(&$install_state) {

  // Set default Pantheon variables
  variable_set('cache', 1);
  variable_set('block_cache', 1);
  variable_set('cache_lifetime', '0');
  variable_set('page_cache_maximum_age', '900');
  variable_set('page_compression', 0);
  variable_set('preprocess_css', 1);
  variable_set('preprocess_js', 1);
  $search_active_modules = array(
    'apachesolr_search' => 'apachesolr_search',
    'user' => 'user',
    'node' => 0
  );
  variable_set('clean_url', true);

  variable_set('search_active_modules', $search_active_modules);
  variable_set('search_default_module', 'apachesolr_search');
  db_update('system')
    ->fields(array('status' => 0))
    ->condition('type', 'theme')
    ->execute();
  theme_enable(array("apigee_devconnect", "apigee_base"));
  variable_set('admin_theme', "rubik");
  variable_set('theme_default', "apigee_devconnect");
  
  variable_set('site_name', "New Apigee Site");
  variable_set('site_mail', "noreply@apigee.com");
  variable_set('date_default_timezone', "America/Los_Angeles"); // Designed by Apigee in California
  variable_set('site_default_country', "US");
  drupal_set_message(t('Apigee defaults configured.'));
  
  $roles = array(3 => true, 4 => true);
  $user = (object)array(
    "uid" => 1,
    "name" => "admin",
    "mail" => "noreply@apigee.com",
    'field_first_name' => array(LANGUAGE_NONE => array(array('value' => "drupal"))),
    'field_last_name' => array(LANGUAGE_NONE => array(array('value' => "admin"))),
    'status' => 1,
    'access' => REQUEST_TIME,
    'roles' => $roles,
  );
  $results = user_save($user);
  drupal_get_messages();
  
  if ($results){
    drupal_set_message(t('Admin user created. Use password recovery or drush to set the password.'));
  } else {
    drupal_set_message(t('Unable to create admin user.'));
  }
  $install_state['completed_task'] = install_verify_completed_task();
  drupal_flush_all_caches();
}




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
    $endpoint = "https://api.entierprise.apigee.com/v1";
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
  drupal_flush_all_caches();
  
}

function apigee_install_pantheon_push_solr() {
  if (array_key_exists("PANTHEON_ENVIRONMENT", $_SERVER)){
    if (!module_exists("pantheon_apachesolr")) {
      module_enable("pantheon_apachesolr", TRUE);
    }
    if (!module_exists("pantheon_api")) {
      module_enable("pantheon_api", TRUE);
    }
    module_load_include("module", "pantheon_apachesolr");
    pantheon_apachesolr_update_schema("profiles/apigee/modules/contrib/apachesolr/solr-conf/solr-3.x/schema.xml");
  }
}
