<?php
namespace Drupal\sps;

class DrupalHookController implements HookControllerInterface {
  /**
   * @see module_invoke_all()
   */
  public function moduleInvokeAll($hook) {
    return module_invoke_all($hook);
  }

  /**
   * @see module_invoke()
   */
  public function moduleInvoke($module, $hook) {
    return module_invoke($module, $hook);
  }

  /**
   * @see drupal_alter()
   */
  public function drupalAlter($type, &$data, &$context1 = NULL, &$context2 = NULL) {
  return drupal_alter($type, $data, $context1, $context2);
  }

  /**
   * @see module_implements()
   */
  public function moduleImplements($hook) {
    return module_implements($hook);
  }


  /**
   * @see drupal_get_form()
   */
  public function drupalGetForm($form) {
    return call_user_func_array('drupal_get_form', func_get_args());
    
  }
}
