<?php

namespace Drupal\devconnect_developer_apps;
use \Drupal\devconnect\ArrayEntity;

class DeveloperAppEntity extends ArrayEntity {
  /**
   * @var string
   */
  public $accessType = '';
  /**
   * @var array
   */
  public $apiProducts = array();
  /**
   * @var string.
   */
  public $appFamily = '';
  /**
   * @var string
   * UUID of this app.
   */
  public $appId = '';
  /**
   * @var array
   */
  public $attributes = array();
  /**
   * @var string
   */
  public $callbackUrl = '';
  /**
   * @var int
   */
  public $createdAt = 0;
  /**
   * @var string
   */
  public $createdBy = '';
  /**
   * @var int
   */
  public $modifiedAt = 0;
  /**
   * @var string
   */
  public $modifiedBy = '';
  /**
   * @var string
   * Corresponds to the developerId attribute of the developer who owns this
   * app.
   */
  public $developerId = '';
  /**
   * @var string
   * Primary key (within this org/developer's app list)
   */
  public $name = '';
  /**
   * @var array
   * The purpose of this field remains unknown.
   */
  public $scopes = array();
  /**
   * @var string
   * There is probably a finite number of possible values, but I haven't found
   * a definitive list yet.
   */
  public $status = '';
  /**
   * @var string
   */
  public $description = '';

  /**
   * @var array
   * Each member of this array is itself an associative array, with keys of
   * 'apiproduct' and 'status'.
   */
  public $credentialApiProducts = array();
  /**
   * @var string
   */
  public $consumerKey = '';
  /**
   * @var string
   */
  public $consumerSecret = '';
  /**
   * @var array
   * The purpose of this field is unknown at this time.
   */
  public $credentialScopes = array();
  /**
   * @var string
   */
  public $credentialStatus = '';
  /**
   * @var array
   */
  public $credentialAttributes = array();

  /**
   * @var string
   * Email of the developer who owns this app.
   */
  public $developer = '';
  /**
   * @var array
   */
  public $debugData = array();
  /**
   * @var string
   */
  public $overallStatus = '';

  /**
   * @var int
   */
  public $uid = 0;

  /**
   * @var array
   */
  public $apiProductCache = array();

  /**
   * Calls the entity controller to do the saving, then copies resulting field
   * values back to the incoming entity.
   *
   * @return int|bool
   */
  public function save() {
    $saved = entity_get_controller('developer_app')->save($this);
    if ($saved) {
      $app = \DeveloperAppController::getLastApp();
      foreach ($app as $key => $value) {
        $this->$key = $value;
      }
    }
    return $saved;
  }

  /**
   * Calls the entity controller to set the key status. This here is a
   * convenience function.
   *
   * Returns TRUE if successful, else FALSE.
   *
   * @param $status
   * @return bool
   */
  public function setKeyStatus($status) {
    return \DeveloperAppController::setKeyStatus($this, $status);
  }
}