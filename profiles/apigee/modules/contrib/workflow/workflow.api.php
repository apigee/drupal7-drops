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
 *
 * @return
 *   Only 'transition permitted' expects a boolean result.
 */
function hook_workflow($op, $id, $new_sid, $entity, $force, $entity_type = '', $field_name = '') {
  switch ($op) {
    case 'transition permitted':
      // This operation is called in the following situations: 
      // 1. when executing a transition, just before the 'transition pre';
      // 2. when the list of available transitions in built;
      // Your module's implementation may return FALSE here and disallow
      // the execution, or avoid the presentation of the new State.
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
