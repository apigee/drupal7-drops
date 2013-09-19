<?php // $Id$

/**
 * @file
 * views api hoooks
 */
function views_highcharts_views_plugins() {
  return array(
    'style' => array(
      'highcharts' => array(
        'title' => t('Highcharts'),
        'help' => t('Display the results as a highchart.'),
        'handler' => 'views_highcharts_plugin_style_highcharts',
        'uses options' => TRUE,
        'uses row plugin' => TRUE,
        'uses grouping' => FALSE,
        'type' => 'normal',
        'parent' => 'list',
        'path' => drupal_get_path('module', 'views_highcharts'),
        'theme' => 'views_highcharts',
        'theme path' => drupal_get_path('module', 'views_highcharts') . '/theme',
        'theme file' => 'views_highcharts.theme.inc',
      ),
    ),
  );
}

/*function views_highcharts_views_data_alter(&$data) {
  foreach ($data as $key => $item) {
    if (strpos($key, 'field_data') !== FALSE) {
      $data[$key][$item['entity_id']['field']['moved to']['1']]['field']['add fields to query'] = 'TRUE';
    }
  }
}*/

