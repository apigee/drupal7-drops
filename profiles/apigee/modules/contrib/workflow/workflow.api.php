<?php
/**
 * @file
 * Hooks provided by the workflow module.
 */

/**
 * Implements hook_workflow().
 *
 * @param $op
 *   The current workflow operation: 'transition permitted', 'transition pre' or 'transition post'.
 * @param $old_state
 *   The state ID of the current state.
 * @param $new_state
 *   The state ID of the new state.
 * @param $node
 *   The node whose workflow state is changing.
 * @param $force
 *   The caller indicated that the transition should be forced. (bool).
 *   This is only available on the "pre" and "post" calls.
 */
function hook_workflow($op, $old_state, $new_state, $node, $force = FALSE) {
  switch ($op) {
    case 'transition permitted':
      // The workflow module does nothing during this operation.
      // This operation occurs when the list of available transitions
      // is built. Your module's implementation could return FALSE
      // here and disallow the presentation of the choice.
      break;

    case 'transition pre':
      // The workflow module does nothing during this operation.
      // But your module's implementation of the workflow hook could
      // return FALSE here and veto the transition.
      break;

    case 'transition post':
      break;

    case 'transition delete':
      break;
  }
}

/**
 * Implements hook_workflow_history_alter().
 * Add an 'undo' operation for the most recent history change.
 *
 * @param $variables
 *   The current workflow history information as an array.
 *   'old_sid' - The state ID of the previous state.
 *   'old_state_name' - The state name of the previous state.
 *   'sid' - The state ID of the current state.
 *   'state_name' - The state name of the current state.
 *   'history' - The row from the workflow_node_history table.
 *
 * If you want to add additional data, such as an operation link,
 * place it in the 'extra' value.
 */
function hook_workflow_history_alter(&$variables) {
  // The Worflow module does nothing with this hook.
  // For an example implementation, see the Workflow Revert add-on.
}
