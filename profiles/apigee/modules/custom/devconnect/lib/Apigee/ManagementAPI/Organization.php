<?php

namespace Apigee\ManagementAPI;

use \Apigee\Util\APIClient;

class Organization extends Base implements OrganizationInterface {

  /**
   * @var string
   */
  protected $name;

  /**
   * @var string
   */
  protected $displayName;

  /**
   * @var array
   */
  protected $environments;

  /**
   * @var array
   */
  protected $properties;

  /**
   * @var string
   */
  protected $type;

  /**
   * @var int
   * Unix timestamp in milliseconds
   */
  protected $createdAt;

  /**
   * @var string
   */
  protected $createdBy;

  /**
   * @var int
   * Unix timestamp in milliseconds
   */
  protected $lastModifiedAt;

  /**
   * @var string
   */
  protected $lastModifiedBy;

  /**
   * @return string
   */
  public function getName() {
    return $this->name;
  }

  /**
   * @return string
   */
  public function getDisplayName() {
    return $this->displayName;
  }

  /**
   * @return array
   */
  public function getEnvironments() {
    return $this->environments;
  }

  /**
   * @return array
   */
  public function getProperties() {
    return $this->properties;
  }

  /**
   * @param string $name
   * @return string|null
   */
  public function getProperty($name) {
    return isset($this->properties[$name]) ? $this->properties[$name] : NULL;
  }

  /**
   * @return string
   */
  public function getType() {
    return $this->type;
  }

  /**
   * @return int
   */
  public function getCreatedAt() {
    return $this->createdAt;
  }

  /**
   * @return string
   */
  public function getCreatedBy() {
    return $this->createdBy;
  }

  /**
   * @return int
   */
  public function getLastModifiedAt() {
    return $this->lastModifiedAt;
  }

  /**
   * @return string
   */
  public function getLastModifiedBy() {
    return $this->lastModifiedBy;
  }

  /**
   * Initializes default values of all member variables.
   *
   * @param \Apigee\Util\APIClient $client
   */
  public function __construct(APIClient $client) {
    $this->init($client);
    $this->baseUrl = '/organizations';
    $this->name = $client->getOrg();
  }

  /**
   * @param string|null $org
   */
  public function load($org = NULL) {
    $org = isset($org) ? $org : $this->name;
    $url = $this->baseUrl . '/' . $this->urlEncode($org);
    $this->client->get($url);
    $organization = $this->getResponse();

    $this->name = $organization['name'];
    $this->displayName = $organization['displayName'];
    $this->environments = $organization['environments'];
    $this->type = $organization['type'];
    $this->createAt = $organization['createdAt'];
    $this->createdBy = $organization['createdBy'];
    $this->lastModifiedAt = $organization['lastModifiedAt'];
    $this->lastModifiedBy = $organization['lastModifiedBy'];
    $this->properties = array();

    if (isset($organization['properties'])) {
      foreach ($organization['properties'] as $prop) {
        list($property) = $prop;
        $this->properties[$property['name']] = $property['value'];
      }
    }
  }
}