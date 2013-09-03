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

  // Make sure autoloader is loaded
  //drupal_load('module', 'devconnect');
  //devconnect_boot();
  if (!class_exists('Apigee\Util\Crypto')) {
    module_load_include('php', 'devconnect', 'lib/Apigee/Util/Crypto');
  }

  $password_encrypted = Apigee\Util\Crypto::encrypt($password);
  variable_set('devconnect_curlauth', "$username:$password_encrypted");
  drush_print(t('Username and password have been set.'));
}

/**
 * Autoloader for Apigee and Symfony\Components\Yaml classes
 *
 * @param string $class
 */
function _devconnect_autoload($class) {
  $class_path = NULL;
  if (strpos($class, '\\') !== FALSE) {
    $path_parts = explode('\\', $class);
    if ($path_parts[0] == 'Apigee') {
      $lib_path = realpath(dirname(__FILE__) . '/lib');
      $class_path = $lib_path . '/' . join('/', $path_parts) . '.php';
    }
    elseif (count($path_parts) > 3 && $path_parts[0] == 'Symfony' && $path_parts[1] == 'Components' && $path_parts[2] == 'Yaml') {
      $path_parts = array_slice($path_parts, 3);
      $lib_path = libraries_get_path('symfony_yaml');
      if ($lib_path) {
        $class_path = $lib_path . '/' . join('/', $path_parts) . '.php';
      }
    }
  }
  if (isset($class_path) && file_exists($class_path)) {
    require_once($class_path);
  }
}

function devconnect_drush_get_authentication() {
  $user_pass = variable_get('devconnect_curlauth', NULL);
  if (!isset($user_pass) || strpos($user_pass, ':') === FALSE) {
    drush_print(t('Username/password have not yet been set.'));
  }
  else {
    list($user, $pass_encrypted) = explode(':', $user_pass);
    // Make sure autoloader is loaded
    if (!class_exists('Apigee\Util\Crypto')) {
      module_load_include('php', 'devconnect', 'lib/Apigee/Util/Crypto');
    }
    $pass = Apigee\Util\Crypto::decrypt($pass_encrypted);

    drush_print(t('Username is @username, password is @pass.', array('@username' => $user, '@pass' => $pass)));
  }
}

function devconnect_drush_set_org() {
  $args = func_get_args();
  if (count($args) < 1) {
    drush_set_error('Orgname argument is required.');
    return;
  }
  list($org) = $args;
  variable_set('devconnect_org', $org);
  drush_print(t('Orgname has been set to @org', array('@org' => $org)));
}

function devconnect_drush_get_org() {
  $org = variable_get('devconnect_org', NULL);
  if (!isset($org)) {
    drush_print(t('Org name has not yet been set.'));
  }
  else {
    drush_print(t('Org name is @orgname', array('@orgname' => $org)));
  }
}
