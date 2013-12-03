<?php

/**
 * @file
 * Defines a Workflow field, widget (from options.module) and formatter (from list field).
 * All hooks are wrapper functions for a D8-style WorkflowDefaultWidget object.
 */

/**
 * Implements hook_field_widget_info().
 */
function workflowfield_field_widget_info() {
  return WorkflowDefaultWidget::settings();
}

/**
 * Implements hook_field_widget_settings_form().
 */
function workflowfield_field_widget_settings_form($field, $instance) {
  $form = array();
  $form_state = array();

  // The form element is created by a D8-like object.
  $widget = new WorkflowDefaultWidget($field, $instance);
  return $widget->settingsForm($form, $form_state, $has_data = 0);
}

/**
 * Implements hook_field_widget_form().
 *
 * This is a wrapper function for the 'workflow form' Widget. $form is modified by reference.
 */
function workflowfield_field_widget_form(&$form, &$form_state, $field, $instance, $langcode, $items, $delta, $element) {
  if (isset($form_state['node'])) {
    $entity = $form_state['node'];
    $entity_type = 'node'; // comments are only supported on nodes.
  }
  elseif (isset($element['#entity_type'])) {
    // We are on an Entity add/edit page.
    $entity = $element['#entity'];
    $entity_type = $element['#entity_type']; // @todo: add support for other entity types.
  }

  if (!$entity) {
    // We are now on the Field settings page, so do nothing.
    return $element;
  }

  if ($entity_type == 'comment') {
    // On a comment form, get the referenced entity.
    $entity_type = 'node'; // Comments are only possible on nodes
    $entity = $form['#node'];
  }

  $widget = new WorkflowDefaultWidget($field, $instance, $entity_type, $entity);
  $element += $widget->formElement($items, $delta, $element, $langcode, $form, $form_state);

  return $element;
}
