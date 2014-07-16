<?php

/**
 * @file
 * Admin page callback file for the module.
 */

/**
 * Form constructor for the devportal_updates settings form.
 */
function devconnect_monetization_admin_settings() {

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
