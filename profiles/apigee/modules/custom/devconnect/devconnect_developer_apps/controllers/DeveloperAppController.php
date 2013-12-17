<?php

class DeveloperAppController implements DrupalEntityControllerInterface, EntityAPIControllerInterface {
  private $appCache;

  private static $lastAppId;

  private static $lastException;

  private static $appCount = 0;

  /**
   * Implements DrupalEntityControllerInterface::__construct().
   *
   * @param $entity_type
   */
  public function __construct($entity_type) {
    $this->appCache = array();
  }

  /**
   * Implements DrupalEntityControllerInterface::resetCache().
   *
   * @param array $ids
   */
  public function resetCache(array $ids = NULL) {
    if (is_array($ids) && !empty($this->appCache)) {
      foreach ($ids as $id) {
        if (isset($this->appCache[$id])) {
          unset ($this->appCache[$id]);
        }
      }
    }
    else {
      $this->appCache = array();
    }
  }

  /**
   * Implements EntityAPIControllerInterface::delete().
   *
   * @param array $ids
   */
  public function delete($ids) {
    foreach ($ids as $id) {
      // If entity is in our cache, we can make one fewer server roundtrips.
      if (array_key_exists($id, $this->appCache)) {
        $dev_app = $this->appCache[$id];
        unset ($this->appCache[$id]);
      }
      else {
        // Not in cache. Fetch, then delete.
        $dev_app = new Apigee\ManagementAPI\DeveloperApp(devconnect_default_api_client(), '');
        try {
          $dev_app->loadByAppId($id, TRUE);
        } catch (Apigee\Exceptions\ResponseException $e) {
          $dev_app = NULL;
          self::$lastException = $e;
        }
      }
      if (isset($dev_app)) {
        try {
          $dev_app->delete();
        } catch (Apigee\Exceptions\ResponseException $e) {
          self::$lastException = $e;
        }
      }
    }
  }

  /**
   * Implements EntityAPIControllerInterface::invoke().
   *
   * @param string $hook
   * @param array $entity
   */
  public function invoke($hook, $entity) {
    // TODO
  }

  /**
   * Implements EntityAPIControllerInterface::save().
   *
   * @param $entity
   */
  public function save($entity) {
    $entity = (array)$entity;
    $is_update = !empty($entity['appId']);
    $dev_app = new Apigee\ManagementAPI\DeveloperApp(devconnect_default_api_client(), $entity['developer']);
    $product_cache = NULL;
    if (isset($entity['apiProductCache'])) {
      $product_cache = $entity['apiProductCache'];
      unset ($entity['apiProductCache']);
    }
    $dev_app->fromArray($entity);
    if (!empty($product_cache)) {
      $dev_app->setApiProductCache($product_cache);
    }
    try {
      $dev_app->save($is_update);
      $this->appCache[$dev_app->getAppId()] = $dev_app;
    } catch (Apigee\Exceptions\ResponseException $e) {
      self::$lastException = $e;
      return FALSE;
    }

    self::$lastAppId = $dev_app->getAppId();

    return ($is_update ? SAVED_UPDATED : SAVED_NEW);
  }

  public static function getLastAppId() {
    return self::$lastAppId;
  }

  /**
   * Implements EntityAPIControllerInterface::create().
   *
   * @param array $values
   */
  public function create(array $values = array()) {
    $dev_app = new Apigee\ManagementAPI\DeveloperApp(devconnect_default_api_client(), '');
    $dev_app->fromArray($values);
    return (object)$dev_app->toArray();
  }

  /**
   * Implements EntityAPIControllerInterface::export().
   *
   * @param stdClass $entity
   * @param string $prefix
   */
  public function export($entity, $prefix = '') {
    return json_encode($entity);
  }

  /**
   * Implements EntityAPIControllerInterface::import().
   *
   * @param string $export
   */
  public function import($export) {
    return @json_decode($export);
  }

  /**
   * Implements EntityAPIControllerInterface::buildContent().
   *
   * @param array $entity
   * @param string $view_mode
   * @param string|null $langcode
   * @param boolean $page
   */
  public function buildContent($entity, $view_mode = 'full', $langcode = NULL, $page = FALSE) {
    $callback = 'devconnect_developer_apps_view_' . $view_mode;
    if (function_exists($callback)) {
      return $callback($entity, $page);
    }
    return array();
  }

  /**
   * Implements EntityAPIControllerInterface::view().
   *
   * @param array $entities
   * @param string $view_mode
   * @param string|null $langcode
   * @param boolean $page
   */
  public function view($entities, $view_mode = 'full', $langcode = NULL, $page = FALSE) {
    $output = array();
    foreach ($entities as $id => $entity) {
      $output[$id] = $this->buildContent($entity, $view_mode, $langcode, $page);
    }
    return $output;
  }

  /**
   * Implements DrupalEntityControllerInterface::load().
   *
   * @param array $names
   * @param array $conditions
   * @return array
   */
  public function load($ids = array(), $conditions = array()) {
    $client = devconnect_default_api_client();

    $page = (isset($conditions['page']) ? intval($conditions['page']) : 0);
    $items_per_page = (isset($conditions['items_per_page']) ? intval($conditions['items_per_page']) : 0);
    $sort = (isset($conditions['sort']) ? intval($conditions['sort']) : 'createdAt');

    if (isset($conditions['mail'])) {
      $dev_app = new Apigee\ManagementAPI\DeveloperApp($client, $conditions['mail']);
      if (isset($conditions['name'])) {
        try {
          $dev_app->load($conditions['name']);
          $list = array($dev_app);
        } catch (Apigee\Exceptions\ResponseException $e) {
          $list = array();
          self::$lastException = $e;
        }
      }
      else {
        try {
          $list = $dev_app->getListDetail();
        }
        catch (Apigee\Exceptions\ResponseException $e) {
          $list = array();
          self::$lastException = $e;
        }
      }
      $this->addListToCache($list, $ids);
    }
    // TODO: add more conditions here such as Status
    elseif (empty($ids)) { // Fetch all apps in the org.
      $dev_app = new Apigee\ManagementAPI\DeveloperApp($client, '');
      try {
        $list = $dev_app->listAllOrgApps();
        $this->addListToCache($list, $ids);
      } catch (Apigee\Exceptions\ResponseException $e) {
        self::$lastException = $e;
        $list = array();
      }
    }
    else {
      // We have a list of appIds. Fetch them now.
      $list = array();
      foreach ($ids as $id) {
        if (isset($this->appCache[$id])) {
          $list[$id] = $this->appCache[$id];
        }
      }
      if (count($list) < count($ids)) {
        $remaining_ids = array_diff($ids, array_keys($list));
        $dev_app = new Apigee\ManagementAPI\DeveloperApp($client, '');
        foreach ($remaining_ids as $id) {
          $app = clone($dev_app);
          try {
            $app->loadByAppId($id);
            $list[] = $app;
          } catch (Apigee\Exceptions\ResponseException $e) {
            self::$lastException = $e;
          }
        }
        $this->addListToCache($list, $ids);
      }
      $list = array_values($list);
    }

    $uids = array();
    foreach ($list as $dev_app) {
      $email = $dev_app->getDeveloperMail();
      if (!array_key_exists($email, $uids)) {
        $uids[strtolower($email)] = NULL;
      }
    }

    if (!empty($uids)) {
      $stmt = db_select('users', 'u');
      $stmt->addExpression('LOWER(mail)', 'mail');
      $uids = $stmt->fields('u', array('uid'))
        ->condition('mail', array_keys($uids))
        ->execute()
        ->fetchAllKeyed();
    }
    $uids = array_flip($uids);

    $app_entities = array();
    $sort_by = array();
    $first_iteration = TRUE;
    foreach ($list as $dev_app) {
      $id = $dev_app->getAppId();
      $mail = strtolower($dev_app->getDeveloperMail());
      $array = $dev_app->toArray();
      $array['uid'] = (isset($uids[$mail]) ? $uids[$mail] : NULL);
      // Validate that sort field is an existing key
      if ($first_iteration) {
        if (!array_key_exists($sort, $array)) {
          $sort = 'createdAt';
        }
        $first_iteration = FALSE;
      }
      $app_entities[$id] = $array;
      $sort_by[$id] = $array[$sort];
    }

    self::$appCount = count($app_entities);

    natcasesort($sort_by);
    if ($sort_by == 'createdAt' || $sort_by == 'modifiedAt') {
      $sort_by = array_reverse($sort_by);
    }

    $start = $page * $items_per_page;
    $end = ($items_per_page > 0 ? $start + $items_per_page : NULL);

    $entities_sorted = array();
    $app_index = 0;

    // Sort and page results
    foreach(array_keys($sort_by) as $id) {
      if ($app_index < $start) {
        continue;
      }
      if ($end && $app_index > $end) {
        break;
      }
      $entities_sorted[$id] = (object)$app_entities[$id];
      $app_index++;
    }

    return $entities_sorted;
  }

  private function getKey(Apigee\ManagementAPI\DeveloperApp $app) {
    return $app->getAppId();
  }

  private function addListToCache(&$list, $ids = array()) {
    foreach ($list as $app) {
      $key = $this->getKey($app);
      $this->appCache[$key] = $app;
    }
    if (!empty($ids)) {
      foreach (array_keys($list) as $i) {
        if (!in_array($list[$i]->getAppId(), $ids)) {
          unset($list[$i]);
        }
      }
    }
  }

  public static function getAppCount() {
    return self::$appCount;
  }

  /**
   * Returns the last exception returned from KMS
   *
   * @return Apigee\Exceptions\ResponseException
   */
  public static function getLastException() {
    return self::$lastException;
  }
}

