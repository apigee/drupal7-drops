<?php

if (!array_key_exists('argc', $_SERVER) || !array_key_exists('argv', $_SERVER)) {
  // This is not being invoked via the CLI.
  exit;
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

define ('DRUPAL_ROOT', $rootdir);
include_once DRUPAL_ROOT . '/includes/bootstrap.inc';

$_SERVER['REMOTE_ADDR'] = '127.0.0.1';
drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);

devconnect_user_process_queue();
