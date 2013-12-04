<?php

/**
 * @file
 * Contains workflow\includes\Entity\WorkflowState.
 */

class WorkflowState {
  // Since workflows do not change, it is implemented as a singleton.
  protected static $states = array();

  public $sid = 0;
  public $wid = 0;
  public $weight = 0;
  protected $sysid = 0;
  protected $state = ''; // @todo D8: remove $state, use $label/$name. (requires conversion of Views displays.)
  public $status = 1;
  protected $workflow = NULL;

  /**
   * CRUD functions.
   */

  /**
   * Constructor.
   */
  protected function __construct($wid = 0, $sid = 0) {
    if (empty($sid) && empty($wid)) {
      // Automatic constructor when casting an array or object.
      if (!isset(self::$states[$this->sid])) {
        self::$states[$this->sid] = $this;
      }
    }
    elseif (empty($sid)) {
      // Creating an dummy/new state for a workflow.
      // Do not add to 'cache' self::$tates.
      $this->wid = $wid;
    }
    else {
      // Fetching an existing state for a workflow.
      if (!isset(self::$states[$sid])) {
        self::$states[$sid] = WorkflowState::load($sid, $wid);
      }
      // State may not exist.
      if (self::$states[$sid]) {
        // @todo: this copy-thing should not be necessary.
        $this->sid = self::$states[$sid]->sid;
        $this->wid = self::$states[$sid]->wid;
        $this->weight = self::$states[$sid]->weight;
        $this->sysid = self::$states[$sid]->sysid;
        $this->state = self::$states[$sid]->state;
        $this->status = self::$states[$sid]->status;
        $this->workflow = self::$states[$sid]->workflow;
      }
    }
  }

  /**
   * Creates and returns a new WorkflowState object.
   *
   * @param $wid
   *  The Workflow ID for which a new State is created.
   * @param $name
   *  The name of the new State. If '(creation)', a CreationState is generated.
   *
   * @return WorkflowState
   *  A new WorkflowState object.
   *
   * "New considered harmful".
   */
  public static function create($wid, $name = '') {
    $state = new WorkflowState($wid);
    $state->state = $name;
    if ($name == '(creation)') {
      $state->sysid = WORKFLOW_CREATION;
      $state->weight = WORKFLOW_CREATION_DEFAULT_WEIGHT;
    }
    return $state;
  }

  /**
   * Alternative constructor, loading objects from table {workflow_states}.
   *
   * @param $sid
   *  the requested State ID
   * @param $wid
   *  an optional Workflow ID, to check if the requested State is valid for the Workflow.
   *
   * @return $state
   *  WorkflowState if state is successfully loaded,
   *  NULL if not loaded,
   *  FALSE if state does not belong to requested Workflow.
   */
  public static function load($sid, $wid = 0) {
    $states = self::getStates();
    $state = isset($states[$sid]) ? $states[$sid] : NULL;
    if ($wid && $state && ($wid != $state->wid)) {
      return FALSE;
    }
    return $state;
  }

  /**
   * Get all states in the system, with options to filter, only where a workflow exists.
   *
   * @param $wid
   *  the requested Workflow ID.
   * @param bool $reset
   *  an option to refresh all caches.
   *
   * @return array $states
   *  an array of cached states.
   *
   * @deprecated workflow_get_workflow_states() --> WorkflowState::getStates()
   * @deprecated workflow_get_workflow_states_all() --> WorkflowState::getStates()
   * @deprecated workflow_get_other_states_by_sid($sid) --> WorkflowState::getStates()
   */
  public static function getStates($wid = 0, $reset = FALSE) {
    if ($reset) {
      self::$states = array();
    }

    if (empty(self::$states)) {
      // Build the query, and get ALL states.
      // Note: self::states[] is populated in respective constructors.
      $query = db_select('workflow_states', 'ws');
      $query->fields('ws');
      $query->orderBy('ws.weight');
      $query->orderBy('ws.wid');
      // Just for grins, add a tag that might result in modifications.
      $query->addTag('workflow_states');

      $query->execute()->fetchAll(PDO::FETCH_CLASS, 'WorkflowState');
    }

    if (!$wid) {
      // All states are requested and cached: return them.
      return self::$states;
    }
    else {
      // All states of only 1 Workflow is requested: return this one.
      $result = array();
      foreach (self::$states as $state) {
        if ($state->wid == $wid) {
          $result[$state->sid] = $state;
        }
      }
      return $result;
    }
  }

  /**
   * Get all states in the system, with options to filter, only where a workflow exists.
   *
   * May return more then one State, since a name is not (yet) an UUID.
   */
  public static function loadByName($name, $wid) {
    foreach ($states = self::getStates($wid) as $state) {
      if ($name == $state->getName()) {
        return $state;
      }
    }
    return NULL;
  }

  /**
   * Save (update/insert) a Workflow State into table workflow_states.
   *
   * @deprecated: workflow_update_workflow_states() --> WorkflowState->save()
   */
  public function save() {
    $sid = $this->sid;

    // Convert all properties to an array, the previous ones, too.
    $data['sid'] = $this->sid;
    $data['wid'] = $this->wid;
    $data['weight'] = $this->weight;
    $data['sysid'] = $this->sysid;
    $data['state'] = $this->state;
    $data['status'] = $this->status;

    if (!empty($this->sid) && count(WorkflowState::load($sid)) > 0) {
      drupal_write_record('workflow_states', $data, 'sid');
    }
    else {
      drupal_write_record('workflow_states', $data);
    }
    // Update the page cache.
    self::$states[$sid] = $this;
  }

  /**
   * Given data, delete from workflow_states.
   */
  public function delete() {
    db_delete('workflow_states')
      ->condition('sid', $this->sid)
      ->execute();
  }

  /**
   * Deactivate a Workflow State, moving existing nodes to a given State.
   *
   * @param $new_sid
   *  the state ID, to which all affected entities must be moved. 
   *
   * @deprecated workflow_delete_workflow_states_by_sid() --> WorkflowState->deactivate() + delete()
   */
  public function deactivate($new_sid) {
    $current_sid = $this->sid;
    $force = TRUE;

    // Notify interested modules. We notify first to allow access to data before we zap it.
    // E.g., Node API (@todo Field API):
    // - re-parents any nodes that we don't want to orphan, whilst deactivating a State.
    // - delete any lingering node to state values.
    module_invoke_all('workflow', 'state delete', $current_sid, $new_sid, NULL, $force);

    // Re-parent any nodes that we don't want to orphan, whilst deactivating a State.
    // This is called in WorkflowState::deactivate().
    // @todo: reparent Workflow Field, whilst deactivating a state.
    if ($new_sid) {
      global $user;
      // A candidate for the batch API.
      // @TODO: Future updates should seriously consider setting this with batch.

      $comment = t('Previous state deleted');
      foreach (workflow_get_workflow_node_by_sid($current_sid) as $workflow_node) {
        // @todo: add Field support in 'state delete', by using workflow_node_history or reading current field.
        $entity_type = 'node';
        $entity = entity_load_single('node', $workflow_node->nid);
        $field_name = '';
        $transition = new WorkflowTransition($entity_type, $entity, $field_name, $current_sid, $new_sid, $user->uid, REQUEST_TIME, $comment);
        $transition->force($force); 
        // Excute Transition, invoke 'pre' and 'post' events, save new state in workflow_node, save also in workflow_node_history.
        // For Workflow Node, only {workflow_node} and {workflow_node_history} are updated. For Field, also the Entity itself.
        $new_sid = workflow_execute_transition($entity_type, $entity, $field_name, $transition, $force);
      }
    }
    // Delete any lingering node to state values.
    workflow_delete_workflow_node_by_sid($current_sid);

    // Find out which transitions this state is involved in.
    $preexisting = array();
    foreach (workflow_get_workflow_transitions_by_sid_involved($current_sid) as $data) {
      $preexisting[$data->sid][$data->target_sid] = TRUE;
    }

    // Delete the transitions.
    foreach ($preexisting as $from => $array) {
      foreach (array_keys($array) as $target_id) {
        if ($transition = workflow_get_workflow_transitions_by_sid_target_sid($from, $target_id)) {
          workflow_delete_workflow_transitions_by_tid($transition->tid);
        }
      }
    }

    // Delete the state. -- We don't actually delete, just deactivate.
    // This is a matter up for some debate, to delete or not to delete, since this
    // causes name conflicts for states. In the meantime, we just stick with what we know.
    // If you really want to delete the states, use workflow_cleanup module, or delete().
    $this->status = FALSE;
    $this->save();

    // Clear the cache.
    self::$states = array();
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
    return isset($this->workflow) ? $this->workflow : Workflow::load($this->wid);
  }

  /**
   * Returns the Workflow object of this State.
   *
   * @return bool
   *  TRUE if state is active, else FALSE.
   */
  public function isActive() {
    return (bool) $this->status;
  }

  public function isCreationState() {
    return $this->sysid == WORKFLOW_CREATION;
  }

  /**
   * Determine if the Workflow Form must be shown. 
   * If not, a formatter must be shown, since there are no valid options.
   *
   * @param array $options
   *   an array with $id => $label options, as determined in WorkflowState->getOptions().
   * 
   * @return bool $show_widget
   *   TRUE = a form (a.k.a. widget) must be shown; FALSE = no form, a formatter must be shown instead.
   */
  public function showWidget(array $options) {
    $count = count($options);
    // The easiest case first: more then one option: always show form.
    if ($count > 1) {
      return TRUE;
    }
    // Only when in creation phase, one option is sufficient,
    // since the '(creation)' option is not included in $options.
    if ($this->isCreationState()) {
      return TRUE;
    }
    return FALSE;
  }

  /**
   * Returns the allowed values for the current state.
   *
   * @deprecated workflow_field_choices() --> WorkflowState->getOptions()
   */
  public function getOptions($entity_type, $entity, $force = FALSE) {
    global $user;
    static $cache = array(); // Entity-specific cache per page load.

    $options = array();

    if (!$entity) {
      // If no entity is given, no result (e.g., on a Field settings page).
      $options = array();
      return $options;
    }

    $entity_id = entity_id($entity_type, $entity);
    $current_sid = $this->sid;
    // Get options from page cache.
    if (isset($cache[$entity_type][$entity_id][$force][$current_sid])) {
      $options = $cache[$entity_type][$entity_id][$force][$current_sid];
      return $options;
    }

    $workflow = Workflow::load($this->wid);
    if ($workflow) {
      $roles = array_keys($user->roles);

      // If this is a new page, give the authorship role.
      if (!$entity_id) {
        $roles = array_merge(array('author'), $roles);
      }
      // Add 'author' role to user if user is author of this entity.
      // - Some entities (e.g, taxonomy_term) do not have a uid.
      // - If 'anonymous' is the author, don't allow access to History Tab,
      //   since anyone can access it, and it will be published in Search engines. 
      elseif (isset($entity->uid) && $entity->uid == $user->uid && $user->uid > 0) {
        $roles = array_merge(array('author'), $roles);
      }

      // Superuser is special. And $force allows Rules to cause transition.
      if ($user->uid == 1 || $force) {
        $roles = 'ALL';
      }

      // Workflow_allowable_transitions() does not return the entire transition row. Would like it to, but doesn't.
      // Instead it returns just the allowable data as:
      // [tid] => 1 [state_id] => 1 [state_name] => (creation) [state_weight] => -50
      $transitions = workflow_allowable_transitions($current_sid, 'to', $roles);

      // Include current state if it is not the (creation) state.
      foreach ($transitions as $transition) {
        if ($transition->sysid != WORKFLOW_CREATION && !$force) {
          // Invoke a callback indicating that we are collecting state choices.
          // Modules may veto a choice by returning FALSE.
          // In this case, the choice is never presented to the user.
          // @todo: for better performance, call a hook only once: can we find a way to pass all transitions at once
          $result = module_invoke_all('workflow', 'transition permitted', $current_sid, $transition->state_id, $entity, $field_name = '');
          // Did anybody veto this choice?
          if (!in_array(FALSE, $result)) {
            // If not vetoed, add to list.
            $options[$transition->state_id] = check_plain(t($transition->state_name));
          }
        }
      }

      // Save to entity-specific cache.
      $cache[$entity_type][$entity_id][$force][$current_sid] = $options;
    }

    return $options;
  }

  /**
   * Returns the number of entities with this state.
   *
   * @return integer
   *  counted number.
   * @todo: add $options, to select on entity type, etc.
   */
  public function count() {
    $sid = $this->sid;
    // Get the number for Workflow Node.
    $result = db_select('workflow_node', 'wn')
      ->fields('wn')
      ->condition('sid', $sid,'=')
      ->execute();
    $count = $result->rowCount();

    // Get the numbers for Workflow Field.
    $fields = field_info_field_map();
    foreach($fields as $field_name => $field_map) {
      if ($field_map['type'] == 'workflow') {
        $query = new EntityFieldQuery();
        $query
          ->fieldCondition($field_name, 'value', $sid, '=')
          //->entityCondition('bundle', 'article')
          // ->addMetaData('account', user_load(1)) // Run the query as user 1.
          ->count(); // We only need the count.

        $result = $query->execute();
        $count += $result;
      }
    }

    return $count;
  }

  /**
   * Mimics Entity API functions.
   */
  public function label($langcode = NULL) {
    return t($this->state, $args = array(), $options = array('langcode' => $langcode));
  }
  public function getName() {
    return $this->state;
  }
  public function setName($name) {
    return $this->state = $name;
  }
  public function value() {
    return $this->sid;
  }

}
