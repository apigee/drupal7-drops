<?php

class SmartDocsDocController extends DrupalDefaultEntityController {

  private $SmartDocsDoc;

  public function __construct($entity_type) {
    $config = devconnect_default_api_client();
    $this->SmartDocsDoc = new Apigee\DocGen\DocGenDoc($config);
  }

  public function resetCache(array $ids = NULL) {

  }

  public function load($ids = array(), $conditions = array('show_private' => FALSE)) {

  }

  public function loadOperation($data, $mid, $name,  $drupal_update = NULL) {
    $flush = (isset($_GET['flush']) && is_numeric($_GET['flush']) && (strlen($_GET['flush']) == 1)) ? $_GET['flush'] : 0;
    $drupal_edit = (isset($_GET['drupal_edit']) && is_numeric($_GET['drupal_edit']) && (strlen($_GET['drupal_edit']) == 1)) ? $_GET['drupal_edit'] : 0;
    if ((bool)$flush) {
      try {
        // Flag Drupal node as synced
        if ((bool)$drupal_edit) {
          db_update('smartdocs')
            ->fields(array('synced' => 1))
            ->condition('nid', $data['nid'])
            ->execute();
        }
        // Flag Drupal node as unsynced
        else {
          db_update('smartdocs')
            ->fields(array('synced' => 0))
            ->condition('nid', $data['nid'])
            ->execute();
        }

        $ret = $this->SmartDocsDoc->requestOperation($data, $mid, $name);
        cache_set($data['nid'], $ret, 'cache_docgen', CACHE_PERMANENT);
        return $ret;
      } catch (Exception $e) {
        watchdog(__FUNCTION__, $e->getCode() . ' ' . $e->getMessage(), array(), WATCHDOG_DEBUG);
        return '';
      }
    } elseif ((bool)$drupal_update) {
      db_update('smartdocs')
        ->fields(array('synced' => 1))
        ->condition('nid', $data['nid'])
        ->execute();

      // retrieve raw json
      $ret = $this->SmartDocsDoc->requestOperation($data, $mid, '');
      return $ret;
    } else {
      $my_data = &drupal_static(__FUNCTION__);
      if (!isset($my_data)) {
        if ($cache = cache_get($data['nid'], 'cache_docgen')) {
          $my_data = $cache->data;
        } else {
          try {
            $ret = $this->SmartDocsDoc->requestOperation($data, $mid, $name);
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