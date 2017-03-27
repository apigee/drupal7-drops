<?php
/**
 * @file
 * The Controller class for the Apigee Company App.
 */

use Apigee\ManagementAPI\DeveloperApp;
use Apigee\ManagementAPI\CompanyApp;
use Apigee\Exceptions\ResponseException;
use Apigee\Exceptions\ParameterException;
use Apigee\ManagementAPI\Company;
use Drupal\devconnect_developer_apps\DeveloperAppEntity;

/**
 * Entity controller class for developer_app entity.
 *
 * This class overrides the usual developer_app controller when apigee_company
 * is enabled.
 */
class ApigeeCompanyAppController extends DeveloperAppController {

  /**
   * Implements EntityAPIControllerInterface::save().
   *
   * @param DeveloperAppEntity $entity
   *   The entity object to be saved.
   *
   * @return int|bool
   *   Returns one of the following: FALSE, SAVED_UPDATED, SAVED_NEW.
   */
  public function save($entity) {

    $company_in_context = apigee_company_get_current_context_company();

    // If company is null, do the normal saving of developer apps.
    if (!isset($company_in_context)) {
      // We cannot do entity_get_controller() here, because we want the
      // original, non-altered developer_app controller in this case.
      $developer_app_controller = new DeveloperAppController('developer_app');
      $is_saved = $developer_app_controller->save($entity);
      return $is_saved;
    }

    $config = self::getConfig($entity);
    $company_in_context = apigee_company_get_current_context_company();
    // Make a copy so we can remove irrelevant members.
    $entity_copy = (array) $entity;
    $company = new Company($config);

    try {
      $company->load($company_in_context->name);
    }
    catch (ResponseException $e) {
      $company->setName($company_in_context->name);
      $company->setDisplayName($company_in_context->name);
      $company->save();
      $company->load($company_in_context->name);
    }

    $is_update = !empty($entity_copy['appId']);
    $dev_app = new CompanyApp($config, $company);
    $dev_app->fromArray($entity_copy);
    try {
      $dev_app->save($is_update);
    }
    catch (ResponseException $e) {
      self::$lastException = $e;
      return FALSE;
    }

    $dev_app_array = $dev_app->toArray();
    // Copy incoming UID to outgoing UID.
    $dev_app_array['uid'] = $entity_copy['uid'];
    $last_app = new ApigeeCompanyAppEntity($dev_app_array);
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
    /** @var ApigeeCompanyEntity $company_in_context */
    $company_in_context = apigee_company_get_current_context_company();

    // If company is null, do the normal loading of developer apps.
    if (!isset($company_in_context)) {
      $developer_app_controller = new DeveloperAppController('developer_app');
      $result = $developer_app_controller->load($ids, $conditions);
      return $result;
    }

    $orgs = self::getOrgs($conditions);
    $disable_logging = (isset($conditions['disableLogging']) && ($conditions['disableLogging'] === TRUE));

    $app_list = array();

    foreach ($orgs as $org) {
      $config = devconnect_default_org_config($org);
      if ($disable_logging) {
        $config->logger = new \Psr\Log\NullLogger();
        $config->subscribers = array();
      }

      if (array_key_exists('mail', $conditions) || array_key_exists('developerId', $conditions)) {
        // Replace the mail or developerId condition with the company name to
        // load company apps instead of developer apps.
        $identifier = $company_in_context->name;
      }
      else {
        $identifier = NULL;
      }

      $company_app_api = new CompanyApp($config, $company_in_context->name);

      if (isset($identifier) && empty($ids)) {
        if (variable_get('devconnect_paging_enabled', FALSE) && method_exists($company_app_api, 'usePaging')) {
          $company_app_api->usePaging();
        }
        try {
          if (array_key_exists('name', $conditions)) {
            $company_app_api->load($conditions['name']);
            $app_list += array($company_app_api);
          }
          else {
            // Fetch all apps in the org.
            $app_list += $company_app_api->getListDetail();
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
    $include_debug_data = (count($app_list) == 1);

    foreach ($app_list as $app) {
      $array = $app->toArray($include_debug_data);
      $array['orgName'] = $app->getConfig()->orgName;
      $app_entity = new ApigeeCompanyAppEntity($array);
      $app_entity->checkStatusChange();
      $app_entities[$app->getAppId()] = $app_entity;
    }
    return $app_entities;
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
        // If entity is in our cache, we can make one fewer server roundtrips.
        if (array_key_exists($id, $this->appCache)) {
          $dev_app = $this->appCache[$id];
          unset($this->appCache[$id]);
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
            $dev_app->delete();
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

}
