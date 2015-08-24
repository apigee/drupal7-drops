<?php
/**
 * @file
 * Contains admin functionality for devconnect_user module
 */

/**
 * Page Callback for admin/config/people/devconnect/settings.
 *
 * @param array $form
 *   The form being created.
 * @param array $form_state
 *   State of the form being created.
 *
 * @return array
 *   The fully-created form.
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
 *   The form being validated.
 * @param array $form_state
 *   State of the form being validated.
 */
function devconnect_user_settings_form_validate($form, &$form_state) {
  $path = $form_state['values']['apigee_user_redirect_path'];
  if ($form_state['values']['apigee_enable_redirect_path'] && !drupal_valid_path(drupal_get_normal_path($path), TRUE)) {
    form_set_error('apigee_user_redirect_path', t("The path '@link_path' is either invalid or you do not have access to it.", array('@link_path' => $path)));
  }
}

/**
 * Submit handler for devconnect_user_settings_form.
 *
 * @param array $form
 *   The form being submitted.
 * @param array $form_state
 *   The state of the form being submitted.
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
 *   The form being created.
 * @param array $form_state
 *   State of the form being created.
 *
 * @return array
 *   The fully-created form.
 */
function devconnect_user_attributes_form($form, &$form_state) {
  $fields = field_info_instances('user', 'user');
  if (isset($fields['field_first_name'])) {
    unset($fields['field_first_name']);
  }
  if (isset($fields['field_last_name'])) {
    unset($fields['field_last_name']);
  }

  $form['explanation'] = array(
    '#type' => 'markup',
    '#markup' => t('This form allows custom fields attached to Drupal users to be saved to the Edge Management server as custom developer attributes.'),
    '#prefix' => '<p>',
    '#suffix' => '</p>',
  );

  if (count($fields) == 0) {
    $form['notice'] = array(
      '#type' => 'markup',
      '#markup' => t('There are no configured user attributes that can be persisted to the Edge Management server. To create custom attributes, you must first <a href="@url">add custom fields to the Drupal user entity</a>. The First Name and Last Name fields are already primary properties of Edge’s developer object and are therefore not persisted as custom attributes.', array('@url' => url('admin/config/people/accounts/fields'))),
      '#prefix' => '<p>',
      '#suffix' => '</p>',
    );
    return $form;
  }

  $default_values = variable_get('devconnect_user_attributes_to_persist_in_kms', array());

  foreach ($fields as $field_name => $field_data) {
    $field_name = preg_replace('!^field_!', '', $field_name);

    $form[$field_name] = array(
      '#tree' => FALSE,
      '#type' => 'fieldset',
      '#title' => t('Field: :label', array(':label' => $field_data['label'])),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
    );

    $form[$field_name]["kms_internal_attribute_selected_$field_name"] = array(
      '#title' => t('Persist this field as an attribute in Edge'),
      '#type' => 'checkbox',
      '#default_value' => isset($default_values[$field_name]['selected']) ? $default_values[$field_name]['selected'] : FALSE,
    );

    $form[$field_name]['container'] = array(
      '#type' => 'container',
      '#states' => array(
        'visible' => array(
          ':input[name="kms_internal_attribute_selected_' . $field_name . '"]' => array('checked' => TRUE),
        ),
      ),
    );

    $form[$field_name]['container']["kms_internal_attribute_name_$field_name"] = array(
      '#title' => t('Edge attribute name'),
      '#type' => 'textfield',
      '#description' => t('Name of the custom attribute as it will appear in Edge.'),
      '#default_value' => isset($default_values[$field_name]['kms_internal_attribute_name']) ? $default_values[$field_name]['kms_internal_attribute_name'] : $field_name,
      '#states' => array(
        'required' => array(
          ':input[name="kms_internal_attribute_selected_' . $field_name . '"]' => array('checked' => TRUE),
        ),
      ),
    );

    $form[$field_name]['container']["kms_attribute_empty_action_$field_name"] = array(
      '#title' => t('If attribute is empty'),
      '#type' => 'radios',
      '#options' => array(
        'default' => t('Send default value'),
        'null' => t('Send NULL'),
        'empty' => t('Send empty string'),
        'avoid' => t('Do not send'),
      ),
      '#default_value' => isset($default_values[$field_name]['empty_action']) ? $default_values[$field_name]['empty_action'] : 'null',
    );

    $form[$field_name]['container']["kms_attribute_set_to_string_boolean_$field_name"] = array(
      '#title' => t('Convert value to true|false string'),
      '#type' => 'checkbox',
      '#description' => t('If checked, the value to be sent to Edge will be cast as a boolean and then converted to a ‘true’ or ‘false’ string value.'),
      '#default_value' => isset($default_values[$field_name]['convert_to_boolean_string_value']) ? $default_values[$field_name]['convert_to_boolean_string_value'] : FALSE,
    );
  }

  $form['devconnect_user_attributes_to_persist_in_kms'] = array(
    '#type' => 'value',
    '#value' => $default_values,
  );

  $form = system_settings_form($form);
  // Push our specialized handler to the front of the submit queue.
  array_unshift($form['#submit'], 'devconnect_user_attributes_form_submit');

  return $form;
}

/**
 * Submit handler for devconnect_user_attributes_form.
 *
 * This converts the array to a sensible numeric array prior to saving in the
 * database.
 *
 * @param array $form
 *   The form being submitted.
 * @param array $form_state
 *   State of the form being submitted.
 */
function devconnect_user_attributes_form_submit($form, &$form_state) {
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
