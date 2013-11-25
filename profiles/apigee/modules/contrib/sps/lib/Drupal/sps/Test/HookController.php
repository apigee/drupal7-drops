<?php
namespace Drupal\sps\Test;
use \Drupal\sps\HookControllerInterface;
class HookController implements HookControllerInterface {
  public $invoke_all = array();
  public $invoke = array();
  public function setInvokeAll($hook, $callable) {
    $this->invoke_all[$hook] = $callable;
    
  }
  /**
   * @see module_invoke_all()
   */
  public function moduleInvokeAll($hook) {
    $args = func_get_args();
    // Remove $module and $hook from the arguments.
    unset($args[0]);
    if (isset($this->invoke_all[$hook])) {
      return call_user_func_array($this->invoke_all[$hook], $args);
    }

  }
  public function setModuleInvoke($module, $hook, $callable) {
    $this->invoke[$hook][$module] = $callable;
  }
  /**
   * @see module_invoke()
   */
  public function moduleInvoke($module, $hook) {
    $args = func_get_args();
    // Remove $module and $hook from the arguments.
    unset($args[0], $args[1]);
    if (isset($this->invoke[$hook][$module])) {
      return call_user_func_array($this->invoke[$hook][$module], $args);
    }
  }
  /**
   * @see drupal_alter()
   */
  public function drupalAlter($type, &$data, &$context1 = NULL, &$context2 = NULL){
    if (isset($this->alter[$type])) {
      $callable = $this->alter[$type];
      if(is_array($callable)) {
        $callable[0]->{$callable[1]}($data, $context1, $context2);
        
      }
      else {
        $callable($data, $context1, $context2);
      }
    }
    
  }

  public function setDrupalAlter($type, $callable) {
    $this->alter[$type] = $callable;
  }

  public function moduleImplements($hook){
    if(isset($this->invoke[$hook])) {
      return array_keys($this->invoke[$hook]);
    }
    return array();
  }

  public function drupalGetForm($form){
    $args = func_get_args();
    // Remove $module and $hook from the arguments.
    unset($args[0]);
    if (isset($this->form[$form])) {
      return call_user_func_array($this->form[$form], $args);
    }
  }

  public function setDrupalGetForm($name, $callable) {
    $this->form[$name] = $callable;
  }
}
