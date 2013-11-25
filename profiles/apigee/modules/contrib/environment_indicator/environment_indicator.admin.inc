<?php

/**
 * @file
 * Administrative interface for environment indicator configuration.
 */

/**
 * Module settings form.
 */
function environment_indicator_settings() {
  $form['environment_indicator_suppress_pages'] = array(
    '#type' => 'textarea',
    '#title' => t('Turn off Environment Indicator on these pages'),
    '#default_value' => variable_get('environment_indicator_suppress_pages', ""),
    '#description' => t("Enter one page per line as Drupal paths. The '*' character is a wildcard. Example paths are '<em>blog</em>' for the blog page and '<em>blog/*</em>' for every personal blog. '<em>&lt;front&gt;</em>' is the front page."),
  );
  return system_settings_form($form);
}
