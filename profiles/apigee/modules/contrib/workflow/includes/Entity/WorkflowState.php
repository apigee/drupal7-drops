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

  /**
   * CRUD functions.
   */

  /**
   * Constructor.
   */
  protected function __construct($wid = 0, $name = '') {
    if (empty($wid) && empty($name)) {
      // Automatic constructor when casting an array or object.
      if (!isset(self::$states[$this->sid])) {
        self::$states[$this->sid] = $this;
      }
    }
    elseif ($wid && empty($name)) {
      // Creating a dummy/new state for a workflow.
      // Do not add to 'cache' self::$states.
      $this->wid = $wid;
      $this->state = $name;
    }
    else {
      // Creating a dummy/new state for a workflow.
      // Do not add to 'cache' self::$states.
      $this->wid = $wid;
      $this->state = $name;
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
    $state = workflow_state_load_by_name($name, $wid);
    if (!$state) {
      $state = new WorkflowState($wid, $name);
      $state->state = $name;
    }
    if ($name == WORKFLOW_CREATION_STATE_NAME) {
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
   * @deprecated workflow_get_workflow_states --> workflow_state_load_multiple
   * @deprecated workflow_get_workflow_states_all --> workflow_state_load_multiple
   * @deprecated workflow_get_other_states_by_sid --> workflow_state_load_multiple
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
  public static function loadByName($name, $wid = 0) {
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

    if (!empty($this->sid) && workflow_state_load_single($sid)) {
      drupal_write_record('workflow_states', $data, 'sid');
    }
    else {
      drupal_write_record('workflow_states', $data);
    }
    // Update the page cache.
    $this->sid = $sid = $data['sid'];
    self::$states[$sid] = $this;

    // Reset the cache for the affected workflow.
    workflow_reset_cache($this->wid);
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
        $transition = new WorkflowTransition();
        $transition->setValues($entity_type, $entity, $field_name, $current_sid, $new_sid, $user->uid, REQUEST_TIME, $comment);
        $transition->force($force); 
        // Execute Transition, invoke 'pre' and 'post' events, save new state in workflow_node, save also in workflow_node_history.
        // For Workflow Node, only {workflow_node} and {workflow_node_history} are updated. For Field, also the Entity itself.
        $new_sid = workflow_execute_transition($entity_type, $entity, $field_name, $transition, $force);
      }
    }
    // Delete any lingering node to state values.
    workflow_delete_workflow_node_by_sid($current_sid);

    // Delete the transitions this state is involved in.
    $workflow = workflow_load_single($this->wid);
    foreach ($transitions = $workflow->getTransitionsBySid($current_sid, 'ALL') as $transition) {
      $transition->delete();
    }
    foreach ($transitions = $workflow->getTransitionsByTargetSid($current_sid, 'ALL') as $transition) {
      $transition->delete();
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
    return workflow_load_single($this->wid);
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

    // Entity may not be given, (e.g., on a Field settings page).
    $entity_id = (!$entity) ? 'x' : entity_id($entity_type, $entity);

    $current_sid = $this->sid;
    // Get options from page cache.
    if (isset($cache[$entity_type][$entity_id][$force][$current_sid])) {
      $options = $cache[$entity_type][$entity_id][$force][$current_sid];
      return $options;
    }

    $workflow = workflow_load_single($this->wid);
    if ($workflow) {
      // Gather roles, to get the proper permissions.
      $roles = array_keys($user->roles);

      if ($user->uid == 1 || $force) {
        // Superuser is special. And $force allows Rules to cause transition.
        $roles = 'ALL';
      }
      elseif (!$entity_id) {
        // If this is a new page, give the authorship role.
        $roles = array_merge(array(WORKFLOW_ROLE_AUTHOR_RID), $roles);
      }
      elseif (isset($entity->uid) && $entity->uid == $user->uid && $user->uid > 0) {
        // Add 'author' role to user if user is author of this entity.
        // - Some entities (e.g, taxonomy_term) do not have a uid.
        // - If 'anonymous' is the author, don't allow access to History Tab,
        //   since anyone can access it, and it will be published in Search engines. 
        $roles = array_merge(array(WORKFLOW_ROLE_AUTHOR_RID), $roles);
      }

      // Set up an array with states - they are already properly sorted.
      // Unfortunately, the config_transitions are not sorted.
      // Also, $transitions does not contain the 'stay on current state' transition.
      // The object will be replaced with names.
      $options = $workflow->getStates();
      $transitions = $workflow->getTransitionsBySid($current_sid, $roles);
      foreach ($transitions as $transition) {
        $new_sid = $transition->target_sid;

        // We now have a list of config_transitions. Check them against the Entity.
        // Invoke a callback indicating that we are collecting state choices.
        // Modules may veto a choice by returning FALSE.
        // In this case, the choice is never presented to the user.
        // @todo: for better performance, call a hook only once: can we find a way to pass all transitions at once
        if ($roles == 'ALL') {
          $permitted = array();
        }
        else {
          $permitted = module_invoke_all('workflow', 'transition permitted', $current_sid, $new_sid, $entity, $force, $entity_type, $field_name = ''); // @todo: add $field_name.
        }
        // Stop if a module says so.
        if (!in_array(FALSE, $permitted, TRUE)) {
          // If not vetoed, add to list (by replacing the object by the name).
          if ($target_state = $workflow->getState($new_sid)) {
            $options[$new_sid] = check_plain(t($target_state->label()));
          }
        }
      }

      // Include current state for same-state transitions (by replacing the object by the name).
      if ($current_sid != $workflow->getCreationSid()) {
        if ($current_state = $workflow->getState($current_sid)) {
          $options[$current_sid] = check_plain(t($current_state->label()));
        }
      }

      // Remove the unpermitted options.
      foreach ($options as $key => $data) {
        if (is_object($data) ) {
          unset($options[$key]);
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
    foreach ($fields as $field_name => $field_map) {
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
