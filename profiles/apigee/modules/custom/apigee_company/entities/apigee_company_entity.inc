<?php
/**
 * @file
 * Entity Class for the Apigee Company Entity
 */

class ApigeeCompanyEntity {

  /**
   * @var string
   */
  public $name = '';

  /**
   * @var array
   */
  public $attributes = array();

  /**
   * @var string
   */
  public $displayName = '';

  /**
   * @var status
   */
  public $status = '';

  /**
   *
   * @var array
   */
  public $developers = array();

  /**
   * Class Constructor.
   *
   * @param array $values
   *   Associative array with keys as the class properties and values as their
   *   values
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


}
