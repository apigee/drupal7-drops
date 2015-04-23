<?php

namespace Drupal\devconnect;

use Apigee\Util\KeyValueStoreInterface;

/**
 * A wrapper class around Drupal's variable_get/variable_set.
 */
class VariableCache implements KeyValueStoreInterface {
  public function get($key, $default = NULL) {
    variable_get($key, $default);
  }

  public function set($key, $value) {
    variable_set($key, $value);
  }

  public function save() {
    // Do nothing, because $this->set saves to DB.
  }
}