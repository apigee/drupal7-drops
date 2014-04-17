<?php

/**
 * @file
 * Contains workflow\includes\Entity\WorkflowConfigTransition.
 * Contains workflow\includes\Entity\WorkflowConfigTransitionController.
 */

/**
 * Implements a controller class for WorkflowConfigTransition.
 *
 * The 'true' controller class is 'Workflow'.
 */
class WorkflowConfigTransitionController extends EntityAPIController {

  /**
   * Overrides DrupalDefaultEntityController::cacheGet()
   * 
   * Override default function, due to core issue #1572466.
   */
  protected function cacheGet($ids, $conditions = array()) {
    // Load any available entities from the internal cache.
    if ($ids === FALSE && !$conditions) {
      return $this->entityCache;
    }
    return parent::cacheGet($ids, $conditions);
  }

  public function save($entity, DatabaseTransaction $transaction = NULL) {
    if (empty($entity->tid)) {
      $workflow = workflow_load_single($entity->wid);
      // First check if this transition already exist.
      $config_transitions = $workflow->getTransitionsBySidTargetSid($entity->sid, $entity->target_sid);
      $config_transition = reset($config_transitions);
      if ($config_transition) {
        $entity->tid = $config_transition->tid;
      }
    }
    $return = parent::save($entity, $transaction);

    // Reset the cache for the affected workflow.
    workflow_reset_cache($entity->wid);

    return $return;
  }
}


/**
 * Implements a configurated Transition.
 *
 */
class WorkflowConfigTransition extends Entity {

  // Transition data.
  public $tid = 0;
  // public $old_sid = 0;
  // public $new_sid = 0;
  public $sid = 0; // @todo D8: remove $sid, use $new_sid. (requires conversion of Views displays.)
  public $target_sid = 0;
  public $roles = array();

  // Extra fields.
  public $wid = 0;
  // protected $is_scheduled = FALSE;
  // protected $is_executed = FALSE;
  // protected $force = NULL;

  /**
   * Entity class functions.
   */

  /**
   * Creates a new entity.
   *
   * @see entity_create()
   */
  public function __construct(array $values = array(), $entityType = NULL) {
    $entityType = 'WorkflowConfigTransition';
    return parent::__construct($values, $entityType);
  }

  /**
   * Permanently deletes the entity.
   */
  public function delete() {
    // Notify any interested modules before we delete, in case there's data needed.
    // @todo D8: this can be replaced by a hook_entity_delete(?)
    module_invoke_all('workflow', 'transition delete', $this->tid, NULL, NULL, FALSE);

    return parent::delete();
  }


  protected function defaultLabel() {
    return ''; // $this->title;
  }
  protected function defaultUri() {
    return array('path' => 'admin/config/workflow/workflow/transitions/' . $this->wid);
  }

  /**
   * Property functions.
   */

  /**
   * Returns the Workflow object of this State.
   *
   * @return Workflow
   *  Workflow object.
   */
  public function getWorkflow() {
    return workflow_load_single($this->wid);
  }

  /**
   * Verifies if the given transition is allowed.
   *
   * - in settings
   * - in permissions
   * - by permission hooks, implemented by other modules.
   *
   * @return bool
   *  TRUE if OK, else FALSE.
   */
  public function isAllowed($user_roles) {
    if ($user_roles == 'ALL') {
      // Superuser.
      return TRUE;
    }
    elseif ($user_roles) {
      if (!is_array($user_roles)) {
        $user_roles = array($user_roles);
      }
      return array_intersect($user_roles, $this->roles) == TRUE;
    }
    return TRUE;
  }

}
