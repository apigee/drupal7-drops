<?php
/**
 * @file
 * Entity controller for developers.
 */

use Apigee\ManagementAPI\Developer;
use Apigee\Exceptions\ResponseException;
use Apigee\Exceptions\ParameterException;
use Drupal\devconnect_user\DeveloperEntity;

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
   * {@inheritdoc}
   */
  public function __construct($entity_type) {
    $this->devCache = array();
    $this->emailCache = array();
    if (!class_exists('Apigee\ManagementAPI\Developer')) {
      module_load_include('module', 'libraries');
      module_load_include('module', 'devconnect');
      devconnect_init();
    }
  }

  /**
   * {@inheritdoc}
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
   * {@inheritdoc}
   */
  public function load($ids = array(), $conditions = array()) {
    $email_lookup = array();

    $list = array();
    foreach (self::getOrgs($conditions) as $org) {
      $config = devconnect_default_org_config($org);
      if (array_key_exists('suppressLogs', $conditions) && $conditions['suppressLogs']) {
        $config->logger = new \Psr\Log\NullLogger();
        $config->subscribers = array();
      }

      $dev_obj = new Developer(devconnect_default_org_config($org));

      if (empty($ids)) {
        // The following may throw Apigee\Exceptions\ResponseException if the
        // endpoint is unreachable.
        try {
          $list += $dev_obj->loadAllDevelopers();
          foreach ($list as $d) {
            /** @var Apigee\ManagementAPI\Developer $d */
            $email = $d->getEmail();
            $this->devCache[$d->getDeveloperId()] = $d;
            if (!array_key_exists($email, $this->emailCache)) {
              $email_lookup[] = $email;
            }
          }
        }
        catch (ResponseException $e) {
        }
      }
      else {
        foreach ($ids as $id) {
          $is_email = valid_email_address($id);
          if ($is_email || !array_key_exists($id, $this->devCache)) {
            try {
              $my_dev = clone $dev_obj;
              $my_dev->load($id);
              $id = $my_dev->getDeveloperId();
              $mail = $my_dev->getEmail();
              $this->devCache[$id] = $my_dev;
              $list[] = $my_dev;
              if (!in_array($mail, $this->emailCache)) {
                $email_lookup[] = $mail;
              }
            }
            catch (Exception $e) {
            }
          }
          else {
            $list[] = $this->devCache[$id];
          }
        }
      }
    }

    // Look up UIDs by email.
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
      $array = $dev->toArray($include_debug_data);
      $email = $array['email'];
      $array['uid'] = (isset($this->emailCache[$email]) ? $this->emailCache[$email] : NULL);
      $return[$email] = new DeveloperEntity($array);
    }
    // Correct for first/last name.
    // TODO: verify that this is really necessary.
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

    if (array_key_exists('cache', $conditions) && $conditions['cache'] === FALSE) {
      $this->devCache = array();
    }

    return $return;
  }

  /**
   * Determines if developer exists with the given email or set of conditions.
   *
   * This is accomplished without writing to logs or watchdog. If the developer
   * exists, the DeveloperEntity is returned, else FALSE.
   *
   * @param string|null $id
   *   Developer email, if any.
   * @param array $conditions
   *   List of conditions for loading, if any.
   *
   * @return bool|Drupal\devconnect_user\DeveloperEntity
   *   Returns developer entity if found, otherwise returns FALSE.
   */
  public function loadIfExists($id = NULL, $conditions = array()) {
    if (!empty($id) && array_key_exists($id, $this->devCache)) {
      return $this->devCache[$id];
    }
    $ids = array();
    if (!empty($id) && is_scalar($id)) {
      $ids[] = $id;
    }
    $conditions['suppressLogs'] = TRUE;
    $entities = $this->load($ids, $conditions);
    return empty($entities) ? FALSE : reset($entities);
  }

  /**
   * {@inheritdoc}
   */
  public function delete($ids) {
    $id_count = count($ids);
    $deleted_count = 0;
    foreach (self::getOrgs() as $org) {
      $config = devconnect_default_org_config($org);
      foreach ($ids as $id) {
        $delete_succeeded = FALSE;
        // If entity is in our cache, we can make one fewer server roundtrips.
        if (array_key_exists($id, $this->devCache)) {
          unset ($this->devCache[$id]);
          $delete_succeeded = TRUE;
        }
        else {
          // Not in cache. Fetch, then delete.
          $dev = new Developer($config);
          try {
            $entity = new DeveloperEntity($dev->toArray());
            $dev->delete($id);
            if ($dev->getDeveloperId() === NULL) {
              devconnect_user_delete_from_cache($entity);
              $deleted_count++;
              $delete_succeeded = TRUE;
              if (array_key_exists($entity->developerId, $this->devCache)) {
                unset($this->devCache[$entity->developerId]);
              }
            }
          }
          catch (ResponseException $e) {
          }
          catch (ParameterException $e) {
          }
        }
        if ($delete_succeeded) {
          $entity = new DeveloperEntity(array('developerId' => $id));
          devconnect_user_delete_from_cache($entity);
          $deleted_count++;
        }

        if ($id_count == $deleted_count) {
          break;
        }
      }
    }
  }

  /**
   * {@inheritdoc}
   */
  public function invoke($hook, $entity) {
    // This function is required by EntityAPIControllerInterface, but is
    // unused.
  }

  /**
   * {@inheritdoc}
   */
  public function save($entity) {
    $is_update = !empty($entity->developerId);
    if (!$is_update) {
      // Force Developer object to figure out if it's an update or insert.
      $is_update = NULL;
    }
    if ($entity->email) {
      $uid = db_select('users', 'u')
        ->fields('u', array('uid'))
        ->condition('mail', $entity->email)
        ->execute()
        ->fetchField();
    }
    else {
      $uid = 0;
    }
    $default_config = devconnect_default_org_config();

    $old_entity = (array) $entity;
    if (empty($entity->orgNames)) {
      $entity->orgNames = array('default');
    }

    $overall_success = TRUE;
    foreach ($entity->orgNames as $org) {
      if ($org == 'default' || $org == $default_config->orgName) {
        $config = $default_config;
      }
      else {
        $config = devconnect_default_org_config($org);
      }

      $dev = new Developer($config);
      $dev->fromArray($old_entity);
      try {
        $dev->save($is_update);
        $new_entity = new DeveloperEntity($dev->toArray());
        if ($new_entity->email && $uid > 1) {
          $new_entity->uid = $uid;
          devconnect_user_write_to_cache($new_entity);
          // Prevent cache from being rewritten.
          $uid = 0;
        }
        $this->devCache[$new_entity->developerId] = $new_entity;
      }
      catch (ResponseException $e) {
        $overall_success = FALSE;
      }

    }
    if (!$overall_success) {
      return FALSE;
    }
    return ($is_update ? SAVED_UPDATED : SAVED_NEW);
  }

  /**
   * {@inheritdoc}
   */
  public function create(array $values = array()) {
    return new DeveloperEntity($values);
  }

  /**
   * {@inheritdoc}
   */
  public function export($entity, $prefix = '') {
    return json_encode($entity);
  }

  /**
   * {@inheritdoc}
   */
  public function import($export) {
    return @json_decode($export);
  }

  /**
   * {@inheritdoc}
   */
  public function buildContent($entity, $view_mode = 'full', $langcode = NULL) {
    // This function is required by EntityAPIControllerInterface, but is unused.
    return array();
  }

  /**
   * {@inheritdoc}
   */
  public function view($entities, $view_mode = 'full', $langcode = NULL, $page = FALSE) {
    // This function is required by EntityAPIControllerInterface, but is unused.
    return array();
  }

  /**
   * Updates the email address associated with a developer.
   *
   * By default, we do not support changing the developer's email address on
   * the user-profile form -- we disable the element. However, customer-
   * specific code may do so, hence this function.
   *
   * @param \Drupal\devconnect_user\DeveloperEntity $entity
   *   The entity whose email address is to be changed.
   * @param string $new_email
   *   The new email to be assigned to this entity.
   *
   * @return bool
   *   TRUE on success, FALSE on failure.
   */
  public function updateEmail(DeveloperEntity $entity, $new_email) {
    $saved = FALSE;
    if (empty($entity->orgNames)) {
      $entity->orgNames = array('default');
    }
    foreach ($entity->orgNames as $org) {
      if (module_exists('devconnect_multiorg')) {
        $org = devconnect_multiorg_find_requested_org($org);
      }
      $config = devconnect_default_org_config($org);
      try {
        $dev = new Developer($config);
        $dev->load($entity->developerId);
        $dev->setEmail($new_email);
        $dev->save(TRUE, $entity->email);
        $saved = TRUE;
        $this->devCache[$entity->developerId] = $dev;
        $entity->email = $new_email;
      }
      catch (Exception $e) {
        watchdog('devconnect_user', 'Error updating the developer email: %response_message', array('%response_message' => $e->getMessage()), WATCHDOG_ERROR);
      }
    }
    return $saved;
  }

  /**
   * Lists all email addresses for all developers in all configured orgs.
   *
   * @return array
   *   List of all email addresses of all developers in all configured orgs.
   */
  public function listEmails() {
    $emails = array();
    foreach (self::getOrgs() as $org) {
      $config = devconnect_default_org_config($org);
      try {
        $dev = new Developer($config);
        $emails += $dev->listDevelopers();
      }
      catch (Exception $e) {
      }
    }
    sort($emails);
    return array_unique($emails);
  }

  /**
   * Lists all org names from which current request should fetch developers.
   *
   * @param array $conditions
   *   Conditions applicable to the current request.
   *
   * @return array
   *   List of strings, each representing an org name. The special value
   *   'default' represents the org from which we expect to fetch data when no
   *   other criteria are present.
   */
  protected static function getOrgs(array $conditions = NULL) {
    // By itself, this function does virtually nothing; it is here so that
    // subclasses can implement custom functionality without having to
    // copy/paste larger chunks of code.
    return array('default');
  }

}
