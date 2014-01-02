<?php
/**
 * @file
 * Rules integration for the Workflow module
 */

/*
 * Include the Condition and Actions for Nodes and Entity.
 * They are in separate files, but must be kept in sync.
 * They contain separate logic for the 'conventional' Workflow Node API
 * and the 'new' Workfow Field API.
 */
module_load_include('inc', 'workflow_rules', 'workflow_rules.node');
module_load_include('inc', 'workflow_rules', 'workflow_rules.field');

/**
 * Implements hook_rules_event_info().
 *
 * @todo: add support for any entity type in hook_rules_event_info.
 */
function workflow_rules_rules_event_info() {
  $label = t('updated content');
  // Add variables for the 'node' type.
  $node_variables = rules_events_node_variables($label, TRUE);

  $events = array(
    'workflow_state_changed' => array(
      'group' => t('Workflow'),
      'label' => t('Workflow state has changed'),
      'variables' => $node_variables,
    ),
    'workflow_comment_added' => array(
      'group' => t('Workflow'),
      'label' => t('Workflow comment was added, but state did not change'),
      'variables' => $node_variables,
    ),
  );
  return $events;
}

/**
 * Implements hook_rules_condition_info().
 */
function workflow_rules_rules_condition_info() {
  $conditions = array();
  if (module_exists('workflownode')) {
    $conditions += _workflow_rules_rules_node_condition_info();
  }
  if (module_exists('workflowfield')) {
    $conditions += _workflow_rules_rules_field_condition_info();
  }
  return $conditions;
}

/**
 * Implements hook_rules_action_info().
 */
function workflow_rules_rules_action_info() {
  $actions = array();
  if (module_exists('workflownode')) {
    $actions += _workflow_rules_rules_node_action_info();
  }
  if (module_exists('workflowfield')) {
    $actions += _workflow_rules_rules_field_action_info();
  }
  return $actions;
}

/*
 * Helper function to parse a token "node:<field_name>".
 * @param string $token
 * @return string $field_name
 */
function _workflow_rules_token_replace($field_name) {
  // This is a poorman's effort to convert a token into a field name.
  $field_name = str_replace('node:', '', $field_name);
  $field_name = str_replace('field:', '', $field_name);
  $field_name = str_replace('-', '_', $field_name);
  return $field_name;
}

/**
 * Condition callback: gather all workflow states, to show in list_options.
 */
function _workflow_rules_workflow_get_options($data) {
  $wid = 0;
  // This is a poorman's effort to convert a token into a field name.
  $token = isset($data->settings['field:select']) ? $data->settings['field:select'] : '';
  $field_name = _workflow_rules_token_replace($token);
  $field = field_info_field($field_name);
  $wid = !empty($field) ? $field['settings']['wid'] : 0;

  $options['ANY'] = 'ANY state';
  $options += workflow_get_workflow_state_names($wid, $grouped = TRUE);
  return $options;
}

/**
 * Condition implementation helper function: check given state.
 *
 * @param mixed $sid
 *   a State ID, to compare with the given list of allowed State ID's
 * @param array $sids
 *   a list of allowed State ID's
 *
 * @return bool
 *   TRUE or FALSE
 */
function _workflow_rules_workflow_check_given_state($sid, array $sids) {
  return in_array('ANY', $sids) || in_array($sid, $sids);
}


/**
 * Condition implementation: check state transition for Workflow Node API.
 * 
 * @param object $node
 *   the node with the new values. Other entity types are not supported.
 * @param array $old_sids
 *   an array of old sids to meet the condition.
 * @param array $new_sids
 *   an array of new sids to meet the condition.
 * @param array $condition
 *   a RulesCondition->settings array.
 */
function _workflow_rules_node_check_transition($node, array $old_sids, array $new_sids, array $condition) {
  $transitions = WorkflowTransition::load('node', $node->nid, '', 1);
  $last_transition = array_pop($transitions);
  $old_sid = $last_transition->old_sid;
  $new_sid = $last_transition->new_sid;

  return _workflow_rules_workflow_check_given_state($old_sid, $old_sids) &&
         _workflow_rules_workflow_check_given_state($new_sid, $new_sids);
}

/**
 * Condition implementation: check current state for Workflow Node API.
 * 
 * @param object $node
 *   the node with the new values. Other entity types are not supported.
 * @param array $sids
 *   an array of sids to meet the condition.
 */
function _workflow_rules_node_check_state($node, $sids) {
  $sid = workflow_node_current_state($node, 'node');
  return _workflow_rules_workflow_check_given_state($sid, $sids);
}

/**
 * Condition implementation: check previous state.
 */
function _workflow_rules_node_check_previous_state($node, $sids) {
  // $sid = workflow_node_previous_state($node, 'node');
  $transitions = WorkflowTransition::load('node', $node->nid, '', 1);
  $last_transition = array_pop($transitions);
  $sid = $last_transition->old_sid;
  return _workflow_rules_workflow_check_given_state($sid, $sids);
}

/**
 * Action implementation: set current state, ignoring current user permission.
 */
function _workflow_rules_node_set_state($node, $sids, $comment = NULL) {
  // Select the last state on the list.
  $sid = array_pop($sids);
  if (!empty($comment)) {
    $node->workflow_comment = $comment;
  }
  workflow_transition($node, $sid, TRUE);
  unset($node->workflow_comment);
}

/**
 * Action implementation: set current state, ignoring current user permission.
 */
function _workflow_rules_field_set_state($entity, $field, $sids, $comment = NULL, array $settings = array()) {
  global $user;

  // Select the last state on the list.
  $new_sid = array_pop($sids);

  // This is a poorman's effort to convert a token into a field name.
  $token = isset($settings['field:select']) ? $settings['field:select'] : '';
  $field_name = _workflow_rules_token_replace($token);
  $instance = array();
  if ($field_name && isset($entity->{$field_name})) {
    $entity_type = 'node'; // @todo: add Entity support in workflow_rules.
    $entity_id = entity_id($entity_type, $entity);

    $old_sid = workflow_node_current_state($entity, $entity_type, $field_name); // Todo : entity support.
    $transition = new WorkflowTransition($entity_type, $entity, $field_name, $old_sid, $new_sid, $user->uid, REQUEST_TIME, $comment);
    $transition->force($force = TRUE); // "ignoring current user permission."

    // Execute the transition. It may bounce, due to extra checks.
    $sid = $transition->execute($force);
    // Save the entity, but not through entity_save(), since this will check permissions again and trigger rules.
    if ($sid == $new_sid) {
      // The transition was successfully executed, or else a message was raised.
      $entity->{$field_name}['und'] = array();
      $entity->{$field_name}['und'][0]['workflow']['workflow_sid'] = $new_sid;
      $entity->{$field_name}['und'][0]['workflow']['workflow_comment'] = $comment;
      field_attach_update($entity_type, $entity);
    }
  }
}
