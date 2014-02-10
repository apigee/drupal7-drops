<?php
/**
 * @file
 * Provides the type_map maintenance form.
 */

/**
 * Page builder. Show a maintenance table for type mapping.
 * 
 * Appends the type_map form, allowing administrator
 * to map workflows to content types and determine placement on content forms.
 */
function workflow_admin_ui_type_map_form($form) {
  // Create list of all Workflow types. Include an initial empty value.
  // Validate each workflow, and generate a message if not complete.
  $workflows[0] = t('None');
  foreach (workflow_load_multiple() as $workflow) {
    if ($is_valid = $workflow->validate()) {
      $workflows[$workflow->wid] = $workflow->label();
    }
  }
  if (count($workflows) == 1) {
    drupal_set_message(t('You must create at least one workflow before content can be assigned to a workflow.'));
    return $form;
  }

  $type_map = workflow_get_workflow_type_map();

  $form['#tree'] = TRUE;
  $form['help'] = array(
    '#type' => 'item',
    '#title' => '<h3>' . t('Content Type Mapping') . '</h3>',
    '#markup' => t('Each content type may have a separate workflow. The form for changing workflow state can be '
      . 'displayed when editing a node, editing a comment for a node, or both.'),
  );

  $placement_options = array(
    'node' => t('Post'),
    'comment' => t('Comment'),
  );

  foreach (node_type_get_names() as $node_type => $name) {
    $form['type_map'][$node_type]['workflow'] = array(
      '#type' => 'select',
      '#options' => $workflows,
      '#default_value' => isset($type_map[$node_type]) ? $type_map[$node_type] : 0,
    );

    $form['type_map'][$node_type]['placement'] = array(
      '#type' => 'checkboxes',
      '#options' => $placement_options,
      '#default_value' => variable_get('workflow_' . $node_type, array()),
    );
  }

  // $form['#validate'] = array('workflow_admin_ui_type_map_form_validate');

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save workflow mapping'),
    '#weight' => 100,
    '#submit' => array('workflow_admin_ui_type_map_form_submit'),
  );

  return $form;
}

/**
 * Theme the workflow type mapping form.
 */
function theme_workflow_admin_ui_type_map_form($variables) {
  $output = '';
  $form = $variables['form'];

  if (isset($form['type_map'])) {
    $header = array(t('Content Type'), t('Workflow'), t('Display Workflow Form on:'));

    $rows = array();
    foreach (node_type_get_names() as $node_type => $name) {
      $rows[] = array(
        check_plain(t($name)),
        drupal_render($form['type_map'][$node_type]['workflow']),
        drupal_render($form['type_map'][$node_type]['placement']),
      );
    }

    $output .= theme('table', array(
      'header' => $header,
      'rows' => $rows,
      'attributes' => array('style' => 'width: auto; clear: both;'),
      )
    );
  }

  return $output;
}

/**
 * Submit handler for workflow type mapping form.
 *
 * Save mapping of workflow to node type. E.g., the story node type is using the Foo workflow.
 * @see workflow_types_form()
 */
function workflow_admin_ui_type_map_form_submit($form, &$form_state) {
  $form_values = $form_state['values'];

  // Empty the table and the variables so that types no longer under workflow go away.
  //@todo: it is possible to switch to new workflow for node_type, leaving nodes are stuck in old workflow.
  workflow_delete_workflow_type_map_all();

  $node_types = node_type_get_names();
  foreach ($node_types as $node_type => $name) {
    $wid = $form_values['type_map'][$node_type]['workflow'];
    variable_del('workflow_' . $node_type);
    if ($wid) {
      workflow_insert_workflow_type_map($node_type, $wid);
      variable_set('workflow_' . $node_type, array_keys(array_filter(($form_values['type_map'][$node_type]['placement']))));

      // If this type uses workflow, make sure pre-existing nodes are set
      // to the workflow's creation state.
      if ($form_values['type_map'][$node_type]['workflow']) {
        _workflow_node_initialize_nodes('node', $node_type, '', $wid);
      }
    }
  }
  drupal_set_message(t('The workflow mapping was saved.'));
}

/**
 * Initialize all pre-existing nodes of a type to their first state.
 * @todo: adjust _workflow_node_initialize_nodes() to handle Workflow Field.
 *
 * @param $node_type - node type
 */
function _workflow_node_initialize_nodes($entity_type, $node_type, $field_name, $wid) {
  global $user;

  $entity_type = 'node';  // 'Initialize' only for 'pure' nodes with type_map + Node API
  $field_name = '';

  // Build the select query.
  // We want all published nodes of this type that don't already have a workflow state.
  $query = db_select('node', 'n');
  $query->leftJoin('workflow_node', 'wn', 'wn.nid = n.nid');
  // Add the fields.
  $query->addField('n', 'nid');
  // Add conditions.
  $query->condition('n.type', $node_type);
  $query->condition('n.status', 1);
  $query->isNull('wn.sid');

  $nids = $query->execute()->fetchCol();

  $how_many = count($nids);
  if ($how_many == 0) {
    return;
  }
  $comment = t('Pre-existing content set to initial state.');

  $workflow = workflow_load_single($wid);
  $creation_sid = $workflow->getCreationSid();

  // Load them all up.
  $nodes = node_load_multiple($nids);
  foreach ($nodes as $entity) {
    // Get the initial state for this entity.
    // Due to permissions, it might be different for each.
    $new_sid = $workflow->getFirstSid($entity_type, $entity);

    $transition = new WorkflowTransition();
    $transition->setValues($entity_type, $entity, $field_name, $creation_sid, $new_sid, $user->uid, REQUEST_TIME, $comment);

    // Force it to transition to the first state and get a history record.
    workflow_execute_transition($entity_type, $entity, $field_name, $transition, $force = TRUE);
  }

  drupal_set_message(t('!count @type nodes have been initialized.', array('@type' => node_type_get_name($node_type), '!count' => $how_many)));
}
