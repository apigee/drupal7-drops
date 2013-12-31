<?php

/**
 * Implements hook_drush_command().
 *
 * @return array
 */
function devconnect_drush_command() {
  $items = array();

  $items['devconnect-set-auth'] = array(
    'callback' => 'devconnect_drush_set_authentication',
    'description' => dt('Sets the username/password for KMS authentication.'),
    'arguments' => array(
      'username' => dt('The Apigee username'),
      'password' => dt('The password (unencrypted)')
    ),
    'aliases' => array('dc-setauth')
  );
  $items['devconnect-get-auth'] = array(
    'callback' => 'devconnect_drush_get_authentication',
    'description' => dt('Gets the username/password for KMS authentication.'),
    'aliases' => array('dc-getauth')
  );
  $items['devconnect-set-org'] = array(
    'callback' => 'devconnect_drush_set_org',
    'description' => dt('Sets the Apigee org name.'),
    'arguments' => array(
      'orgname' => dt('The organization name')
    ),
    'aliases' => array('dc-setorg')
  );
  $items['devconnect-get-org'] = array(
    'callback' => 'devconnect_drush_get_org',
    'description' => dt('Gets the configured Apigee org name.'),
    'aliases' => array('dc-getorg')
  );

  $items['devconnect-get-endpoint'] = array(
    'callback' => 'devconnect_drush_get_endpoint',
    'description' => dt('Gets the configured Apigee gateway management server endpoint URL.'),
    'aliases' => array('dc-getend')
  );
  $items['devconnect-set-endpoint'] = array(
    'callback' => 'devconnect_drush_set_endpoint',
    'description' => dt('Sets the configured Apigee gateway management server endpoint URL.'),
    'aliases' => array('dc-setend'),
    'arguments' => array(
      'endpoint' => dt('The endpoint URL')
    ),
  );

  $items['devconnect-get-req-timeout'] = array(
    'callback' => 'devconnect_drush_get_request_timeout',
    'description' => dt('Gets the configured Apigee gateway request timeout in seconds.'),
    'aliases' => array('dc-getrt')
  );
  $items['devconnect-set-req-timeout'] = array(
    'callback' => 'devconnect_drush_set_request_timeout',
    'description' => dt('Sets the configured Apigee gateway request timeout in seconds.'),
    'aliases' => array('dc-setrt'),
    'arguments' => array(
      'timeout' => dt('The timeout value (integer) in seconds')
    ),
  );

  $items['devconnect-get-con-timeout'] = array(
    'callback' => 'devconnect_drush_get_connect_timeout',
    'description' => dt('Gets the configured Apigee gateway connection timeout in seconds.'),
    'aliases' => array('dc-getcon')
  );
  $items['devconnect-set-con-timeout'] = array(
    'callback' => 'devconnect_drush_set_connect_timeout',
    'description' => dt('Sets the configured Apigee gateway connection timeout in seconds.'),
    'aliases' => array('dc-setcon'),
    'arguments' => array(
      'timeout' => dt('The timeout value (integer) in seconds')
    ),
  );

  $items['devconnect-update-settings'] = array(
    'callback' => 'devconnect_drush_update_settings',
    'description' => dt('Migrates org settings from the database to the filesystem.'),
    'aliases' => array('dc-settings')
  );


  $items['devconnect-test-conn'] = array(
    'callback' => 'devconnect_drush_test_connection',
    'description' => dt('Tests the configured connection parameters.'),
    'aliases' => array('dc-test')
  );


  return $items;
}

/**
 * Implements hook_drush_help().
 *
 * @param $section
 * @return null|string
 */
function devconnect_drush_help($section) {
  switch ($section) {
    case 'drush:devconnect-set-auth':
      return dt('This command sets the username and password which DevConnect uses to
        communicate with the Apigee gateway management server backend.');
    case 'drush:devconnect-get-auth':
      return dt('This command displays the username and password which DevConnect uses
        to communicate with the Apigee gateway management server backend.');
    case 'drush:devconnect-set-org':
      return dt('This command displays the configured Apigee organization.');
    case 'drush:devconnect-get-org':
      return dt('This command sets the Apigee organization name.');
    case 'drush:devconnect-set-endpoint':
      return dt('This command sets the Apigee gateway management server endpoint URL.');
    case 'drush:devconnect-get-endpoint':
      return dt('This command displays the Apigee gateway management server endpoint URL.');
    case 'drush:devconnect-update-settings':
      return dt('This command migrates Drupal-var devconnect variables to a YAML file in the filesystem.');
    case 'drush:devconnect-test-conn':
      return dt('This command tests the configured connection parameters.');
  }
  return NULL;
}

function devconnect_drush_set_authentication() {
  $args = func_get_args();
  if (count($args) == 1 && strpos($args[0], ':') !== FALSE) {
    list($username, $password) = explode(':', $args[0], 2);
  }
  elseif (count($args) < 2) {
    drush_set_error('Username and password arguments are required.');
    return;
  }
  else {
    list($username, $password) = $args;
  }

  $config = _devconnect_drush_get_config();
  $password_encrypted = Apigee\Util\Crypto::encrypt($password);
  $config->set('user', $username);
  $config->set('pass', $password_encrypted);
  $config->save();
  drush_print(t('Username and password have been set.'));
}

function devconnect_drush_test_connection() {
  $config = _devconnect_drush_get_config(); // Load modules
  $user = $config->get('user');
  $pass_encrypted = $config->get('pass');
  $pass = FALSE;
  if ($pass_encrypted) {
    $pass = Apigee\Util\Crypto::decrypt($pass_encrypted);
  }
  if (!$pass) {
    $pass = 'secret';
  }
  if ($user == 'fixme' || $pass == 'secret') {
    drush_print(dt('Username/password have not yet been set.'));
    return;
  }
  $org = $config->get('org');
  if ($org == 'fixme') {
    drush_print(dt('Org name has not yet been set.'));
    return;
  }
  $endpoint = $config->get('endpoint');

  $ch = curl_init();
  curl_setopt($ch, CURLOPT_URL, "$endpoint/o/$org");
  curl_setopt($ch, CURLOPT_RETURNTRANSFER, TRUE);
  curl_setopt($ch, CURLOPT_FOLLOWLOCATION, TRUE);
  curl_setopt($ch, CURLOPT_TIMEOUT, 4);
  curl_setopt($ch, CURLOPT_USERPWD, "$user:$pass");
  curl_setopt($ch, CURLOPT_HTTPHEADER, array('Accept: application/json'));

  curl_exec($ch);
  $response_code = curl_getinfo($ch, CURLINFO_HTTP_CODE);
  if ($response_code == 200) {
    drush_print(dt('Connection to the endpoint succeeded.'));
  }
  else {
    drush_print(dt('Connection to the endpoint failed.'));
  }
}

function devconnect_drush_get_authentication() {
  $config = _devconnect_drush_get_config();
  $user = $config->get('user');
  $pass_encrypted = $config->get('pass');
  $pass = FALSE;
  if ($pass_encrypted) {
    $pass = Apigee\Util\Crypto::decrypt($pass_encrypted);
  }
  if (!$pass) {
    $pass = 'secret';
  }
  if ($user == 'fixme' || $pass == 'secret') {
    drush_print(dt('Username/password have not yet been set.'));
  }
  else {
    drush_print(dt('Username is @username, password is @pass.', array('@username' => $user, '@pass' => $pass)));
  }
}

function devconnect_drush_set_org() {
  $args = func_get_args();
  if (count($args) < 1) {
    drush_set_error('Orgname argument is required.');
    return;
  }
  list($org) = $args;
  $config = _devconnect_drush_get_config();
  if ($org != $config->get('org')) {
    $config->set('org', $org);
    $config->save();
    drush_print(dt('Orgname has been set to @org', array('@org' => $org)));
  }
  else {
    drush_print(dt('Orgname @org remains unchanged.', array('@org' => $org)));
  }
}

function devconnect_drush_get_org() {
  $config = _devconnect_drush_get_config();
  $org = $config->get('org');
  if ($org == 'fixme') {
    drush_print(dt('Org name has not yet been set.'));
  }
  else {
    drush_print(dt('Org name is @orgname', array('@orgname' => $org)));
  }
}

function devconnect_drush_get_endpoint() {
  $config = _devconnect_drush_get_config();
  $endpoint = $config->get('endpoint');
  drush_print(dt('Endpoint is @endpoint', array('@endpoint' => $endpoint)));
}

function devconnect_drush_set_endpoint() {
  $args = func_get_args();
  if (count($args) < 1) {
    drush_set_error('Endpoint argument is required.');
    return;
  }
  list($endpoint) = $args;
  $config = _devconnect_drush_get_config();
  if ($endpoint != $config->get('endpoint')) {
    $config->set('endpoint', $endpoint);
    $config->save();
    drush_print(dt('Endpoint has been set to @endpoint', array('@endpoint' => $endpoint)));
  }
  else {
    drush_print(dt('Endpoint @endpoint remains unchanged.', array('@endpoint' => $endpoint)));
  }
}


function devconnect_drush_update_settings() {
  module_load_install('devconnect');
  devconnect_port_settings_to_d8cmi();
  drush_print(dt('Settings have been moved to the filesystem.'));
}

function _devconnect_drush_get_config() {
  drupal_load('module', 'devconnect');
  drupal_load('module', 'd8cmi');
  devconnect_boot();
  d8cmi_init();
  return Drupal::config('devconnect.settings');
}

function devconnect_drush_get_request_timeout() {
  $config = _devconnect_drush_get_config();
  $timeout = $config->get('request_timeout');
  drush_print(dt('Request timeout is @sec seconds', array('@sec' => $timeout)));
}
function devconnect_drush_set_request_timeout() {
  $args = func_get_args();
  if (count($args) < 1) {
    drush_set_error('Timeout argument is required.');
    return;
  }
  list($timeout) = $args;
  $config = _devconnect_drush_get_config();
  if ($timeout != $config->get('request_timeout')) {
    $config->set('request_timeout', $timeout);
    $config->save();
    drush_print(dt('Request timeout has been set to @sec seconds', array('@sec' => $timeout)));

  }
  else {
    drush_print(dt('Request timeout @sec remains unchanged.', array('@sec' => $timeout)));
  }
}

function devconnect_drush_get_connect_timeout() {
  $config = _devconnect_drush_get_config();
  $timeout = $config->get('connection_timeout');
  drush_print(dt('Connection timeout is @sec seconds', array('@sec' => $timeout)));
}
function devconnect_drush_set_connect_timeout() {
  $args = func_get_args();
  if (count($args) < 1) {
    drush_set_error('Timeout argument is required.');
    return;
  }
  list($timeout) = $args;
  $config = _devconnect_drush_get_config();
  if ($timeout != $config->get('connection_timeout')) {
    $config->set('connection_timeout', $timeout);
    $config->save();
    drush_print(dt('Connection timeout has been set to @sec seconds', array('@sec' => $timeout)));

  }
  else {
    drush_print(dt('Connection timeout @sec remains unchanged.', array('@sec' => $timeout)));
  }
}