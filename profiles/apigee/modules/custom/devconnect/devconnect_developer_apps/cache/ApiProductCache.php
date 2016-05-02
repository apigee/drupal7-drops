<?php
/**
 * @file
 * Handles caching for API Products.
 */

namespace Drupal\devconnect_developer_apps;

/**
 * Class ApiProductCache.
 *
 * @package Drupal\devconnect_developer_apps
 */
class ApiProductCache implements \DrupalCacheInterface {

  /**
   * {@inheritdoc}
   *
   * $cid is orgName:appId.
   */
  public function get($cid) {

    list($org_name, $product_name) = explode(':', $cid, 2);

    $prod_result = db_select('dc_api_product', 'p')
      ->fields('p')
      ->condition('name', $product_name)
      ->condition('org_name', $org_name)
      ->execute();
    if ($prod_result->rowCount() == 0) {
      return FALSE;
    }
    $row = $prod_result->fetchAssoc();

    $product = new ApiProductEntity();
    $product->name = $row['name'];
    $product->displayName = $row['display_name'];
    $product->approvalType = ($row['approval_type'] ? 'auto' : 'manual');
    $product->description = $row['description'];
    $product->isPublic = (bool) $row['is_public'];
    $product->orgName = $row['org_name'];
    if (array_key_exists('environments', $row)) {
      $product->environments = explode(',', $row['environments']);
    }
    if (array_key_exists('attributes', $row) && !empty($row['attributes'])) {
      $product->attributes = unserialize($row['attributes']);
    }

    return $product;
  }

  /**
   * {@inheritdoc}
   */
  public function getMultiple(&$cids) {
    $orgs = array();
    foreach ($cids as $cid) {
      list($org_name, $product_name) = explode(':', $cid, 2);
      $orgs[$org_name][] = $product_name;
    }

    $products = array();

    foreach ($orgs as $org_name => $product_names) {
      $prod_result = db_select('dc_api_product', 'p')
        ->fields('p')
        ->condition('name', $product_names)
        ->condition('org_name', $org_name)
        ->execute();

      if ($prod_result->rowCount() == 0) {
        continue;
      }

      $found_prod_names = array();

      while ($row = $prod_result->fetchAssoc()) {
        $product = new ApiProductEntity();
        $product->name = $row['name'];
        $product->displayName = $row['display_name'];
        $product->approvalType = ($row['approval_type'] ? 'auto' : 'manual');
        $product->description = $row['description'];
        $product->isPublic = (bool) $row['is_public'];
        $product->orgName = $row['org_name'];
        if (array_key_exists('environments', $row)) {
          $product->environments = explode(',', $row['environments']);
        }
        if (array_key_exists('attributes', $row) && !empty($row['attributes'])) {
          $product->attributes = unserialize($row['attributes']);
        }

        $found_prod_names[] = $product->name;
        $cid = $product->orgName . ':' . $product->name;
        $products[$cid] = $product;
      }
    }
    if (count($products) == 0) {
      return FALSE;
    }

    foreach (array_keys($products) as $cid) {
      $i = array_search($cid, $cids);
      if ($i !== FALSE) {
        unset($cids[$i]);
      }
    }

    return $products;
  }

  /**
   * {@inheritdoc}
   */
  public function set($cid, $entity, $expire = CACHE_PERMANENT) {
    $cid = $entity->orgName . ':' . $entity->name;
    $this->clear($cid);

    $fields = array(
      'name' => $entity->name,
      'org_name' => $entity->orgName,
      'display_name' => $entity->displayName,
      'approval_type' => ($entity->approvalType == 'auto' ? 0 : 1),
      'description' => $entity->description,
      'is_public' => $entity->isPublic ? 1 : 0,
    );
    if (db_field_exists('dc_api_product', 'environments')) {
      $fields['environments'] = implode(',', $entity->environments);
    }
    if (db_field_exists('dc_api_product', 'attributes')) {
      $fields['attributes'] = serialize($entity->attributes);
    }
    db_insert('dc_api_product')
      ->fields($fields)
      ->execute();
  }

  /**
   * {@inheritdoc}
   */
  public function clear($cid = NULL, $wildcard = FALSE) {
    if (empty($cid) || ($wildcard && $cid == '*')) {
      db_truncate('dc_api_product')->execute();
      return;
    }

    $org_op = $prod_op = '=';
    if ($wildcard) {
      if (strpos($cid, ':') !== FALSE) {
        list($org_name, $product_name) = explode(':', $cid, 2);
      }
      else {
        $org_name = $cid;
        $product_name = '*';
      }

      if (strpos($org_name, '*') !== FALSE) {
        $org_name = str_replace('*', '%', $org_name);
        $org_op = 'LIKE';
      }
      if (strpos($product_name, '*') !== FALSE) {
        $product_name = str_replace('*', '%', $product_name);
        $prod_op = 'LIKE';
      }
    }
    else {
      list($org_name, $product_name) = @explode(':', $cid, 2);
    }
    db_delete('dc_api_product')
      ->condition('org_name', $org_name, $org_op)
      ->condition('name', $product_name, $prod_op)
      ->execute();
  }

  /**
   * {@inheritdoc}
   */
  public function isEmpty() {
    $app_id = db_select('dc_api_product', 'a')
      ->range(0, 1)
      ->fields('a', array('name'))
      ->execute()
      ->fetchCol();
    return empty($app_id);
  }

}
