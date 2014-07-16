<?php

class SmartDocsResourceController extends DrupalDefaultEntityController {

  private $SmartDocsResource;

  public function __construct($entity_type) {
    $config = devconnect_default_api_client();
    $this->SmartDocsResource = new Apigee\DocGen\DocGenResource($config);
  }

  public function resetCache(array $ids = NULL) {

  }

  public function load($ids = array(), $conditions = array('show_private' => FALSE)) {

  }

  public function loadResources($mid, $revision) {
    try {
      $ret = $this->SmartDocsResource->loadResources($mid, $revision);
      return $ret;
    } catch (Exception $e) {
      watchdog(__FUNCTION__, $e->getCode() . ' ' . $e->getMessage(), array(), WATCHDOG_DEBUG);
      return '';
    }
  }

  public function loadResource($mid, $revision, $resource) {
    try {
      $ret = $this->SmartDocsResource->loadResource($mid, $revision, $resource);
      return $ret;
    } catch (Exception $e) {
      watchdog(__FUNCTION__, $e->getCode() . ' ' . $e->getMessage(), array(), WATCHDOG_DEBUG);
      return '';
    }
  }

  public function createResource($mid, $rev, $payload) {
    try {
      $ret = $this->SmartDocsResource->createResource($mid, $rev, $payload);
      return $ret;
    } catch (Exception $e) {
      watchdog(__FUNCTION__, $e->getCode() . ' ' . $e->getMessage(), array(), WATCHDOG_DEBUG);
      return '';
    }
  }

  public function updateResource($mid, $rev, $resource, $payload) {
    try {
      $ret = $this->SmartDocsResource->updateResource($mid, $rev, $resource, $payload);
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