<?php
/**
 * @file
 * Provide views filter handler for workflow.module.
 */

/**
 * Filter by state.
 */
class workflow_views_handler_filter_sid extends views_handler_filter_in_operator {
  var $value_form_type = 'select';

  function get_value_options() {
    if (!isset($this->value_options)) {
      $this->value_title = t('Workflow state');
      $workflows = array();
      foreach (workflow_get_workflows() as $data) {
        $workflows[$data->wid] = check_plain(t($data->name));
      }
      if (!isset($workflows) || empty($workflows)) {
        $this->value_options = array();
        return;
      }
      $options = array('status' => 1);
      if (count($workflows) > 1) {
        $states = array('' => t('No state'));
        foreach ($workflows as $wid => $name) {
          foreach (workflow_get_workflow_states_by_wid($wid, $options) as $state) {
            $states[$state->sid] = check_plain($name . ': ' . $state->state);
          }
        }
      }
      else {
        foreach (workflow_get_workflow_states($options) as $state) {
          $states[$state->sid] = check_plain($state->state);
        }
      }
      $this->value_options = $states;
    }
  }

  function query() {
    $value = $this->is_a_group() && !$this->options['expose']['multiple'] ? drupal_array_merge_deep_array($this->value) : $this->value;
    if (empty($value)) {
      return;
    }
    $this->ensure_my_table();
    $placeholder = !empty($this->definition['numeric']) ? '%d' : "'%s'";
    if (count($value) == 1) {
      $this->operator = ($this->operator == 'in') ? '= ' : '!= ';
      $in = !empty($this->definition['numeric']) ? '%d' : "'%s'";
    }

    if ($this->operator == 'empty' || $this->operator == 'not empty') {
      $value = NULL;
      if ($this->operator == 'empty') {
        $this->operator = "IS NULL";
      }
      else {
        $this->operator = "IS NOT NULL";
      }
    }

    $this->query->add_where($this->options['group'], $this->table_alias . '.' . $this->real_field, $value,  $this->operator);
  }
}
