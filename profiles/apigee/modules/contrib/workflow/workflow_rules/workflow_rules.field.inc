<?php
/**
 * @file
 * Rules integration for the Workflow module with Entity API
 */

/**
 * Implements hook_rules_data_info().
 * depends on 'property_info' in hook_field_info().
 */
function workflow_rules_rules_data_info() {
  // No custom support for Workflow Field: use Entity functionality.
  return array();

  return array(
    'workflow' => array(
      'label' => t('workflow'),
//      'ui class' => 'RulesDataUIWorkflow',
      'wrap' => FALSE,
//      'iswrapped' => TRUE,
//      'property info' => workflow_rules_workflow_data_property_info(),
      'group' => t('Workflow'),
      'token type' => 'workflow',
    ),
  );
}

/**
 * Implements subfunction of hook_rules_condition_info().
 * Using "node:" and "node:unchanged", there is no need to create more conditions to check transitions.
 */
function _workflow_rules_rules_field_condition_info() {
  $conditions = array();

  // No custom support for Workflow Field: use Entity functionality.
  return conditions;

  $conditions['workflow_rules_field_check_state'] = array(
    'group' => t('Workflow'),
    'label' => t('Workflow Field state comparison'),
    'parameter' => array(
      'field' => array(
        'type' => WORKFLOWFIELD_PROPERTY_TYPE,
        'label' => t('Workflow state to compare'),
        'description' => t('The state to be compared. Use "node:" to compare the new state; use "node-unchanged:" to compare the previous state.'),
        'restriction' => 'selector',
//        'allow null' => TRUE,
      ),
      'sid' => array(
        'type' => 'list<integer>',
        'label' => t('Workflow state'),
        'options list' => '_workflow_rules_workflow_get_options',
        'description' => t('The workflow state value.'),
      ),
    ),
    'base' => '_workflow_rules_workflow_check_given_state',
    'callbacks' => array(
      'execute' => '_workflow_rules_workflow_check_given_state',
    ),
  );

  return $conditions;
}

/**
 * Implements subfunction of hook_rules_action_info().
 */
function _workflow_rules_rules_field_action_info() {
  $actions = array();

  // No custom support for Workflow Field: use Entity functionality.
  return $actions;

  $actions['workflow_rules_field_set_state'] = array(
    'group' => t('Workflow'),
    'label' => t('Set a Workflow Field state'),
    'parameter' => array(
      'node' => array(
        'type' => 'node',
        'label' => t('Node'),
        'description' => t('The node to set the current workflow state of.'),
//        'save' => TRUE,
      ),
      'field' => array(
        'type' => WORKFLOWFIELD_PROPERTY_TYPE,
        'label' => t('Workflow field to set'),
        'description' => t('The workflow field to set.'),
        'restriction' => 'selector',
//        'allow null' => TRUE,
      ),
      'workflow_state' => array(
        'type' => 'list<integer>',
        'label' => t('New workflow state'),
        'options list' => '_workflow_rules_workflow_get_options',
        'description' => t('The workflow state to set (select only one).'),
      ),
      'workflow_comment' => array(
        'type' => 'text',
        'label' => t('Workflow Comment'),
        'description' => t('The workflow comment to set.'),
        'optional' => TRUE,
      ),
    ),
    'base' => '_workflow_rules_field_set_state',
    'callbacks' => array(
      'execute' => '_workflow_rules_field_set_state',
    ),
  );
  return $actions;
}
