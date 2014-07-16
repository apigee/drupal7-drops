<?php

/**
 * @file
 *   Administrative settings for Devconnect monetization monitor.
 */


/**
* Settings form for monetization monitor.
*
* @ingroup forms
*/
function devconnect_monetization_monitor_admin_settings_form($form, &$form_state) {
  $addresses = devconnect_monetization_monitor_load_notification_emails();

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
 * Handle validation of monetization monitor settings.
 */
function devconnect_monetization_monitor_admin_settings_form_validate($form, &$form_state) {
  if ($form_state['values']['email']) {
    if (!valid_email_address($form_state['values']['email'])) {
      form_set_error('email', t('E-mail address %email is not valid.', array('%email' => $form_state['values']['email'])));
    }
    elseif (devconnect_monetization_monitor_find_address($form_state['values']['email'])) {
      form_set_error('email', t('Entered address %email is already registered on this site.', array('%email' => $form_state['values']['email'])));
    }
  }
  else {
    form_set_error('email', t('Please enter an email address to add to the Monetization monitoring.', array('%email' => $form_state['values']['email'])));
  }
}

/**
 * Handle submission of multiple_email_manage_form.
 */
function devconnect_monetization_monitor_admin_settings_form_submit($form, &$form_state) {

  // Add a new e-mail address.
  if ($form_state['values']['email']) {
    $id = devconnect_monetization_monitor_register_email($form_state['values']['email']);

    if (!isset($id)) {
      form_set_error('', t('Error attempting to register %email', array('%email' => $form_state['values']['email'])));
    }
  }
}

/**
 * Theme devconnect_monetization_monitor_admin_settings_form form.
 *
 * @param array $form
 */
function theme_devconnect_monetization_monitor_admin_settings_form($variables) {
  $output = '<p>' . t('Emails added to the monitization monitor will be emailed when ' .
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
    $ops[] = l(t('Delete'), 'admin/config/devconnect_monetization/monitor/' . $address . '/delete/' . $id);

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
function devconnect_monetization_monitor_admin_email_delete_form($form, &$form_state, $email, $id) {
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
 * Processes devconnect_monetization_monitor_admin_email_delete_form.
 *
 * Delete an email address from system.
 */
function devconnect_monetization_monitor_admin_email_delete_form_submit($form, &$form_state) {
  $email = $form_state['values']['email'];
  $id = $form_state['values']['id'];

  devconnect_monetization_monitor_remove_email($id);
  drupal_set_message(t('The e-mail address %email has been removed.', array('%email' => $email)));

  $form_state['redirect'] = 'admin/config/devconnect_monetization/monitor';
}
