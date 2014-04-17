<?php

class DocGenRevisionController extends DrupalDefaultEntityController {

  private $docGenRevision;

  public function __construct($entity_type) {
    $config = devconnect_default_api_client();
    $this->docGenRevision = new Apigee\DocGen\DocGenRevision($config);
  }

  public function resetCache(array $ids = NULL) {

  }

  public function load($ids = array(), $conditions = array('show_private' => FALSE)) {

  }

  public function getAllRevisions($apiId) {
    try {
      $ret = $this->docGenRevision->getAllRevisions($apiId);
      return $ret;
    } catch (Exception $e) {
      watchdog(__FUNCTION__, $e->getCode() . ' ' . $e->getMessage(), array(), WATCHDOG_DEBUG);
      return '';
    }
  }

  public function oAuthEnable($apiId, $rev, $auth) {
    try {
      $auth = drupal_json_encode($auth);
      $ret = $this->docGenRevision->oAuthEnable($apiId, $rev, $auth);
      return $ret;
    } catch (Exception $e) {
      watchdog(__FUNCTION__, $e->getCode() . ' ' . $e->getMessage(), array(), WATCHDOG_DEBUG);
      return '';
    }
  }

  public function loadVerbose($apiId, $revId) {
    try {
      $ret = $this->docGenRevision->loadVerbose($apiId, $revId);
      return $ret;
    } catch (Exception $e) {
      watchdog(__FUNCTION__, $e->getCode() . ' ' . $e->getMessage(), array(), WATCHDOG_DEBUG);
      return '';
    }
  }

  public function create() {

  }

  public function save($entity) {

  }

  public function delete($entity) {

  }

  public function delete_multiple($entities) {

  }
}