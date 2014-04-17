<?php

/**
 * @file
 * Code required only when fetching information about available updates.
 */


function devportal_updates_manager_get_status($refresh_cache = FALSE) {
  static $status;
  if (!isset($status) || $refresh_cache) {
    if (!$refresh_cache && ($cache = cache_get('devportal_update_status')) && !empty($cache->data)) {
      $status = unserialize($cache->data);
    }
    else {
      $status = devportal_updates_manager_check_update_server();
      cache_set('devportal_update_status', serialize($status), 'cache');
    }
  }
  return $status;
}

/**
 * Check if there is a new Dev Portal update on server.
 */
function devportal_updates_manager_check_update_server() {

  // Whether this worked or not, we did just (try to) check for updates.
  variable_set('devportal_updates_last_check', REQUEST_TIME);

  $devportal_version = devportal_updates_get_devportal_version();
  if(!$devportal_version) {
    watchdog('devportal_updates', 'Cannot determine if Dev Portal is out of date since the Dev Portal Version cannot be determined.', array(), WATCHDOG_ERROR);
    return array(
      'status' => DEVPORTAL_UPDATE_STATUS_UNKNOWN,
      'severity' => REQUIREMENT_ERROR,
      'current_version' => 'unknown',
      'message' => t('Unable to determine installed version'),
    );
  }

  // Call server to get latest version information.
  $url = variable_get('devportal_updates_url', DEVPORTAL_UPDATES_DEFAULT_URL) . '/' . $devportal_version;
  $options['timeout'] = 5;
  $http_result = drupal_http_request($url, $options);

  // Decode HTTP Data response.
  $response = json_decode($http_result->data, TRUE);

  // Make sure response was valid.
  if($http_result->code != 200 || empty($response)) {
    watchdog('devportal_updates', 'Update server returned "%code: %error" when using URL: %url', array('%code' => $http_result->code, '%error' => $http_result->error, '%url' => $url), WATCHDOG_ERROR);
    return array(
      'status' => DEVPORTAL_UPDATE_STATUS_UNKNOWN,
      'severity' => REQUIREMENT_ERROR,
      'current_version' => NULL,
      'message' => t('There was a problem contacting the Dev Portal update server. Please verify Dev Portal can connect to the internet.'),
    );
  }

  // Response from server was valid, return info.
  return array(
    'status' => $response['status'],
    'severity' => $response['severity'],
    'current_version' => $response['current_version'],
    'message' => $response['message'],
  );
}


