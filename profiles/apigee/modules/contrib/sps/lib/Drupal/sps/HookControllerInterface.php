<?php
namespace Drupal\sps;

interface HookControllerInterface {
  /**
   * @see module_invoke_all()
   */
  public function moduleInvokeAll($hook);
  /**
   * @see module_invoke()
   */
  public function moduleInvoke($module, $hook);
  /**
   * @see drupal_alter()
   */
  public function drupalAlter($type, &$data, &$context1 = NULL, &$context2 = NULL);
  /**
   * @see module_implements()
   */
  public function moduleImplements($hook);
  /**
   * @see drupalGetForm()
   */
  public function drupalGetForm($form);

}
