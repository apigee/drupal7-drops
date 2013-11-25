<?php
/**
 * @file
 * Base class for Management API test cases.
 */
use Apigee\Util\Crypto as Crypto;

class ManagementAPITestCase extends DrupalWebTestCase {

  /**
   * @var Apigee\Util\OrgConfig
   */
  protected $client;

  protected function setUp() {
    $config = Drupal::config('devconnect.settings');
    $org = $config->get('org');
    $endpoint = $config->get('endpoint');
    $username = $config->get('user');
    $pass_encrypted = $config->get('pass');

    if ($org == 'fixme' || $username == 'fixme') {
      throw new Exception('Org/endpoint/user/pass variables have not been set.');
    }

    // Now make the switch to the sandbox.
    parent::setUp();
    $password = Crypto::decrypt($pass_encrypted);
    $this->client = new Apigee\Util\OrgConfig($org, $endpoint, $username, $password);
  }
}