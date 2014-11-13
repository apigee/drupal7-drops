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
function hook_devconnect_developer_app_list_alter(array &$parameters) {
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
function hook_devconnect_developer_app_alter(array &$info) {
  if (count($info['raw_data']['parameters']) > 0 && !empty($info['raw_data']['parsed_attributes']['DisplayName'])) {
    $info['parsed_app_name'] = $info['raw_data']['parsed_attributes']['DisplayName'];
  }
  else {
    $info['parsed_app_name'] = $info['name'];
  }
  $info['page_title'] = $info['parsed_app_name'];
}

/**
 * Alter tabs or panes on the developer app details page.
 *
 * @param array $tabs
 *   Numerically-indexed array of tab links, containing 'text' and 'path'
 *   members (as well as optional 'options' member -- see l() for more info).
 *   Tab links' href may start with a hashmark, corresponding to the #id
 *   of a corresponding pane.
 * @param array $panes
 *   Numerically-indexed array of panes. Each pane is a Drupal render-array
 *   representing the contents of a tab-correspondent pane.
 */
function hook_devconnect_developer_app_details_alter(array &$tabs, array &$panes) {
  // swap the first two tabs
  $tabs2 = $tabs;
  $tabs2[0] = $tabs[1];
  $tabs2[1] = $tabs[0];
  $tabs = $tabs2;

  // swap the first two panes
  $panes2 = $panes;
  $panes2[0] = $panes[1];
  $panes2[1] = $panes[0];
  $panes = $panes2;
}

/**
 * Alter, augment or take other action right before a dev app is saved.
 *
 * If you do something to the developer_app entity in an implementation of this
 * hook such as deleting it, you should return FALSE here.
 *
 * @param $form_state (array)
 *   consists of $form_state from devconnect_developer_apps_edit_form
 * @return bool
 *   If any value other than FALSE is returned (including NULL or no value at
 *   all), further processing in saving the developer app is aborted.
 */
function hook_devconnect_developer_app_presave(array &$form_state) {
  $form_state['values']['attribute_drupal_uid'] = $form_state['values']['uid'];
  return TRUE;
}

/**
 * Take some action right after a developer app is saved.
 *
 * If you do something to the developer_app entity in an implementation of this
 * hook such as deleting it, you should return FALSE here.
 *
 * @param $results (associative array returned from KMS)
 * @param $form_state (array)
 *   consists of $form_state from devconnect_developer_apps_edit_form.
 *   $form_state['storage']['entity'] holds the developer_app entity.
 * @return bool
 *   If any value other than FALSE is returned (including NULL or no value at
 *   all), further processing in saving the developer app is aborted.
 */
function hook_devconnect_developer_app_save(array $results, array &$form_state) {
  $form_state['redirect'] = '<front>';
  return TRUE;
}

/**
 * Take some action right before a developer app is deleted.
 *
 * @param $form_state
 * @return bool
 *   If any value other than FALSE is returned (including NULL or no value at
 *   all), further processing in deleting the developer app is aborted.
 */
function hook_devconnect_developer_app_predelete(array &$form_state) {
  drupal_set_message('Deleting app!');
  return TRUE;
}

/**
 * Take some action right after a developer app is deleted.
 *
 * @param $results (associative array returned from KMS)
 * @param $form_state (array)
 *   consists of $form_state from devconnect_developer_apps_edit_form
 * @return bool
 *   If any value other than FALSE is returned (including NULL or no value at
 *   all), further processing in deleting the developer app is aborted.
 */
function hook_devconnect_developer_app_delete(array $results, array &$form_state) {
  drupal_set_message('App deleted!');
  return TRUE;
}

/**
 * Take some action before a user-app page is generated.
 *
 * @param string $op
 *   May be one of the following: list, detail, delete, edit
 * @param mixed $arg1
 *   If $op is 'list', 'detail' or 'edit', this is a reference to the $user object.
 *   If $op is 'edit', this is a reference to the $user object.
 *   Otherwise this is NULL
 * @param mixed $arg2
 *   If $op is 'edit', this is the app id (if set)
 *   Otherwise this is NULL
 */
function hook_devconnect_developer_apps_prerender($op, $arg1 = NULL, $arg2 = NULL) {
  if ($op == 'list') {
    if (!user_access('access foobar', $arg1)) {
      drupal_goto('<front>');
    }
  }
}

/**
 * Alter the list of available API Products.
 *
 * @param array $api_products
 * @param stdClass|null $account
 */
function hook_apiproduct_list_alter(array &$api_products, $account = NULL) {
  if ($account->uid == 1) {
    unset($api_products['worlds_greatest_api']);
  }
}

/**
 * Alter the label by which Developer Apps are called.
 *
 * @param string $label
 * @param mixed $form_value
 * @param bool $plural
 */
function hook_devconnect_developer_app_label_alter(&$label, $form_value, $plural) {
  if ($form_value == 3) {
    $label = 'Chimpanzee' . ($plural ? 's' : '');
  }
}