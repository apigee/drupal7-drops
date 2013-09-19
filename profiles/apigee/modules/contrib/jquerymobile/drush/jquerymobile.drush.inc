<?php

/**
 * @file
 *   drush integration for jquerymobile.
 */

/**
 * Implementation of hook_drush_command().
 *
 * In this hook, you specify which commands your
 * drush module makes available, what it does and
 * description.
 *
 * Notice how this structure closely resembles how
 * you define menu hooks.
 *
 * @See drush_parse_command() for a list of recognized keys.
 *
 * @return
 *   An associative array describing your command(s).
 */
function jquerymobile_drush_command() {
  $items = array();

  $items['jquerymobile'] = array(
    'description' => "Downloads the desired version of the jQuery Mobile framework and places it in the correct location.",
    'options' => array(
      'jqm' => dt('Enter the version of the jQuery Mobile framework you wish to download. If omitted Drush will use 1.0.1'),
      'jquery' => dt('Enter the version of the jQuery framework you wish to download. If omitted Drush will use 1.6.4'),
      'path' => dt('Optional. A path to the download folder. If omitted Drush will use the default location (sites/all/libraries).'),
    ),
   'bootstrap' => DRUSH_BOOTSTRAP_FULL,
  );
  
    return $items;
}

/**
 * Example drush command callback.
 *
 * This is where the action takes place.
 *
 * In this function, all of Drupals API is (usually) available, including
 * any functions you have added in your own modules/themes.
 *
 * To print something to the terminal window, use drush_print().
 *
 */
function drush_jquerymobile() {
  $jqm = drush_get_option('jqm');
  $jquery = drush_get_option('jquery');
  $path = drush_get_option('path');
  
  if (is_null($jqm)) {
    $jqm = '1.0.1';
  }
  if (is_null($jquery)) {
    $jquery = '1.6.4';
  }
  if (is_null($path)) {
    $path = 'sites/all/libraries';
  }
  
  variable_set('jquerymobile_library_path', $path);

  $path = drush_get_context('DRUSH_DRUPAL_ROOT') . '/' . $path;
  
  // Download jQuery Mobile
  if (!is_dir($path)) {
    drush_op('mkdir', $path);
    drush_log(dt('Directory @path was created', array('@path' => $path)), 'notice');
  }
  
  drush_op('chdir', $path);
  if (drush_shell_exec('wget http://code.jquery.com/mobile/' . $jqm . '/jquery.mobile-' . $jqm . '.zip')) {
    drush_log(dt('jquery.mobile-' . $jqm . '.zip has been downloaded to @path', array('@path' => $path)), 'success');
  }
  else {
    drush_log(dt('Drush was unable to download jquery.mobile-' . $jqm . '.zip to @path', array('@path' => $path)), 'error');
  }
  
  if (drush_shell_exec('unzip jquery.mobile-' . $jqm . '.zip')) {
    drush_log(dt('jquery.mobile-' . $jqm . '.zip has been extacted to @path', array('@path' => $path)), 'success');
  }
  else {
    drush_log(dt('Drush was unable to extract jquery.mobile-' . $jqm . '.zip to @path', array('@path' => $path)), 'error');
  }
  
  if (drush_shell_exec('rm jquery.mobile-' . $jqm . '.zip')) {
    drush_log(dt('jquery.mobile-' . $jqm . '.zip has been removed from @path', array('@path' => $path)), 'success');
  }
  else {
    drush_log(dt('Drush was unable to remove jquery.mobile-' . $jqm . '.zip from @path', array('@path' => $path)), 'error');
  } 
  
  // Download jQuery
  $files = array('.js', '.min.js');

  if (!is_dir($path . '/jquery-' . $jquery)) {
    drush_op('mkdir', $path . '/jquery-' . $jquery);
    drush_log(dt('Directory @path was created', array('@path' => $path . '/jquery-' . $jquery)), 'notice');
  }
  
  drush_op('chdir', $path . '/jquery-' . $jquery);

  foreach ($files as $key => $file) {
    if (drush_shell_exec('wget http://code.jquery.com/jquery-' . $jquery . $file)) {
      drush_log(dt('jquery-' . $jquery . $file . ' has been downloaded to @path', array('@path' => $path . '/jquery-' . $jquery)), 'success');
    }
    else {
      drush_log(dt('Drush was unable to download jquery-' . $jquery . $file . ' to @path', array('@path' => $path . '/jquery-' . $jquery)), 'error');
    }
  }
  
  variable_set('jquerymobile_jquerymobile_version', $jqm);
  variable_set('jquerymobile_jquery_version', $jquery);
}

/**
 * Implements drush_MODULE_post_COMMAND().
 */
function drush_jquerymobile_post_pm_enable() {
  $extensions = func_get_args();
  // Deal with comma delimited extension list.
  if (strpos($extensions[0], ', ') !== FALSE) {
    $extensions = explode(', ', $extensions[0]);
  }

  if (in_array('jquerymobile', $extensions) && !drush_get_option('skip')) {
    drush_jquerymobile();
  }
}
