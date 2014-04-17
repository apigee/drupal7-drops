<?php

class DocGenResourceController extends DrupalDefaultEntityController {

  private $docGenResource;

  public function __construct($entity_type) {
    $config = devconnect_default_api_client();
    $this->docGenResource = new Apigee\DocGen\DocGenResource($config);
  }

  public function resetCache(array $ids = NULL) {

  }

  public function load($ids = array(), $conditions = array('show_private' => FALSE)) {

  }

  public function loadResources($mid, $revision) {
    try {
      $ret = $this->docGenResource->loadResources($mid, $revision);
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