<?php

/**
 * @file
 * Admin page callback file for the module.
 */

/**
 * Form constructor for the admin settings form.
 */
function devconnect_monetization_admin_settings() {

  $role_ids = array(
    variable_get('devconnect_monetization_role_' . MONETIZATION_ADMIN_ROLE_NAME, NULL),
    variable_get('devconnect_monetization_role_' . MONETIZATION_FINANCE_ADMIN_ROLE_NAME, NULL),
    variable_get('devconnect_monetization_role_' . MONETIZATION_DEVELOPER_ROLE_NAME, NULL),
  );

  $roles = db_select('role', 'r')
    ->fields('r', array('rid', 'name'))
    ->condition('rid', $role_ids, 'IN')
    ->orderBy('weight')
    ->execute();

  $role_options = array();
  while ($role = $roles->fetchAssoc()) {
    $role_options[$role['rid']] = $role['name'];
  }

  $default_role =
  $form['devconnect_monetization_default_role'] = array(
    '#type' => 'radios',
    '#title' => t('Default Monetization Role'),
    '#default_value' => variable_get('devconnect_monetization_default_role', NULL),
    '#options' => $role_options,
  );

  $form['devconnect_monetization_recurring_enabled'] = array(
    '#type' => 'checkbox',
    '#title' => t('Display recurring table to edit recurring parameters in company settings form.'),
    '#description' => t('Allows developer to choose if they want to use recurring payment.'),
    '#default_value' => intval(variable_get('devconnect_monetization_recurring_enabled', FALSE)),
    '#return_value' => 1,
  );

  $form['devconnect_monetization_clear_cache'] = array(
    '#type' => 'textfield',
    '#title' => t('Perform periodic cache clear of monetization API every (seconds)'),
    '#description' => t('Endpoint responses are cached to avoid overhead of GET calls, this setting specified how often this responses are clear from cache. Value must be specified in seconds'),
    '#default_value' => variable_get('devconnect_monetization_clear_cache', 600 /*10 minutes*/),
    '#required' => TRUE,
  );

  $form['devconnect_monetization_debug_endpoint_response'] = array(
    '#type' => 'checkbox',
    '#title' => t("Display error messages returned by Monetization Endpoint"),
    '#default_value' => intval(variable_get('devconnect_monetization_debug_endpoint_response', FALSE)),
    '#description' => t('Will display Monetization Endpoint error messages as warnings.'),
    '#return_value' => 1,
    '#required' => FALSE,
  );

  $form['devconnect_monetization_balance_report_type'] = array(
    '#type' => 'radios',
    '#title' => t('Billing and Reports report table'),
    '#description' => t('This setting changes the API call used to populate Billing and Reports page report table.'),
    '#default_value' => variable_get('devconnect_monetization_balance_report_type', BILLING_AND_REPORTS_USE_PREPAID_API_CALL),
    '#options' => array(
      BILLING_AND_REPORTS_USE_PREPAID_API_CALL => t('Use Prepaid API call (/developers/{{developer_email}}/prepaid-developer-balance?billingMonth={{month}}&billingYear={{year}})'),
      BILLING_AND_REPORTS_USE_DEVELOPER_BALANCES_API_CALL => t('Use Developer Balances API call (/developers/{{developer_email}}/developer-balances?all=true)'),
    ),
  );

  return system_settings_form($form);
}

/**
 * Form validator for the admin settings form.
 */
function devconnect_monetization_admin_settings_validate($form, &$form_state) {
  $clear_cache_time = $form_state['values']['devconnect_monetization_clear_cache'];
  if (!is_numeric($clear_cache_time)) {
    form_set_error('devconnect_monetization_clear_cache', t('You must enter a positive integer for the cache clear value.'));
  }
  elseif ($clear_cache_time < 0) {
    form_set_error('devconnect_monetization_clear_cache', t('Cache clear value must be positive.'));
  }
}