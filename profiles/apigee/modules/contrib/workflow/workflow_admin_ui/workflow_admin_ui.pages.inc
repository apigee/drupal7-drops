<?php
/**
 * @file
 * Provides administrative UI for workflow.
 * Why it's own module? Lower code footprint and better performance.
 * Additional credit to gcassie ( http://drupal.org/user/80260 ) for
 * the initial push to split UI out of core workflow.
 * We're moving workflow in a API direction, so UI and the like - out.
 */

/**
 * Form builder. Create the form for adding/editing a workflow.
 */
function workflow_admin_ui_add_form($form, &$form_state, $name = NULL) {
  $form = array();

  $form['wf_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Workflow Name'),
    '#maxlength' => '254',
    '#required' => TRUE,
    '#default_value' => $name,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Add Workflow'),
  );

  return $form;
}

/**
 * Validate the workflow add form.
 *
 * @see workflow_add_form()
 */
function workflow_admin_ui_add_form_validate($form, &$form_state) {
  $name = $form_state['values']['wf_name'];

  // Make sure workflow name is not a duplicate.
  foreach (workflow_load_multiple() as $workflow) {
    if ($name == check_plain($workflow->getName())) {
      form_set_error('wf_name', t('A workflow with the name %name already exists. Please enter another name for your new workflow.',
        array('%name' => $name)));
      break;
    }
  }
}

/**
 * Submit handler for the workflow add form.
 *
 * @see workflow_add_form()
 */
function workflow_admin_ui_add_form_submit($form, &$form_state) {
  $name = $form_state['values']['wf_name'];

  $workflow = workflow_create($name);
  $workflow->save();

  $args = array(
    '%name' => $name,
    '@url' => url('admin/config/workflow/workflow/edit/' . $workflow->wid),
  );
  watchdog('workflow', 'Created workflow %name', $args);
  drupal_set_message(t('The workflow %name was created. Please maintain the settings.', $args), 'status');

  // The workflow ID is now generated upon save().
  $wid = $workflow->wid;
  $form_state['redirect'] = 'admin/config/workflow/workflow/' . $wid;
}

/**
 * Form builder. Create form for confirmation of workflow deletion.
 *
 * @param $wid
 *   The workflow object to delete.
 *
 * @return
 *   Form definition array.
 */
function workflow_admin_ui_delete_form($form, &$form_state, $workflow) {
  // If we don't have a workflow that goes with this, return to the admin page.
  if (!$workflow) {
    drupal_goto('admin/config/workflow/workflow');
  }
  else {
    // Let's add some breadcrumbs.
    workflow_admin_ui_breadcrumbs($workflow);

    $form = array();
    $form['workflow'] = array(
      '#type' => 'value',
      '#value' => $workflow,
    );

    return confirm_form(
      $form,
      t('Are you sure you want to delete %title? All nodes and fields that have a workflow state associated with this workflow will ' .
        'have those workflow states removed.', array('%title' => $workflow->getName())),
      !empty($_GET['destination']) ? $_GET['destination'] : 'admin/config/workflow/workflow',
      t('This action cannot be undone.'),
      t('Delete ' . $workflow->getName()),   // This seems to get check_plain'ed.
      t('Cancel')
    );
  }
}

/**
 * Submit handler for workflow deletion form.
 *
 * @see workflow_admin_ui_delete_form()
 */
function workflow_admin_ui_delete_form_submit($form, &$form_state) {
  $workflow = $form_state['values']['workflow'];
  if ($form_state['values']['confirm'] == 1 && $workflow = workflow_load_single($workflow->wid)) {
    $name = $workflow->getName();
    $message = 'The workflow %name with all its states was deleted.';

    $workflow->delete();

    watchdog('workflow', $message, array('%name' => $name));
    drupal_set_message(t($message, array('%name' => $name)));

    $form_state['redirect'] = 'admin/config/workflow/workflow';
  }
}

/**
 * View workflow permissions by role
 *
 * @param $workflow
 *   The workflow object.
 */
function workflow_admin_ui_view_permissions_form($workflow) {
  // If we don't have a workflow at this point, go back to admin page.
  if ($workflow) {
    // Let's add some breadcrumbs.
    workflow_admin_ui_breadcrumbs($workflow);

    $all = array();
    $roles = workflow_get_roles();
    foreach ($roles as $rid => $value) {
      $all[$rid]['name'] = $value;
    }

    $transitions = $workflow->getTransitions();
    foreach ($transitions as $transition) {
      foreach ($transition->roles as $rid) {
        $current_state = $workflow->getState($transition->sid);
        $target_state = $workflow->getState($transition->target_sid);
        $all[$rid]['transitions'][] = array(check_plain(t($current_state->label())), WORKFLOW_ADMIN_UI_ARROW, check_plain(t($target_state->label())));
      }
    }

    $header = array(t('From'), '', t('To'));
    $output = '';

    // TODO we should theme out the html here.
    foreach ($all as $rid => $value) {
      $role_name = !empty($value['name']) ? $value['name'] : t('deleted role !rid', array('!rid' => $rid));
      $output .= '<h3>' . t('%role may do these transitions:', array('%role' => $role_name)) . '</h3>';
      if (!empty($value['transitions'])) {
        $output .= theme('table', array('header' => $header, 'rows' => $value['transitions'])) . '<p></p>';
      }
      else {
        $output .= '<table><tbody><tr class="odd"><td>' . t('None') . '</td><td></tr></tbody></table><p></p>';
      }
    }

    return $output;
  }
  else {
    drupal_goto('admin/config/workflow/workflow');
  }
}

/**
 * Theme the workflow permissions view.
 */
function theme_workflow_admin_ui_view_permissions_form($variables) {
  $header = $variables['header'];
  $all = $variables['all'];
  $output = '';

  foreach ($all as $rid => $value) {
    if (!empty($value['name'])) {
      $output .= '<h3>' . t('%role may do these transitions:', array('%role' => $value['name'])) . '</h3>';
    }
    if (!empty($value['transitions'])) {
      $output .= theme('table', array('header' => $header, 'rows' => $value['transitions'])) . '<p></p>';
    }
    else {
      $output .= '<table><tbody><tr class="odd"><td>' . t('None') . '</td><td></tr></tbody></table><p></p>';
    }
  }

  return $output;
}

/**
 * Menu callback. Edit a workflow's properties.
 *
 * @param $wid
 *   The workflow object.
 * @return
 *   HTML form and permissions table.
 */
function workflow_admin_ui_edit_form($form, $form_state, $workflow = NULL) {
  // @todo D8: remove workflow_admin_ui_edit_form, after converting Workflow Node to Field.

  $noyes = array(t('No'), t('Yes'));

  // If we don't have a workflow by this point, we need to go back
  // to creating one at admin/config/workflow/workflow/add
  // I think the menu loader won't allow this to happen.
  if (!$workflow) {
    drupal_goto('admin/config/workflow/workflow/add');
  }
  else {
    // Let's add some breadcrumbs.
    workflow_admin_ui_breadcrumbs($workflow);

    $form = array();
    $form['workflow'] = array(
      '#type' => 'value',
      '#value' => $workflow,
    );

    $form['basic'] = array(
      '#type' => 'fieldset',
      '#title' => t('Workflow information'),
    );

    $form['basic']['wf_name'] = array(
      '#type' => 'textfield',
      '#default_value' => $workflow->getName(),
      '#title' => t('Workflow name'),
      '#maxlength' => '254',
      '#required' => TRUE,
    );

    $form['basic']['options'] = array(
      '#type' => 'select',
      '#title' => t('How to show the available states'),
      '#required' => FALSE,
      '#default_value' => isset($workflow->options['options']) ? $workflow->options['options'] : 'radios',
      // '#multiple' => TRUE / FALSE,
      '#options' => array(
        // These options are taken from options.module
        'select' => 'Select list',
        'radios' => 'Radio buttons',
        // This option does not work properly on Comment Add form.
        'buttons' => 'Action buttons',
      ),
      '#description' => t("The Widget shows all available states. Decide which
        is the best way to show them. ('Action buttons' do not work on Comment form.)"
      ),
    );
    $form['basic']['name_as_title'] = array(
      '#type' => 'radios',
      '#options' => $noyes,
      '#attributes' => array('class' => array('container-inline')),
      '#title' => t('Use the workflow name as the title of the workflow form?'),
      '#default_value' => isset($workflow->options['name_as_title']) ? $workflow->options['name_as_title'] : 0,
      '#description' => t('The workflow section of the editing form is in its own fieldset. Checking the box will add the workflow ' .
        'name as the title of workflow section of the editing form.'),
    );

    $form['schedule'] = array(
      '#type' => 'fieldset',
      '#title' => t('Scheduling for Workflow'),
    );

    $form['schedule']['schedule'] = array(
      '#type' => 'radios',
      '#options' => $noyes,
      '#attributes' => array('class' => array('container-inline')),
      '#title' => t('Allow scheduling of workflow transitions?'),
      '#default_value' => isset($workflow->options['schedule']) ? $workflow->options['schedule'] : 1,
      '#description' => t('Workflow transitions may be scheduled to a moment in the future. ' .
        'Soon after the desired moment, the transition is executed by Cron.'),
    );

    $form['schedule']['schedule_timezone'] = array(
      '#type' => 'radios',
      '#options' => $noyes,
      '#attributes' => array('class' => array('container-inline')),
      '#title' => t('Show a timezone when scheduling a transition?'),
      '#default_value' => isset($workflow->options['schedule_timezone']) ? $workflow->options['schedule_timezone'] : 1,
    );

    $form['comment'] = array(
      '#type' => 'fieldset',
      '#title' => t('Comment for Workflow Log'),
    );

    $form['comment']['comment_log_node'] = array(
      '#type' => 'radios',
      '#options' => $noyes,
      '#attributes' => array('class' => array('container-inline')),
      '#title' => t('Show a comment field in the workflow section of the editing form?'),
      '#default_value' => isset($workflow->options['comment_log_node']) ? $workflow->options['comment_log_node'] : 0,
      '#description' => t('On the node editing form, a Comment form can be shown so that the person making the state change can record ' .
        'reasons for doing so. The comment is then included in the node\'s workflow history.'),
    );

    $form['comment']['comment_log_tab'] = array(
      '#type' => 'radios',
      '#options' => $noyes,
      '#attributes' => array('class' => array('container-inline')),
      '#title' => t('Show a comment field in the workflow section of the workflow tab form?'),
      '#default_value' => isset($workflow->options['comment_log_tab']) ? $workflow->options['comment_log_tab'] : 0,
      '#description' => t('On the workflow tab, a Comment form can be shown so that the person making the state change can record ' .
        'reasons for doing so. The comment is then included in the node\'s workflow history.'),
    );

    $form['watchdog'] = array(
      '#type' => 'fieldset',
      '#title' => t('Watchdog Logging for Workflow'),
    );

    $form['watchdog']['watchdog_log'] = array(
      '#type' => 'radios',
      '#options' => $noyes,
      '#attributes' => array('class' => array('container-inline')),
      '#title' => t('Log informational watchdog messages when a transition is executed (state of a node is changed)?'),
      '#default_value' => isset($workflow->options['watchdog_log']) ? $workflow->options['watchdog_log'] : 0,
      '#description' => t('Optionally log transition state changes to watchdog.'),
    );

    $form['tab'] = array(
      '#type' => 'fieldset',
      '#title' => t('Workflow tab permissions'),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
    );
    $form['tab']['history_tab_show'] = array(
      '#type' => 'checkbox',
      '#title' => t('Use the workflow history, and show it on a separate tab.'),
      '#required' => FALSE,
      '#default_value' => isset($workflow->options['history_tab_show']) ? $workflow->options['history_tab_show'] : 1,
      '#description' => t("Every state change is recorded in table
        {workflow_node_history}. If checked and user has proper permission, a
        tab 'Workflow' is shown on the entity view page, which gives access to
        the History of the workflow."),
    );
    $form['tab']['tab_roles'] = array(
      '#type' => 'checkboxes',
      '#options' => workflow_get_roles(),
      '#default_value' => array_keys($workflow->tab_roles),
      '#description' => t('Select any roles that should have access to the workflow tab on nodes that have a workflow.'),
    );

    $form['submit'] = array('#type' => 'submit', '#value' => t('Save configuration'));

    return $form;
  }
}

/**
 * Theme the workflow editing form.
 *
 * @see workflow_edit_form()
 */
function theme_workflow_admin_ui_edit_form($variables) {
  $output = '';

  $form = $variables['form'];
  $workflow = $variables['form']['workflow']['#value'];

  // If we don't have a workflow here, we need to go back to admin.
  if ($workflow) {
    drupal_set_title(t('Edit workflow %name', array('%name' => $workflow->getName())), PASS_THROUGH);

    $output .= drupal_render_children($form);
    return $output;
  }
  else {
    drupal_goto('admin/config/workflow/workflow');
  }
}

/**
 * Validate the workflow editing form.
 *
 * @see workflow_edit_form()
 * @todo: this duplicates code from workflow_admin_ui_add_form_validate()
 */
function workflow_admin_ui_edit_form_validate($form_id, $form_state) {
  $workflow = $form_state['values']['workflow'];
  $name = $form_state['values']['wf_name'];

  // Make sure workflow name is not a duplicate.
  foreach (workflow_load_multiple() as $stored_workflow) {
    if ($name == check_plain($stored_workflow->getName()) && $workflow->wid != $stored_workflow->wid) {
      form_set_error('wf_name', t('A workflow with the name %name already exists. Please enter another name for this workflow.',
        array('%name' => $name)));
      break;
    }
  }
}

/**
 * Submit handler for the workflow editing form.
 *
 * @see workflow_edit_form()
 * @todo: this is only valid for Node API, not for Field API.
 *        Field API has 'Field settings'.
 */
function workflow_admin_ui_edit_form_submit($form, &$form_state) {
  $workflow = $form_state['values']['workflow'];
  $workflow->name = $form_state['values']['wf_name'];
  $workflow->tab_roles = array_filter($form_state['values']['tab_roles']);
  $workflow->options = array(
    'name_as_title' => $form_state['values']['name_as_title'],
    'options' => $form_state['values']['options'],
    'schedule' => $form_state['values']['schedule'],
    'schedule_timezone' => $form_state['values']['schedule_timezone'],
    'comment_log_node' => $form_state['values']['comment_log_node'],
    'comment_log_tab' => $form_state['values']['comment_log_tab'],
    'watchdog_log' => $form_state['values']['watchdog_log'],
    'history_tab_show' => $form_state['values']['history_tab_show'],
  );

  $workflow->save();

  drupal_set_message(t('The workflow was updated.'));

  $form_state['redirect'] = 'admin/config/workflow/workflow/' . $workflow->wid;
}

/**
 * Menu callback. Edit a workflow's transitions.
 *
 * @param array $transitions from values.
 *   Transitions, for example:
 *     18 => array(
 *       20 => array(
 *         'author' => 1,
 *         1        => 0,
 *         2        => 1,
 *       )
 *     )
 *   means the transition from state 18 to state 20 can be executed by
 *   the node author or a user in role 2. The $transitions array should
 *   contain ALL transitions for the workflow.
 *
 * @param $wid
 *   The workflow object.
 * @return
 *   HTML form and permissions table.
 */
function workflow_admin_ui_transitions_form($form, $form_state, $workflow) {
  // Make sure we have a workflow.
  if ($workflow) {
    // Let's add some breadcrumbs.
    workflow_admin_ui_breadcrumbs($workflow);

    $form = array();
    $form['workflow'] = array(
      '#type' => 'value',
      '#value' => $workflow,
    );
    $form['transitions'] = _workflow_admin_ui_transition_grid_form($workflow);
    $form['submit'] = array('#type' => 'submit', '#value' => t('Save configuration'));

    return $form;
  }
}

/**
 * Validate the workflow editing form.
 *
 * @see workflow_edit_form()
 */
function workflow_admin_ui_transitions_form_validate($form, $form_state) {
  $workflow = $form_state['values']['workflow'];
  $wid = $workflow->wid;

  // Make sure 'author' is checked for (creation) -> [something].
  $creation_state = $workflow->getCreationState();
  $creation_sid = $creation_state->sid;
  if (isset($form_state['values']['transitions'][$creation_sid]) && is_array($form_state['values']['transitions'][$creation_sid])) {
    foreach ($form_state['values']['transitions'][$creation_sid] as $roles) {
      if ($roles[WORKFLOW_ROLE_AUTHOR_RID]) {
        $author_has_permission = TRUE;
        break;
      }
    }
  }
  $state_count = db_query('SELECT COUNT(sid) FROM {workflow_states} WHERE wid = :wid', array(':wid' => $wid))->fetchField();
  if (empty($author_has_permission) && $state_count > 1) {
    form_set_error('transitions', t('Please give the author permission to go from %creation to at least one state!',
      array('%creation' => $creation_state->label())));
  }
}

/**
 * Theme the workflow editing form.
 *
 * @see workflow_edit_form()
 */
function theme_workflow_admin_ui_transitions_form($variables) {
  $output = '';
  $form = $variables['form'];

  $workflow = $form['workflow']['#value'];
  $name = $workflow->getName();

  // If we don't have a workflow here, we need to go back to admin.
  if ($workflow) {
    drupal_set_title(t('Edit workflow %name transitions', array('%name' => $name)), PASS_THROUGH);

    $states = $workflow->getStates($all = 'CREATION');
    if ($states) {
      $roles = workflow_get_roles();

      $header = array(array('data' => t('From / To') . ' &nbsp;' . WORKFLOW_ADMIN_UI_ARROW));
      $rows = array();
      foreach ($states as $state) {
        $label = check_plain($state->label());
        // Don't allow transition TO (creation).
        if (!$state->isCreationState()) {
          $header[] = array('data' => $label);
        }
        $row = array(array('data' => $label));
        foreach ($states as $nested_state) {
          if ($nested_state->isCreationState()) {
            // Don't allow transition TO (creation).
            continue;
          }
          if ($nested_state != $state) {
            // Need to render checkboxes for transition from $state to $nested_state.
            $from = $state->sid;
            $to = $nested_state->sid;
            $cell = '';
            foreach ($roles as $rid => $role_name) {
              $cell .= drupal_render($form['transitions'][$from][$to][$rid]);
            }
            $row[] = array('data' => $cell);
          }
          else {
            $row[] = array('data' => '');
          }
        }
        $rows[] = $row;
      }
      $output .= theme('table', array('header' => $header, 'rows' => $rows));
    }
    else {
      $output = t('There are no states defined for this workflow.');
    }

    $output .= drupal_render_children($form);
    return $output;
  }
}

/**
 * Submit handler for the workflow editing form.
 *
 * @see workflow_edit_form()
 */
function workflow_admin_ui_transitions_form_submit($form, &$form_state) {
  $workflow = $form['workflow']['#value'];
  $wid = $workflow->wid;

  if (isset($form_state['values']['transitions'])) {
    $transitions = $form_state['values']['transitions'];

    // Empty string is sometimes passed in instead of an array.
    if (!$transitions) {
      return;
    }
    foreach ($transitions as $from => $to_data) {
      foreach ($to_data as $to => $role_data) {
        $roles = array();
        foreach ($role_data as $role => $can_do) {
          if ($can_do) {
            $roles += array($role => $role);
          }
        }
        if (count($roles)) {
          $config_transition = $workflow->createTransition($from, $to);
          $config_transition->roles = $roles;
          $config_transition->save();
        }
        else {
          foreach ($workflow->getTransitionsBySidTargetSid($from, $to, 'ALL') as $config_transition) {
            $config_transition->delete();
          }
        }
      }
    }
  }

  drupal_set_message(t('The workflow was updated.'));

  $form_state['redirect'] = 'admin/config/workflow/workflow/' . $wid;
}

/**
 * Form builder. Build the grid of transitions for defining a workflow.
 *
 * @param int $wid
 *   The workflow object.
 */
function _workflow_admin_ui_transition_grid_form($workflow) {
  $form = array('#tree' => TRUE);

  $roles = workflow_get_roles();

  $states = $workflow->getStates($all = 'CREATION');
  if (!$states) {
    $form['error'] = array(
      '#type' => 'markup',
      '#value' => t('There are no states defined for this workflow.'),
    );
    return $form;
  }

  foreach ($states as $state1) {
    $state_id = $state1->sid;
    $name = $state1->label();
    foreach ($states as $state2) {
      $nested_state_id = $state2->sid;
      $nested_name = $state2->label();
      if ($state2->isCreationState()) {
        // Don't allow transition TO (creation).
        continue;
      }
      if ($nested_state_id != $state_id) {
        // Need to generate checkboxes for transition from $state to $nested_state.
        $from = $state_id;
        $to = $nested_state_id;
        foreach ($roles as $rid => $role_name) {
          $checked = FALSE;
          $config_transitions = $workflow->getTransitionsBySidTargetSid($from, $to);
          $config_transition = reset($config_transitions);
          if ($config_transition) {
            $checked = $config_transition->isAllowed($rid);
          }
          $form[$from][$to][$rid] = array(
            '#type' => 'checkbox',
            '#title' => check_plain($role_name),
            '#default_value' => $checked,
          );
        }
      }
    }
  }
  return $form;
}

/**
 * Menu callback. Create the main workflow page, which gives an overview
 * of workflows and workflow states.
 * Replaced by http://drupal.org/node/1367530.
 */
function workflow_admin_ui_states_form($form, $form_state, $workflow) {
  // Let's add some breadcrumbs.
  $bc = array(l(t('Home'), '<front>'));
  $bc[] = l(t('Configuration'), 'admin/config');
  $bc[] = l(t('Workflow'), 'admin/config/workflow');
  $bc[] = l(t('Workflow'), 'admin/config/workflow/workflow');
  drupal_set_breadcrumb($bc);

  drupal_set_title(t('Workflow: @label', array('@label' => $workflow->label())));

  $form = array();
  $form['#tree'] = TRUE;
  $form['workflow'] = array(
    '#type' => 'value',
    '#value' => $workflow,
  );

  // Allow modules to insert their own workflow operations.
  $links = module_invoke_all('workflow_operations', 'workflow', $workflow);
  $links_args = array(
    'links' => $links,
    'attributes' => array('class' => array('inline', 'action-links')),
  );
  $form['action-links'] = array(
    '#type' => 'markup',
    '#markup' => theme('links', $links_args),
  );

  // Build select options for reassigning states.
  // We put a blank state first for validation.
  $state_list = array('' => ' ');
  $state_list += workflow_get_workflow_state_names($workflow->wid, $grouped = FALSE, $all = FALSE);
  // Is this the last state available?
  $form['#last_mohican'] = count($state_list) == 2;

  // Get the state objects as array ($sid => WorkflowState)
  $states = $workflow->getStates($all = TRUE);
  // Create a dummy object for new state item.
  $wid = $workflow->wid;
  $dummy = $workflow->createState('');
  $dummy->weight = 99;
  $states[$dummy->sid] = $dummy; // Although the index is 0, the state is appended at the end of the list.

  foreach ($states as $state) {
    // Allow modules to insert state operations.
    $links = module_invoke_all('workflow_operations', 'state', $workflow, $state);

    // Make it impossible to reassign to the same state that is disabled.
    if ($state->isCreationState() || !$state->sid || !$state->isActive()) {
      $current_state = array();
      $current_state_list = array();
    }
    else {
      $current_state = array($state->sid => $state_list[$state->sid]);
      $current_state_list = array_diff($state_list, $current_state);
    }

    $form['states'][$state->sid] = array(
      'state' => array(
        '#type' => 'textfield',
        '#size' => 30,
        '#maxlength' => 255,
        '#default_value' => $state->label(),
      ),

      'weight' => array(
        '#type' => 'weight',
        '#delta' => 20,
        '#default_value' => $state->weight,
        '#title-display' => 'invisible',
        '#attributes' => array('class' => array('state-weight')),
      ),

      'status' => array(
        '#type' => 'checkbox',
        '#default_value' => $state->isActive(),
      ),

      // Save the original status for the validation handler.
      'orig_status' => array(
        '#type' => 'value',
        '#value' => $state->isActive(),
      ),

      'reassign' => array(
        '#type' => 'select',
        '#options' => $current_state_list,
      ),

      'count' => array(
        '#type' => 'value',
        '#value' => $state->count(),
      ),

      'ops' => array(
        '#type' => 'markup',
        '#markup' => theme('links', array('links' => $links)),
       ),
    );

    // Don't let the creation state change weight or status or name.
    if ($state->isCreationState()) {
      $form['states'][$state->sid]['weight']['#value'] = -50;
      $form['states'][$state->sid]['sysid']['#value'] = 1;
      $form['states'][$state->sid]['state']['#disabled'] = TRUE;
      $form['states'][$state->sid]['status']['#disabled'] = TRUE;
      $form['states'][$state->sid]['reassign']['#type'] = 'hidden';
      $form['states'][$state->sid]['reassign']['#disabled'] = TRUE;
    }
    if (!$state->sid || !$state->isActive()) {
      $form['states'][$state->sid]['reassign']['#type'] = 'hidden';
      $form['states'][$state->sid]['reassign']['#disabled'] = TRUE;
    }
  }

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array('#type' => 'submit', '#value' => t('Save Changes'));

  return $form;
}

function theme_workflow_admin_ui_states_form($variables) {
  $form = $variables['form'];
  $output = '';
  $table_id = 'workflow_admin_ui_states';

  $table = array(
    'rows' => array(),
    'header' => array(
      t('State'),
      t('Weight'),
      t('Active'),
      t('Reassign'),
      t('Count'),
      array('data' => t('Operations'), 'class' => 'state-ops'),
    ),
    'attributes' => array('id' => $table_id, 'style' => 'width: auto;'),
  );

  // The output needs to have the action links at the top.
  $output .= drupal_render($form['action-links']);

  // Iterate over each element in our $form['states'] array
  foreach (element_children($form['states']) as $id) {
    // We are now ready to add each element of our $form data to the rows
    // array, so that they end up as individual table cells when rendered
    // in the final table.  We run each element through the drupal_render()
    // function to generate the final html markup for that element.
    $table['rows'][] = array(
      'data' => array(
        // Add our 'name' column
        array('data' => drupal_render($form['states'][$id]['state']), 'class' => 'state-name'),
        // Add our 'weight' column
        drupal_render($form['states'][$id]['weight']),
        // Add our 'status' column
        array('data' => drupal_render($form['states'][$id]['status']), 'class' => 'state-status'),
        // Add our 'reassign' column
        array('data' => drupal_render($form['states'][$id]['reassign']), 'class' => 'state-reassign'),
        // Add our 'count' column
        array('data' => $form['states'][$id]['count']['#value'], 'class' => 'state-count'),
        // Add our 'operations' column
        array('data' => drupal_render($form['states'][$id]['ops']), 'class' => 'state-ops'),
      ),
      // To support the tabledrag behavior, we need to assign each row of the
      // table a class attribute of 'draggable'. This will add the 'draggable'
      // class to the <tr> element for that row when the final table is
      // rendered.
      'class' => array('draggable'),
    );
  }

  $output .= theme('table', $table);

  // And then render any remaining form elements (such as our submit button)
  $output .= drupal_render_children($form);

  // We now call the drupal_add_tabledrag() function in order to add the
  // tabledrag.js goodness onto our page.
  //
  // For a basic sortable table, we need to pass it:
  //   - the $table_id of our <table> element,
  //   - the $action to be performed on our form items ('order'),
  //   - a string describing where $action should be applied ('siblings'),
  //   - and the class of the element containing our 'weight' element.
  drupal_add_tabledrag($table_id, 'order', 'sibling', 'state-weight');

  return $output;
}

/**
 * Validation handler for the state form.
 */
function workflow_admin_ui_states_form_validate($form, &$form_state) {
  // Get the workflow id.
  $workflow = $form_state['values']['workflow'];

  // Because the form elements were keyed with the item ids from the database,
  // we can simply iterate through the submitted values.
  foreach ($form_state['values']['states'] as $sid => $item) {
    // Reload $state from db, in case the states were changed by anyone else. And it is just as fast.
    $state = workflow_state_load_single($sid);

    // Does user want to deactivate the state (reassign current nodes)?
    if ($sid > 0 && $item['status'] == 0 && $state->isActive()) {
      $args = array('%state' => $state->getName(), );

      // Does that state have nodes in it?
      if ($item['count'] > 0 && empty($item['reassign'])) {
        if ($form['#last_mohican']) {
          $message = 'Since you are deleting the last available workflow state
            in this workflow, all content items which are in that state will have their
            workflow state removed.';
          drupal_set_message(t($message, $args), 'warning');
        }
        else {
          $message = 'The %state state has content; you must reassign the content to another state.';
          form_set_error("states'][$sid]['reassign'",
            t($message, $args));
        }
      }
      if (module_exists('workflowfield')) {
        // @todo: Reassign states for Workflow Field.
        $message = 'Deactivating state %state for does not reassign Fields of type Workflow with this status.';
        drupal_set_message(t($message, $args), 'warning');
      }
    }
  }
}

/**
 * Submission handler for the state form.
 */
function workflow_admin_ui_states_form_submit($form, &$form_state) {
  // Get the workflow id, then save it for the next round.
  $workflow = $form_state['values']['workflow'];
  $wid = $workflow->wid;

  // Because the form elements were keyed with the item ids from the database,
  // we can simply iterate through the submitted values.
  foreach ($form_state['values']['states'] as $sid => $item) {
    $item['sid'] = $sid;
    $item['wid'] = $wid;

    // Is there not a new state name?
    if (empty($item['state'])) {
      // No new state entered, so skip it.
      continue;
    }

    // Reload $state from db, in case the states were changed by anyone else. And it is just as fast.
    $state = ($sid) ? workflow_state_load_single($sid) : $workflow->createState('');

    // Does user want to deactivate the state (reassign current nodes)?
    if ($sid > 0 && $item['status'] == 0 && $state->isActive()) {
      $new_sid = $item['reassign'];
      $new_state = workflow_state_load_single($new_sid);

      $args = array(
        '%workflow' => $workflow->getName(),
        '%old_state' => $state->getName(),
        '%new_state' => isset($new_state) ? $new_state->getName() : '',
      );

      if ($item['count'] > 0) {
        if ($form['#last_mohican']) {
          $new_sid = NULL; // Do not reassign to new state.
          $message = 'Removing workflow states from content in the %workflow.';
          drupal_set_message(t($message, $args));
        }
        else {
          // Prepare the state delete function.
          $message = 'Reassigning content from %old_state to %new_state.';
          drupal_set_message(t($message, $args));
        }
      }
      // Delete the old state without orphaning nodes, move them to the new state.
      $state->deactivate($new_sid);

      $message = 'Deactivated workflow state %old_state in %workflow.';
      watchdog('workflow', $message, $args);
      drupal_set_message(t($message, $args));
    }
    $state->setName($item['state']);
    $state->status = $item['status'];
    $state->weight = $item['weight'];
    $state->save();
  }

  $form_state['redirect'] = "admin/config/workflow/workflow/$wid";
}

/**
 * Page builder. Show a list of all workflows.
 *
 * Appends the type_map form, allowing administrator
 * to map workflows to content types and determine placement on content forms.
 */
function workflow_admin_ui_overview_page($form) {
  $form = array();

  // Allow modules to insert their own action links.
  $links = module_invoke_all('workflow_operations', 'top_actions', NULL);
  $links_args = array(
    'links' => $links,
    'attributes' => array('class' => array('inline', 'action-links')),
  );
  $form['action-links'] = array(
    '#type' => 'markup',
    '#markup' => theme('links', $links_args),
  );

  // Add a form with the type map, which is only valid for Workflow Node API.
  if (module_exists('workflownode')) {
    module_load_include('inc', 'workflow_admin_ui', 'workflow_admin_ui.type_map.page');
    $type_map_form = array();
    $form += workflow_admin_ui_type_map_form($type_map_form);
  }
  return $form;
}

/**
 * Theme the workflow overview page, include type_map form.
 */
function theme_workflow_admin_ui_overview_page($variables) {
  $output = '';
  $form = $variables['form'];

  // Do the action links at the top.
  $output .= drupal_render($form['action-links']);
  $output .= drupal_render($form['help']);

  if (module_exists('workflownode')) {
    // Add the type_map maintenance form.
    $output .= theme_workflow_admin_ui_type_map_form($variables);
    unset($form['type_map']);
  }

  return $output . drupal_render_children($form);
}
