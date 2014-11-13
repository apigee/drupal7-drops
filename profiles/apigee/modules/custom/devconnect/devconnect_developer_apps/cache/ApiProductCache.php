<?php

namespace Drupal\devconnect_developer_apps;

class ApiProductCache implements \DrupalCacheInterface {

  // $cid is orgName:appId
  public function get($cid) {

    list($orgName, $productName) = explode(':', $cid, 2);

    $prod_result = db_select('dc_api_product', 'p')
      ->fields('p', array('name', 'display_name', 'approval_type', 'description', 'is_public', 'org_name'))
      ->condition('name', $productName)
      ->condition('org_name', $orgName)
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
    $product->isPublic = (bool)$row['is_public'];
    $product->orgName = $row['org_name'];

    return $product;
  }

  public function getMultiple(&$cids) {
    $orgs = array();
    foreach ($cids as $cid) {
      list($orgName, $productName) = explode(':', $cid, 2);
      $orgs[$orgName][] = $productName;
    }

    $products = array();

    foreach ($orgs as $orgName => $productNames) {
      $prod_result = db_select('dc_api_product', 'p')
        ->fields('p', array('name', 'display_name', 'approval_type', 'description', 'is_public', 'org_name'))
        ->condition('name', $productNames)
        ->condition('org_name', $orgName)
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
        $product->isPublic = (bool)$row['is_public'];
        $product->orgName = $row['org_name'];

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

  public function set($cid, $entity, $expire = CACHE_PERMANENT) {
    $cid = $entity->orgName . ':' . $entity->name;
    $this->clear($cid);
    db_insert('dc_api_product')
      ->fields(array(
        'name' => $entity->name,
        'org_name' => $entity->orgName,
        'display_name' => $entity->displayName,
        'approval_type' => ($entity->approvalType == 'auto' ? 0 : 1),
        'description' => $entity->description,
        'is_public' => $entity->isPublic ? 1 : 0
      ))
      ->execute();
  }

  public function clear($cid = NULL, $wildcard = FALSE) {
    if (empty($cid) || ($wildcard && $cid == '*')) {
      db_truncate('dc_api_product')->execute();
      return;
    }

    $org_op = $prod_op = '=';
    if ($wildcard) {
      if (strpos($cid, ':') !== FALSE) {
        list($orgName, $productName) = explode(':', $cid, 2);
      }
      else {
        $orgName = $cid;
        $productName = '*';
      }

      if (strpos($orgName, '*') !== FALSE) {
        $orgName = str_replace('*', '%', $orgName);
        $org_op = 'LIKE';
      }
      if (strpos($productName, '*') !== FALSE) {
        $productName = str_replace('*', '%', $productName);
        $prod_op = 'LIKE';
      }
    }
    else {
      list($orgName, $productName) = @explode(':', $cid, 2);
    }
    db_delete('dc_api_product')
      ->condition('org_name', $orgName, $org_op)
      ->condition('name', $productName, $prod_op)
      ->execute();
  }

  public function isEmpty() {
    $app_id = db_select('dc_api_product', 'a')
      ->range(0, 1)
      ->fields('a', array('name'))
      ->execute()
      ->fetchCol();
    return empty($app_id);
  }
}