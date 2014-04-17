<?php

/**
 * Overrides theme_devconnect_monetization_roles_form().
 */
function apigee_responsive_devconnect_monetization_roles_form($variables) {
  $form = $variables['form'];

  $rows = array();

  foreach (element_children($form['developers']) as $uid) {
    $row = array();
    foreach (element_children($form['developers'][$uid]) as $element) {
      $row[] = drupal_render($form['developers'][$uid][$element]);
    }
    $rows[] = array('data' => $row);
  }

  $output = '<div class="table-responsive">' .
    theme_table(array(
      'header' => $form['#table_headers'],
      'rows' => $rows,
      'attributes' => array('class' => array('table', 'table-bordered')),
      'caption' => '',
      'colgroups' => array(),
      'sticky' => TRUE,
      'empty' => t('Your company has no developers assigned.'),
    )) .
    '</div>' .
    drupal_render($form['submit']) . drupal_render_children($form);
  return  $output;
}

/**
 * Overrides theme_devconnect_monetization_recurring_balances().
 */
function apigee_responsive_devconnect_monetization_recurring_balances($vars) {
  $rows = array();
  foreach (element_children($vars['balances']['items']) as $currency) {
    $rows[] = array(
      array('data' => $vars['balances']['items'][$currency]['charge_per_usage']),
      array('data' => $vars['balances']['items'][$currency]['is_recurring']),
      array('data' => $vars['balances']['items'][$currency]['#provider']),
      array('data' => $vars['balances']['items'][$currency]['#currency']),
      array('data' => $vars['balances']['items'][$currency]['recurring_amount']),
      array('data' => $vars['balances']['items'][$currency]['replenish_amount']),
    );
  }

  $header = array(
    t('Charge per Usage'),
    t('Recurring'),
    t('Provider'),
    t('Currency'),
    t('Recurring Amount'),
    t('Replenish Amount')
  );
  return '<div class="table-responsive">' . theme('table', array('header' => $header, 'rows' => $rows)) . '</div>';
}
