<?php

class DocGenMethodController extends DrupalDefaultEntityController {

  private $docGenMethod;

  public function __construct($entity_type) {
    $config = devconnect_default_api_client();
    $this->docGenMethod = new Apigee\DocGen\DocGenMethod($config);
  }

  public function resetCache(array $ids = NULL) {

  }

  public function load($ids = array(), $conditions = array('show_private' => FALSE)) {

  }

  public function create() {

  }

  /**
   * Updates a given operation
   */
  public function updateMethod($mid, $element) {
    global $base_url;
    try {
      $full = $element['method_full'];
      if (isset($element['tags']) && isset($full['tags'])) {
        $full['tags'] = $element['tags'];
      }
      if (isset($element['verb']) && isset($full['verb'])) {
        $full['verb'] = $element['verb'];
      }
      if (isset($element['path']) && isset($full['path'])) {
        $full['path'] = $element['path'];
      }
      if (isset($element['auth']) && isset($full['authSchemes'])) {
        $full['authSchemes'] = $element['auth'];
      }
      if ((isset($full['displayName']) || $full['name']) && isset($element['title'])) {
        $full['displayName'] = $element['title'];
      }
      if (isset($full['description']) && isset($element['body'])) {
        $full['description'] = $element['body'];
      }
      if (isset($full['customAttributes']) && $element['devportal']) {
        if ($element['devportal']) {
          $url = parse_url($base_url);
          $full['customAttributes'][] = array('name' => 'drupal_' . $url['host'], 'value' => $url['host']);
        }
      }
      $payload = drupal_json_encode($full);
      $ret = $this->docGenMethod->updateMethod($mid, $element['revision'], $element['rid'], $element['method_id'], $payload);
      return $ret;
    } catch (Exception $e) {
      watchdog(__FUNCTION__, $e->getCode() . ' ' . $e->getMessage(), array(), WATCHDOG_DEBUG);
      return array();
    }
  }

  public function save($entity) {

  }

  public function delete($entity) {

  }

  public function deleteMultiple($entities) {

  }
}