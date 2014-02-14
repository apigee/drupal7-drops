<?php

namespace Drupal\devconnect;

abstract class ArrayEntity implements \ArrayAccess, \Iterator {

  /**
   * @var array
   */
  protected $classProperties;

  /**
   * @var int
   */
  private $iterator;

  public function __construct(array $values = array()) {
    $class_properties = get_object_vars($this);
    // Remove private members
    unset($class_properties['classProperties']);
    unset($class_properties['iterator']);

    $this->classProperties = array_keys($class_properties);
    // Rewind iterator
    $this->iterator = 0;

    // Populate values if available.
    foreach ($values as $key => $value) {
      $this->offsetSet($key, $value);
    }
  }

  /**
   * Implements ArrayAccess::offsetExists.
   *
   * @param mixed $offset
   * @return bool
   */
  public function offsetExists($offset) {
    return in_array($offset, $this->classProperties);
  }

  /**
   * Implements ArrayAccess::offsetGet.
   *
   * @param mixed $offset
   * @return mixed|null
   */
  public function offsetGet($offset) {
    if (in_array($offset, $this->classProperties)) {
      return $this->$offset;
    }
    return NULL;
  }

  /**
   * Implements ArrayAccess::offsetSet.
   *
   * @param mixed $offset
   * @param mixed $value
   */
  public function offsetSet($offset, $value) {
    if (in_array($offset, $this->classProperties)) {
      $this->$offset = $value;
    }
  }

  /**
   * Implements ArrayAccess::offsetUnset.
   *
   * @param mixed $offset
   */
  public function offsetUnset($offset) {
    if (in_array($offset, $this->classProperties)) {
      if (is_array($this->$offset)) {
        $this->$offset = array();
      }
      elseif (is_int($this->$offset)) {
        $this->$offset = 0;
      }
      else {
        $this->$offset = '';
      }
    }
  }

  /**
   * Implements Iterator::current.
   *
   * @return mixed
   */
  public function current() {
    $property = $this->classProperties[$this->iterator];
    return $this->$property;
  }

  /**
   * Implements Iterator::key.
   *
   * @return mixed
   */
  public function key() {
    return $this->classProperties[$this->iterator];
  }

  /**
   * Implements Iterator::next.
   */
  public function next() {
    $this->iterator++;
  }

  /**
   * Implements Iterator::rewind
   */
  public function rewind() {
    $this->iterator = 0;
  }

  /**
   * Implements Iterator::valid.
   * @return bool
   */
  public function valid() {
    return ($this->iterator >= 0 && $this->iterator < count($this->classProperties));
  }
}