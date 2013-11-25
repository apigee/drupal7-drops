<?php

interface DocGenTemplateControllerInterface
  extends DrupalEntityControllerInterface {
  public function load($ids = array(), $conditions = array('show_private' => FALSE));
  public function loadTemplate($apiId, $type);
  public function saveTemplate($apiId, $type, $html);
  public function create($entity);
  public function save($entity);
  public function delete($entity);
}

class DocGenTemplateController
  extends DrupalDefaultEntityController
  implements DocGenTemplateControllerInterface {

  private $docGenTemplate;

  public function __construct($entity_type) {
    $config = devconnect_default_api_client();
    $this->docGenTemplate = new Apigee\DocGen\DocGenTemplate($config);
  }

  public function load($ids = array(), $conditions = array('show_private' => FALSE)) {

  }

  /**
   * Loads template from the modeling API of a given type
   *
   * @param $apiId
   * @param $type
   * @return array|string
   */
  public function loadTemplate($apiId, $type) {
    switch($type) {
      case 'index':
        try {
          $ret = $this->docGenTemplate->getIndexTemplate($apiId);
          return $ret;
        } catch (Exception $e) {
          drupal_set_message($e->getCode() . ' ' . $e->getMessage());
          return array();
        }
        break;
      case 'method':
        try {
          $ret = $this->docGenTemplate->getOperationTemplate($apiId);
          return $ret;
        } catch (Exception $e) {
          drupal_set_message($e->getCode() . ' ' . $e->getMessage());
          return array();
        }
      default:
        return '';
    }
  }

  /**
   * Saves template back to the Modeling API
   *
   * @param $apiId
   * @param $type
   */
  public function saveTemplate($apiId, $type, $html) {
    try {
      $ret = $this->docGenTemplate->saveTemplate($apiId, $type, $html);
      return $ret;
    } catch (Exception $e) {
      drupal_set_message($e->getCode() . ' ' . $e->getMessage());
      return array();
    }
  }

  public function create($entity) {

  }

  public function save($entity) {

  }

  public function delete($entity) {

  }

}