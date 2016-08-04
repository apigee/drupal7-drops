<?php
/**
 * @file
 * Administration pages for Google OAuth settings.
 */

/**
 * Menu callback; Listing of all current Api accounts.
 */
function gauth_account_list() {
  $query = db_select('gauth_accounts', 'ga')->extend('PagerDefault');
  $query->fields('ga');

  $accounts = $query
    ->limit(25)
    ->execute();
  return theme('gauth_account_list', array('accounts' => $accounts));
}

/**
 * Returns HTML for the page containing the list of accounts.
 *
 * @param array $variables
 *   An associative array containing:
 *   - accounts: An array of all the accounts returned by
 *               gauth_account_load().
 *
 * @see gauth_account_load()
 * @ingroup themeable
 */
function theme_gauth_account_list($variables) {
  $accounts = $variables['accounts'];
  $header = array(
    t('Name'),
    t('Api key'),
    t('Client Id'),
    t('Client Secret'),
    t('User'),
    t('Services'),
    t('Access Type'),
    t('Redirect Url'),
    array('data' => t('Operations'), 'colspan' => 3),
  );
  $rows = array();
  foreach ($accounts as $account) {
    $account_user = user_load($account->uid);
    $row = array();
    $row[] = $account->name;
    $row[] = $account->developer_key;
    $row[] = $account->client_id;
    $row[] = $account->client_secret;
    $row[] = l($account_user->name, "user/$account->uid");
    $row[] = implode(", ", gauth_google_services_names($account->services));
    $row[] = $account->access_type;
    $row[] = '<pre>' . gauth_callback_url() . '</pre>';
    $row[] = l(t('edit'), 'admin/config/services/gauth_account/edit/' . $account->id);
    $row[] = l(t('delete'), 'admin/config/services/gauth_account/delete/' . $account->id);
    $row[] = $account->is_authenticated ? l(t('Revoke'), 'gauth/revoke_token/' . $account->id) : l(t('Authenticate'), 'gauth/response_handler/' . $account->id);
    $rows[] = $row;
  }
  if (empty($rows)) {
    $rows[] = array(
      array(
        'colspan' => 5,
        'data' => t('There are currently no Api Accounts. <a href="!url">Add a new one</a>.', array('!url' => url('admin/config/services/gauth_account/add'))),
      ),
    );
  }

  $build['pager_table'] = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
  );

  $build['pager_pager'] = array('#theme' => 'pager');

  return $build;
}

/**
 * Form builder; Edit an account.
 *
 * @param array $form_state
 *   An associative array containing the current state of the form.
 * @param int $id
 *   An id of the account.
 *
 * @ingroup forms
 * @see gauth_account_edit_form_submit()
 * @see gauth_account_edit_form_validate()
 */
function gauth_account_edit_form($form, &$form_state, $id = NULL) {
  $form['id'] = array(
    '#type' => 'value',
    '#value' => $id,
  );
  $account = array();
  if ($id) {
    $account = gauth_account_load($id, FALSE);
  }
  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Account Name'),
    '#description' => t('The unique name for this account.'),
    '#default_value' => isset($account['name']) ? $account['name'] : '',
    '#required' => TRUE,
  );
  $form['developer_key'] = array(
    '#type' => 'textfield',
    '#title' => t('Api Key'),
    '#description' => t('The server api key of web application.'),
    '#default_value' => isset($account['developer_key']) ? $account['developer_key'] : '',
    '#required' => TRUE,
  );
  $form['client_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Client Id'),
    '#description' => t('The CLIENT ID in the "Client ID for web application" section.'),
    '#default_value' => isset($account['client_id']) ? $account['client_id'] : '',
    '#required' => TRUE,
  );
  $form['client_secret'] = array(
    '#type' => 'textfield',
    '#title' => t('Client Secret Key'),
    '#description' => t('The CLIENT SECRET in the "Client ID for web application" section.'),
    '#default_value' => isset($account['client_secret']) ? $account['client_secret'] : '',
    '#required' => TRUE,
  );
  $options = gauth_google_services_names();
  $form['services'] = array(
    '#type' => 'select',
    '#title' => t('Services'),
    '#description' => t('Services that will be enabled to be used by this account.'),
    '#options' => $options,
    '#multiple' => TRUE,
    '#default_value' => isset($account['services']) ? explode(",", $account['services']) : array(),
    '#required' => TRUE,
  );
  $form['access_type'] = array(
    '#type' => 'radios',
    '#title' => t('Access Type'),
    '#description' => t('The Access Type of the account. Select offline if the site can perform actions even when the user is not online.'),
    '#options' => array('offline' => t('Offline'), 'online' => t('Online')),
    '#default_value' => isset($account['access_type']) ? $account['access_type'] : 'offline',
  );
  $form['callback_url'] = array(
    '#type' => 'textfield',
    '#title' => t('Redirect Url'),
    '#description' => t('Copy this url and paste it in google project as a authorized redirect url.'),
    '#default_value' => gauth_callback_url(),
    '#attributes' => array('disabled' => 'disabled')
  );
  $form['uid'] = array(
    '#type' => 'value',
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#suffix' => l(t('Cancel'), 'admin/config/services/gauth_account'),
  );
  return $form;
}

/**
 * Validate handler for adding a new account to google auth accounts.
 */
function gauth_account_edit_form_validate($form, &$form_state) {
  $accounts = gauth_account_load();
  $accounts = array_keys($accounts);
  if (in_array($form_state['values']['name'], $accounts)) {
    if (!isset($form_state['values']['id'])) {
      form_set_error('name', t('Name already in use. Please choose a unique name for the account'));
    }
  }
}

/**
 * Submit handler for adding a new account to google auth accounts.
 */
function gauth_account_edit_form_submit($form, &$form_state) {
  $id = gauth_account_save($form_state['values']);
  if (!$id['is_authenticated']) {
    gauth_account_authenticate($id['id'], FALSE);
  }
  drupal_set_message(t('Api Account saved'));
  $form_state['redirect'] = 'admin/config/services/gauth_account';
}

/**
 * Form builder; Form for deleting an google auth account.
 *
 * @param int $id
 *   Id of the account to be deleted.
 *
 * @ingroup forms
 * @see gauth_account_delete_form_submit()
 */
function gauth_account_delete_form($form, &$form_state, $id) {
  $form['id'] = array(
    '#type' => 'value',
    '#value' => $id,
  );
  $question = check_plain(t('Are you sure you want to delete this account'));
  $path = 'admin/config/services/gauth_account';
  $description = check_plain(t("This account will be deleted from the system and won't be available"));
  $yes = check_plain(t('Delete'));
  $no = check_plain(t('Cancel'));
  return confirm_form($form, $question, $path, $description, $yes, $no);
}

/**
 * Submit handler to delete an google auth account.
 */
function gauth_account_delete_form_submit($form, &$form_state) {
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
  $form_state['redirect'] = 'admin/config/services/gauth_account';
}

/**
 * Form builder; Form for revoking access of an google auth account.
 *
 * @param int $id
 *   Id of the account to be deleted.
 *
 * @ingroup forms
 * @see gauth_account_token_revoke_form_submit()
 */
function gauth_account_token_revoke_form($form, &$form_state, $id) {
  $form['id'] = array(
    '#type' => 'value',
    '#value' => $id,
  );
  $question = check_plain(t('Are you sure you want to revoke access token of this account'));
  $path = 'admin/config/services/gauth_account';
  $description = check_plain(t("This account can't be used for api call until authenticated again"));
  $yes = check_plain(t('Revoke'));
  $no = check_plain(t('Cancel'));
  return confirm_form($form, $question, $path, $description, $yes, $no);
}

/**
 * Submit handler to revoke access of an google auth account.
 */
function gauth_account_token_revoke_form_submit($form, &$form_state) {
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
  $form_state['redirect'] = 'admin/config/services/gauth_account';
}
