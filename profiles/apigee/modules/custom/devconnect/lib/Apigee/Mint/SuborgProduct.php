<?php
namespace Apigee\Mint;

use Apigee\Util\CacheFactory;
use \Apigee\Exceptions\ParameterException as ParameterException;
use \Apigee\Util\Log as Log;

class SuborgProduct extends Base\BaseObject {

  /**
   * Organization
   * @var \Apigee\Mint\Organization
   */
  private $organization;

  /**
   * Product
   * @var \Apigee\Mint\Product
   */
  private $product;

  /**
   * Status
   * @var string
   */
  private $status;

  /**
   * Organization id
   * @var string
   */
  private $org;

  /**
   * Product id
   * @var string
   */
  private $productId;

  /**
   * Class constructor
   * @param string $product_id Product id
   * @param \Apigee\Util\APIClient $client
   */
  public function __construct($product_id, \Apigee\Util\APIClient $client) {
    $this->init($client);
    $this->org = $this->client->getOrg();
    $this->base_url = '/mint/organizations/' . rawurlencode($this->org) . '/products/' . rawurlencode($product_id) . '/suborg-products';
    $this->wrapper_tag = 'suborgProduct';
    $this->id_field = 'id';
    $this->productId = $product_id;
    $this->initValues();
  }

  // Override of BaseObject methods

  /**
   * @see \Apigee\Mint\Base\BaseObject::save()
   * @param string $save_method Allowed values: update
   * @throws \Apigee\Exceptions\ParameterException;
   */
  public function save($save_method) {
    if ($save_method != 'update') {
      throw new ParameterException("Only update method is supported");
    }
    parent::save('update');
  }

  // Implementation of BaseObject abstract methods

  public function instantiateNew() {
    return new SuborgProduct($this->productId, $this->client);
  }

  public function loadFromRawData($data, $reset = FALSE) {
    if ($reset) {
      $this->initValues();
    }

    if (isset($data['organization']) && is_array($data['organization'])) {
      $organization = new Organization($this->client);
      $organization->loadFromRawData($data['organization']);
      $this->organization = $organization;
    }

    if (isset($data['product'])) {
      $product = new Product($this->client);
      $product->loadFromRawData($data['product']);
      $this->product = $product;
    }

    $this->status = isset($data['status']) ? $data['status'] : NULL;
  }

  public function initValues() {
    $this->organization = NULL;
    $this->product = NULL;
    $this->status = 'ACTIVE';
  }

  public function __toString() {
    // @TODO Verify
    $obj = array();
    $properties = array_keys(get_object_vars($this));
    $excluded_properties = array('org', 'productId');
    $excluded_properties = array_merge(array_keys(get_class_vars(get_parent_class($this))), $excluded_properties);
    foreach ($properties as $property) {
      if (in_array($property, $excluded_properties)) {
        continue;
      }
      if (isset($this->$property)) {
        if (is_object($this->$property)) {
          $obj[$property] = json_decode((string) $this->$property, TRUE);
        }
        else {
          $obj[$property] = $this->$property;
        }
      }
    }
    return json_encode($obj);
  }

  // getters/setters

  /**
   * Get Organization
   * @return \Apigee\Mint\Organization
   */
  public function getOrganization() {
    return $this->organization;
  }

  /**
   * Get Product
   * @return \Apigee\Mint\Product
   */
  public function getProduct() {
    return $this->product;
  }

  /**
   * Get Status
   * @return string
   */
  public function getStatus() {
    return $this->status;
  }

  /**
   * Set Organization
   * @param \Apigee\Mint\Organization $organization
   */
  public function setOrganization(Organization $organization) {
    $this->organization = $organization;
  }

  /**
   * Set Product
   * @param \Apigee\Mint\Product $product
   */
  public function setProduct(Product $product) {
    $this->product = $product;
  }

  /**
   * Set Status
   * @param string $status Possible values CREATED|INACTIVE|ACTIVE
   * @throws \Apigee\Exceptions\ParameterException
   */
  public function setStatus($status) {
    if (!in_array($status, array('CREATED', 'INACTIVE', 'ACTIVE'))) {
      throw new ParameterException('Invalid suborg product status value: ' . $status);
    }
    $this->status = $status;
  }

  public function getList($page_num = NULL, $page_size = 20) {

    $cache_manager = CacheFactory::getCacheManager(NULL);
    $data = $cache_manager->get('suborg_product:' . $this->productId, NULL);
    if (!isset($data)) {
      $this->client->get($this->base_url);
      $data = $this->getResponse();
      $cache_manager->set('suborg_product:' . $this->productId, $data);
    }

    $return_objects = array();

    foreach ($data[$this->wrapper_tag] as $response_data) {
      $obj = $this->instantiateNew();
      $obj->loadFromRawData($response_data);
      $return_objects[] = $obj;
    }
    return $return_objects;
  }
}