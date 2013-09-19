<?php

/**
 * @file
 * Provide views argument handler for workflow.module.
 */

/**
 * Argument handler to accept a node type.
 */
class views_handler_argument_workflow_state extends views_handler_argument {
  function construct() {
    parent::construct('type');
  }

  /**
   * Override the behavior of summary_name(). Get the user-friendly version
   * of the workflow state.
   */
  function summary_name($data) {
    return $this->workflow_states($data->{$this->name_alias});
  }

  /**
   * Override the behavior of title(). Get the user-friendly version of the
   * workflow state.
   */
  function title() {
    return $this->workflow_states($this->argument);
  }

  /**
   * Helper function, to gather the workflow name from a given arguement.
   */
  function workflow_states($sid) {
    if (empty($sid)) {
      return t('No state');
    }
    static $states;
    if (!isset($states)) {
      // Get all, even the non-active states, filter by sid.
      foreach (workflow_get_workflow_states() as $state) {
        $states[$state->sid] = check_plain($state->state);
      }
    }
    $output = $states[$sid];
    if (empty($output)) {
      $output = t('No state');
    }
    return check_plain($output);
  }
}
