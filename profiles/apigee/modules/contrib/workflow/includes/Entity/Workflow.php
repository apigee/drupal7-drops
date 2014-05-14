<?php

/**
 * @file
 * Contains workflow\includes\Entity\Workflow.
 * Contains workflow\includes\Entity\WorkflowController.
 */

/**
 * Implements a controller class for Workflow.
 */
class WorkflowController extends EntityAPIControllerExportable {

  // public function create(array $values = array()) {    return parent::create($values);  }
  // public function load($ids = array(), $conditions = array()) { }

  public function delete($ids, DatabaseTransaction $transaction = NULL) {
    // @todo: replace WorkflowController::delete() with parent.
    foreach($ids as $wid) {
      if ($workflow = workflow_load($wid)) {
        $workflow->delete();
      }
    }
    $this->resetCache();
  }

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
      $entity->transitions = $entity->getTransitions(FALSE);
      $entity->typeMap = $entity->getTypeMap();
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

  // Attached States.
  public $states = NULL;
  public $transitions = NULL;

  /**
   * CRUD functions.
   */

//  public function __construct(array $values = array(), $entityType = NULL) {
//    return parent::__construct($values, $entityType);
//  }

  public function __clone() {
    foreach ($this->states as &$state) {
      $state = clone $state;
    }
    foreach ($this->transitions as &$transition) {
      $transition = clone $transition;
    }
  }

  /**
   * Given information, update or insert a new workflow.
   *
   * This also handles importing, rebuilding, reverting from Features, 
   * as defined in workflow.features.inc.
   * todo: reverting does not refresh States and transitions, since no
   * machine_name was present. As of 7.x-2.3, the machine_name exists in 
   * Workflow and WorkflowConfigTransition, so rebuilding is possible.
   *
   * When changing this function, test with the following situations:
   * - maintain Workflow in Admin UI;
   * - clone Workflow in Admin UI;
   * - create/revert/rebuild Workflow with Features; @see workflow.features.inc
   * - save Workflow programmatically;
   */
  public function save($create_creation_state = TRUE) {
    // Are we saving a new Workflow?
    $is_new = !empty($this->is_new);
    // Are we rebuilding, reverting a new Workflow? @see workflow.features.inc
    $is_rebuild = !empty($this->is_rebuild);
    $is_reverted = !empty($this->is_reverted);

    // If rebuild by Features, make some conversions.
    if (!$is_rebuild && !$is_reverted) {
      // Avoid troubles with features clone/revert/..
      unset($this->module);
    }
    else {
      $role_map = isset($this->system_roles) ? $this->system_roles : array();
      if ($role_map) {
        // Remap roles. They can come from another system with shifted role IDs.
        // See also https://drupal.org/node/1702626 .
        $this->tab_roles = _workflow_rebuild_roles($this->tab_roles, $role_map);
        foreach ($this->transitions as &$transition) {
          $transition['roles'] = _workflow_rebuild_roles($transition['roles'], $role_map);
        }
      }

      // Insert the type_map when building from Features.
      if ($this->typeMap) {
        foreach ($this->typeMap as $node_type) {
          workflow_insert_workflow_type_map($node_type, $this->wid);
        }
      }
    }
    // After update.php or import feature, label might be empty. @todo: remove in D8.
    if (empty($this->label)) {
      $this->label = $this->name;
    }

    $return = parent::save();

    // If a workflow is cloned in Admin UI, it contains data from original workflow.
    // Redetermine the keys.
    if (($is_new) && $this->states) {
      foreach ($this->states as $state) {
        // Can be array when cloning or with features.
        $state = is_array($state) ? new WorkflowState($state) : $state;
        // Set up a conversion table, while saving the states.
        $old_sid = $state->sid;
        $state->wid = $this->wid;
        // @todo: setting sid to FALSE should be done by entity_ui_clone_entity().
        $state->sid = FALSE;
        if($state->isActive()) {
          $state->save();
          $sid_conversion[$old_sid] = $state->sid;
        }
      }

      // Reset state cache.
      $this->getStates(TRUE, TRUE);
      foreach ($this->transitions as &$transition) {
        // Can be array when cloning or with features.
        $transition = is_array($transition) ? new WorkflowConfigTransition($transition, 'WorkflowConfigTransition') : $transition;
        // Convert the old sids of each transitions before saving.
        // @todo: in this be done in 'clone $transition'?
        // (That requires a list of transitions without tid and a wid-less conversion table.)
        $transition->tid = FALSE;
        $transition->sid = $sid_conversion[$transition->sid];
        $transition->target_sid = $sid_conversion[$transition->target_sid];
        $transition->save();
      }
    }

    // Make sure a Creation state exists.
    if ($is_new) {
      $state = $this->getCreationState();
      $return2 = $state->save();
    }

    workflow_reset_cache($this->wid);

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
    if ($this->getTypeMap() && !count($this->options)) {
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
    $wid = $this->wid;
    $state = workflow_state_load_by_name($name, $wid);
    if (!$state) {
      $state = entity_create('WorkflowState', array('name' => $name, 'wid' => $wid));
    }

    // Properly maintain the states list.
    $this->states[] = $state;
    return $state;
  }

  /**
   * Gets the initial state for a newly created entity.
   */
  public function getCreationState() {
    $sid = $this->getCreationSid();
    return ($sid) ? $this->getState($sid) : $this->createState(WORKFLOW_CREATION_STATE_NAME);
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
   * Uses WorkflowState::getOptions(), because this does a access check.
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
  public function getStates($all = FALSE, $reset = FALSE) {
    if ($this->states === NULL || $reset) {
      $this->states = $this->wid ? WorkflowState::getStates($this->wid, $reset) : array();
    }
    // Do not unset, but add to array - you'll remove global objects otherwise.
    $states = array();
    foreach ($this->states as $state) {
      if ($all === TRUE) {
        $states[$state->sid] = $state;
      }
      elseif (($all === FALSE) && ($state->isActive() && !$state->isCreationState())) {
        $states[$state->sid] = $state;
      }
      elseif (($all == 'CREATION') && ($state->isActive() || $state->isCreationState())) {
        $states[$state->sid] = $state;
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
      $transition = entity_create('WorkflowConfigTransition', $values);
    }
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
        $transition->setWorkflow($this);
        $transitions[$transition->tid] = $transition;
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
    $result = array();

    $type_maps = module_exists('workflownode') ? workflow_get_workflow_type_map_by_wid($this->wid) : array();
    foreach ($type_maps as $map) {
      $result[] = $map->type;
    }

    return $result;
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
   * Mimics Entity API functions.
   */
  public function getName() {
    return $this->name;
  }

  protected function defaultLabel() {
    return isset($this->label) ? $this->label : '';
  }

  protected function defaultUri() {
    return array('path' => 'admin/config/workflow/workflow/' . $this->wid);
  }

}

function _workflow_rebuild_roles(array $roles, array $role_map) {
  // See also https://drupal.org/node/1702626 .
  $new_roles = array();
  foreach ($roles as $key => $rid) {
    if ($rid == -1) {
      $new_roles[$rid] = $rid;
    }
    else {
      $role = user_role_load_by_name($role_map[$rid]);
      $new_roles[$role->rid] = $role->rid;
    }
  }
  return $new_roles;
}
