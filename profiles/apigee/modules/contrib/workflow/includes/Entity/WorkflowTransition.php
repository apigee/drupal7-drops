<?php

/**
 * @file
 * Contains workflow\includes\Entity\WorkflowTransition.
 * Contains workflow\includes\Entity\WorkflowTransitionController.
 *
 * Implements (scheduled/executed) state transitions on entities.
 */

/**
 * Implements a controller class for WorkflowTransition.
 *
 * The 'true' controller class is 'Workflow'.
 */
class WorkflowTransitionController extends EntityAPIController {

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

  /**
   * Insert (no update) a transition.
   *
   * @deprecated workflow_insert_workflow_node_history() --> WorkflowTransition::save()
   */
  public function save($entity, DatabaseTransaction $transaction = NULL) {
    // Check for no transition.
    if ($entity->old_sid == $entity->new_sid) {
      if (!$entity->comment) {
        // Write comment into history though.
        return;
      }
    }

    // Make sure we haven't already inserted history for this update.
    $last_history = workflow_transition_load_single($entity->entity_type, $entity->entity_id, $entity->field_name, $limit = 1);
    if ($last_history &&
        $last_history->stamp == REQUEST_TIME &&
        $last_history->new_sid == $this->new_sid) {
      return;
    }

    unset($entity->hid);
    $entity->stamp = REQUEST_TIME;

    return parent::save($entity, $transaction);
  }

}


/**
 * Implements an actual Transition.
 *
 * If a transition is executed, the new state is saved in the Field or {workflow_node}.
 * If a transition is saved, it is saved in table {workflow_history_node}
 */
class WorkflowTransition extends Entity {
  // Field data.
  public $entity_type;
  public $field_name = '';
  public $language = LANGUAGE_NONE;
  public $delta = 0;
  // Entity data.
  public $entity_id;
  public $nid; // @todo D8: remove $nid, use $entity_id. (requires conversion of Views displays.)
  protected $entity; // This is dynamically loaded. Use WorkflowTransition->getEntity() to fetch this.
  // Transition data.
  public $old_sid = 0;
  public $new_sid = 0;
  public $sid = 0; // @todo D8: remove $sid, use $new_sid. (requires conversion of Views displays.)
  public $uid = 0;
  public $stamp;
  public $comment = '';
  protected $is_scheduled = NULL;
  protected $is_executed = NULL;
  protected $force = NULL;

  /**
   * Entity class functions.
   */

  /**
   * Creates a new entity.
   *
   * @see entity_create()
   *
   * @param $entity_type
   *  The entity type of the attached $entity.
   * @param $entityType
   *  The entity type of this Entity subclass.
   *
   * No arguments passed, when loading from DB.
   * All arguments must be passed, when creating an object programmatically.
   * One argument $entity may be passed, only to directly call delete() afterwards.
   */
  public function __construct(array $values = array(), $entityType = 'WorkflowTransition') {
    parent::__construct($values = array(), $entityType);

    // This transition is not scheduled, 
    $this->is_scheduled = FALSE; // This transition is not scheduled, 
    $this->is_executed = NULL;   // But we do not know if it is executed, yet.

    // Fill the 'new' fields correctly. @todo D8: rename these fields in db table.
    $this->entity_id = $this->nid;
    $this->new_sid = $this->sid;
  }

  /*
   * Helper function for __construct. Used for all children of WorkflowTransition (aka WorkflowScheduledTransition)
   */
  public function setValues($entity_type, $entity, $field_name, $old_sid, $new_sid, $uid, $stamp, $comment) {

    // Normally, the values are passed in an array, and set in parent::__construct, but we do it ourselves.
    // (But there is no objection to do it there.) 

    $this->entity_type = (!$entity_type) ? $this->entity_type : $entity_type;
    $this->field_name = (!$field_name) ? $this->field_name : $field_name;
    $this->language = ($this->language) ? $this->language : LANGUAGE_NONE;

    // If constructor is called with new() and arguments.
    // Load the supplied entity.
    if ($entity && !$entity_type) {
      // Not all paramaters are passed programmatically.
      drupal_set_message('Wrong call to new Workflow*Transition()', 'error');
    }
    elseif ($entity) {
      // When supplying the $entity, the $entity_type must be known, too.
      $this->entity = $entity;
      $this->entity_id = entity_id($entity_type, $entity);
      $this->nid = $this->entity_id;
    }

    if (!$entity && !$old_sid && !$new_sid) {
      // If constructor is called without arguments, e.g., loading from db.
    }
    elseif ($entity && $old_sid) {
      // Caveat: upon entity_delete, $new_sid is '0'.
      // If constructor is called with new() and arguments.
      $this->old_sid = $old_sid;
      $this->sid = $new_sid;

      $this->uid = $uid;
      $this->stamp = $stamp;
      $this->comment = $comment;
    }
    elseif (!$old_sid) {
      // Not all paramaters are passed programmatically.
      drupal_set_message(
        t('Wrong call to constructor Workflow*Transition(@old_sid to @new_sid)', array('@old_sid' => $old_sid, '@new_sid' => $new_sid)),
        'error');
    }

    // Fill the 'new' fields correctly. @todo D8: rename these fields in db table.
    $this->entity_id = $this->nid;
    $this->new_sid = $this->sid;
  }

  /**
   * Permanently deletes the entity.
   */
//  public function delete() {
//    return parent::delete();
//  }


  protected function defaultLabel() {
    return ''; // $this->title;
  }

//  protected function defaultUri() {
//    return array('path' => 'admin/config/workflow/workflow/transitions/' . $this->wid);
//  }

  /**
   * CRUD functions.
   */

  /**
   * Given a node, get all transitions for it.
   *
   * Since this may return a lot of data, a limit is included to allow for only one result.
   *
   * @param $entity_type
   * @param $entity_id
   * @param $field_name
   *   Optional.
   *
   * @return array
   *   An array of WorkflowTransitions.
   *
   * @deprecate: workflow_get_workflow_node_history_by_nid() --> workflow_transition_load_single()
   * @deprecate: workflow_get_recent_node_history() --> workflow_transition_load_multiple()
   */
  public static function loadMultiple($entity_type, $entity_id, $field_name = '', $limit = NULL) {
    if (!$entity_id) {
      return array();
    }
    $query = db_select('workflow_node_history', 'h');
    $query->condition('h.entity_type', $entity_type);
    $query->condition('h.nid', $entity_id);
    $query->condition('h.field_name', $field_name);
    $query->fields('h');
    // The timestamp is only granular to the second; on a busy site, we need the id.
    // $query->orderBy('h.stamp', 'DESC');
    $query->orderBy('h.hid', 'DESC');
    if ($limit) {
      $query->range(0, $limit);
    }
    $result = $query->execute()->fetchAll(PDO::FETCH_CLASS, 'WorkflowTransition');

    return $result;
  }

  /**
   * Given a Condition, delete transitions for it.
   * @todo: find a way to make $table automatically set for this class and its subclasses,
   *  so we do not need to override it.
   */
  public static function deleteMultiple(array $conditions, $table = 'workflow_node_history') {
    if (count($conditions) == 0) {
      return 0;
    }
    $query = db_delete($table);
    foreach ($conditions as $field_name => $value) {
      $query->condition($field_name, $value);
    }
    return $query->execute();
  }

  /**
   * Given an Entity, delete transitions for it.
   *
   * @todo: With Field API, having 2 fields, both are deleted :-( .
   */
  public static function deleteById($entity_type, $entity_id) {
    $conditions = array(
      'entity_type' => $entity_type,
      'nid' => $entity_id,
    );
    return self::deleteMultiple($conditions);
  }

  /**
   * Property functions.
   */

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
  public function isAllowed($roles, $force) {
    global $user;

    $old_sid = $this->old_sid;
    $new_sid = $this->new_sid;
    $old_state = workflow_state_load_single($old_sid);
    $entity_type = $this->entity_type;
    $entity = $this->getEntity(); // Entity may not be loaded, yet.

    $t_args = array(
      '%old_sid' => $old_sid,
      '%new_sid' => $new_sid,
    );

    // Check allow-ability of state change if user is not superuser (might be cron).
    if (($user->uid != 1) && !$force) {
      // Get the WorkflowConfigTransition.
      // @todo: some day, config_transition can be a parent of entity_transition.
      $workflow = $old_state->getWorkflow();
      $config_transitions = $workflow->getTransitionsBySidTargetSid($old_sid, $new_sid);
      $config_transition = reset($config_transitions);
      if ($config_transition) {
        if (!$config_transition->isAllowed($roles)) {
          return FALSE;
        }
      }
      else {
        watchdog('workflow', 'Attempt to go to nonexistent transition (from %old_sid to %new_sid)', $t_args, WATCHDOG_ERROR);
        return $old_sid;
      }
    }

    // Get all states from the Workflow, or only the valid transitions for this state.
    // WorkflowState::getOptions() will consider all permissions, etc.
    $options = array();
    if ($old_state) {
      $options = $old_state->getOptions($entity_type, $entity, $force);
    }
    if (!array_key_exists($new_sid, $options)) {
      drupal_set_message(t('The transition from %old_sid to %new_sid is not allowed.', $t_args), 'error');
      return FALSE;
    }

    return TRUE;
  }

  /**
   * Execute a transition (change state of a node).
   * @deprecated: workflow_execute_transition() --> WorkflowTransition::execute().
   *
   * @param bool $force
   *   If set to TRUE, workflow permissions will be ignored.
   *
   * @return int
   *  new state ID. If execution failed, old state ID is returned,
   */
  public function execute($force = FALSE) {
    global $user;

    $old_sid = $this->old_sid;
    $new_sid = $this->new_sid;
    $entity_type = $this->entity_type;
    $entity_id = $this->entity_id;
    $entity = $this->getEntity(); // Entity may not be loaded, yet.
    $field_name = $this->field_name;

    $args = array(
      '%user' => isset($user->name) ? $user->name : '',
      '%old' => $old_sid,
      '%new' => $new_sid,
    );

    $old_state = workflow_state_load_single($old_sid);
    $workflow = $old_state->getWorkflow();

    // Check if the state has changed. If not, we only record the comment.
    $state_changed = ($old_sid != $new_sid);
    if ($state_changed) {
      // State has changed. Do some checks upfront.

      $roles = array_keys($user->roles);
      $roles = array_merge(array(WORKFLOW_ROLE_AUTHOR_RID), $roles);
      if (!$this->isAllowed($roles, $force)) {
        watchdog('workflow', 'User %user not allowed to go from state %old to %new', $args, WATCHDOG_NOTICE);
        // If incorrect, quit.
        return $old_sid;
      }

      if (!$force) {
        // Make sure this transition is allowed.
        $permitted = module_invoke_all('workflow', 'transition permitted', $old_sid, $new_sid, $entity, $force, $entity_type, $field_name);
        // Stop if a module says so.
        if (in_array(FALSE, $permitted, TRUE)) {
          watchdog('workflow', 'Transition vetoed by module.');
          return $old_sid;
        }
      }
    }

    // Let other modules modify the comment.
    // @todo D8: remove all but last items from $context.
    $context = array(
      'node' => $entity,
      'sid' => $new_sid,
      'old_sid' => $old_sid,
      'uid' => $this->uid,
      'transition' => $this,
    );
    drupal_alter('workflow_comment', $this->comment, $context);

    // Make sure this transition is valid and allowed for the current user.
    if ($state_changed) {
      // Invoke a callback indicating a transition is about to occur.
      // Modules may veto the transition by returning FALSE.
      $permitted = module_invoke_all('workflow', 'transition pre', $old_sid, $new_sid, $entity, $force, $entity_type, $field_name);
      // Stop if a module says so.
      if (in_array(FALSE, $permitted, TRUE)) {
        watchdog('workflow', 'Transition vetoed by module.');
        return $old_sid;
      }
    }

    // Log the new state in {workflow_node}.
    // @todo D8: remove; this is only for Node API.
    if (!$field_name) {
      if ($state_changed) {
        // If the node does not have an existing 'workflow' property,
        // save the $old_sid there, so it can be logged.
        if (!isset($entity->workflow)) {
          $entity->workflow = $old_sid;
        }

        // Change the state for {workflow_node}.
        // The equivalent for Field API is in WorkflowDefaultWidget::submit.
        $data = array(
          'nid' => $entity_id,
          'sid' => $new_sid,
          'uid' => (isset($entity->workflow_uid) ? $entity->workflow_uid : $user->uid),
          'stamp' => REQUEST_TIME,
        );
        workflow_update_workflow_node($data);

        $entity->workflow = $new_sid;
      }
      elseif ($this->comment) {
        // If no state change, but comment, update node stamp.
        $entity->workflow_stamp = REQUEST_TIME;
        workflow_update_workflow_node_stamp($this->entity_id, REQUEST_TIME);
      }
    }

    // Log the transition in {workflow_node_history}.
    $this->is_executed = TRUE;
    $this->save();

    // Register state change with watchdog.
    if ($state_changed && $state = workflow_state_load_single($new_sid)) {
      if (!empty($workflow->options['watchdog_log'])) {
        $entity_type_info = entity_get_info($entity_type);
        $message = ($this->isScheduled()) ? 'Scheduled state change of @type %label to %state_name executed' : 'State of @type %label set to %state_name';
        $args = array(
          '@type' => $entity_type_info['label'],
          '%label' => entity_label($entity_type, $entity),
          '%state_name' => $state->label(),
        );
        $uri = entity_uri($entity_type, $entity);
        watchdog('workflow', $message, $args, WATCHDOG_NOTICE, l('view', $uri['path']));
      }
    }

    // Notify modules that transition has occurred.
    // Action triggers should take place in response to this callback, not the 'transaction pre'.
    if (!$field_name) { // @todo D8: remove; this is only for Node API.
      unset($entity->workflow_comment);
      module_invoke_all('workflow', 'transition post', $old_sid, $new_sid, $entity, $force, $entity_type, $field_name);
    }
    else {
      // @todo: we have a problem here, when using Rules, etc: The entity
      // is not saved here, but only after this call. Alternatives:
      // 1. Save the field here explicitely, using field_attach_save;
      // 2. Move the invoke to another place (but there is no entity_postsave());
      // 3. Emulate the new Entity.
      // 4. Something else.
      module_invoke_all('workflow', 'transition post', $old_sid, $new_sid, $entity, $force, $entity_type, $field_name);
    }

    // Clear any references in the scheduled listing.
    foreach (WorkflowScheduledTransition::load($entity_type, $entity_id, $field_name) as $scheduled_transition) {
      $scheduled_transition->delete();
    }

    return $new_sid;
  }

  /**
   * Get the Transitions $entity.
   *
   * @return object
   *   The entity, that is added to the Transition.
   */
  public function getEntity() {
    // A correct call, return the $entity.
    if (empty($this->entity)) {
      $entity_type = $this->entity_type;
      $entity_id = $this->entity_id;
      $this->entity = entity_load_single($entity_type, $entity_id);
    }
    return $this->entity;
  }

  /**
   * Set the Transitions $entity.
   *
   * @param $entity_type
   *  the entity type of the entity.
   * @param $entity
   *  the Entity ID or the Entity object, to add to the Transition.
   *
   * @return object $entity
   *  the entity, that is added to the Transition.
   */
  public function setEntity($entity_type, $entity) {
    if (!is_object($entity)) {
      $entity_id = $entity;
      // Use node API or Entity API to load the object first.
      $entity = entity_load_single($entity_type, $entity_id);
    }
    $this->entity = $entity;
    $this->entity_type = $entity_type;
    $this->entity_id = entity_id($entity_type, $entity);
    $this->nid = $this->entity_id;

    return $this->entity;
  }

  /**
   * Functions, common to the WorkflowTransitions.
   */

  /**
   * Returns if this is a Scheduled Transition.
   */
  public function isScheduled() {
    return $this->is_scheduled;
  }
  public function schedule($schedule = TRUE) {
    return $this->is_scheduled = $schedule;
  }

  public function isExecuted() {
    return $this->is_executed;
  }

  /**
   * A transition may be forced skipping checks.
   */
  public function isForced() {
    return (bool) $this->force;
  }
  public function force($force = TRUE) {
    return $this->force = $force;
  }

}
