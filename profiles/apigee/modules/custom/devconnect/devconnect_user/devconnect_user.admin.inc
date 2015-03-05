<?php
/**
 * @file
 * Contains admin functionality for devconnect_user module
 */

/**
 * Page Callback for admin/config/people/devconnect/settings.
 *
 * @param array $form
 * @param array $form_state
 */
function devconnect_user_settings_form($form, &$form_state) {
  $form = array();
  $form['apigee_redirect_paths'] = array(
    '#type' => 'fieldset',
    '#title' => t('Redirect Settings'),
    '#weight' => 0,
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['apigee_redirect_paths']['apigee_enable_redirect_path'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable user login redirect path.'),
    '#default_value' => variable_get('apigee_enable_redirect_path', ''),
  );
  $form['apigee_redirect_paths']['apigee_user_redirect_path'] = array(
    '#type' => 'textfield',
    '#title' => t('Redirect Path'),
    '#default_value' => variable_get('apigee_user_redirect_path', ''),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#weight' => 9999,
    '#value' => t('Save Configuration'),
  );
  return $form;
}

/**
 * Validate handler for devconnect_user_settings_form.
 *
 * @param array $form
 * @param array $form_state
 */
function devconnect_user_settings_form_validate($form, &$form_state) {
  $path = $form_state['values']['apigee_user_redirect_path'];
  if ($form_state['values']['apigee_enable_redirect_path'] && !drupal_valid_path(drupal_get_normal_path($path), TRUE)) {
    form_set_error('apigee_user_redirect_path', t("The path '@link_path' is either invalid or you do not have access to it.", array('@link_path' => $path)));
  }
}

/**
 * Submit hander for devconnect_user_settings_form.
 */
function devconnect_user_settings_form_submit($form, &$form_state) {
  $vars = array(
    'apigee_user_redirect_path',
    'apigee_enable_redirect_path',
  );
  foreach ($vars as $var) {
    variable_set($var, $form_state['values'][$var]);
  }
  drupal_set_message(t('Your changes have been saved.'));
}

/**
 * Form constructor to administer user attributes persisted to Edge.
 *
 * @param array $form
 * @param array $form_state
 *
 * @return array
 */
function devconnect_user_attributes_form($form, &$form_state) {
  $fields = field_info_instances('user', 'user');
  if (isset($fields['field_first_name'])) {
    unset($fields['field_first_name']);
  }
  if (isset($fields['field_last_name'])) {
    unset($fields['field_last_name']);
  }

  if (count($fields) == 0) {
    $form['notice'] = array(
      '#type' => 'markup',
      '#markup' => t('There are no configured user attributes that can be persisted to KMS.'),
      '#prefix' => '<p>',
      '#suffix' => '</p>',
    );
    return $form;
  }

  $default_values = variable_get('devconnect_user_attributes_to_persist_in_kms', array());

  $form['kms_persisted_user_attribute'] = array(
    '#tree' => FALSE,
    '#title' => t('User attributes to be persisted to KMS'),
    '#type' => 'fieldset',
    '#collapsible' => FALSE,
  );

  foreach ($fields as $field_name => $field_data) {
    $field_name = preg_replace('!^field_!', '', $field_name);

    $form['kms_persisted_user_attribute'][$field_name] = array(
      '#tree' => FALSE,
      '#type' => 'fieldset',
      '#title' => $field_data['label'],
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );

    $form['kms_persisted_user_attribute'][$field_name]["kms_internal_attribute_selected_$field_name"] = array(
      '#title' => t('Persist attribute in KMS?'),
      '#type' => 'checkbox',
      '#default_value' => isset($default_values[$field_name]["selected"]) ? $default_values[$field_name]["selected"] : FALSE,
    );

    $form['kms_persisted_user_attribute'][$field_name]["kms_internal_attribute_name_$field_name"] = array(
      '#title' => t('KMS Attribute Name'),
      '#type' => 'textfield',
      '#default_value' => isset($default_values[$field_name]["kms_internal_attribute_name"]) ? $default_values[$field_name]["kms_internal_attribute_name"] : NULL,
    );

    $form['kms_persisted_user_attribute'][$field_name]["kms_attribute_empty_action_$field_name"] = array(
      '#title' => t('If Attribute Is Empty'),
      '#type' => 'radios',
      '#options' => array(
        'default' => t('Send Default Value'),
        'null' => t('Send NULL'),
        'empty' => t('Send Empty String'),
        'avoid' => t('Do Not Send'),
      ),
      '#default_value' => isset($default_values[$field_name]["empty_action"]) ? $default_values[$field_name]["empty_action"] : 'null',
    );

    $form['kms_persisted_user_attribute'][$field_name]["kms_attribute_set_to_string_boolean_$field_name"] = array(
      '#title' => t('Convert to true|false string'),
      '#type' => 'checkbox',
      '#description' => t('Value to be sent to KMS will be set as (field_value ? \'true\' : \'false\')'),
      '#default_value' => isset($default_values[$field_name]["convert_to_boolean_string_value"]) ? $default_values[$field_name]["convert_to_boolean_string_value"] : FALSE,
    );
  }

  $form['devconnect_user_attributes_to_persist_in_kms'] = array(
    '#type' => 'value',
    '#value' => $default_values,
  );

  return system_settings_form($form);
}

/**
 * Validator for devconnect_user_attributes_form.
 *
 * This doesn't actually validate the form, but rather converts the array
 * to a sensible numeric array prior to saving in the database.
 *
 * @param array $form
 * @param array $form_state
 */
function devconnect_user_attributes_form_validate($form, &$form_state) {
  $default_values = array();
  foreach ($form_state['values'] as $input => $value) {
    if (strpos($input, 'kms_internal_attribute_selected_') === 0) {
      $field_name = substr($input, 32);
      $default_values[$field_name] = array(
        'selected' => $form_state['values']["kms_internal_attribute_selected_$field_name"],
        'kms_internal_attribute_name' => $form_state['values']["kms_internal_attribute_name_$field_name"],
        'empty_action' => $form_state['values']["kms_attribute_empty_action_$field_name"],
        'convert_to_boolean_string_value' => $form_state['values']["kms_attribute_set_to_string_boolean_$field_name"],
      );
      unset($form_state['values']["kms_internal_attribute_selected_$field_name"]);
    }
    else {
      if (strpos($input, 'kms_internal_attribute_name_') === 0) {
        $field_name = substr($input, 28);
        unset($form_state['values']["kms_internal_attribute_name_$field_name"]);
      }
      else {
        $field_name = NULL;
      }
    }
    if (isset($field_name)) {
      unset($form_state['values']["kms_attribute_empty_action_$field_name"]);
      unset($form_state['values']["kms_attribute_set_to_string_boolean_$field_name"]);
    }
  }
  $form_state['values']['devconnect_user_attributes_to_persist_in_kms'] = $default_values;
}


/**
 * Implements hook_admin_paths_alter().
 *
 * Treat all user pages as not administrative.
 */
function devconnect_user_admin_paths_alter(&$paths) {
  $paths['user/*'] = FALSE;
}


/**
 * Implements hook_variable_info().
 */
function devconnect_user_variable_info($options = array()) {
  $variables['devconnect_user_tnc_url'] = array(
    'type' => 'string',
    'title' => t('Terms & Conditions URL'),
    'default' => _devconnect_user_get_tnc_url(),
    'description' => t('The URL of the Terms & Conditions page. Omit hostname and leading slash.'),
    'required' => TRUE,
    'group' => 'devconnect',
  );
  return $variables;
}
