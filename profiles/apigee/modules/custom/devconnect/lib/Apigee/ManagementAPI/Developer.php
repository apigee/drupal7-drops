<?php
namespace Apigee\ManagementAPI;

use \Apigee\Exceptions\ResponseException as ResponseException;
/**
 * @file
 * Abstracts the Developer object in the Management API and allows clients to
 * manipulate it.
 *
 * @author djohnson
 */


class Developer extends Base {

  /**
   * @var array
   */
  private $apps;
  /**
   * @var string
   * This is actually the unique-key (within the org) for the Developer
   */
  private $email;
  /**
   * @var string
   * Read-only
   */
  private $developer_id;
  /**
   * @var string
   */
  private $first_name;
  /**
   * @var string
   */
  private $last_name;
  /**
   * @var string
   */
  private $user_name;
  /**
   * @var string
   * Read-only
   */
  private $org_name;
  /**
   * @var string
   * Should be either 'active' or 'inactive'.
   */
  private $status;
  /**
   * @var array
   * This must be protected because Base wants to twiddle with it.
   */
  protected $attributes;
  /**
   * @var int
   * Read-only
   */
  private $created_at;
  /**
   * @var string
   * Read-only
   */
  private $created_by;
  /**
   * @var int
   * Read-only
   */
  private $modified_at;
  /**
   * @var string
   * Read-only
   */
  private $modified_by;

  /* Accessors (getters/setters) */
  public function get_apps() {
    return $this->apps;
  }
  public function get_email() {
    return $this->email;
  }
  public function set_email($email) {
    $this->email = $email;
  }
  public function get_developer_id() {
    return $this->developer_id;
  }
  public function get_first_name() {
    return $this->first_name;
  }
  public function set_first_name($fname) {
    $this->first_name = $fname;
  }
  public function get_last_name() {
    return $this->last_name();
  }
  public function set_last_name($lname) {
    $this->last_name = $lname;
  }
  public function get_user_name() {
    return $this->user_name;
  }
  public function set_user_name($uname) {
    $this->user_name = $uname;
  }
  public function get_status() {
    return $this->status;
  }
  public function set_status($status) {
    if ($status === 0 || $status === FALSE) {
      $status = 'inactive';
    }
    elseif ($status === 1 || $status === TRUE) {
      $status = 'active';
    }
    if ($status != 'active' && $status != 'inactive') {
      throw new \Apigee\Exceptions\InvalidDataException('Status may be either active or inactive; value "' . $status . '" is invalid.');
    }
    $this->status = $status;
  }
  public function get_attribute($attr) {
    if (array_key_exists($attr, $this->attributes)) {
      return $this->attributes[$attr];
    }
    return NULL;
  }
  public function set_attribute($attr, $value) {
    $this->attributes[$attr] = $value;
  }
  public function get_attributes() {
    return $this->attributes;
  }
  public function get_modified_at() {
    return $this->modified_at;
  }

  /**
   * Initializes default values of all member variables.
   *
   * @param \Apigee\Util\APIClient $client
   * @param bool $double_escape_urls
   */
  public function __construct(\Apigee\Util\APIClient $client, $double_escape_urls = FALSE) {
    $this->init($client, $double_escape_urls);
    $this->base_url = '/organizations/' . $this->url_encode($client->get_org()) . '/developers';
    $this->blank_values();
  }

  /**
   * Loads a developer from the Management API using $email as the unique key.
   *
   * @param $email
   */
  public function load($email) {
    $url = $this->base_url . '/' . $this->url_encode($email);
    $this->client->get($url);
    $developer = $this->get_response();
    $this->apps = $developer['apps'];
    $this->email = $developer['email'];
    $this->developer_id = $developer['developerId'];
    $this->first_name = $developer['firstName'];
    $this->last_name = $developer['lastName'];
    $this->user_name = $developer['userName'];
    $this->org_name = $developer['orgName'];
    $this->status = $developer['status'];
    $this->attributes = $developer['attributes'];
    $this->created_at = $developer['createdAt'];
    $this->created_by = $developer['createdBy'];
    $this->modified_at = $developer['lastModifiedAt'];
    $this->modified_by = $developer['lastModifiedBy'];
  }

  /**
   * Attempts to load developer from Management API. Returns TRUE if load was
   * successful.
   *
   * If $email is not supplied, the result will always be FALSE.
   *
   * @param null|string $email
   * @return bool
   */
  public function validate($email = NULL) {
    if (isset($email)) {
      $url = $this->base_url . '/' . $this->url_encode($email);
      try {
        $this->client->get($url);
        return $this->client->was_successful();
      }
      catch (ResponseException $e) { }
    }
    return FALSE;
  }

  /**
   * Saves user data to the Management API. This operates as both insert and
   * update.
   *
   * If user's email doesn't look valid (must contain @), an
   * InvalidDataException is thrown.
   *
   * @throws \Apigee\Exceptions\InvalidDataException
   */
  public function save($force_update = FALSE) {
    if (!$this->validate_user()) {
      throw new \Apigee\Exceptions\InvalidDataException('Invalid email address; cannot save user.');
    }

    $payload = array(
      'email' => $this->email,
      'userName' => $this->user_name,
      'firstName' => $this->first_name,
      'lastName' => $this->last_name,
      'userName' => $this->user_name,
      'status' => $this->status,
    );
    if (count($this->attributes) > 0) {
      $payload['attributes'] = array();
      foreach ($this->attributes as $name => $value) {
        $payload['attributes'][] = array('name' => $name, 'value' => $value);
      }
    }
    $url = $this->base_url;
    if ($force_update || $this->created_at) {
      if ($this->developer_id) {
        $payload['developerId'] = $this->developer_id;
      }
      $url .= '/' . $this->url_encode($this->email);
    }
    $this->client->post($url, $payload);
    $this->get_response();
  }

  /**
   * Deletes a developer.
   *
   * If $email is not supplied, $this->email is used.
   *
   * @param null|string $email
   */
  public function delete($email = NULL) {
    if (!isset($email)) {
      $email = $this->email;
    }
    $this->client->delete($this->base_url . '/' . $this->url_encode($email));
    $this->get_response();
    if ($email == $this->email) {
      $this->blank_values();
    }
  }

  /**
   * Returns an array of all developer emails for this org.
   *
   * @return array
   */
  public function list_developers() {
    $this->client->get($this->base_url);
    $developers = $this->get_response();
    return $developers;
  }

  /**
   * Ensures that current developer's email looks at least sort of valid.
   *
   * If first name and/or last name are not supplied, they are auto-
   * populated based on email. This is kind of shoddy but it's the best we can
   * do.
   *
   * @return bool
   */
  public function validate_user() {
    if (!empty($this->email) && (strpos($this->email, '@') > 0)) {
      $name = explode('@', $this->email, 2);
      if (empty($this->first_name)) {
        $this->first_name = $name[0];
      }
      if (empty($this->last_name)) {
        $this->last_name = $name[1];
      }
      return TRUE;
    }
    else {
      return FALSE;
    }
  }

  /**
   * Populates this object's properties based on a Drupal user object.
   *
   * Be aware that previous properties are not blanked first. If you are
   * creating a new user, you may want to call $this->blank_values() first.
   *
   * @param $account
   */
  public function populate_from_user_account($account) {
    $this->email = $account->mail;
    $this->first_name = $account->field_first_name[LANGUAGE_NONE][0]['value'];
    $this->last_name = $account->field_last_name[LANGUAGE_NONE][0]['value'];
    $this->user_name = $account->name;
    $this->status = ($account->status ? 'active' : 'inactive');

    $vars = get_object_vars($account);
    foreach ($vars as $key => $value) {
      if (substr($key, 0, 10) == 'attribute_') {
        $this->attributes[substr($key, 10)] = $value;
      }
    }
  }

  /**
   * Restores this object's properties to their pristine state.
   */
  public function blank_values() {
    $this->apps = array();
    $this->email = NULL;
    $this->developer_id = NULL;
    $this->first_name = NULL;
    $this->last_name = NULL;
    $this->user_name = NULL;
    $this->org_name = NULL;
    $this->status = NULL;
    $this->attributes = array();
    $this->created_at = NULL;
    $this->created_by = NULL;
    $this->modified_at = NULL;
    $this->modified_by = NULL;
  }
}