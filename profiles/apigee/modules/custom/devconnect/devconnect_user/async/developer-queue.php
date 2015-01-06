<?php

if (!array_key_exists('argc', $_SERVER) || !array_key_exists('argv', $_SERVER)) {
  // This is not being invoked via the CLI.
  exit;
}
// Let it work on Pantheon, when there are no database declarations in
// settings.php
if ($_SERVER['argc'] > 1) {
  $_SERVER['PRESSFLOW_SETTINGS'] = $_SERVER['argv'][1];
}
if ($_SERVER['argc'] > 3) {
  $_SERVER['PANTHEON_SITE_NAME'] = $argv[2];
  $_SERVER['PANTHEON_ENVIRONMENT'] = $argv[3];
}
// Find base of Drupal install. Cannot use __DIR__ because it resolves
// symlinks, which may cause issues. Note that this necessitates invoking
// this script using its absolute (non-symlink-resolved) path.
$rootdir = $_SERVER['PHP_SELF'];
$parts = explode('/', $rootdir);
for ($i = count($parts) - 1; $i > 0; $i--) {
  $rootdir = join('/', array_slice($parts, 0, $i));
  if (file_exists("$rootdir/index.php") && file_exists("$rootdir/update.php")) {
    break;
  }
}

define('DRUPAL_ROOT', $rootdir);
require_once DRUPAL_ROOT . '/includes/bootstrap.inc';

// Make bootstrap process happy.
$_SERVER['REMOTE_ADDR'] = '127.0.0.1';
$_SERVER['REQUEST_METHOD'] = 'GET';
$_SERVER['SERVER_SOFTWARE'] = '';

drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);

devconnect_user_process_queue();
