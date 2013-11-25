<?php
/**
 * @file
 * Very thin simulation of Drupal 8's Config class.
 *
 * This assumes a flat configuration, i.e. no nested array structures. Merging
 * of nested structures is a complex problem that we won't try to solve here.
 *
 * @author Daniel Johnson <djohnson@apigee.com>
 */

namespace Drupal\Core\Config;

use Symfony\Component\Yaml\Yaml;

class Config {

  /**
   * @var array
   */
  private $values;

  /**
   * @var string
   */
  private $override_config_file;

  /**
   * Reads config YAML files (if available).
   *
   * @param $name
   */
  public function __construct($name) {
    list($object_name) = explode('.', $name, 2);
    $type = 'module';
    if (!module_exists($object_name)) {
      // Let it work with a theme's settings as well
      $themes = list_themes();
      if (array_key_exists($object_name, $themes)) {
        $type = 'theme';
      }
      else {
        throw new \Exception('Invalid config name ' . $name);
      }
    }

    $config_dir = 'private://config/active';
    $is_prepared = file_prepare_directory($config_dir, FILE_CREATE_DIRECTORY);
    if (!$is_prepared) {
      drupal_set_message('Private filesystem is not writable.', 'error');
    }
    $filename = "$name.yml";

    $object_path = drupal_get_path($type, $object_name);

    $default_config_file = $object_path . '/config/' . $filename;
    $this->override_config_file = $config_dir . '/' . $filename;

    if (file_exists($default_config_file)) {
      $defaults = Yaml::parse($default_config_file);
    }
    else {
      $defaults = array();
    }
    if (file_exists($this->override_config_file)) {
      $overrides = Yaml::parse($this->override_config_file);
    }
    else {
      $overrides = array();
    }
    $this->values = $overrides + $defaults;
  }

  /**
   * Gets a value associated with a key, or NULL if not set.
   *
   * @param string $key
   * @return mixed
   */
  public function get($key = '') {
    if (empty($key)) {
      return $this->values;
    }
    elseif (array_key_exists($key, $this->values)) {
      return $this->values[$key];
    }
    else {
      return NULL;
    }
  }

  /**
   * Sets a value associated with a key. This does not automatically save
   * the results -- you must call save() to do that.
   *
   * @param string $key
   * @param mixed $value
   */
  public function set($key, $value) {
    $this->values[$key] = $value;
  }

  /**
   * Writes the values to the YAML file under sites/<sitename>/private/config
   */
  public function save() {
    return file_unmanaged_save_data(Yaml::dump($this->values, 5), $this->override_config_file, FILE_EXISTS_REPLACE);
  }

}