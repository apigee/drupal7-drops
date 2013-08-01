<?php

namespace Apigee\ManagementAPI;
use Apigee\Exceptions\ResponseException as ResponseException;

class KeyValueMap extends Base {

  /**
   * Initializes default values of all member variables.
   *
   * @param \Apigee\Util\APIClient $client
   */
  public function __construct(\Apigee\Util\APIClient $client) {
    $this->init($client);
    $this->base_url = '/organizations/' . $this->url_encode($client->get_org()) . '/keyvaluemaps';
  }

  /**
   * Fetches a value from a named map/key. If no such map or key is found,
   * returns NULL.
   *
   * @param $map_name
   * @param $key_name
   * @return null|string
   */
  public function get_entry_value($map_name, $key_name) {
    $url = $this->base_url . '/' . $this->url_encode($map_name) . '/entries/' . $this->url_encode($key_name);
    $value = NULL;
    try {
      $this->client->get($url);
      $response_obj = $this->get_response();
      $value = $response_obj['value'];
    }
    catch (ResponseException $e) {}
    return $value;
  }

  /**
   * Fetches all entries for a named map and returns them as an associative
   * array.
   *
   * @throws \Apigee\Exceptions\ResponseException
   *
   * @param $map_name
   * @return array
   */
  public function get_all_entries($map_name) {
    $url = $this->base_url . '/' . $this->url_encode($map_name);
    $this->client->get($url);
    $entries = array();
    // If something went wrong, the following line will throw a ResponseException.
    $response = $this->get_response();
    foreach ($response['entry'] as $entry) {
      $entries[$entry['name']] = $entry['value'];
    }
    return $entries;
  }

  /**
   * Sets a value for a named map/key. This performs both inserts and updates;
   * that is, if the key does not yet exist, it will create it.
   *
   * @throws \Apigee\Exceptions\ResponseException
   *
   * @param $map_name
   * @param $key_name
   * @param $value
   */
  public function set_entry_value($map_name, $key_name, $value) {
    $url = $this->base_url . '/' . $this->url_encode($map_name) . '/entries/' . $this->url_encode($key_name);
    $payload = array(
      'entry' => array(
        'name' => $key_name,
        'value' => $value
      ),
      'name' => $map_name
    );
    $this->client->put($url, $payload);
    // If something went wrong, the following line will throw a ResponseException.
    $this->get_response();
  }

  public function delete_entry($map_name, $key_name) {
    $url = $this->base_url . '/' . $this->url_encode($map_name) . '/entries/' . $this->url_encode($key_name);
    $this->client->delete($url);
    // If something went wrong, the following line will throw a ResponseException.
    $this->get_response();
  }

  public function create($map_name, $entries = NULL) {
    $payload = array(
      'entry' => array(),
      'name' => $map_name
    );
    if (!empty($entries) && is_array($entries)) {
      foreach ($entries as $key => $value) {
        $payload['entry'][] = array('name' => $key, 'value' => $value);
      }
    }
    $this->client->post($this->base_url, $payload);
    // If something went wrong, the following line will throw a ResponseException.
    $this->get_response();
  }

  public function delete($map_name) {
    $url = $this->base_url . '/' . $this->url_encode($map_name);
    $this->client->delete($url);
    // If something went wrong, the following line will throw a ResponseException.
    $this->get_response();
  }
}