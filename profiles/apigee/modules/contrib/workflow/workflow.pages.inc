<?php
/**
 * @file
 * Provide user interface for changing workflow state.
 */

define('MARK_STATE_IS_DELETED', '*');

/**
 * Menu callback. Display workflow summary of a node.
 */
function workflow_tab_page($entity_type, $entity = NULL) {
  drupal_set_title(entity_label($entity_type, $entity));

  $form = array();

  // Figure out the $entity's bundle and id.
  list($entity_id, , $entity_bundle) = entity_extract_ids($entity_type, $entity);
  $entity_id = entity_id($entity_type, $entity);

  // @todo: workflow_tab_page(): what to do with multiple workflow_fields per bundle? Use Views instead!
  $workflow = workflow_get_workflows_by_type($entity_bundle, $entity_type);

  // Show the current state and the Workflow form to allow state changing.
  // N.B. This part is replicated in hook_node_view, workflow_tab_page, workflow_vbo.
  if ($workflow) {
    $workflow_item = $workflow->getWorkflowItem();
    $field = $workflow_item->getField();
    $instance = $workflow_item->getInstance();
    if (!$field['id']) {
      // This is a Workflow Node workflow. Set widget options as in v7.x-1.2
      $field['settings']['widget']['comment'] = !empty($workflow->options['comment_log_tab']); // vs. ['comment_log_node']; 
      $field['settings']['widget']['current_status'] = TRUE;
    }
  }

  // Add the form/widget to the formatter, and include the nid and field_id in the form id,
  // to allow multiple forms per page (in listings, with hook_forms() ).
  // Ultimately, this is a wrapper for WorkflowDefaultWidget.
  $field_name = $field['field_name'];
  $field_id = $field['id'];

  $form['workflow_current_state'] = workflow_state_formatter($entity_type, $entity, $field, $instance);
  $form += drupal_get_form(implode('_', array('workflow_transition_form', $entity_type, $entity_id, $field_id)), $field, $instance, $entity_type, $entity);
  $output = drupal_render($form);

  // Show the history table.
  $rows = array();
  $current_themed = FALSE;
  $current_sid = workflow_node_current_state($entity, $entity_type, $field_name);
  $limit = variable_get('workflow_states_per_page', 20);
  foreach (workflow_transition_load_multiple($entity_type, array($entity_id), $field_name, $limit) as $history) {
    $old_state_name = $new_state_name = '';

    $label = $name = '';
    $state = workflow_state_load_single($history->new_sid);
    if ($state) {
      $name = $state->getName();
      $label = $state->label();
    }

    if (!$state) {
      // This is an invalid/deleted state.
      $old_state_name = check_plain($label);
    }
    elseif ($history->new_sid == $current_sid && $state->isActive() && !$current_themed) {
      // Theme the current state differently so it stands out.
      $new_state_name = theme('workflow_history_current_state', array('state_name' => $label,
                                                                      'state_system_name' => $name,
                                                                      'sid' => $history->new_sid));
      // Make a note that we have themed the current state; other times in the history
      // of this node where the node was in this state do not need to be specially themed.
      $current_themed = TRUE;
    }
    elseif (!$state->isActive()) {
      // The state has been deleted, but we include it in the history.
      $new_state_name = theme('workflow_deleted_state', array('state_name' => $label,
                                                              'state_system_name' => $name,
                                                              'sid' => $history->new_sid));
      $footer_needed = TRUE;
    }
    else {
      // Regular state.
      $new_state_name = check_plain($label);
    }

    $label = $name = MARK_STATE_IS_DELETED;
    $state = workflow_state_load_single($history->old_sid);
    if ($state) {
      $name = $state->getName();
      $label = $state->label();
    }

    if (!$state) {
      // This is an invalid/deleted state.
      $old_state_name = check_plain($label);
    }
    elseif (!$state->isActive()) {
      $old_state_name = theme('workflow_deleted_state', array('state_name' => $label,
                                                              'state_system_name' => $name,
                                                              'sid' => $history->old_sid));
      $footer_needed = TRUE;
    }
    else {
      // Regular state.
      $old_state_name = check_plain($label);
    }

    $variables = array(
      'transition' => $history, // @todo D8: pass this WorkflowTransition as only variable. It contains everything.
      'extra' => '',

      'history' => $history,  // @todo D8: remove, as this is the same as 'transition'.
      'old_sid' => $history->old_sid, // @todo D8: remove this redundant property.
      'sid' => $history->new_sid, // @todo D8: remove this redundant property.
      'uid' => $history->uid, // @todo D8: remove this redundant property.
      'old_state_name' => $old_state_name,
      'state_name' => $new_state_name,
    );

    // Allow other modules to modify the row.
    // $todo D8: pass only a $transition object.
    drupal_alter('workflow_history', $variables);

    $rows[] = theme('workflow_history_table_row', $variables);
  }

  // Mark the first and last rows.
  $rows[0]['class'][] = 'first';
  $last = count($rows) - 1;
  $rows[$last]['class'][] = 'last';

  $header = array(t('Date'), t('Old State'), t('New State'), t('By'), t('Comment'));
  $header[] = array('data' => t('Operations'));

  // Only display the table if there's anything in it.
  if ($rows) {
    $output .= theme('workflow_history_table', array('header' => $header, 'rows' => $rows, 'footer' => !empty($footer_needed)));
    $output .= theme('pager', array('tags' => $limit));
  }
  return $output;
}

/*
 * Theme one WorkflowTansition in a workflow history table row.
 *
 * $old_state_name and $state_name must be run through check_plain(t()) prior
 * to calling this theme function.
 */
function theme_workflow_history_table_row($variables) {
  $row = array();
  $old_state_name = $variables['old_state_name'];
  $state_name = $variables['state_name'];
  $transition = $variables['transition'];
  $account = user_load($transition->uid);
  $row = array(
    'data' => array(
      array('data' => format_date($transition->stamp), 'class' => array('timestamp')),
      array('data' => $old_state_name, 'class' => array('previous-state-name')),
      array('data' => $state_name, 'class' => array('state-name')),
      array('data' => theme('username', array('account' => $account)), 'class' => array('user-name')),
      array('data' => filter_xss($transition->comment), 'class' => array('log-comment')),
      $variables['extra'],
    ),
  'class' => array('workflow_history_row'),
  );

  return $row;
}

/*
 * Theme entire workflow history table.
 */
function theme_workflow_history_table($variables) {
  $header = $variables['header'];
  $rows = $variables['rows'];
  $footer = $variables['footer'];
  $extra_column = 5;

  // Remove the Operations column if none are added.
  $empty = TRUE;
  foreach ($rows as $row) {
    $empty &= empty($row['data'][$extra_column]);
  }
  if ($empty) {
    foreach ($rows as &$row) {
      unset($row['data'][$extra_column]);
      unset($header[$extra_column]);
    }
  }

  $output = theme('table', array('header' => $header, 'rows' => $rows, 'caption' => t('Workflow History')));
  if ($footer) {
    $output .= MARK_STATE_IS_DELETED . ' ' . t('State is no longer available.');
  }
  return $output;
}

/**
 * Theme the current state in the workflow history table.
 */
function theme_workflow_history_current_state($variables) {
  return check_plain(t($variables['state_name']));
}

/**
 * Theme a deleted state in the workflow history table.
 */
function theme_workflow_deleted_state($variables) {
  return check_plain(t($variables['state_name'])) . MARK_STATE_IS_DELETED;
}
