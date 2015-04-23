<?php
/**
 * Implements hook_drush_command().
 *
 * @return array
 */
function apigee_checklist_drush_command() {
  $items = array();
  $items['apigee-healthcheck'] = array(
    'callback' => 'apigee_checklist_drush_healthcheck',
    'description' => dt('Phones home to Apigee’s BaaS to log health status.'),
    'aliases' => array('healthcheck'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_FULL,
  );
  return $items;
}

/**
 * Implements hook_drush_help().
 *
 * @param $section
 * @return null|string
 */
function apigee_checklist_help($section) {
  switch ($section) {
    case 'drush:apigee-healthcheck':
      return dt('This command gathers health-check info from the system and makes a REST call to Apigee’s BaaS service to write it to the log.');
  }
  return NULL;
}

function apigee_checklist_drush_healthcheck() {
  apigee_checklist_heartbeat(TRUE);
}