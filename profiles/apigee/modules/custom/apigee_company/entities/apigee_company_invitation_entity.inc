<?php
/**
 * @file
 * Entity Class for the Apigee Company Invitation Entity.
 */

class ApigeeCompanyInvitationEntity {

  /**
   * Company ID.
   * @var string
   */
  public $companyId = '';

  /**
   * Created date.
   * @var string
   */
  public $created_at = '';

  /**
   * Developer ID.
   * @var string
   */
  public $developerId = '';

  /**
   * Unique identifier representing a specific request.
   *
   * @var string
   */
  public $id = '';

  /**
   * Last updated date.
   * @var string
   */
  public $lastmodified_at = '';

  /**
   * The org id
   * @var string
   */
  public $orgId = '';

  /**
   * The entity that initiated the request.
   * @var string
   */
  public $requestor = '';

  /**
   * The current state of the request.
   * @var string
   */
  public $state = '';

  /**
   * The type of request.
   * @var string
   */
  public $type = '';


  /**
   *The source developer email address.
   * @var string
   */
  public $sourceDeveloperEmail = '';

  /**
   *The response Url to be sent in the email to the developer.
   * @var string
   */
  public $responseUrl = '';


  /**
   * Class Constructor.
   *
   * @param array $values
   *   Associative array with keys as the class properties and values as their
   *   values
   */
  public function __construct($values) {
    // Set initial values.
    foreach ($values as $key => $value) {
      $this->$key = $value;
    }
  }
  /**
   * Save callback for entity_save.
   *
   * @return int
   *   SAVED_NEW=1 if a new entity was saved or SAVED_UPDATED=2 if an entity
   *   was updated.
   */
  public function save() {
    $saved = entity_get_controller('apigee_company_invitation')->save($this);
    return $saved;
  }

}
