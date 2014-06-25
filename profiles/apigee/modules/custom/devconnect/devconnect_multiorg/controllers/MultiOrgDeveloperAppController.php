<?php

use Drupal\devconnect_developer_apps\DeveloperAppEntity;

class MultiOrgDeveloperAppController extends DeveloperAppController {

  protected static function getConfig(DeveloperAppEntity $entity = NULL) {
    if (empty($entity)) {
      $org = 'default';
    }
    else {
      try {
        $org = devconnect_multiorg_find_requested_org($entity->orgName);
      } catch (Exception $e) {
        $org = 'default';
        watchdog('devconnect_multiorg', 'Invalid requested org “@orgName”; returning default instead', array('@orgName' => $entity->orgName), WATCHDOG_WARNING);
      }
    }
    $config = devconnect_default_org_config($org);
    $config->tags['org'] = $org;
    return $config;
  }

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