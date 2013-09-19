<?php
namespace Apigee\ManagementAPI;

use \Apigee\Util\APIClient;

class UserRole extends Base implements UserRoleInterface {

  private $baseUrl;

  public function __construct(APIClient $client) {
    $this->client = $client;
    $this->baseUrl = '/organizations/' . $this->urlEncode($client->getOrg()) . '/userroles';
  }

  public function getUsersByRole($role) {
    if (!in_array($role, $this->listRoles())) {
      return array();
    }
    $this->client->get($this->baseUrl . '/' . $this->urlEncode($role) . '/users');
    return $this->getResponse();
  }

  public function listRoles() {
    static $roles;
    if (empty($roles)) {
      $this->client->get($this->baseUrl);
      $roles = $this->getResponse();
    }
    return $roles;
  }
}
