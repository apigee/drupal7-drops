<?php

use Apigee\ManagementAPI\APIProduct;
use Drupal\devconnect_developer_apps\ApiProductEntity;

class ApiProductController implements DrupalEntityControllerInterface {
  protected $productCache;

  /**
   * Implements DrupalEntityControllerInterface::__construct().
   *
   * @param $entity_type
   */
  public function __construct($entity_type) {
    $this->productCache = array();
    if (!class_exists('Apigee\ManagementAPI\APIProduct')) {
      module_load_include('module', 'libraries');
      module_load_include('module', 'devconnect');
      devconnect_init();
    }
  }

  protected static function getOrgs($conditions = NULL) {
    return array('default');
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
    static $string_keys = array('description', 'displayName', 'name', 'quota', 'quotaInterval', 'quotaTimeUnit');
    static $array_keys = array('apiResources', 'attributes', 'environments', 'proxies', 'scopes');

    if (!isset($conditions['show_private'])) {
      $conditions['show_private'] = FALSE;
    }

    $list = array();
    foreach (self::getOrgs($conditions) as $org) {
      $api_product = new Apigee\ManagementAPI\APIProduct(devconnect_default_org_config($org));

      if (empty($ids)) {
        try {
          $sub_list = $api_product->listProducts($conditions['show_private']);
          if (!empty($sub_list)) {
            foreach ($list as $p) {
              $this->productCache[$p->getName()] = $p;
            }
          }
          $list += $sub_list;
        } catch (Apigee\Exceptions\ResponseException $e) {
        }
      }
      else {
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
    }

    $return = array();
    foreach ($list as $api_product) {
      $array = $api_product->toArray();
      $array['isPublic'] = $api_product->isPublic();
      $array['orgName'] = $api_product->getConfig()->orgName;

      // Ensure that non-null values exist for non-nullable fields.
      foreach ($string_keys as $string_key) {
        if (!array_key_exists($string_key, $array)) {
          $array[$string_key] = '';
        }
      }
      foreach ($array_keys as $array_key) {
        if (!array_key_exists($array_key, $array)) {
          $array[$array_key] = array();
        }
      }

      $return[$api_product->getName()] = new ApiProductEntity($array);
    }
    return $return;
  }

}
