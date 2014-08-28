<?php

class SmartDocsModelController extends DrupalDefaultEntityController {

  private $SmartDocsModel;

  public function __construct($entity_type) {
    $config = devconnect_default_api_client();
    $this->SmartDocsModel = new Apigee\DocGen\DocGenModel($config);
  }

  public function resetCache(array $ids = NULL) {

  }

  public function loadSingle($mid) {
    try {
      $ret = $this->SmartDocsModel->getModel($mid);
      return $ret;
    } catch (Exception $e) {
      watchdog(__FUNCTION__, $e->getCode() . ' ' . $e->getMessage(), array(), WATCHDOG_DEBUG);
      return '';
    }
  }

  public function load($ids = array(), $conditions = array('show_private' => FALSE)) {
    try {
      $ret = $this->SmartDocsModel->getModels();
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
      $ret = $this->SmartDocsModel->createModel($payload);
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
          $ret = $this->SmartDocsModel->importSwagger($entity['apiId'], $entity['url']);
          return $ret;
          break;
        case 'wadl':
          $ret = $this->SmartDocsModel->importWADL($entity['apiId'], $entity['xml']);
          return $ret;
          break;
        case 'apigee_json':
          $ret = $this->SmartDocsModel->importApigeeJSON($entity['apiId'], $entity['json']);
          return $ret;
          break;
        default:
          drupal_set_message('Unsupported format, needs to be either swagger or wadl.', 'error');
      }
      return '';
    } catch (Exception $e) {
      watchdog(__FUNCTION__, $e->getMessage() , array(), WATCHDOG_DEBUG);
      return array('code' => $e->getCode(), 'message' => $e->getMessage());
    }
  }

  public function export($model, $format, $rev) {
    try {
      switch ($format) {
        case 'wadl':
          $ret = $this->SmartDocsModel->exportModel($model, $format, $rev);
          return $ret;
          break;
        default:
          $ret = $this->SmartDocsModel->exportModel($model, '', $rev);
          return $ret;
          break;
      }
      return '';
    } catch (Exception $e) {
      watchdog(__FUNCTION__, $e->getCode() . ' ' . $e->getMessage(), array(), WATCHDOG_DEBUG);
      return array('code' => $e->getCode(), 'message' => $e->getMessage());
    }
  }

  public function delete($entity) {
    try {
      $ret = $this->SmartDocsModel->deleteModel($entity);
      return $ret;
    } catch (Exception $e) {
      watchdog(__FUNCTION__, $e->getCode() . ' ' . $e->getMessage(), array(), WATCHDOG_DEBUG);
      return '';
    }
  }

  public function deleteMultiple($entities) {

  }
}