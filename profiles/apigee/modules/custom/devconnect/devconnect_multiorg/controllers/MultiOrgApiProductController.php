<?php

class MultiOrgApiProductController extends ApiProductController {

  protected static function getOrgs(array $conditions = NULL) {
    if (empty($conditions) || !array_key_exists('orgName', $conditions)) {
      return devconnect_multiorg_get_configured_orgs();
    }
    try {
      $orgs = array(devconnect_multiorg_find_requested_org($conditions['orgName']));
    }
    catch (Exception $e) {
      watchdog('devconnect_multiorg', 'Invalid requested org “@orgName”; returning default instead', array('@orgName' => $conditions['orgName']), WATCHDOG_WARNING);
      $orgs = array('default');
    }
    return $orgs;
  }
}