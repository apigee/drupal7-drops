<?php

use Drupal\devconnect\ErrorHandling;

/**
 * Implements hook_rules_event_info().
 *
 *
 */
function devconnect_developer_apps_rules_event_info() {
  $items = array();

  $items['devconnect_developer_app_create'] = array(
    'label' => t('Upon creation of a developer app'),
    'group' => 'Devconnect',
    'variables' => array(
      'app' => array('type' => 'developer_app', 'label' => t('created app'))
    )
  );

  $items['devconnect_developer_app_update'] = array(
    'label' => t('Upon update of a developer app'),
    'group' => 'Devconnect',
    'variables' => array(
      'app' => array('type' => 'developer_app', 'label' => t('updated app'))
    )
  );

  $items['devconnect_developer_app_delete'] = array(
    'label' => t('Upon deletion of a developer app'),
    'group' => 'Devconnect',
    'variables' => array(
      'app' => array('type' => 'developer_app', 'label' => t('deleted app'))
    )
  );

  $statuses = array('revoked', 'pending', 'approved');
  foreach ($statuses as $from) {
    foreach ($statuses as $to) {
      if ($to == $from) {
        continue;
      }
      $items['devconnect_developer_app_status_' . $from . '_' . $to] = array(
        'label' => t("Upon change of a developer app’s overall status from $from to $to"),
        'group' => 'Devconnect',
        'variables' => array(
          'app' => array('type' => 'developer_app', 'label' => t('updated app')),
        )
      );
    }
  }

  return $items;
}

/**
 * Implements hook_rules_event_info_alter().
 *
 * Remove entity-based Developer App events because they don't work.
 * The events declared in devconnect_developer_apps_rules_event_info()
 * should be used instead.
 *
 * @param $events
 */
function devconnect_developer_apps_rules_event_info_alter(&$events) {
  foreach ($events as $key => $event) {
    if (array_key_exists('group', $event) && $event['group'] == 'Developer App') {
      unset($events[$key]);
    }
  }
}

/**
 * Implements hook_rules_action_info().
 */
function devconnect_developer_apps_rules_action_info() {
  $actions = array();
  $parameters = array();
  $parameters['DisplayName_customattr'] = array(
    'type' => 'text',
    'label' => t(_devconnect_developer_apps_get_app_label(FALSE) . ' Display Name'),
    'description' => t('Please provide a human-readable name for the new application.'),
  );

  $parameters['app_name'] = array(
    'type' => 'text',
    'label' => t(_devconnect_developer_apps_get_app_label(FALSE) . ' Internal Name'),
    'description' => t('Please provide a name to create an application with. Valid characters are letters, numbers, underscores, dollars, percent, or hyphens.'),
  );
  $parameters['developer_uid'] = array(
    'type' => 'text',
    'label' => t('Developer UID'),
    'description' => t('Please provide the UID of the developer to create the application for.'),
  );
  $parameters['status'] = array(
    'type' => 'text',
    'label' => t('Status'),
    'description' => t('Please provide the status of the application. Example: pending, approved, etc'),
  );
  if (variable_get('devconnect_callback_handling', 'require') != 'none') {
    $parameters['callback_url'] = array(
      'type' => 'uri',
      'label' => t('Callback URL'),
      'description' => t('Please specify the callback URL that will be used.'),
    );
  }
  $custom_attributes = module_invoke_all('devconnect_app_attributes');
  foreach ($custom_attributes as $machine_name => $attribute) {
    $name = $attribute['name'];
    $type = (array_key_exists('type', $attribute) ? $attribute['type'] : 'textfield');
    $add_to_parameters = TRUE;
    switch ($type) {
      case 'radios':
        $description = t('Example: my_custom_option');
        break;
      case 'textfield':
        $description = t('Please specify text for textfield.');
        break;
      case 'textarea':
        $description = t('Please specify text for textarea.');
        break;
      case 'select':
        $description = t('Example: my_custom_option');
        break;
      case 'checkbox':
        $description = t('Value for checkbox.');
        break;
      case 'checkboxes':
        $description = t('Value for checkboxes.');
        break;
      default:
        $add_to_parameters = FALSE;
        break;
    }
    if ($add_to_parameters) {
      $parameters[$machine_name . '_customattr'] = array(
        'type' => 'text',
        'label' => t($name),
        'description' => t($description),
      );
    }
  }

  $products = devconnect_developer_apps_get_api_products();
  $product_list = implode(', ', array_keys($products));

  $parameters['api_products'] = array(
    'type' => 'text',
    'label' => t('API Products'),
    'description' => t('Please specify the default API Products for the application. Available options are: %o', array(
      '%o' => $product_list
    )),
  );
  $actions['devconnect_developer_apps_create_app'] = array(
    'label' => t('Create a Developer Application'),
    'group' => 'Devconnect',
    'parameter' => $parameters,
    'named parameter' => TRUE,
  );
  return $actions;
}

/**
 * Action function for creating a developer application
 *
 * @param $roles
 * @param $message
 */
function devconnect_developer_apps_create_app($parameters) {

  $account = user_load($parameters['developer_uid']);

  $entity = entity_create('developer_app', array('uid' => $account->uid));
  $entity->name = $parameters['app_name'];
  $entity->developer = $account->mail;
  $entity->overallStatus = $parameters['status'];
  $entity->uid = $account->uid;

  $api_products = array();
  $products = explode('\n\r' , $parameters['api_products']);
  foreach ($products as $product) {
    $api_products[] = $product;
  }
  $entity->apiProducts = $api_products;

  $attributes = array();
  foreach ($parameters as $key => $parameter) {
    if (strpos($key, '_customattr') !== false) {
      $attributes[str_replace('_customattr', '', $key)] = $parameters[$key];
    }
  }
  $entity->attributes = $attributes;

  $saved = entity_save('developer_app', $entity);
  if (!$saved) {
    $e = DeveloperAppController::getLastException();
    $code = $e->getCode();
    if ($code == 409) {
      $summary = t('The App Name “@app_name” is already being used.', array('@app_name' => $account->name));
      $detail = 'Duplicate app name "' . $account->name . '" for user "' . $account->name . '"!'
        . "\n" . $e->getResponse();

      devconnect_default_org_config()->logger->warning($detail);
    } else {
      $response = @json_decode($e->getResponse());
      $uri = $e->getUri();
      if ($response && property_exists($response, 'code') && $response->code == 'keymanagement.service.app_invalid_name') {
        $summary = $response->message;
      }
      else {
        $summary = t('There was an error trying to create the application. Please try again later.');
      }
      $message = 'Saving app @app_name for user @user_name generated @ex with a code of @status when accessing URI @uri. Details as follows: @params';
      $params = $e->getParams();
      if ($summary != $response->message) {
        $params['message'] = $response->message;
      }
      $exception_class = get_class($e);
      $exception_class = 'a' . (preg_match('!^[AEIOUaeiou]!', $exception_class) ? 'n' : '') . ' ' . $exception_class;
      $args = array(
        '@app_name' => $account->name,
        '@user_name' => $account->name,
        '@ex' => $exception_class,
        '@status' => $code,
        '@uri' => $uri,
        '@params' => print_r($params, TRUE)
      );
      $detail = t($message, $args);
    }
    devconnect_notify(ErrorHandling::CODE_APP_CREATED, $summary, $detail, ErrorHandling::SEVERITY_STATUS);
  }
}
