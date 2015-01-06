<?php
/**
 * @file
 * Views handler for attribute filtering.
 */

/**
 * Filter by attribute name/value.
 *
 * @ingroup views_filter_handlers
 */
class du_handler_filter_attribute extends views_handler_filter_many_to_one {

  function init(&$view, &$options) {
    parent::init($view, $options);
    if (!empty($this->definition['attrib_name'])) {
      $this->options['attrib_name'] = $this->definition['attrib_name'];
    }
  }

  function has_extra_options() {
    return TRUE;
  }

  function get_value_options() {
    // don't overwrite the value options.
  }

  function option_definition() {
    $options = parent::option_definition();

    $options['attrib_name'] = array('default' => '');
    $options['attrib_value'] = array('default' => '');

    return $options;
  }

  function extra_options_form(&$form, &$form_state) {

    $query = db_select('dc_dev_attributes', 'd')
      ->fields('d', array('name'))
      ->distinct(TRUE)
      ->orderBy('name')
      ->execute();
    $options = array();
    while ($option = $query->fetchField()) {
      $options[$option] = $option;
    }
    $widget = (count($options) < 9 ? 'radios' : 'select');

    $form['attrib_name'] = array(
      '#type' => $widget,
      '#title' => t('Attribute name'),
      '#options' => $options,
      '#default_value' => $this->options['attrib_name'],
    );

  }

  function value_form(&$form, &$form_state) {

    $form['value'] = array(
      '#type' => 'textfield',
      '#title' => t('Attribute value'),
      '#default_value' => !empty($this->options['value']) ? $this->options['value'] : NULL,
    );

    if (empty($form_state['exposed'])) {
      // Retain the helper option.
      $this->helper->options_form($form, $form_state);
    }
  }

  private function _query($operator, $value) {
    $this->ensure_my_table();

    $value_alias = $this->table_alias . '.value';
    $name_alias = $this->table_alias . '.name';
    $group = $this->options['group'];

    if (empty($value)) {
      $value = NULL;
    }
    elseif (is_array($value)) {
      $value = array_values($value);
    }

    // We use array_values() because the checkboxes keep keys and that can cause
    // array addition problems.
    $this->query->add_where($group, $value_alias, $value, $operator);
    $this->query->add_where($group, $name_alias, $this->options['attrib_name'], '=');
  }

  function op_simple() {
    if (empty($this->value)) {
      return;
    }
    $this->_query($this->operator, $this->value);
  }

  function op_empty() {
    if ($this->operator == 'empty') {
      $operator = "IS NULL";
    }
    else {
      $operator = "IS NOT NULL";
    }
    $this->_query($operator, NULL);
  }

}
