<?php

/**
 * @file
 * Defines a Workflow formatter.
 * You won't find a DefaultFormatter, because:
 * - The 'default' formatter provided by the List module;
 * - The 'workflow' formatter is only representing the WorkflowDefault Widget.
 *
 * All hooks are wrapper functions for a D8-style WorkflowDefaultWidget object.
 */

/**
 * Implements hook_field_formatter_info().
 */
function workflowfield_field_formatter_info() {
  return WorkflowDefaultWidget::settings();
}

/**
 * Implements hook_field_formatter_view().
 *
 * Shows the current state and the Widget on a Node View page or a Workflow History tab.
 */
function workflowfield_field_formatter_view($entity_type, $entity, $field, $instance, $langcode = 'und', $items = array(), $display = array()) {
  $field_name = isset($field['field_name']) ? $field['field_name'] : '';
  $field_id = isset($field['id']) ? $field['id'] : 0;

  $current_sid = workflow_node_current_state($entity, $entity_type, $field_name);
  $current_state = workflow_state_load_single($current_sid);

  if ($field_name) {
    // First compose the current value with the normal formatter from list.module.
    $list_element = workflow_state_formatter($entity_type, $entity, $field, $instance);
  }
  elseif (!empty($field['settings']['widget']['current_status'])) {
    $list_element = workflow_state_formatter($entity_type, $entity, $field, $instance);
  }

  if ($entity_type == 'comment' || // No Workflow form on a comment display, (Also, this avoids a lot of error messages.)
      (isset($entity->status) && $entity->status == 0) || // or if the entity is not published (not possible for every entity type),  
      !user_access('show workflow state form') // or if the user has no access to Workflow Form.
      ) {
    return $list_element;
  }

  // Only build form if user has possible target state(s).
  $options = $current_state->getOptions($entity_type, $entity, $force = FALSE);
  if (!$current_state->showWidget($options)) {
    return $list_element;
  }

  $workflow = $current_state->getWorkflow();

  // Add some data for the form.
  // Add the current value to the form.
  // Make sure the current value is before the form. (which has weight = 0.005)
  $list_element['#weight'] = 0;
  $element['workflow_current_state'] = $list_element;

  // Add the form/widget to the formatter, and include the nid in the form id,
  // to allow multiple forms per page (in listings, with hook_forms() ).
  // Ultimately, this is a wrapper for WorkflowDefaultWidget.
  $entity_id = entity_id($entity_type, $entity);
  $element += drupal_get_form(implode('_', array('workflow_transition_form', $entity_type, $entity_id, $field_id)), $field, $instance, $entity_type, $entity);

  return $element;
}
