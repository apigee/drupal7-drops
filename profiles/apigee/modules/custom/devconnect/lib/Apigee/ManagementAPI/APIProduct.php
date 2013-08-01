<?php

/**
 * @file
 * Abstracts the API Product object in the Management API and allows clients
 * to manipulate it.
 *
 * Write support is purely experimental and should not be used unless you're
 * feeling adventurous.
 *
 * @author djohnson
 */
namespace Apigee\ManagementAPI;

use Apigee\Exceptions\ResponseException as ResponseException;
use Apigee\Util\Cache as Cache;
use Apigee\Util\APIClient as APIClient;

class APIProduct extends Base {

  /**
   * @var array
   */
  private $api_resources;
  /**
   * @var string
   * 'manual' or 'auto'
   */
  private $approval_type;
  /**
   * @var int
   */
  private $created_at;
  /**
   * @var string
   * read-only
   */
  private $created_by;
  /**
   * @var int
   * read-only
   */
  private $modified_at;
  /**
   * @var string
   * read-only
   */
  private $modified_by;
  /**
   * @var string
   * read-only
   */
  private $description;
  /**
   * @var string
   */
  private $display_name;
  /**
   * @var array
   */
  private $environments;
  /**
   * @var string
   */
  private $name;
  /**
   * @var array
   * FIXME: the purpose of this member is unknown
   */
  private $proxies;
  /**
   * @var int
   * Quota limit. It's safer to use attributes['developer.quota.limit'] instead.
   */
  private $quota;
  /**
   * @var int
   * It's safer to use attributes['developer.quota.interval'] instead.
   */
  private $quota_interval;
  /**
   * @var string
   * It's safer to use attributes['developer.quota.timeunit'] instead.
   */
  private $quota_time_unit;
  /**
   * @var array
   * FIXME: the purpose of this member is unknown
   */
  private $scopes;

  /**
   * @var array
   * Attributes must be protected because Base wants to twiddle with it.
   */
  protected $attributes;

  /**
   * @var string
   */
  private $base_url;
  /**
   * @var bool
   */
  private $loaded;
  /**
   * @var string
   */
  private $org;

  /**
   * Initializes all member variables
   *
   * @param \Apigee\Util\APIClient $client
   */
  public function __construct(\Apigee\Util\APIClient $client) {
    $this->init($client);
    $this->org = $client->get_org();
    $this->base_url = '/organizations/' . $this->url_encode($this->org) . '/apiproducts';
    $this->blank_values();
  }

  /**
   * Queries the Management API and populates self's properties from
   * the result.
   *
   * If neither $name nor $result is passed, tries to load from $this->name.
   * If $name is passed, loads from $name instead of $this->name.
   * If $response is passed, bypasses API query and uses the given array
   * instead.
   *
   * @param null|string $name
   * @param null|array $response
   */
  public function load($name = NULL, $response = NULL) {
    if (!isset($name)) {
      $name = $this->name;
    }
    if (!isset($response)) {
      $this->client->get($this->base_url . '/' . $this->url_encode($name));
      $response = $this->get_response();
    }
    $this->api_resources = $response['apiResources'];
    $this->approval_type = $response['approvalType'];
    $this->read_attributes($response);
    $this->created_at = $response['createdAt'];
    $this->created_by = $response['createdBy'];
    $this->modified_at = $response['lastModifiedAt'];
    $this->modified_by = $response['lastModifiedBy'];
    $this->description = $response['description'];
    $this->display_name = $response['displayName'];
    $this->environments = $response['environments'];
    $this->name = $response['name'];
    $this->proxies = $response['proxies'];
    $this->quota = isset($response['quota']) ? $response['quota'] : NULL;
    $this->quota_interval = isset($response['quotaInterval']) ? $response['quotaInterval'] : NULL;
    $this->quota_time_unit = isset($response['quotaTimeUnit']) ? $response['quotaTimeUnit'] : NULL;
    $this->scopes = $response['scopes'];

    $this->loaded = TRUE;
  }

  /**
   * POSTs self's properties to Management API. This handles both
   * inserts and updates.
   */
  public function save() {
    $payload = array(
      'apiResources' => $this->api_resources,
      'approvalType' => $this->approval_type,
      'description' => $this->description,
      'displayName' => $this->display_name,
      'environments' => $this->environments,
      'name' => $this->name,
      'proxies' => $this->proxies,
      'quota' => $this->quota,
      'quotaInterval' => $this->quota_interval,
      'quotaTimeUnit' => $this->quota_time_unit,
      'scopes' => $this->scopes
    );
    $this->write_attributes($payload);
    $url = $this->base_url;
    if ($this->modified_by) {
      $url .= '/' . $this->name;
    }
    $this->client->post($url, $payload);
    $this->get_response();
  }

  /**
   * Deletes an API Product.
   *
   * If $name is not passed, uses $this->name.
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
   * Determines whether an API Product should be displayed to the public.
   *
   * If $product is passed, we expect it to be a raw response array as returned
   * from the Management API, and determination is based on those contents.
   *
   * If $product is NOT passed, we assume that $this is already loaded, and we
   * make the determination based on self's properties.
   *
   * @param null|array $product
   * @return bool
   */
  public function is_public($product = NULL) {
    if (!isset($product)) {
      if (isset($this->attributes['access']) && ($this->attributes['access'] == 'internal' || $this->attributes['access'] == 'private')) {
        return FALSE;
      }
    }
    else {
      foreach ($product['attributes'] as $attr) {
        if ($attr['name'] == 'access') {
          return ($attr['value'] != 'internal' && $attr['value'] != 'private');
        }
      }
    }
    return TRUE;
  }

  /**
   * Reads, caches and returns a detailed list of org's API Products.
   *
   * @return array
   */
  private function get_products_cache() {
    static $api_products;
    if (!isset($api_products)) {
      $this->client->get($this->base_url . '?expand=true');
      $response = $this->get_response();
      foreach ($response['apiProduct'] as $prod) {
        $product = new APIProduct($this->get_client());
        $product->load(NULL, $prod);
        $api_products[] = $product;
      }
    }
    return $api_products;
  }

  /**
   * Returns a detailed list of all products. This list may have been cached
   * from a previous call.
   *
   * If $show_nonpublic is TRUE, even API Products which are marked as hidden
   * or internal are returned.
   *
   * @param bool $show_nonpublic
   * @return array
   */
  public function list_products($show_nonpublic = FALSE) {
    $products = $this->get_products_cache();
    if (!$show_nonpublic) {
      foreach ($products as $i => $product) {
        if (!$product->is_public()) {
          unset ($products[$i]);
        }
      }
    }
    return $products;
  }

  /* Accessors (getters/setters) */
  public function get_attributes() {
    return $this->attributes;
  }
  public function get_created_at() {
    return $this->created_at;
  }
  public function get_created_by() {
    return $this->created_by;
  }
  public function get_modified_at() {
    return $this->modified_at;
  }
  public function get_modified_by() {
    return $this->modified_by;
  }
  public function get_environments() {
    return $this->environments;
  }
  public function get_name() {
    return $this->name;
  }
  public function get_proxies() {
    return $this->proxies;
  }
  public function get_quota_limit() {
    if (isset($this->attributes['developer.quota.limit'])) {
      return $this->attributes['developer.quota.limit'];
    }
    elseif (!empty($this->quota)) {
      return $this->quota;
    }
    return NULL;
  }
  public function get_quota_interval() {
    if (isset($this->attributes['developer.quota.interval'])) {
      return $this->attributes['developer.quota.interval'];
    }
    elseif (!empty($this->quota_interval)) {
      return $this->quota_interval;
    }
    return NULL;
  }
  public function get_quota_time_unit() {
    if (isset($this->attributes['developer.quota.timeunit'])) {
      return $this->attributes['developer.quota.timeunit'];
    }
    elseif (!empty($this->quota_time_unit)) {
      return $this->quota_time_unit;
    }
    return NULL;
  }
  public function get_display_name() {
    return $this->display_name;
  }

  public function get_description() {
    if (!empty($this->description)) {
      return $this->description;
    }
    if (isset($this->attributes['description'])) {
      return $this->attributes['description'];
    }
    return NULL;
  }

  //TODO: populate getters/setters for other properties


  /**
   * Finds all API Proxies that this API Product uses.
   *
   * This is contingent upon a particular attribute of the API Product, which
   * at the moment remains undocumented. The apiResourcesInfo attribute (as it
   * is returned from the Management API) is a JSON-encoded string
   * representation of an array of objects, each of which describes an API
   * Proxy. How this attribute gets populated remains unknown for certain, but
   * I believe it is created when API Products are created using the Enterprise
   * UI.
   *
   * If $name is not passed, $this->name is used. Also, we try to avoid
   * reloading an already-loaded APIProduct object.
   *
   * @param null $name
   * @return array
   */
  public function get_related_proxies($name = NULL) {
    if (isset($name)) {
      if ($name != $this->name) {
        $this->loaded = FALSE;
      }
    }
    else {
      $name = $this->name;
      $this->loaded = FALSE;
    }
    if (!$this->loaded) {
      $this->load($name);
    }
    $related_proxies = array();

    if (isset($this->attributes['apiResourcesInfo']) && is_array($this->attributes['apiResourcesInfo'])) {
      foreach ($this->attributes['apiResourcesInfo'] as $resource_info) {
        if (isset($resource_info['isApi']) && $resource_info['isApi']) {
          $related_proxies[] = array(
            'name' => $resource_info['slug'],
            'base_path' => $resource_info['deploymentFullPath']
          );
        }
      }
    }
    return $related_proxies;
  }

  /**
   * Returns an associative array of OAuth token request URLs. The key of the
   * array is a path (or rather, a path base), to which the value of the array
   * is the corresponding OAuth token URL.
   *
   * Right now, only OAuth 2.0 Client Credentials are supported.
   *
   * @return array
   */
  public function get_oauth_token_urls() {
    static $urls = array();

    if (!empty($urls)) {
      return $urls;
    }

    $proxies = $this->get_related_proxies();
    $proxy_list = '';
    foreach ($proxies as $proxy) {
      $proxy_list .= ', ' . $proxy['name'];
    }

    $proxies_url_base = '/organizations/' . $this->url_encode($this->org) . '/apis';
    foreach ($proxies as $proxy) {
      $endpoint_url = $proxy['base_path'];
      $proxy_name = $proxy['name'];
      $proxy_url_base = $proxies_url_base . '/' . $this->url_encode($proxy_name);

      try {
        $this->client->get($proxy_url_base);
        $revision_list = $this->get_response();
      }
      catch (ResponseException $e) {
        continue;
      }
      $revision_num = end($revision_list['revision']);

      $revision_url_base = $proxy_url_base . '/revisions/' . $revision_num;

      try {
        $this->client->get($revision_url_base);
        $revision_info = $this->get_response();
      }
      catch (ResponseException $e) {
        continue;
      }

      $path_conditions = array();
      foreach ($revision_info['proxyEndpoints'] as $endpoint) {
        try {
          $this->client->get($revision_url_base . '/proxies/' . $this->url_encode($endpoint));
          $endpoint_obj = $this->get_response();
        }
        catch (ResponseException $e) {
          continue;
        }
        $base_path = $endpoint_obj['connection']['basePath'];
        if (!empty($endpoint_obj['preFlow']) && array_key_exists('condition', $endpoint_obj['preFlow'])) {
          $condition = $endpoint_obj['preFlow']['condition'];
          if (preg_match('!^proxy\.pathsuffix\s*(==|MatchesPath)\s*(.+)', $condition, $matches)) {
            foreach ($endpoint_obj['preFlow']['request']['steps'] as $step) {
              $step_name = $step['Step']['name'];
              $path_conditions[$step_name][] = trim($matches[2], '\'"');
            }
          }
        }
        foreach ($endpoint_obj['flows'] as $flow) {
          if (isset($flow['condition'])) {
            $condition = $flow['condition'];
            if (preg_match('!^proxy\.pathsuffix\s*(==|MatchesPath)\s*(.+)$!', $condition, $matches)) {
              foreach ($flow['request']['steps'] as $step) {
                $step_name = $step['Step']['name'];
                $path_conditions[$step_name][] = trim($matches[2], '\'"');
              }
            }
          }
        }
      }

      foreach ($revision_info['policies'] as $policy) {
        try {
          $this->client->get($revision_url_base . '/policies/' . $this->url_encode($policy));
          $policy_obj = $this->get_response();
        }
        catch (ResponseException $e) {
          continue;
        }
        if ($policy_obj['policyType'] == 'OAuthV2' && isset($policy_obj['operation']) && $policy_obj['operation'] == 'GenerateAccessToken') {
          $grant_type_items = explode('.', $policy_obj['grantType']);
          $grant_type_param = end($grant_type_items); //TODO: handle more than just request.querystring
          $allowed_grant_types = $policy_obj['supportedGrantTypes'];
          foreach ($path_conditions[$policy_obj['name']] as $path_condition) {
            foreach ($allowed_grant_types as $grant_type) {
              // TODO: $base_path corresponds to endpoint. If there is more than one endpoint, this could be a problem.
              $urls[$base_path][] = $endpoint_url . $path_condition . '?' . $grant_type_param . '=' . $grant_type;
            }
          }
        }
      }
    }
    return $urls;
  }

  /**
   * Attempts to fetch an OAuth 2.0 access token for a particular path. If
   * that path is not configured for OAuth tokens (as best we can determine),
   * FALSE is returned instead.
   *
   * @param string $path
   * @param string $consumer_key
   * @param string $consumer_secret
   * @return string|FALSE
   */
  public function get_access_token($path, $consumer_key, $consumer_secret) {

    if (isset($_SESSION['oauth_tokens'][$path])) {
      if ($_SESSION['oauth_tokens']['expires'] > time()) {
        return $_SESSION['oauth_tokens'][$path]['token'];
      }
      else {
        unset ($_SESSION['oauth_tokens'][$path]);
      }
    }

    // Use cached value wherever possible.
    $all_urls = Cache::get('devconnect_oauth_token_urls', NULL);
    if (!isset($all_urls)) {
      $urls = $this->get_oauth_token_urls();
    }
    elseif(isset($all_urls[$this->name])) {
      $urls = $all_urls[$this->name];
    }
    else {
      return FALSE;
    }

    $active_token_uri = NULL;
    foreach ($urls as $base_path => $token_uris) {
      if (substr($path, 0, strlen($base_path)) == $base_path) {
        foreach ($token_uris as $token_uri) {
          // For the moment, we only support client_credentials grant type.
          // TODO: expand this.
          if (strpos($token_uri, '=client_credentials') !== FALSE) {
            $active_token_uri = $token_uri;
            break 2;
          }
        }
      }
    } // break 2 here
    if (!isset($active_token_uri)) {
      return FALSE;
    }

    $opts = array(
      'headers' => array(
        'Accept' => 'application/json; charset=utf-8',
        'Authorization' => 'Basic ' . $consumer_key . ':' . $consumer_secret
      ),
      'method' => 'GET'
    );
    try {
      $response_obj = APIClient::make_http_request($active_token_uri, $opts);
    }
    catch (ResponseException $e) {
      //TODO: Log error here
      return FALSE;
    }
    $response = @json_decode($response_obj->data, TRUE);
    if (!isset($response) || !isset($response['access_token'])) {
      //TODO: Log error here
      return FALSE;
    }

    $_SESSION['oauth_tokens'][$path] = array(
      'token' => $response['access_token'],
      'expires' => $response['expires_in'] + time()
    );

    return $response['access_token'];
  }

  /* Accessors (getters/setters) */
  public function add_api_resource($resource) {
    $this->api_resources[] = $resource;
  }
  public function del_api_resource($resource) {
    $index = array_search($resource, $this->api_resources);
    if ($index !== FALSE) {
      unset($this->api_resources[$index]);
      // reindex this array to be sequential zero-based.
      $this->api_resources = array_values($this->api_resources);
    }
  }
  public function get_api_resources() {
    return $this->api_resources;
  }
  public function get_approval_type() {
    return $this->approval_type;
  }
  public function set_approval_type($type) {
    if ($type != 'auto' && $type != 'manual') {
      throw new \Exception('Invalid approval type ' . $type . '; allowed values are "auto" and "manual".'); // TODO: use custom exception class
    }
    $this->approval_type = $type;
  }

  /**
   * Initializes this object to its pristine blank state.
   */
  private function blank_values() {
    $this->api_resources = array();
    $this->approval_type = 'auto';
    $this->attributes = array();
    $this->created_at = NULL;
    $this->created_by = NULL;
    $this->modified_at = NULL;
    $this->modified_by = NULL;
    $this->description = '';
    $this->display_name = '';
    $this->environments = array();
    $this->name = '';
    $this->proxies = array();
    $this->quota = '';
    $this->quota_interval = '';
    $this->quota_time_unit = '';
    $this->scopes = array();

    $this->loaded = FALSE;
  }
}