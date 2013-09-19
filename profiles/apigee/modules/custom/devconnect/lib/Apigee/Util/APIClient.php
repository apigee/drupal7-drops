<?php
/**
 * @file
 * Acts as a wrapper for all HTTP-based API invocations.
 *
 * @author djohnson
 */

namespace Apigee\Util;

use Apigee\Exceptions\EnvironmentException as EnvironmentException;
use Apigee\Exceptions\ResponseException as ResponseException;
use Apigee\Exceptions\IllegalMethodException as IllegalMethodException;
use Apigee\Util\Log as Log;

class APIClient {

  /**
   * @var string
   */
  protected $endpoint;
  /**
   * @var string
   */
  protected $org;
  /**
   * @var string
   */
  protected $user;
  /**
   * @var string
   */
  protected $pass;

  /**
   * @var int
   */
  protected $responseCode;
  /**
   * @var string
   */
  protected $responseString;
  /**
   * @var array
   */
  protected $responseObj;
  /**
   * @var array
   */
  protected $responseOpts;
  /**
   * @var string
   */
  protected $responseMimeType;

  /**
   * Allows implementation-specific attributes to be set on the client.
   * @var array
   */
  protected $attributes;

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
  public static function getInstance($environment, $org, $user, $pass) {
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
  public function wasSuccessful() {
    return (floor($this->responseCode / 100) == 2);
  }

  /**
   * Gets a named attribute, or $default_value if the attribute doesn't exist.
   *
   * @param string $name
   * @param mixed $default_value
   * @return null
   */
  public function getAttribute($name, $default_value = NULL) {
    if (array_key_exists($name, $this->attributes)) {
      return $this->attributes[$name];
    }
    return $default_value;
  }

  /**
   * Sets a named attribute.
   *
   * @param $name
   * @param $value
   */
  public function setAttribute($name, $value) {
    $this->attributes[$name] = $value;
  }

  /**
   * Returns the HTTP response code from the last call.
   * @return int
   */
  public function getResponseCode() {
    return $this->responseCode;
  }

  /**
   * Returns the raw response body from the last call.
   * @return string
   */
  public function getResponseString() {
    return $this->responseString;
  }

  /**
   * Returns the parsed response array from the last call.
   * @return array
   */
  public function getResponse() {
    return $this->responseObj;
  }

  /**
   * Returns the org name that this class was initialized with.
   * @return string
   */
  public function getOrg() {
    return $this->org;
  }

  /**
   * Returns associative array of response options from the last call. This
   * function is primarily intended for logging purposes.
   *
   * @return array
   */
  public function getResponseOpts() {
    return $this->responseOpts;
  }

  /**
   * Returns the URL of our endpoint
   *
   * @return string
   */
  public function getEndpoint() {
    return $this->endpoint;
  }

  public function getResponseMimeType() {
    return $this->responseMimeType;
  }

  /**
   * Initializes this object and determines its endpoint URL.
   *
   * @throws \Apigee\Exceptions\EnvironmentException
   *
   * @param string $environment
   * @param string $org
   * @param string $user
   * @param string $pass
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
          $this->endpoint = rtrim($environment, '/');
        }
        else {
          throw new EnvironmentException('Unknown environment "' . $environment . '".');
        }
    }
    $this->org = $org;
    $this->user = $user;
    $this->pass = $pass;

    $this->responseCode = NULL;
    $this->responseString = NULL;
    $this->responseObj = NULL;
    $this->responseOpts = array();
    $this->attributes = array();
    $this->responseMimeType = NULL;
  }

  /**
   * Performs an HTTP POST on a URI with a given object as payload.
   * The result can be read from $this->response_* variables.
   *
   * @param string $url
   * @param mixed $payload
   * @param string $content_type
   * @param string $accept_type
   * @param array $custom_headers
   */
  public function post($url, $payload, $content_type = 'application/json; charset=utf-8', $accept_type = 'application/json; charset=utf-8', $custom_headers = array()) {
    self::preparePayload($content_type, $payload);

    $opts = array(
      'method' => 'POST',
      'headers' => array(
        'Accept' => $accept_type,
        'Content-Type' => $content_type
      ),
      'payload' => $payload
    );
    if (!empty($custom_headers)) {
      $opts['headers'] += $custom_headers;
    }
    if (strlen($payload) == 0) {
      unset($opts['headers']['Content-Type']);
    }
    $this->exec($this->endpoint . $url, $opts);
  }

  /**
   * Performs an HTTP GET on a URI. The result can be read from
   * $this->response_* variables.
   *
   * @param string $url
   * @param string $accept_mime_type
   * @param array $custom_headers
   */
  public function get($url, $accept_mime_type = 'application/json; charset=utf-8', $custom_headers = array()) {
    $opts = array(
      'method' => 'GET',
      'headers' => array('Accept' => $accept_mime_type)
    );
    if (!empty($custom_headers)) {
      $opts['headers'] += $custom_headers;
    }
    $this->exec($this->endpoint . $url, $opts);
  }

  /**
   * Performs an HTTP DELETE on a URI. The result can be read from
   * $this->response_* variables.
   *
   * @param string $url
   * @param string $accept
   * @param array $custom_headers
   */
  public function delete($url, $accept = 'application/json; charset=utf-8', $custom_headers = array()) {

    $opts = array('method' => 'DELETE', 'headers' => array());
    if (!empty($accept)) {
      $opts['headers']['Accept'] = $accept;
    }
    if (!empty($custom_headers)) {
      $opts['headers'] += $custom_headers;
    }
    $this->exec($this->endpoint . $url, $opts);
  }

  /**
   * Performs an HTTP PUT on a URI. The result can be read from
   * $this->response_* variables.
   *
   * @param string $url
   * @param mixed $payload
   * @param string $content_type
   * @param array $custom_headers
   */
  public function put($url, $payload, $content_type = 'application/json; charset=utf-8', $custom_headers = array()) {
    self::preparePayload($content_type, $payload);
    $opts = array(
      'headers' => array('Content-Type' => $content_type),
      'method' => 'PUT',
      'payload' => $payload
    );
    if (!empty($custom_headers)) {
      $opts['headers'] += $custom_headers;
    }
    $this->exec($this->endpoint . $url, $opts);
  }

  /**
   * Performs an HTTP HEAD on a URI.
   *
   * @param $url
   * @param array $custom_headers
   */
  public function head($url, $custom_headers = array()) {
    $opts = array(
      'headers' => array('Accept' => 'application/json; charset=utf-8'),
      'method' => 'HEAD'
    );
    if (!empty($custom_headers)) {
      $opts['headers'] += $custom_headers;
    }
    $this->exec($this->endpoint . $url, $opts);
  }

  /**
   * Executes an HTTP request and returns a response object.
   *
   * @static
   * @param $url
   * @param $opts
   * @return \Apigee\Util\HTTPResponse
   * @throws \Apigee\Exceptions\ResponseException
   */
  public static function makeHttpRequest($url, $opts) {
    $response = HTTPClient::exec($url, $opts);
    if (property_exists($response, 'error') && floor($response->code / 100) != 2) {
      $exc = new ResponseException($response->error, $response->code, $url, $opts, (property_exists($response, 'data') ? $response->data : NULL));
      $exc->responseObj = $response;
      throw $exc;
    }
    return $response;
  }

  /**
   * Makes an HTTP call and populates internal properties with the results.
   *
   * @throws \Apigee\Exceptions\ResponseException
   * @param $url
   * @param $opts
   */
  protected function exec($url, $opts) {

    if (!empty($this->user) && !empty($this->pass) && !array_key_exists('user', $opts) && !array_key_exists('pass', $opts)) {
      $opts['user'] = $this->user;
      $opts['pass'] = $this->pass;
    }
    if (!empty($this->user) && !empty($this->pass)) {
      $auth_string = base64_encode($this->user . ':' . $this->pass);
    }

    try {
      $response = self::makeHttpRequest($url, $opts);
    }
    catch (ResponseException $e) {
      $obj_dump = (string)$e->responseObj;
      if (isset($auth_string)) {
        $obj_dump = str_replace($auth_string, '[encrypted]', $obj_dump);
      }
      Log::write(__CLASS__, Log::LOGLEVEL_DEBUG, $obj_dump);
      throw $e;
    }

    $raw_response = $response->data;
    $this->responseCode = $response->code;
    if (isset($response->headers['content-type'])) {
      $this->responseMimeType = $response->headers['content-type'];
    }
    else {
      $this->responseMimeType = 'text/plain';
    }

    $this->responseObj = $raw_response;
    self::parsePayload($this->responseMimeType, $this->responseObj);
    $this->responseString = $raw_response;


    $opts['url'] = $url;
    $opts['authentication'] = $this->user . ':[encrypted]';
    $opts['response'] = $response;
    if (isset($auth_string) && is_object($opts['response']) && property_exists($opts['response'], 'request')) {
      $opts['response']->request = str_replace($auth_string, '[encrypted]', $opts['response']->request);
    }
    Log::write(__CLASS__, Log::LOGLEVEL_DEBUG, $opts);

    $this->responseOpts = $opts;
  }

  /**
   * If payload is not already a string, stringify it (based on its content-type).
   *
   * @static
   * @param $content_type
   * @param $payload
   */
  protected static function preparePayload($content_type, &$payload) {
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
  protected static function parsePayload($content_type, &$payload) {
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
      $d = new \DOMDocument();
      $d->loadXML($payload);
      $payload = $d;
    }
  }


  /**
   * Intercepts any snake_case method invocations that aren't already
   * defined, turns them into camelCase, and tries to invoke them.
   *
   * Incoming snake_case method names must contain no uppercase to
   * qualify for this transmogrification. In the interest of efficiency,
   * we also don't process any method names not containing an underscore.
   *
   * @TODO When we require PHP 5.4, make this a mix-in.
   *
   * @param string $method
   * @param array $args
   * @return mixed
   * @throws \Apigee\Exceptions\IllegalMethodException
   */
  public function __call($method, $args) {
    $class = get_class($this);

    if ($method == strtolower($method) && strpos($method, '_') !== FALSE) {
      $parts = explode('_', $method);
      $camel_case = array_shift($parts);
      foreach ($parts as $part) {
        $camel_case .= ucfirst($part);
      }
      if (method_exists($this, $camel_case)) {
        Log::warnDeprecated($class);
        return call_user_func_array(array($this, $camel_case), $args);
      }
      throw new IllegalMethodException('Class “' . $class . '” contains no such method “' . $method . '” (even after camelCasing)');
    }
    throw new IllegalMethodException('Class “' . $class . '” contains no such method “' . $method . '”');
  }

  /**
   * Same as above, except for static methods
   *
   * @param $method
   * @param $args
   * @return mixed
   * @throws \Apigee\Exceptions\IllegalMethodException
   */
  public static function __callstatic($method, $args) {
    $class = get_class();

    if ($method == strtolower($method) && strpos($method, '_') !== FALSE) {
      $parts = explode('_', $method);
      $camel_case = array_shift($parts);
      foreach ($parts as $part) {
        $camel_case .= ucfirst($part);
      }
      if (method_exists($class, $camel_case)) {
        Log::warnDeprecated($class);
        return forward_static_call_array(array($class, $camel_case), $args);
      }
      throw new IllegalMethodException('Class “' . $class . '” contains no such static method “' . $method . '” (even after camelCasing)');
    }
    throw new IllegalMethodException('Class “' . $class . '” contains no such static method “' . $method . '”');
  }

}
