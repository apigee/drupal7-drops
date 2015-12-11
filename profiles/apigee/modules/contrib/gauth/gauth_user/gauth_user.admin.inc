<?php
/**
 * @file
 * Administration pages for Google Auth User Services.
 */

/**
 * Menu callback; Listing of all current services accounts.
 */
function gauth_user_services_list() {

  $query = db_select('gauth_user_services', 'gu')->extend('PagerDefault');
  $query->fields('gu');

  $accounts = $query
    ->limit(25)
    ->execute();
  return theme('gauth_user_services_list', array('accounts' => $accounts));
}

/**
 * Returns HTML for the page containing the list of accounts.
 *
 * @param array $variables
 *   An associative array containing:
 *   - accounts: An array of all the accounts returned by
 *               gauth_user_services_load().
 *
 * @see gauth_user_services_load()
 * @ingroup themeable
 */
function theme_gauth_user_services_list($variables) {
  $accounts = $variables['accounts'];
  $header = array(
    t('Name'),
    t('Api key'),
    t('Client Id'),
    t('Client Secret'),
    t('Services'),
    t('Access Type'),
    array('data' => t('Operations'), 'colspan' => 2),
  );
  $rows = array();
  foreach ($accounts as $account) {
    $row = array();
    $row[] = $account->name;
    $row[] = $account->developer_key;
    $row[] = $account->client_id;
    $row[] = $account->client_secret;
    $row[] = implode(", ", gauth_google_services_names($account->services));
    $row[] = $account->access_type;
    $row[] = l(t('edit'), 'admin/config/services/gauth_user/edit/' . $account->id);
    $row[] = l(t('delete'), 'admin/config/services/gauth_user/delete/' . $account->id);
    $rows[] = $row;
  }
  if (empty($rows)) {
    $rows[] = array(
      array(
        'colspan' => 5,
        'data' => t('There are currently no Services Accounts. <a href="!url">Add a new one</a>.', array('!url' => url('admin/config/services/gauth_user/add'))),
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
 * Form builder; Edit an services account.
 *
 * @param array $form_state
 *   An associative array containing the current state of the form.
 * @param int $id
 *   An id of the account.
 *
 * @ingroup forms
 * @see gauth_user_services_edit_form_submit()
 * @see gauth_user_services_edit_form_validate()
 */
function gauth_user_services_edit_form($form, &$form_state, $id = NULL) {
  $account = array();
  if ($id) {
    $account = gauth_user_services_load($id, FALSE);
    $form['is_new'] = array(
      '#type' => 'value',
      '#value' => FALSE,
    );
  }
  else {
    $form['is_new'] = array(
      '#type' => 'value',
      '#value' => TRUE,
    );
  }
  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Account Name'),
    '#description' => t('The unique name for this account.'),
    '#default_value' => isset($account['name']) ? $account['name'] : '',
    '#required' => TRUE,
  );
  // Machine-readable type name.
  $form['id'] = array(
    '#type' => 'machine_name',
    '#default_value' => $id ? $id : '',
    '#maxlength' => 32,
    '#machine_name' => array(
      'exists' => 'gauth_user_services_types',
      'source' => array('name'),
    ),
    '#description' => t('A unique machine-readable name for this service account. It must only contain lowercase letters, numbers, and underscores.'),
  );
  $form['developer_key'] = array(
    '#type' => 'textfield',
    '#title' => t('Api Key'),
    '#description' => t('The server api key of the web application.'),
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
    '#default_value' => $account['access_type'],
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#suffix' => l(t('Cancel'), 'admin/config/services/gauth_user'),
  );
  return $form;
}

/**
 * Validate handler for adding a new account to google auth services accounts.
 */
function gauth_user_services_edit_form_validate($form, &$form_state) {
  $accounts = gauth_user_services_load();
  $accounts = array_keys($accounts);
  if (in_array($form_state['values']['name'], $accounts)) {
    if (!isset($form_state['values']['is_new'])) {
      form_set_error('name', t('Name already in use. Please choose a unique name for the services account'));
    }
  }
}

/**
 * Submit handler for adding a new account to google auth services accounts.
 */
function gauth_user_services_edit_form_submit($form, &$form_state) {
  $id = gauth_user_services_save($form_state['values']);
  drupal_set_message(t('Services Account saved.  Click !url to configure permissions for this account', array('!url' => l(t('here'), 'admin/people/permissions', array('fragment' => 'module-gauth_user')))));
  $form_state['redirect'] = 'admin/config/services/gauth_user';
}

/**
 * Form builder; Form for deleting an google auth services account.
 *
 * @param int $id
 *   Id of the account to be deleted.
 *
 * @ingroup forms
 * @see gauth_account_delete_form_submit()
 */
function gauth_user_services_delete_form($form, &$form_state, $id) {
  $form['id'] = array(
    '#type' => 'value',
    '#value' => $id,
  );
  $question = check_plain(t('Are you sure you want to delete this services account'));
  $path = 'admin/config/services/gauth_user';
  $description = check_plain(t("This service account will be deleted from the system and won't be available for end users.  The previous user authentications will still be present."));
  $yes = check_plain(t('Delete'));
  $no = check_plain(t('Cancel'));
  return confirm_form($form, $question, $path, $description, $yes, $no);
}

/**
 * Submit handler to delete an google auth services account.
 */
function gauth_user_services_delete_form_submit($form, &$form_state) {
  if ($form_state['values']['id'] != NULL) {
    $rows = gauth_user_services_delete($form_state['values']['id'], FALSE);
    if ($rows == 1) {
      drupal_set_message(t("The service is deleted successfully"));
    }
    else {
      drupal_set_message(t("Error occured while deleting the service"), "error");
    }
  }
  else {
    drupal_set_message(t("Error occured: Can't find service to be deleted"), "error");
  }
  $form_state['redirect'] = 'admin/config/services/gauth_user';
}
