<?php
/**
 * @file
 * User pages for Google Auth User Services.
 */

/**
 * Menu callback; Creates authenticate link for end users.
 */
function gauth_user_services_authenticate($user) {
  $page = array();
  $accounts = gauth_user_auth_services_enabled($user, FALSE);
  $page['gauth_user_services_authenticate_list'] = array(
    '#markup' => theme('gauth_user_services_authenticate_list', array('accounts' => $accounts)),
  );
  return $page;
}

/**
 * Returns HTML for the page containing the list of services accounts.
 *
 * @param array $variables
 *   An associative array containing:
 *   - accounts: An array of all the accounts returned by
 *               gauth_user_services_enabled().
 *
 * @ingroup themeable
 */
function theme_gauth_user_services_authenticate_list($variables) {
  $accounts = $variables['accounts'];
  $header = array(
    t('Name'),
  );
  $rows = array();
  foreach ($accounts as $account) {
    $row = array();
    $row[] = $account->name;
    $me = (module_exists('me') && arg(1) == 'me');
    if ($GLOBALS['user']->uid == arg(1) || $me) {
      $row[] = isset($account->is_authenticated) ? ($account->is_authenticated ? l(t('Revoke'), 'user/' . arg(1) . '/gauth_user/revoke/' . $account->id) : l(t('Authenticate'), 'gauth/response_handler/' . $account->id)) : l(t('Create and Authenticate'), 'user/' . arg(1) . '/gauth_user/add_authenticate/' . $account->id) ;
      $row[] = isset($account->is_authenticated) ? l(t('Delete'), 'user/' . arg(1) . '/gauth_user/delete/' . $account->id) : '';
      $header[] = array('data' => t('Operations'), 'colspan' => 2);
    }
    else {
      $row[] = isset($account->is_authenticated) ? ($account->is_authenticated ? 'Authenticated' : 'Un-Authenticated') : 'Not Created' ;
      $header[] = t('Status');
    }
    $rows[] = $row;
  }

  return theme('table', array('header' => $header, 'rows' => $rows));
}

/**
 * Function creates a service account of the specified type.
 */
function gauth_user_services_user_account_create($type) {
  $account = gauth_user_services_load($type, FALSE);
  $gauth_account = array(
    'client_id' => $account['client_id'],
    'client_secret' => $account['client_secret'],
    'developer_key' => $account['developer_key'],
    'services' => $account['services'],
    'name' => $type . '|' . $GLOBALS['user']->uid,
  );
  gauth_account_save($gauth_account);
  gauth_account_authenticate($type . '|' . $GLOBALS['user']->uid, TRUE);
}

/**
 * Function returns a form to confirm delete of account.
 */
function gauth_user_services_user_account_delete($form, &$form_state, $account_id) {
  $form['id'] = array(
    '#type' => 'value',
    '#value' => $account_id,
  );
  $question = check_plain(t('Are you sure you want to delete this account'));
  $path = 'user/' . arg(1) . '/gauth';
  $description = check_plain(t("This account will be deleted from the system and won't be available"));
  $yes = check_plain(t('Delete'));
  $no = check_plain(t('Cancel'));
  return confirm_form($form, $question, $path, $description, $yes, $no);
}

/**
 * Delete account form submit handler.
 */
function gauth_user_services_user_account_delete_submit($form, &$form_state) {
  if ($form_state['values']['id'] != NULL) {
    $rows = gauth_account_delete($form_state['values']['id'], FALSE);
    if ($rows == 1) {
      drupal_set_message(t("The account is deleted successfully"));
    }
    else {
      drupal_set_message(t("Error occured while deleting the account"), "error");
    }
  }
  else {
    drupal_set_message(t("Error occured: Can't find account to be deleted"), "error");
  }
  $form_state['redirect'] =  'user/' . arg(1) . '/gauth';
}

/**
 * Function returns a form to confirm revoke access token of account.
 */
function gauth_user_services_user_account_revoke($form, &$form_state, $account_id) {
  $form['id'] = array(
    '#type' => 'value',
    '#value' => $account_id,
  );
  $question = check_plain(t('Are you sure you want to revoke access token of this account'));
  $path = 'user/' . arg(1) . '/gauth';
  $description = check_plain(t("This account can't be used for api call until authenticated again"));
  $yes = check_plain(t('Revoke'));
  $no = check_plain(t('Cancel'));
  return confirm_form($form, $question, $path, $description, $yes, $no);
}

/**
 * Revoke access form submit handler
 */
function gauth_user_services_user_account_revoke_submit($form, &$form_state) {
  if ($form_state['values']['id'] != NULL) {
    $rows = gauth_account_revoke_token($form_state['values']['id'], FALSE);
    if ($rows == TRUE) {
      drupal_set_message(t("Acess token revoked successfully"));
    }
    else {
      drupal_set_message(t("Error occured while revoking token of this account"), "error");
    }
  }
  else {
    drupal_set_message(t("Error occured: Can't find account to be revoked"), "error");
  }
  $form_state['redirect'] =  'user/' . arg(1) . '/gauth';
}
