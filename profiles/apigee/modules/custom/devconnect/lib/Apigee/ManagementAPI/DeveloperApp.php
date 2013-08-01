<?php
/**
 * @file
 * Abstracts the Developer App object in the Management API and allows clients
 * to manipulate it.
 *
 * @author djohnson
 */

namespace Apigee\ManagementAPI;

use Apigee\Exceptions\InvalidDataException as InvalidDataException;
use Apigee\Exceptions\ResponseException as ResponseException;
use Apigee\Util\APIClient as APIClient;

class DeveloperApp extends Base {

  /**
   * @var string
   * 'read', 'write', or 'both' (empty is also valid). This property doesn't
   * appear to ever be used.
   */
  private $access_type;
  /**
   * @var array
   */
  private $api_products;
  /**
   * @var string.
   * Read-only. Purpose of this field is unknown at this time.
   */
  private $app_family;
  /**
   * @var string
   * Read-only. GUID of this app.
   */
  private $app_id;
  /**
   * @var array
   * This is protected because Base wants to twiddle with it.
   */
  protected $attributes;
  /**
   * @var string
   */
  private $callback_url;
  /**
   * @var int
   * Read-only.
   */
  private $created_at;
  /**
   * @var string
   * Read-only.
   */
  private $created_by;
  /**
   * @var int
   * Read-only.
   */
  private $modified_at;
  /**
   * @var string
   * Read-only.
   */
  private $modified_by;
  /**
   * @var string
   * Read-only. Corresponds to the developer_id attribute of the developer who
   * owns this app.
   */
  private $developer_id;
  /**
   * @var string
   * Primary key (within this org/developer's app list)
   */
  private $name;
  /**
   * @var array
   * The purpose of this field remains unknown.
   */
  private $scopes;
  /**
   * @var string
   * There is probably a finite number of possible values, but I haven't found
   * a definitive list yet.
   */
  private $status;
  /**
   * @var string
   */
  private $description;

  /**
   * @var array
   * Each member of this array is itself an associative array, with keys of
   * 'apiproduct' and 'status'.
   */
  private $credential_apiproducts;
  /**
   * @var string
   */
  private $consumer_key;
  /**
   * @var string
   */
  private $consumer_secret;
  /**
   * @var array
   * The purpose of this field is unknown at this time.
   */
  private $credential_scopes;
  /**
   * @var string
   */
  private $credential_status;
  /**
   * @var array
   */
  private $credential_attributes;

  /**
   * @var string
   */
  private $developer;
  /**
   * @var array
   */
  private $cached_api_products;

  /* Accessors (getters/setters) */
  public function get_api_products() {
    return $this->api_products;
  }
  public function set_api_products($products) {
    if (!is_array($products)) {
      $products = array($products);
    }
    $this->cached_api_products = $this->api_products;
    $this->api_products = $products;
  }
  public function get_attributes() {
    return $this->attributes;
  }
  public function has_attribute($attr) {
    return array_key_exists($attr, $this->attributes);
  }
  public function get_attribute($attr) {
    return (array_key_exists($attr, $this->attributes) ? $this->attributes[$attr] : NULL);
  }
  public function set_attribute($attr, $value) {
    $this->attributes[$attr] = $value;
  }
  public function set_name($name) {
    $this->name = $name;
  }
  public function get_name() {
    return $this->name;
  }
  public function set_callback_url($url) {
    $this->callback_url = $url;
  }
  public function get_callback_url() {
    return $this->callback_url;
  }
  public function set_description($descr) {
    $this->description = $descr;
    $this->attributes['description'] = $descr;
  }
  public function get_description() {
    return $this->description;
  }
  public function set_access_type($type) {
    if ($type != 'read' && $type != 'write' && $type != 'both') {
      throw new InvalidDataException('Invalid access type ' . $type . '.');
    }
    $this->access_type = $type;
  }
  public function get_access_type() {
    return $this->access_type;
  }
  public function get_status() {
    return $this->status;
  }
  public function get_developer_id() {
    return $this->developer_id;
  }

  public function get_credential_api_products() {
    return $this->credential_apiproducts;
  }
  public function get_consumer_key() {
    return $this->consumer_key;
  }
  public function get_consumer_secret() {
    return $this->consumer_secret;
  }
  public function get_credential_scopes() {
    return $this->credential_scopes;
  }
  public function get_credential_status() {
    return $this->credential_status;
  }
  public function get_created_at() {
    return $this->created_at;
  }
  public function get_created_by() {
    return $this->created_by;
  }

  public function has_credential_info() {
    $credential_fields = array('credential_apiproducts', 'consumer_key', 'consumer_secret', 'credential_scopes', 'credential_status');
    foreach ($credential_fields as $field) {
      if (!empty($this->$field)) {
        return TRUE;
      }
    }
    return FALSE;
  }
  // TODO: write other getters/setters


  /**
   * Initializes this object
   *
   * @param \Apigee\Util\APIClient $client
   * @param string $developer
   */
  public function __construct(\Apigee\Util\APIClient $client, $developer) {
    $this->init($client);
    if ($developer instanceof \Apigee\ManagementAPI\Developer) {
      $this->developer = $developer->get_email();
    }
    else {
      // $developer may be either an email or a developerId.
      $this->developer = $developer;
    }
    $this->base_url = '/organizations/' . $this->url_encode($client->get_org()) . '/developers/' . $this->url_encode($this->developer) . '/apps';
    $this->blank_values();
  }

  /**
   * Loads a DeveloperApp object with the contents of a raw Management API
   * response.
   *
   * @static
   * @param DeveloperApp $obj
   * @param array $response
   */
  private static function load_from_response(DeveloperApp &$obj, $response) {
    $obj->access_type = $response['accessType'];
    $obj->app_family = (isset($response['appFamily']) ? $response['appFamily'] : NULL);
    $obj->app_id = $response['appId'];
    $obj->callback_url = $response['callbackUrl'];
    $obj->created_at = $response['createdAt'];
    $obj->created_by = $response['createdBy'];
    $obj->modified_at = $response['lastModifiedAt'];
    $obj->modified_by = $response['lastModifiedBy'];
    $obj->developer_id = $response['developerId'];
    $obj->name = $response['name'];
    $obj->scopes = $response['scopes'];
    $obj->status = $response['status'];

    $obj->read_attributes($response);

    if (!empty($response['description'])) {
      $obj->description = $response['description'];
    }
    elseif (isset($obj->attributes['description'])) {
      $obj->description = $obj->get_attribute('description');
    }
    else {
      $obj->description = NULL;
    }

    self::load_credentials($obj, $response['credentials']);
  }

  /**
   * Reads the credentials array from the API response and sets object
   * properties.
   *
   * @static
   * @param DeveloperApp $obj
   * @param $credentials
   */
  private static function load_credentials(DeveloperApp &$obj, $credentials) {
    // Find the credential with the max create_date attribute.
    if (count($credentials) > 0) {
      $credential = NULL;
      // Sort credentials by create_date descending.
      usort($credentials, array('Apigee\\ManagementAPI\\DeveloperApp', 'sort_credentials'));
      // Look for the first member of the array that is approved.
      foreach ($credentials as $c) {
        if ($c['status'] == 'approved') {
          $credential = $c;
        }
      }
      // If none were approved, use the first member of the array.
      if (!isset($credential)) {
        $credential = $credentials[0];
      }
      $obj->credential_apiproducts = $credential['apiProducts'];
      $obj->consumer_key = $credential['consumerKey'];
      $obj->consumer_secret = $credential['consumerSecret'];
      $obj->credential_scopes = $credential['scopes'];
      $obj->credential_status = $credential['status'];

      $obj->credential_attributes = array();
      foreach ($credential['attributes'] as $attribute) {
        $obj->credential_attributes[$attribute['name']] = $attribute['value'];
      }

      // Some apps may be misconfigured and need to be populated with their apiproducts based on credential.
      if (count($obj->api_products) == 0) {
        foreach ($obj->credential_apiproducts as $product) {
          $obj->api_products[] = $product['apiproduct'];
        }
      }
    }
  }

  /**
   * Populates this object with information retrieved from the Management API.
   *
   * If $name is not passed, $this->name is used.
   *
   * @param null|string $name
   */
  public function load($name = NULL) {
    if (!isset($name)) {
      $name = $this->name;
    }
    $url = $this->base_url . '/' . $this->url_encode($name);
    $this->client->get($url);
    $response = $this->get_response();
    self::load_from_response($this, $response);
  }

  /**
   * Checks to see if an app with the given name exists for this developer.
   *
   * If $name is not passed, $this->name is used.
   *
   * @param null|string $name
   * @return bool
   */
  public function validate($name = NULL) {
    if (!isset($name)) {
      $name = $this->name;
    }
    $url = $this->base_url . '/' . $this->url_encode($name);
    $this->client->get($url);
    return $this->client->was_successful();
  }

  /**
   * Determines difference between cached version of API Products array for
   * this app and current version. Returned object enumerates which API
   * Products are due for removal (if any), and which should be added.
   *
   * @return \stdClass
   */
  private function api_products_diff() {
    // Find apiproducts that we will have to delete.  These are found in the
    // cached list but not in the live list.
    $to_delete = array();
    foreach ($this->cached_api_products as $api_product) {
      if (!in_array($api_product['apiproduct'], $this->api_products)) {
        $to_delete[] = $api_product['apiproduct'];
      }
    }
    // Find apiproducts that we will have to add. These are found in the
    // live list but not in the cached list.
    $to_add = array();
    foreach ($this->api_products as $api_product) {
      if (!in_array($api_product, $this->cached_api_products)) {
        $to_add[] = $api_product;
      }
    }
    return (object)array('to_delete' => $to_delete, 'to_add' => $to_add);
  }

  /**
   * Write this app's data to the Management API, preserving client key/secret.
   *
   * The function attempts to determine if this should be an insert or an
   * update automagically. However, when $force_update is set to TRUE, this
   * determination is short-circuited and an update is assumed.
   *
   * @param bool $force_update
   */
  public function save($force_update = FALSE) {
    $is_update = ($force_update || $this->modified_at);

    $payload = array(
      'accessType' => $this->access_type,
      'name' => $this->name,
      'callbackUrl' => $this->callback_url
    );
    $this->write_attributes($payload);

    $url = $this->base_url;
    if ($is_update) {
      $url .= '/' . $this->url_encode($this->name);
    }
    $created_new_key = FALSE;
    // NOTE: On update, we send APIProduct information separately from other
    // fields, in order to preserve the client-key/secret pair. Updates to
    // APIProducts must be made separately against the app's client-key,
    // rather than just against the app. Additionally, deletions from the
    // APIProducts list must be handled separately from additions.
    if ($is_update && !empty($this->consumer_key)) {
      $key_uri = "$url/keys/" . $this->url_encode($this->consumer_key);
      $diff = $this->api_products_diff();
      // api-product deletions must happen one-by-one.
      foreach ($diff->to_delete as $api_product) {
        $delete_uri = "$key_uri/apiproducts/" . $this->url_encode($api_product);
        $this->client->delete($delete_uri);
        $this->get_response();
      }
      // api-product additions can happen in a batch.
      if (count($diff->to_add) > 0) {
        $this->client->post($key_uri, array('apiProducts' => $diff->to_add));
        $this->get_response();
      }
    }
    else {
      $payload['apiProducts'] = $this->api_products;
      $created_new_key = TRUE;
    }

    $this->client->post($url, $payload);
    $response = $this->get_response();

    // If we created a new key, add a create_date attribute to it.
    if ($created_new_key && count($response['credentials']) > 0) {
      $credentials = $response['credentials'];
      $no_timestamp_index = NULL;
      // Look for the first credential that has no create_date timestamp.
      foreach ($credentials as $i => $cred) {
        $attrs = $cred['attributes'];
        $found_create_date = FALSE;
        foreach ($attrs as $attr) {
          if ($attr['name'] == 'create_date') {
            $found_create_date = TRUE;
            break;
          }
        }
        if (!$found_create_date) {
          $no_timestamp_index = $i;
          break;
        }
      }
      // If all credentials have a create_date timestamp, there's nothing
      // for us to update here.

      if (isset($no_timestamp_index)) {
        // Get reference to array member so we are actually updating $response
        $new_credential =& $response['credentials'][$no_timestamp_index];

        $create_date = time();
        $key = $new_credential['consumerKey'];

        // Set our create_date attribute.
        $new_credential['attributes'][] = array('name' => 'create_date', 'value' => strval($create_date));
        $payload = $new_credential;
        // Payload only has to send bare minimum for update.
        unset($payload['apiProducts'], $payload['scopes'], $payload['status']);
        $url = $this->base_url . '/' . $this->url_encode($this->name) . '/keys/' . $key;
        // POST that sucker!
        $this->client->post($url, $payload);
      }
    }

    // Refresh our fields so we get latest autogenerated data such as consumer key etc.
    self::load_from_response($this, $response);
  }

  /**
   * Usort callback to sort credentials by create date (most recent first).
   *
   * @static
   * @param $a
   * @param $b
   * @return int
   */
  private static function sort_credentials($a, $b) {
    $a_create_date = 0;
    foreach ($a['attributes'] as $attr) {
      if ($attr['name'] == 'create_date') {
        $a_create_date = intval($attr['value']);
        break;
      }
    }
    $b_create_date = 0;
    foreach ($b['attributes'] as $attr) {
      if ($attr['name'] == 'create_date') {
        $b_create_date = intval($attr['value']);
        break;
      }
    }
    if ($a_create_date == $b_create_date) {
      return 0;
    }
    return ($a_create_date > $b_create_date) ? -1 : 1;
  }

  /**
   * Approves or revokes a client key for an app, and optionally also for all
   * API Products associated with that app.
   *
   * @param mixed $status
   *        May be TRUE, FALSE, 0, 1, 'approve' or 'revoke'
   * @param bool $also_set_apiproduct
   * @throws \Apigee\Exceptions\InvalidDataException
   */
  public function set_key_status($status, $also_set_apiproduct = TRUE) {
    if ($status === 0 || $status === FALSE) {
      $status = 'revoke';
    }
    elseif ($status === 1 || $status === TRUE) {
      $status = 'approve';
    }
    elseif ($status != 'revoke' && $status != 'approve') {
      throw new InvalidDataException('Invalid key status ' . $status);
    }

    if (empty($this->name)) {
      throw new InvalidDataException('No app specified; cannot set key status.');
    }
    if (empty($this->consumer_key)) {
      throw new InvalidDataException('App has no consumer key; cannot set key status.');
    }
    $base_url = $this->base_url . '/' . $this->url_encode($this->name) . '/keys/' . $this->url_encode($this->consumer_key);
    // First, approve or revoke the overall key for the app.
    $app_url = $base_url . '?action=' . $status;
    $this->client->post($app_url, '');
    $this->get_response();

    // Now, unless specified otherwise, approve or revoke the same key for all
    // associated API Products.
    if ($also_set_apiproduct && !empty($this->api_products)) {
      foreach ($this->api_products as $api_product) {
        $product_url = $base_url . '/apiproducts/' . $this->url_encode($api_product) . '?action=' . $status;
        $this->client->post($product_url, '');
        $this->get_response();
      }
    }
  }

  /**
   * Deletes a developer app from the Management API.
   *
   * If $name is not passed, $this->name is used.
   *
   * @param null|string $name
   */
  public function delete($name = NULL) {
    if (!isset($name)) {
      $name = $this->name;
    }
    $this->client->delete($this->base_url . '/' . $this->url_encode($name));
    $this->get_response();
    if ($name == $this->name) {
      $this->blank_values();
    }
  }

  /**
   * Returns names of all apps belonging to this developer.
   *
   * @return array
   */
  public function get_list() {
    $this->client->get($this->base_url);
    return $this->get_response();
  }

  /**
   * Returns array of all DeveloperApp objects belonging to this developer.
   *
   * @return array
   */
  public function get_list_detail($developer = NULL) {
    if (!isset($developer)) {
      $developer = $this->developer;
    }
    $url = '/organizations/' . $this->url_encode($this->client->get_org()) . '/developers/' . $this->url_encode($developer) . '/apps?expand=true';
    $this->client->get($url);
    $list = $this->get_response();
    $app_list = array();
    if (!array_key_exists('app', $list) || empty($list['app'])) {
      return $app_list;
    }
    foreach ($list['app'] as $response) {
      $app = new DeveloperApp($this->client, $developer);
      self::load_from_response($app, $response);
      $app_list[] = $app;
    }
    return $app_list;
  }

  // public function make_authenticated_call() was removed by Daniel on
  // 2-Apr-2013.  If you really want to see it, look for earlier git commits.

  /**
   * Accepts a Drupal $form_state['values'] array and populates the current
   * DeveloperApp object with the relevant information.
   *
   * @param array $form_values
   */
  public function populate_from_form_values($form_values) {

    if (isset($form_values['api_product'])) {
      $api_products = array();
      if (is_array($form_values['api_product'])) {
        foreach ($form_values['api_product'] as $key => $value) {
          if ($value) {
            $api_products[] = str_replace('prod-', '', $key);
          }
        }
      }
      else {
        // Allow customized sites to declare api_product as non-multiple.
        // This results in a scalar value rather than an array.
        $api_products[] = str_replace('prod-', '', $form_values['api_product']);
      }
    }
    else {
      $api_products = NULL;
    }
    // cgalindo - cache preexisting_api_products
    $this->cached_api_products = $form_values['preexisting_api_products'];
    $this->access_type = isset($form_values['access_type']) ? $form_values['access_type'] : '';
    $this->callback_url = isset($form_values['callback_url']) ? $form_values['callback_url'] : '';
    $this->name = $form_values['machine_name'];
    $this->consumer_key = $form_values['client_key'];
    if (isset($api_products)) {
      $this->api_products = $api_products;
    }
    $attributes = array();
    foreach($form_values as $key => $value) {
      if (substr($key, 0, 10) == 'attribute_') {
        $attributes[substr($key, 10)] = $value;
      }
    }
    if (count($attributes) > 0) {
      $this->attributes = $attributes;
    }

  }

  /**
   * Creates a key/secret pair for this app against its component APIProducts.
   *
   * @todo Find out if we need to individually set the key on each APIProduct.
   *
   * @param string $consumer_key
   * @param string $consumer_secret
   * @throws \Apigee\Exceptions\InvalidDataException
   */
  public function create_key($consumer_key, $consumer_secret) {
    if (strlen($consumer_key) < 5 || strlen($consumer_secret) < 5) {
      throw new InvalidDataException('Consumer Key and Consumer Secret must both be at least 5 characters long.');
    }
    // This is by nature a two-step process. API Products cannot be added
    // to a new key at the time of key creation, for some reason.
    $create_date = strval(time());
    $payload = array(
      'attributes' => array(array('name' => 'create_date', 'value' => $create_date)),
      'consumerKey' => $consumer_key,
      'consumerSecret' => $consumer_secret,
      'scopes' => $this->credential_scopes,
    );

    $url = $this->base_url . '/' . $this->url_encode($this->name) . '/keys/create';
    $this->client->post($url, $payload);

    $new_credential = $this->get_response();
    // We now have the new key, sans apiproducts. Let us add them now.
    $new_credential['apiProducts'] = $this->credential_apiproducts;
    $key = $new_credential['consumerKey'];
    $url = $this->base_url . '/' . $this->url_encode($this->name) . '/keys/' . $this->url_encode($key);
    $this->client->post($url, $new_credential);
    // The following line may throw an exception if the POST was unsuccessful
    // (e.g. consumer_key already exists, etc.)
    $credential = $this->get_response();

    if ($credential['status'] == 'approved' || empty($this->consumer_key)) {
      // Update $this with new credential info ONLY if the key is auto-approved
      // or if there are no keys yet.
      $this->credential_apiproducts = $credential['apiProducts'];
      $this->consumer_key = $credential['consumerKey'];
      $this->consumer_secret = $credential['consumerSecret'];
      $this->credential_scopes = $credential['scopes'];
      $this->credential_status = $credential['status'];

      $this->credential_attributes = array();
      foreach ($credential['attributes'] as $attribute) {
        $this->credential_attributes[$attribute['name']] = $attribute['value'];
      }
    }
  }

  /**
   * Deletes a given key from a developer app.
   *
   * @param string $consumer_key
   */
  public function delete_key($consumer_key) {
    $url = $this->base_url . '/' . $this->url_encode($this->name) . '/keys/' . $this->url_encode($consumer_key);
    $this->client->delete($url);
    // We ignore whether or not the delete was successful. Either way, we can
    // be sure it doesn't exist now, if it did before.

    // Reload app to repopulate credential fields.
    $this->load();
  }

  /**
   * Lists all apps within the org. Each member of the returned array is a
   * fully-populated DeveloperApp product.
   *
   * @return array
   */
  public function list_all_org_apps() {
    $url = '/organizations/' . $this->url_encode($this->get_client()->get_org()) . '/apps?expand=true';
    $this->client->get($url);
    $response = $this->get_response();
    $app_list = array();
    foreach ($response['app'] as $app_detail) {
      $developer = $app_detail['developerId'];
      $app = new DeveloperApp($this->client, $developer);
      self::load_from_response($app, $app_detail);
      $app_list[] = $app;
    }
    return $app_list;
  }

  /**
   * Restores this object to its pristine state.
   */
  public function blank_values() {
    $this->access_type = 'read';
    $this->api_products = array();
    $this->app_family = NULL;
    $this->app_id = NULL;
    $this->attributes = array();
    $this->callback_url = NULL;
    $this->created_at = NULL;
    $this->created_by = NULL;
    $this->modified_at = NULL;
    $this->modified_by = NULL;
    $this->developer_id = NULL;
    $this->name = NULL;
    $this->scopes = array();
    $this->status = 'pending';
    $this->description = NULL;

    $this->credential_apiproducts = array();
    $this->consumer_key = NULL;
    $this->consumer_secret = NULL;
    $this->credential_scopes = array();
    $this->credential_status = NULL;

    $this->cached_api_products = array();
  }
}
