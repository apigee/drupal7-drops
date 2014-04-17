<?php

class UserRoleController implements DrupalEntityControllerInterface, EntityAPIControllerInterface {
  private $roleCache;
  private $userCache;

  /**
   * Implements DrupalEntityControllerInterface::__construct().
   *
   * @param $entity_type
   */
  public function __construct($entity_type) {
    $this->roleCache = array();
    if (!class_exists('Apigee\ManagementAPI\UserRole')) {
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
    if (is_array($ids) && !empty($this->userCache)) {
      foreach ($ids as $id) {
        if (isset($this->userCache[$id])) {
          unset ($this->userCache[$id]);
        }
      }
    }
    else {
      $this->userCache = array();
    }
  }

  /**
   * Implements DrupalEntityControllerInterface::load().
   *
   * @param array $names
   * @param array $conditions
   *
   * @throws Apigee\Exceptions\ResponseException
   *
   * @return array
   */
  public function load($ids = array(), $conditions = array()) {
    $ur = new Apigee\ManagementAPI\UserRole(devconnect_default_api_client());

    if (empty($ids)) {
      if (empty($this->roleCache)) {
        $this->roleCache = $ur->listRoles();
      }
      $ids = $this->roleCache;
    }

    $list = array();
    foreach ($ids as $id) {
      if (!array_key_exists($id, $this->userCache)) {
        $this->userCache[$id] = $ur->getUsersByRole($id);
      }
      $item = array(
        'name' => $id,
        'users' => $this->userCache[$id]
      );
      $list[$id] = new Drupal\devconnect_user\UserRoleEntity($item);
    }

    return $list;
  }

  /**
   * Implements EntityAPIControllerInterface::delete().
   *
   * @param array $ids
   */
  public function delete($ids) {
    $ur = new Apigee\ManagementAPI\UserRole(devconnect_default_api_client());
    foreach ($ids as $id) {
      try {
        $ur->deleteRole($id);
      } catch (Apigee\Exceptions\ResponseException $e) {
        if ($e->getCode() != 404) {
          throw $e;
        }
      }
      if (($i = array_search($id, $this->roleCache)) !== FALSE) {
        unset ($this->roleCache[$i]);
      }
      if (array_key_exists($id, $this->userCache)) {
        unset ($this->userCache[$id]);
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
    $name = $entity['name'];
    $users = $entity['users'];
    $return_val = SAVED_UPDATED;
    $ur = new Apigee\ManagementAPI\UserRole(devconnect_default_api_client());
    if (!$ur->roleExists($name)) {
      try {
        $ur->addRole($name);
        $this->roleCache[] = $name;
        $return_val = SAVED_NEW;
      } catch (Apigee\Exceptions\ResponseException $e) {
        return FALSE;
      }
    }
    try {
      $ur->setRoleUsers($users);
    } catch (Apigee\Exceptions\ResponseException $e) {
      return FALSE;
    }
    $this->userCache[$name] = $users;
    return $return_val;
  }

  /**
   * Implements EntityAPIControllerInterface::create().
   *
   * @param array $values
   */
  public function create(array $values = array()) {
    return new Drupal\devconnect_user\UserRoleEntity($values);
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
   * @todo fill this in
   *
   * @param array $entity
   * @param string $view_mode
   * @param string|null $langcode
   */
  public function buildContent($entity, $view_mode = 'full', $langcode = NULL) {
    return array();
  }

  /**
   * Implements EntityAPIControllerInterface::view().
   *
   * @todo fill this in
   *
   * @param array $entities
   * @param string $view_mode
   * @param string|null $langcode
   * @param boolean $page
   */
  public function view($entities, $view_mode = 'full', $langcode = NULL, $page = FALSE) {
    return array();
  }
}
