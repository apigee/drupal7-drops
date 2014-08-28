<?php

/**
 * @file
 *   Administrative settings for Devconnect monitor.
 */

/**
 * Form constructor for the admin settings form.
 *
 * @ingroup forms
 */
function devconnect_monitor_admin_settings_form() {
  $form = array();

  $form['description'] = array(
    '#markup' => '<p>' . t('Select one or more alerts to send out notifications for.  If the log type and log severity is matched in the watchdog logs, a notification will be sent.  The description gives you information on when this alert will happen.  Note that the error will also display on the screen if the user has the permission "DevConnect Monitor:View error details".') . '</p>'
  );

  $header = array(
    'description' => t('Description'),
    'log_type' => t('Log Type'),
    'log_severity_display_name' => t('Log Severity'),
  );

  $options = module_invoke_all('devconnect_monitor_event_info');

  $log_display_names = array(
    WATCHDOG_DEBUG     => 'Debug',
    WATCHDOG_INFO      => 'Info',
    WATCHDOG_NOTICE    => 'Notice',
    WATCHDOG_WARNING   => 'Warning',
    WATCHDOG_ERROR     => 'Error',
    WATCHDOG_CRITICAL  => 'Critical',
    WATCHDOG_ALERT     => 'Alert',
    WATCHDOG_EMERGENCY => 'Emergency',
  );

  // Switch out the int to the name of the watchdog severity level.
  foreach($options as $event => $event_details) {
    $options[$event]['log_severity_display_name'] = $log_display_names[$event_details['log_severity']];
  }

  $alert_defaults = variable_get('devconnect_monitor_alerts_selected');

  $form['alerts_table'] = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $options,
    '#default_value' => $alert_defaults,
    '#multiple' => TRUE,
    '#empty' => t('No alerts found'),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
  );

  return $form;
}

/**
 * Form submit function for the admin settings form.
 */
function devconnect_monitor_admin_settings_form_submit($form , $form_state) {
  $results = array_filter($form_state['values']['alerts_table']);
  variable_set('devconnect_monitor_alerts_selected',$results);
  drupal_set_message(t('The alert settings have been updated.'));
}

/**
* Settings form for devconnect monitor.
*/
function devconnect_monitor_admin_emails_form($form, &$form_state) {
  $addresses = devconnect_monitor_load_notification_emails();

  $form['addresses']['#tree'] = TRUE;
  foreach ($addresses as $email) {
    $form['addresses'][$email->id] = array(
      '#email' => $email,
      'email' => array(
        '#type' => 'item',
        '#markup' => $email->email,
      ),
    );
  }

  $form['new'] = array(
    'email' => array(
      '#prefix' => '<div class="add-new-placeholder">' . t('Add new e-mail') . '</div>',
      '#type' => 'textfield',
      '#size' => 30,
    ),
  );

  $form['save'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );



  return $form;
}

/**
 * Handle validation of monitor settings.
 */
function devconnect_monitor_admin_emails_form_validate($form, &$form_state) {
  if ($form_state['values']['email']) {
    if (!valid_email_address($form_state['values']['email'])) {
      form_set_error('email', t('E-mail address %email is not valid.', array('%email' => $form_state['values']['email'])));
    }
    elseif (devconnect_monitor_find_address($form_state['values']['email'])) {
      form_set_error('email', t('Entered address %email is already registered for monitoring.', array('%email' => $form_state['values']['email'])));
    }
  }
  else {
    form_set_error('email', t('Please enter an email address to add to monitoring.', array('%email' => $form_state['values']['email'])));
  }
}

/**
 * Handle submission of multiple_email_manage_form.
 */
function devconnect_monitor_admin_emails_form_submit($form, &$form_state) {

  // Add a new e-mail address.
  if ($form_state['values']['email']) {
    $id = devconnect_monitor_register_email($form_state['values']['email']);

    if (!isset($id)) {
      form_set_error('', t('Error attempting to register %email', array('%email' => $form_state['values']['email'])));
    }
  }
}

/**
 * Theme devconnect_monitor_admin_settings_form form.
 */
function theme_devconnect_monitor_admin_emails_form($variables) {
  $output = '<p>' . t('Emails added to the Dev Portal Monitor will be emailed when ' .
    'any critical error occurs.  If there are no emails listed, no emails are '.
    'sent.') . '</p>';

  $form = $variables['form'];

  $rows = array();

  foreach (element_children($form['addresses']) as $id) {
    $address = $form['addresses'][$id]['#email']->email;
    $row = array();
    $row[] = drupal_render($form['addresses'][$id]['email']);

    $ops = array();

    // Only allow deletes for now.
    $ops[] = l(t('Delete'), 'admin/config/devconnect/monitor/' . $address . '/delete/' . $id);

    $row[] = implode(' | ', $ops);
    $rows[] = $row;
  }

  $headers = array(t('E-mail'), t('Operations'));


  $output .= theme('table', array('header' => $headers, 'rows' => $rows, 'empty' => t('No email addresses.')));
  $output .= drupal_render_children($form);

  return $output;
}

/**
 * Form displayed to confirm deletion of an e-mail address from notifications.
 */
function devconnect_monitor_admin_email_delete_form($form, &$form_state, $email, $id) {
  $form['email'] = array(
    '#type' => 'value',
    '#value' => $email,
  );

  $form['id'] = array(
    '#type' => 'value',
    '#value' => $id,
  );

  return confirm_form($form,
    t('Are you sure you wish to delete the address %email?', array('%email' => $email)),
    'admin/config/devconnect_monetization/monitor',
    NULL,
    t('Delete'), t('Cancel'));
}

/**
 * Processes devconnect_monitor_admin_email_delete_form.
 *
 * Delete an email address from system.
 */
function devconnect_monitor_admin_email_delete_form_submit($form, &$form_state) {
  $email = $form_state['values']['email'];
  $id = $form_state['values']['id'];

  devconnect_monitor_remove_email($id);
  drupal_set_message(t('The e-mail address %email has been removed.', array('%email' => $email)));

  $form_state['redirect'] = 'admin/config/devconnect_monetization/monitor';
}
