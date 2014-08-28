<?php

class SmartDocsMethodController extends DrupalDefaultEntityController {

  private $SmartDocsMethod;

  public function __construct($entity_type) {
    $config = devconnect_default_api_client();
    $this->SmartDocsMethod = new Apigee\DocGen\DocGenMethod($config);
  }

  public function resetCache(array $ids = NULL) {

  }

  public function load($ids = array(), $conditions = array('show_private' => FALSE)) {

  }

  public function create() {

  }

  /**
   * Updates a given operation
   */
  public function updateMethod($mid, $rev, $res, $method_id, $payload) {
    try {
      $ret = $this->SmartDocsMethod->updateMethod($mid, $rev, $res, $method_id, $payload);
      return $ret;
    } catch (Exception $e) {
      watchdog(__FUNCTION__, $e->getCode() . ' ' . $e->getMessage(), array(), WATCHDOG_DEBUG);
      return array();
    }
  }

  public function createMethod($apiId, $revisionId, $resourceId, $payload) {
    try {
      $ret = $this->SmartDocsMethod->createMethod($apiId, $revisionId, $resourceId, $payload);
      return $ret;
    } catch (Exception $e) {
      watchdog(__FUNCTION__, $e->getCode() . ' ' . $e->getMessage(), array(), WATCHDOG_DEBUG);
      return $e->getMessage();
    }
  }

  public function getMethod($apiId, $revisionId, $resourceId, $methodId) {
    try {
      $ret = $this->SmartDocsMethod->getMethod($apiId, $revisionId, $resourceId, $methodId);
      return $ret;
    } catch (Exception $e) {
      watchdog(__FUNCTION__, $e->getCode() . ' ' . $e->getMessage(), array(), WATCHDOG_DEBUG);
      return '';
    }
  }

  public function save($entity) {

  }

  public function delete($entity) {

  }

  public function deleteMultiple($entities) {

  }
}