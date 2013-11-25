<?php

interface DocGenDocControllerInterface
  extends DrupalEntityControllerInterface {
  public function load($ids = array(), $conditions = array('show_private' => FALSE));
  public function create($entity);
  public function save($entity);
  public function delete($entity);
}

class DocGenDocController
  extends DrupalDefaultEntityController
  implements DocGenDocControllerInterface {

  private $docGenDoc;

  public function __construct($entity_type) {
    $config = devconnect_default_api_client();
    $this->docGenDoc = new Apigee\DocGen\DocGenDoc($config);
  }

  public function resetCache(array $ids = NULL) {

  }

  public function load($ids = array(), $conditions = array('show_private' => FALSE)) {

  }

  public function loadOperation($data, $mid) {
    $value = $_GET['cache'];
    $use_cache = (isset($value) && is_numeric($value) && (strlen($value) == 1)) ? (bool)$_GET['cache'] : TRUE;
    if (!$use_cache) {
      try {
        $ret = $this->docGenDoc->requestOperation($data, $mid);
        cache_set('html_' . $mid . '_' . $data['nid'], $ret, 'cache_docgen', CACHE_PERMANENT);
        return $ret;
      } catch (Exception $e) {
        drupal_set_message($e->getCode() . ' ' . $e->getMessage(), 'error');
        return '';
      }
    }
    $my_data = &drupal_static(__FUNCTION__);
    if (!isset($my_data)) {
      if ($cache = cache_get('html_' . $mid . '_' . $data['nid'], 'cache_docgen')) {
        $my_data = $cache->data;
      } else {
        try {
          $ret = $this->docGenDoc->requestOperation($data, $mid);
          return $ret;
        } catch (Exception $e) {
          drupal_set_message($e->getCode() . ' ' . $e->getMessage(), 'error');
          return '';
        }
      }
    }
    return $my_data;
  }

  public function create($entity) {

  }

  public function save($entity) {

  }

  public function delete($entity) {

  }

  public function delete_multiple($entities) {

  }
}