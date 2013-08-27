<?php
// $Id:

/**
 * @file
 * Contains all admin pages, settings, and validate.
 */

/**
 * Settings form for menu callback
 */
function autologout_settings() {
  $form = array();
  $timeout = variable_get('autologout_timeout', 1800);

  if (module_exists('jstimer')) {
    if (!module_exists('jst_timer')) {
      drupal_set_message(t('The "Widget: timer" module must also be enabled for the dynamic countdown to work in the automated logout block.'), 'error');
    }

    if (variable_get('jstimer_js_load_option', 0) != 1) {
      drupal_set_message(t('The Javascript timer module\'s "Javascript load options" setting should be set to "Every page" for the dynamic countdown to work in the automated logout block.'), 'error');
    }
  }

  $form['autologout_timeout'] = array(
    '#type' => 'textfield',
    '#title' => t('Timeout value in seconds'),
    '#default_value' => $timeout,
    '#size' => 8,
    '#weight' => -10,
    '#description' => t('The length of inactivity time, in seconds, before automated log out.  Must be 60 seconds or greater. Will not be used if role timeout is activated.'),
  );

  $form['autologout_max_timeout'] = array(
    '#type' => 'textfield',
    '#title' => t('Max timeout setting'),
    '#default_value' => variable_get('autologout_max_timeout', '172800'),
    '#size' => 10,
    '#maxlength' => 12,
    '#weight' => -8,
    '#description' => t('The maximum logout threshold time that can be set by users who have the permission to set user level timeouts.'),
  );

  $form['autologout_padding'] = array(
    '#type' => 'textfield',
    '#title' => t('Timeout padding'),
    '#default_value' => variable_get('autologout_padding', 10),
    '#size' => 8,
    '#weight' => -6,
    '#description' => t('How many seconds to give a user to respond to the logout dialog before ending their session.'),
  );

  $form['autologout_role_logout'] = array(
    '#type' => 'checkbox',
    '#title' => t('Role Timeout'),
    '#default_value' => variable_get('autologout_role_logout', FALSE),
    '#weight' => -4,
    '#description' => t('Enable each role to have its own timeout threshold, a refresh maybe required for changes to take effect. Any role not ticked will use the default timeout value. Any role can have a value of 0 which means that they will never be logged out.'),
  );

  $form['autologout_redirect_url']  = array(
    '#type' => 'textfield',
    '#title' => t('Redirect URL at logout'),
    '#default_value' => variable_get('autologout_redirect_url', 'user/login'),
    '#size' => 40,
    '#description' => t('Send users to this internal page when they are logged out.'),
  );

  $form['autologout_message']  = array(
    '#type' => 'textarea',
    '#title' => t('Message to display in the logout dialog'),
    '#default_value' => variable_get('autologout_message', 'Your session is about to expire. Do you want to reset it?'),
    '#size' => 40,
    '#description' => t('This message must be plain text as it might appear in a JavaScript confirm dialog.'),
  );

  $form['autologout_inactivity_message']  = array(
    '#type' => 'textarea',
    '#title' => t('Message to display to the user after they are logged out.'),
    '#default_value' => variable_get('autologout_inactivity_message', 'You have been logged out due to inactivity.'),
    '#size' => 40,
    '#description' => t('This message is displayed after the user was logged out due to inactivity. You can leave this blank to show no message to the user.'),
  );

  $form['autologout_use_watchdog'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable watchdog Automated Logout logging'),
    '#default_value' => variable_get('autologout_use_watchdog', ''),
    '#description' => t('Enable logging of automatically logged out users'),
  );

  $form['autologout_enforce_admin'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enforce auto logout on admin pages'),
    '#default_value' => variable_get('autologout_enforce_admin', FALSE),
    '#description' => t('If checked, then users will be automatically logged out when administering the site.'),
  );

  if (module_exists('jstimer') && module_exists('jst_timer')) {
    $form['autologout_jstimer_format']  = array(
      '#type' => 'textfield',
      '#title' => t('Autologout block time format'),
      '#default_value' => variable_get('autologout_jstimer_format', '%hours%:%mins%:%secs%'),
      '#description' => t('Change the display of the dynamic timer.  Available replacement values are: %day%, %month%, %year%, %dow%, %moy%, %years%, %ydays%, %days%, %hours%, %mins%, and %secs%.')
    );
  }

  $form['table'] = array(
    '#type' => 'item',
    '#weight' => -2,
    '#title' => t('If Enabled every user in role will be logged out based on that roles timeout, unless the user has an indivual timeout set.'),
    '#theme' => 'autologout_render_table',
  );

  foreach (user_roles(TRUE) as $key => $role){

    $form['table']['autologout_roles']['autologout_role_'. $key .'_timeout'] = array(
      '#type' => 'textfield',
      '#default_value' => variable_get('autologout_role_' . $key.'_timeout', $timeout),
      '#size' => 8,
      '#theme' => 'textfield',
    );

  }

  foreach (user_roles(TRUE) as $key => $role){
    $form['table']['autologout_roles']['autologout_role_' . $key] = array(
      '#type' => 'checkbox',
      '#default_value' => variable_get('autologout_role_' . $key, FALSE),
      '#theme' => 'checkbox'
    );
  }

  return system_settings_form($form);
}

/**
 * Settings validation.
 */
function autologout_settings_validate($form, &$form_state) {
  $max_timeout = $form_state['values']['autologout_max_timeout'];
  $role_timeout = _autologout_get_role_timeout();

  // Validate timeouts for each role.
  foreach (user_roles(TRUE) as $key => $role) {
    if (empty($form_state['values']['autologout_role_' . $key])) {
      // Don't validate role timeouts for non enabled roles.
      continue;
    }

    $timeout = $form_state['values']['autologout_role_' . $key . '_timeout'];
    $validate = autologout_timeout_validate($timeout, $max_timeout);

    if (!$validate) {
      form_set_error('autologout_role_' . $key . '_timeout', t('%role role timeout must be an integer greater than 60, less then %max or 0 to disable autologout for that role.', array('%role' => $role, '%max' => $max_timeout)));
    }
  }

  $timeout = $form_state['values']['autologout_timeout'];

  // Validate timeout.
  if (!is_numeric($timeout) || ((int) $timeout != $timeout) || $timeout < 60 || $timeout > $max_timeout) {
    form_set_error('autologout_timeout', t('The timeout must be an integer greater than 60 and less then %max.', array('%max' => $max_timeout)));
  }
}
