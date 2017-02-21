<?php
/**
 * @file
 * Entity Class for the Apigee Company Entity.
 */

/**
 * Entity class for apigee_company entity.
 */
class ApigeeCompanyEntity {

  /**
   * Machine name of the company.
   *
   * @var string
   */
  public $name = '';

  /**
   * Key-value pairs of company attributes.
   *
   * @var array
   */
  public $attributes = array();

  /**
   * Display name of the company.
   *
   * @var string
   */
  public $displayName = '';

  /**
   * Status of the company.
   *
   * @var string
   */
  public $status = '';

  /**
   * List of developers in the company.
   *
   * @var array
   */
  public $developers = array();

  /**
   * Class Constructor.
   *
   * @param array $values
   *   Associative array with keys as the class properties and values as their
   *   values.
   */
  public function __construct($values = array()) {
    // Set initial values.
    foreach ($values as $key => $value) {
      $this->$key = $value;
    }
  }

  /**
   * Fetch a single attribute by name.
   *
   * @param string $name
   *   The attribute name.
   *
   * @return mixed
   *   The value for the passed in attribute $name if the attribute exist else
   *   return NULL.
   */
  public function getAttribute($name) {
    if (!is_array($this->attributes) || !array_key_exists($name, $this->attributes)) {
      return NULL;
    }
    return $this->attributes[$name];
  }

  /**
   * Persist an Apigee Company entity's values as an array.
   *
   * @return array
   *   The values of this entity's internal properties.
   */
  public function toArray() {
    return array(
      'name' => $this->name,
      'displayName' => $this->displayName,
      'status' => $this->status,
      'developers' => $this->developers,
    );
  }

}
