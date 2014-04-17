<?php

/**
 * Returns the user account associated with the user's partner. If user is not
 * associated with a partner, returns self.
 *
 * @param int $uid
 * @return stdClass
 */
function devconnect_partner_get_partner_account($uid) {
  $partner_id = _devconnect_partner_get_partner_uid($uid);
  if ($partner_id) {
    $uid = $partner_id;
  }
  return entity_load_single('user', $uid);
}

/**
 * Page callback for app list.
 *
 * @param stdClass $user
 * @return string
 */
function devconnect_partner_apps_getlist($user) {
  $account = devconnect_partner_get_partner_account($user->uid);
  return devconnect_developer_apps_getlist($account);
}

/**
 * Page callback for app detail.
 *
 * @param stdClass $user
 * @param string $app_name
 * @return string
 */
function devconnect_partner_apps_detail($user, $app_name) {
  $account = devconnect_partner_get_partner_account($user->uid);
  return devconnect_developer_apps_detail($account, $app_name);
}

/**
 * Form constructor for app edit form.
 *
 * @param stdClass $user
 * @param string|null $app_name
 * @return array
 */
function devconnect_partner_apps_edit_form($user, $app_name = NULL) {
  $account = devconnect_partner_get_partner_account($user->uid);
  $form = drupal_get_form('devconnect_developer_apps_edit_form', $account, $app_name);
  return $form;
}

/**
 * Form constructor for app delete form.
 *
 * @param stdClass $user
 * @param string $app_name
 * @return string
 */
function devconnect_partner_app_delete_form($user, $app_name) {
  $args = array('devconnect_developer_app_delete_form', $user, $app_name);
  try {
    return _devconnect_partner_impersonate_company_admin('drupal_get_form', $args);
  }
  catch (Exception $e) {
    drupal_set_message('Your application cannot be deleted. Perhaps one of its keys has already been used?', 'error');
  }
  return '';
}

/**
 * Menu callback for analytics download
 *
 * @param $user
 * @param $app_name
 * @param $format
 * @return mixed
 */
function devconnect_partner_analytics_performance_download($user, $app_name, $format) {
  $args = array($user, $app_name, $format);
  module_load_include('inc', 'devconnect_developer_apps', 'analytics');
  return _devconnect_partner_impersonate_company_admin('devconnect_developer_analytics_performance_download', $args);
}

/**
 * Makes a function call on behalf of the company owner
 *
 * @param callable $callback
 * @param array $args
 * @return mixed
 * @throws Exception
 */
function _devconnect_partner_impersonate_company_admin($callback, $args = array()) {
  // Try to find user arg in $args. If not found, use currently logged in user.
  $user = NULL;
  foreach ($args as $arg) {
    if (is_object($arg) && property_exists($arg, 'uid') && property_exists($arg, 'mail')) {
      $user = $arg;
      break;
    }
  }
  if (!isset($user)) {
    $user = $GLOBALS['user'];
  }
  $company = devconnect_partner_get_partner_account($user->uid);

  $old_state = drupal_save_session();
  drupal_save_session(FALSE);
  $exception = $callback_result = NULL;
  try {
    $GLOBALS['user'] = $company;
    $callback_result = call_user_func_array($callback, $args);
  }
  catch (Exception $exception) {
    // do nothing
  }
  $GLOBALS['user'] = $user;
  drupal_save_session($old_state);
  if (isset($exception)) {
    throw $exception;
  }
  return $callback_result;
}
