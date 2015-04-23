<?php

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

  public $developers = array();

  public function __construct($values = array()) {
    // Set initial values.
    foreach ($values as $key => $value) {
      $this->$key = $value;
    }
  }


  public function getAttribute($name) {
    if (!is_array($this->attributes) || !array_key_exists($name, $this->attributes)) {
      return NULL;
    }
    return $this->attributes[$name];
  }


}

?>
