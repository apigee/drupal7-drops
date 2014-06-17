<?php

class DeveloperController implements DrupalEntityControllerInterface, EntityAPIControllerInterface {
  /**
   * @var array
   */
  private $devCache;

  /**
   * @var array
   */
  private $emailCache;

  /**
   * @var array
   */
  private $orgConfigs;

  /**
   * Initializes internal variables.
   *
   * @param $entity_type
   */
  public function __construct($entity_type) {
    $this->devCache = array();
    $this->emailCache = array();
    if (!class_exists('Apigee\ManagementAPI\Developer')) {
      module_load_include('module', 'libraries');
      module_load_include('module', 'devconnect');
      devconnect_init();
    }
    $this->orgConfigs = devconnect_default_api_client("all");
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
   * @param array $ids
   * @param array $conditions
   *
   * @throws Apigee\Exceptions\ResponseException
   *
   * @return array
   */
  public function load($ids = array(), $conditions = array()) {
    $list = array();
    $email_lookup = array();
    foreach ($this->orgConfigs as $orgName => $config) {
      $dev_obj = new Apigee\ManagementAPI\Developer($config);


      if (empty($ids)) {
        // The following may throw Apigee\Exceptions\ResponseException if the
        // endpoint is unreachable.
        foreach ($dev_obj->loadAllDevelopers() as $d) {
          $email = $d->getEmail();
          $this->devCache[$email]['obj'] = $d;
          $this->devCache[$email]['orgNames'][$d->getDeveloperId()] = $orgName;
          $list[$email] = $d;
          if (!array_key_exists($email, $this->emailCache)) {
            $email_lookup[] = $email;
          }
        }
      }
      else {
        foreach ($ids as $email) {
          if (array_key_exists($email, $this->devCache) && $this->devCache[$email]['loaded'] === TRUE) {
            $list[] = $this->devCache[$email]['obj'];
          }
          else {
            $my_dev = clone $dev_obj;
            try {
              $my_dev->load($email);
              $email = $my_dev->getEmail(); // correct for case
              $this->devCache[$email]['obj'] = $my_dev;
              $this->devCache[$email]['orgNames'][$my_dev->getDeveloperId()] = $orgName;
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
    }
    foreach ($this->devCache as &$value) {
      $value['loaded'] = TRUE;
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
    $include_debug_data = (count($list) == 1);
    foreach ($list as $dev) {
      if (!($dev instanceof Apigee\ManagementAPI\DeveloperInterface)) {
        watchdog('DeveloperController', 'Non-developer object returned: @object', array('@object' => print_r($dev, TRUE)), WATCHDOG_ERROR);
        continue;
      }
      $array = $dev->toArray($include_debug_data);
      $email = $array['email'];
      $array['uid'] = (isset($this->emailCache[$email]) ? $this->emailCache[$email] : NULL);
      $array['orgNames'] = (isset($this->devCache[$email]['orgNames']) ? $this->devCache[$email]['orgNames'] : NULL);
      $array['forceSync'] = count(array_diff(array_keys($this->orgConfigs), $array['orgNames'])) != 0;
      $return[$email] = new Drupal\devconnect_user\DeveloperEntity($array);
    }
    // Correct for first/last name
    // TODO: verify that this is really necessary
    if (!empty($return) && db_table_exists('field_data_field_first_name') && db_table_exists('field_data_field_last_name')) {
      $query = db_select('users', 'u');
      $query->innerJoin('field_data_field_first_name', 'fn', 'u.uid = fn.entity_id AND fn.entity_type = \'user\'');
      $query->innerJoin('field_data_field_last_name', 'ln', 'u.uid = ln.entity_id AND ln.entity_type = \'user\'');
      $result = $query->fields('u', array('mail'))
        ->fields('fn', array('field_first_name_value'))
        ->fields('ln', array('field_last_name_value'))
        ->condition('u.mail', array_keys($return))
        ->execute();
      while ($row = $result->fetchAssoc()) {
        $email = $row['mail'];
        if (!array_key_exists($email, $return)) {
          // Case mismatch. Can't do anything about this...
          continue;
        }
        if (!empty($row['field_first_name_value'])) {
          $return[$email]->firstName = $row['field_first_name_value'];
        }
        if (!empty($row['field_last_name_value'])) {
          $return[$email]->lastName = $row['field_last_name_value'];
        }
      }
    }
    return $return;
  }


  /**
   * Determines if a developer exists with the given email or set of
   * conditions.
   *
   * This is accomplished without writing to logs or watchdog. If the developer
   * exists, the DeveloperEntity is returned, else FALSE.
   *
   * @param string|null $appId
   * @param array $conditions
   * @return bool|Drupal\devconnect_user\DeveloperEntity
   */
  public function loadIfExists($email = NULL, $conditions = array()) {
    if (!empty($email) && array_key_exists($email, $this->devCache)) {
      return $this->devCache[$email]['obj'];
    }
    // Store initial state of the config object
    $cached_org_config = array();
    foreach ($this->orgConfigs as $orgName => $config) {
      $cached_org_config[$orgName] = $config;
      // Turn off error logging
      $this->orgConfigs[$orgName]->logger = new \Psr\Log\NullLogger();
      $this->orgConfigs[$orgName]->subscribers = array();
    }

    $ids = array();
    if (!empty($email) && is_scalar($email)) {
      $ids[] = $email;
    }
    $entities = $this->load($ids, $conditions);
    // Restore initial state of the config object
    $this->orgConfigs = $cached_org_config;
    return empty($entities) ? FALSE : reset($entities);
  }

  /**
   * Implements EntityAPIControllerInterface::delete().
   *
   * @param array $ids
   */
  public function delete($ids) {
    foreach ($this->load($ids) as $id => $entity) {
      foreach ($entity->orgNames as $developer_id => $config_name) {

        $dev_app = new Apigee\ManagementAPI\Developer($this->orgConfigs[$config_name]);
        try {
          $dev_app->delete($id);
        } catch (Apigee\Exceptions\ResponseException $e) {
          if ($e->getCode() != 404) {
            throw $e;
          }
        }
        $entity->developer_id = $developer_id;
        devconnect_user_delete_from_cache($entity);
      }
    }
    $this->resetCache($ids);
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
    $developer = reset($this->load(array($entity->email)));
    $entity->developerId = NULL;
    $entity->organizationName = NULL;
    $entity->createdAt = NULL;
    $entity->createdBy = NULL;
    $entity->modifiedAt = NULL;
    $entity->modifiedBy = NULL;

    foreach ($this->orgConfigs as $config) {
      $dev = new Apigee\ManagementAPI\Developer($config);
      $dev->fromArray($entity);
      try {
        // Force Developer object to figure out if it's an update or insert.
        $dev->save(NULL);
        $_entity = new Drupal\devconnect_user\DeveloperEntity($dev->toArray());
        if ($_entity->email) {
          $uid = db_select('users', 'u')
            ->fields('u', array('uid'))
            ->condition('mail', $_entity->email)
            ->execute()
            ->fetchField();
          if ($uid > 1) {
            $_entity->uid = $uid;
            devconnect_user_write_to_cache($_entity);
          }
        }
      } catch (Apigee\Exceptions\ResponseException $e) {
        return FALSE;
      }
    }
    $this->resetCache(array($entity->email));
    return ($developer != NULL ? SAVED_UPDATED : SAVED_NEW);
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

  /**
   * Updates the email address associated with a developer.
   *
   * By default, we do not support changing the developer's email address on
   * the user-profile form -- we disable the element. However, customer-
   * specific code may do so, hence this function.
   *
   * Returns TRUE on success or FALSE on failure.
   *
   * @param \Drupal\devconnect_user\DeveloperEntity $entity
   * @param string $new_email
   * @return bool
   */
  public function updateEmail(Drupal\devconnect_user\DeveloperEntity $entity, $new_email) {
    $return = TRUE;
    foreach ($this->orgConfigs as $config) {
      try {
        $dev = new Apigee\ManagementAPI\Developer($config);
        $dev->load($entity->email);
        $dev->setEmail($new_email);
        $dev->save(TRUE, $entity->email);

        $this->devCache[$new_email] = $this->devCache[$entity->email];
        unset($this->devCache[$entity->email]);
        $entity->email = $new_email;

        devconnect_user_write_to_cache($entity);
      } catch (Exception $e) {
        $return = FALSE;
      }
    }
    return $return;
  }

  /**
   * Lists all email addresses for all developers in all configured orgs.
   *
   * @return array
   */
  public function listEmails() {
    $emails = array();
    try {
      foreach ($this->orgConfigs as $config) {
        $dev = new Apigee\ManagementAPI\Developer($config);
        $emails += $dev->listDevelopers();
      }
    } catch (Exception $e) {
      // Do nothing
    }
    sort($emails);
    return array_unique($emails);
  }
}
