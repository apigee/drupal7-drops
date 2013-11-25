<?php

/**
 * @file
 * This is now effectively a light-weight duplicate of FAPE.module, providing
 * just the parts Edit needs.
 */

/**
 * Form constructor; in-place editing form for a (single) Field API field.
 *
 * This isn't a true form. As such it modifies the $form by reference.
 *
 * This leverages http://drupal.org/node/1825844.
 *
 * @see edit_field_edit_form_validate()
 * @see edit_field_edit_form_submit()
 * @ingroup forms
 */
function edit_field_edit_form(&$form, &$form_state) {
  $entity_type = $form_state['entity_type'];
  $entity = $form_state['entity'];
  $field_name = $form_state['field_name'];
  $langcode = $form_state['langcode'];
  $bundle = $form_state['bundle'];

  // Adds the form elements for only the specified field to the form.
  $options = array(
    'field_name' => $field_name,
    'language' => field_valid_language($langcode),
    'default' => TRUE,
  );
  field_attach_form($entity_type, $entity, $form, $form_state, $langcode, $options);

  // @todo make this not node-specific
  if ($entity_type === 'node') {
    $entity->revision = in_array('revision', variable_get('node_options_' . $bundle, array()));
    $entity->log = NULL;
  }

  $form['#validate'][] = 'edit_field_edit_form_validate';
  $form['#submit'][] = 'edit_field_edit_form_submit';
}

/**
 * Form validation handler for edit_field_edit_form().
 *
 * @see edit_field_edit_form()
 * @see edit_field_edit_form_submit()
 * @see \Drupal\edit\Form\EditFieldForm::validate() in Drupal 8.
 */
function edit_field_edit_form_validate($form, &$form_state) {
  $entity = _edit_build_entity($form, $form_state);
  field_attach_form_validate($form_state['entity_type'], $entity, $form, $form_state, array('field_name' =>  $form_state['field_name']));
}

/**
 * Form submission handler for edit_field_edit_form().
 *
 * @see edit_field_edit_form()
 * @see edit_field_edit_form_validate()
 * @see edit_field_edit()
 * @see \Drupal\edit\Form\EditFieldForm::submit() in Drupal 8.
 */
function edit_field_edit_form_submit(&$form, &$form_state) {
  $entity = _edit_build_entity($form, $form_state);
  entity_save($form_state['entity_type'], $entity);

  // Update the entity in the form state, so that edit_field_edit() can use it
  // to render the updated field.
  $form_state['entity'] = $entity;
}

/**
 * Returns a cloned entity containing updated field values.
 *
 * Calling code may then validate the returned entity, and if valid, transfer
 * it back to the form state and save it.
 *
 * @see \Drupal\edit\Form\EditFieldForm::buildEntity() in Drupal 8.
 */
function _edit_build_entity($form, &$form_state) {
  $entity = clone $form_state['entity'];

  // Extract form values. (Renamed to field_attach_extract_form_values() in D8.)
  field_attach_submit($form_state['entity_type'], $entity, $form, $form_state, array('field_name' =>  $form_state['field_name']));

  $instance = field_info_instance($form_state['entity_type'], $form_state['field_name'], $form_state['bundle']);
  _edit_entity_set_log($entity, $instance['label']);

  return $entity;
}

/**
 * Sets a revision log message for entities configured to create a new revision.
 */
function _edit_entity_set_log(&$entity, $field_name) {
  if ($entity->revision) {
    $entity->log = t('Updated the %field-name field through in-place editing.', array('%field-name' => $field_name));
  }
}
