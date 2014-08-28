<?php

class SmartDocsRevisionController extends DrupalDefaultEntityController {

  private $SmartDocsRevision;

  public function __construct($entity_type) {
    $config = devconnect_default_api_client();
    $this->SmartDocsRevision = new Apigee\DocGen\DocGenRevision($config);
  }

  public function resetCache(array $ids = NULL) {

  }

  public function load($ids = array(), $conditions = array('show_private' => FALSE)) {

  }

  public function getAllRevisions($apiId) {
    try {
      $ret = $this->SmartDocsRevision->getAllRevisions($apiId);
      return $ret;
    } catch (Exception $e) {
      watchdog(__FUNCTION__, $e->getCode() . ' ' . $e->getMessage(), array(), WATCHDOG_DEBUG);
      return '';
    }
  }

  public function getRevision($apiId, $revId) {
    try {
      $ret = $this->SmartDocsRevision->getRevision($apiId, $revId);
      return $ret;
    } catch (Exception $e) {
      watchdog(__FUNCTION__, $e->getCode() . ' ' . $e->getMessage(), array(), WATCHDOG_DEBUG);
      return '';
    }
  }


  public function addAuth($apiId, $rev, $auth) {
    try {
      $auth = drupal_json_encode($auth);
      $ret = $this->SmartDocsRevision->addAuth($apiId, $rev, $auth);
      return $ret;
    } catch (Exception $e) {
      watchdog(__FUNCTION__, $e->getCode() . ' ' . $e->getMessage(), array(), WATCHDOG_DEBUG);
      return '';
    }
  }

  public function updateAuth($apiId, $rev, $auth) {
    try {
      $auth = drupal_json_encode($auth);
      $ret = $this->SmartDocsRevision->updateAuth($apiId, $rev, $auth);
      return $ret;
    } catch (Exception $e) {
      watchdog(__FUNCTION__, $e->getCode() . ' ' . $e->getMessage(), array(), WATCHDOG_DEBUG);
      return '';
    }
  }

  public function getOAuthCredentials($apiId, $rev) {
    try {
      $ret = $this->SmartDocsRevision->getOAuthCredentials($apiId, $rev);
      if (is_numeric($ret['code']) && (floor($ret['code'] / 100) == 2)) {
        return $ret['data'];
      }
      return array();
    } catch (Exception $e) {
      if ($e->getCode() == 404) {
        return '';
      } else {
        watchdog(__FUNCTION__, $e->getCode() . ' ' . $e->getMessage(), array(), WATCHDOG_DEBUG);
        return '';
      }
    }
  }

  public function getTokenCredentials($apiId, $rev) {
    try {
      $ret = $this->SmartDocsRevision->getTokenCredentials($apiId, $rev);
      if (is_numeric($ret['code']) && (floor($ret['code'] / 100) == 2)) {
        return $ret['data'];
      }
      return array();
    } catch (Exception $e) {
      if ($e->getCode() == 404) {
        return '';
      } else {
        watchdog(__FUNCTION__, $e->getCode() . ' ' . $e->getMessage(), array(), WATCHDOG_DEBUG);
        return '';
      }
    }
  }

  public function loadVerbose($apiId, $revId) {
    try {
      $ret = $this->SmartDocsRevision->loadVerbose($apiId, $revId);
      return $ret;
    } catch (Exception $e) {
      watchdog(__FUNCTION__, $e->getCode() . ' ' . $e->getMessage(), array(), WATCHDOG_DEBUG);
      return '';
    }
  }

  public function newRevision($apiId, $payload) {
    try {
      $ret = $this->SmartDocsRevision->newRevision($apiId, $payload);
      return $ret;
    } catch (Exception $e) {
      watchdog(__FUNCTION__, $e->getCode() . ' ' . $e->getMessage(), array(), WATCHDOG_DEBUG);
      return '';
    }
  }

  public function updateRevision($apiId, $revId, $payload) {
    try {
      $ret = $this->SmartDocsRevision->updateRevision($apiId, $revId, $payload);
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