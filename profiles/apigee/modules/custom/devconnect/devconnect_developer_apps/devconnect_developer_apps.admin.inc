<?php

function devconnect_developer_apps_config($form, &$form_state) {

  $form['apiproduct'] = array(
    '#type' => 'fieldset',
    '#title' => t('API Product settings'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE
  );

  $handling = _devconnect_developer_apps_get_default_value('devconnect_api_product_handling', 'multiple_required', $form_state);
  $form['apiproduct']['devconnect_api_product_handling'] = array(
    '#type' => 'radios',
    '#title' => t('API Product handling'),
    '#default_value' => $handling,
    '#options' => array(
      'no_api_product' => t('Do not associate apps with any API Product.'),
      'default_api_product' => t('Associate all apps with one or more Default API Products (configured below).'),
      'single_unrequired' => t('Allow selection of a single API Product, but do not require it.'),
      'single_required' => t('Require selection of a single API Product.'),
      'multiple_unrequired' => t('Allow selection of multiple API Products, but do not require any.'),
      'multiple_required' => t('Allow selection of multiple API Products, and require at least one.')
    ),
    '#required' => TRUE,
    '#ajax' => array(
      'callback' => '_devconnect_developer_apps_render_apiproducts',
      'wrapper' => 'devconnect-default-api-product-wrapper',
      'method' => 'replace',
      'effect' => 'fade',
    )
  );

  $form['apiproduct']['devconnect_developer_apps_apiproduct_widget'] = array(
    '#type' => 'radios',
    '#title' => t('API Product widget'),
    '#description' => t('The form element type to use when selecting an API Product on the app create/edit page.'),
    '#default_value' => _devconnect_developer_apps_get_default_value('devconnect_developer_apps_apiproduct_widget', 'select', $form_state),
    '#options' => array(
      'select' => t('Dropdown select box'),
      'checkboxes' => t('Checkboxes/radio buttons')
    ),
    '#required' => TRUE
  );

  $required = (strpos($handling, 'default') === 0);
  $product_required = ($required || strpos($handling, 'required') !== FALSE);
  // Show check boxes if default_api_product option is chosen
  if ($handling == 'default_api_product') {
    $multiple = TRUE;
  }
  else {
    $multiple = (strpos($handling, 'multiple') !== FALSE);
  }

  $api_products = devconnect_developer_apps_get_api_products();
  $options = array();
  foreach ($api_products as $product) {
    $options[$product->name] = $product->displayName;
  }
  $default_api_product = _devconnect_developer_apps_get_default_value('devconnect_default_api_product', NULL, $form_state);
  if (is_array($default_api_product) && !empty($default_api_product)) {
    $keys = array_keys($default_api_product);
    if (!is_numeric($keys[0])) {
      $dap = array();
      foreach ($keys as $name) {
        if ($default_api_product[$name]) {
          $dap[] = $name;
        }
      }
      $default_api_product = $dap;
    }
  }
  if ($multiple) {
    if (empty($default_api_product)) {
      $default_api_product = array();
    }
    elseif (!is_array($default_api_product)) {
      $default_api_product = array($default_api_product);
    }
  }
  else {
    if (empty($default_api_product)) {
      $default_api_product = NULL;
    }
    elseif (is_array($default_api_product)) {
      $default_api_product = reset($default_api_product);
    }
  }

  asort($options);
  if (!$multiple && !$required) {
    $options = array('' => '- None -') + $options;
    if ($default_api_product === NULL) {
      $default_api_product = '';
    }
  }

  $form['apiproduct']['devconnect_default_api_product'] = array(
    '#prefix' => '<div id="devconnect-default-api-product-wrapper">',
    '#suffix' => '</div>',
    '#type' => ($multiple ? 'checkboxes' : 'radios'),
    '#title' => t('Default API Product'),
    '#options' => $options,
    '#default_value' => $default_api_product,
  );

  $form['cache'] = array(
    '#type' => 'fieldset',
    '#title' => t('Cache settings'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE
  );
  $form['cache']['devconnect_api_product_cache_enable'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable the API Product cache. <em>Unchecking this box will degrade performance. Do not disable the cache on a production instance.</em>'),
    '#default_value' => variable_get('devconnect_api_product_cache_enable', 1),
    '#return_value' => 1,
  );
  $form['cache']['devconnect_developer_apps_cache_cron'] = array(
    '#type' => 'checkbox',
    '#title' => t('Rebuild API Product and Developer App caches every time cron runs.'),
    '#default_value' => variable_get('devconnect_developer_apps_cache_cron', 1),
    '#return_value' => 1,
  );

  $form['ui_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Application UI Settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE
  );

  $form['ui_settings']['myapis'] = array(
    '#type' => 'radios',
    '#title' => t('Text to use in the UI to refer to a Developer App'),
    '#default_value' => variable_get('myapis', 0),
    '#options' => array(
      0 => 'App',
      1 => 'API',
      2 => 'Key'
    )
  );

  $form['callback'] = array(
    '#type' => 'fieldset',
    '#title' => t('Callback URL settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE
  );

  $callback_external = 'Callback URLs should be required for all production-grade apps using a ' .
    '3-legged authentication scheme (see http://tetraph.com/covert_redirect/oauth2_openid_covert_redirect.html)';

  $form['callback']['devconnect_callback_handling'] = array(
    '#type' => 'radios',
    '#title' => t('Callback URL handling'),
    '#description' => t($callback_external),
    '#options' => array(
      'require' => t('Callback URL is required for all developer apps. %o', array('%o' => '(best practice)')),
      'optional' => t('Callback URL is optional for all developer apps.'),
      'none' => t('Callback URL is neither required nor displayed.')
    ),
    '#default_value' => _devconnect_developer_apps_get_default_value('devconnect_callback_handling', 'none', $form_state),
    '#required' => TRUE
  );

  $form['callback']['devconnect_validate_callback_url'] = array(
    '#type' => 'textfield',
    '#title' => t('Regex to Validate App Callback URL'),
    '#default_value' => _devconnect_developer_apps_get_default_value('devconnect_validate_callback_url', '', $form_state),
    '#description' => t('When not empty, Callback URL is validated against this Regex.'),
  );

  $form['callback']['devconnect_tooltip_callback_url'] = array(
    '#type' => 'textfield',
    '#title' => t('Callback URL Field Tooltip'),
    '#default_value' => _devconnect_developer_apps_get_default_value('devconnect_tooltip_callback_url', '', $form_state),
    '#description' => t('If Callback URL Regex is not empty, this should contain a human-readable high-level description of the valid pattern, to be shown in the browser when the user enters an invalid pattern.'),
  );

  $form['analytics'] = array(
    '#type' => 'fieldset',
    '#title' => t('App Performance settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE
  );

  $form['credential_lifetime'] = array(
    '#type' => 'fieldset',
    '#title' => t('Credential Lifetime'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['credential_lifetime']['devconnect_developer_apps_key_expiry'] = array(
    '#type' => 'textfield',
    '#title' => t('Default API key lifetime in days'),
    '#description' => t('When an app is newly-created, this is the default number of days until its API Key expires. A value of -1 indicates no set expiry.'),
    '#default_value' => variable_get('devconnect_developer_apps_key_expiry', -1),
    '#post_render' => array('_devconnect_developer_apps_number_postrender')
  );

  $form['analytics']['devconnect_show_analytics'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show Developer App analytics tab'),
    '#return_value' => '1',
    '#default_value' => _devconnect_developer_apps_get_default_value('devconnect_show_analytics', 1, $form_state)
  );

  // Analytics environments
  $options = $environments = array();
  try {
    $dca = new Apigee\ManagementAPI\DeveloperAppAnalytics(devconnect_default_org_config());
    $environments = $dca->getAllEnvironments();
    foreach ($environments as $environment) {
      switch ($environment) {
        case 'prod':
          $env_name = 'Production';
          break;
        case 'test':
          $env_name = 'Test';
          break;
        case 'dev':
          $env_name = 'Development';
          break;
        default:
          $env_name = $environment;
      }
      $options[$environment] = t($env_name);
    }
  } catch (Apigee\Exceptions\ResponseException $e) {
    // Ignore this exception. It is probably the result of a misconfigured
    // endpoint.
    $options = array('prod' => 'Production');
  }
  if (empty($options) || empty($environments) || array_key_exists('prod', $options)) {
    $default_env = 'prod';
  }
  else {
    $default_env = $environments[0];
  }
  $form['analytics']['devconnect_analytics_environment'] = array(
    '#type' => 'radios',
    '#title' => t('Environment to query for analytics data'),
    '#default_value' => variable_get('devconnect_analytics_environment', $default_env),
    '#options' => $options
  );

  $form['actions'] = array(
    '#type' => 'actions',
    'submit' => array(
      '#type' => 'submit',
      '#value' => t('Save Changes')
    )
  );

  return $form;
}

function _devconnect_developer_apps_render_apiproducts($form, &$form_state) {
  return $form['apiproduct']['devconnect_default_api_product'];
}

function _devconnect_developer_apps_number_postrender($content, $element) {
  return str_replace('type="text"', 'type="number" style="width:7em !important" min="-1"', $content);
};

function devconnect_developer_apps_config_submit($form, &$form_state) {
  foreach ($form_state['values'] as $key => $value) {
    if (substr($key, 0, 10) != 'devconnect') {
      continue;
    }
    if ($key == 'devconnect_default_api_product' && is_array($value)) {
      $defaults = array();
      foreach ($value as $n => $v) {
        if ($v) {
          $defaults[] = $v;
        }
      }
      $value = $defaults;
    }
    variable_set($key, $value);
  }
  variable_set('myapis', $form_state['values']['myapis']);
  $myapis = _devconnect_developer_apps_get_app_label(TRUE);

  db_update('menu_links')
    ->fields(array('link_title' => 'My ' . $myapis))
    ->condition('menu_name', 'main-menu')
    ->condition('link_path', 'user/me/apps')
    ->execute();
  cache_clear_all(NULL, 'cache_menu');
  drupal_set_message(t('The configuration options have been saved.'));
}
