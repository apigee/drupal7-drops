<?php

/**
 * @file
 * Admin page callback file for the module.
 */

/**
 * Form constructor for the admin settings form.
 */
function apigee_company_admin_settings() {
  $form['apigee_company_is_company_cache_enabled'] = array(
    '#type' => 'checkbox',
    '#title' => t('Cache companies'),
    '#description' => t('Cache company data from Apigee Edge API. Turning off caching will cause performance issues.'),
    '#default_value' => variable_get('apigee_company_is_company_cache_enabled', 1),
  );

  return system_settings_form($form);
}
