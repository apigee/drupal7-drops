<?php

class SmartDocsTemplateController extends DrupalDefaultEntityController {

  private $SmartDocsTemplate;

  public function __construct($entity_type) {
    $config = devconnect_default_api_client();
    $this->SmartDocsTemplate = new Apigee\DocGen\DocGenTemplate($config);
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
  public function loadDefaultTemplate($apiId, $type) {
    switch($type) {
      case 'index':
        try {
          $ret = $this->SmartDocsTemplate->getIndexTemplate($apiId, 'default-cms');
          return $ret;
        } catch (Exception $e) {
          watchdog(__FUNCTION__, $e->getCode() . ' ' . $e->getMessage(), array(), WATCHDOG_DEBUG);
          return array();
        }
        break;
      case 'method':
        try {
          $ret = $this->SmartDocsTemplate->getOperationTemplate($apiId, 'default-cms');
          return $ret;
        } catch (Exception $e) {
          watchdog(__FUNCTION__, $e->getCode() . ' ' . $e->getMessage(), array(), WATCHDOG_DEBUG);
          return array();
        }
      default:
        return '';
    }
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
          $ret = $this->SmartDocsTemplate->getIndexTemplate($apiId, 'drupal-cms');
          return $ret;
        } catch (Exception $e) {
          watchdog(__FUNCTION__, $e->getCode() . ' ' . $e->getMessage(), array(), WATCHDOG_DEBUG);
          return array();
        }
        break;
      case 'method':
        try {
          $ret = $this->SmartDocsTemplate->getOperationTemplate($apiId, 'drupal-cms');
          return $ret;
        } catch (Exception $e) {
          watchdog(__FUNCTION__, $e->getCode() . ' ' . $e->getMessage(), array(), WATCHDOG_DEBUG);
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
      $ret = $this->SmartDocsTemplate->saveTemplate($apiId, $type, 'drupal-cms', $html);
      return $ret;
    } catch (Exception $e) {
      watchdog(__FUNCTION__, $e->getCode() . ' ' . $e->getMessage(), array(), WATCHDOG_DEBUG);
      return array();
    }
  }

  /**
   * Saves template back to the Modeling API, if the drupal centric one doesn't exist, create another.
   *
   * @param $apiId
   * @param $type
   */
  public function updateTemplate($apiId, $type, $html) {
    try {
      $ret = $this->SmartDocsTemplate->updateTemplate($apiId, $type, 'drupal-cms', $html);
      return $ret;
    } catch (Exception $e) {
        switch($e->getCode()) {
          case 404:
            try {
              $this->SmartDocsTemplate->saveTemplate($apiId, $type, 'drupal-cms', $html);
            } catch (Exception $e) {
              drupal_set_message($e->getCode() . ' ' . $e->getMessage());
            }
            $ret = $this->SmartDocsTemplate->updateTemplate($apiId, $type, 'drupal-cms', $html);
            return $ret;
            break;
          default:
            watchdog(__FUNCTION__, $e->getCode() . ' ' . $e->getMessage(), array(), WATCHDOG_DEBUG);
        }
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