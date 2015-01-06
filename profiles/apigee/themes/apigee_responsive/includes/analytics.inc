<?php


/**
 * Provides the analytics rendering for the apigee_responsive theme.
 */
function _apigee_responsive_render_analytics(&$vars) {
  module_load_include('inc', 'devconnect_developer_apps', 'analytics');
  foreach ($vars['applications'] as $key => $app) {
    $form = drupal_get_form('apigee_responsive_analytics_form', $app);
    $vars['applications'][$key]['analytics_filter_form'] = $form;
  }
  module_load_include('module', 'libraries', 'libraries');
  if ($path = libraries_get_path('highcharts')) {
    drupal_add_js($path . '/js/highcharts.js');
  }
  drupal_add_js(drupal_get_path('theme', 'apigee_responsive') . '/js/analytics.js');
}

/**
 * Form that renders analytics on the my apps page
 */
function apigee_responsive_analytics_form($form, $form_state, $app) {
  global $user;
  $form = array();
  drupal_add_library ('system' , 'ui.tabs');
  drupal_add_library('system', 'ui.datepicker');
  drupal_add_js(drupal_get_path('module', 'devconnect_developer_apps') . '/js/jquery-ui-timepicker-addon.js');
  drupal_add_js('jQuery(document).ready(function(){
    jQuery("#edit-date-from" ).datetimepicker();
    jQuery("#edit-date-to" ).datetimepicker();
  });', 'inline');
  $form['#prefix'] = '<div class="panel panel-default">';
  $form['#suffix'] = '</div>';
  $form['#attributes'] = array(
    'class' => 'form-inline',
  );
  $metric_options = _devconnect_developer_apps_analytics_metrics();
  $form['header'] = array(
    '#prefix' => '<div class="panel-heading"><div class="row"><div class="col-md-12">',
    '#suffix' => '</div></div></div>',
  );
  $form['header']['metric'] = array(
    '#type' => 'select',
    '#title' => t('Display'),
    '#multiple' => FALSE,
    '#options' => $metric_options,
    '#default' => array('total_response_time'),
  );
  $form['header']['date_from'] = array(
    '#type' => 'textfield',
    '#title' => t('From'),
    '#size' => 20,
    '#default_value' => gmdate('m/d/Y H:i', strtotime("-1 day")) . ' UTC',
    '#attributes' => array(
      'class' => array(
        'pickadate',
      ),
    )
  );
  $form['header']['date_to'] = array(
    '#type' => 'textfield',
    '#title' => t('To'),
    '#size' => 20,
    '#default_value' => gmdate('m/d/Y H:i', time()) . ' UTC',
    '#attributes' => array(
      'class' => array(
        'pickadate',
      ),
    )
  );
  $wrapper = preg_replace('/[^A-Za-z0-9]/', "", strtolower($app['app_name'] . $user->name));
  $form['header']['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Submit',
    '#data' => $app,
    '#attributes' => array(
      'class' => array(
        'pull-right',
      ),
    ),
    '#ajax' => array(
      'wrapper' => $wrapper,
      'callback' => '_apigee_responsive_analytics_ajax_callback',
      'effect' => 'fade',
    ),
  );
  $form['body'] = array(
    '#prefix' => '<div class="panel-body">',
    '#suffix' => '</div>'
  );
  $values = array(
    'metric' => 'total_response_time',
    'end' => $form['header']['date_to']['#default_value'],
    'start' => $form['header']['date_from']['#default_value'],
    'unit' => 'hour',
  );
  $form['body']['result'] = array(
    '#type' => 'markup',
    '#data' => $app,
    '#markup' => _apigee_responsive_render_analytic_data($form, $form_state, $wrapper, $values),
  );
  return $form;
}

/**
 * Ajax Callback for apigee_responsive analytics.
 */
function _apigee_responsive_analytics_ajax_callback($form, $form_state) {
  $values = array(
    'metric' => $form_state['input']['metric'],
    'end' => $form_state['input']['date_to'],
    'start' => $form_state['input']['date_from'],
    'unit' => 'hour',
  );
  $data = _apigee_responsive_render_analytic_data($form, $form_state, $form['header']['submit']['#ajax']['wrapper'], $values);
  $form['body']['result']['#markup'] = $data;
  return $form['body']['result']['#markup'];
}

/**
 * Renders the Analytic data via the highcharts library
 */
function _apigee_responsive_render_analytic_data($form, $form_state, $wrapper, $values) {
  $analytics_data = _apigee_responsive_render_analytics_json($form, $form_state, $values);
  $data[$wrapper] = array(
    'analytics_data' => $analytics_data
  );
  drupal_add_js(array('apigee_responsive_analytics_data' => array($data)), 'setting');
  return '<div id="' . $wrapper . '"></div>';
}

/**
 * Renders the necessary JSON for the highcharts library
 */
function _apigee_responsive_render_analytics_json($form, $form_state, $values) {
  global $user;
  module_load_include('inc', 'devconnect_developer_apps', 'analytics');
  $my_data = _devconnect_developer_apps_analytics_get_datapoints($user, $form['header']['submit']['#data']['app_name'], $values);
  $metric_fullname = $my_data['params']['metric']['display_name'];
  $metric_name = $form_state['input']['metric'];

  $chart_data = array(
    'chart_name' => $metric_fullname,
    'chart_name_y' => $metric_name,
    'categories' => array(),
    'series' => array(),
  );


  $app = $form['header']['submit']['#data']['app_name'];
  $envs = array();

  foreach($my_data['datapoints'][$app] as $env => $data) {
    $_series = $my_data['default_datapoints'];
    $envs[] = $env;
    foreach($data as $time => $point) {
      $_series[$time] = $point;
    }
    $series[] = array('name' => $app, 'data'=>array_values($_series));
  }
  // Step across X-axis labels if there are too many.
  $step = 1;
  if (count($my_data['time_points']) > DEVCONNECT_ANALYTICS_MAX_XAXIS_LINES) {
    $step = ceil(count($my_data['time_points']) / DEVCONNECT_ANALYTICS_MAX_XAXIS_LINES);
  }

  $last_time = 0;
  foreach ($my_data['time_points'] as $i => $time_point) {
    $x_caption = date($my_data['date_format'], $time_point);
    if (substr($my_data['date_format'], 0, 1) == 'H' && ($i % $step == 0) && ($last_time == 0 || date('d', $time_point) != date('d', $last_time))) {
      $x_caption .= date('F j', $time_point);
      $last_time = $time_point;
    }
    $chart_data['categories'][] = $x_caption;
  }
  $chart_data['series'] = $series;

  return $chart_data;
}