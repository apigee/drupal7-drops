<?php
/**
 * @file
 * Provides pages for administrative UI.
 */

/**
 * Implements hook_form().
 *
 * Add a "three dimensional" (state, role, permission type) configuration
 * interface to the workflow edit form.
 */
function workflow_access_form($form, $form_state, $workflow) {
  if (!$workflow) {
    // Leave this page immediately.
    drupal_set_message(t('Unknown workflow'));
    drupal_goto('admin/config/workflow/workflow');
  }

  drupal_set_title(t('@name Access', array('@name' => $workflow->getName())));

  // Let's add some breadcrumbs.
  workflow_admin_ui_breadcrumbs($workflow);

  $form = array('#tree' => TRUE);

  $form['#wid'] = $workflow->wid;

  // A list of role names keyed by role ID, including the 'author' role.
  $roles = workflow_get_roles('participate in workflow');

  // Add a table for every workflow state.
  foreach ($workflow->getStates($all = TRUE) as $state) {
    if ($state->isCreationState()) {
      // No need to set perms on creation.
      continue;
    }
    $view = $update = $delete = array();
    $count = 0;
    foreach (workflow_access_get_workflow_access_by_sid($state->sid) as $access) {
      $count++;
      if ($access->grant_view) {
        $view[] = $access->rid;
      }
      if ($access->grant_update) {
        $update[] = $access->rid;
      }
      if ($access->grant_delete) {
        $delete[] = $access->rid;
      }
    }
    // Allow view grants by default for anonymous and authenticated users,
    // if no grants were set up earlier.
    if (!$count) {
      $view = array(DRUPAL_ANONYMOUS_RID, DRUPAL_AUTHENTICATED_RID);
    }
    // TODO better tables using a #theme function instead of direct #prefixing
    $form[$state->sid] = array(
      '#type' => 'fieldset',
      '#title' => check_plain($state->label()),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
      '#tree' => TRUE,
      );

    $form[$state->sid]['view'] = array(
      '#type' => 'checkboxes',
      '#options' => $roles,
      '#default_value' => $view,
      '#title' => t('Roles who can view posts in this state'),
      '#prefix' => '<table width="100%" style="border: 0;"><tbody style="border: 0;"><tr><td>',
      );

    $form[$state->sid]['update'] = array(
      '#type' => 'checkboxes',
      '#options' => $roles,
      '#default_value' => $update,
      '#title' => t('Roles who can edit posts in this state'),
      '#prefix' => "</td><td>",
      );

    $form[$state->sid]['delete'] = array(
      '#type' => 'checkboxes',
      '#options' => $roles,
      '#default_value' => $delete,
      '#title' => t('Roles who can delete posts in this state'),
      '#prefix' => "</td><td>",
      '#suffix' => "</td></tr></tbody></table>",
    );
  }

  $form['warning'] = array(
    '#type' => 'markup',
    '#markup' => '<p><strong>'
      . t('WARNING:')
      . '</strong> '
      . t('Use of the "Edit any," "Edit own," and even "View published content" permissions
        for the content type may override these access settings.')
      . '</p>',
      );

  $form['submit'] = array('#type' => 'submit', '#value' => t('Save configuration'));

  return $form;
  // Place our block comfortably down the page.
  $form['submit']['#weight'] = 10;
  $form['#submit'][] = 'workflow_access_form_submit';
}

/**
 * Store permission settings for workflow states.
 */
function workflow_access_form_submit($form, &$form_state) {
  foreach ($form_state['values'] as $sid => $access) {
    // Ignore irrelevant keys.
    if (!is_numeric($sid)) {
      continue;
    }
    foreach ($access['view'] as $rid => $checked) {
      $data = array(
        'sid' => $sid,
        'rid' => $rid,
        'grant_view' => (!empty($checked)) ? (bool) $checked : 0,
        'grant_update' => (!empty($access['update'][$rid])) ? (bool) $access['update'][$rid] : 0,
        'grant_delete' => (!empty($access['delete'][$rid])) ? (bool) $access['delete'][$rid] : 0,
      );
      $id = workflow_access_insert_workflow_access_by_sid($data);
    }

    // Update all nodes having same workflow state to reflect new settings.
    foreach (workflow_get_workflow_node_by_sid($sid) as $data) {
      // Instead of trying to construct what the grants should be per node as we save.
      // Let's fall back on existing node grant systems that will find it for us.
      $entity_type = 'node'; // @todo: add support for other entity types in workflow_access_form_submit()
      $node = node_load($data->nid);
      node_access_acquire_grants($node);
    }
  }

  drupal_set_message(t('Workflow access permissions updated.'));
  $form_state['redirect'] = 'admin/config/workflow/workflow/' . $form['#wid'];
}
