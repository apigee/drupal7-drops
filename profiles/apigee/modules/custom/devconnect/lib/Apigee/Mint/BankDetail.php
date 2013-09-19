<?php
namespace Apigee\Mint;

use \Apigee\Util\Log as Log;

class BankDetail extends Base\BaseObject {

  /**
   * @var string
   *   Developer's email owning this banks details
   */
  private $dev_email;

  /**
   * @var \Apigee\Mint\DataStructures\Address
   *     Bank's Addresses
   */
  private $address;

  /**
   * @var string
   *     Bank's ABAN
   */
  private $aban;

  /**
   * @var string
   *     Account name
   */
  private $account_name;

  /**
   * @var string
   *     Account number
   */
  private $account_number;

  /**
   * @var string
   *     Bank's BIC
   */
  private $bic;

  /**
   * @var string
   *     ISO 4217 currency code
   */
  private $currency;

  /**
   * @var string
   *     Bank's IBAN/Router number
   */
  private $iban_number;

  /**
   * @var string
   *     Bank Detail object id
   */
  protected $id;

  /**
   * @var string
   *     Bank's name
   */
  private $name;

  /**
   * @var string
   *     Bank's Sort Code
   */
  private $sort_code;


  public function __construct($developer_email, \Apigee\Util\APIClient $client) {
    $this->init($client);
    $this->dev_email = $developer_email;
    $this->base_url = '/mint/organizations/' . rawurlencode($this->client->getOrg()) . '/developers/' . rawurldecode($developer_email) . '/bank-details';
    $this->wrapper_tag = 'bankDetail';
    $this->id_field = 'id';

    $this->initValues();
  }

  protected function initValues() {
    $this->aban = NULL;
    $this->account_name = NULL;
    $this->account_number = NULL;
    $this->address = NULL;
    $this->bic = NULL;
    $this->currency = NULL;
    $this->iban_number = NULL;
    $this->id = NULL;
    $this->name = NULL;
    $this->sort_code = NULL;
  }

  public function instantiateNew() {
    return new BankDetail($this->dev_email, $this->client);
  }

  public function load($id = NULL) {
    $url = $this->base_url;
    $this->client->get($url);
    $data = $this->getResponse();
    foreach ($data[$this->wrapper_tag] as $bank_detail_data) {
      $this->loadFromRawData($bank_detail_data);
      break;
    }
  }

  public function loadFromRawData($data, $reset = FALSE) {
    if ($reset) {
      $this->initValues();
    }
    $excluded_properties = array('address');
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

    if (isset($data['address']) && is_array($data['address']) && count($data['address']) > 0) {
      $this->address = new DataStructures\Address($data['address']);
    }
  }

  private static function getSetterMethods($class_name) {
    $class = new ReflectionClass($class_name);
    $getter_methods = array();
    foreach ($class->getMethods() as $method) {
      if ($method->getDeclaringClass() != $class) {
        continue;
      }
      $method_name = $method->getName();

      if (strpos($method_name, 'get') !== 0 || $method->getNumberOfParameters() > 0) {
        continue;
      }

      if ($method->isProtected() || $method->isPublic()) {
        $setter_methods[$method_name] = $method;
      }
    }
    return $setter_methods;
  }

  public function save($save_method) {
    if ($this->id == NULL) {
      $url = $this->base_url;
      $this->client->post($url, $this->__toString(), 'application/json', 'application/json');
    }
    else {
      $url = '/mint/organizations/'. rawurlencode($this->client->getOrg()) . '/bank-details/' . $this->id;
      $this->client->put($url, $this->__toString());
    }
  }

  public function delete() {
    $url = '/mint/organizations/' . rawurlencode($this->client->getOrg()) . '/bank-details/' . $this->id;
    $this->client->delete($url);
  }

  public function __toString() {
    $object = array(
      'name' => $this->name,
      'accountName' => $this->account_name,
      'accountNumber' => $this->account_number,
      'currency' => $this->currency,
      'sortCode' => $this->sort_code,
      'aban' => $this->aban,
      'bic' => $this->bic,
      'ibanNumber' => $this->iban_number,
    );

    if (isset($this->id)) {
      $object['id'] = $this->id;
    }
    if (isset($this->address)) {
      $object['address'] = array(
        'address1' => $this->address->getAddress1(),
        'address2' => $this->address->getAddress2(),
        'isPrimary' => 'true',
        'city' => $this->address->getCity(),
        'state' => $this->address->getState(),
        'zip' => $this->address->getZip(),
        'country' => $this->address->getCountry(),
        'id' => $this->address->getId(),
      );
    }
    return json_encode((object)$object);
  }

  public function getAban() {
    return $this->aban;
  }
  public function setAban($aban) {
    // TODO: validate
    $this->aban = $aban;
  }
  public function getAccountName() {
    return $this->account_name;
  }
  public function setAccountName($name) {
    $this->account_name = $name;
  }
  public function getAccountNumber() {
    return $this->account_number;
  }
  public function setAccountNumber($num) {
    $this->account_number = $num;
  }
  public function getAddress() {
    return $this->address;
  }
  public function setAddress(DataStructures\Address $addr) {
    $this->address = $addr;
  }
  public function getBic() {
    return $this->bic;
  }
  public function setBic($bic) {
    $this->bic = $bic;
  }
  public function getCurrency() {
    return $this->currency;
  }
  public function setCurrency($curr) {
    // TODO: validate
    $this->currency = $curr;
  }
  public function getIbanNumber() {
    return $this->iban_number;
  }
  public function setIbanNumber($num) {
    // TODO: validate?
    $this->iban_number = $num;
  }
  public function getId() {
    return $this->id;
  }
  public function setId($id) {
    $this->id = $id;
  }
  public function getName() {
    return $this->name;
  }
  public function setName($name) {
    $this->name = $name;
  }
  public function getSortCode() {
    return $this->sort_code;
  }
  public function setSortCode($code) {
    // TODO: validate
    $this->sort_code = $code;
  }

}