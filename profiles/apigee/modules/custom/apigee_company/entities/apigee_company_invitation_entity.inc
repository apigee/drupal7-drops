<?php

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
  
  public $responseUrl ='';


  public function __construct($values) {
    // Set initial values.
    foreach ($values as $key => $value) {
      $this->$key = $value;
    }
  }
  
  public function save(){
    $saved = entity_get_controller('apigee_company_invitation')->save($this);
    return $saved;
  }
  
}

?>
