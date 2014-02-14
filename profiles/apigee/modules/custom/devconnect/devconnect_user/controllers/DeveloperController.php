<?php

class DeveloperController implements DrupalEntityControllerInterface, EntityAPIControllerInterface {
  private $devCache;
  private $emailCache;

  /**
   * Implements DrupalEntityControllerInterface::__construct().
   *
   * @param $entity_type
   */
  public function __construct($entity_type) {
    $this->devCache = array();
    $this->emailCache = array();
  }

  /**
   * Implements DrupalEntityControllerInterface::resetCache().
   *
   * @param array $ids
   */
  public function resetCache(array $ids = NULL) {
    if (is_array($ids) && !empty($this->devCache)) {
      foreach ($ids as $id) {
        if (isset($this->devCache[$id])) {
          unset ($this->devCache[$id]);
        }
      }
    }
    else {
      $this->devCache = array();
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
    $dev_obj = new Apigee\ManagementAPI\Developer(devconnect_default_api_client());

    $email_lookup = array();

    if (empty($ids)) {
      // The following may throw Apigee\Exceptions\ResponseException if the
      // endpoint is unreachable.
      $list = $dev_obj->loadAllDevelopers();
      foreach ($list as $d) {
        $email = $d->getEmail();
        $this->devCache[$email] = $d;
        if (!array_key_exists($email, $this->emailCache)) {
          $email_lookup[] = $email;
        }
      }
    }
    else {
      $list = array();
      foreach ($ids as $email) {
        if (array_key_exists($email, $this->devCache)) {
          $list[] = $this->devCache[$email];
        }
        else {
          $my_dev = clone $dev_obj;
          try {
            $my_dev->load($email);
            $email = $my_dev->getEmail(); // correct for case
            $this->devCache[$email] = $my_dev;
            if (!array_key_exists($email, $this->emailCache)) {
              $email_lookup[] = $email;
            }
            $list[] = $my_dev;
          } catch (Apigee\Exceptions\ResponseException $e) {
            if ($e->getCode() != 404) {
              throw $e;
            }
          }
        }
      }
    }

    // Look up UIDs by email
    if (!empty($email_lookup)) {
      $result = db_select('users', 'u')
        ->fields('u', array('mail', 'uid'))
        ->condition('mail', $email_lookup)
        ->execute();
      while ($row = $result->fetchAssoc()) {
        $this->emailCache[$row['mail']] = $row['uid'];
      }
    }

    $return = array();
    foreach ($list as $dev) {
      if (!($dev instanceof Apigee\ManagementAPI\DeveloperInterface)) {
        watchdog('DeveloperController', 'Non-developer object returned: @object', array('@object' => print_r($dev, TRUE)), WATCHDOG_ERROR);
        continue;
      }
      $array = $dev->toArray();
      $array['uid'] = (isset($this->emailCache[$array['email']]) ? $this->emailCache[$array['email']] : NULL);
      $return[$array['email']] = new Drupal\devconnect_user\DeveloperEntity($array);
    }
    return $return;
  }

  /**
   * Implements EntityAPIControllerInterface::delete().
   *
   * @param array $ids
   */
  public function delete($ids) {
    $dev_app = new Apigee\ManagementAPI\Developer(devconnect_default_api_client());
    foreach ($ids as $id) {
      try {
        $dev_app->delete($id);
      } catch (Apigee\Exceptions\ResponseException $e) {
        if ($e->getCode() != 404) {
          throw $e;
        }
      }
      if (isset($this->devCache[$id])) {
        unset ($this->devCache[$id]);
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
    $is_update = !empty($entity['developerId']);
    if (!$is_update) {
      // Force Developer object to figure out if it's an update or insert.
      $is_update = NULL;
    }
    $dev = new Apigee\ManagementAPI\Developer(devconnect_default_api_client());
    $dev->fromArray($entity);
    try {
      $dev->save($is_update);
    } catch (Apigee\Exceptions\ResponseException $e) {
      return FALSE;
    }
    $this->devCache[$entity['email']] = $entity;

    return ($is_update ? SAVED_UPDATED : SAVED_NEW);
  }

  /**
   * Implements EntityAPIControllerInterface::create().
   *
   * @param array $values
   */
  public function create(array $values = array()) {
    return new Drupal\devconnect_user\DeveloperEntity($values);
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
