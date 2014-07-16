<?php
/**
 * @file
 * The configuration pages for the Siteminder module.
 */

function siteminder_admin() {
  $form = array();

  $form['siteminder_general'] = array(
    '#type' => 'fieldset',
    '#title' => t('General settings'),
    '#description' => t('Configure the basic settings to allow Drupal to authenticate via Siteminder.'),
  );

  $form['siteminder_general']['siteminder_username'] = array(
    '#type' => 'textfield',
    '#title' => t('Username Field'),
    '#default_value' => variable_get('siteminder_username', 'HTTP_SM_USER'),
    '#size' => 24,
    '#maxlength' => 24,
    '#description' => t('Define the name of the variable that your Siteminder configuration will use to pass the authenticated user name.'),
    '#required' => TRUE,
  );

  $form['siteminder_general']['siteminder_strip_prefix'] = array(
    '#type' => 'checkbox',
    '#title' => t('Strip prefix'),
    '#default_value' => variable_get('siteminder_strip_prefix', FALSE),
    '#description' => t('Enable this if your Siteminder configuration adds a prefix to the username and you do not want it used in the username.'),
  );

  $form['siteminder_general']['siteminder_strip_domain'] = array(
    '#type' => 'checkbox',
    '#title' => t('Strip domain'),
    '#default_value' => variable_get('siteminder_strip_domain', FALSE),
    '#description' => t('Enable this if your Siteminder configuration adds a domain to the username and you do not want it used in the username.'),
  );
  
  return system_settings_form($form);
}
