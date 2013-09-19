<?php
/**
 * @file
 *   drush commands to initiate site imports
 *
 */


/**
 * Implements hook_drush_command().
 *
 * drush auto-command-loader assumes a command name scheme of
 *
 * "drush_{commandfilename}_{command}"
 *
 *
 * @return   An associative array describing command(s).
 */
function import_html_drush_command() {
  $items = array();

  $items['import-html'] = array(
    'description' => "Start an import using the appropriate import_html settings profile, a site base location, and the given relative path",
    'options' => array(
      '--profile-id' => 'ID of the import_html settings profile. Defaults to the one last used if not set.',
      '--source-siteroot' => 'Base siteroot to import from. Uses the currently active path if not set.',
    ),
    'arguments' => array(
      'modules' => 'A space delimited list of relative paths, taken from underneath the source siteroot location. Paths can be html files or folders. Folders will be recursed IF they end with a "/". Use "." to indicate the source siteroot itself ',
    ),
    'examples' => array(
      'drush import-html about/index.html' => 'Imports just the "about" page from the currently configured site.',
      'drush import-html about/' => 'Recursively imports the contents of the "about" folder from the currently configured site.',
      'drush import-html about' => 'Don\'t do this. It will not recurse, and will just create a placeholder item for that section.',
      'drush import-html --profile-id=custom_settings --source-siteroot=/var/www/archive .' => 'Imports everything from the named path using custom settings.',
    ),
    # Needs a working site
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_FULL,
  );

  return $items;
}


/**
 * Implements hook_drush_help().
 *
 * @param   A string with the help section (prepend with 'drush:')
 *
 * @return
 *   A string with the help text for your command.
 */
function import_html_drush_help($section) {
  // recycle the descriptions from above.
  $commands = import_html_drush_command();
  list($drush, $command_name) = split(':', $section);
  if (isset($commands[$command_name]) && $command_info = $commands[$command_name]) {
    return $command_info['description'];
  }
}

/**
 * Perform the action 'import-html'
 */
function drush_import_html() {
  $file_list = func_get_args();
  if (empty($file_list)) {
    drush_log('No paths given. You must include at least one relative path to define the page or folder to be imported', 'error');
    return FALSE;
  }
  $context = array();
  $context['profile_id'] = drush_get_option(array('profile-id'), import_html_current_profile_id());
  $context['source_siteroot'] = drush_get_option(array('source-siteroot'), variable_get('import_html_source_siteroot', '/var/www/old'));
  drush_print(dt("Initiating import. Using settings from %profile_id to import content from underneath %source_siteroot", array('%profile_id' => $context['profile_id'], '%source_siteroot' => $context['source_siteroot'])));

  global $user;
  if ($user->uid != 1) {
    drush_log('import_html really only works as user #1. Use the "drush -u 1 " option on the commandline, or mysterious things may fail due to permissions during the node creation process. User was ' . $user->uid . ' - switching to uid1 now', 'notice');
    $user = user_load(1);
  }


  variable_set('import_html_last_import_timestamp', REQUEST_TIME);
  // drush_print_r($file_list);
  // $profile = import_html_profile_load($context['profile_id']);
  import_html_import_files($file_list, $context);

}
