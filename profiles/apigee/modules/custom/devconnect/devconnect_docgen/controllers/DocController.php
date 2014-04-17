<?php

class DocGenDocController extends DrupalDefaultEntityController {

  private $docGenDoc;

  public function __construct($entity_type) {
    $config = devconnect_default_api_client();
    $this->docGenDoc = new Apigee\DocGen\DocGenDoc($config);
  }

  public function resetCache(array $ids = NULL) {

  }

  public function load($ids = array(), $conditions = array('show_private' => FALSE)) {

  }

  public function loadOperation($data, $mid, $name) {
    $flush = (isset($_GET['flush']) && is_numeric($_GET['flush']) && (strlen($_GET['flush']) == 1)) ? $_GET['flush'] : 0;
    if ((bool)$flush) {
      try {
        $ret = $this->docGenDoc->requestOperation($data, $mid, $name);
        cache_set($data['nid'], $ret, 'cache_docgen', CACHE_PERMANENT);
        return $ret;
      } catch (Exception $e) {
        watchdog(__FUNCTION__, $e->getCode() . ' ' . $e->getMessage(), array(), WATCHDOG_DEBUG);
        return '';
      }
    } else {
      $my_data = &drupal_static(__FUNCTION__);
      if (!isset($my_data)) {
        if ($cache = cache_get($data['nid'], 'cache_docgen')) {
          $my_data = $cache->data;
        } else {
          try {
            $ret = $this->docGenDoc->requestOperation($data, $mid, $name);
            cache_set($data['nid'], $ret, 'cache_docgen', CACHE_PERMANENT);
            return $ret;
          } catch (Exception $e) {
            watchdog(__FUNCTION__, $e->getCode() . ' ' . $e->getMessage(), array(), WATCHDOG_DEBUG);
            return '';
          }
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