<?php

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

  protected static function getConfig() {
    return devconnect_default_org_config();
  }

  public function __construct($entity_type) {
    $this->entityInfo = entity_get_info($entity_type);
    $this->entityType = $entity_type;
  }

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

  public function load($ids = array(), $conditions = array()) {
    $entities = array();

    if (!empty($ids)) {
      $config = self::getConfig();
      $company = new Company($config);
      foreach ($ids as $id) {
        $company->load($id);
//        $entities[$company->getName()] = $company;

        $array = $company->toArray();
        $entity = new ApigeeCompanyEntity($array);
        $entity->developers = $company->listDevelopers();
        $entities[$company->getName()] = $entity;
      }
      return $entities;
    }

    if (empty($ids) && empty($conditions)) {
      // Load all companies for this org
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


  public function removeDeveloper($developer_email, $company_name = NULL) {
    $company_name = $company_name? : $this->name;
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

  public function getDeveloperRoles($developer_email, $company_name){
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

  public function resetCache(array $ids = NULL) {

  }

  public function delete($ids) {
    //@todo Implement the logic to delete a company
  }

  public function invoke($hook, $entity) {

  }

  public function export($entity, $prefix = '') {

  }

  public function import($export) {

  }

  public function buildContent($entity, $view_mode = 'full', $langcode = NULL) {

  }

  public function view($entities, $view_mode = 'full', $langcode = NULL, $page = NULL) {

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

?>
