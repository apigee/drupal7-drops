<?php

define('DEVCONNECT_ANALYTICS_MAX_XAXIS_LINES', 12);
define('DEVCONNECT_ANALYTICS_MAX_YAXIS_LINES', 10);

/**
 * Returns a "singleton" of the analytics object, or rather,
 * the closest PHP can come to a singleton.
 *
 * @return Apigee\ManagementAPI\DeveloperAppAnalytics
 */
function &_devconnect_developer_apps_analytics_api_client() {
  static $dca;

  if (!isset($dca)) {
    module_load_include("module", "devconnect");
    $client = devconnect_default_api_client();
    $dca = new Apigee\ManagementAPI\DeveloperAppAnalytics($client);
  }
  return $dca;
}

function _devconnect_developer_apps_analytics_metrics() {
  return array(
    'total_response_time' => t('Throughput'),
    'max_response_time' => t('Max Response Time'),
    'min_response_time' => t('Min Response Time'),
    'end_point_response_time' => t('Endpoint Response Time'),
    'message_count' => t('Message Count'),
    'error_count' => t('Error Count'),
    'user_count' => t('User Count'),
  );
}

function devconnect_developer_apps_analytics_form() {
  $form['#attributes']['class'] = array('form-inline', 'analytics-form');

  $metric_options = _devconnect_developer_apps_analytics_metrics();
  $metric_default = (isset($_GET['metric']) && isset($metric_options[$_GET['metric']]) ? $_GET['metric'] : 'total_response_time');

  $form['metric'] = array(
    '#type' => 'select',
    '#title' => t('Display'),
    '#required' => TRUE,
    '#multiple' => FALSE,
    '#options' => $metric_options,
    '#default_value' => $metric_default
  );

  $from = 0;
  if (isset($_GET['from'])) {
    $from = strtotime($_GET['from']);
  }
  if (!$from) {
    $from = strtotime('-1 day');
  }
  $from_text = gmdate('m/d/Y H:i', $from) . ' UTC';

  drupal_add_library('system','ui.datepicker');
  drupal_add_js('jQuery(document).ready(function(){
    jQuery("#edit-date-from" ).datetimepicker();
    jQuery("#edit-date-to" ).datetimepicker();
  });', 'inline');


  $form['date_from'] = array(
    '#type' => 'textfield',
    '#title' => t('From'),
    '#required' => TRUE,
    '#size' => 25,
    '#default_value' => $from_text,
    '#attributes' => array('class' => array('pickadate'))
  );

  $to = 0;
  if (isset($_GET['to'])) {
    $to = strtotime($_GET['to']);
  }
  if (!$to) {
    $to = time();
  }
  $to_text = gmdate('m/d/Y H:i', $to) . ' UTC';

  $form['date_to'] = array(
    '#type' => 'textfield',
    '#title' => t('To'),
    '#required' => TRUE,
    '#size' => 25,
    '#default_value' => $to_text,
    '#attributes' => array('class' => array('pickadate'))
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Submit',
  );

  return $form;
}

function devconnect_developer_apps_analytics_form_validate($form, &$form_state) {
  $from = $form_state['values']['date_from'];
  $to = $form_state['values']['date_to'];

  $date_error = FALSE;

  $parsed_from = @strtotime($from);
  $parsed_to = @strtotime($to);

  if ($parsed_from === FALSE) {
    form_set_error('date_from', t('Invalid "From" date.'));
    $date_error = TRUE;
  }
  if ($parsed_to === FALSE) {
    form_set_error('date_to', t('Invalid "To" date.'));
    $date_error = TRUE;
  }
  if (!$date_error) {
    if ($parsed_from >= $parsed_to) {
      form_set_error('date_from', '"From" date must be earlier than "To" date.');
      $date_error = TRUE;
    }
  }
  if (!$date_error) {
    $form_state['storage']['from'] = $parsed_from;
    $form_state['storage']['to'] = $parsed_to;
  }
}

function devconnect_developer_apps_analytics_form_submit($form, &$form_state) {
  $from = $form_state['storage']['from'];
  $to = $form_state['storage']['to'];
  $metric = $form_state['values']['metric'];

  $args = array('from' => $from, 'to' => $to, 'metric' => $metric);

  $url = join('/', arg());
  drupal_goto($url, array('query' => $args, 'fragment' => 'performance'));
}

function devconnect_developer_apps_analytics_performance($app_name) {
  $dca = _devconnect_developer_apps_analytics_api_client();

  // Are we showing proof-of-concept, or real data?
  $debug = ($dca->get_client()->get_org() == 'enterprisestickers');
  if ($debug) {
    $app_name = 'APIConsole';
  }

  $metrics = _devconnect_developer_apps_analytics_metrics();
  $units = array(
    'second', 'minute', 'hour', 'day', 'week', 'month', 'year'
  );

  if (isset($_GET['metric']) && isset($metrics[$_GET['metric']])) {
    $metric = $_GET['metric'];
  }
  else {
    $metric = 'total_response_time';
  }

  if (isset($_GET['to']) && is_int($_GET['to'])) {
    $end = hexdec($_GET['to']);
  }
  else {
    $end = time();
  }

  if (isset($_GET['from']) && is_int($_GET['from']) && $_GET['from'] < $end) {
    $start = $_GET['from'];
  }
  else {
    $start = strtotime('-24 hours', $end);
  }

  if (isset($_GET['unit']) && in_array($_GET['unit'], $units)) {
    $unit = $_GET['unit'];
  }
  else {
    $unit = 'hour';
  }
  $order = 'ASC';

  // 4G accepts UTC dates.
  $end_utc = strtotime(gmdate('Y-m-d H:i:s', $end));
  $start_utc = strtotime(gmdate('Y-m-d H:i:s', $start));

  // TODO: use user's configured timezone
  // hack for now
  $start = $start_utc;
  $end = $end_utc;

  $time_points = array();
  switch ($unit) {
    case 'second':
      $date_format = 'H:i:s';
      $current_time = $start;
      while ($current_time <= $end) {
        $time_points[] = $current_time;
        $current_time++;
      }
      break;
    case 'minute':
      $date_format = 'H:i';
      $current_time = $start - ($start % 60);
      while ($current_time <= $end) {
        $time_points[] = $current_time;
        $current_time += 60;
      }
      break;
    case 'hour':
      $date_format = 'H:00';
      $current_time = $start - ($start % 3600);
      while ($current_time <= $end) {
        $time_points[] = $current_time;
        $current_time += 3600;
      }
      break;
    case 'day':
      $date_format = 'j M';
      $current_time = $start - ($start % 86400);
      while ($current_time <= $end) {
        $time_points[] = $current_time;
        $current_time += 86400;
      }
      break;
    case 'week':
      $date_format = 'j M';
      $week_seconds = 86400 * 7;
      $current_time = $start - ($start % $week_seconds);
      while ($current_time <= $end) {
        $time_points[] = $current_time;
        $current_time += $week_seconds;
      }
      break;
    case 'month':
      $date_format = 'M Y';
      $current_time = mktime(0, 0, 0, date('m', $start), 1, date('Y', $start));
      while ($current_time <= $end) {
        $time_points[] = $current_time;
        $current_time = strtotime('+1 month', $current_time);
      }
      break;
    case 'year':
      $date_format = 'Y';
      $current_time = mktime(0, 0, 0, 1, 1, date('Y', $start));
      while ($current_time <= $end) {
        $time_points[] = $current_time;
        $current_time = strtotime('+1 year', $current_time);
      }
      break;
  }
  $default_datapoints = array();
  foreach ($time_points as $time_point) {
    $default_datapoints[$time_point] = 0;
  }

  try {
    if (isset($_GET['env'])) {
      $dca->set_environment($_GET['env']);
    }
    $datapoints = $dca->get_by_app_name($app_name, $metric, $start, $end, $unit, $metric, $order);
  }
  catch (Apigee\Exceptions\EnvironmentException $e) {
    drupal_set_message($e->getMessage(), 'error');
    drupal_not_found();
    exit;
  }
  catch (Apigee\Exceptions\InvalidDataException $e) {
    drupal_set_message($e->getMessage(), 'error');
    drupal_not_found();
    exit;
  }
  catch (Exception $e) {
    //TODO FIXME
    print $e->getMessage(); die();
  }

  if (count($datapoints) == 0) {
    $output = NULL;
  }
  else {
    drupal_add_css(drupal_get_path('module', 'devconnect_developer_apps') . '/devconnect_developer_apps_analytics_svg.css');

    $chart = array(
      'chart_width' => 960,
      'chart_height' => 250,
      'x_translate' => 50,
      'y_translate' => 5,
      'vertical_lines' => array(),
      'horizontal_lines' => array(),
      'axis_overhang' => 5,
      'x_axis' => NULL,
      'y_axis' => NULL,
      'y_axis_title' => NULL,
      'data_lines' => array(),
      'actual_chart_width' => 0,
      'actual_chart_height' => 0
    );

    $app = $datapoints[$app_name];

    // Read in all values for all environments.
    $max_value = 0;
    $data = array();
    foreach ($app as $env => $env_datapoints) {
      $data[$env] = $default_datapoints;
      foreach ($env_datapoints as $time_point => $value) {
        $data[$env][$time_point] = $value;
      }
      $local_max = max($data[$env]);
      $max_value = max($local_max, $max_value);
    }
    $max_value = ceil($max_value);

    $digit_count = strlen($max_value);
    switch ($digit_count) {
      case 1: case 2:
      $round_to = 1;
      break;
      case 3:
        $round_to = 5;
        break;
      default:
        $round_to = pow(10, floor($digit_count / 2));
        break;
    }

    // figure out how wide the actual chart is, with all X-points being on integral boundaries
    $actual_chart_width = 860;
    $x_step = ($actual_chart_width / (count($default_datapoints) - 1));
    $actual_chart_width = $x_step * (count($default_datapoints) - 1);

    // figure out how high the actual chart is, with all Y-points being on integral boundaries
    $actual_chart_height = 200;
    $y_step = ($actual_chart_height / $max_value); // floor?
    $actual_chart_height = $y_step * $max_value;

    // Now calculate vertical lines
    $x_values = array();
    for ($x = 0; floor($x) <= $actual_chart_width; $x += $x_step) {
      $x_values[] = $x;
    }
    $step = ceil(count($x_values) / DEVCONNECT_ANALYTICS_MAX_XAXIS_LINES);
    $last_time = 0;
    $x_count = count($x_values);
    foreach ($x_values as $index => $x) {
      if ($index % $step != 0 && $index != $x_count - 1) {
        continue;
      }
      $which_time = $time_points[$index];
      $date = date($date_format, $which_time);

      $line = new stdClass;
      $line->x = $x;
      $line->y1 = 0;
      $line->y2 = $actual_chart_height;
      $line->caption = $date;
      // If granularity < day and date has changed, show date under time.
      if (substr($date_format, 0, 1) == 'H' && ($last_time == 0 || date('d', $which_time) != date('d', $last_time))) {
        $line->subcaption = date('F j', $which_time);
      }
      $chart['vertical_lines'][] = $line;
      $last_time = $which_time;
    }

    // Horizontal lines next
    $y_values = array();
    for ($y = 0; $y <= $actual_chart_height; $y += $y_step) {
      $y_values[] = $actual_chart_height - $y;
    }
    $step = ceil(count($y_values) / DEVCONNECT_ANALYTICS_MAX_YAXIS_LINES);
    if ($round_to > 1) {
      $difference = $step % $round_to;
      if ($difference > 0) {
        if ($difference < $round_to / 2) {
          $step -= $difference;
        }
        else {
          $step += ($round_to - $difference);
        }
      }
    }
    $y = $actual_chart_height;
    $actual_value = 0;
    do {
      $y -= ($y_step * $step);
      $actual_value += $step;
      $line = new stdClass;
      $line->y = $y;
      $line->x1 = 0;
      $line->x2 = $actual_chart_width;
      $line->caption = $actual_value;
      $chart['horizontal_lines'][] = $line;

    } while ($y > 0);

    $extra_y_axis = 0;
    if ($y < 0) {
      foreach (array_keys($chart['vertical_lines']) as $i) {
        $chart['vertical_lines'][$i]->y1 = $y;
      }
      $chart['y_translate'] -= $y;
      $extra_y_axis = $y;
    }

    // X axis
    $axis = new stdClass;
    $axis->x1 = 0;
    $axis->x2 = $actual_chart_width;
    $axis->y = $actual_chart_height;
    $chart['x_axis'] = $axis;

    // Y axis
    $axis = new stdClass;
    $axis->x = 0;
    $axis->y1 = $extra_y_axis;
    $axis->y2 = $actual_chart_height;
    $chart['y_axis'] = $axis;

    // Y axis title
    $title = new stdClass;
    $title->x = -40;
    $title->y = floor($actual_chart_height / 2);
    if (substr($metric, -5) == '_time') {
      $title->caption = 'Milliseconds';
    }
    else {
      $title->caption = $metrics[$metric];
    }
    $chart['y_axis_title'] = $title;

    foreach ($data as $env => $points) {
      $line = new stdClass;
      $line->caption = $env;
      $line->color = _devconnect_developer_apps_analytics_next_color();
      $line->points = array();
      foreach (array_values($points) as $x => $y) {
        $line->points[] = (object)array('x' => $x_step * $x, 'y' => $actual_chart_height - ($y_step * $y));
      }
      $chart['data_lines'][] = $line;
    }

    $chart['actual_chart_width'] = $actual_chart_width;
    $chart['actual_chart_height'] = $actual_chart_height;

    $output = theme('devconnect_developer_line_chart', $chart);

    if ($debug) {
      $output .= '<h1>Debug: Sample Data Only</h1>';
    }

    //$output .= kpr($result, TRUE);
  }

  return $output;
}

function _devconnect_developer_apps_analytics_next_color() {
  static $i = 0;
  $colors = array(
    '#ff8c00', // dark orange
    '#800080', // purple
    '#228b22', // forest green
    '#4169e1', // royal blue
    '#c71585', // medium violet red
    '#a0522d', // sienna
    '#556b2f', // dark olive green
  );
  if (isset($colors[$i])) {
    $color = $colors[$i];
  }
  else {
    // we're out of presets, so generate something random
    $color = '#';
    for ($j = 0; $j < 3; $j++) {
      $color .= str_pad(dechex(mt_rand(0, 200)), 2, '0', STR_PAD_LEFT);
    }
  }
  $i++;
  return $color;
}
