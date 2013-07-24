<?php
namespace Apigee\ManagementAPI;

/**
 * @file
 * Abstracts the Developer App object in the Management API and allows clients
 * to manipulate it.
 *
 * @author djohnson
 */
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
   * @var string
   */
  private $developer;
  /**
   * @var array
   */
  private $cached_api_products;

  /**
   * @var bool
   */
  private $double_escape_app_name;

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
   * @param bool $double_escape_app_name
   */
  public function __construct(\Apigee\Util\APIClient $client, $developer, $double_escape_app_name = FALSE) {
    $this->init($client, $double_escape_app_name);
    if ($developer instanceof \Apigee\ManagementAPI\Developer) {
      $this->developer = $developer->get_email();
    }
    else {
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
    $obj->app_family = $response['appFamily'];
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

    $credential = end($response['credentials']);
    $obj->credential_apiproducts = $credential['apiProducts'];
    $obj->consumer_key = $credential['consumerKey'];
    $obj->consumer_secret = $credential['consumerSecret'];
    $obj->credential_scopes = $credential['scopes'];
    $obj->credential_status = $credential['status'];

    // Some apps may be misconfigured and need to be populated with their apiproducts based on credential.
    if (count($obj->api_products) == 0) {
      foreach ($obj->credential_apiproducts as $product) {
        $obj->api_products[] = $product['apiproduct'];
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
      if (!in_array($api_product, $this->api_products)) {
        $to_delete[] = $api_product;
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
        $url = "$key_uri/apiproducts/" . $this->url_encode($api_product);
        $this->client->delete($url);
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
    }

    $this->client->post($url, $payload);
    $response = $this->get_response();
    // Refresh our fields so we get latest autogenerated data such as consumer key etc.
    self::load_from_response($this, $response);
  }

  /**
   * Approves or revokes a client key for an app, and optionally also for all
   * API Products associated with that app.
   *
   * @param $status
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
  public function get_list_detail() {
    $this->client->get($this->base_url . '?expand=true');
    $list = $this->get_response();
    $app_list = array();
    foreach ($list['app'] as $response) {
      $app = new DeveloperApp($this->client, $this->developer);
      self::load_from_response($app, $response);
      $app_list[] = $app;
    }
    return $app_list;
  }

  /**
   * Attempts to make a call against a given URI. If that URI requires an OAuth
   * Token, the token negotiation is handled transparently.
   *
   * The return value is an associative array with three members:
   *   'Content-Type' contains the MIME-type of the return payload
   *   'data' contains the raw return payload
   *   'Used-OAuth' is a boolean indicating whether or not an OAuth Token was
   *                used.
   *   'Time-Elapsed' is an associative array of floats indicating how long
   *                each step of the process took. Useful for identifying
   *                bottlenecks.
   *
   * @param string $uri
   * @param string $verb
   * @param array $http_headers
   * @param mixed $payload
   * @return array
   * @throws \Apigee\Exceptions\InvalidDataException
   * @throws \Apigee\Exceptions\ResponseException
   */
  public function make_authenticated_call($uri, $verb = 'GET', $http_headers = array(), $payload = NULL) {
    $timestamps = array();
    if (!in_array($verb, array('GET', 'POST', 'PUT', 'DELETE', 'HEAD'))) {
      throw new \Apigee\Exceptions\InvalidDataException('Unknown HTTP verb ' . $verb . '.');
    }
    if (empty($this->consumer_key) || empty($this->consumer_secret)) {
      throw new \Apigee\Exceptions\InvalidDataException('Consumer key/secret are not set.');
    }
    if (preg_match('!^https?://[^/]+(/.+)$!', $uri, $matches)) {
      $path = $matches[1];
    }
    // TODO: what happens when the above regex fails?
    $oauth_token = NULL;
    foreach ($this->api_products as $api_product_name) {
      $api_product = new APIProduct($this->client, $this->double_escape_app_name);
      $start = microtime(TRUE);
      $api_product->load($api_product_name);
      $timestamps['Load ' . $api_product_name] = microtime(TRUE) - $start;
      $start = microtime(TRUE);
      $oauth_token = $api_product->get_access_token($path, $this->consumer_key, $this->consumer_secret);
      $timestamps['Get token for ' . $api_product_name] = microtime(TRUE) - $start;
      if (!empty($oauth_token)) {
        break;
      }
    }
    $using_oauth_token = FALSE;
    if (!empty($oauth_token)) {
      $http_headers['Authorization'] = 'Bearer ' . $oauth_token;
      $using_oauth_token = TRUE;
    }
    if (empty($payload) && ($verb == 'POST' || $verb == 'PUT') && isset($http_headers['Content-Type'])) {
      unset($http_headers['Content-Type']);
    }

    $opts = array(
      'method' => $verb,
      'headers' => $http_headers
    );
    if ($verb == 'POST' || $verb == 'PUT') {
      $opts['headers']['Content-Length'] = strlen($payload);
      if (!empty($payload)) {
        $opts['data'] = $payload;
      }
    }

    $start = microtime(TRUE);
    $response = APIClient::make_http_request($uri, $opts);

    if (isset($response->headers['content-type'])) {
      $content_type = $response->headers['content-type'];
    }
    else {
      $content_type = 'text/plain';
    }
    $timestamps['Executing HTTP call'] = microtime(TRUE) - $start;
    return array('Content-Type' => $content_type, 'data' => $response->data, 'Used-Oauth' => $using_oauth_token, 'Time-Elapsed' => $timestamps);
  }

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