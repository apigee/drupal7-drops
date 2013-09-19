<?php
namespace Apigee\Mint;

use Apigee\Util\CacheFactory;
use \Apigee\Exceptions\ParameterException as ParameterException;
use \Apigee\Util\Log as Log;

class PricePoint extends Base\BaseObject {

  /**
   * PricePont id
   * @var string
   */
  private $id;

  /**
   * Net Price Start range
   * @var double
   */
  private $net_start_price;

  /**
   * Net Price End range
   * @var double
   */
  private $net_end_price;

  /**
   * Gross Price Start range
   * @var double
   */
  private $gross_start_price;

  /**
   * Gross Price End range
   * @var double
   */
  private $gross_end_price;

  /**
   * Is published?
   * @var bool
   */
  private $published;

  /**
   * Effective Start date
   * @var string
   */
  private $start_date;

  /**
   * Effective End date
   * @var string
   */
  private $end_date;

  /**
   * Organization
   * @var \Apigee\Mint\Organization
   */
  private $organization;

  /**
   * Name of the organization this PricePoint is in
   * @var string
   */
  private $org;

  /**
   * Product id this PricePoint is in
   * @var string
   */
  private $product_id;

  /**
   * PricePoint class constructor
   * @param string $product_id Product Id this PricePoint is in
   * @param \Apigee\Util\APIClient $client
   */
  public function __construct($product_id, \Apigee\Util\APIClient $client) {
    $this->init($client);
    $this->org = $this->client->getOrg();
    $this->product_id = $product_id;
    $this->base_url = '/mint/organizations/' . rawurlencode($this->org) . '/products/' . rawurlencode($product_id) . '/price-points';
    $this->wrapper_tag = 'pricePoint';
    $this->id_field = 'id';

    $this->initValues();
  }

  // Override of BaseObject methods

  /**
   * @see \Apigee\Mint\Base\BaseObject::save()
   * @param string $save_method Allowed values: update
   * @throws \Apigee\Exceptions\ParameterException;
   */
  public function save($save_method = 'auto') {
    if ($save_method != 'update') {
      throw new ParameterException("Only update method is supported");
    }
    parent::save('update');
  }

  // Implementation of BaseObject abstract methods

  public function instantiateNew() {
    return new PricePoint($this->product_id, $this->client);
  }

  public function loadFromRawData($data, $reset = FALSE) {
    if ($reset) {
      $this->initValues();
    }

    $excluded_properties = array('org', 'product_id', 'organization');
    foreach (array_keys($data) as $property) {
      if (in_array($property, $excluded_properties)) {
        continue;
      }

      // form the setter method name to invoke setXxxx
      $setter_method = 'set' . ucfirst($property);

      if (method_exists($this, $setter_method)) {
        $this->$setter_method($data[$property]);
      }
      else {
        Log::write(__CLASS__, Log::LOGLEVEL_NOTICE, 'No setter method was found for property "' . $property . '"');
      }
    }

    if (isset($data['organization'])) {
      $organization = new Organization($this->client);
      $organization->loadFromRawData($data['organization']);
      $this->organization = $organization;
    }
  }

  public function initValues() {
    $this->id = '';
    $this->net_start_price = NULL;
    $this->net_end_price = NULL;
    $this->gross_start_price = NULL;
    $this->gross_end_price = NULL;
    $this->published = FALSE;
    $this->start_date = '';
    $this->end_date = '';
    $this->organization = FALSE;
  }

  public function __toString() {
    // @TODO Verify
    $obj = array();
    $properties = array_keys(get_object_vars($this));
    $excluded_properties = array('org', 'product_id');
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
   * Get Price Point id
   * @return string
   */
  public function getId() {
    return $this->id;
  }

  /**
   * Set Price Point id
   * @param string $id
   * @return void
   */
  public function setId($id) {
    $this->id = $id;
  }

  /**
   * Get Net Price Start range
   * @return double
   */
  public function getNetStartPrice() {
    return $this->net_start_price;
  }

  /**
   * Set Net Price Start range
   * @param double $net_start_price
   * @return void
   */
  public function setNetStartPrice($net_start_price) {
    $this->net_start_price = $net_start_price;
  }

  /**
   * Get Net Price End range
   * @return double
   */
  public function getNetEndPrice() {
    return $this->net_end_price;
  }

  /**
   * Set Net Price End range
   * @param double $net_end_price
   * @return void
   */
  public function setNetEndPrice($net_end_price) {
    $this->net_end_price = $net_end_price;
  }

  /**
   * Get Gross Price Start range
   * @return double
   */
  public function getGrossStartPrice() {
    return $this->gross_start_price;
  }

  /**
   * Set Gross Price Start range
   * @param double $gross_start_price
   * @return void
   */
  public function setGrossStartPrice($gross_start_price) {
    $this->gross_start_price = $gross_start_price;
  }

  /**
   * Get Gross Price End range
   * @return double
   */
  public function getGrossEndPrice() {
    return $this->gross_end_price;
  }

  /**
   * Set Gross Price End range
   * @param double $gross_end_price
   * @return void
   */
  public function setGrossEndPrice($gross_end_price) {
    $this->gross_end_price = $gross_end_price;
  }

  /**
   * Retrieve if this Price Point is published?
   * @return bool
   */
  public function isPublished() {
    return $this->published;
  }

  /**
   * Set if this Price Point is published
   * @param bool $published
   * @return void
   */
  public function setPublished($published) {
    $this->published = $published;
  }

  /**
   * Get Effective Start date
   * @return string
   */
  public function getStartDate() {
    return $this->start_date;
  }

  /**
   * Set Effective Start date
   * @param string $start_date
   * @return void
   */
  public function setStartDate($start_date) {
    $this->start_date = $start_date;
  }

  /**
   * Get Effective End date
   * @return string
   */
  public function getEndDate() {
    return $this->end_date;
  }

  /**
   * Set Effective End date
   * @param string $end_date
   * @return void
   */
  public function setEndDate($end_date) {
    $this->end_date = $end_date;
  }

  /**
   * Get Organization
   * @return \Apigee\Mint\Organization
   */
  public function getOrganization() {
    return $this->organization;
  }

  /**
   * Set Organization
   * @param \Apigee\Mint\Organization $organization
   * @return void
   */
  public function setOrganization(\Apigee\Mint\Organization $organization) {
    $this->organization = $organization;
  }

  public function getList($page_num = NULL, $page_size = 20) {
    $cache_manager = CacheFactory::getCacheManager(NULL);
    $data = $cache_manager->get('price_points:' . $this->product_id, NULL);
    if (!isset($data)) {
      $this->client->get($this->base_url);
      $data = $this->getResponse();
      $cache_manager->set('price_points:' . $this->product_id, $data);
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


