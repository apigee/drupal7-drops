<?php

/**
 * @file
 * A very thin wrapper class for D8 forward-compatibility.
 *
 * @author Daniel Johnson <djohnson@apigee.com>
 */

class Drupal {

  /**
   * Fetches a config instance. Returns a cached one if possible.
   *
   * @param string $name
   * @return Drupal\Core\Config\Config
   * @throws Exception
   */
  public static function &config($name) {
    static $instances = array();
    if (!array_key_exists($name, $instances)) {
      $instances[$name] = new Drupal\Core\Config\Config($name);
    }
    return $instances[$name];
  }
}