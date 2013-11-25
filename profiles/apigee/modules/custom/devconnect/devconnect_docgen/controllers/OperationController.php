<?php

interface DocGenOperationControllerInterface
  extends DrupalEntityControllerInterface {
  public function create();
  public function save($entity);
  public function delete($entity);
}

class DocGenOperationController
  extends DrupalDefaultEntityController
  implements DocGenOperationControllerInterface {

  public function __construct($entity_type) {

  }

  public function resetCache(array $ids = NULL) {

  }

  public function load($ids = array(), $conditions = array('show_private' => FALSE)) {

  }

  public function create() {

  }

  public function save($entity) {

  }

  public function delete($entity) {

  }

  public function deleteMultiple($entities) {

  }
}