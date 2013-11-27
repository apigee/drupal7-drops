<?php

/**
 * @file
 * Hooks provided by the workflow module.
 */

/**
 * Implements hook_workflow().
 */
function workflownode_workflow($op, $id, $new_sid, $entity, $force, $entity_type = '', $field_name = '') {
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
      // A transition is deleted Only the first parameter is used.
      $tid = $id;
      break;

    case 'state delete':
      $current_sid = $id;
      break;

    case 'workflow delete':
      // Only the first parameter is used.
      $wid = $id;
      workflow_delete_workflow_type_map_by_wid($wid);
      break;
  }
}

/**
 * Implements hook_workflow_history_alter().
 */
// function workflownode_workflow_history_alter(array &$variables) {
  // The Workflow module does nothing with this hook.
  // For an example implementation, see the Workflow Revert add-on.
// }
