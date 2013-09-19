<?php
/**
 * @file
 * Provide views field handler for workflow.module.
 */

/**
 * Field handler to provide simple status name or renderer.
 */
class workflow_views_handler_field_sid extends views_handler_field {

  function option_definition() {
    $options = parent::option_definition();
    $options['value'] = array('default' => FALSE, 'bool' => TRUE);
    return $options;
  }

  function options_form(&$form, &$form_state) {
    $form['value'] = array(
      '#title' => t('Display value'),
      '#description' => t('If checked, row field value will be displayed.'),
      '#type' => 'checkbox',
      '#default_value' => $this->options['value'],
    );
    parent::options_form($form, $form_state);
  }

  function render($values) {
    if ($this->options['value']) {
      if (empty($values->{$this->field_alias})) {
        return NULL;
      }
      return $values->{$this->field_alias};
    }
    else {
      if (empty($values->{$this->field_alias})) {
        return t('No state');
      }
      static $states;
      if (!isset($states)) {
        $states = array();
        foreach (workflow_get_workflow_states() as $state) {
          $states[$state->sid] = $state->state;
        }
      }
      $output = t($states[$values->{$this->field_alias}]);
      if (empty($output)) {
        $output = t('Unknown state');
      }
      return check_plain($output);
    }
  }
}
