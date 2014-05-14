<?php
/**
 * @file
 * Hooks provided by the workflow module.
 */

/**
 * Implements hook_workflow().
 *
 * NOTE: This hook may reside in the implementing module
 * or in a module.workflow.inc file.
 *
 * @param string $op
 *   The current workflow operation.
 *   E.g., 'transition permitted', 'transition pre' or 'transition post'.
 * @param mixed $id
 *   The ID of the current state/transition/workflow.
 * @param mixed $new_sid
 *   The state ID of the new state.
 * @param object $entity
 *   The entity whose workflow state is changing.
 * @param bool $force
 *   The caller indicated that the transition should be forced. (bool).
 *   This is only available on the "pre" and "post" calls.
 * @param string $entity_type
 *   The entity_type of the entity whose workflow state is changing.
 * @param string $field_name
 *   The name of the Workflow Field. Empty in case of Workflow Node.
 *   This is used when saving a state change of a Workflow Field.
 * @param $transition
 *   The transition, that contains all of the above.
 *    @todo D8: remove all other paramters.
 *
 * @return
 *   Only 'transition permitted' expects a boolean result.
 */
function hook_workflow($op, $id, $new_sid, $entity, $force, $entity_type = '', $field_name = '', $transition = NULL) {
  switch ($op) {
    case 'transition permitted':
      // This operation is called in the following situations: 
      // 1. when the widget with list of available transitions is built;
      // 2. when executing a transition, just before the 'transition pre';
      // 3. when showing a 'revert state' link in a Views display.
      // Your module's implementation may return FALSE here and disallow
      // the execution, or avoid the presentation of the new State.
      // As of 7.x-2.3, better use hook_workflow_permitted_state_transitions_alter() in option 1.
      // For options 2 and 3, the 'transition pre' gives an alternative.
      return TRUE;

    case 'transition pre':
      // The workflow module does nothing during this operation.
      // But your module's implementation of the workflow hook could
      // return FALSE here and veto the transition.
      break;

    case 'transition post':
      // This is called by Workflow Node during update of the state, directly
      // after updating {workflow_node}. Workflow Field does not call this,
      // since you can call an Entity event after saving the entity. 
      break;

    case 'transition delete':
      // A transition is deleted. Only the first parameter is used.
      //$tid = $id;
      break;

    case 'state delete':
      // A state is deleted. Only the first parameter is used.
      // $sid = $id;
      break;

    case 'workflow delete':
      // A workflow is deleted. Only the first parameter is used.
      // $wid = $id;
      break;
  }
}

/**
 * Implements hook_workflow_history_alter().
 * 
 * Allow other modules to add Operations to the most recent history change.
 * E.g., Workflow Revert implements an 'undo' operation.
 *
 * @param array $variables
 *   The current workflow history information as an array.
 *   'old_sid' - The state ID of the previous state.
 *   'old_state_name' - The state name of the previous state.
 *   'sid' - The state ID of the current state.
 *   'state_name' - The state name of the current state.
 *   'history' - The row from the workflow_node_history table.
 *   'transition' - a WorkflowTransition object, containing all of the above.
 *
 * If you want to add additional data, such as an operation link,
 * place it in the 'extra' value.
 */
function hook_workflow_history_alter(array &$variables) {
  // The Workflow module does nothing with this hook.
  // For an example implementation, see the Workflow Revert add-on.
  $options = array();
  $path = '<front>';
  $variables['extra'] = l(t('My new operation: go to frontpage'), $path, $options);
}

/**
 * Implements hook_workflow_comment_alter().
 * 
 * Allow other modules to change the user comment when saving a state change.
 *
 * @param $comment
 *   The comment of the current state transition.
 * @param array $context
 *   'transition' - The current transition itself.
 */
function hook_workflow_comment_alter(&$comment, &$context) {
  $transition = $context->transition;
  $comment = $transition->uid . 'says: ' . $comment;
}

/*
 * Implements hook_workflow_permitted_state_transitions_alter().
 *
 * @param array $transitions
 *  An array of allowed transitions from the current state (as provided in
 *  $context). They are already filtered by the settings in Admin UI.
 * @param array $context
 *  An array of relevant objects. Currently:
 *    $context = array(
 *      'entity_type' => $entity_type,
 *      'entity' => $entity,
 *      'field_name' => $field_name,
 *      'force' => $force,
 *      'workflow' => $workflow,
 *      'state' => $current_state,
 *      'user_roles' => $roles,
 *    );
 *
 * This hook in invoked in WorkflowState::getOptions().
 * This hooks allows you to add custom filtering of allowed target states,
 * add new custom states, change labels, etc.
 */
function hook_workflow_permitted_state_transitions_alter(&$transitions, $context) {
  // This example creates a new custom target state.
  $values = array(
    // Fixed values for new transition.
    'wid' => $context['workflow']->wid,
    'sid' => $context['state']->sid,

    // Custom values for new transition.
    // The ID must be an integer, due to db-table constraints.
    'target_sid' => '998',
    'label' => 'go to my new fantasy state',
  );
  $new_transition = new WorkflowConfigTransition($values);

  $transitions[] = $new_transition;
}
