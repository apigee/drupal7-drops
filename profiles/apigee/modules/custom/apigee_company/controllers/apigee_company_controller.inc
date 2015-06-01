<?php
/**
 * @file
 * The controller class for the Apigee Company Entity.
 */

use Apigee\ManagementAPI\Company;
use Apigee\Exceptions\ResponseException;

class ApigeeCompanyController implements DrupalEntityControllerInterface, EntityAPIControllerInterface {

  /**
   * Entity type for this controller instance.
   *
   * @var string
   */
  protected $entityType;
  protected $entityInfo;

  /**
   * @var \Exception
   */
  protected static $lastException;

  /**
   * Get the org config.
   *
   * @return array
   *   The org configurations
   */
  protected static function getConfig() {
    return devconnect_default_org_config();
  }

  /**
   * {@inheritdoc}
   */
  public function __construct($entity_type) {
    $this->entityInfo = entity_get_info($entity_type);
    $this->entityType = $entity_type;
  }

  /**
   * {@inheritdoc}
   */
  public function create(array $values = array()) {
    // Add is_new property if it is not set.
    $values += array('is_new' => TRUE);

    // If there is a class for this entity type, instantiate it now.
    if (isset($this->entityInfo['entity class']) && $class = $this->entityInfo['entity class']) {
      $entity = new $class($values, $this->entityType);
    }
    else {
      // Otherwise use a good old stdClass.
      $entity = (object) $values;
    }

    // Allow other modules to alter the created entity.
    drupal_alter('apigee_company_entity_create', $this->entityType, $entity);

    return $entity;
  }

  /**
   * {@inheritdoc}
   */
  public function save($entity) {
    $config = self::getConfig();
    $company = new Company($config);

    // If the company createdAt field is not null, this is an update.
    $is_update = !empty($entity->createdAt);

    $attributes = $entity->attributes;
    if (!empty($attributes)) {
      foreach ($entity->attributes as $key => $value) {
        $company->setAttribute($key, $value);
      }
    }
    $company->setDisplayName($entity->displayName);
    $company->setName($entity->name);
    $company->setStatus($entity->status);

    try {
      $company->save($is_update);
    }
    catch (ResponseException $e) {
      self::$lastException = $e;
      return FALSE;
    }

    return ($is_update ? SAVED_UPDATED : SAVED_NEW);
  }

  /**
   * {@inheritdoc}
   */
  public function load($ids = array(), $conditions = array()) {
    $entities = array();

    if (!empty($ids)) {
      $config = self::getConfig();
      $company = new Company($config);
      foreach ($ids as $id) {
        $company->load($id);
        // $entities[$company->getName()] = $company;

        $array = $company->toArray();
        $entity = new ApigeeCompanyEntity($array);
        $entity->developers = $company->listDevelopers();
        $entities[$company->getName()] = $entity;
      }
      return $entities;
    }

    if (empty($ids) && empty($conditions)) {
      // Load all companies for this org.
      $config = self::getConfig();
      $company = new Company($config);
      $companies = $company->listCompaniesDetail();
      foreach ($companies as $company) {
        $array = $company->toArray();
        $entity = new ApigeeCompanyEntity($array);
        $entity->developers = $company->listDevelopers();
        $entities[$company->getName()] = $entity;
      }
      return $entities;
    }
  }

  /**
   * Updates a developer in a company.
   *
   * @param string $company_name
   *   The company machine name.
   * @param string $developer_email
   *   The developers email.
   * @param string $developer_role
   *   The developer role.
   *
   * @return bool
   *   TRUE if the developer was updated successfull else FALSE.
   */
  public function updateDeveloper($company_name, $developer_email, $developer_role) {

    $config = self::getConfig();
    $company = new Company($config);

    try {
      $company->updateDeveloper($developer_email, $developer_role, $company_name);
    }
    catch (ResponseException $e) {
      self::$lastException = $e;
      return FALSE;
    }
    return TRUE;
  }

  /**
   * Remove a developer from a company.
   *
   * @param string $developer_email
   *   The email id of the developer to be removed from the company.
   * @param string $company_name
   *   The machine name of the company from which to remove the developer.
   *
   * @return bool
   *   TRUE if the developer was removed successfully, else FALSE.
   */
  public function removeDeveloper($developer_email, $company_name = NULL) {
    $company_name = $company_name ?: $this->name;
    $config = self::getConfig();
    $company = new Company($config);

    try {
      $company->removeDeveloper($developer_email, $company_name);
    }
    catch (ResponseException $e) {
      self::$lastException = $e;
      return FALSE;
    }
    return TRUE;
  }

  /**
   * Fetch the roles of a developer in a company.
   *
   * @param string $developer_email
   *   The email id of the developer whose roles are to be retrieved.
   * @param string $company_name
   *   The machine name of the company from which to fetch the developer roles.
   *
   * @return array
   *   An array of developer roles, returns false in case of failure.
   */
  public function getDeveloperRoles($developer_email, $company_name) {
    $config = self::getConfig();
    $company = new Company($config);
    try{
      $developer_roles = $company->getDeveloperRoles($developer_email, $company_name);
    }
    catch(ResponseException $e){
      self::$lastException = $e;
      return FALSE;
    }
    return $developer_roles;
  }

  /**
   * {@inheritdoc}
   */
  public function resetCache(array $ids = NULL) {

  }

  /**
   * {@inheritdoc}
   */
  public function delete($ids) {
    // @TODO Implement the logic to delete a company.
  }

  /**
   * {@inheritdoc}
   */
  public function invoke($hook, $entity) {

  }

  /**
   * {@inheritdoc}
   */
  public function export($entity, $prefix = '') {

  }

  /**
   * {@inheritdoc}
   */
  public function import($export) {

  }

  /**
   * {@inheritdoc}
   */
  public function buildContent($entity, $view_mode = 'full', $langcode = NULL) {

  }

  /**
   * {@inheritdoc}
   */
  public function view($entities, $view_mode = 'full', $langcode = NULL, $page = NULL) {

  }

  /**
   * Returns the last exception returned from Edge.
   *
   * @return Apigee\Exceptions\ResponseException
   *   The last exception that occured.
   */
  public static function getLastException() {
    return self::$lastException;
  }

}
