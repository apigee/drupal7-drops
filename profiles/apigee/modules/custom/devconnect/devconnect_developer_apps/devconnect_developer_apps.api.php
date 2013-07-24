<?php

/**
 * Alter, remove from, or add to parameters in the developer app list.
 *
 * @param array $parameters
 *   By default, $parameters will include the following members:
 *     application_count (integer)
 *     applications (array of associative arrays -- see below)
 *     user (stdClass user object)
 *   Each member of the applications array has the following keys:
 *     app_name (string)
 *     callback_url (string)
 *     attributes (associative array of strings)
 *     credential (associative array -- see documentation under hook_devconnect_developer_app_alter below)
 *     delete_url (string)
 *     edit_url (string)
 *     raw_data (associative array returned from KMS)
 */
function hook_devconnect_developer_app_list_alter(&$parameters) {
  foreach (array_keys($parameters['applications']) as $i) {
    $app =& $parameters['applications'][$i];
    if (count($app['raw_data']['parameters']) > 0 && !empty($app['raw_data']['parsed_attributes']['DisplayName'])) {
      $app['parsed_app_name'] = $app['raw_data']['parsed_attributes']['DisplayName'];
    }
    else {
      $app['parsed_app_name'] = $app['name'];
    }
  }
}

/**
 * Alter, remove from, or add to themeable info for a developer app.
 *
 * @param $info
 *   By default, $info will include the following members
 *     account (stdClass user object of app owner)
 *     access_type (string, probably unused, probably here due to a bug)
 *     callback_url
 *     name
 *     status (dev app status, not apiproduct status)
 *     attributes (array, currently unused)
 *     credentials (array of credential info -- see below)
 *     app_attributes (array)
 *     analytics_chart (string -- only here if this instance is so configured)
 *     page_title (string; will be set as the page title)
 *     breadcrumb (array; will be set as the Drupal breadcrumb)
 *     raw_data (associative array returned from KMS)
 *   Each member of the credentials array consists of the following:
 *     consumer_key (string)
 *     consumer_secret (string)
 *     status (this is the credential status)
 *     apiproducts (array with the following keys:)
 *       display_name
 *       description
 *       status
 *       name (*this* is the apiproduct status)
 */
function hook_devconnect_developer_app_alter(&$info) {
  if (count($info['raw_data']['parameters']) > 0 && !empty($info['raw_data']['parsed_attributes']['DisplayName'])) {
    $info['parsed_app_name'] = $info['raw_data']['parsed_attributes']['DisplayName'];
  }
  else {
    $info['parsed_app_name'] = $info['name'];
  }
  $info['page_title'] = $info['parsed_app_name'];
}

/**
 * Alter, augment or take other action right before a dev app is saved.
 *
 * @param $form_state (array)
 *   consists of $form_state from devconnect_developer_apps_edit_form
 */
function hook_devconnect_developer_app_presave(&$form_state) {
  $form_state['values']['attribute_drupal_uid'] = $form_state['values']['uid'];
}

/**
 * Take some action right after a developer app is saved.
 *
 * @param $results (associative array returned from KMS)
 * @param $form_state (array)
 *   consists of $form_state from devconnect_developer_apps_edit_form.
 *   $form_state['storage']['app'] holds the Apigee\ManagementAPI\DeveloperApp
 *   object.
 */
function hook_devconnect_developer_app_save($results, &$form_state) {
  $form_state['redirect'] = '<front>';
}

/**
 * Take some action right before a developer app is deleted.
 *
 * @param $form_state
 */
function hook_devconnect_developer_app_predelete(&$form_state) {

}

/**
 * Take some action right after a developer app is deleted.
 *
 * @param $results (associative array returned from KMS)
 * @param $form_state (array)
 *   consists of $form_state from devconnect_developer_apps_edit_form
 */
function hook_devconnect_developer_app_delete($results, &$form_state) {

}