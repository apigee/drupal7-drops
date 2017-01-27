<?php

/**
 * @file
 * Settings form.
 */

/**
 * Module settings page.
 */
function devconnect_admin_notify_settings() {
  $form['user_notify'] = array(
    '#type' => 'fieldset',
    '#title' => t('User Alerts'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE
  );
  $user_notify_opts = array(
    'Custom' => t('Send to a custom email address'),
    'Role' => t('Send to a specific role(s)'),
    'Both' => t('Send to Both a custom email address and a specific role(s)'),
  );
  $form['user_notify']['devconnect_admin_notify_type'] = array(
    '#type' => 'radios',
    '#options' => $user_notify_opts,
    '#title' => t('Send by Role or Custom Email address?'),
    '#default_value' => variable_get('devconnect_admin_notify_type', 'Custom'),
    '#prefix' => '<div class="user-register-notify-user-notify">',
    '#suffix' => '</div>',
  );
  $form['user_notify']['devconnect_admin_notify_mailto'] = array(
    '#type' => 'textarea',
    '#title' => t('Send notifications to this custom email address'),
    '#description' => t('Please specify multiple email ids with comma(,) separated values eg(%mailto,%mailto). If you leave this blank, the site email address, %mailto, will be used.', array('%mailto' => variable_get('site_mail', ini_get('sendmail_from')))),
    '#default_value' => variable_get('devconnect_admin_notify_mailto', variable_get('site_mail', ini_get('sendmail_from'))),
  );
  $user_roles = user_roles(TRUE);
  $form['user_notify']['devconnect_admin_notify_roles'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Send to a specific role(s)'),
    '#options' => $user_roles,
    '#description' => t('All users with these checked roles will receive a notification email when selected.'),
    '#default_value' => variable_get('devconnect_admin_notify_roles', array()),
  );
  $form['devconnect_admin_notify_subject'] = array(
    '#type' => 'textfield',
    '#title' => t('Email Subject'),
    '#default_value' => variable_get('devconnect_admin_notify_subject', DEVCONNECT_ADMIN_NOTIFY_SUBJECT),
    '#required' => TRUE,
    '#description' => t('Subject of user registration messages')
  );
  $form['devconnect_admin_notify_body'] = array(
    '#type' => 'textarea',
    '#title' => t('Email Body'),
    '#default_value' => variable_get('devconnect_admin_notify_body', DEVCONNECT_ADMIN_NOTIFY_BODY),
    '#rows' => 10,
    '#required' => TRUE,
    '#description' => t('Customize the body of the user registration notification email.') . ' ' . t('Available variables are:') . ' !user_name, !user_mail, !user_view, !user_edit, !user_delete, !site, !uri, !uri_brief, !date, !action, !approved, !user_uid.'
  );
  $alert_options = array(
    'create' => 'Receive Email upon user creation.',
    'update' => 'Receive Email upon user creation and update.',
  );
  $form['devconnect_admin_notify_alert'] = array(
    '#type' => 'radios',
    '#options' => $alert_options,
    '#title' => t('Page Sort order'),
    '#default_value' => variable_get('devconnect_admin_notify_alert', 'create'),
    '#description' => t('When to send an Email'),
    '#required' => FALSE,
  );
  return system_settings_form($form);
}
