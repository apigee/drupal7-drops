<?php

interface DocGenModelControllerInterface
  extends DrupalEntityControllerInterface {
  public function create($entity);
  public function loadSingle($mid);
  public function import($entity);
  public function save($entity);
  public function delete($entity);
}

class DocGenModelController
  extends DrupalDefaultEntityController
  implements DocGenModelControllerInterface {

  private $docGenModel;

  public function __construct($entity_type) {
    $config = devconnect_default_api_client();
    $this->docGenModel = new Apigee\DocGen\DocGenModel($config);
  }

  public function resetCache(array $ids = NULL) {

  }

  public function loadSingle($mid) {
    try {
      $ret = $this->docGenModel->getModel($mid);
      return $ret;
    } catch (Exception $e) {
      drupal_set_message($e->getCode() . ' ' . $e->getMessage());
      return '';
    }
  }

  public function load($ids = array(), $conditions = array('show_private' => FALSE)) {
    try {
      $ret = $this->docGenModel->getModels();
      return $ret;
    } catch (Exception $e) {
      drupal_set_message($e->getCode() . ' ' . $e->getMessage());
      return '';
    }
  }

  public function create($entity) {
    $payload = array(
      'name' => $entity['model_name'],
      'displayName' => $entity['display_name'],
      'description' => $entity['model_description']
    );
    try {
      $ret = $this->docGenModel->createModel($payload);
      return $ret;
    } catch (Exception $e) {
      drupal_set_message($e->getCode() . ' ' . $e->getMessage());
      return '';
    }
  }

  public function save($entity) {

  }

  public function import($entity) {
    try {
      $ret = $this->docGenModel->importWADL($entity['apiId'], $entity['xml']);
      return $ret;
    } catch (Exception $e) {
      drupal_set_message($e->getCode() . ' ' . $e->getMessage());
      return '';
    }
  }

  public function delete($entity) {
    try {
      $ret = $this->docGenModel->deleteModel($entity);
      return $ret;
    } catch (Exception $e) {
      drupal_set_message($e->getCode() . ' ' . $e->getMessage());
      return '';
    }
  }

  public function deleteMultiple($entities) {

  }
}