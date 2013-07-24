<?php

namespace Apigee\Util;

/**
 * @file
 * Acts as a wrapper for all HTTP-based API invocations.
 *
 * @author djohnson
 */

use Apigee\Exceptions\EnvironmentException as EnvironmentException;
use Apigee\Exceptions\ResponseException as ResponseException;
use Apigee\Util\Log as Log;

class APIClient {

  /**
   * @var string
   */
  protected $endpoint;
  /**
   * @var string
   */
  private $org;
  /**
   * @var string
   */
  private $user;
  /**
   * @var string
   */
  private $pass;

  /**
   * @var int
   */
  private $response_code;
  /**
   * @var string
   */
  private $response_string;
  /**
   * @var array
   */
  private $response_obj;
  /**
   * @var array
   */
  private $response_opts;

  /**
   * @var float
   */
  private $timeout;

  /**
   * Gets a singleton for the given endpoint/org/user/pass combo.
   *
   * Determines the environment based on the endpoint, then invokes
   * self::get_instance().
   *
   * @static
   * @param $endpoint
   * @param $org
   * @param $user
   * @param $pass
   * @return \Apigee\Util\APIClient
   */
  public static function get_instance_by_endpoint($endpoint, $org, $user, $pass) {
    if (strpos($endpoint, 'jupiter') !== FALSE) {
      $env = 'test';
    }
    elseif (strpos($endpoint, 'mars') !== FALSE) {
      $env = 'stage';
    }
    else {
      $env = 'prod';
    }
    return self::get_instance($env, $org, $user, $pass);
  }

  /**
   * Gets a singleton for the given environment/org/user/pass combo.
   *
   * @static
   * @param $environment
   * @param $org
   * @param $user
   * @param $pass
   * @return \Apigee\Util\APIClient
   */
  public static function get_instance($environment, $org, $user, $pass) {
    static $instances;
    if (!isset($instances)) {
      $instances = array();
    }
    $key = "$environment/$org/$user/$pass";
    if (!isset($instances[$key])) {
      $instances[$key] = new APIClient($environment, $org, $user, $pass);
    }
    return $instances[$key];
  }

  /**
   * Determines if the last HTTP call resulted in a success code.
   *
   * @return bool
   */
  public function was_successful() {
    return (floor($this->response_code / 100) == 2);
  }

  /**
   * Returns the HTTP response code from the last call.
   * @return int
   */
  public function get_response_code() {
    return $this->response_code;
  }

  /**
   * Returns the raw response body from the last call.
   * @return string
   */
  public function get_response_string() {
    return $this->response_string;
  }

  /**
   * Returns the parsed response array from the last call.
   * @return array
   */
  public function get_response() {
    return $this->response_obj;
  }

  /**
   * Returns the org name that this class was initialized with.
   * @return string
   */
  public function get_org() {
    return $this->org;
  }

  /**
   * Returns associative array of response options from the last call. This
   * function is primarily intended for logging purposes.
   *
   * @return array
   */
  public function get_response_opts() {
    return $this->response_opts;
  }

  /**
   * Returns the URL of our endpoint
   *
   * @return string
   */
  public function get_endpoint() {
    return $this->endpoint;
  }

  /**
   * Initialize this object and determine its endpoint URL.
   *
   * @param $environment
   * @param $org
   * @param $user
   * @param $pass
   */
  public function __construct($environment, $org, $user, $pass) {

    switch ($environment) {
      case 'dit':
      case 'test':
        $this->endpoint = 'https://api.jupiter.apigee.net/v1';
        break;
      case 'stage':
        $this->endpoint = 'https://api.mars.apigee.net/v1';
        break;
      case 'prod':
        $this->endpoint = 'https://api.enterprise.apigee.com/v1';
        break;
      default:
        if (preg_match('!^https?://!', $environment)) {
          $this->endpoint = $environment;
        }
        else {
          throw new EnvironmentException('Unknown environment "' . $environment . '".');
        }
    }
    $this->org = $org;
    $this->user = $user;
    $this->pass = $pass;

    $this->response_code = NULL;
    $this->response_string = NULL;
    $this->response_obj = NULL;
    $this->response_opts = array();

    $this->timeout = 10.0;
  }

  public function set_timeout($t) {
    $this->timeout = floatval($t);
  }

  /**
   * Perform an HTTP POST on a URI with a given object as payload.
   * The result can be read from $this->response_* variables.
   *
   * @param string $url
   * @param mixed $payload
   */
  public function post($url, $payload, $content_type = 'application/json; charset=utf-8', $accept_type = 'application/json; charset=utf-8') {
    self::prepare_payload($content_type, $payload);

    $opts = array(
      'method' => 'POST',
      'headers' => array(
        'Accept' => $accept_type,
        'Content-Type' => $content_type
      ),
      'data' => $payload
    );
    if (strlen($payload) == 0) {
      unset($opts['headers']['Content-Type']);
    }
    $this->exec($this->endpoint . $url, $opts);
  }

  /**
   * Perform an HTTP GET on a URI. The result can be read from
   * $this->response_* variables.
   *
   * @param string $url
   */
  public function get($url, $accept_mime_type = 'application/json; charset=utf-8') {
    $opts = array(
      'method' => 'GET',
      'headers' => array('Accept' => $accept_mime_type)
    );
    $this->exec($this->endpoint . $url, $opts);
  }

  /**
   * Perform an HTTP DELETE on a URI. The result can be read from
   * $this->response_* variables.
   *
   * @param string $url
   */
  public function delete($url, $accept = 'application/json; charset=utf-8') {

    $opts = array('method' => 'DELETE');
    if (!empty($accept)) {
      $opts['headers'] = array('Accept' => $accept);
    }
    $this->exec($this->endpoint . $url, $opts);
  }

  /**
   * Perform an HTTP PUT on a URI. The result can be read from
   * $this->response_* variables.
   *
   * @param $url
   * @param $string_payload
   * @param string $content_type
   */
  public function put($url, $payload, $content_type = 'application/json; charset=utf-8') {
    self::prepare_payload($content_type, $payload);
    $opts = array(
      'headers' => array('Content-Type' => $content_type),
      'method' => 'PUT',
      'data' => $payload
    );
    $this->exec($this->endpoint . $url, $opts);
  }

  /**
   * Perform an HTTP HEAD on a URI.
   *
   * @param $url
   * @param int $timeout_ms
   */
  public function head($url, $timeout_ms = 5000) {
    $opts = array(
      'timeout' => $timeout_ms / 1000,
      'headers' => array('Accept' => 'application/json; charset=utf-8'),
      'method' => 'HEAD'
    );
    $this->exec($this->endpoint . $url, $opts);
  }

  public static function make_http_request($url, $opts) {
    if (function_exists('drupal_http_request')) {
      // If we're running within Drupal, use its version of the function.
      $response = drupal_http_request($url, $opts);
    }
    else {
      $response = \Apigee\Util\HTTPClient::exec($url, $opts);
    }
    // Workaround for drupal_http_request's failure to handle return codes of 201 and 202
    if (property_exists($response, 'error') && floor($response->code / 100) != 2) {
      throw new ResponseException($response->error, $response->code, $url, $opts, (property_exists($response, 'data') ? $response->data : NULL));
    }
    return $response;
  }

  /**
   * Makes an HTTP call and populates internal properties with the results.
   *
   * @param $url
   * @param $opts
   */
  private function exec($url, $opts) {

    // Inject user/pass into URI
    $url = preg_replace('!^(https?://)(.*)$!i', '$1' . $this->user . ':' . $this->pass . '@$2', $url);

    $response = self::make_http_request($url, $opts);

    $raw_response = $response->data;
    $this->response_code = $response->code;
    if (isset($response->headers['content-type'])) {
      $content_type = $response->headers['content-type'];
    }
    else {
      $content_type = 'text/plain';
    }

    $this->response_obj = $raw_response;
    self::parse_payload($content_type, $this->response_obj);
    $this->response_string = $raw_response;

    if (!empty($this->user) && !empty($this->pass)) {
      $auth_string = base64_encode($this->user . ':' . $this->pass);
    }

    $opts['url'] = $url;
    $opts['authentication'] = $this->user . ':[encrypted]';
    $opts['response'] = $response;
    if (isset($auth_string) && is_object($opts['response']) && property_exists($opts['response'], 'request')) {
      $opts['response']->request = str_replace($auth_string, '[encrypted]', $opts['response']->request);
    }
    Log::write('APIClient\\exec', Log::LOGLEVEL_DEBUG, $opts);

    $this->response_opts = $opts;
  }

  /**
   * If payload is not already a string, stringify it (based on its content-type).
   *
   * @static
   * @param $content_type
   * @param $payload
   */
  private static function prepare_payload($content_type, &$payload) {
    // If content_type includes charset, strip it off.
    if (($i = strpos($content_type, ';')) !== FALSE) {
      $content_type = trim(substr($content_type, 0, $i));
    }
    if ($content_type == 'application/json' && (is_object($payload) || is_array($payload))) {
      // Turn objects/arrays into JSON strings.
      $payload = json_encode($payload);
    }
    elseif ($content_type == 'application/xml') {
      // Turn XML document representations into strings.
      if ($payload instanceof \DOMDocument) {
        $payload = $payload->saveXML($payload->documentElement);
      }
      elseif ($payload instanceof \SimpleXMLElement) {
        $payload = $payload->asXML();
        // strip off processing instruction if present
        $payload = preg_replace('!^<\?[^?]+\?>!', '', $payload);
      }
    }
  }

  /**
   * Turn string payload into object/array when possible (based on its
   * content-type).
   *
   * @static
   * @param $content_type
   * @param $payload
   */
  private static function parse_payload($content_type, &$payload) {
    // If content_type includes charset, strip it off.
    if (($i = strpos($content_type, ';')) !== FALSE) {
      $content_type = trim(substr($content_type, 0, $i));
    }
    // If content_type is JSON, parse it out
    if ($content_type == 'application/json') {
      $payload = @json_decode($payload, TRUE);
    }
    // If content_type is XML, return DOMDocument
    elseif ($content_type == 'application/xml') {
      // loadXML emits E_STRICT when called statically, so @suppress it
      $payload = @\DOMDocument::loadXML($payload);
    }
  }
}