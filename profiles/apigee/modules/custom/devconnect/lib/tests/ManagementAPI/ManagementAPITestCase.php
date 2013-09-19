<?php
/**
 * @file
 * Base class for Management API test cases.
 */
use Apigee\Util\Crypto as Crypto;

class ManagementAPITestCase extends DrupalWebTestCase {

  /**
   * @var Apigee\Util\APIClient
   */
  protected $client;

  protected function setUp() {
    // Read Drupal vars *before* simpletest switches us to a sandbox db
    $org = variable_get('devconnect_org', NULL);
    $endpoint = variable_get('devconnect_endpoint', NULL);
    $endpoint_auth = variable_get('devconnect_curlauth', NULL);

    // Now make the switch to the sandbox.
    parent::setUp();

    if (empty($org) || empty($endpoint) || empty($endpoint_auth)) {
      throw new Exception('Cannot read org/endpoint/endpoint-auth variables.');
    }

    list($username, $pass_encrypted) = explode(':', $endpoint_auth);
    $password = Crypto::decrypt($pass_encrypted);
    $this->client = Apigee\Util\APIClient::getInstance($endpoint, $org, $username, $password);
  }
}