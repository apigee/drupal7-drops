<?php

interface DocGenRevisionControllerInterface
  extends DrupalEntityControllerInterface {
  public function create();
  public function loadVerbose($apiId, $revId);
  public function save($entity);
  public function delete($entity);
}

class DocGenRevisionController
  extends DrupalDefaultEntityController
  implements DocGenRevisionControllerInterface {

  private $docGenRevision;

  public function __construct($entity_type) {
    $config = devconnect_default_api_client();
    $this->docGenRevision = new Apigee\DocGen\DocGenRevision($config);
  }

  public function resetCache(array $ids = NULL) {

  }

  public function load($ids = array(), $conditions = array('show_private' => FALSE)) {

  }

  public function loadVerbose($apiId, $revId) {
    try {
      $ret = $this->docGenRevision->loadVerbose($apiId, $revId);
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