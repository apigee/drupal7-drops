<?php
/**
 * @file
 * Controller class for Apigee Company Invitation Entity.
 */

use Apigee\ManagementAPI\CompanyInvitation;
use Apigee\Exceptions\ResponseException;

class ApigeeCompanyInvitationController implements DrupalEntityControllerInterface, EntityAPIControllerInterface {

  /**
   * Entity type.
   *
   * @var string
   */
  protected $entityType;

  /**
   * The entity info.
   *
   * @var array.
   */
  protected $entityInfo;

  /**
   *
   * @var \Exception
   */
  protected static $lastException;

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
  public function resetCache(array $ids = NULL) {

  }

  /**
   * {@inheritdoc}
   */
  public function load($ids = array(), $conditions = array()) {
    $entities = array();
    if (!empty($ids)) {
      $config = self::getConfig();
      $invitation = new CompanyInvitation($config);
      foreach ($ids as $id) {
        $invitation->load($id);
        $invitation_array = $invitation->toArray();
        $entity = new ApigeeCompanyInvitationEntity($invitation_array);
        $entities[$entity->id] = $entity;
      }
      return $entities;
    }

    $config = self::getConfig();
    $invitation = new CompanyInvitation($config);
    $invitations = array();
    if (array_key_exists('developerId', $conditions)) {
      $state = array_key_exists('state', $conditions) ? $conditions['state'] : NULL;
      $invitations = $invitation->getAllInvitationsForDeveloper($conditions['developerId'], $state);
    }
    elseif (array_key_exists('companyId', $conditions)) {
      $state = array_key_exists('state', $conditions) ? $conditions['state'] : NULL;
      $invitations = $invitation->getAllInvitationsForCompany($conditions['companyId'], $state);
    }
    else {
      $invitations = $invitation->getAllInvitationsForOrg();
    }

    if (!empty($invitations)) {
      foreach ($invitations as $invite) {
        if ($invite instanceof \Apigee\ManagementAPI\CompanyInvitation) {
          $array = $invite->toArray();
          $entity = new ApigeeCompanyInvitationEntity($array);
          $entities[$entity->id] = $entity;
        }
      }
    }
    return $entities;
  }

  /**
   * {@inheritdoc}
   */
  public function save($entity) {
    $config = self::getConfig();
    $invitation = new CompanyInvitation($config);
    $invitation->setCompanyId($entity->companyId);
    $invitation->setDeveloperId($entity->developerId);
    $invitation->setRequestor($entity->requestor);
    $invitation->setType($entity->type);
    $invitation->setSourceDeveloperEmail($entity->sourceDeveloperEmail);
    $invitation->setResponseUrl($entity->responseUrl);
    if (isset($entity->id)) {
      $invitation->setId($entity->id);
    }
    if (isset($entity->state)) {
      $invitation->setSate($entity->state);
    }

    $is_update = FALSE;
    if (!empty($entity->id)) {
      $is_update = TRUE;
    }
    try {
      $invitation->save($is_update);
    }
    catch (ResponseException $e) {
      self::$lastException = $e;
      return FALSE;
    }
    $entity->id = $invitation->getId();
    $entity->created_at = $invitation->getCreatedAt();
    $entity->lastmodifed_at = $invitation->getLastModifedAt();
    $entity->state = $invitation->getState();
    $entity->orgId = $invitation->getOrgId();

    return ($is_update ? SAVED_UPDATED : SAVED_NEW);
  }

  /**
   * {@inheritdoc}
   */
  public function delete($ids) {
    global $user;
    foreach ($ids as $id) {
      $config = self::getConfig();
      $invitation = new CompanyInvitation($config);
      $invitation->setSourceDeveloperEmail($user->mail);
      try{
        $invitation->delete($id);
      }
      catch (ResponseException $e){
        self::$lastException = $e;
        return FALSE;
      }
    }
    return TRUE;
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
   * {@inheritdoc}
   */
  protected static function getConfig() {
    return devconnect_default_org_config();
  }

  /**
   * Returns the last exception returned from Edge.
   *
   * @return Apigee\Exceptions\ResponseException
   *   The last exception.
   */
  public static function getLastException() {
    return self::$lastException;
  }

}
