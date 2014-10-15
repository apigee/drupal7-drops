<?php

include_once(dirname(__FILE__) . '/includes/apigee_base.inc');

/**
 * Implements hook_form_FORM_ID_alter().
 */
function apigee_base_form_system_theme_settings_alter(&$form, &$form_state) {
  $form['theme_settings']['toggle_sign_up'] = array(
    '#type' => 'checkbox',
    '#title' => t('Sign up'),
    '#default_value' => theme_get_setting('toggle_sign_up'),
  );
  $form['theme_settings']['toggle_sign_in'] = array(
    '#type' => 'checkbox',
    '#title' => t('Sign in'),
    '#default_value' => theme_get_setting('toggle_sign_in'),
  );
  $form['theme_settings']['toggle_search'] = array(
    '#type' => 'checkbox',
    '#title' => t('Search box'),
    '#default_value' => theme_get_setting('toggle_search'),
  );
  $form['theme_settings']['toggle_breadcrumbs'] = array(
    '#type' => 'checkbox',
    '#title' => t('Breadcrumbs'),
    '#default_value' => theme_get_setting('toggle_breadcrumbs'),
  );
  $form['theme_settings']['toggle_subscribe'] = array(
    '#type' => 'checkbox',
    '#title' => t('Subscribe links'),
    '#default_value' => theme_get_setting('toggle_subscribe'),
  );
  $form['logo_link'] = array(
    '#type' => 'fieldset',
    '#title' => t('Logo Link'),
    '#description' => t("Use this textfield to add a custom link to the logo.  If left blank, it will default to the homepage."),
  );
  $form['logo_link']['logo_link_href'] = array(
    '#type' => 'textfield',
    '#title' => t('Logo Link'),
    '#default_value' => theme_get_setting('logo_link_href'),
  );
}

