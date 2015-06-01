<?php
/**
 * @file
 * The Controller class for the Apigee Company App.
 */

use Apigee\ManagementAPI\DeveloperApp;
use Apigee\ManagementAPI\CompanyApp;
use Apigee\Exceptions\ResponseException;
use Apigee\Exceptions\ParameterException;
use Drupal\devconnect_developer_apps\DeveloperAppEntity;
use Apigee\ManagementAPI\Company;
use Apigee\ManagementAPI\Developer;

class ApigeeCompanyAppController extends DeveloperAppController {

  /**
   * Implements DrupalEntityControllerInterface::__construct().
   *
   * @param string $entity_type
   *   The type of entity.
   */
  public function __construct($entity_type) {
    $this->appCache = array();
    if (!class_exists('CompanyApp')) {
      module_load_include('module', 'libraries');
      module_load_include('module', 'devconnect');
      devconnect_init();
    }
  }

  /**
   * Implements EntityAPIControllerInterface::save().
   *
   * @param object $entity
   *   The entity object to be saved.
   */
  public function save($entity) {

    $company_in_context = apigee_company_get_current_context_company();

    // If company is null, do the normal saving of developer apps.
    if ($company_in_context == NULL) {
      $developer_app_controller = new DeveloperAppController('developer_app');
      $is_saved = $developer_app_controller->save($entity);
      return $is_saved;
    }

    $config = self::getConfig($entity);
    $company_in_context = apigee_company_get_current_context_company();
    $entity->companyName = $company_in_context->name;
    // Make a copy so we can remove irrelevant members.
    $entity = (array) $entity;
    $company = new Company($config);

    try {
      $company->load($entity['companyName']);
    }
    catch (ResponseException $e) {
      $company->setName($entity['companyName']);
      $company->setDisplayName($entity['companyName']);
      $company->save();
      cache_clear_all('emc_company_apps:', 'cache_emc_company', TRUE);
      $company->load($entity['companyName']);
    }

    $is_update = !empty($entity['appId']);
    $dev_app = new CompanyApp($config, $company);
    // $product_cache = $entity['apiProductCache'];
    // unset ($entity['apiProductCache']);
    $dev_app->fromArray($entity);
    // $dev_app->setApiProductCache($product_cache);
    try {
      $dev_app->save($is_update);
      // $this->appCache[$dev_app->getAppId()] = $dev_app;
      // cache_clear_all('emc_company_apps:', 'cache_emc_company', TRUE);
    }
    catch (ResponseException $e) {
      self::$lastException = $e;
      return FALSE;
    }

    $dev_app_array = $dev_app->toArray();
    // Copy incoming UID to outgoing UID.
    $dev_app_array['uid'] = $entity['uid'];
    $last_app = new ApigeeCompanyAppEntity($dev_app_array);
    devconnect_developer_apps_write_to_cache($last_app);
    self::$lastApp = $last_app;

    return ($is_update ? SAVED_UPDATED : SAVED_NEW);
  }

  /**
   * Implements EntityAPIControllerInterface::create().
   *
   * Creates an empty developer_app entity, but does not save it.
   *
   * @param array $values
   *   An associative array with the keys as the object properties and values
   *   as the corresponding property value.
   *
   * @return Drupal\devconnect_developer_apps\DeveloperAppEntity
   *   Developer app entity object.
   */
  public function create(array $values = array()) {
    $dev_app = new CompanyApp(self::getConfig(), '');
    $dev_app->fromArray($values);
    return new ApigeeCompanyAppEntity($dev_app->toArray());
  }

  /**
   * {@inheritdoc}
   */
  public function buildContent($entity, $view_mode = 'full', $langcode = NULL, $page = FALSE) {
    $callback = 'devconnect_company_apps_view_' . $view_mode;
    if (function_exists($callback)) {
      return $callback($entity, $page, $langcode);
    }
    return array();
  }

  /**
   * {@inheritdoc}
   */
  public function load($ids = array(), $conditions = array()) {
    $company_in_context = apigee_company_get_current_context_company();

    // If company is null, do the normal loading of developer apps.
    if ($company_in_context == NULL) {
      $developer_app_controller = new DeveloperAppController('developer_app');
      $result = $developer_app_controller->load($ids, $conditions);
      return $result;
    }

    $orgs = self::getOrgs($conditions);
    $disable_logging = (isset($conditions['disableLogging']) && ($conditions['disableLogging'] === TRUE));

    $list = array();
    foreach ($orgs as $org) {
      $config = devconnect_default_org_config($org);
      if ($disable_logging) {
        $config->logger = new \Psr\Log\NullLogger();
        $config->subscribers = array();
      }

      if (array_key_exists('mail', $conditions)) {
        // Replace the mail condition with the company name to
        // load company apps instead of developer apps.
        $identifier = $company_in_context->name;
      }
      elseif (array_key_exists('developerId', $conditions)) {
        // Replace the developer ID condition with the company name to
        // load company apps instead of developer apps.
        $identifier = $company_in_context->name;
      }
      else {
        $identifier = NULL;
      }

      $company_app_api = new CompanyApp($config, $company_in_context->name);

      $app_list = array();
      if (isset($identifier) && empty($ids)) {
        // Fetch all apps in the org.
        try {
          $apps = $company_app_api->getListDetail();
          if ($apps != NULL) {
            $app_list = $apps;
          }
        }
        catch (ResponseException $e) {
          self::$lastException = $e;
        }
      }
      else {
        // We have a list of appIds. Fetch them now.
        foreach ($ids as $id) {

          try {
            // $dev_app->loadByAppId($id, TRUE) takes care of loading the
            // developer or the company app based on the developerId in the
            // response of loading an app by id.
            $dev_app = new DeveloperApp($config, $company_in_context->name);
            $app_list[] = $dev_app->loadByAppId($id, TRUE);
          }
          catch (ResponseException $e) {
            self::$lastException = $e;
          }
          catch (ParameterException $e) {
            self::$lastException = $e;
          }
        }
      }
    }

    $app_entities = array();
    $include_debug_data = (count($list) == 1);

    foreach ($app_list as $app) {
      $array = $app->toArray($include_debug_data);
      $array['orgName'] = $app->getConfig()->orgName;
      $app_entities[$app->getAppId()] = new ApigeeCompanyAppEntity($array);
    }
    return $app_entities;
  }

//  /**
//   * Sets the key status for a given developer app.
//   *
//   * Status may be TRUE|FALSE, 1|0, or approve|revoke.
//   *
//   * @param Drupal\devconnect_developer_apps\DeveloperAppEntity $entity
//   * @param string $status
//   * @return bool
//   */
//  public static function setKeyStatus(DeveloperAppEntity &$entity, $status) {
//    try {
//      $da = self::getAppFromEntity($entity);
//      if ($da) {
//        $da->setKeyStatus($status);
//        cache_clear_all('emc_company_apps:', 'cache_emc_company', TRUE);
//        $entity = new ApigeeCompanyAppEntity($da->toArray());
//        return TRUE;
//      }
//      return FALSE;
//    }
//    catch (ParameterException $e) {
//      return FALSE;
//    }
//    catch (ResponseException $e) {
//      return FALSE;
//    }
//  }

//  /**
//   * Deletes a named attribute from an app. Returns TRUE if successful, else
//   * FALSE. Note that if the Edge SDK is not recent enough, this functionality
//   * may be missing; in such a case, FALSE will consistently be returned.
//   *
//   * @param DeveloperAppEntity $entity
//   * @param string $attr_name
//   * @return bool
//   */
//  public static function deleteAttribute(DeveloperAppEntity &$entity, $attr_name) {
//    if (!method_exists('Apigee\ManagementAPI\DeveloperApp', 'deleteAttribute')) {
//      return FALSE;
//    }
//    try {
//      $da = self::getAppFromEntity($entity);
//      if ($da) {
//        $success = $da->deleteAttribute($attr_name);
//        if ($success) {
//          cache_clear_all('emc_company_apps:', 'cache_emc_company', TRUE);
//          $entity = new ApigeeCompanyAppEntity($da->toArray());
//        }
//        return TRUE;
//      }
//      return FALSE;
//    } catch (ParameterException $e) {
//      return FALSE;
//    } catch (ResponseException $e) {
//      return FALSE;
//    }
//  }

//  /**
//   * Deletes a named attribute from an app's credential. Returns TRUE if
//   * successful, else FALSE. Note that if the Edge SDK is not recent enough,
//   * this functionality may be missing; in such a case, FALSE will consistently
//   * be returned.
//   *
//   * @param DeveloperAppEntity $entity
//   * @param string $attr_name
//   * @return bool
//   */
//  public static function deleteCredentialAttribute(ApigeeCompanyAppEntity &$entity, $attr_name) {
//    if (!method_exists('Apigee\ManagementAPI\DeveloperApp', 'deleteCredentialAttribute')) {
//      return FALSE;
//    }
//    try {
//      $da = self::getAppFromEntity($entity);
//      if ($da) {
//        $success = $da->deleteCredentialAttribute($attr_name);
//        if ($success) {
//          cache_clear_all('emc_company_apps:', 'cache_emc_company', TRUE);
//          $entity = new ApigeeCompanyAppEntity($da->toArray());
//        }
//        return TRUE;
//      }
//      return FALSE;
//    } catch (ParameterException $e) {
//      return FALSE;
//    } catch (ResponseException $e) {
//      return FALSE;
//    }
//  }
//  private static function &getAppFromEntity(ApigeeCompanyAppEntity $entity) {
//    try {
//      $config = self::getConfig($entity);
//      $da = new CompanyApp($config, $entity->companyName);
//      $da->fromArray((array) $entity);
//      return $da;
//    } catch (ParameterException $e) {
//      return FALSE;
//    } catch (ResponseException $e) {
//      return FALSE;
//    }
//  }


  /**
   * {@inheritdoc}
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
          $dev_app0 = new DeveloperApp($config, '');
          try {
            $dev_app = $dev_app0->loadByAppId($id, TRUE);
          }
          catch (ResponseException $e) {
            $dev_app = NULL;
            self::$lastException = $e;
          }
          catch (ParameterException $e) {
            $dev_app = NULL;
            self::$lastException = $e;
          }
        }
        if (isset($dev_app)) {
          try {
            $entity = new DeveloperAppEntity($dev_app->toArray());
            $dev_app->delete();
            devconnect_developer_apps_delete_from_cache($entity);
            $deleted_count++;
          }
          catch (ResponseException $e) {
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
   * Determines if an app exists with the given appId or set of conditions.
   *
   * This is accomplished without writing to logs or watchdog. If the app
   * exists, the DeveloperAppEntity is returned, else FALSE.
   *
   * @param string|null $appId
   *   The the uuid of the app to be loaded.
   * @param array $conditions
   *   An array of condition for the apps to be loaded.
   *
   * @return bool|\Drupal\devconnect_developer_apps\DeveloperAppEntity
   *   A fully loaded Developer App Entity on success, False otherwise
   */
  public function loadIfExists($appId = NULL, $conditions = array(), $criteria = NULL) {
    return FALSE;

//    if ($criteria) {
//      // Check if displayName is unique for existing app
//      $display_names = db_select('cache_emc_company', 'e')
//        ->fields('e', array('data'))
//        ->condition('e.cid', '--app_human_names--')
//        ->execute()
//        ->fetchField();
//
//      if (!empty($display_names)) {
//        $displaynames = unserialize($display_names);
//        if ($displaynames[$conditions['name']]) {
//          return TRUE;
//        } else {
//          return FALSE;
//        }
//      }
//    } else {
//      // Check cached array of company names first
//      $app_names = db_select('cache_emc_company', 'e')
//        ->fields('e', array('data'))
//        ->condition('e.cid', '--app_names--')
//        ->execute()
//        ->fetchField();
//
//
//      if (!empty($app_names)) {
//        $appnames = unserialize($app_names);
//        if ($appnames[$conditions['name']]) {
//          return TRUE;
//        } else {
//          return FALSE;
//        }
//      }
//    }
//
//    if (!empty($appId) && array_key_exists($appId, $this->appCache)) {
//      return $this->appCache[$appId];
//    }
//
//    $ids = array();
//    if (!empty($appId) && is_scalar($appId)) {
//      $ids[] = $appId;
//    }
//
//    $conditions2 = array();
//    $conditions2['disableLogging'] = TRUE;
//    $entities = $this->load($ids, $conditions2);
//
//    if ($criteria) {
//      // Grab app_names from cacche
//      $display_names = db_select('cache_emc_company', 'e')
//        ->fields('e', array('data'))
//        ->condition('e.cid', '--app_human_names--')
//        ->execute()
//        ->fetchField();
//
//      if (!empty($display_names)) {
//        $displaynames = unserialize($display_names);
//        if ($displaynames[$conditions['name']]) {
//          return TRUE;
//        } else {
//          return FALSE;
//        }
//      }
//    } else {
//      // Grab app_names from cache
//      $app_names = db_select('cache_emc_company', 'e')
//        ->fields('e', array('data'))
//        ->condition('e.cid', '--app_names--')
//        ->execute()
//        ->fetchField();
//
//      if (!empty($app_names)) {
//        $appnames = unserialize($app_names);
//        if ($appnames[$conditions['name']]) {
//          return TRUE;
//        } else {
//          return FALSE;
//        }
//      }
//    }
  }

}
