<?php
namespace Drupal\sps;

/**
 * The Drupal class allow code to call a function via the class, and then
 * let others override what that function is.
 *
 * The override is mostly used in testing when one is call an external function
 * and knows what that function should return.
 *
 */
class Drupal implements \ArrayAccess {

  protected $overrides = array();
  public $ref;

  /**
  * Set the ref to be ourself so that people can use the ArrayAccess syntax
  *
  * @return 
  */
  public function __construct() {
    $this->ref = $this;
  }

  /**
  * Magic Method for finding function calls
  *
  * we look up the name to see if we have an override then 
  * we call the callable that we have.
  *
  */
  public function __call($name, $args) {
    $call = isset($this->overrides[$name]) ? $this->overrides[$name] : $name;
    return call_user_func_array($call, $args);
  }


  /**
  * if we have a callable we treturn true;
  *
  * @see ArrayAccess::offsetExists()
  */
  public function offsetExists ( $offset ) {
    $call = isset($this->overrides[$offset]) ? $this->overrides[$offset] : $offset;
    return is_callable($call);
  }

  /**
  * if we have a override return it otherwise return the offset itself
  *
  * @see ArrayAccess::offsetGet()
  */
  public function offsetGet ( $offset ) {
    $call = isset($this->overrides[$offset]) ? $this->overrides[$offset] : $offset;
    return $call;
  }

  /**
  * set a callable to be used instead of the offset
  *
  * @see ArrayAccess::offsetSet()
  */
  public function offsetSet ( $offset , $value ) {
    $this->overrides[$offset] = $value;
  }

  /**
  * revert to using the original callabke
  *
  * @see ArrayAccess::offsetUnSet()
  */
  public function offsetUnset ($offset) {
    unset($this->overrides[$offset]);
  }

  /**
  * set an override callabke
  *
  * @param $name
  *   the name of the function that should be overridden
  * @param $value
  *   a callable that should be used
  *
  * @return 
  *  Self
  */
  public function set($name, $value) {
    $this[$name] = $value;
    return $this;
  }

  /**
  * revert to the orginal function
  *
  * @param $name
  *   the function name that should revert to orginal
  *
  * @return 
  */
  public function revert($name) {
    unset($this[$name]);
    return $this;
  }

  public function revertAll() {
    foreach($this as $id => $stuff) {
      unset($this[$id]);
    }
    return $this;
    
  }
}

