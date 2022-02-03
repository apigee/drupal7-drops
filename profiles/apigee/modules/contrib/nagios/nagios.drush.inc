<?php
/**
 * @file
 * Provides drush integration for the Nagios module.
 */

/**
 * Implements hook_drush_command().
 */
function nagios_drush_command() {
  $items['nagios-list'] = [
    'description' => dt('List all available nagios commands.'),
    'arguments' => [],
    'aliases' => ['nagios-ls'],
  ];
  $items['nagios-check'] = [
    'description' => dt('Run a nagios command.'),
    'arguments' => ['command' => 'The command to run'],
    'aliases' => ['nagios'],
  ];
  $items['nagios-updates'] = [
    'description' => dt('Return a space delimited string of all monitored projects with available updates.'),
    'arguments' => [
      'update type' => 'Type of update available, accepts either all or security.',
      'nagios flag' => 'Send \'nagios\' as a flag to enable Nagios prefixes to the output.',
    ],
    'aliases' => ['nagios-up'],
  ];
  return $items;
}

/**
 * Drush command callback: nagios-list.
 */
function drush_nagios_list() {
  // Invoke hook_nagios_checks() here so people can add in their own checks.
  $commands = array_merge(module_invoke_all('nagios_checks'), drush_command_invoke_all('nagios_checks'));
  $rows = [];
  foreach ($commands as $cmd => $desc) {
    $rows[] = [$cmd, $desc];
  }
  return drush_print_table($rows);
}

/**
 * Drush command callback: nagios-check.
 */
function drush_nagios_check($check = NULL) {
  if (isset($check)) {
    foreach (module_implements('nagios_checks') as $module) {
      if (array_key_exists($check, module_invoke($module, 'nagios_checks'))) {
        nagios_check($module, $check);
        return;
      }
    }
    foreach (drush_command_implements('nagios_checks') as $command) {
      /** @var callable $function */
      $function = $command . '_nagios_checks';
      if (array_key_exists($check, $function())) {
        nagios_check($command, $check);
        return;
      }
    }
    drush_set_error(dt('The requested nagios check does not exist: !check.', ['!check' => $check]));
  }
  else {
    drush_set_error(dt('Missing argument for nagios-check'));
  }
  drush_set_error(dt('Run `drush nagios-list` for valid checks.'));
}

/**
 * Helper function for drush_nagios_check().
 */
function nagios_check($module, $check) {
  $function = $module . '_nagios_check_' . $check;
  if (function_exists($function)) {
    $nagios_data = $function();
  }
  else {
    $function = $module . '_nagios_check';
    $nagios_data = $function($check);
  }
  $key = key($nagios_data);
  $status = nagios_status();
  $exit_status = (int) $nagios_data[$key]['status'];
  $message = $key . ' ' . $status[$exit_status] . ' - ' . $nagios_data[$key]['text'];
  if ($nagios_data[$key]['type'] == 'perf') {
    $message .= '|' . $key . '=' . $nagios_data[$key]['text'];
  }
  // We want to exit with the correct exit status so Nagios knows the outcome
  // of our drush nagios check so set DRUSH_EXECUTION_COMPLETED to true.
  drush_set_context('DRUSH_EXECUTION_COMPLETED', TRUE);
  drush_print($message);

  $users = array_unique(_nagios_update_os_user());
  if (count($users) > 1 && $exit_status != NAGIOS_STATUS_OK) {
    $warning = dt('Warning') . ': ';
    $warning .= dt('All nagios checks should be executed as the same user as the web page.') . "\n";
    $warning .= dt('This is important when modules confirm file system permissions are correct.') . "\n";
    $warning .= dt('You can use `sudo -u` to run drush under a different user.') . "\n";
    drush_print($warning);

    $rows = drush_key_value_to_array_table($users);
    array_unshift($rows, ['PHP_SAPI', '', strtoupper(dt('Operating system user'))]);
    drush_print_table($rows, TRUE);
  }

  exit($exit_status);
}

/**
 * Drush command callback: nagios-updates.
 *
 * @param string $update_type
 * A string containing:
 * - 'all' to list all updates.
 * - 'security' to list security updates only.
 *
 * @param integer $nagios_flag
 * Send 'nagios' to prefix the output with a Nagios string.
 */
function drush_nagios_updates($update_type = NULL, $nagios_flag = NULL) {
  $flag_insecure = FALSE;
  $exit_status = NULL;
  $message = '';
  // Need to ensure the Update module was installed before continuing.
  if (db_table_exists('cache_update')) {
    if ($available = _nagios_update_get_available(TRUE)) {
      module_load_include('inc', 'update', 'update.compare');
      $data = update_calculate_project_data($available);
      // Unset any data referencing projects we're not interested in.
      $nagios_ignored_modules = variable_get('nagios_ignored_modules', []);
      $nagios_ignored_themes = variable_get('nagios_ignored_themes', []);
      $nagios_ignored_projects = $nagios_ignored_modules + $nagios_ignored_themes;
      foreach ($nagios_ignored_projects as $key => $value) {
        if ($value == TRUE && isset($data[$key])) {
          unset($data[$key]);
        }
      }

      $module_list = [];

      $updates = nagios_updates($data);
      if (count($updates)) {
        foreach ($updates as $project_name => $update_info) {
          // In any case we need to flag security updates.
          if ($update_info['status'] == UPDATE_NOT_SECURE) {
            $flag_insecure = TRUE;
            $module_list[] = $project_name;
          }
          // If type is 'security' we don't want a full list.
          elseif ($update_type && $update_type == 'all') {
            $module_list[] = $project_name;
          }
        }
        if ($nagios_flag == 'nagios') {
          if ($flag_insecure) {
            // Returning the value didn't work in drush 5.x, so we print it instead.
            $message = 'ADMIN CRITICAL - Updates required for: ' . implode(' ', $module_list);
            $exit_status = 2;
          }
          else {
            if (empty($module_list)) {
              $message = 'ADMIN WARNING - Updates required, but no security updates.';
            }
            else {
              $message = 'ADMIN WARNING - Updates required for: ' . implode(' ', $module_list);
            }
            $exit_status = 1;
          }
        }
        else {
          $message = implode(' ', $module_list);
          // Set $exit_status to zero so any dependent scripts do not get a false failure.
          $exit_status = 0;
        }
      }
    }
    if ($nagios_flag == 'nagios' && $exit_status === NULL) {
      $message = 'OK - No updates';
    }
  }
  else {
    $message = 'The core update module was never installed so we cannot use update check features.';
    // Set a warning, because someone is clearly explicitly trying to use this feature and cannot.
    $exit_status = 1;
  }

  drush_set_context('DRUSH_EXECUTION_COMPLETED', TRUE);
  if ($exit_status !== NULL || $nagios_flag == 'nagios') {
    drush_print($message);
  }
  exit($exit_status);
}

/**
 * Helper function for drush_nagios_updates().
 */
function nagios_updates($data) {
  if (!is_array($data)) {
    return NULL;
  }

  $updates = [];

  // Create an array of status values keyed by module or theme name, since
  // we'll need this while generating the report if we have to cross reference
  // anything (e.g. subthemes which have base themes missing an update).
  foreach ($data as $project) {
    foreach ($project['includes'] as $key => $name) {
      $status[$key] = $project['status'];
    }
  }

  foreach ($data as $project) {
    if ($project['status'] == UPDATE_NOT_SECURE || $project['status'] == UPDATE_NOT_CURRENT) {
      $row_key = drupal_strtolower($project['name']);
      if (!isset($updates[$row_key])) {
        $updates[$row_key] = [];
      }
      $updates[$row_key]['type'] = $project['project_type'];
      $updates[$row_key]['status'] = $project['status'];
    }
  }

  return $updates;
}

