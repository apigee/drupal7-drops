<?php


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
  drupal_get_messages();
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
      printf("Connect failed: %s\n", $e->getMessage());
      exit();
    }
  //}
  
  $settings = array(
    "driver" => "mysql",
    "mysql" => array(
      "host" => "localhost",
      "driver" => "mysql",
      "username" => "root",
      "password" => "",
      'database' => 'drops7',
    ),
    "save" => "Save and continue",
    "op" => "Save and continue"
  );

  $errors = install_database_errors($settings, $settings_file);
  if (count($errors) >= 1) {
    throw new \Apigee\Exceptions\InstallException($errors[0]);
  } else {
    $form_state = array('values' => $settings);
    install_settings_form_submit(array(), $form_state);
  }
  
  $install_state['completed_task'] = install_verify_completed_task();
  drupal_flush_all_caches();
  drupal_get_messages();
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
  $form = array();
  $form['devconnect_org'] = array(
    '#type' => 'textfield',
    '#title' => t("Devconnect Organization"),
    '#default_value' => "todo: get constants from Matt",
    '#description' => t('The v4 product organization name. Changing this value could make your site not work.'),
    '#required' => TRUE,
  );
  $form['devconnect_endpoint'] = array(
    '#type' => 'textfield',
    '#title' => t("Devconnect Endpoint"),
    '#default_value' => "https://api.entierprise.apigee.com/v1",
    '#description' => t('URL to which to make Apigee REST calls.'),
    '#required' => TRUE,
  );
  $form['devconnect_curlauth'] = array(
    '#type' => 'textfield',
    '#title' => t("Authentication for the Endpoint"),
    '#default_value' => "<USERNAME>:<PASSWORD>",
    '#description' => t('Will be used to authenticate with the endpoint. Separate the Username and Password with a colon (e.g. "guest:secret").'),
    '#required' => TRUE,
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
  $values = $form_state['values'];
  foreach($values as $key => $value) {
    if (substr($key, 0, 10) == "devconnect") {
      variable_set($key, $value);
    }
  }
  $install_state['completed_task'] = install_verify_completed_task();
  drupal_flush_all_caches();
  
}

