<?php

class DeveloperAppController implements DrupalEntityControllerInterface, EntityAPIControllerInterface {
  private $appCache;

  private static $lastAppId;

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
        $dev_app->loadByAppId($id, TRUE);
      }
      $dev_app->delete();
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
    $dev_app->save($is_update);
    $this->appCache[$dev_app->getAppId()] = $dev_app;

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
    return $dev_app->toArray();
  }

  /**
   * Implements EntityAPIControllerInterface::export().
   *
   * @param $entity
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

    if (isset($conditions['mail'])) {
      $dev_app = new Apigee\ManagementAPI\DeveloperApp($client, $conditions['mail']);
      if (isset($conditions['name'])) {
        $dev_app->load($conditions['name']);
        $list = array($dev_app);
      }
      else {
        $list = $dev_app->getListDetail();
      }
      $this->addListToCache($list, $ids);
    }
    // TODO: add more conditions here such as Status
    elseif (empty($ids)) { // Fetch all apps in the org.
      $dev_app = new Apigee\ManagementAPI\DeveloperApp($client, '');
      $list = $dev_app->listAllOrgApps();
      $this->addListToCache($list, $ids);
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
          $app->loadByAppId($id);
          $list[] = $app;
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
      $uids = db_select('users', 'u')
        ->fields('u', array('mail', 'uid'))
        ->condition('mail', array_keys($uids))
        ->execute()
        ->fetchAllKeyed();
    }

    $app_entities = array();
    foreach ($list as $dev_app) {
      $mail = strtolower($dev_app->getDeveloperMail());
      $array = $dev_app->toArray();
      $array['uid'] = (isset($uids[$mail]) ? $uids[$mail] : NULL);
      $app_entities[$dev_app->getAppId()] = $array;
    }
    return $app_entities;
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
}
