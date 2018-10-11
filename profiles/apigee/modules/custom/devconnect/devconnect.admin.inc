<?php
/**
 * @file
 * Handles the devconnect admin configuration screen.
 */

use Apigee\Exceptions\ResponseException;
use Apigee\Exceptions\SamlResponseException;
use Apigee\Util\OrgConfig;
use Apigee\ManagementAPI\Organization;

define('DEVCONNECT_SHOW_CONFIG_FIELDS', TRUE);

/**
 * Constructor for devconnect admin form.
 *
 * @param array $form
 *   The form being created.
 * @param array $form_state
 *   State of the form being created.
 *
 * @return array
 *   The newly-created form.
 */
function devconnect_admin_form(array $form, array &$form_state) {
  $original_org_settings = devconnect_get_org_settings();
  if (isset($form_state['input']['org_settings'])) {
    $org_settings = $form_state['input']['org_settings'][0];
    if (empty($org_settings['user']) && !empty($original_org_settings['user'])) {
      $org_settings['user'] = $original_org_settings['user'];
    }
    if (empty($org_settings['pass']) && !empty($original_org_settings['pass'])) {
      $org_settings['pass'] = $original_org_settings['pass'];
    }
  }
  else {
    $org_settings = $original_org_settings;
  }
  if (!array_key_exists('saml_enabled', $org_settings)) {
    $org_settings['saml_enabled'] = 0;
  }
  if (!array_key_exists('saml', $org_settings)) {
      $org_settings['saml'] = [];
  }

  $form['#attributes']['autocomplete'] = 'off';

  $form['original_org_settings'] = [
    '#type' => 'value',
    '#value' => $org_settings,
  ];

  $form['connection_config_container'] = [
    '#title' => t('Connection Configuration'),
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#id' => 'edit-connection-config-container',
    'org_settings' => [
      '#tree' => TRUE,
    ],
  ];
  $element =& $form['connection_config_container']['org_settings'][0];
  if (
    DEVCONNECT_SHOW_CONFIG_FIELDS
    || isset($form_state['input']['org_settings'])
    || empty($org_settings['org'])
    || empty($org_settings['endpoint'])
    || empty($org_settings['user'])
    || empty($org_settings['pass'])
  ) {
    $element['org'] = [
      '#type' => 'textfield',
      '#title' => t('Management API organization'),
      '#required' => TRUE,
      '#default_value' => $org_settings['org'],
      '#description' => t('The v4 product organization name. Changing this value could make your site stop working.'),
      '#weight' => -6,
    ];
    $element['endpoint'] = [
      '#type' => 'textfield',
      '#title' => t('Management API endpoint URL'),
      '#required' => TRUE,
      '#default_value' => $org_settings['endpoint'],
      '#description' => t('URL to which to make Edge REST calls.'),
      '#weight' => -5,
    ];

    $element['user'] = [
      '#type' => 'textfield',
      '#title' => t('Endpoint authenticated user'),
      '#required' => FALSE,
      '#default_value' => $org_settings['user'],
      '#description' => t('User name used when authenticating with the endpoint. Generally this takes the form of an email address.  (Only enter it if you want to change the existing user.)'),
      '#weight' => -4,
      '#attributes' => ['autocomplete' => 'off'],
    ];

    $element['pass'] = [
      '#type' => 'textfield',
      '#title' => t('Authenticated user’s password'),
      '#required' => FALSE,
      '#default_value' => '',
      '#description' => t('Password used when authenticating with the endpoint. (Only enter it if you want to change the existing password.)'),
      '#weight' => -3,
      '#post_render' => ['devconnect_password_post_render'],
      '#attributes' => ['autocomplete' => 'off'],
    ];

    $element['saml_enabled'] = [
      '#type' => 'checkbox',
      '#title' => t('This org is SAML-enabled'),
      '#return_value' => 1,
      '#default_value' => $org_settings['saml_enabled'],
      '#id' => 'org-settings-0-saml-enabled',
    ];

    $element['saml'] = [
      '#type' => 'fieldset',
      '#title' => t('SAML authentication settings'),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
      '#id' => 'edit-saml-container',
      '#states' => [
        'invisible' => [
          '#org-settings-0-saml-enabled' => ['checked' => FALSE]
        ],
      ],
      '#tree' => TRUE,
    ];

    $endpoint = $key = $secret = NULL;
    if (!empty($org_settings['saml'])) {
      $endpoint = $org_settings['saml']['endpoint'];
      $key = $org_settings['saml']['key'];
      $secret = $org_settings['saml']['secret'];
    }

    $element['saml']['endpoint'] = [
      '#type' => 'textfield',
      '#title' => t('SAML bearer-token authentication endpoint'),
      '#required' => FALSE,
      '#default_value' => $endpoint,
      '#description' => t('URL to call when fetching a bearer token for SAML-enabled orgs.')
    ];

    $element['saml']['key'] = [
      '#type' => 'textfield',
      '#title' => t('API key'),
      '#required' => FALSE,
      '#default_value' => $key,
      '#description' => t('Key used when fetching a bearer token.'),
    ];

    $element['saml']['secret'] = [
      '#type' => 'textfield',
      '#title' => t('Consumer secret'),
      '#required' => FALSE,
      '#default_value' => $secret,
      '#description' => t('Secret used when fetching a bearer token '),
      '#post_render' => ['devconnect_password_post_render'],
      '#attributes' => ['autocomplete' => 'off'],
    ];
  }
  else {
    $element['org'] = [
      '#type' => 'item',
      '#title' => t('Management API Organization'),
      '#markup' => (empty($org_settings['org']) ? '<span style="color:red">' . t('No value') . '</span>' : $org_settings['org']),
    ];
    $element['endpoint'] = [
      '#type' => 'item',
      '#title' => t('Management API Endpoint URL'),
      '#markup' => (empty($org_settings['endpoint']) ? '<span style="color:red">' . t('No value') . '</span>' : $org_settings['endpoint']),
    ];
    if (empty($org_settings['user'])) {
      $element['user'] = [
        '#type' => 'item',
        '#title' => t('Endpoint Authenticated User'),
        '#markup' => '<span style="color:red">' . t('No value') . '</span>',
      ];
    }
    if (empty($org_settings['pass'])) {
      $element['user'] = [
        '#type' => 'item',
        '#title' => t('Authenticated User’s Password'),
        '#markup' => '<span style="color:red">' . t('No value') . '</span>',
      ];
    }
    $saml_enabled = array_key_exists('saml_enabled', $org_settings) && $org_settings['saml_enabled'];
    $element['saml_enabled'] = [
      '#type' => 'item',
      '#title' => t('Is this org SAML-enabled?'),
      '#markup' => $saml_enabled ? t('Yes') : t('No'),
    ];
    if ($saml_enabled) {
      $element['saml_endpoint'] = [
        '#type' => 'item',
        '#title' => t('SAML bearer-token endpoint'),
        '#markup' => $org_settings['saml']['endpoint'] ?: '<span style="color:red">' . t('No value') . '</span>',
      ];
      if (empty($org_settings['saml']['key']) || empty($org_settings['saml']['secret'])) {
        $element['saml_key_secret'] = [
          '#type' => 'item',
          '#title' => t('SAML bearer-token key/secret pair'),
          '#markup' => '<span style="color:red">' . t('No value') . '</span>',
        ];
      }
    }
  }

  if (array_key_exists('connection_timeout', $org_settings)) {
    $conn_timeout = intval($org_settings['connection_timeout']);
    if ($conn_timeout < 1) {
      $conn_timeout = 8;
    }
  }
  else {
    $conn_timeout = 4;
  }
  if (array_key_exists('request_timeout', $org_settings)) {
    $req_timeout = intval($org_settings['request_timeout']);
    if ($req_timeout < 1) {
      $req_timeout = 8;
    }
  }
  else {
    $req_timeout = 4;
  }

  $form['connection_config_container']['connection_timeout'] = [
    '#type' => 'textfield',
    '#title' => t('Connection Timeout'),
    '#required' => TRUE,
    '#default_value' => $conn_timeout,
    '#description' => t('Number of seconds before an HTTP connection to Edge is assumed to have timed out.'),
    '#weight' => 10,
    '#post_render' => ['devconnect_number_post_render'],
  ];

  $form['connection_config_container']['request_timeout'] = [
    '#type' => 'textfield',
    '#title' => t('Request Timeout'),
    '#required' => TRUE,
    '#default_value' => $req_timeout,
    '#description' => t('Number of seconds before an HTTP response from Edge is assumed to have timed out.'),
    '#weight' => 11,
    '#post_render' => ['devconnect_number_post_render'],
  ];

  $default_log_threshold = (array_key_exists('log_threshold', $org_settings) ? intval($org_settings['log_threshold']) : WATCHDOG_WARNING);

  $form['connection_config_container']['log_threshold'] = [
    '#type' => 'radios',
    '#title' => t('Edge logging threshold'),
    '#description' => t('Controls what baseline level of Edge output should be written to the logs.'),
    '#required' => TRUE,
    '#default_value' => $default_log_threshold,
    '#options' => [
      WATCHDOG_DEBUG => 'Debug',
      WATCHDOG_INFO => 'Info',
      WATCHDOG_NOTICE => 'Notice',
      WATCHDOG_WARNING => 'Warning',
      WATCHDOG_ERROR => 'Error',
      WATCHDOG_CRITICAL => 'Critical',
      WATCHDOG_ALERT => 'Alert',
      WATCHDOG_EMERGENCY => 'Emergency',
    ],
    '#weight' => 12,
  ];

  if (empty($org_settings['org'])) {
    $status = '';
  }
  else {
    $saml_settings = $org_settings['saml'];
    if (!$org_settings['saml_enabled']) {
      $saml_settings = NULL;
    }
    $status = _devconnect_test_kms_connection($org_settings['org'], $org_settings['endpoint'], $org_settings['user'], $org_settings['pass'], $conn_timeout, $saml_settings);
  }

  if (DEVCONNECT_SHOW_CONFIG_FIELDS || empty($org_settings['org']) || isset($form_state['input']['org_settings'])) {
    $form['connection_config_container']['connect_test'] = [
      '#type' => 'fieldset',
      '#title' => t('Test Configuration'),
      '#weight' => 9,
      '#prefix' => '<br><br>',
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
    ];

    $form['connection_config_container']['connect_test']['test_connection'] = [
      '#name' => 'test_connection',
      '#type' => 'button',
      '#limit_validation_errors' => [],
      '#value' => t('Test Connection'),
      '#ajax' => [
        'callback' => '_devconnect_test_management_connection_ajax',
        'wrapper' => 'test-connect-result',
        'method' => 'replace',
        'effect' => 'fade',
      ],
      '#states' => [
        'disabled' => [
          ':input[name="org_settings[0][org]"]' => ['empty' => TRUE],
        ],
      ],
    ];
    $form['connection_config_container']['connect_test']['test_connection_status'] = [
      '#prefix' => '<div id="test-connect-result"><br>',
      '#suffix' => '</div>',
      '#markup' => $status,
    ];
  }
  else {
    $form['connection_config_container']['test_connection_status'] = [
      '#weight' => 9,
      '#type' => 'item',
      '#title' => t('Connection Status'),
      '#markup' => $status,
    ];
  }

  $modules = module_implements('variable_info');
  // Sort modules by name.
  sort($modules);
  $my_variables = [];
  foreach ($modules as $module) {
    if (substr($module, 0, 10) == 'devconnect') {
      $my_variables += call_user_func($module . '_variable_info');
    }
  }

  $weight = 0;

  $form['devconnect_config_container'] = [
    '#title' => t('Dev Portal UI configuration'),
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  ];

  foreach ($my_variables as $name => $info) {
    $default = (isset($info['default'])) ? $info['default'] : '';
    $default_value = variable_get($name, $default);
    $form_element = [];
    if ($info['type'] == 'boolean') {
      $form_element['#type'] = 'checkbox';
      $form_element['#default_value'] = intval($default_value);
      $form_element['#return_value'] = 1;
    }
    elseif ($info['type'] == 'select') {
      $form_element['#default_value'] = $default_value;
      if ((!isset($info['options']) || !is_array($info['options'])) && (isset($info['options callback']) && is_callable($info['options callback']))) {
        $form_element['#options'] = call_user_func($info['options callback']);
      }
      else {
        $form_element['#options'] = $info['options'];
      }
      // If configured default value is not a valid option, but the
      // hook_variable_info default is, use the hook_variable_info value.
      if (!array_key_exists($default_value, $form_element['#options'])) {
        if (array_key_exists($default, $form_element['#options'])) {
          $form_element['#default_value'] = $default;
        }
        else {
          // Choose the first option as a default.
          $option_values = array_keys($form_element['#options']);
          if (count($option_values) > 0) {
            $form_element['#default_value'] = $option_values[0];
          }
        }
      }
      if (count($form_element['#options']) < 7) {
        $form_element['#type'] = 'radios';
      }
      else {
        $form_element['#type'] = 'select';
      }
    }
    else {
      $form_element['#type'] = 'textfield';
      $form_element['#default_value'] = $default_value;
    }
    $form_element['#title'] = $info['title'];
    if (isset($info['description'])) {
      $form_element['#description'] = $info['description'];
    }
    if (isset($info['required'])) {
      $form_element['#required'] = $info['required'];
    }
    $form_element['#weight'] = $weight++;
    $form_element['#suffix'] = '<br/>';

    if (strpos($name, 'monetization') !== FALSE) {
      $set = 'monetization_config';
    }
    else {
      $set = 'devconnect_config';
    }
    $form[$set . '_container'][$name] = $form_element;
  }

  $form['experimental'] = [
    '#title' => t('Experimental features'),
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#after_build' => ['_devconnect_hide_if_empty'],
  ];
  $form['experimental']['description'] = [
    '#type' => 'markup',
    '#markup' => '<svg xmlns="http://www.w3.org/2000/svg" version="1.1" width="24" height="32" style="float:left;margin-right:1em">
  <g transform="translate(0,31) scale(0.0375,-0.0375)">
   <path style="stroke:none; fill:#ff4300" d="M347 439l180 -366c16 -32 -1 -51 -29 -51 h-358c-28 0 -45 19 -29 51l180 366h56z"/>
   <path style="stroke:#999; stroke-width: 28px; fill:none" d="M260 744c0 18 118 18 118 0c0 -38 -19 -22 -19 -56v-220l200 -394c23 -45 -2 -74 -42 -74h-396c-40 0 -65 29 -42 74l200 394v220c0 34 -19 18 -19 56z" />
  </g>
  <circle cx="11" cy="22" r="1.5" style="stroke:none;fill:white"/>
  <circle cx="14" cy="25.5" r="2" style="stroke:none;fill:white"/>
</svg>'
      . '<p>' . t('This is a testing ground for experimental features that aren’t quite ready for prime time.<br>They may <strong>change</strong>, <strong>break</strong> or <strong>disappear</strong> at any time.') . '</p>',
 ];

  $form = system_settings_form($form);
  $form['#submit'] = ['devconnect_admin_form_submit'];
  return $form;
}

/**
 * After-build callback for 'experimental' fieldset element.
 *
 * If no other modules have added anything to this fieldset, hide the whole
 * thing.
 *
 * @param array $element
 *
 * @return array
 */
function _devconnect_hide_if_empty(array $element) {
  $has_content = FALSE;
  foreach (array_keys($element) as $key) {
    if (substr($key, 0, 1) != '#' && $key != 'description') {
      $has_content = TRUE;
      break;
    }
  }
  if (!$has_content) {
    $element['#access'] = FALSE;
  }
  return $element;
}

/**
 * Submit handler for the devconnect admin form.
 *
 * @param array $form
 *   Form being submitted.
 * @param array $form_state
 *   State of the form being submitted.
 */
function devconnect_admin_form_submit(array $form, array &$form_state) {

  $changes = [];
  // Read existing settings. These are our comparison baseline.
  $existing_config = devconnect_get_org_settings();
  // Prepare for new configuration.
  // Make sure we have some default values present.
  $form_state['values'] += [
    'connection_timeout' => 8,
    'request_timeout' => 8,
    'log_threshold' => 3,
    'saml_enabled' => 0,
  ];
  $org_config_new = [
    'connection_timeout' => intval($form_state['values']['connection_timeout']),
    'request_timeout' => intval($form_state['values']['request_timeout']),
    'log_threshold' => intval($form_state['values']['log_threshold']),
    'saml_enabled' => intval($form_state['values']['saml_enabled']),
    'saml' => [],
  ];

  // Set default org settings.
  if (isset($form_state['values']['org_settings'][0])) {
    $org_config_submitted = $form_state['values']['org_settings'][0];
  }
  else {
    // This should never happen!
    $org_config_submitted = [];
  }
  $updateable_form = ($form['connection_config_container']['org_settings'][0]['org']['#type'] == 'textfield');
  foreach (['org', 'endpoint', 'user', 'pass', 'saml_enabled'] as $field) {
    $hide_field_in_log = FALSE;
    if ($updateable_form) {
      // Pull user field values from existing config if fields are empty.
      if ($field == 'pass' && empty($org_config_submitted[$field])) {
        $org_config_new[$field] = $existing_config[$field];
      }
      elseif ($field == 'saml_enabled') {
        // This is a 0/1 boolean field, we need a special case since the logic
        // in  lower elseif statement will always consider this field when set
        // to 0 to be empty(), and will always take existing_config value.  This
        // makes it so you can never turn off oAuth (is saml enambled field)
        $org_config_new[$field] = $org_config_submitted[$field];
      }
      // Use value from $form_state only if non-blank.
      elseif (array_key_exists($field, $org_config_submitted) && !empty($org_config_submitted[$field])) {
        // Use trim to remove any whitespace around value
        $org_config_new[$field] = trim($org_config_submitted[$field]);
      }
      // Otherwise, pull from existing config.
      else {
        $org_config_new[$field] = $existing_config[$field];
      }
      if ($field == 'pass') {
        $hide_field_in_log = TRUE;
      }
    }
    else {
      $org_config_new[$field] = $existing_config[$field];
      $hide_field_in_log = TRUE;
    }

    if ($org_config_new[$field] != $existing_config[$field]) {
      $old_value = (empty($existing_config[$field]) ? '*empty*' : ($hide_field_in_log ? '*hidden*' : $existing_config[$field]));
      $new_value = (empty($org_config_new[$field]) ? '*empty*' : ($hide_field_in_log ? '*hidden*' : $org_config_new[$field]));
      $changes["Default Connection: $field"] = [$old_value, $new_value];
    }
  }
  if (!array_key_exists('saml', $org_config_submitted)) {
    $org_config_submitted['saml'] = [];
  }
  foreach (['endpoint', 'key', 'secret'] as $field) {
    $hide_field_in_log = FALSE;
    $found_key = FALSE;
    if ($updateable_form) {
      if (array_key_exists($field, $org_config_submitted['saml'])) {
        $org_config_new['saml'][$field] = trim($org_config_submitted['saml'][$field]);
        $found_key = TRUE;
      }
      if ($field == 'key' || $field == 'secret') {
        $hide_field_in_log = TRUE;
      }
    }
    elseif (array_key_exists($field, $existing_config['saml'])) {
      $org_config_new['saml'][$field] = $existing_config['saml'][$field];
      $found_key = TRUE;
    }
    if ($found_key && $org_config_new['saml'][$field] != $existing_config[$field]) {
      $old_value = (empty($existing_config['saml'][$field]) ? '*empty*' : ($hide_field_in_log ? '*hidden*' : $existing_config['saml'][$field]));
      $new_value = (empty($org_config_new['saml'][$field]) ? '*empty*' : ($hide_field_in_log ? '*hidden*' : $org_config_new['saml'][$field]));
      $changes["Default Connection: saml $field"] = [$old_value, $new_value];
    }
  }

  $default_private_dir = variable_get('file_private_path', 'sites/default/files/private');
  $private_dir = variable_get('apigee_credential_dir', $default_private_dir);

  if (substr($private_dir, 0, 1) == '/') {
    $config_file = $private_dir . '/.apigee';
  }
  else {
    $config_file = DRUPAL_ROOT . '/' . $private_dir . '/.apigee';
  }

  $settings_success = file_put_contents($config_file, Drupal\devconnect\Crypto::encrypt(serialize($org_config_new)));
  @chmod($config_file, 0666);

  if (isset($form_state['values']['devconnect_user_tnc_url'])) {
    $old_url = _devconnect_user_get_tnc_url();
    $new_url = $form_state['values']['devconnect_user_tnc_url'];
    if ($new_url != $old_url) {
      $changes['Terms & Conditions URL'] = [$old_url, $new_url];
      variable_set('devconnect_user_tnc_url', $new_url);
      // Refresh the footer block, since it contains a link to this URI.
      cache_clear_all('devconnect_default_structure:footer', 'cache_block', TRUE);
    }
  }
  if (!$settings_success) {
    drupal_set_message(t('An error occurred, and your connection settings were not saved.'), 'error');
    watchdog('devconnect', 'Failed to write connection settings to file :file',[':file' => $config_file], WATCHDOG_ERROR);
  }

  if (!empty($changes)) {
    $change_items = [];
    foreach ($changes as $key => $values) {
      $change_items[] = "$key was changed from “" . $values[0] . '” to “' . $values[1] . '”';
    }
    $message = 'The following authentication configuration items were changed: ' . implode(',', $change_items);
    watchdog('devconnect', $message, [], WATCHDOG_INFO);

    try {
      $org = new Organization(devconnect_default_org_config());
      $org->load($org_config_new['org']);
      $is_paging_enabled = ($org->getProperty('features.isCpsEnabled') === 'true');
      variable_set('devconnect_paging_enabled', $is_paging_enabled);
    }
    catch (Exception $e) {
    }
  }

  drupal_set_message(t('Your changes have been saved.'), 'status');
}

/**
 * Postrender callback for password form elements.
 *
 * @param string $content
 *   Incoming rendered HTML of the element.
 * @param array $element
 *   FAPI representation of the element.
 *
 * @return string
 *   Rendered HTML for the password form element.
 */
function devconnect_password_post_render($content, array $element) {
  return str_replace('type="text"', 'type="password"', $content);
}

/**
 * Postrender callback for number form elements.
 *
 * @param string $content
 *   Incoming rendered HTML of the element.
 * @param array $element
 *   FAPI representation of the element.
 *
 * @return string
 *   Rendered HTML for the number form element.
 */
function devconnect_number_post_render($content, array $element) {
  return str_replace('type="text"', 'type="number" style="width:3em !important"', $content);
}


/**
 * Ajax Callback for testing the Edge connection.
 *
 * @param array $form
 *   The form whose state is being changed.
 * @param array $form_state
 *   State of the form being changed.
 *
 * @return array
 *   Connection status element of the form.
 */
function _devconnect_test_management_connection_ajax(array $form, array &$form_state) {
  return $form['connection_config_container']['connect_test']['test_connection_status'];
}

/**
 * Test connection with the Management server.
 *
 * @param string $org
 *   Organization that user has supplied.
 * @param string $endpoint
 *   Endpoint that user has supplied.
 * @param string $user
 *   Username that the user has supplied.
 * @param string $pass
 *   Password (unencrypted) that the user has supplied.
 * @param int $timeout
 *   Number of seconds until connection timeout, between 1 and 8.
 * @param array $saml_options
 *   SAML authentication options, if any. Allowed keys include endpoint, key,
 *   and secret.
 *
 * @return string
 *   Success or failure message.
 */
function _devconnect_test_kms_connection($org, $endpoint, $user, $pass, $timeout = 4, array $saml_options = NULL) {
  // Trim whitespace from fields
  $org = trim($org);
  $endpoint = trim($endpoint);
  $user = trim($user);
  $pass = trim($pass);

  // Trim whitespace from oAuth fields
  if (is_array($saml_options)) {
    foreach($saml_options as $saml_setting_key => $saml_setting_value) {
      $saml_options[$saml_setting_key] = trim($saml_setting_value);
    }
  }

  if (!valid_url($endpoint)) {
    return '<span style="color:red">' . t('Invalid endpoint') . '</span>';
  }
  // Make sure timeout does not exceed 8 seconds, or failure ensues.
  $timeout = intval(min($timeout, 8));
  // Zero timeout is infinite; big problem.
  if ($timeout < 1) {
    $timeout = 4;
  }
  $use_saml = is_array($saml_options)
    && array_key_exists('endpoint', $saml_options)
    && array_key_exists('key', $saml_options)
    && array_key_exists('secret', $saml_options);

  if ($use_saml && !valid_url($saml_options['endpoint'])) {
    return '<span style="color:red">' . t('Invalid SAML endpoint') . '</span>';
  }

  $settings = devconnect_get_org_settings();
  drupal_alter('devconnect_org_settings', $settings, $org);

  // Merge in any HTTP options that may have been provided by implementors of
  // hook_devconnect_org_settings_alter().
  $http_options = array_key_exists('http_options', $settings) ? $settings['http_options'] : [];
  $http_options['connect_timeout'] = $timeout;
  $http_options['follow_location'] = FALSE;
  $curl_options = array_key_exists('curl_options', $settings) ? $settings['curl_options'] : [];

  $user_agent = 'DevConnect' . ($use_saml ? ' (SAML)' : '');
  drupal_alter('devconnect_user_agent', $user_agent);

  module_load_include('php', 'devconnect', 'lib/EncryptedCredentialStorage');
  $options = [
    'http_options' => $http_options,
    'curl_options' => $curl_options,
    'user_agent' => $user_agent,
    'referer' => url($_GET['q'], ['absolute' => TRUE]),
    'credential_storage' => new Drupal\devconnect\EncryptedCredentialStorage(),
  ];
  if ($use_saml) {
    $options['saml'] = $saml_options;
  }

  try {
    $config = new OrgConfig($org, $endpoint, $user, $pass, $options);
    $org = new Organization($config);
    $org->load();
    return '<span style="color:green">' . t('Connection Successful') . '</span>';
  }
  catch (SamlResponseException $e) {
    watchdog('devconnect', $e->__toString(), [], WATCHDOG_WARNING);
    $e_code = $e->getCode();
    if ($e_code == 401) {
      $message = t('SAML Endpoint responded with 401 Unauthorized. Check the user, password, SAML key and secret.');
    }
    elseif ($e_code == 0) {
      $message = t('SAML authentication returned an invalid response.');
    }
    else {
      $response_text = $e->getResponse();
      if (!empty($response_text)) {
        $message = t('SAML authentication unsuccessful. HTTP code: %error_code, server response: %server_response',
          [
            '%error_code' => $e_code,
            '%server_response' => $response_text,
          ]
        );
      }
      else {
        $message = t('SAML authentication unsuccessful. HTTP code: %error_code', ['%error_code' => $e_code]);
      }
    }
    return '<span style="color:red">' . $message . '</span>';
  }
  catch (ResponseException $e) {
    $e_code = $e->getCode();
    // Give a nicer message if the user/pass is incorrect.
    if ($e_code == 401) {
      $message = t('Edge Endpoint responded with 401 Unauthorized. Check the username and password.');
      return '<span style="color:red">' . $message . '</span>';
    }

    // If the user put in a non HTTPS endpoint, give them a hint that they may
    // want to use HTTPS instead.
    if (substr($endpoint, 0, 5) != 'https') {
      $message_try_ssl = t('Edge Endpoint may be SSL enabled. Try using <strong><em>@endpoint</em></strong> instead.',
        ['@endpoint' => str_replace('http', 'https', $endpoint)]
      );
    }
    else {
      $message_try_ssl = '';
    }

    // See if we get a response from server to get a better idea what happened.
    $response_text = $e->getResponse();
    if (!empty($response_text)) {
      $message = t('Edge connection unsuccessful. HTTP code: %error_code, server response: %server_response',
        [
          '%error_code' => $e_code,
          '%server_response' => $response_text,
        ]
      );
      return '<span style="color:red">' . $message . '</span> ' . $message_try_ssl;
    }

    // Create as verbose an error message as we can.
    switch ($e_code) {
      case CURLE_COULDNT_RESOLVE_PROXY:
        $message = t('Edge connection unsuccessful; couldn’t resolve proxy.');
        break;

      case CURLE_COULDNT_RESOLVE_HOST:
        $message = t('Edge connection unsuccessful; couldn’t resolve host.');
        break;

      case CURLE_COULDNT_CONNECT:
        $message = t('Edge connection unsuccessful; failed to establish a connection to the host or proxy.');
        break;

      case CURLE_OPERATION_TIMEOUTED:
        $message = t('Edge connection unsuccessful; HTTP timeout.');
        break;

      case CURLE_SSL_CONNECT_ERROR:
        $message = t('Edge connection unsuccessful; a problem occurred somewhere in the SSL/TLS handshake.');
        break;

      case CURLE_TOO_MANY_REDIRECTS:
        $message = t('Edge connection unsuccessful; too many redirects.');
        break;

      case CURLE_SSL_CACERT:
        $message = t('Edge connection unsuccessful; the peer certificate cannot be authenticated with known CA certificates. This may occur if the system date is significantly skewed.');
        break;

      default:
        if ($e_code < 100) {
          $message = t('Edge connection failed with a CURL error code of %e. <a href="@url" target="_blank">Read more about CURL error codes.</a>', [
            '%e' => $e_code,
            '@url' => 'https://curl.haxx.se/libcurl/c/libcurl-errors.html',
          ]);
        }
        else {
          $message = t('Edge connection unsuccessful. HTTP code: %e', ['%e' => $e_code]);
        }
        break;
    }
    return '<span style="color:red">' . $message . '</span> ' . $message_try_ssl;
  }
}

