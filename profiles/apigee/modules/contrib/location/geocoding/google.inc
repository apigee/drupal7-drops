<?php

/**
 * @file
 * Google geocoder.
 */

/**
 * Returns an XML document containing the list of countries supported by the
 * Google geocoder.
 * A cached version is stored in the Drupal cache in case Google is unreachable.
 */
function google_geocode_country_list_xml() {
  // Get the google data from the feed.
  $source = drupal_http_request('http://spreadsheets.google.com/feeds/list/p9pdwsai2hDMsLkXsoM05KQ/default/public/values');

  if (!$source->data) {
    // Use the cache.
    $data = cache_get('location_google');
    if (!defined('LIBXML_VERSION') || (version_compare(phpversion(), '5.1.0', '<'))) {
      $xml = simplexml_load_string($data->data, NULL);
    }
    else {
      $xml = simplexml_load_string($data->data, NULL, LIBXML_NOERROR | LIBXML_NOWARNING);
    }
  }
  else {
    if (!defined('LIBXML_VERSION') || (version_compare(phpversion(), '5.1.0', '<'))) {
      $xml = simplexml_load_string($source->data, NULL);
      // Stores the XML in the cache to eventually use it later.
      cache_set('location_google', $xml->asXML());
    }
    else {
      $xml = simplexml_load_string($source->data, NULL, LIBXML_NOERROR | LIBXML_NOWARNING);
      // Store the XML in the cache to eventually use it later.
      cache_set('location_google', $xml->asXML());
    }
  }

  return $xml;
}

/**
 * Return the list of ISO3166 codes supported by this geocoder.
 * Coverage list: http://gmaps-samples.googlecode.com/svn/trunk/mapcoverage_filtered.html
 * Coverage list feed: http://spreadsheets.google.com/feeds/list/p9pdwsai2hDMsLkXsoM05KQ/default/public/values
 */
function google_geocode_country_list() {
  // Get the google data from the feed.
  $xml = google_geocode_country_list_xml();

  // Loop through google data and find all valid entries.
  $regionclean = array();
  foreach($xml->entry as $region) {
    $pos = strpos($region->content, 'geocoding:') + 11;
    $geocoding = substr($region->content, $pos, strpos($region->content, ',', $pos) - $pos);
    if (strpos($geocoding, "Yes") !== FALSE) {
     $regionclean[] = t(htmlentities($region->title));
    }
  }

  // Get the countries list and clean it up so that names will match to google.
  // The regex removes parenthetical items so that both of the "Congo" entries
  // and the "Coco Islands" work.
  // The $countriesfixes overwrites values in the Drupal API countries list
  // with values that will match to google's entries.
  // "Sao Tome and Principe" are non-accented in the Drupal API so the entry
  // here is to match the htmlentities() fix in the foreach loop below.
  // Note: it may be neccessary to adjust/add to the fixes list in the future
  // if google adds countries that don't match the Drupal API list for whatever
  // reason.
  $countries = location_get_iso3166_list();
  $regex = "#[ (].*[)]#e";
  $cntryclean = preg_replace($regex, "", $countries);
  $countriesfixes = array_merge($cntryclean, array(
    "hk" => t("China"),
    "mo" => t("China"),
    "pn" => t("Pitcairn Islands"),
    "wf" => t("Wallis Futuna"),
    "st" => t("S&Atilde;&pound;o Tom&Atilde;&copy; and Pr&Atilde;&shy;ncipe"),
  ));

  // Compare new google data found to fixed country name values and return
  // matches with abbreviations as keys.
  $googlematched = array_intersect($countriesfixes, $regionclean);

  // Compare new keys to original Drupal API and return the array with the
  // original name values.
  $fixedkeys = array_intersect_key($countries, $googlematched);
  return array_keys($fixedkeys);
}

/**
 * Return general information about this geocoder.
 */
function google_geocode_info() {
  return array(
    'name' => 'Google Maps',
    'url' => 'http://maps.google.com',
    'tos' => 'http://www.google.com/help/terms_local.html',
    'general' => TRUE,
  );
}

/**
 * Perform a geocode on a location array.
 * @param $location
 *   The location array to process.
 * @return
 *   an associative array with keys 'lat' and 'lon' containing the coordinates.
 */
function google_geocode_location($location = array()) {

  $delay_trigger = &drupal_static(__FUNCTION__);
  $delay = variable_get('location_geocode_google_delay', 0);
  if ($delay > 0 && $delay_trigger) {
    usleep($delay * 1000);
  }

  $query = array(
    'region' => $location['country'],
    'address' => _google_geocode_flatten($location),
    'sensor' => 'false', // Required by TOS.
  );

  $url = url('http://maps.googleapis.com/maps/api/geocode/json', array(
    'query' => $query,
    'external' => TRUE,
  ));

  $http_reply = drupal_http_request($url);
  $delay_trigger = TRUE;
  $data = json_decode($http_reply->data);

  $status_code = $data->status;
  if ($status_code != 'OK') {
    watchdog('location', 'Google geocoding returned status code: %status_code for the query url: %url', array('%status_code' => $data->status, '%url' => $url));
    return NULL;
  }
  $location = $data->results[0]->geometry->location;

  return array('lat' => $location->lat, 'lon' => $location->lng);
}

/**
 * General settings for this geocoder.
 */
function google_geocode_settings() {
  $form = array();
  $key = '';
  if (function_exists('gmap_get_key')) {
    $key = gmap_get_key();
  }

  if (!empty($key)) {
    $form['location_geocode_google_apikey'] = array(
      '#type' => 'item',
      '#title' => t('Google Maps API Key'),
      '#value' => $key,
      '#description' => t('The key in use was automatically provided by GMap.'),
    );
  }
  else {
    $form['location_geocode_google_apikey'] = array(
      '#type' => 'textfield',
      '#title' => t('Google Maps API Key'),
      '#size' => 64,
      '#maxlength' => 128,
      '#default_value' => variable_get('location_geocode_google_apikey', ''),
      '#description' => t('In order to use the Google Maps API geocoding web-service, you will need a Google Maps API Key.  You can obtain one at the !sign_up_link for the !google_maps_api.  PLEASE NOTE: You will <em>not</em> have to re-enter your API key for each country for which you have selected Google Maps for geocoding.  This setting is global.', array('!sign_up_link' => '<a href="http://www.google.com/apis/maps/signup.html">sign-up page</a>', '!google_maps_api' => '<a href="http://www.google.com/apis/maps/">Google Maps API</a>'))
    );
  }

  $form['location_geocode_google_delay'] = array(
    '#type' => 'textfield',
    '#title' => t('Delay between geocoding requests (is milliseconds)'),
    '#description' => t('To avoid a 620 error (denial of service) from Google, you can add a delay between geocoding requests. 200ms is recommended.'),
    '#default_value' => variable_get('location_geocode_google_delay', 0),
    '#size' => 10,
  );

  $country = arg(5);
  if ($country) {
    $form['location_geocode_' . $country . '_google_accuracy_code'] = array(
      '#type' => 'select',
      '#title' => t('Google Maps Geocoding Accuracy for %country', array('%country' => $country ) ),
      '#default_value' => variable_get('location_geocode_' . $country . '_google_accuracy_code', variable_get('location_geocode_google_minimum_accuracy', '3')),
      '#options' => location_google_geocode_accuracy_codes(),
      '#description' => t('The minimum required accuracy for the geolocation data to be saved.'),
    );
  }
  return $form;
}

function _google_geocode_flatten($location = array()) {
  // Check if its a valid address
  if (empty($location)) {
    return '';
  }

  $address = '';
  if (!empty($location['street'])) {
    $address .= $location['street'];
  }

  if (!empty($location['city'])) {
    if (!empty($address)) {
      $address .= ', ';
    }

    $address .= $location['city'];
  }

  if (!empty($location['province'])) {
    if (!empty($address)) {
      $address .= ', ';
    }

    // @@@ Fix this!
    if (substr($location['province'], 0, 3) == $location['country'] . '-') {
      $address .= substr($location['province'], 3);
      watchdog('Location', 'BUG: Country found in province attribute.');
    }
    else {
      $address .= $location['province'];
    }
  }

  if (!empty($location['postal_code'])) {
    if (!empty($address)) {
      $address .= ', ';
    }
    $address .= $location['postal_code'];
  }

  if (!empty($location['country'])) {
    if (!empty($address)) {
      $address .= ', ';
    }
    $address .= $location['country'];
  }

  return $address;
}
