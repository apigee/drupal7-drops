<?php

/**
 * @file
 * Contains workflow\includes\Entity\Workflow.
 * Contains workflow\includes\Entity\WorkflowController.
 */

/**
 * Implements a controller class for Workflow.
 */
class WorkflowController extends EntityAPIController {

  // public function create(array $values = array()) { }
  // public function load($ids = array(), $conditions = array()) { }

  /**
   * Overrides DrupalDefaultEntityController::cacheGet()
   * 
   * Override default function, due to Core issue #1572466.
   */
  protected function cacheGet($ids, $conditions = array()) {
    // Load any available entities from the internal cache.
    if ($ids === FALSE && !$conditions) {
      return $this->entityCache;
    }
    return parent::cacheGet($ids, $conditions);
  }

  /**
   * Overrides DrupalDefaultEntityController::cacheSet()
   */
  // protected function cacheSet($entities) { }
  //   return parent::cacheSet($entities);
  // }

  /**
   * Overrides DrupalDefaultEntityController::resetCache().
   *
   * Called by workflow_reset_cache, to
   * Reset the Workflow when States, Transitions have been changed.
   */
  // public function resetCache(array $ids = NULL) {
  //   parent::resetCache($ids);
  // }

  /**
   * Overrides DrupalDefaultEntityController::attachLoad()
   */
  protected function attachLoad(&$queried_entities, $revision_id = FALSE) {
    foreach ($queried_entities as $entity) {
      // Load the states, so they are already present on the next (cached) load.
      $entity->states = $entity->getStates($all = TRUE);
    }

    parent::attachLoad($queried_entities, $revision_id);
  }
}
 
class Workflow extends Entity {
  public $wid = 0;
  public $name = '';
  public $tab_roles = array();
  public $options = array();
  protected $creation_sid = 0;
  // Helper for workflow_get_workflows_by_type().
  protected $item = NULL; // Helper to get/set the Item of a Workflow.
  // Attached States.
  public $states = NULL;

  /**
   * CRUD functions.
   */

  // public function __construct(array $values = array(), $entityType = NULL) { }

  /**
   * Given information, update or insert a new workflow.
   *
   * @deprecated: workflow_update_workflows() --> Workflow->save()
   */
  public function save($create_creation_state = TRUE) {
    $is_new = !empty($this->is_new);

    $return = parent::save();

    if ($is_new) {
      $state = $this->getCreationState();
      $return2 = $state->save();
    }

    return $return;
  }

  /**
   * Given a wid, delete the workflow and its data.
   *
   * @deprecated: workflow_delete_workflows_by_wid() --> Workflow::delete().
   */
  public function delete() {
    $wid = $this->wid;

    // Notify any interested modules before we delete the workflow.
    // E.g., Workflow Node deletes the {workflow_type_map} record.
    module_invoke_all('workflow', 'workflow delete', $wid, NULL, NULL, FALSE);

    // Delete associated state (also deletes any associated transitions).
    foreach ($this->getStates($all = TRUE) as $state) {
      $state->deactivate($new_sid = 0);
      $state->delete();
    }

    // Delete the workflow.
    db_delete('workflows')->condition('wid', $wid)->execute();
  }

  /**
   * Validate the workflow. Generate a message if not correct.
   *
   * This function is used on the settings page of:
   * - Workflow node: workflow_admin_ui_type_map_form()
   * - Workflow field: WorkflowItem->settingsForm()
   *
   * @return bool
   *   $is_valid
   */
  public function validate() {
    $is_valid = TRUE;

    // Don't allow workflows with no states. There should always be a creation state.
    $states = $this->getStates($all = FALSE);
    if (count($states) < 1) {
      // That's all, so let's remind them to create some states.
      $message = t('Workflow %workflow has no states defined, so it cannot be assigned to content yet.',
        array('%workflow' => $this->getName()));
      drupal_set_message($message, 'warning');

      // Skip allowing this workflow.
      $is_valid = FALSE;
    }

    // Also check for transitions, at least out of the creation state. Use 'ALL' role.
    $transitions = $this->getTransitionsBySid($this->getCreationSid(), $roles = 'ALL');
    if (count($transitions) < 1) {
      // That's all, so let's remind them to create some transitions.
      $message = t('Workflow %workflow has no transitions defined, so it cannot be assigned to content yet.',
        array('%workflow' => $this->getName()));
      drupal_set_message($message, 'warning');

      // Skip allowing this workflow.
      $is_valid = FALSE;
    }

    // If the Workflow is mapped to a node type, check if workflow->options is set.
    if ($type_map = $this->getTypeMap() && !count($this->options)) {
      // That's all, so let's remind them to create some transitions.
      $message = t('Please maintain Workflow %workflow on its <a href="@url">settings</a> page.',
        array(
          '%workflow' => $this->getName(),
          '@url' => url('admin/config/workflow/workflow/edit/' . $this->wid),
        )
      );
      drupal_set_message($message, 'warning');

      // Skip allowing this workflow.
      // $is_valid = FALSE;
    }

    return $is_valid;
  }

  /**
   * Property functions.
   */

  /**
   * Create a new state for this workflow.
   */
  public function createState($name) {
    $state = WorkflowState::create($this->wid, $name);
    // Properly maintain the states list.
    $this->states[] = $state;
    return $state;
  }

  /**
   * Gets the initial state for a newly created entity.
   */
  public function getCreationState() {
    $sid = $this->getCreationSid();

    if ($sid) {
      return $this->getState($sid);
    }
    else {
      return $this->createState(WORKFLOW_CREATION_STATE_NAME);
    }
  }

  /**
   * Gets the ID of the initial state for a newly created entity.
   */
  public function getCreationSid() {
    if (!$this->creation_sid) {
      foreach ($this->getStates($all = TRUE) as $state) {
        if ($state->isCreationState()) {
          $this->creation_sid = $state->sid;
        }
      }
    }
    return $this->creation_sid;
  }

  /**
   * Gets the first valid state ID, after the creation state.
   *
   * Use WorkflowState::getOptions(), because this does a access check.
   */
  public function getFirstSid($entity_type, $entity) {
    $creation_state = $this->getCreationState();
    $options = $creation_state->getOptions($entity_type, $entity);
    if ($options) {
      $keys = array_keys($options);
      $sid = $keys[0];
    }
    else {
      // This should never happen, but it did during testing.
      drupal_set_message(t('There are no workflow states available. Please notify your site administrator.'), 'error');
      $sid = 0;
    }
    return $sid;
  }

  /**
   * Gets all states for a given workflow.
   *
   * @param mixed $all
   *   Indicates to which states to return.
   *   - TRUE = all, including Creation and Inactive;
   *   - FALSE = only Active states, not Creation;
   *   - 'CREATION' = only Active states, including Creation.
   *
   * @return array
   *   An array of WorkflowState objects.
   */
  public function getStates($all = FALSE) {
    if ($this->states === NULL) {
      $this->states = $this->wid ? WorkflowState::getStates($this->wid) : array();
    }

    $states = $this->states;
    if ($all !== TRUE) {
      foreach ($states as $state) {
        if (($all == FALSE) && $state->isCreationState()) {
          unset($states[$state->sid]);
        }
        elseif (($all === FALSE) && !$state->isActive()) {
          unset($states[$state->sid]);
        }
        elseif (($all == 'CREATION') && !$state->isActive()) {
          unset($states[$state->sid]);
        }
      }
    }
    return $states;
  }

  /**
   * Gets a state for a given workflow.
   *
   * @param $key
   *   A state ID or state Name.
   *
   * @return WorkflowState
   *   A WorkflowState object.
   */
  public function getState($key) {
    if (is_numeric($key)) {
      return workflow_state_load_single($key, $this->wid);
    }
    else {
      return workflow_state_load_by_name($key, $this->wid);
    }
  }

  /**
   * Creates a Transition for this workflow.
   */
  public function createTransition($sid, $target_sid, $values = array()) {
    if (is_numeric($sid) && is_numeric($target_sid)) {
      $values['sid'] = $sid;
      $values['target_sid'] = $target_sid;
    }
    else {
      $workflow = $this;
      $state = $workflow->getState($sid);
      $target_state = $workflow->getState($target_sid);
      $values['sid'] = $state->sid;
      $values['target_sid'] = $target_state->sid;
    }

    // First check if this transition already exists.
    if ($transitions = entity_load('WorkflowConfigTransition', FALSE, $values)) {
      $transition = reset($transitions);
    }
    else {
      $values['wid'] = $this->wid;
      $transition = new WorkflowConfigTransition($values);
    }
    $transition->wid = $this->wid;

    return $transition;
  }

  /**
   * Loads all allowed Transition for this workflow.
   *
   * @param array $tids
   *  Array of Transitions IDs. If FALSE, show all transitions.
   * @param array $conditions
   *  $conditions['sid'] : if provided, a 'from' State ID.
   *  $conditions['target_sid'] : if provided, a 'to' state ID.
   *  $conditions['roles'] : if provided, an array of roles, or 'ALL'.
   */
  public function getTransitions($tids = FALSE, $conditions = array(), $reset = FALSE) {
    $transitions = array();
    $states = $this->getStates('CREATION'); // Get valid + creation states.

    // Filter on 'from' states, 'to' states, roles.
    $sid = isset($conditions['sid']) ? $conditions['sid'] : FALSE;
    $target_sid = isset($conditions['target_sid']) ? $conditions['target_sid'] : FALSE;
    $roles = isset($conditions['roles']) ? $conditions['roles'] : 'ALL';

    // Get all transitions. (Even from other workflows. :-( )
    $config_transitions = entity_load('WorkflowConfigTransition', $tids, array(), $reset);
    foreach ($config_transitions as $transition) {
      if (!isset($states[$transition->sid])) {
        // Not a valid transition for this workflow.
      }
      elseif ($sid && $sid != $transition->sid) {
        // Not the requested 'from' state.
      }
      elseif ($target_sid && $target_sid != $transition->target_sid) {
        // Not the requested 'to' state.
      }
      elseif ($transition->isAllowed($roles)) {
        // Transition is allowed, permitted. Add to list.
        $transitions[$transition->tid] = clone $transition;
      }
    }
    return $transitions;
  }

  public function getTransitionsByTid($tid, $roles = '', $reset = FALSE) {
    $conditions = array(
      'roles' => $roles,
    );
    return $this->getTransitions(array($tid), $conditions, $reset);
  }

  public function getTransitionsBySid($sid, $roles = '', $reset = FALSE) {
    $conditions = array(
      'sid' => $sid,
      'roles' => $roles,
    );
    return $this->getTransitions(FALSE, $conditions, $reset);
  }

  public function getTransitionsByTargetSid($target_sid, $roles = '', $reset = FALSE) {
    $conditions = array(
      'target_sid' => $target_sid,
      'roles' => $roles,
    );
    return $this->getTransitions(FALSE, $conditions, $reset);
  }

  /*
   * Get a specific transition. Therefore, use $roles = 'ALL'.
   */
  public function getTransitionsBySidTargetSid($sid, $target_sid, $roles = 'ALL', $reset = FALSE) {
    $conditions = array(
      'sid' => $sid,
      'target_sid' => $target_sid,
      'roles' => $roles,
    );
    return $this->getTransitions(FALSE, $conditions, $reset);
  }

  /**
   * Gets a the type map for a given workflow.
   *
   * @param $sid
   *   A state ID.
   *
   * @return array
   *   An array of typemaps.
   */
  public function getTypeMap() {
    if (module_exists('workflownode')) {
      return workflow_get_workflow_type_map_by_wid($this->wid);
    }
    return array();
  }

  /**
   * Gets a setting from the state object.
   */
  public function getSetting($key, array $field = array()) {
    switch ($key) {
      case 'watchdog_log':
        if (isset($this->options['watchdog_log'])) {
          // This is set via Node API.
          return $this->options['watchdog_log'];
        }
        elseif ($field) {
          if (isset($field['settings']['watchdog_log'])) {
            // This is set via Field API.
            return $field['settings']['watchdog_log'];
          }
        }
        drupal_set_message('Setting Workflow::getSetting(' . $key . ') does not exist', 'error');
        break;

      default:
        drupal_set_message('Setting Workflow::getSetting(' . $key . ') does not exist', 'error');
    }
  }

  /**
   * Helper function for workflow_get_workflows_by_type().
   *
   * Get/set the Item of a particular Workflow.
   * It loads the Workflow object with the particular Field Instance data.
   * @todo 1: this is not robust: 1 Item has 1 Workflow; 1 Workflow may have N Items (fields)
   * @todo 2: find other solution.
   */
  public function getWorkflowItem(WorkflowItem $item = NULL) {
    if ($item) {
      $this->item = $item;
    }
    if (empty($this->item)) {
      // This is for Workflow Node. Emulate a Field API interface.
      // @todo D8: Remove, after converting workflow node to workflow field.
      $workflow = &$this;

      $field = array();
      $field['field_name'] = '';
      $field['id'] = 0;
      $field['settings']['wid'] = $workflow->wid;
      $field['settings']['widget'] = $workflow->options;
      // Add default values.
      $field['settings']['widget'] += array(
        'name_as_title' => TRUE,
        'options' => 'radios',
        'schedule' => TRUE,
        'schedule_timezone' => TRUE,
        'comment_log_node' => TRUE,
        'comment_log_tab' => TRUE,
        'watchdog_log' => TRUE,
        'history_tab_show' => TRUE,
      );

      $instance = array();

      $this->item = new WorkflowItem($field, $instance);
    }

    return $this->item;
  }

  /**
   * Mimics Entity API functions.
   */
  public function label($langcode = NULL) {
    return t($this->name, $args = array(), $options = array('langcode' => $langcode));
  }
  public function getName() {
    return $this->name;
  }
  public function value() {
    return $this->wid;
  }

  protected function defaultLabel() {
    return $this->name;
  }

//  protected function defaultUri() {
//    return array('path' => 'admin/config/workflow/workflow/' . $this->wid);
//  }

}
