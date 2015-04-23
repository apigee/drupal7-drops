<?php

namespace Drupal\devconnect_developer_apps;

class DeveloperAppCache implements \DrupalCacheInterface {

  // $cid is orgName:appId
  public function get($cid) {

    list($orgName, $appId) = explode(':', $cid, 2);

    $app_result = db_select('dc_dev_app', 'a')
      ->fields('a', array('app_id', 'uid', 'name', 'display_name', 'description', 'consumer_key', 'consumer_secret', 'overall_status', 'app_status', 'credential_status', 'callback_url', 'created_at', 'modified_at', 'app_family', 'org_name'))
      ->condition('app_id', $appId)
      ->condition('org_name', $orgName)
      ->execute();
    if ($app_result->rowCount() == 0) {
      return FALSE;
    }
    $row = $app_result->fetchAssoc();

    $app = new DeveloperAppEntity();
    $app->appId = $row['app_id'];
    $app->uid = $row['uid'];
    $app->name = $row['name'];
    $app->attributes['DisplayName'] = $row['display_name'];
    $app->description = $row['description'];
    $app->consumerKey = $row['consumer_key'];
    $app->consumerSecret = self::getStatusText($row['consumer_secret']);
    $app->overallStatus = self::getStatusText($row['overall_status']);
    $app->status = self::getStatusText($row['app_status']);
    $app->credentialStatus = self::getStatusText($row['credential_status']);
    $app->callbackUrl = $row['callback_url'];
    $app->createdAt = $row['created_at'];
    $app->modifiedAt = $row['modified_at'];
    $app->accessType = $row['access_type'];
    $app->appFamily = $row['app_family'];
    $app->orgName = $row['org_name'];

    $product_result = db_select('dc_dev_app_api_products', 'p')
      ->fields('p', array('name'))
      ->condition('app_id', $appId)
      ->condition('org_name', $orgName)
      ->execute();
    while ($product = $product_result->fetchCol()) {
      $app->apiProducts[] = $product;
      $app->credentialApiProducts[] = $product;
    }

    $attr_result = db_select('dc_dev_app_attributes', 'a')
      ->fields('a', array('name', 'value'))
      ->condition('app_id', $appId)
      ->execute();
    while ($row = $attr_result->fetchAssoc()) {
      $app->attributes[$row['name']] = $row['value'];
    }

    return $app;
  }

  public function getMultiple(&$cids) {
    $orgs = array();
    foreach ($cids as $cid) {
      list($orgName, $appId) = explode(':', $cid, 2);
      $orgs[$orgName][] = $appId;
    }

    $apps = array();

    foreach ($orgs as $orgName => $appIds) {
      $app_result = db_select('dc_dev_app', 'a')
        ->fields('a', array('app_id', 'uid', 'name', 'display_name', 'description', 'consumer_key', 'consumer_secret', 'overall_status', 'app_status', 'credential_status', 'callback_url', 'created_at', 'modified_at', 'app_family', 'org_name'))
        ->condition('app_id', $appIds)
        ->condition('org_name', $orgName)
        ->execute();
      if ($app_result->rowCount() == 0) {
        continue;
      }

      $found_app_ids = array();

      while ($row = $app_result->fetchAssoc()) {
        $app = new DeveloperAppEntity();
        $app->appId = $row['app_id'];
        $app->uid = $row['uid'];
        $app->name = $row['name'];
        $app->attributes['DisplayName'] = $row['display_name'];
        $app->description = $row['description'];
        $app->consumerKey = $row['consumer_key'];
        $app->consumerSecret = self::getStatusText($row['consumer_secret']);
        $app->overallStatus = self::getStatusText($row['overall_status']);
        $app->status = self::getStatusText($row['app_status']);
        $app->credentialStatus = self::getStatusText($row['credential_status']);
        $app->callbackUrl = $row['callback_url'];
        $app->createdAt = $row['created_at'];
        $app->modifiedAt = $row['modified_at'];
        $app->accessType = $row['access_type'];
        $app->appFamily = $row['app_family'];
        $app->orgName = $row['org_name'];

        $found_app_ids[] = $app->appId;
        $cid = $app->orgName . ':' . $app->appId;
        $apps[$cid] = $app;
      }

      $product_result = db_select('dc_dev_app_api_products', 'p')
        ->fields('p', array('app_id, name'))
        ->condition('app_id', $found_app_ids)
        ->condition('org_name', $orgName)
        ->execute();
      while ($row = $product_result->fetchAssoc()) {
        $cid = $orgName . ':' . $row['app_id'];
        $apps[$cid]->apiProducts[] = $row['name'];
        $apps[$cid]->credentialApiProducts[] = $row['name'];
      }

      $attr_result = db_select('dc_dev_app_attributes', 'a')
        ->fields('a', array('app_id', 'name', 'value'))
        ->condition('app_id', $found_app_ids)
        ->condition('org_name', $orgName)
        ->execute();
      while ($row = $attr_result->fetchAssoc()) {
        $cid = $orgName . ':' . $row['app_id'];
        $apps[$cid]->attributes[$row['name']] = $row['value'];
      }
    }
    if (count($apps) == 0) {
      return FALSE;
    }

    foreach (array_keys($apps) as $cid) {
      $i = array_search($cid, $cids);
      if ($i !== FALSE) {
        unset($cids[$i]);
      }
    }

    return $apps;
  }

  public function set($cid, $entity, $expire = CACHE_PERMANENT) {
    if (!$entity->uid) {
      return;
    }
    switch ($entity->overallStatus) {
      case 'revoked':
        $overall_status = -1;
        break;
      case 'approved':
        $overall_status = 1;
        break;
      default:
        $overall_status = 0;
        break;
    }
    switch ($entity->status) {
      case 'revoked':
        $app_status = -1;
        break;
      case 'approved':
        $app_status = 1;
        break;
      default:
        $app_status = 0;
        break;
    }
    switch ($entity->credentialStatus) {
      case 'revoked':
        $cred_status = -1;
        break;
      case 'approved':
        $cred_status = 1;
        break;
      default:
        $cred_status = 0;
        break;
    }

    $cid = $entity->orgName . ':' . $entity->appId;
    $this->clear($cid);

    // Avoid rare duplicate-key errors
    $cached_app_id = db_select('dc_dev_app', 'd')
      ->fields('d', array('app_id'))
      ->condition('uid', $entity->uid)
      ->condition('org_name', $entity->orgName)
      ->condition('name', $entity->name)
      ->execute()
      ->fetchCol();
    if ($cached_app_id) {
      $this->clear($entity->orgName . ':' . $cached_app_id);
    }

    $fields = array(
      'app_id' => $entity->appId,
      'uid' => intval($entity->uid),
      'name' => $entity->name,
      'display_name' => (array_key_exists('DisplayName', $entity->attributes) ? $entity->attributes['DisplayName'] : ''),
      'description' => (string) $entity->description,
      'consumer_key' => (string) $entity->consumerKey,
      'consumer_secret' => (string) $entity->consumerSecret,
      'overall_status' => $overall_status,
      'app_status' => $app_status,
      'credential_status' => $cred_status,
      'callback_url' => (string) $entity->callbackUrl,
      'created_at' => round($entity->createdAt / 1000),
      'modified_at' => round($entity->modifiedAt / 1000),
      'access_type' => (string) $entity->accessType,
      'app_family' => (string) $entity->appFamily,
      'org_name' => (string) $entity->orgName,
    );
    if (strlen($fields['access_type']) > 5) {
      if ($fields['access_type'] == 'readonly') {
        $fields['access_type'] = 'read';
      }
      else {
        $fields['access_type'] = substr($fields['access_type'], 0, 5);
      }
    }

    db_insert('dc_dev_app')->fields($fields)->execute();
    foreach ($entity->attributes as $name => $value) {
      db_insert('dc_dev_app_attributes')
        ->fields(array(
          'app_id' => $entity->appId,
          'name' => $name,
          'value' => $value,
          'org_name' => $entity->orgName
        ))
        ->execute();
    }
    $products = array();
    foreach ($entity->credentialApiProducts as $product) {
      // Work around rare Edge bug in which an app may have the same apiproduct
      // listed twice for the same credential.
      if (in_array($product['apiproduct'], $products)) {
        continue;
      }
      $products[] = $product['apiproduct'];
      switch ($product['status']) {
        case 'revoked':
          $cred_apiproduct_status = -1;
          break;
        case 'approved':
          $cred_apiproduct_status = 1;
          break;
        default:
          $cred_apiproduct_status = 0;
          break;
      }

      db_insert('dc_dev_app_api_products')
        ->fields(array(
          'app_id' => $entity->appId,
          'org_name' => $entity->orgName,
          'name' => $product['apiproduct'],
          'status' => $cred_apiproduct_status
        ))
        ->execute();
    }

    $old_status = db_select('dc_dev_app_previous_status', 's')->fields('s', array('status'))
      ->condition('app_id', $entity->appId)->execute()->fetchField();
    if ($old_status === FALSE) {
      db_insert('dc_dev_app_previous_status')->fields(array('app_id' => $entity->appId, 'status' => $overall_status, 'org_name' => $entity->orgName))
        ->execute();
    }
    elseif ($old_status != $overall_status) {
      if (module_exists('rules')) {
        $event = NULL;
        switch ($old_status) {
          case -1:
            $event = 'devconnect_developer_app_status_revoked_';
            break;
          case 0:
            $event = 'devconnect_developer_app_status_pending_';
            break;
          case 1:
            $event = 'devconnect_developer_app_status_approved_';
            break;
        }
        switch ($overall_status) {
          case -1:
            $event .= 'revoked';
            break;
          case 0:
            $event .= 'pending';
            break;
          case 1:
            $event .= 'approved';
            break;
        }

        if ($event && strlen($event) > 10) {
          rules_invoke_event($event, $entity);
        }
      }

      db_update('dc_dev_app_previous_status')->fields(array('status' => $overall_status))
        ->condition('app_id', $entity->appId)->execute();
    }
  }

  public function clear($cid = NULL, $wildcard = FALSE) {
    static $tables = array('dc_dev_app', 'dc_dev_app_attributes', 'dc_dev_app_api_products');

    if (empty($cid) || ($wildcard && $cid == '*')) {
      foreach ($tables as $table) {
        db_truncate($table)->execute();
      }
      return;
    }

    $org_op = $app_op = '=';
    if ($wildcard) {
      if (strpos($cid, ':') !== FALSE) {
        list($orgName, $appId) = explode(':', $cid, 2);
      }
      else {
        $orgName = $cid;
        $appId = '*';
      }

      if (strpos($orgName, '*') !== FALSE) {
        $orgName = str_replace('*', '%', $orgName);
        $org_op = 'LIKE';
      }
      if (strpos($appId, '*') !== FALSE) {
        $appId = str_replace('*', '%', $appId);
        $app_op = 'LIKE';
      }
    }
    else {
      list($orgName, $appId) = @explode(':', $cid, 2);
    }
    foreach ($tables as $table) {
      db_delete($table)
        ->condition('app_id', $appId, $app_op)
        ->condition('org_name', $orgName, $org_op)
        ->execute();
    }
  }

  public function isEmpty() {
    $app_id = db_select('dc_dev_app', 'a')
      ->range(0, 1)
      ->fields('a', array('app_id'))
      ->execute()
      ->fetchCol();
    return empty($app_id);
  }

  private static function getStatusText($num) {
    switch ($num) {
      case -1: return 'revoked';
      case 1: return 'approved';
      default: return 'pending';
    }
  }

}