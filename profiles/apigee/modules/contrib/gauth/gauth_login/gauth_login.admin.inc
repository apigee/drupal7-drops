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
    '#description'   => t('The CLIENT ID in the "Client ID for web application section.'),
  );
  $form['gauth_login_client_secret'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Client Secret'),
    '#required'      => TRUE,
    '#description' => t('The client secret of google account.'),
    '#default_value' => variable_get('gauth_login_client_secret', ''),
    '#description'   => t('The CLIENT SECRET in the "Client ID for web application" section.'),
  );
  $form['gauth_login_developer_key'] = array(
    '#type' => 'textfield',
    '#title' => t('Api Key'),
    '#required' => TRUE,
    '#description' => t('The server api key of the web application.'),
    '#default_value' => variable_get('gauth_login_developer_key', ''),
  );
  $form['gauth_login_prom_message'] = array(
    '#type' => 'checkbox',
    '#title' => t('Prom message to change password on firts login.'),
    '#description' => t('If this option is enabled a new user will see prom message to change password.'),
    '#default_value' => variable_get('gauth_login_prom_message', TRUE),
  );
  $form['advanced_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Advanced Settings'),
    '#description' => t(''),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['advanced_settings']['gauth_login_domain_restriction'] = array(
    '#type' => 'textfield',
    '#title' => t('Restrict to Domain'),
    '#description' => t('If you want to restrict the users from a specific domain to use loging with google insert your domain here. eg mycollege.edu. Note that this works only for Google Apps hosted accounts, also note that if some user other than this domain tries to login with google then he will face a issues. Leave this blank if you are not sure how it works as wrong configuration will break google login.'),
    '#default_value' => variable_get('gauth_login_domain_restriction', ''),
  );
  $form['advanced_settings']['gauth_login_create_user'] = array(
    '#type' => 'checkbox',
    '#title' => t('Create new user if not found.'),
    '#description' => t('Creates new drupal account when some new users uses login with google. If disabled will not create a new drupal account and show a message to the user.'),
    '#default_value' => variable_get('gauth_login_create_user', TRUE),
  );
  $form['advanced_settings']['gauth_login_create_user_not_allowed_message'] = array(
    '#type' => 'textfield',
    '#title' => t('Message to be shown when create new user is disabled'),
    '#description' => t('Type in the message which should be shown to the users when create new user is disabled and a new user uses login with google.'),
    '#default_value' => variable_get('gauth_login_create_user_not_allowed_message', 'Can not find a user with this email. Did you use other google account while registering?'),
    '#states' => array(
      'visible' => array(
        ':input[name="gauth_login_create_user"]' => array('checked' => FALSE),
      ),
      'required' => array(
        ':input[name="gauth_login_create_user"]' => array('checked' => FALSE),
      ),
    ),
  );
  return system_settings_form($form);
}

function gauth_login_settings_validate($form, $form_state) {
  if (!$form_state['values']['gauth_login_create_user'] && trim($form_state['values']['gauth_login_create_user_not_allowed_message']) == '') {
    form_set_error('gauth_login_create_user_not_allowed_message', t('Message to be shown is a required field'));
  }
}
