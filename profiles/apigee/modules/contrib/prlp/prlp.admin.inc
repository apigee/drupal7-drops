<?php
/**
 * Form builder function for PRLP config settings.
 */
function prlp_admin_settings($form) {
  $form['prlp'] = array(
    '#type' => 'container',
    '#tree' => FALSE,
  );
  $form['prlp']['prlp_password_required'] = array(
    '#type' => 'checkbox',
    '#title' => t('Password Entry Required'),
    '#description' => t('If set, users will be required to enter a new password when they use a password reset link to login'),
    '#default_value' => variable_get('prlp_password_required', 1),
  );
  $form['prlp']['prlp_destination'] = array(
    '#type' => 'textfield',
    '#title' => t('Login Destination'),
    '#description' => t('User will be taken to this path after they log in with the password reset link. Token %uid can be used in the path, and will be replaced with the uid of the current user. Use %front for site front-page.', array('%uid' => '%uid', '%front' => '<front>')),
    '#default_value' => variable_get('prlp_destination', PRLP_DESTINATION_DEFAULT),
  );
  $form['prlp']['prlp_hidden_fields'] = array(
    '#type' => 'textarea',
    '#title' => t('Fields Hidden'),
    '#description' => t('List of form keys that should be hidden on password reset landing page. Put one per line.'),
    '#default_value' => variable_get('prlp_hidden_fields', PRLP_HIDDEN_FIELDS_DEFAULT),
  );
  $form['prlp']['prlp_hidden_account_fields'] = array(
    '#type' => 'textarea',
    '#title' => t('Account Fields Hidden'),
    '#description' => t('List of form sub-keys under \'account\' key (e.g. \'mail\', \'name\' etc.) that should be hidden on password reset landing page. Put one per line.'),
    '#default_value' => variable_get('prlp_hidden_account_fields', PRLP_HIDDEN_ACCOUNT_FIELDS_DEFAULT),
  );
  $form = system_settings_form($form);
  $form['actions']['reset'] = array(
    '#type' => 'submit',
    '#value' => t('Reset to defaults'),
    '#description' => t('Delete all configuration and reset to hard-coded default settings'),
    '#submit' => array('prlp_admin_settings_reset'),
  );
  return $form;
}

/**
 * Submit handler for 'Reset to defaults' button. Deletes all configuration (variables) used by this module.
 */
function prlp_admin_settings_reset(&$form, &$form_state) {
  prlp_delete_config_variables();
  $form_state['rebuild'] = FALSE;
  drupal_set_message(t('PRLP configuration settings have been reset to hard-coded defaults.'));
}

/**
 * Deletes all configuration (variables) used by this module.
 */
function prlp_delete_config_variables() {
  $variable_names = array(
    'prlp_password_required',
    'prlp_destination',
    'prlp_hidden_fields',
    'prlp_hidden_account_fields',
  );
  foreach($variable_names as $variable_name) {
    variable_del($variable_name);
  }
}