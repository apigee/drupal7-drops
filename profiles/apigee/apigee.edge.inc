<?php
/**
 * @file
 * Contains functions to configure Edge.
 */

use Drupal\devconnect\Crypto;

require_once DRUPAL_ROOT . '/profiles/apigee/modules/custom/devconnect/lib/Crypto.php';

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
    if (defined('PANTHEON_ENVIRONMENT')) {
      $org = str_replace(PANTHEON_ENVIRONMENT . '-', '', $_SERVER['HTTP_HOST']);
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
  $user = '';
  $password = '';

  drupal_load('module', 'devconnect');
  $config = devconnect_get_org_settings();
  if (!empty($config['user'])) {
    $org = $config['org'];
    $endpoint = $config['endpoint'];
    $user = $config['user'];
    $password = $config['pass'];
  }

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
    '#description' => t('URL to which to make Edge REST calls. For Apigee cloud, the value is <em>%url</em>. For on-prem installs you will need to change this value.', array('%url' => 'https://api.enterprise.apigee.com/v1')),
    '#attributes' => $attributes,
  );
  $form['user'] = array(
    '#type' => 'textfield',
    '#title' => t('Endpoint Authenticated User'),
    '#required' => TRUE,
    '#default_value' => $user,
    '#description' => t('User name used when authenticating with the endpoint. Generally this takes the form of an email address.'),
    '#attributes' => $attributes + array('placeholder' => 'username'),
  );
  $form['pass'] = array(
    '#type' => 'textfield',
    '#title' => t('Authenticated User’s Password'),
    '#required' => TRUE,
    '#default_value' => $password,
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
    '#name' => 'save',
  );
  $form['actions']['skip'] = array(
    '#type' => 'submit',
    '#limit_validation_errors' => array(),
    '#value' => t('Skip this config'),
    '#submit' => array('apigee_skip_api_endpoint'),
    '#attributes' => array(
      'style' => 'float:left;',
    ),
    '#name' => 'skip',
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
  // Short-circuit if we are skipping this step.
  if (isset($form_state['triggering_element']) && $form_state['triggering_element']['#name'] == 'skip') {
    return;
  }
  $org = trim($form_state['values']['org']);
  $endpoint = trim($form_state['values']['endpoint']);
  $user = trim($form_state['values']['user']);
  $pass = trim($form_state['values']['pass']);
  drupal_load('module', 'devconnect');
  devconnect_init();
  module_load_include('inc', 'devconnect', 'devconnect.admin');
  $return = _devconnect_test_kms_connection($org, $endpoint, $user, $pass);
  // Was connection successful?
  if (strpos($return, t('Connection Successful')) === FALSE) {
    form_set_error('form', $return);
  }
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
 *   The form being submitted.
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
 *   The form being submitted.
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
  $config['connection_timeout'] = 30;
  $config['request_timeout'] = 30;

  $private_dir = variable_get('apigee_credential_dir', NULL);
  $key = devconnect_get_crypt_key();
  Crypto::setKey($key);
  file_put_contents(DRUPAL_ROOT . '/' . $private_dir . '/.apigee', Crypto::encrypt(serialize($config)));

  // Reset the static cache.
  drupal_static_reset('devconnect_default_org_config');

  $org = new Apigee\ManagementAPI\Organization(devconnect_default_org_config());
  try {
    $org->load($config['org']);
    $is_paging_enabled = ($org->getProperty('features.isCpsEnabled') === 'true');
    variable_set('devconnect_paging_enabled', $is_paging_enabled);
  }
  catch (Exception $e) {
  }

  $install_state['parameters']['edge_configured'] = TRUE;
  $install_state['completed_task'] = install_verify_completed_task();
}
