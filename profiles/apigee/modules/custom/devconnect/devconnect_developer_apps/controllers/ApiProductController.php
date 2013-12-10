<?php

class ApiProductController implements DrupalEntityControllerInterface {
  private $productCache;

  /**
   * Implements DrupalEntityControllerInterface::__construct().
   *
   * @param $entity_type
   */
  public function __construct($entity_type) {
    $this->productCache = array();
  }

  /**
   * Implements DrupalEntityControllerInterface::resetCache().
   *
   * @param array $ids
   */
  public function resetCache(array $ids = NULL) {
    if (is_array($ids) && !empty($this->productCache)) {
      foreach ($ids as $id) {
        if (isset($this->productCache[$id])) {
          unset ($this->productCache[$id]);
        }
      }
    }
    else {
      $this->productCache = array();
    }
  }

  /**
   * Implements DrupalEntityControllerInterface::load().
   *
   * @param array $names
   * @param array $conditions
   *
   * @return array
   */
  public function load($ids = array(), $conditions = array('show_private' => FALSE)) {
    $api_product = new Apigee\ManagementAPI\APIProduct(devconnect_default_api_client());
    if (!isset($conditions['show_private'])) {
      $conditions['show_private'] = FALSE;
    }

    if (empty($ids)) {
      try {
        $list = $api_product->listProducts($conditions['show_private']);
        if (!empty($list)) {
          foreach ($list as $p) {
            $this->productCache[$p->getName()] = $p;
          }
        }
      } catch (Apigee\Exceptions\ResponseException $e) {
        return array();
      }
    }
    else {
      $list = array();
      foreach ($ids as $name) {
        if (array_key_exists($name, $this->productCache)) {
          $list[] = $this->productCache[$name];
        }
        else {
          $my_product = clone $api_product;
          try {
            $my_product->load($name);
            $this->productCache[$my_product->getName()] = $my_product;
            if ($my_product->isPublic() || $conditions['show_private']) {
              $list[] = $my_product;
            }
          } catch (Apigee\Exceptions\ResponseException $e) {
            // do nothing
          }
        }
      }
    }

    $return = array();
    foreach ($list as $api_product) {
      if ($api_product->isPublic() || $conditions['show_private']) {
        $return[$api_product->getName()] = $api_product->toArray();
      }
    }
    return $return;
  }

}
