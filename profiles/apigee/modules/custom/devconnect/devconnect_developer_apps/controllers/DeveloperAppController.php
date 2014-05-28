<?php

class DeveloperAppController implements DrupalEntityControllerInterface, EntityAPIControllerInterface {
  /**
   * @var array
   */
  private $appCache;

  /**
   * @var \Drupal\devconnect_developer_apps\DeveloperAppEntity
   */
  private static $lastApp;

  /**
   * @var \Exception
   */
  private static $lastException;

  /**
   * @var \Apigee\Util\OrgConfig
   */
  private $orgConfig;

  /**
   * Implements DrupalEntityControllerInterface::__construct().
   *
   * @param $entity_type
   */
  public function __construct($entity_type) {
    $this->appCache = array();
    if (!class_exists('Apigee\ManagementAPI\DeveloperApp')) {
      module_load_include('module', 'libraries');
      module_load_include('module', 'devconnect');
      devconnect_init();
    }

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
        if (!($this->orgConfig instanceof \Apigee\Util\OrgConfig)) {
          $this->orgConfig = devconnect_default_api_client();
        }

        $dev_app = new Apigee\ManagementAPI\DeveloperApp($this->orgConfig, '');
        try {
          $dev_app->loadByAppId($id, TRUE);
        } catch (Apigee\Exceptions\ResponseException $e) {
          $dev_app = NULL;
          self::$lastException = $e;
        } catch (Apigee\Exceptions\ParameterException $e) {
          $dev_app = NULL;
          self::$lastException = $e;
        }
      }
      if (isset($dev_app)) {
        try {
          $entity = new Drupal\devconnect_developer_apps\DeveloperAppEntity($dev_app->toArray());
          $dev_app->delete();
          devconnect_developer_apps_delete_from_cache($entity);
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
   * @param Drupal\devconnect_developer_apps\DeveloperAppEntity $entity
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
    // Make a copy so we can remove irrelevant members
    $entity = (array) $entity;
    $is_update = !empty($entity['appId']);
    if (!($this->orgConfig instanceof \Apigee\Util\OrgConfig)) {
      $this->orgConfig = devconnect_default_api_client();
    }
    $dev_app = new Apigee\ManagementAPI\DeveloperApp($this->orgConfig, $entity['developer']);
    $product_cache = $entity['apiProductCache'];
    unset ($entity['apiProductCache']);
    $dev_app->fromArray($entity);
    $dev_app->setApiProductCache($product_cache);
    try {
      $dev_app->save($is_update);
      $this->appCache[$dev_app->getAppId()] = $dev_app;
    } catch (Apigee\Exceptions\ResponseException $e) {
      self::$lastException = $e;
      return FALSE;
    }

    $dev_app_array = $dev_app->toArray();
    // Copy incoming UID to outgoing UID
    $dev_app_array['uid'] = $entity['uid'];
    $last_app = new \Drupal\devconnect_developer_apps\DeveloperAppEntity($dev_app_array);

    devconnect_developer_apps_write_to_cache($last_app);

    self::$lastApp = $last_app;

    return ($is_update ? SAVED_UPDATED : SAVED_NEW);
  }

  /**
   * Fetches appId from last created app.
   *
   * @static
   * @return string
   */
  public static function getLastAppId() {
    return self::$lastApp->appId;
  }

  /**
   * Fetches last created app entity.
   *
   * @static
   * @return \Drupal\devconnect_developer_apps\DeveloperAppEntity
   */
  public static function getLastApp() {
    return self::$lastApp;
  }

  /**
   * Implements EntityAPIControllerInterface::create().
   *
   * Creates an empty developer_app entity, but does not save it.
   *
   * @param array $values
   * @return Drupal\devconnect_developer_apps\DeveloperAppEntity
   */
  public function create(array $values = array()) {
    if (!($this->orgConfig instanceof \Apigee\Util\OrgConfig)) {
      $this->orgConfig = devconnect_default_api_client();
    }
    $dev_app = new Apigee\ManagementAPI\DeveloperApp($this->orgConfig, '');
    $dev_app->fromArray($values);
    return new Drupal\devconnect_developer_apps\DeveloperAppEntity($dev_app->toArray());
  }

  /**
   * Implements EntityAPIControllerInterface::export().
   *
   * @param stdClass $entity
   * @param string $prefix
   * @return string
   */
  public function export($entity, $prefix = '') {
    return json_encode($entity);
  }

  /**
   * Implements EntityAPIControllerInterface::import().
   *
   * @param string $export
   * @return mixed
   */
  public function import($export) {
    return @json_decode($export, TRUE);
  }

  /**
   * Implements EntityAPIControllerInterface::buildContent().
   *
   * @param array $entity
   * @param string $view_mode
   * @param string|null $langcode
   * @param boolean $page
   * @return array
   */
  public function buildContent($entity, $view_mode = 'full', $langcode = NULL, $page = FALSE) {
    $callback = 'devconnect_developer_apps_view_' . $view_mode;
    if (function_exists($callback)) {
      return $callback($entity, $page, $langcode);
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
   * @return array
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
    if (!($this->orgConfig instanceof \Apigee\Util\OrgConfig)) {
      $this->orgConfig = devconnect_default_api_client();
    }

    if (isset($conditions['mail'])) {
      $dev_app = new Apigee\ManagementAPI\DeveloperApp($this->orgConfig, $conditions['mail']);
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
        } catch (Apigee\Exceptions\ResponseException $e) {
          $list = array();
          self::$lastException = $e;
        }
      }
      $this->addListToCache($list, $ids);
    }
    // TODO: add more conditions here such as Status
    elseif (empty($ids)) { // Fetch all apps in the org.
      $dev_app = new Apigee\ManagementAPI\DeveloperApp($this->orgConfig, '');
      try {
        $list = $dev_app->listAllApps();
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
        $dev_app = new Apigee\ManagementAPI\DeveloperApp($this->orgConfig, '');
        foreach ($remaining_ids as $id) {
          $app = clone($dev_app);
          try {
            $app->loadByAppId($id);
            $list[] = $app;
          } catch (Apigee\Exceptions\ResponseException $e) {
            self::$lastException = $e;
          } catch (Apigee\Exceptions\ParameterException $e) {
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
    foreach ($list as $dev_app) {
      $id = $dev_app->getAppId();
      $mail = strtolower($dev_app->getDeveloperMail());
      $array = $dev_app->toArray();
      $array['uid'] = (isset($uids[$mail]) ? $uids[$mail] : NULL);
      $app_entities[$id] = new Drupal\devconnect_developer_apps\DeveloperAppEntity($array);
    }

    return $app_entities;
  }

  /**
   * Sets the key status for a given developer app.
   *
   * Status may be TRUE|FALSE, 1|0, or approve|revoke.
   *
   * @param Drupal\devconnect_developer_apps\DeveloperAppEntity $entity
   * @param string $status
   * @return bool
   */
  public static function setKeyStatus(Drupal\devconnect_developer_apps\DeveloperAppEntity &$entity, $status) {
    try {
      $da = new Apigee\ManagementAPI\DeveloperApp(devconnect_default_api_client(), $entity->developer);
      $da->fromArray((array) $entity);
      $da->setKeyStatus($status);
      $entity = new Drupal\devconnect_developer_apps\DeveloperAppEntity($da->toArray());
      return TRUE;
    } catch (Apigee\Exceptions\ParameterException $e) {
      return FALSE;
    } catch (Apigee\Exceptions\ResponseException $e) {
      return FALSE;
    }
  }

  /**
   * Adds an array of Apigee\ManagementAPI\DeveloperApp objects to our internal
   * cache. If $ids is passed in, $list is modified such that only apps with
   * an appId in $ids are preserved (all others are unset).
   *
   * @param array $list
   * @param array|bool $ids
   */
  private function addListToCache(array &$list, $ids = array()) {
    foreach ($list as $app) {
      $key = $app->getAppId();
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

  /**
   * Determines if an app exists with the given appId or set of conditions.
   *
   * This is accomplished without writing to logs or watchdog. If the app
   * exists, the DeveloperAppEntity is returned, else FALSE.
   *
   * @param string|null $appId
   * @param array $conditions
   * @return bool|\Drupal\devconnect_developer_apps\DeveloperAppEntity
   */
  public function loadIfExists($appId = NULL, $conditions = array()) {
    if (!empty($appId) && array_key_exists($appId, $this->appCache)) {
      return TRUE;
    }
    if (!($this->orgConfig instanceof \Apigee\Util\OrgConfig)) {
      $this->orgConfig = devconnect_default_api_client();
    }
    // Store initial state of the config object
    $cached_org_config = $this->orgConfig;
    // Turn off error logging
    $this->orgConfig->logger = new \Psr\Log\NullLogger();
    $this->orgConfig->subscribers = array();

    $ids = array();
    if (!empty($appId) && is_scalar($appId)) {
      $ids[] = $appId;
    }
    $entities = $this->load($ids, $conditions);
    // Restore initial state of the config object
    $this->orgConfig = $cached_org_config;
    return empty($entities) ? FALSE : reset($entities);
  }

  /**
   * Returns the last exception returned from Edge.
   *
   * @return Apigee\Exceptions\ResponseException
   */
  public static function getLastException() {
    return self::$lastException;
  }
}

