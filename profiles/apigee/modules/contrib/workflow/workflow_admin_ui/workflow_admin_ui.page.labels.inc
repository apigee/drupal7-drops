<?php

/**
 * @file
 * Provides an Admin UI page for the Workflow Transition labels.
 *
 * Modify the workflow form items so specific workflow transitions
 * can have their own labels which the admin can describe relative
 * to the beginning and ending states.
 * Rather than showing the user a workflow box containing options like
 * "review required" as a state in the workflow, it could say "move to
 * the editing department for grammar review".
 *
 * AUTHOR
 * ------
 * David Kent Norman (http://deekayen.net/)
 * 
 * Amazon Honor System donation:
 * http://zme.amazon.com/exec/varzea/pay/T2EOCSRRDQ9CL2
 * 
 * Paypal donation:
 * https://www.paypal.com/us/cgi-bin/webscr?cmd=_xclick&business=paypal@deekayen.net&item_name=Drupal%20contribution&currency_code=USD&amount=20.00
 */

/**
 * Label edit form, where each fieldset represents a starting workflow state.
 *
 * Each contains the transitions with that starting workflow state.
 *
 * @return
 *   array of form items for editing labels on transitions
 */
function workflow_admin_ui_labels_form($form, $form_state, $workflow, $op) {
  // $form = array();

  if (!is_object($workflow)) {
    drupal_set_message(t('Improper worklow ID provided.'), 'error');
    watchdog('workflow_named_transitions', 'Improper worklow ID provided.');
    drupal_goto('admin/config/workflow/workflow');
  }

  $headers = array(
    t('Transition from'),
    t('to'),
    t('label'),
  );

  $rows = array();
  // Use a variable, to set label of FROM state, including '(creation)'.
  $first_row = TRUE;
  // Get transitions, sorted by weight of the old state.
  $config_transitions = $workflow->getTransitions();
  usort($config_transitions, '_workflow_transitions_sort_by_weight');
  foreach ($config_transitions as $transition) {
    $old_state = workflow_state_load($transition->sid);
    $new_state = workflow_state_load($transition->target_sid);
    $rows[] = array(
      'data' => array(
        array('data' => ($first_row || $old_state == $new_state) ? $old_state->label() : ''),
        array('data' => $new_state->label() ),
        array(
          'data' => array(
            '#type' => 'textfield',
            '#value' => $transition->label(),
            '#size' => 60, 
            '#maxlength' => 128, 
            '#name' => 'label_' . $transition->tid,
            '#id' => 'label_' . $transition->tid,
          ),
        ),
      ),
    );
    $first_row = FALSE;
  }
  $form['transition_labels'] = array(
    '#theme' => 'table',
    '#header' => $headers,
    '#rows' => $rows,
  );

  // Save the transitions in the form to fetch upon submit.
  $form['#transitions'] = $config_transitions;

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Submit',
  );

  return $form;
}

/**
 * Automatic submission handler for the Transition labels form.
 *
 * @see workflow_admin_ui_labels_form()
 */
function workflow_admin_ui_labels_form_submit($form, &$form_state) {
  foreach($form['#transitions'] as $config_transition) {
    $config_transition->label = trim($form_state['input']['label_' . $config_transition->tid]);
    $config_transition->save();
  }
  drupal_set_message(t('The transition labels have been saved.'));
}

/**
 * Helper function to sort the transitions.
 */
function _workflow_transitions_sort_by_weight($a, $b) {
  $old_state_a1 = workflow_state_load($a->sid);
  $old_state_b1 = workflow_state_load($b->sid);
  if ($old_state_a1->weight < $old_state_b1->weight)  return -1;
  if ($old_state_a1->weight > $old_state_b1->weight)  return +1;

  $old_state_a2 = workflow_state_load($a->target_sid);
  $old_state_b2 = workflow_state_load($b->target_sid);
  if ($old_state_a1->weight == $old_state_b1->weight && $old_state_a2->weight == $old_state_b2->weight) return 0;
  if ($old_state_a1->weight == $old_state_b1->weight && $old_state_a2->weight < $old_state_b2->weight)  return -1;
  if ($old_state_a1->weight == $old_state_b1->weight && $old_state_a2->weight > $old_state_b2->weight)  return +1;
  return ($old_state_a->weight < $old_state_b->weight) ? -1 : 1;
}
