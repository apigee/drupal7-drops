<?php
/**
 * @file
 * Contains install steps for the Apigee profile.
 */

require_once DRUPAL_ROOT . '/profiles/apigee/apigee.caches.inc';
require_once DRUPAL_ROOT . '/profiles/apigee/apigee.configure.inc';
require_once DRUPAL_ROOT . '/profiles/apigee/apigee.content.inc';
require_once DRUPAL_ROOT . '/profiles/apigee/apigee.edge.inc';
require_once DRUPAL_ROOT . '/profiles/apigee/apigee.smartdocs.inc';

define('OFAC_SDN_ENDPOINT', 'https://baas-devportal.apigee.net/apigee-devportal/ofac-sdn-validation/individuals');

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
  if (defined('PANTHEON_ENVIRONMENT')) {
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

  $batch = array(
    'title' => t('Configuring your install...'),
    'operations' => array(
      // The following functions are in apigee.configure.inc.
      array('apigee_install_configure_variables', array()),
      array('apigee_install_configure_search', array()),
      array('apigee_install_configure_users', array()),
      array('apigee_install_configure_themes', array()),
      array('apigee_install_content_types', array()),
      array('apigee_install_enable_blog_content_types', array()),
      array('apigee_install_rebuild_permissions', array()),
      array('apigee_install_base_ckeditor_settings', array()),
      array('apigee_install_create_environmental_indicators', array()),
      // The following functions are in apigee.content.inc.
      array('apigee_install_create_homepage', array()),
      array('apigee_install_create_taxonomy_terms', array()),
      array('apigee_install_create_tutorial_content', array()),
      array('apigee_install_create_forum_content', array()),
      array('apigee_install_create_page_content', array()),
      array('apigee_install_create_faq_content', array()),
    ),
    'finished' => '_apigee_install_configure_task_finished',
  );

  // The following functions are in apigee.caches.inc.
  $batch['operations'][] = array('apigee_install_clear_caches_flush', array());
  $batch['operations'][] = array('apigee_install_clear_caches_css', array());
  $batch['operations'][] = array('apigee_install_clear_caches_js', array());
  $batch['operations'][] = array('apigee_install_clear_caches_theme', array());
  $batch['operations'][] = array('apigee_install_clear_caches_entity', array());
  $batch['operations'][] = array('apigee_install_clear_caches_nodes', array());
  $batch['operations'][] = array('apigee_install_clear_caches_menu', array());
  $batch['operations'][] = array('apigee_install_clear_caches_actions', array());
  $batch['operations'][] = array('apigee_install_clear_caches_core_path', array());
  $batch['operations'][] = array('apigee_install_clear_caches_core_filter', array());
  $batch['operations'][] = array('apigee_install_clear_caches_core_bootstrap', array());
  $batch['operations'][] = array('apigee_install_clear_caches_core_page', array());
  $batch['operations'][] = array('apigee_install_clear_caches_core', array());
  $batch['operations'][] = array('apigee_install_bootstrap_status', array());

  return $batch;
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
  // If we are using Drush, then read in the admin-pass option passed to
  // 'drush site-install'.
  if (function_exists('drush_get_option')) {
    $admin_pass = drush_get_option('admin-pass', drush_generate_password());
    watchdog('apigee_install', 'Drush detected, setting administrator password from the --admin-pass option.', array(), WATCHDOG_INFO);
  }
  else {
    $admin_pass = NULL;
  }

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
  // If we are using drush site-install, then change this field to a textfield
  // and put in the admin password.  If we use password_confirm field we are
  // not able to set the field when using drush.  To set this field during
  // 'drush site-install' pass the --admin-pass option.
  $form['pass'] = array(
    '#type' => (!$admin_pass) ? 'password_confirm' : 'textfield',
    '#title' => t('Developer Portal Password'),
    '#required' => TRUE,
    '#description' => t('An admin password used when logging into the Developer Portal.'),
    '#attributes' => $attributes,
    '#pre_render' => array('apigee_password_pre_render'),
  );

  // If we are using texfield, add textfield attributes.
  if ($form['pass']['#type'] == 'textfield') {
    $form['pass']['#default_value'] = $admin_pass;
  }

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
    form_set_error('username', t('Please select a different username.'));
  }
  if (!valid_email_address($form_state['values']['emailaddress'])) {
    form_set_error('emailaddress', t('Please select a valid email address.'));
  }
  if (apigee_install_create_admin_user_is_sdn_match($form_state['values']['firstname'], $form_state['values']['lastname'])) {
    form_set_error('', t('This name cannot be used as an administrator account. Please contact Apigee support for more details.'));
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
  if (!defined('PANTHEON_ENVIRONMENT')) {
    return FALSE;
  }

  $url = OFAC_SDN_ENDPOINT . "?ql=" . urlencode("firstName='" . $first_name . "' AND lastName='" . $last_name . "'");
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

  if (is_array($response) && array_key_exists('count', $response)) {
    return (bool) $response['count'];
  }

  // The system could not check the SDN list, let Dev Portal team know.
  $my_module = 'sdn_check_error';
  $my_mail_token = 'apigee_profile';
  $from = variable_get('system_mail', 'noreply@apigee.com');

  $http_response_string = '';
  if (is_array($response)) {
    foreach ($response as $response_key => $response_value) {
      $http_response_string .= "$response_key => $response_value ";
    }
  }

  $pantheon_site_name = '';
  if (array_key_exists('PANTHEON_SITE_NAME', $_SERVER) && defined('PANTHEON_ENVIRONMENT')) {
    $pantheon_site_name = $_SERVER['PANTHEON_SITE_NAME'] . '.' . PANTHEON_ENVIRONMENT;
  }
  $pantheon_site_uuid = '';
  if (defined('PANTHEON_SITE')) {
    $pantheon_site_uuid = PANTHEON_SITE;
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
    watchdog('apigee_install', "SDN Validation error email NOT sent. !body", array('!body' => implode(' ', $message_body)), WATCHDOG_WARNING);
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
 * Batch callback to verify we meet minimum requirements.
 */
function apigee_install_check_postrequisites() {
  if (!extension_loaded('openssl')) {
    drupal_set_message(st('The OpenSSL PHP extension is required.'), 'error');
  }
}

/**
 * Boosts PHP's memory and execution time for large-capacity batch processes.
 */
function _apigee_manage_memory() {
  ini_set('memory_limit', '1024M');
  ini_set('max_execution_time', 300);
}
