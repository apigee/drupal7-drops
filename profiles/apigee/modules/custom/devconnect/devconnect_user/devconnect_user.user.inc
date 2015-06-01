<?php


/**
 * Implements hook_user_presave().
 */
function devconnect_user_user_presave(&$edit, $account, $category) {
  if (isset($edit['already_in_kms']) && $edit['already_in_kms'] === TRUE) {
    unset($edit['already_in_kms']);
    return;
  }
  // Don't try to save a user with no configured email.
  if (empty($account->mail) && empty($edit['mail'])) {
    return;
  }
  // Don't update user 1.
  if (is_object($account) && property_exists($account, 'uid') && $account->uid == 1) {
    return;
  }
  $config =& devconnect_get_org_settings();
  $org = $config['org'];
  if (empty($org) || $org == 'fixme') {
    return;
  }

  $first_name = _devconnect_user_get_field_value('field_first_name', $edit, $account);
  $last_name = _devconnect_user_get_field_value('field_last_name', $edit, $account);
  $mail = _devconnect_user_get_field_value('mail', $edit, $account);
  $name = _devconnect_user_get_field_value('name', $edit, $account);
  $orig_mail = (empty($account->mail) ? $edit['mail'] : $account->mail);

  $entity = entity_get_controller('developer')->loadIfExists($orig_mail);

  // If there is a change in the email we try to update the email first.
  if ($entity && isset($edit['mail']) && $orig_mail != $edit['mail']) {
    $saved = entity_get_controller('developer')->updateEmail($entity, $edit['mail']);
    // If the email was not udpated in EDGE then we dont allow updating email
    // of the user and set it back to the original email.
    if (!$saved) {
      $edit['mail'] = $orig_mail;
      drupal_set_message(t("There was an error updating the email, please try again"));
    }
  }
  if (!$entity) {
    $entity = entity_create('developer', array());
  }
  $entity->email = $mail;
  $entity->userName = $name;
  $entity->firstName = $first_name;
  $entity->lastName = $last_name;

  $fields = variable_get('devconnect_user_attributes_to_persist_in_kms', array());
  foreach ($fields as $field_name => $field_specs) {
    $field_full_name = 'field_' . $field_name;
    if ($field_specs['selected']) {
      $att_name = $field_specs['kms_internal_attribute_name'];

      $field_value = _devconnect_user_get_field_value($field_full_name, $edit, $account, NULL);

      if (strlen($field_value) === 0) {
        $set_value = TRUE;
        $att_value = NULL;
        switch ($field_specs['empty_action']) {
          case 'null':
            break;

          case 'empty':
            $att_value = '';
            break;

          case 'default':
            $field = field_info_instance('user', $field_full_name, 'user');
            $default_value = isset($field['default_value'][0]['value']) ? $field['default_value'][0]['value'] : NULL;
            if ($field_specs['convert_to_boolean_string_value']) {
              $default_value = (bool) $default_value;
            }
            $att_value = _devconnect_user_scrub_attribute($default_value);
            break;

          case 'avoid': //Attribute is not to be sent.
          default:
            $set_value = FALSE;
            break;
        }
        if ($set_value) {
          $entity->attributes[$att_name] = $att_value;
        }
      }
      else {
        $entity->attributes[$att_name] = _devconnect_user_scrub_attribute($field_value);
      }
    }
  }

  // Bring in any attributes that were defined by other hook_user_presave()
  // implementations.
  foreach ($edit as $key => $field_value) {
    if (substr($key, 0, 10) == 'attribute_') {
      $entity->attributes[substr($key, 10)] = _devconnect_user_scrub_attribute($field_value);
    }
  }

  if (array_key_exists('status', $edit)) {
    $status = (bool) $edit['status'];
  }
  elseif (property_exists($account, 'status')) {
    $status = (bool) $account->status;
  }
  else {
    $status = FALSE;
  }
  $entity->status = $status ? 'active' : 'inactive';

  $saved = entity_save('developer', $entity);
  $results = devconnect_get_debug_data();

  if (!$saved) {
    devconnect_default_org_config()->logger->error(print_r($results, TRUE));
  }
  else {
    foreach (module_implements('devconnect_user_save') as $module) {
      $function = $module . '_devconnect_user_save';
      $function($results, $edit, $account);
    }
    devconnect_default_org_config()->logger->debug(print_r($results, TRUE));
  }
}


/**
 * Implements hook_user_delete().
 */
function devconnect_user_user_delete($account) {
  // Don't try to delete users from KMS if org hasn't been configured yet.
  $config =& devconnect_get_org_settings();
  if (empty($config['org']) || $config['org'] == 'fixme') {
    return;
  }
  // Don't update anonymous or user 1.
  if (property_exists($account, 'uid') && $account->uid < 2) {
    return;
  }

  $log_level = 'debug';
  try {
    entity_delete('developer', $account->mail);
  }
  catch (Exception $e) {
    $log_level = 'error';
  }
  $results = devconnect_get_debug_data();
  devconnect_default_org_config()->logger->$log_level(print_r($results, TRUE));
  module_invoke_all('devconnect_user_delete', $results, $account);
}


/**
 * Implements hook_user_login().
 */
function devconnect_user_user_login(&$edit, $account) {
  if (!isset($_POST['form_id']) || $_POST['form_id'] != 'user_pass_reset') {
    if ((bool) variable_get('apigee_enable_redirect_path', 0)) {
      drupal_goto(variable_get('apigee_user_redirect_path', ''));
    }
  }
}

/**
 * Implements hook_redirect_alter
 */
function devconnect_user_redirect_alter(&$redirect) {
  // Only act if user token in redirect path.
  if (!empty($redirect->redirect) && strpos($redirect->redirect, '[user') !== FALSE) {
    if (!user_is_logged_in()) {
      // Bounce via login back to source path.
      $redirect->redirect = 'user/login';
      $redirect->redirect_options['query'] = array('destination' => $redirect->source);
    }
    else {
      // Apply user tokens to redirect path.
      $redirect->redirect = token_replace($redirect->redirect, array('user' => $GLOBALS['user']));
    }
  }
}
