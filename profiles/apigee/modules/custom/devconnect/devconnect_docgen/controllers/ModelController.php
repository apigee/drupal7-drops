<?php

class DocGenModelController extends DrupalDefaultEntityController {

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
      watchdog(__FUNCTION__, $e->getCode() . ' ' . $e->getMessage(), array(), WATCHDOG_DEBUG);
      return '';
    }
  }

  public function load($ids = array(), $conditions = array('show_private' => FALSE)) {
    try {
      $ret = $this->docGenModel->getModels();
      return $ret;
    } catch (Exception $e) {
      watchdog(__FUNCTION__, $e->getCode() . ' ' . $e->getMessage(), array(), WATCHDOG_DEBUG);
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
      watchdog(__FUNCTION__, $e->getCode() . ' ' . $e->getMessage(), array(), WATCHDOG_DEBUG);
      return '';
    }
  }

  public function save($entity) {

  }

  public function import($entity, $format) {
    try {
      switch ($format) {
        case 'swagger':
          $ret = $this->docGenModel->importSwagger($entity['apiId'], $entity['url']);
          return $ret;
          break;
        case 'wadl':
          $ret = $this->docGenModel->importWADL($entity['apiId'], $entity['xml']);
          return $ret;
          break;
        case 'apigee_json':
          $ret = $this->docGenModel->importApigeeJSON($entity['apiId'], $entity['json']);
          return $ret;
          break;
        default:
          drupal_set_message('Unsupported format, needs to be either swagger or wadl.', 'error');
      }
      return '';
    } catch (Exception $e) {
      watchdog(__FUNCTION__, $e->getCode() . ' ' . $e->getMessage(), array(), WATCHDOG_DEBUG);
      return array('code' => $e->getCode(), 'message' => $e->getMessage());
    }
  }

  public function export($model, $format) {
    try {
      $ret = $this->docGenModel->exportModel($model, $format);
      return $ret;
    } catch (Exception $e) {
      watchdog(__FUNCTION__, $e->getCode() . ' ' . $e->getMessage(), array(), WATCHDOG_DEBUG);
      return array('code' => $e->getCode(), 'message' => $e->getMessage());
    }
  }

  public function delete($entity) {
    try {
      $ret = $this->docGenModel->deleteModel($entity);
      return $ret;
    } catch (Exception $e) {
      watchdog(__FUNCTION__, $e->getCode() . ' ' . $e->getMessage(), array(), WATCHDOG_DEBUG);
      return '';
    }
  }

  public function deleteMultiple($entities) {

  }
}