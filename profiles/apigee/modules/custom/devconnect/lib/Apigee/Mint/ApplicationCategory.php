<?php

namespace Apigee\Mint;

class ApplicationCategory extends Base\BaseObject {

  /**
   * @var string
   */
  private $name;

  /**
   * @var string
   */
  private $description;

  /**
   * @var string
   * read-only uuid; auto-generated
   */
  private $id;

  public function __construct(\Apigee\Util\APIClient $client) {
    $this->init($client);
    $this->base_url = '/mint/organizations/' . rawurlencode($this->client->getOrg()) . '/application-categories';
    $this->id_field = 'id';
    $this->wrapper_tag = 'applicationCategory';
    $this->initValues();
  }

  protected function initValues() {
    $this->description = '';
    $this->name = '';
    $this->id = '';
  }

  public function loadFromRawData($data, $reset = FALSE) {
    if ($reset) {
      $this->initValues();
    }
    foreach (array('description', 'id', 'name') as $field) {
      $this->$field = (isset($data[$field]) ? $data[$field] : NULL);
    }
  }

  public function __toString() {
    $obj = array(
      'description' => $this->description,
      'id' => $this->id,
      'name' => $this->name
    );
    return json_encode($obj);
  }

  public function instantiateNew() {
    return new ApplicationCategory($this->client);
  }

  /*
   * accessors (getters/setters)
   */
  public function getDescription() {
    return $this->description;
  }
  public function setDescription($desc) {
    $this->description = (string)$desc;
  }
  public function getId() {
    return $this->id;
  }
  // Used in data load invoked by $this->loadFromRawData()
  private function setId($id) {
    $this->id = $id;
  }
  public function getName() {
    return $this->name;
  }
  public function setName($name) {
    $this->name = (string)$name;
  }
}