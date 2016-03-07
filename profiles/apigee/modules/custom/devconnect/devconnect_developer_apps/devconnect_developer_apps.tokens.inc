<?php
/**
 * @file
 * Token-related functions for devconnect_developer_apps.
 */

/**
 * Implements hook_tokens().
 */
function devconnect_developer_apps_tokens($type, array $tokens, array $data = array(), array $options = array()) {
  if ($type != 'developer_app' || empty($data['developer_app'])) {
    return array();
  }
  $url_options = array('absolute' => TRUE);
  if (isset($options['language'])) {
    $url_options['language'] = $options['language'];
    $language_code = $options['language']->language;
  }
  else {
    $language_code = NULL;
  }
  $sanitize = !empty($options['sanitize']);
  /** @var Drupal\devconnect_developer_apps\DeveloperAppEntity $app */
  $app = $data['developer_app'];
  $replacements = array();
  foreach ($tokens as $name => $original) {
    switch ($name) {
      case 'appUrl':
        $replacements[$original] = url('user/' . $app->uid . '/apps/' . $app->appId, $url_options);
        break;

      // Timestamps:
      case 'createdAt':
      case 'modifiedAt':
        $replacements[$original] = format_date(floor($app->$name / 1000), 'medium', '', NULL, $language_code);;
        break;

      // Arrays of strings:
      case 'apiProducts':
        $text = implode(', ', $app->$name);
        $replacements[$original] = $sanitize ? check_plain($text) : $text;
        break;

      case 'developerUid':
        $replacements[$original] = $app->uid;
        break;

      case 'displayName':
        $name = (array_key_exists('DisplayName', $app->attributes) ? $app->attributes['DisplayName'] : $app->name);
        $replacements[$original] = $sanitize ? check_plain($name) : $name;
        break;

      default:
        if (property_exists($app, $name)) {
          $replacements[$original] = $sanitize ? check_plain($app->$name) : $app->$name;
        }
        break;
    }
  }

  $attributes_info = variable_get('app_attributes', array());

  if ($attribute_tokens = token_find_with_prefix($tokens, 'attributes')) {
    foreach ($attribute_tokens as $name => $original) {
      $value = (string) $app->getAttribute($name);
      if (isset($attributes_info[$name]['options'])) {
        $_arr = array();
        foreach (explode(',', $value) as $v) {
          $_arr[] = isset($attributes_info[$name]['options'][$v]) ? $attributes_info[$name]['options'][$v] : $v;
        }
        $value = implode(',', $_arr);
      }
      $replacements[$original] = $sanitize ? check_plain($value) : $value;
    }
  }

  if ($attribute_tokens = token_find_with_prefix($tokens, 'credentialAttributes')) {
    foreach ($attribute_tokens as $name => $original) {
      $value = (string) $app->getCredentialAttribute($name);
      $replacements[$original] = $sanitize ? check_plain($value) : $value;
    }
  }

  return $replacements;
}

/**
 * Implements hook_token_info().
 */
function devconnect_developer_apps_token_info() {
  $type = array(
    'name' => t('Developer Apps'),
    'description' => t('Tokens related to developer apps'),
    'needs-data' => 'developer_app',
  );
  $app['apiProducts'] = array(
    'name' => t('API Products'),
    'description' => t('List of all API Products used by the app.'),
    'type' => 'list',
  );
  $app['attributes'] = array(
    'name' => t('Attributes'),
    'description' => t('List of custom attributes for this app. Use [app:attributes:<attribute_name>] to fetch a single named attribute.'),
    'type' => 'list',
  );
  $app['callbackUrl'] = array(
    'name' => t('Callback URL'),
    'description' => t('Callback URL configured for this app.'),
  );
  $app['createdAt'] = array(
    'name' => t('Created At'),
    'description' => t('Timestamp at which the app was created.'),
    'type' => 'date',
  );
  $app['modifiedAt'] = array(
    'name' => t('Modified At'),
    'description' => t('Timestamp at which the app was last modified.'),
    'type' => 'date',
  );
  $app['name'] = array(
    'name' => t('App Name'),
    'description' => t('The machine-readable name of the app.'),
  );
  $app['displayName'] = array(
    'name' => t('Display Name'),
    'description' => t('The human-readable name of the app.'),
  );
  $app['status'] = array(
    'name' => t('App Status'),
    'description' => t('The active status of this app.'),
  );
  $app['description'] = array(
    'name' => t('Description'),
    'description' => t('Description of the app, if present.'),
  );
  $app['consumerKey'] = array(
    'name' => t('Consumer Key'),
    'description' => t('The Consumer Key (public key or API Key) for this app.'),
  );
  $app['consumerSecret'] = array(
    'name' => t('Consumer Secret'),
    'description' => t('The Consumer Secret (private key) for this app.'),
  );
  $app['credentialStatus'] = array(
    'name' => t('Credential Status'),
    'description' => t('Status of the Consumer Key/Secret credential.'),
  );
  $app['credentialAttributes'] = array(
    'name' => t('Credential Attributes'),
    'description' => t('Custom attributes of the Consumer Key/Secret credential. Use [app:credentialAttributes:<attribute_name>] to fetch a single named attribute.'),
    'type' => 'list',
  );
  $app['developer'] = array(
    'name' => t('Developer Email'),
    'description' => t('Email address of the app’s developer.'),
  );

  $app['appUrl'] = array(
    'name' => t('App URL'),
    'description' => t('URL on this site where the app’s details may be viewed.'),
  );
  $app['developerUid'] = array(
    'name' => t('Drupal User ID'),
    'description' => t('The unique numeric uid of the developer who owns the app.'),
  );
  $app['overallStatus'] = array(
    'name' => t('Overall Status'),
    'description' => t('The overall status, determined from the hierarchy of API Product, Key and App statuses.'),
  );
  $app['appId'] = array(
    'name' => t('Application ID'),
    'description' => t('The unique identifier (UUID) of the app.'),
  );
  $app['developerId'] = array(
    'name' => t('Developer ID'),
    'description' => t('The unique identifier of the developer on Edge.'),
  );

  ksort($app);

  return array(
    'types' => array('developer_app' => $type),
    'tokens' => array('developer_app' => $app),
  );
}

/**
 * Implements hook_token_info_alter().
 *
 * Removes all developer_app tokens added by any module other than this one.
 */
function devconnect_developer_apps_token_info_alter(&$data) {
  if (array_key_exists('developer_app', $data['tokens'])) {
    $token_info = devconnect_developer_apps_token_info();
    $valid_keys = array_keys($token_info['tokens']['developer_app']);
    $actual_keys = array_keys($data['tokens']['developer_app']);

    foreach ($actual_keys as $key) {
      if (!in_array($key, $valid_keys)) {
        unset($data['tokens']['developer_app'][$key]);
      }
    }
  }
}
