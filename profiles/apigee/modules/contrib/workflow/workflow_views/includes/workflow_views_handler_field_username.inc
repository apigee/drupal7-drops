<?php
/**
 * @file
 * Provide views field handler for workflow.module.
 */

/**
 * Field handler to allow linking to a user account
 */
class workflow_views_handler_field_username extends views_handler_field {
  /**
   * Override init function to add uid field.
   */
  function init(&$view, &$data) {
    parent::init($view, $data);
    $this->additional_fields['uid'] = 'uid';
  }

  function option_definition() {
    $options = parent::option_definition();
    $options['link_to_user'] = array('default' => TRUE);
    return $options;
  }

  function options_form(&$form, &$form_state) {
    $form['link_to_user'] = array(
      '#title' => t("Link this field to its user"),
      '#type' => 'checkbox',
      '#default_value' => $this->options['link_to_user'],
    );
    parent::options_form($form, $form_state);
  }

  function render_link($data, $values) {
    if (!empty($this->options['link_to_user']) && $this->get_value($values, 'uid') > 0) {
      $account = user_load($this->get_value($values, 'uid'));

      return theme('username', array(
        'account' => $account
      ));
    }
    else {
      return $data;
    }
  }

  function render($values) {
    $value = $this->get_value($values);
    return $this->render_link($this->sanitize_value($value), $values);
  }
}
