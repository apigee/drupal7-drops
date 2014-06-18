<?php

namespace Drupal\devconnect_user;

class UserRoleEntity {
  /**
   * @var string
   */
  public $name;
  /**
   * @var array
   */
  public $users;

  public function __construct(array $values = array()) {
    // Populate values if available.
    foreach ($values as $key => $value) {
      if (property_exists($this, $key)) {
        $this->$key = $value;
      }
    }
  }

}
