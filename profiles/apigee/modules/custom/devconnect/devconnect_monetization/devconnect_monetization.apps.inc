<?php

function devconnect_monetization_get_company_account($user_id) {
  $account = entity_load_single('user', $user_id);
  $company_id = !empty($account->field_mint_company_id[LANGUAGE_NONE][0]['value']) ? $account->field_mint_company_id[LANGUAGE_NONE][0]['value'] : NULL;
  if (valid_email_address($company_id)) {
    $user = user_load_by_mail($company_id);
    $account = entity_load_single('user', $user->uid);
  }
  return $account;
}

function devconnect_monetization_company_apps_getlist() {
  $account = devconnect_monetization_get_company_account($GLOBALS['user']->uid);
  return devconnect_developer_apps_getlist($account);
}

function devconnect_monetization_company_apps_detail($user, $app_name) {
  $account = devconnect_monetization_get_company_account($GLOBALS['user']->uid);
  return devconnect_developer_apps_detail($account, $app_name);
}

function devconnect_monetization_company_apps_edit_form($user, $app_name = NULL) {
  $account = devconnect_monetization_get_company_account($GLOBALS['user']->uid);
  $form = drupal_get_form('devconnect_developer_apps_edit_form', $account, $app_name);
  return $form;
}

function devconnect_monetization_company_app_delete_form($user, $app_id) {
  $args = array('devconnect_developer_apps_delete_form', $user, $app_id);
  try {
    return _devconnect_monetization_impersonate_company_admin('drupal_get_form', $args);
  }
  catch (Exception $e) {
    drupal_set_message(t('Your application can not be deleted, Perhaps one of its key has already been used'), 'error');
  }
  return '';
}

function devconnect_monetization_company_analytics_performance_download($user, $app_name, $format) {
  $args = array('user_arg', $app_name, $format);
  module_load_include('inc', 'devconnect_developer_apps', 'analytics');
  return _devconnect_monetization_impersonate_company_admin('devconnect_developer_analytics_performance_download', $args);
}

function _devconnect_monetization_impersonate_company_admin($callback, $args = array()) {
  global $user;

  $account = entity_load_single('user', $GLOBALS['user']->uid);
  $email = !empty($account->field_mint_company_id[LANGUAGE_NONE][0]['value']) ? $account->field_mint_company_id[LANGUAGE_NONE][0]['value'] : NULL;
  if($email == null) {
    $email = $account->mail;
  }
  if (valid_email_address($email)) {

    $original_user = $user;
    $old_state = drupal_save_session();
    drupal_save_session(FALSE);
    $user = user_load_by_mail($email);
    $user_arg_index = array_search('user_arg', $args);
    if ($user_arg_index !== FALSE) {
      $args[$user_arg_index] = $user;
    }
    $callback_result = call_user_func_array($callback, $args);
    $user = $original_user;
    drupal_save_session($old_state);
  }
  else {
    $user_arg_index = array_search('user_arg', $args);
    if ($user_arg_index !== FALSE) {
      $args[$user_arg_index] = $user;
    }
    $callback_result = call_user_func_array($callback, $args);
  }
  return $callback_result;
}
