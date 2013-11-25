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

  $items['devconnect-update-settings'] = array(
    'callback' => 'devconnect_drush_update_settings',
    'description' => dt('Migrates org settings from the database to the filesystem.'),
    'aliases' => array('dc-settings')
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
        communicate with the KMS backend.');
    case 'drush:devconnect-get-auth':
      return dt('This command displays the username and password which DevConnect uses
        to communicate with the KMS backend.');
    case 'drush:devconnect-set-org':
      return dt('This command displays the configured Apigee organization.');
    case 'drush:devconnect-get-org':
      return dt('This command sets the Apigee organization name.');
  }
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