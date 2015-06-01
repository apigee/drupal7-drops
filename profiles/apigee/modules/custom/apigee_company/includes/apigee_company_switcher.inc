<?php
/**
 * @file
 * The company context switcher.
 *
 * This file contains code for the company context switcher dropdown, which
 * enables the developers to switch between companies.
 */

/**
 * Generates the drop down for switching companies.
 *
 * @global type $user
 * @return string
 *   HTML for the drop down
 */
function apigee_company_context_switcher() {
  global $user;

  // Do not show switcher if user is not logged in.
  if (!user_is_logged_in()) {
    return;
  }

  $user_company = array(
    'name' => NULL,
    'display_name' => format_username($user),
    'url' => 'api_company/company/switch/',
  );

  $default_company = apigee_company_get_current_context_company();

  $company_list = array();
  $companies = apigee_company_get_company_list();
  $default_company_info = NULL;
  foreach ($companies as $company) {

    // Add company to list if it is not the default company.
    $company_info = array(
      'name' => $company->name,
      'display_name' => $company->displayName,
      'url' => 'api_company/company/switch/' . $company->name,
    );

    if ($default_company != NULL && $default_company->name == $company->name) {
      $default_company_info = $company_info;
    }
    else {
      $company_list[$company->name] = $company_info;
    }
  }

  // If the default company is deleted while the user is still in that
  // company context, set default company context to null.
  if ($default_company_info == NULL) {
    $default_company = NULL;
    apigee_company_set_current_context_company(NULL);
  }

  // Put default company/user on top of list.
  if ($default_company == NULL) {
    // Put the user on the top of the list.
    array_unshift($company_list, $user_company);
  }
  else {
    // Put the default company on top of list & default user next in list.
    array_unshift($company_list, $default_company_info, $user_company);
  }

  return theme('apigee_company_switcher', array('company_list' => $company_list));
}

/**
 * Callback to switch company context.
 *
 * @param string $company_id
 *   The machine name of the company to which to switch the context to.
 */
function apigee_company_switch($company_id) {
  // Set the company context, or back to user context if NULL.
  if ($company_id != '') {
    $company = entity_load_single('apigee_company', $company_id);
    apigee_company_set_current_context_company($company);
  }
  else {
    apigee_company_set_current_context_company(NULL);
  }
  apigee_company_update_user_roles();
  drupal_goto();
}


/**
 * Returns the list of company for the given user email.
 *
 * If no user email is given then returns the list of companies
 * for the current user.
 * @global type $user
 *
 * @param string $developer_email
 *   The email of the developer.
 *
 * @return array
 *   An array of company objects that the developer belongs to.
 */
function apigee_company_get_company_list($developer_email = NULL) {
  global $user;

  $developer_email = $developer_email ?: $user->mail;
  $developer = entity_load_single('developer', $developer_email);
  if (is_object($developer)) {
    $company_ids = $developer->companies;
  }

  // If the $developer->companies is an empty array then we should return
  // immediately as loading the entities with empty ids array will return
  // all the companies in the org.
  if (empty($company_ids)) {
    return array();
  }

  $company_list = apigee_company_get_company_objects($company_ids);
  return $company_list;
}
