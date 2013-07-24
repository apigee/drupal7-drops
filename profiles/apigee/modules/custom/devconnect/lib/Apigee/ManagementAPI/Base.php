<?php
namespace Apigee\ManagementAPI;

/**
 * @file
 * Base class for Management API classes. Handles a bit of the APIClient
 * invocation, which makes the actual HTTP calls.
 *
 * @author djohnson
 */

use Apigee\Exceptions\ResponseException as ResponseException;
use Apigee\Util\APIClient as APIClient;
use Apigee\Util\Log as Log;

class Base {

  /**
   * @var \Apigee\Util\APIClient
   */
  protected $client;
  /**
   * @var array
   * Contains raw data from Management API in a format compatible with older
   * PHP implementations of this library.
   */
  protected $debug_data;

  protected $double_url_encode;

  /**
   * Initializes the APIClient for this class.
   *
   * @param \Apigee\Util\APIClient $client
   */
  protected function init(APIClient $client, $double_url_encode = FALSE) {
    $this->client =& $client;
    $this->double_url_encode = $double_url_encode;
  }

  /**
   * URL-encodes parts of a KMS path.
   *
   * If $this->double_url_encode is TRUE, this is done twice. That is to say,
   * when FALSE, a space is %20, but when TRUE, it is %2520.
   *
   * @param $string
   * @return string
   */
  public function url_encode($string) {
    $string = rawurlencode($string);
    if ($this->double_url_encode) {
      // KMS R20 and earlier are buggy and require double-encoding
      $string = rawurlencode($string);
    }
    return $string;
  }

  /**
   * Returns the APIClient in use by this class, so it can be reused by other
   * instances of Base.
   *
   * @return \Apigee\Util\APIClient
   */
  public function get_client() {
    return $this->client;
  }

  /**
   * Returns data from the last API call in a way that clients of the old
   * versions of the devconnect_apigee classes can handle.
   *
   * @return array
   */
  public function get_debug_data() {
    return $this->debug_data;
  }

  /**
   * Returns the parsed response array from the last API call. If the HTTP
   * response code is not in the 2xx class, throws a ResponseException.
   *
   * This function also populates the debug_data member.
   *
   * @return array
   * @throws \Apigee\Exceptions\ResponseException
   */
  protected function get_response() {
    $response = $this->client->get_response();
    $response_code = $this->client->get_response_code();
    $opts = $this->client->get_response_opts();
    $status = $opts['response']->status_message;
    unset($opts['response']);
    $this->debug_data = array(
      'raw' => $this->client->get_response_string(),
      'opts' => $opts,
      'data' => $response,
      'code' => $response_code,
      'code_status' => $status,
      'code_class' => floor($response_code / 100)
    );
    if (!$this->client->was_successful()) {
      if (is_array($response) && isset($response_code) && isset($response['message'])) {
        $message = 'Code: ' . $response_code . '; Message: ' . $response['message'];
      }
      else {
        $message = 'API returned HTTP code of ' . $response_code . ' when fetching from ' . $opts['url'];
      }
      $this->debug_data['exception'] = $message;
      Log::write('Apigee\\ManagementAPI\\Base', Log::LOGLEVEL_ERROR, $this->client->get_response_string());
      throw new ResponseException($message, $response_code, NULL, $opts);
    }

    return $response;
  }

  /**
   * Reads the 'attributes' member of the Base subclass, and adds properly-
   * formatted members to the passed-by-reference $payload array. Note
   * that $this->attributes must be in scope (protected or public).
   *
   * @param $payload
   */
  protected function write_attributes(&$payload) {
    if (property_exists($this, 'attributes') && !empty($this->attributes)) {
      $payload['attributes'] = array();
      foreach ($this->attributes as $name => $value) {
        if ($name == 'apiResourcesInfo' && is_array($value)) {
          $value = json_encode($value);
        }
        $payload['attributes'][] = array('name' => $name, 'value' => $value);
      }
    }
  }

  /**
   * Reads the response from the Management API and populates the 'attributes'
   * member of the Base subclass. Note that $this->attributes must be in scope
   * (protected or public).
   *
   * @param array $response
   * @param bool $return
   * @return array|void
   */
  protected function read_attributes($response, $return = FALSE) {
    $attributes = array();

    // We cannot use property_exists() because it ignores scope.
    // But get_object_vars only returns variables within current scope.
    $this_attributes = get_object_vars($this);
    $has_attributes = (array_key_exists('attributes', $this_attributes) && is_array($this->attributes));

    if ($has_attributes) {
      if (isset($response['attributes']) && is_array($response['attributes'])) {
        foreach ($response['attributes'] as $attrib) {
          if (!is_array($attrib) || !array_key_exists('name', $attrib) || !array_key_exists('value', $attrib)) {
            continue;
          }
          if ($attrib['name'] == 'apiResourcesInfo') {
            $attrib['value'] = @json_decode($attrib['value'], TRUE);
          }
          $attributes[$attrib['name']] = $attrib['value'];
        }
      }
    }
    if ($return) {
      return $attributes;
    }
    if ($has_attributes) {
      $this->attributes = $attributes;
    }
  }
}