<?php
/**
 * @file
 * Base class for API object classes. Handles a bit of the APIClient
 * invocation, which makes the actual HTTP calls.
 *
 * @author djohnson
 */

namespace Apigee\Util;

use Apigee\Exceptions\ResponseException as ResponseException;
use Apigee\Exceptions\IllegalMethodException as IllegalMethodException;

class APIObject {

  /**
   * @var \Apigee\Util\APIClient
   */
  protected $client;

  /**
   * @var array
   * Contains raw data from Management API in a format compatible with older
   * PHP implementations of this library.
   */
  protected $debugData;

  /**
   * Initializes the APIClient for this class.
   *
   * @param \Apigee\Util\APIClient $client
   */
  protected function init(APIClient $client) {
    $this->client =& $client;
  }

  /**
   * Returns the APIClient in use by this class, so it can be reused by other
   * instances of Base.
   *
   * @return \Apigee\Util\APIClient
   */
  public function getClient() {
    return $this->client;
  }

  /**
   * Returns the parsed response array from the last API call. If the HTTP
   * response code is not in the 2xx class, throws a ResponseException.
   *
   * This function also populates the debugData member.
   *
   * @return array
   * @throws \Apigee\Exceptions\ResponseException
   */
  protected function getResponse() {
    $response = $this->client->getResponse();
    $response_code = $this->client->getResponseCode();
    $opts = $this->client->getResponseOpts();
    $status = $opts['response']->statusMessage;
    unset($opts['response']);
    $this->debugData = array(
      'raw' => $this->client->getResponseString(),
      'opts' => $opts,
      'data' => $response,
      'code' => $response_code,
      'code_status' => $status,
      'code_class' => floor($response_code / 100)
    );
    if (!$this->client->wasSuccessful()) {
      if (is_array($response) && isset($response_code) && isset($response['message'])) {
        $message = 'Code: ' . $response_code . '; Message: ' . $response['message'];
      }
      else {
        $message = 'API returned HTTP code of ' . $response_code . ' when fetching from ' . $opts['url'];
      }
      $this->debugData['exception'] = $message;
      Log::write(__CLASS__, Log::LOGLEVEL_ERROR, $this->client->getResponseString());
      $uri = preg_replace('!^(https?://)[^:]*:(.*)$!', '$1$2', $opts['url']);

      throw new ResponseException($message, $response_code, $uri, $opts);
    }

    return $response;
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
    $class = get_class();

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