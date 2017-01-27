<?php

namespace Drupal\devconnect_developer_apps;

class DeveloperAppEntity {
  /**
   * @var string
   */
  public $orgName = '';
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
   * List of raw data for all credentials/keys, whether active or not.
   */
  public $credentials;
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
   * @var int
   */
  public $credentialIssuedAt = -1;
  /**
   * @var int
   */
  public $credentialExpiresAt = -1;

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
   * @var int
   */
  public $keyExpiresIn = -1;

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
   * @param mixed $status
   * @return bool
   */
  public function setKeyStatus($status) {
    return \DeveloperAppController::setKeyStatus($this, $status);
  }

  public function __construct(array $values = array()) {
    // Populate values if available.
    foreach ($values as $key => $value) {
      if (property_exists($this, $key)) {
        $this->$key = $value;
      }
    }
  }

  /**
   * Returns a named attribute, if it exists; else returns null.
   *
   * @param string $name
   * @return string|null
   */
  public function getAttribute($name) {
    if (array_key_exists($name, $this->attributes)) {
      return $this->attributes[$name];
    }
    return NULL;
  }

  /**
   * Returns a named credential attribute, if it exists; else returns null.
   *
   * @param string $name
   * @return string|null
   */
  public function getCredentialAttribute($name) {
    if (array_key_exists($name, $this->credentialAttributes)) {
      return $this->credentialAttributes[$name];
    }
    return NULL;
  }

  /**
   * Deletes a named attribute from the app. Returns TRUE if successful, else
   * FALSE. If Edge SDK is not recent enough to support this functionality,
   * FALSE will consistently be returned.
   *
   * @param string $name
   * @return bool
   */
  public function deleteAttribute($name) {
    return \DeveloperAppController::deleteAttribute($this, $name);
  }

  /**
   * Deletes a named attribute from the credential. Returns TRUE if successful,
   * else FALSE. If Edge SDK is not recent enough to support this
   * functionality, FALSE will consistently be returned.
   *
   * @param string $name
   * @return bool
   */
  public function deleteCredentialAttribute($name) {
    return \DeveloperAppController::deleteCredentialAttribute($this, $name);
  }

  /**
   * Checks to see if overall status has changed.
   *
   * If status has changed and the rules module is enabled, fires the
   * appropriate event.
   */
  public function checkStatusChange() {
    if (empty($this->appId) || empty($this->orgName)) {
      return;
    }
    $new_status = self::getNumericStatus($this->overallStatus);
    $previous_status = db_select('dc_dev_app_previous_status', 'd')
      ->fields('d', array('status'))
      ->condition('app_id', $this->appId)
      ->execute()
      ->fetchField();
    if ($previous_status === FALSE) {
      if (isset($new_status)) {
        db_insert('dc_dev_app_previous_status')
          ->fields(array('app_id' => $this->appId, 'org_name' => $this->orgName, 'status' => $new_status))
          ->execute();
      }
    }
    elseif ($new_status != $previous_status) {
      db_update('dc_dev_app_previous_status')
        ->fields(array('status' => $new_status))
        ->condition('app_id', $this->appId)
        ->execute();
      if (module_exists('rules')) {
        $event = NULL;
        switch ($previous_status) {
          case -1:
            $event = 'revoked_';
            break;
          case 0:
            $event = 'pending_';
            break;
          case 1:
            $event = 'approved_';
            break;
        }
        $event .= $this->overallStatus;

        if ($event && strlen($event) > 10) {
          rules_invoke_event('devconnect_developer_app_status_' . $event, $this);
        }
      }
    }
  }

  /**
   * Translates a text status into a numeric representation.
   *
   * @param $text_status
   * @return int|null
   */
  protected static function getNumericStatus($text_status) {
    switch ($text_status) {
      case 'revoked':
        $new_status = -1;
        break;
      case 'pending':
        $new_status = 0;
        break;
      case 'approved':
        $new_status = 1;
        break;
      default:
        $new_status = NULL;
    }
    return $new_status;
  }

}