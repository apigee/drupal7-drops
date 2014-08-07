<?php

/**
 * @file
 * Administrative page callbacks for the modal_forms module.
 */

/**
 * Administrative settings for bootstrap_modal_forms.
 *
 * @param $form
 * @param $form_state
 * @return mixed
 */
function bootstrap_model_forms_settings($form, $form_state) {
  $form = array();

  $form['bootstrap_modal_form_bootstrap_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Bootstrap Settings'),
  );
  // Defines which version of bootstrap.
  $form['bootstrap_modal_form_bootstrap_settings']['bootstrap_version'] = array(
    '#type' => 'select',
    '#title' => t('Bootstrap Version'),
    '#options' => array(
      '2' => '2.3.2',
      '3' => '3',
    ),
    '#default_value' => variable_get('bootstrap_version', '3'),
    '#description' => t('The bootstrap version the active theme is using.'),
  );

  // Defines whether or not to use the User Login form.
  $form['bootstrap_modal_form_login_form_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Login Form Settings'),
  );
  $form['bootstrap_modal_form_login_form_settings']['bootstrap_modal_forms_login'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable for the Login Form'),
    '#default_value' => variable_get('bootstrap_modal_forms_login', 0),
    '#description' => t('Use the bootstrap modal for login.'),
  );
  // Defines whether or not to use the User Registration form.
  $form['bootstrap_modal_form_registration_form_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Register Form Settings'),
  );
  $form['bootstrap_modal_form_registration_form_settings']['bootstrap_modal_forms_register'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable for the Registration Form'),
    '#default_value' => variable_get('bootstrap_modal_forms_register', 0),
    '#description' => t('Use the bootstrap modal for registration.'),
  );
  // Defines whether or not to use the Password Request form.
  $form['bootstrap_modal_form_pass_form_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Lost Password Form Settings'),
  );
  $form['bootstrap_modal_form_pass_form_settings']['bootstrap_modal_forms_pass'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable for the Request New Password Form'),
    '#default_value' => variable_get('bootstrap_modal_forms_pass', 0),
    '#description' => t('Use the bootstrap modal for requesting a new password.'),
  );
  return system_settings_form($form);
}
