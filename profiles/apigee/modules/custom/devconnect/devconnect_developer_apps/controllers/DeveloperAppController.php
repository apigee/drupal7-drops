<?php

use Apigee\ManagementAPI\DeveloperApp;
use Apigee\Exceptions\ResponseException;
use Apigee\Exceptions\ParameterException;
use Drupal\devconnect_developer_apps\DeveloperAppEntity;

class DeveloperAppController implements DrupalEntityControllerInterface, EntityAPIControllerInterface {
  /**
   * @var array
   */
  protected $appCache;

  /**
   * @var \Drupal\devconnect_developer_apps\DeveloperAppEntity
   */
  protected static $lastApp;

  /**
   * @var \Exception
   */
  protected static $lastException;

  /**
   * @var \Apigee\Util\OrgConfig
   */
  protected $orgConfig;

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
    $id_count = count($ids);
    $deleted_count = 0;
    foreach (self::getOrgs() as $org) {
      $config = devconnect_default_org_config($org);
      foreach ($ids as $id) {
        // If entity is in our cache, we can make one fewer server roundtrips.
        if (array_key_exists($id, $this->appCache)) {
          $dev_app = $this->appCache[$id];
          unset ($this->appCache[$id]);
        }
        else {
          // Not in cache. Fetch, then delete.
          $dev_app = new DeveloperApp($config, '');
          try {
            $dev_app->loadByAppId($id, TRUE);
          } catch (ResponseException $e) {
            $dev_app = NULL;
            self::$lastException = $e;
          } catch (ParameterException $e) {
            $dev_app = NULL;
            self::$lastException = $e;
          }
        }
        if (isset($dev_app)) {
          try {
            $entity = new DeveloperAppEntity($dev_app->toArray());
            $entity->orgName = $config->orgName;
            $dev_app->delete();
            devconnect_developer_apps_delete_from_cache($entity);
            $deleted_count++;
          } catch (ResponseException $e) {
            self::$lastException = $e;
          }
        }
        if ($id_count == $deleted_count) {
          break;
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
   * @param Drupal\devconnect_developer_apps\DeveloperAppEntity $entity
   */
  public function save($entity) {

    $config = self::getConfig($entity);
    // Make a copy so we can remove irrelevant members
    $entity = (array)$entity;
    $is_update = !empty($entity['appId']);
    $dev_app = new DeveloperApp($config, $entity['developer']);
    $product_cache = $entity['apiProductCache'];
    unset ($entity['apiProductCache']);
    $dev_app->fromArray($entity);
    $dev_app->setApiProductCache($product_cache);

    // If Edge SDK is recent enough, support setting key expiry.
//    if (method_exists($dev_app, 'setKeyExpiry')) {
//      $dev_app->setKeyExpiry($entity['keyExpiresIn'] * 86400);
//    }

    try {
      $dev_app->save($is_update);
      $this->appCache[$dev_app->getAppId()] = $dev_app;
    } catch (ResponseException $e) {
      self::$lastException = $e;
      return FALSE;
    }

    $dev_app_array = $dev_app->toArray();
    // Copy incoming UID to outgoing UID
    $dev_app_array['uid'] = $entity['uid'];
    $dev_app_array['orgName'] = $config->orgName;
    $last_app = new DeveloperAppEntity($dev_app_array);
    $last_app->orgName = $config->orgName;

    devconnect_developer_apps_write_to_cache($last_app);

    self::$lastApp = $last_app;

    return ($is_update ? SAVED_UPDATED : SAVED_NEW);
  }

  protected static function getConfig(DeveloperAppEntity $entity = NULL) {
    return devconnect_default_org_config();
  }

  protected static function getOrgs($conditions = NULL) {
    return array('default');
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
    $dev_app = new DeveloperApp(self::getConfig(), '');
    $dev_app->fromArray($values);
    return new DeveloperAppEntity($dev_app->toArray());
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
    $orgs = self::getOrgs($conditions);
    $disableLogging = (isset($conditions['disableLogging']) && ($conditions['disableLogging'] === TRUE));

    $list = array();
    foreach ($orgs as $org) {
      $config = devconnect_default_org_config($org);
      if ($disableLogging) {
        $config->logger = new \Psr\Log\NullLogger();
        $config->subscribers = array();
      }

      if (array_key_exists('mail', $conditions)) {
        $identifier = $conditions['mail'];
      }
      elseif (array_key_exists('developerId', $conditions)) {
        $identifier = $conditions['developerId'];
      }
      else {
        $identifier = NULL;
      }

      if (isset($identifier) && empty($ids)) {
        $dev_app = new DeveloperApp($config, $identifier);
        if (array_key_exists('name', $conditions)) {
          try {
            $dev_app->load($conditions['name']);
            $list += array($dev_app);
          } catch (ResponseException $e) {
            self::$lastException = $e;
          }
        }
        else {
          try {
            $list += $dev_app->getListDetail();
          } catch (ResponseException $e) {
            self::$lastException = $e;
          }
        }
      }
      // TODO: add more conditions here such as Status
      elseif (empty($ids)) { // Fetch all apps in the org.
        $dev_app = new DeveloperApp($config, '');
        try {
          $list += $dev_app->listAllApps();
          $this->addListToCache($list, $ids);
        } catch (ResponseException $e) {
          self::$lastException = $e;
        }
      }
      else {
        $sub_list = array();
        // We have a list of appIds. Fetch them now.
        foreach ($ids as $id) {
          if (isset($this->appCache[$id])) {
            $sub_list[$id] = $this->appCache[$id];
          }
        }
        if (count($list) < count($ids)) {
          $remaining_ids = array_diff($ids, array_keys($list));
          $dev_app = new DeveloperApp($config, '');
          foreach ($remaining_ids as $id) {
            $app = clone $dev_app;
            try {
              $app->loadByAppId($id, TRUE);
              $sub_list[] = $app;
            } catch (ResponseException $e) {
              self::$lastException = $e;
            } catch (ParameterException $e) {
              self::$lastException = $e;
            }
          }
        }
        $list += array_values($sub_list);
      }
    }
    $this->addListToCache($list, $ids);

    $uids = array();
    foreach ($list as $dev_app) {
      if ($dev_app instanceof Apigee\ManagementAPI\DeveloperApp) {
        $email = $dev_app->getDeveloperMail();
        if (!array_key_exists($email, $uids)) {
          $uids[strtolower($email)] = NULL;
        }
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
    $include_debug_data = (count($list) == 1);
    foreach ($list as $dev_app) {
      if ($dev_app instanceof Apigee\ManagementAPI\DeveloperApp) {
        $id = $dev_app->getAppId();
        $mail = strtolower($dev_app->getDeveloperMail());
        $array = $dev_app->toArray($include_debug_data);
        $array['orgName'] = $dev_app->getConfig()->orgName;
        $array['uid'] = (array_key_exists($mail, $uids) ? $uids[$mail] : NULL);
        $app_entities[$id] = new DeveloperAppEntity($array);
      }
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
  public static function setKeyStatus(DeveloperAppEntity &$entity, $status) {
    try {
      $da = self::getAppFromEntity($entity);
      if ($da) {
        $da->setKeyStatus($status);
        $entity = new DeveloperAppEntity($da->toArray());
        return TRUE;
      }
      return FALSE;
    } catch (ParameterException $e) {
      return FALSE;
    } catch (ResponseException $e) {
      return FALSE;
    }
  }

  /**
   * Deletes a named attribute from an app. Returns TRUE if successful, else
   * FALSE. Note that if the Edge SDK is not recent enough, this functionality
   * may be missing; in such a case, FALSE will consistently be returned.
   *
   * @param DeveloperAppEntity $entity
   * @param string $attr_name
   * @return bool
   */
  public static function deleteAttribute(DeveloperAppEntity &$entity, $attr_name) {
    if (!method_exists('Apigee\ManagementAPI\DeveloperApp', 'deleteAttribute')) {
      return FALSE;
    }
    try {
      $da = self::getAppFromEntity($entity);
      if ($da) {
        $success = $da->deleteAttribute($attr_name);
        if ($success) {
          $entity = new DeveloperAppEntity($da->toArray());
        }
        return TRUE;
      }
      return FALSE;
    } catch (ParameterException $e) {
      return FALSE;
    } catch (ResponseException $e) {
      return FALSE;
    }
  }

  /**
   * Deletes a named attribute from an app's credential. Returns TRUE if
   * successful, else FALSE. Note that if the Edge SDK is not recent enough,
   * this functionality may be missing; in such a case, FALSE will consistently
   * be returned.
   *
   * @param DeveloperAppEntity $entity
   * @param string $attr_name
   * @return bool
   */
  public static function deleteCredentialAttribute(DeveloperAppEntity &$entity, $attr_name) {
    if (!method_exists('Apigee\ManagementAPI\DeveloperApp', 'deleteCredentialAttribute')) {
      return FALSE;
    }
    try {
      $da = self::getAppFromEntity($entity);
      if ($da) {
        $success = $da->deleteCredentialAttribute($attr_name);
        if ($success) {
          $entity = new DeveloperAppEntity($da->toArray());
        }
        return TRUE;
      }
      return FALSE;
    } catch (ParameterException $e) {
      return FALSE;
    } catch (ResponseException $e) {
      return FALSE;
    }
  }
  private static function &getAppFromEntity(DeveloperAppEntity $entity) {
    try {
      $config = self::getConfig($entity);
      $da = new DeveloperApp($config, $entity->developer);
      $da->fromArray((array) $entity);
      return $da;
    } catch (ParameterException $e) {
      return FALSE;
    } catch (ResponseException $e) {
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
  protected function addListToCache(array &$list, $ids = array()) {
    foreach ($list as $app) {
      /** @var Apigee\ManagementAPI\DeveloperApp $app */
      $key = $app->getAppId();
      $this->appCache[$key] = $app;
    }
    if (!empty($ids)) {
      foreach (array_keys($list) as $i) {
        $app =& $list[$i];
        if (!in_array($app->getAppId(), $ids)) {
          unset($app);
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
      return $this->appCache[$appId];
    }

    $ids = array();
    if (!empty($appId) && is_scalar($appId)) {
      $ids[] = $appId;
    }
    $conditions['disableLogging'] = TRUE;
    $entities = $this->load($ids, $conditions);
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

