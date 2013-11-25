<?php

interface DocGenResourceControllerInterface
  extends DrupalEntityControllerInterface {
  public function create();
  public function loadResources($mid, $revision);
  public function save($entity);
  public function delete($entity);
}

class DocGenResourceController
  extends DrupalDefaultEntityController
  implements DocGenResourceControllerInterface {

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
      drupal_set_message($e->getCode() . ' ' . $e->getMessage());
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