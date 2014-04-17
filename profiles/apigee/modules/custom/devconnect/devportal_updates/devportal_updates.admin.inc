<?php

/**
 * @file
 * Administration page callbacks for the devportal_updates module.
 */

/**
 * Form constructor for the devportal_updates settings form.
 */
function devportal_updates_admin_settings() {

  $form['devportal_updates_url'] = array(
    '#type' => 'textfield',
    '#title' => t('Update server URL. Default is: %default',
      array('%default' => DEVPORTAL_UPDATES_DEFAULT_URL)),
    '#default_value' => variable_get('devportal_updates_url', DEVPORTAL_UPDATES_DEFAULT_URL),
    //'#description' => t('TODO: add description'),
    '#required' => TRUE,
  );

  $form['devportal_updates_check_frequency'] = array(
    '#type' => 'radios',
    '#title' => t('Check for updates'),
    '#default_value' => variable_get('devportal_updates_check_frequency', 0),
    '#options' => array(
      0 => t('Never (manually)'),
      1 => t('Daily'),
      7 => t('Weekly'),
    ),
    '#description' => t('Select how frequently you want to automatically check for updated translations for installed modules and themes.'),
  );


  return system_settings_form($form);
}

function devportal_updates_force_check() {
  // Force a check of the server for updates.
  module_load_include('inc', 'devportal_updates', 'devportal_updates.manager');
  devportal_updates_manager_get_status(TRUE);
  drupal_goto('admin/reports/status');
}