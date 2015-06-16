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
    'description' => dt('Sets the username/password for Edge authentication.'),
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
    'description' => dt('Gets the username/password for Edge authentication.'),
    'aliases' => array('dc-getauth'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_FULL,
  );
  $items['devconnect-set-org'] = array(
    'callback' => 'devconnect_drush_set_org',
    'description' => dt('Sets the Edge org name.'),
    'arguments' => array(
      'orgname' => dt('The organization name')
    ),
    'aliases' => array('dc-setorg'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_FULL,
  );
  $items['devconnect-get-org'] = array(
    'callback' => 'devconnect_drush_get_org',
    'description' => dt('Gets the configured Edge org name.'),
    'aliases' => array('dc-getorg'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_FULL,
  );

  $items['devconnect-get-endpoint'] = array(
    'callback' => 'devconnect_drush_get_endpoint',
    'description' => dt('Gets the configured Edge endpoint URL.'),
    'aliases' => array('dc-getend'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_FULL,
  );
  $items['devconnect-set-endpoint'] = array(
    'callback' => 'devconnect_drush_set_endpoint',
    'description' => dt('Sets the configured Edge endpoint URL.'),
    'aliases' => array('dc-setend'),
    'arguments' => array(
      'endpoint' => dt('The endpoint URL')
    ),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_FULL,
  );

  $items['devconnect-get-req-timeout'] = array(
    'callback' => 'devconnect_drush_get_request_timeout',
    'description' => dt('Gets the configured Edge request timeout in seconds.'),
    'aliases' => array('dc-getrt'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_FULL,
  );
  $items['devconnect-set-req-timeout'] = array(
    'callback' => 'devconnect_drush_set_request_timeout',
    'description' => dt('Sets the configured Edge request timeout in seconds.'),
    'aliases' => array('dc-setrt'),
    'arguments' => array(
      'timeout' => dt('The timeout value (integer) in seconds')
    ),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_FULL,
  );

  $items['devconnect-get-con-timeout'] = array(
    'callback' => 'devconnect_drush_get_connect_timeout',
    'description' => dt('Gets the configured Edge connection timeout in seconds.'),
    'aliases' => array('dc-getcon'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_FULL,
  );
  $items['devconnect-set-con-timeout'] = array(
    'callback' => 'devconnect_drush_set_connect_timeout',
    'description' => dt('Sets the configured Edge connection timeout in seconds.'),
    'aliases' => array('dc-setcon'),
    'arguments' => array(
      'timeout' => dt('The timeout value (integer) in seconds')
    ),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_FULL,
  );

  $items['devconnect-test-conn'] = array(
    'callback' => 'devconnect_drush_test_connection',
    'description' => dt('Tests the configured Edge connection parameters.'),
    'aliases' => array('dc-test'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_FULL,
  );

  $items['devconnect-sample-smartdocs'] = array(
    'callback' => 'devconnect_drush_generate_smartdocs_sample',
    'description' => dt('Generates sample SmartDocs content.'),
    'aliases' => array('dc-smartdocs'),
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
      return dt('This command sets the username and password which Dev Portal uses to communicate with the Edge backend.');
    case 'drush:devconnect-get-auth':
      return dt('This command displays the username and password which Dev Portal uses to communicate with the Edge backend.');
    case 'drush:devconnect-set-org':
      return dt('This command displays the configured Edge organization.');
    case 'drush:devconnect-get-org':
      return dt('This command sets the Edge organization name.');
    case 'drush:devconnect-set-endpoint':
      return dt('This command sets the Edge endpoint URL.');
    case 'drush:devconnect-get-endpoint':
      return dt('This command displays the Edge endpoint URL.');
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

  $config =& devconnect_get_org_settings();
  if (!$config) {
    return;
  }

  $old_user = $config['user'];
  $old_pass = $config['pass'];
  $changes = array();
  if ($username != $old_user) {
    $changes[] = "user changed from “$old_user” to “$username”";
    $config['user'] = $username;
  }
  if ($old_pass != $password) {
    $from = (empty($old_pass) ? '*empty*' : '*encrypted*');
    $to = (empty($password) ? '*empty*' : '*encrypted*');
    $changes[] = "password changed from $from to $to";
    $config['pass'] = $password;
  }
  if (empty($changes)) {
    drush_print(t('Username and password remain unchanged.'));
  }
  else {
    _devconnect_drush_save_config($config);
    $message = 'The following authentication configuration items were changed: ' . join(',', $changes);
    watchdog('devconnect', $message, array(), WATCHDOG_INFO);
    drush_print(t('Username and password have been set.'));
  }
}

/**
 * Drush command callback for devconnect-test-conn
 */
function devconnect_drush_test_connection() {
  $config =& devconnect_get_org_settings();
  if (!$config) {
    return;
  }
  $user = $config['user'];
  $pass = $config['pass'];
  if (empty($user) || $user == 'fixme' || empty($pass) || $pass == 'secret') {
    drush_print(dt('Username/password have not yet been set.'));
    return;
  }
  $org = $config['org'];
  if (empty($org) || $org == 'fixme') {
    drush_print(dt('Org name has not yet been set.'));
    return;
  }
  $endpoint = $config['endpoint'];

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
  $config =& devconnect_get_org_settings();
  if (!$config) {
    return;
  }
  $user = $config['user'];
  $pass = $config['pass'];
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
  $config =& devconnect_get_org_settings();
  if (!$config) {
    return;
  }
  $old_org = $config['org'];
  if ($org != $old_org) {
    $config['org'] = $org;
    _devconnect_drush_save_config($config);
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
  $config =& devconnect_get_org_settings();
  if (!$config) {
    return;
  }
  $org = $config['org'];
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
  $config =& devconnect_get_org_settings();
  if (!$config) {
    return;
  }
  $endpoint = $config['endpoint'];
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
  $config =& devconnect_get_org_settings();
  if (!$config) {
    return;
  }
  $old_endpoint = $config['endpoint'];
  if ($endpoint != $old_endpoint) {
    $config['endpoint'] = $endpoint;
    _devconnect_drush_save_config($config);
    watchdog('devconnect', 'The following authentication configuration item was changed: endpoint was changed from “%old” to “%new”.', array('%old' => $old_endpoint, '%new' => $endpoint), WATCHDOG_INFO);
    drush_print(dt('Endpoint has been set to @endpoint', array('@endpoint' => $endpoint)));
  }
  else {
    drush_print(dt('Endpoint @endpoint remains unchanged.', array('@endpoint' => $endpoint)));
  }
}


/**
 * Drush command callback for devconnect-get-req-timeout
 */
function devconnect_drush_get_request_timeout() {
  $config =& devconnect_get_org_settings();
  if (!$config) {
    return;
  }
  $timeout = $config['request_timeout'];
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
  $config =& devconnect_get_org_settings();
  if (!$config) {
    return;
  }
  if ($timeout != $config['request_timeout']) {
    $config['request_timeout'] = $timeout;
    _devconnect_drush_save_config($config);
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
  $config =& devconnect_get_org_settings();
  if (!$config) {
    return;
  }
  $timeout = $config['connection_timeout'];
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
  $config =& devconnect_get_org_settings();
  if (!$config) {
    return;
  }
  if ($timeout != $config['connection_timeout']) {
    $config['connection_timeout'] = $timeout;
    _devconnect_drush_save_config($config);
    drush_print(dt('Connection timeout has been set to @sec seconds', array('@sec' => $timeout)));
  }
  else {
    drush_print(dt('Connection timeout @sec remains unchanged.', array('@sec' => $timeout)));
  }
}

function _devconnect_drush_save_config($config) {
  $private_dir = variable_get('apigee_credential_dir', 'sites/default/files/private');
  if (!is_dir(DRUPAL_ROOT . '/' . $private_dir)) {
    drush_log(dt('Your filesystem is misconfigured. Please set the ‘@var’ variable to a directory that is writable.', array('@var' =>'apigee_credential_dir')), 'error');
    return;
  }
  $config_file = DRUPAL_ROOT . '/' . $private_dir . '/.apigee';
  if (file_exists($config_file)) {
    if (!is_writable($config_file)) {
      drush_log(dt('The config file ‘@file’ is not writable.', array('@file' => $private_dir . '/.apigee')), 'error');
      return;
    }
  }
  elseif (!is_writable(dirname($config_file))) {
    drush_log(dt('The config directory ‘@dir’ is not writable.', array('@dir' => $private_dir)), 'error');
    return;
  }
  file_put_contents($config_file, Drupal\devconnect\Crypto::encrypt(serialize($config)));
}

/**
 * Drush command callback for devconnect-sample-smartdocs
 */
function devconnect_drush_generate_smartdocs_sample() {
  $profile = variable_get('install_profile', NULL);
  if ($profile != 'apigee') {
    drush_log(dt('SmartDocs can only be generated when the install profile is Apigee.'), 'error');
  }
  drupal_load('profile', 'apigee');
  apigee_smartdocs_import_model_content();
  $batch = apigee_smartdocs_render_model_content();
  $batch['progressive'] = FALSE;
  batch_set($batch);
  drush_backend_batch_process();
}
