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
    $sid = $values->{$this->field_alias};
    if ($this->options['value']) {
      return (empty($sid)) ? NULL : $sid;
    }
    else {
      return _workflow_views_get_workflow_state_name($sid);
    }
  }
}
