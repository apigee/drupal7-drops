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
    ),
    'options' => array(
      'password' => 'Unencrypted password',
    ),
    'aliases' => array('dc-setauth'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_FULL,
  );
  $items['devconnect-get-auth'] = array(
    'callback' => 'devconnect_drush_get_authentication',
    'description' => dt('Gets the username/password for KMS authentication.'),
    'aliases' => array('dc-getauth'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_FULL,
  );
  $items['devconnect-set-org'] = array(
    'callback' => 'devconnect_drush_set_org',
    'description' => dt('Sets the Apigee org name.'),
    'arguments' => array(
      'orgname' => dt('The organization name')
    ),
    'aliases' => array('dc-setorg'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_FULL,
  );
  $items['devconnect-get-org'] = array(
    'callback' => 'devconnect_drush_get_org',
    'description' => dt('Gets the configured Apigee org name.'),
    'aliases' => array('dc-getorg'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_FULL,
  );

  $items['devconnect-get-endpoint'] = array(
    'callback' => 'devconnect_drush_get_endpoint',
    'description' => dt('Gets the configured Apigee gateway management server endpoint URL.'),
    'aliases' => array('dc-getend'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_FULL,
  );
  $items['devconnect-set-endpoint'] = array(
    'callback' => 'devconnect_drush_set_endpoint',
    'description' => dt('Sets the configured Apigee gateway management server endpoint URL.'),
    'aliases' => array('dc-setend'),
    'arguments' => array(
      'endpoint' => dt('The endpoint URL')
    ),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_FULL,
  );

  $items['devconnect-get-req-timeout'] = array(
    'callback' => 'devconnect_drush_get_request_timeout',
    'description' => dt('Gets the configured Apigee gateway request timeout in seconds.'),
    'aliases' => array('dc-getrt'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_FULL,
  );
  $items['devconnect-set-req-timeout'] = array(
    'callback' => 'devconnect_drush_set_request_timeout',
    'description' => dt('Sets the configured Apigee gateway request timeout in seconds.'),
    'aliases' => array('dc-setrt'),
    'arguments' => array(
      'timeout' => dt('The timeout value (integer) in seconds')
    ),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_FULL,
  );

  $items['devconnect-get-con-timeout'] = array(
    'callback' => 'devconnect_drush_get_connect_timeout',
    'description' => dt('Gets the configured Apigee gateway connection timeout in seconds.'),
    'aliases' => array('dc-getcon'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_FULL,
  );
  $items['devconnect-set-con-timeout'] = array(
    'callback' => 'devconnect_drush_set_connect_timeout',
    'description' => dt('Sets the configured Apigee gateway connection timeout in seconds.'),
    'aliases' => array('dc-setcon'),
    'arguments' => array(
      'timeout' => dt('The timeout value (integer) in seconds')
    ),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_FULL,
  );

  $items['devconnect-update-settings'] = array(
    'callback' => 'devconnect_drush_update_settings',
    'description' => dt('Migrates org settings from the database to the filesystem.'),
    'aliases' => array('dc-settings'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_FULL,
  );


  $items['devconnect-test-conn'] = array(
    'callback' => 'devconnect_drush_test_connection',
    'description' => dt('Tests the configured connection parameters.'),
    'aliases' => array('dc-test'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_FULL,
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

/**
 * Drush command callback for devconnect-set-auth
 */
function devconnect_drush_set_authentication() {
  $args = func_get_args();
  $password = drush_get_option('password', NULL);
  if (count($args) == 1 && strpos($args[0], ':') !== FALSE) {
    list($username, $password) = explode(':', $args[0], 2);
  }
  elseif (count($args) < 1) {
    drush_set_error('Username argument is required.');
    return;
  }
  else {
    $username = $args[0];
    if (count($args) > 1 && !isset($password)) {
      $password = $args[1];
    }
  }

  if (!isset($password)) {
    $password = drush_prompt(dt('Enter @user’s password', array('@user' => $username)), NULL, TRUE, TRUE);
  }

  $config =& _devconnect_drush_get_settings();
  if (!$config) {
    return;
  }

  $old_user = $config->get('user');
  try {
    $old_pass = Apigee\Util\Crypto::decrypt($config->get('pass'));
  } catch (Apigee\Exceptions\ParameterException $e) {
    $old_pass = '';
  }
  $changes = array();
  if ($username != $old_user) {
    $changes[] = "user changed from “$old_user” to “$username”";
    $config->set('user', $username);
  }
  if ($old_pass != $password) {
    $from = (empty($old_pass) ? '*empty*' : '*encrypted*');
    $to = (empty($password) ? '*empty*' : '*encrypted*');
    $changes[] = "password changed from $from to $to";
    $config->set('pass', Apigee\Util\Crypto::encrypt($password));
  }
  if (empty($changes)) {
    drush_print(t('Username and password remain unchanged.'));
  }
  else {
    $config->save();
    $message = 'The following authentication configuration items were changed: ' . join(',', $changes);
    watchdog('devconnect', $message, array(), WATCHDOG_INFO);
    drush_print(t('Username and password have been set.'));
  }
}

/**
 * Drush command callback for devconnect-test-conn
 */
function devconnect_drush_test_connection() {
  $config =& _devconnect_drush_get_settings();
  if (!$config) {
    return;
  }
  $user = $config->get('user');
  $pass_encrypted = $config->get('pass');
  $pass = FALSE;
  if ($pass_encrypted) {
    try {
      $pass = Apigee\Util\Crypto::decrypt($pass_encrypted);
    }
    catch (Apigee\Exceptions\ParameterException $e) {}
  }
  if (empty($user) || $user == 'fixme' || empty($pass) || $pass == 'secret') {
    drush_print(dt('Username/password have not yet been set.'));
    return;
  }
  $org = $config->get('org');
  if (empty($org) || $org == 'fixme') {
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

/**
 * Drush command callback for devconnect-get-auth
 */
function devconnect_drush_get_authentication() {
  $config =& _devconnect_drush_get_settings();
  if (!$config) {
    return;
  }
  $user = $config->get('user');
  $pass_encrypted = $config->get('pass');
  $pass = FALSE;
  if ($pass_encrypted) {
    try {
      $pass = Apigee\Util\Crypto::decrypt($pass_encrypted);
    }
    catch (Apigee\Exceptions\ParameterException $e) {}
  }
  if (empty($user) || $user == 'fixme' || empty($pass) || $pass == 'secret') {
    drush_print(dt('Username/password have not yet been set.'));
  }
  else {
    drush_print(dt('Username is @username, password is @pass.', array('@username' => $user, '@pass' => $pass)));
  }
}

/**
 * Drush command callback for devconnect-set-org
 */
function devconnect_drush_set_org() {
  $args = func_get_args();
  if (count($args) < 1) {
    drush_set_error('Orgname argument is required.');
    return;
  }
  list($org) = $args;
  $config =& _devconnect_drush_get_settings();
  if (!$config) {
    return;
  }
  $old_org = $config->get('org');
  if ($org != $old_org) {
    $config->set('org', $org);
    $config->save();
    watchdog('devconnect', 'The following authentication configuration item was changed: org was changed from “%old” to “%new”.', array('%old' => $old_org, '%new' => $org), WATCHDOG_INFO);
    drush_print(dt('Orgname has been set to @org', array('@org' => $org)));
  }
  else {
    drush_print(dt('Orgname @org remains unchanged.', array('@org' => $org)));
  }
}

/**
 * Drush command callback for devconnect-get-org
 */
function devconnect_drush_get_org() {
  $config =& _devconnect_drush_get_settings();
  if (!$config) {
    return;
  }
  $org = $config->get('org');
  if (empty($org) || $org == 'fixme') {
    drush_print(dt('Org name has not yet been set.'));
  }
  else {
    drush_print(dt('Org name is @orgname', array('@orgname' => $org)));
  }
}

/**
 * Drush command callback for devconnect-get-endpoint
 */
function devconnect_drush_get_endpoint() {
  $config =& _devconnect_drush_get_settings();
  if (!$config) {
    return;
  }
  $endpoint = $config->get('endpoint');
  drush_print(dt('Endpoint is @endpoint', array('@endpoint' => $endpoint)));
}

/**
 * Drush command callback for devconnect-set-endpoint
 */
function devconnect_drush_set_endpoint() {
  $args = func_get_args();
  if (count($args) < 1) {
    drush_set_error('Endpoint argument is required.');
    return;
  }
  list($endpoint) = $args;
  $config =& _devconnect_drush_get_settings();
  if (!$config) {
    return;
  }
  $old_endpoint = $config->get('endpoint');
  if ($endpoint != $old_endpoint) {
    $config->set('org', $endpoint);
    $config->save();
    watchdog('devconnect', 'The following authentication configuration item was changed: endpoint was changed from “%old” to “%new”.', array('%old' => $old_endpoint, '%new' => $endpoint), WATCHDOG_INFO);
    drush_print(dt('Endpoint has been set to @endpoint', array('@endpoint' => $endpoint)));
  }
  else {
    drush_print(dt('Endpoint @endpoint remains unchanged.', array('@endpoint' => $endpoint)));
  }
}

/**
 * Drush command callback for devconnect-update-settings
 */
function devconnect_drush_update_settings() {
  module_load_install('devconnect');
  devconnect_port_settings_to_d8cmi();
  drush_print(dt('Settings have been moved to the filesystem.'));
}

/**
 * Drush command callback for devconnect-get-req-timeout
 */
function devconnect_drush_get_request_timeout() {
  $config =& _devconnect_drush_get_settings();
  if (!$config) {
    return;
  }
  $timeout = $config->get('request_timeout');
  drush_print(dt('Request timeout is @sec seconds', array('@sec' => $timeout)));
}

/**
 * Drush command callback for devconnect-set-req-timeout
 */
function devconnect_drush_set_request_timeout() {
  $args = func_get_args();
  if (count($args) < 1) {
    drush_set_error('Timeout argument is required.');
    return;
  }
  list($timeout) = $args;
  $config =& _devconnect_drush_get_settings();
  if (!$config) {
    return;
  }
  if ($timeout != $config->get('request_timeout')) {
    $config->set('request_timeout', $timeout);
    $config->save();
    drush_print(dt('Request timeout has been set to @sec seconds', array('@sec' => $timeout)));

  }
  else {
    drush_print(dt('Request timeout @sec remains unchanged.', array('@sec' => $timeout)));
  }
}

/**
 * Drush command callback for devconnect-get-con-timeout
 */
function devconnect_drush_get_connect_timeout() {
  $config =& _devconnect_drush_get_settings();
  if (!$config) {
    return;
  }
  $timeout = $config->get('connection_timeout');
  drush_print(dt('Connection timeout is @sec seconds', array('@sec' => $timeout)));
}

/**
 * Drush command callback for devconnect-set-con-timeout
 */
function devconnect_drush_set_connect_timeout() {
  $args = func_get_args();
  if (count($args) < 1) {
    drush_set_error('Timeout argument is required.');
    return;
  }
  list($timeout) = $args;
  $config =& _devconnect_drush_get_settings();
  if (!$config) {
    return;
  }
  if ($timeout != $config->get('connection_timeout')) {
    $config->set('connection_timeout', $timeout);
    $config->save();
    drush_print(dt('Connection timeout has been set to @sec seconds', array('@sec' => $timeout)));
  }
  else {
    drush_print(dt('Connection timeout @sec remains unchanged.', array('@sec' => $timeout)));
  }
}

function _devconnect_drush_get_settings() {
  try {
    return Drupal::config('devconnect.settings');
  } catch (Exception $e) {
    drush_set_error('DEVCONNECT_ERROR', $e->getMessage());
    return FALSE;
  }
}