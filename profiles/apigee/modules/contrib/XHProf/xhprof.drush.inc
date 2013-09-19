<?php

/**
 * @file
 */

/**
 * Implements hook_drush_command().
 */
function xhprof_drush_command() {
  $items['xhprof-list'] = array(
    'description' => dt(''),
    'arguments' => array(
    ),
  );
  $items['xhprof-combine'] = array(
    'description' => dt(''),
    'arguments' => array(
    ),
  );
  $items['xhprof-clear'] = array(
    'description' => dt(''),
    'arguments' => array(
    ),
  );
  return $items;
}

/**
 * A command callback.
 */
function drush_xhprof_combine() {
  $run_ids = func_get_args();
  $runs = drush_xhprof_get_runs();

  if (empty($run_ids)) {
    $options = array_keys($runs);
    $options[] = 'All';
    // return dt("You must provide a run id argument!");
    //$choice = drush_choice_multiple($options, FALSE, 'Select xhprof runs to combine.');
    $choice = drush_choice($options, 'Select xhprof runs to combine.');
    drush_print_r($options[$choice]);

    if ($choice !== FALSE) {
      if ($options[$choice] == 'All') {
        $run_ids = array_keys($runs);
      }
      else {
        // TODO: This doesn't work. Probably going to have to use drush_choice_multiple.
        $ids = explode(",", $choice);
        foreach ($ids as $id) $run_ids[] = $options[$id];
      }
    }
  }

  $xhprof = new XHProfRuns_Default();
  $runs = drush_xhprof_get_runs();
  $run_data = array();
  $keys = array();

  $desc = "";
  foreach ($run_ids as $run_id) {
    if (isset($runs[$run_id])) {
      $run = $runs[$run_id];
      if ($data = $xhprof->get_run($run['run_id'], $run['source'], $desc)) {
        $run_data[] = $data;
        $keys = $keys + array_keys($data);
      }
    }
  }

  $agg_run = array();
  $run_count = count($run_data);
  foreach ($keys as $key) {
    $agg_key = array();
    // Check which runs have this parent_child function key, collect metrics if so.
    foreach ($run_data as $data) {
      if (isset($data[$key])) {
        foreach ($data[$key] as $metric => $val) {
          $agg_key[$metric][] = $val;
        }
      }
    }

    // Average each metric for the key into the aggregated run.
    $agg_run[$key] = array();
    foreach ($agg_key as $metric => $vals) {
      $agg_run[$key][$metric] = (array_sum($agg_key[$metric]) / count($agg_key[$metric]));
    }
  }

  $namespace = 'drush-' . variable_get('site_name', '');
  drupal_alter('xhprof_namespace', $namespace);
  if ($agg_run_id = $xhprof->save_run($agg_run, $namespace)) {
    drush_print(dt("Aggregated run id: !id", array('!id' => $agg_run_id)));
  }
  else {
    drush_print(dt("Unable to save aggregated xhprof data!"));
  }
}

/**
 * A command callback.
 */
function drush_xhprof_list() {
  $runs = drush_xhprof_get_runs();
  $rows = array();
  $headers = array();
  foreach ($runs as $run) {
    $row = array();
    foreach ($run as $key => $value) {
      if (!in_array($key, $headers)) $headers[] = $key;

      $row[] = $value;
    }
    $rows[] = array_reverse($row);
  }

  $headers = array_reverse($headers);
  array_unshift($rows, $headers);

  drush_print_table($rows, TRUE);
}


/**
 * A command callback.
 */
function drush_xhprof_clear() {
  drush_xhprof_get_dir();

  foreach (drush_xhprof_list_run_files() as $file) {
    drush_register_file_for_deletion($file);
  }
}

function drush_xhprof_get_dir() {
  $dir = ini_get("xhprof.output_dir");
  return (!empty($dir) && is_dir($dir)) ? $dir = "/tmp" : FALSE;
}

function drush_xhprof_list_run_files() {
  $dir = drush_xhprof_get_dir();
  $files = array();
  foreach (glob("{$dir}/*.xhprof") as $file) $files[] = $file;
  return $files;
}
function drush_xhprof_get_runs() {
  $dir = drush_xhprof_get_dir();
  $run_ids = array();

  foreach (glob("{$dir}/*.xhprof") as $file) {
    $run = array();
    list($run['run_id'], $run['source']) = explode('.', basename($file));
    $runs[$run['run_id']] = $run;
  }

  return $runs;
}
