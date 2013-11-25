<?php

/**
 * @file
 * Theme implementation file.
 */

/**
 * Theme function for the indicator name.
 */
function theme_environment_indicator_indicator_name($variables) {
  $output = '<div class="environment-indicator-name">' . t($variables['name']) . '</div>';
  $environments = environment_indicator_get_all(TRUE);
  if (!empty($environments)) {
    foreach ($environments as $environment) {
      $items[] = array(
        'data' => l(t('Open in %name', array('%name' => t($environment->name))), 'http://' . $environment->regexurl . '/' . current_path(), array('html' => TRUE)),
        'class' => array('environment-switcher'),
      );
    }
    $switcher = theme('item_list', array(
      'items' => $items,
      'attributes' => array(
        'class' => array('environment-switcher-container'),
      ),
    ));
    $output .= $switcher;
  }
  return $output;
}

/**
 * Theme function for the indicator bar.
 */
function theme_environment_indicator_indicator_bar($variables) {
  $info = $variables['info'];
  $output = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array(
        drupal_html_class('environment-indicator-' . $info['machine']),
        'position-' . $info['position'],
        'fixed-' . ($info['fixed'] ? 'yes' : 'no'),
      ),
      'id' => 'environment-indicator',
      'style' => 'background-color: ' . $info['color'],
    ),
    'name' => array(
      '#theme' => 'html_tag',
      '#tag' => 'div',
      '#value' => theme('environment_indicator_indicator_name', $info),
      '#attributes' => array(
        'class' => array('gradient-bar'),
      ),
    ),
  );
  return drupal_render($output);
}

/**
 * Theme function for the overriden environment indicator in the header of the list.
 */
function theme_environment_indicator_overritten_header($variables) {
  $header = '<div class="overriden-indicator"><h2 class="label">' . t('Overriden indicator.') . '</h2>';
  $header .= '<div class="description">' . t('This indicator is in your settings.php file and will take precedence over any other indicator that matches a hostname. To properly set an indicator in your settings.php file, please visit the !help.', array('!help' => l(t('help pages'), 'admin/help/environment_indicator'))) . '</div></div>';
  $table = '<p>' . t('There is no overriden environment indicator in you settings.php file. You only need one in case you want your indicator to be stored in your settings.php file instead of matching against the hostname. The environment switcher is not supported for overriden environments.') . '</p>';
  if (!is_null(variable_get('environment_indicator_overwrite', NULL)) || !is_null(variable_get('environment_indicator_enabled', NULL))) {
    $headers = array(
      t('Enabled'),
      t('Name'),
      t('Color'),
      t('Position'),
      t('Fixed'),
    );
    $env = _environment_indicator_load_overritten();
    $color = $env['color'];
    if (strpos($color, '#') !== 0) {
      $color .= ' <span class="error">(' . t('Invalid color definition') . ')</span>';
    }
    $rows[] = array(
      _environment_indicator_is_overritten() ? 'TRUE' : 'FALSE',
      $env['name'],
      $color,
      $env['position'],
      $env['fixed'] ? 'TRUE' : 'FALSE',
    );
    $table = theme('table', array('rows' => $rows, 'header' => $headers));
  }
  return $header . $table . '<h2 class="label">' . t('Hostname indicators') . '</h2>';
}
