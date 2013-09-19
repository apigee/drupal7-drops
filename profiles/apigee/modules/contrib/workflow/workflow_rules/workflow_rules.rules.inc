<?php
/**
 * @file
 * Rules integration for the Workflow module
 */

/**
 * Implements hook_rules_event_info().
 */
function workflow_rules_rules_event_info() {
  $events = array(
    'workflow_state_changed' => array(
      'group' => t('Workflow'),
      'label' => t('Workflow state has changed'),
      'variables' => rules_events_node_variables(t('updated content'), TRUE),
    ),
    'workflow_comment_added' => array(
      'group' => t('Workflow'),
      'label' => t('Workflow comment was added, but state did not change'),
      'variables' => rules_events_node_variables(t('updated content'), TRUE),
    ),
  );
  return $events;
}

/**
 * Implements hook_rules_condition_info().
 */
function workflow_rules_rules_condition_info() {
  return array(
    'workflow_check_transition' => array(
      'group' => t('Workflow'),
      'label' => t('Check workflow transition'),
      'parameter' => array(
        'node' => array(
          'type' => 'node',
          'label' => t('Node'),
          'description' => t('The node whose workflow state is being checked.'),
        ),
        'old_state' => array(
          'type' => 'list<integer>',
          'label' => t('Old workflow state'),
          'options list' => '_workflow_rules_condition_select',
          'description' => t('The workflow state moved from.'),
        ),
        'new_state' => array(
          'type' => 'list<integer>',
          'label' => t('New workflow state'),
          'options list' => '_workflow_rules_condition_select',
          'description' => t('The workflow state moved to.'),
        ),
      ),
    ),
    'workflow_check_state' => array(
      'group' => t('Workflow'),
      'label' => t('Content has a workflow state'),
      'parameter' => array(
        'node' => array(
          'type' => 'node',
          'label' => t('Node'),
          'description' => t('The node to compare the current workflow state of.'),
        ),
        'workflow_state' => array(
          'type' => 'list<integer>',
          'label' => t('Compare workflow state'),
          'options list' => '_workflow_rules_condition_select',
          'description' => t('The possible workflow states to compare against.'),
        ),
      ),
    ),
    'workflow_check_previous_state' => array(
      'group' => t('Workflow'),
      'label' => t('Content has a previous workflow state'),
      'parameter' => array(
        'node' => array(
          'type' => 'node',
          'label' => t('Node'),
          'description' => t('The node to compare the previous workflow state of.'),
        ),
        'workflow_state' => array(
          'type' => 'list<integer>',
          'label' => t('Compare workflow state'),
          'options list' => '_workflow_rules_condition_select',
          'description' => t('The possible workflow states to compare against.'),
        ),
      ),
    ),
  );
}

/**
 * Implements hook_rules_action_info().
 */
function workflow_rules_rules_action_info() {
  return array(
    'workflow_rules_set_state' => array(
      'group' => t('Workflow'),
      'label' => t('Set workflow state for content'),
      'parameter' => array(
        'node' => array(
          'type' => 'node',
          'label' => t('Node'),
          'description' => t('The node to set the current workflow state of.'),
//          'save' => TRUE,
        ),
        'workflow_state' => array(
          'type' => 'list<integer>',
          'label' => t('New workflow state'),
          'options list' => '_workflow_rules_action_select',
          'description' => t('The workflow state to set (select only one).'),
        ),
        'workflow_comment' => array(
          'type' => 'text',
          'label' => t('Workflow Comment'),
          'description' => t('The workflow comment to set.'),
          'optional' => TRUE,
        ),
      ),
    ),
  );
}

/**
 * Condition callback: gather all workflow states.
 */
function _workflow_rules_condition_select() {
  $options['ANY'] = 'ANY state';
  foreach (workflow_get_workflows() as $workflow) {
    foreach (workflow_get_workflow_states_by_wid($workflow->wid) as $state) {
      $states[$state->sid] = check_plain($workflow->name) . ': ' . check_plain($state->state);
    }
  }
  $options = $options + $states;
  return $options;
}

/**
 * Condition callback: gather all workflow states.
 */
function _workflow_rules_action_select() {
  foreach (workflow_get_workflows() as $workflow) {
    foreach (workflow_get_workflow_states_by_wid($workflow->wid) as $state) {
      $states[$state->sid] = check_plain($workflow->name) . ': ' . check_plain($state->state);
    }
  }
  return $states;
}

/**
 * Condition implementation: check state transition.
 */
function workflow_check_transition($node, $old_states, $new_states) {
  $node_current_state = workflow_node_current_state($node);
  $node_old_state = workflow_node_previous_state($node);

  if (in_array('ANY', $old_states)) {
    if (in_array('ANY', $new_states)) {
      return TRUE;
    }
    return in_array($node_current_state, $new_states);
  }

  if (in_array('ANY', $new_states)) {
    return in_array($node_old_state, $old_states);
  }
  return in_array($node_old_state, $old_states) && in_array($node_current_state, $new_states);
}

/**
 * Condition implementation: check current state.
 */
function workflow_check_state($node, $states) {
  $node_state = workflow_node_current_state($node);
  return workflow_check_given_state($node, $states, $node_state);
}

/**
 * Condition implementation: check previous state.
 */
function workflow_check_previous_state($node, $states) {
  $node_state = workflow_node_previous_state($node);
  return workflow_check_given_state($node, $states, $node_state);
}

/**
 * Condition implementation helper function: check given state.
 */
function workflow_check_given_state($node, $states, $node_state) {
  if (in_array('ANY', $states)) {
    return TRUE;
  }

  if (in_array($node_state, $states)) {
    return TRUE;
  }
  return FALSE;
}

/**
 * Action implementation: set current state, ignoring current user permissione.
 */
function workflow_rules_set_state($node, $states, $comment = NULL) {
  // Select the last state on the list.
  $sid = array_pop($states);
  if (!empty($comment)) {
    $node->workflow_comment = $comment;
  }
  workflow_transition($node, $sid, TRUE);
  unset($node->workflow_comment);
}
