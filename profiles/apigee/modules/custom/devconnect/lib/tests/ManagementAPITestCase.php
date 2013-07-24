<?php
/**
 * @file
 * Base class for Management API test cases.
 */
use Apigee\Util\Crypto as Crypto;

class ManagementAPITestCase extends DrupalWebTestCase {

  protected $client;

  protected $double_escape_urls;

  protected function setUp() {
    $org = variable_get('devconnect_org', NULL);
    $endpoint = variable_get('devconnect_endpoint', NULL);
    $endpoint_auth = variable_get('devconnect_curlauth', NULL);
    $this->double_escape_urls = (bool)variable_get('devconnect_appname_bug', FALSE);

    parent::setUp();

    if (empty($org) || empty($endpoint) || empty($endpoint_auth)) {
      throw new Exception('Cannot read org/endpoint/endpoint-auth variables.');
    }

    list($username, $pass_encrypted) = explode(':', $endpoint_auth);
    $password = Crypto::decrypt($pass_encrypted);
    $this->client = Apigee\Util\APIClient::get_instance_by_endpoint($endpoint, $org, $username, $password);
  }
}