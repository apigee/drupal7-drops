<?php
/**
 * @file
 * Administration pages for GAuth Login settings.
 */

/**
 * Administrative settings.
 *
 * @return
 *   An array containing form items to place on the module settings page.
 */
function gauth_login_settings() {
  $form['gauth_login_client_id'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Client Id'),
    '#required'      => TRUE,
    '#description' => t('The client id of google account.'),
    '#default_value' => variable_get('gauth_login_client_id', ''),
    '#description'   => t('Client id of the google drive app.'),
  );
  $form['gauth_login_client_secret'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Client Secret'),
    '#required'      => TRUE,
    '#description' => t('The client secret of google account.'),
    '#default_value' => variable_get('gauth_login_client_secret', ''),
    '#description'   => t('Client secret of the google drive app.'),
  );
  $form['gauth_login_developer_key'] = array(
    '#type' => 'textfield',
    '#title' => t('Api Key'),
    '#required' => TRUE,
    '#description' => t('The api key of google account.'),
    '#default_value' => variable_get('gauth_login_developer_key', ''),
  );
  return system_settings_form($form);
}
