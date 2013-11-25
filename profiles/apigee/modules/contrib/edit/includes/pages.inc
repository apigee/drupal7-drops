<?php
/**
 * @file
 * AJAX endpoint to retrieve & save subforms for fields and re-render fields.
 */

/**
 * Page callback: Returns the metadata for a set of fields.
 *
 * Given a list of field edit IDs as POST parameters, run access checks on the
 * entity and field level to determine whether the current user may edit them.
 * Also retrieves other metadata.
 *
 * @return
 *   The JSON response.
 *
 * @see Drupal 8's EditController::metadata()
 * @see EditMetadataGenerator
 * @see EditEntityFieldAccessCheck
 * @see EditEditorSelector
 */
function edit_metadata() {
  $fields = $_POST['fields'];
  if (!isset($fields)) {
    return MENU_NOT_FOUND;
  }
  module_load_include('php', 'edit', 'includes/EditEntityFieldAccessCheck');
  module_load_include('php', 'edit', 'includes/EditEditorSelector');
  module_load_include('php', 'edit', 'includes/EditMetadataGenerator');
  $accessChecker = new EditEntityFieldAccessCheck();
  $editorSelector = new EditEditorSelector();
  $metadataGenerator = new EditMetadataGenerator($accessChecker, $editorSelector);

  // Build metadata for each field, track all in-place editors.
  $metadata = array();
  $editors = array();
  foreach ($fields as $field) {
    list($entity_type, $entity_id, $field_name, $langcode, $view_mode) = explode('/', $field);

    // Load the entity.
    if (!$entity_type || !entity_get_info($entity_type)) {
      return MENU_NOT_FOUND;
    }
    $entity = entity_load_single($entity_type, $entity_id);
    if (!$entity) {
      return MENU_NOT_FOUND;
    }
    list($id, $vid, $bundle) = entity_extract_ids($entity_type, $entity);

    // Validate the field name and language.
    if (!_edit_is_extra_field($entity_type, $field_name)) {
      if (!$field_name || !($instance = field_info_instance($entity_type, $field_name, $bundle))) {
        return MENU_NOT_FOUND;
      }
    }
    else {
      $instance = array('field_name' => $field_name);
    }
    if (!$langcode || (field_valid_language($langcode) !== $langcode)) {
      return MENU_NOT_FOUND;
    }

    // Generate metadata for the current field.
    $metadata[$field] = $metadataGenerator->generate($entity_type, $entity, $instance, $langcode, $view_mode);

    // Track all editors.
    if ($metadata[$field]['access'] !== FALSE) {
      $editors[] = $metadata[$field]['editor'];
    }
  }

  // Build response.
  $commands = array();
  $commands[] = array(
    'command' => 'editMetadata',
    'results' => $metadata,
  );

  // Ensure an AJAX command is generated to load in-place editor attachments.
  $elements['#attached'] = $editorSelector->getEditorAttachments($editors, $metadata);
  drupal_process_attached($elements);

  return array('#type' => 'ajax', '#commands' => $commands);
}

/**
 * Page callback: Returns a single field edit form as an Ajax response.
 *
 * @param $entity
 *   The entity being edited.
 * @param string $field_name
 *   The name of the field that is being edited.
 * @param string $langcode
 *   The name of the language for which the field is being edited.
 * @param string $view_mode
 *   The view mode the field should be rerendered in.
 * @return
 *   The Ajax response.
 *
 * @see Drupal 8's EditController::fieldForm()
 */
function edit_field_edit($entity_type, $entity_id, $field_name, $langcode = NULL, $view_mode = NULL) {
  $commands = array();

  // Load entity whose field is to be edited.
  $entity = entity_load_single($entity_type, $entity_id);
  list($id, $vid, $bundle) = entity_extract_ids($entity_type, $entity);

  // Ensure the user is allowed to edit this field.
  module_load_include('php', 'edit', 'includes/EditEntityFieldAccessCheck');
  $accessChecker = new EditEntityFieldAccessCheck();
  if (!$accessChecker->accessEditEntityField($entity_type, $entity, $field_name)) {
    return MENU_ACCESS_DENIED;
  }

  // Determine which subform should be used, initialize form state, build form.
  $subform_id = 'edit_field_edit_form';
  if (_edit_is_extra_field($entity_type, $field_name)) {
    $subform_id = edit_extra_field_info($entity_type, $field_name, 'edit subform id');
  }
  $form_state = array(
    'entity_type' => $entity_type,
    'entity' => $entity,
    'field_name' => $field_name,
    'langcode' => $langcode,
    'view_mode' => $view_mode,
    'no_redirect' => TRUE,
    'bundle' => $bundle,
    'subform_id' => $subform_id,
  );
  $form = drupal_build_form('edit_field_form', $form_state);

  // If the form was successfully submitted (executed), then rerender the field.
  if (!empty($form_state['executed'])) {
    $entity = $form_state['entity'];

    if (!_edit_is_extra_field($entity_type, $field_name)) {
      // The form submission took care of saving the updated entity. Return the
      // updated view of the field.
      $entities = entity_load($entity_type, array($id), TRUE);
      $entity = reset($entities);
      $field = field_view_field($entity_type, $entity, $field_name, $view_mode, $langcode);
      $rerendered_field = drupal_render($field);
    }
    else {
      $rerender_callback = edit_extra_field_info($entity_type, $field_name, 'rerender callback');
      $result = $rerender_callback($entity);
      $rerendered_field = edit_wrap_pseudofield(
        $result['value'],
        "$entity_type/$id/$field_name/$langcode/$view_mode",
        $result['inline']
      );
    }

    $commands[] = array(
      'command' => 'editFieldFormSaved',
      'data' => $rerendered_field,
    );
  }
  // Otherwise, render the form. (And if it was insuccessfully submitted, there
  // will be form validation errors.)
  else {
    $commands[] = array(
      'command' => 'editFieldForm',
      'data' => drupal_render($form),
    );

    $errors = form_get_errors();
    if (count($errors)) {
      $commands[] = array(
        'command' => 'editFieldFormValidationErrors',
        'data' => theme('status_messages'),
      );
    }
  }

  // When working with a hidden form, we don't want any CSS or JS to be loaded.
  if (isset($_POST['nocssjs']) && $_POST['nocssjs'] === 'true') {
    drupal_static_reset('drupal_add_css');
    drupal_static_reset('drupal_add_js');
  }

  return array('#type' => 'ajax', '#commands' => $commands);
}

/**
 * Page callback: Returns an Ajax response to render a text field without
 * transformation filters.
 *
 * @param $entity
 *   The entity being edited.
 * @param string $field_name
 *   The name of the field that is being edited.
 * @param string $langcode
 *   The name of the language for which the field is being edited.
 * @param string $view_mode
 *   The view mode the field should be rerendered in.
 * @return
 *   The Ajax response.
 *
 * @see Drupal 8's EditorController::getUntransformedText()
 */
function edit_ckeditor_get_untransformed_text($entity_type, $entity_id, $field_name, $langcode = NULL, $view_mode = NULL) {
  $commands = array();

  $entity = entity_load_single($entity_type, $entity_id);
  list($id, $vid, $bundle) = entity_extract_ids($entity_type, $entity);

  // Ensure the user is allowed to edit this field.
  module_load_include('php', 'edit', 'includes/EditEntityFieldAccessCheck');
  $accessChecker = new EditEntityFieldAccessCheck();
  if (!$accessChecker->accessEditEntityField($entity_type, $entity, $field_name)) {
    return MENU_ACCESS_DENIED;
  }

  // Render the field in our custom display mode; retrieve the re-rendered
  // markup, this is what we're after.
  $field_output = field_view_field($entity_type, $entity, $field_name, 'edit-render-without-transformation-filters', $langcode);
  $output = $field_output[0]['#markup'];

  $commands[] = array(
    'command' => 'editCKEditorGetUntransformedText',
    'id' => "$entity_type/$id/$field_name/$langcode/$view_mode",
    'data' => $output,
  );

  return array('#type' => 'ajax', '#commands' => $commands);
}

/**
 * Form constructor; in-place editing form for a (single) Field API or "extra"
 * field.
 *
 * For Field API fields, the edit_field_edit_form subform will be used. For
 * "extra" fields, the subform indicated in hook_edit_extra_fields_info() will
 * be used.
 *
 * @see edit_field_edit_form()
 * @ingroup forms
 */
function edit_field_form($form, &$form_state) {
  $form['#parents'] = array();

  form_load_include($form_state, 'inc', 'edit', 'includes/fape');

  if ($form_state['subform_id'] && function_exists($form_state['subform_id'])) {
    $form_state['subform_id']($form, $form_state);
  }

  // Add a submit button. Give it a class for easy JavaScript targeting.
  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#attributes' => array('class' => array('edit-form-submit')),
  );

  // Remove http://drupal.org/project/metatag form elements.
  // @see http://drupal.org/node/1895142
  unset($form['#metatags']);

  // Tell http://drupal.org/project/redirect to not add form elements.
  // @see http://drupal.org/node/1935676
  $form['redirect'] = array();

  // Simplify it for optimal in-place use.
  edit_field_form_simplify($form, $form_state);
  return $form;
}

/**
 * Removes unneeded elements from the field from.
 *
 * @see Drupal 8's EditFieldForm::simplify().
 */
function edit_field_form_simplify(&$form, &$form_state) {
  $field_name = $form_state['field_name'];
  $langcode = $form_state['langcode'];

  if (_edit_is_extra_field($form_state['entity_type'], $field_name)) {
    $key = edit_extra_field_info($form_state['entity_type'], $field_name, 'form simplification element key');
    $widget_element = &$form[$key];
  }
  else {
    $widget_element = &$form[$field_name][$langcode];
  }

  // Hide the field label from displaying within the form, because JavaScript
  // displays the equivalent label that was provided within an HTML data
  // attribute of the field's display element outside of the form. Do this for
  // widgets without child elements (like Option widgets) as well as for ones
  // with per-delta elements. Skip single checkboxes, because their title is
  // key to their UI. Also skip widgets with multiple subelements, because in
  // that case, per-element labeling is informative.
  $num_children = count(element_children($widget_element));
  if ($num_children == 0 && $widget_element['#type'] != 'checkbox') {
    $widget_element['#title_display'] = 'invisible';
  }
  if ($num_children == 1 && isset($widget_element[0]['value'])) {
    // @todo While most widgets name their primary element 'value', not all
    //   do, so generalize this.
    $widget_element[0]['value']['#title_display'] = 'invisible';
  }

  // Adjust textarea elements to fit their content.
  if (isset($widget_element[0]['value']['#type']) && $widget_element[0]['value']['#type'] == 'textarea') {
    $lines = count(explode("\n", $widget_element[0]['value']['#default_value']));
    $widget_element[0]['value']['#rows'] = $lines + 1;
  }
}
