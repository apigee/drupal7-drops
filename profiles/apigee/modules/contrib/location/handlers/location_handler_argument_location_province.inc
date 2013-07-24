<?php

/**
 * Argument handler to accept a province code or name.
 */
class location_handler_argument_location_province extends views_handler_argument {

  function option_definition() {
    $options = parent::option_definition();
    $options['break_phrase'] = array('default' => FALSE);
    return $options;
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    // allow for , delimited values
    $form['break_phrase'] = array(
      '#type' => 'checkbox',
      '#title' => t('Allow multiple terms per argument.'),
      '#description' => t('If selected, users can enter multiple arguments in the form of 1,2,3.'),
      '#default_value' => !empty($this->options['break_phrase']),
    );
  }

  function query() {
    $this->ensure_my_table();
    if (!empty($this->options['break_phrase'])) {
      if ($this->argument != '') {
        $this->value = explode(',', $this->argument);
      }
      $this->operator = 'or';
    }
    else {
      $this->value = array($this->argument);
      $this->operator = 'or';
    }

    if (count($this->value) > 1) {
      $operator = empty($this->options['not']) ? 'IN' : 'NOT IN';
      $placeholders = '"' . implode('", "', $this->value) . '"';
      $this->query->add_where(0, "$this->table_alias.$this->real_field $operator ($placeholders)", $this->value);
    }
    else {
      $operator = empty($this->options['not']) ? '=' : '!=';
      $this->query->add_where(0, "$this->table_alias.$this->real_field $operator \"%s\"", $this->argument);
    }
  }

  /**
   * Override the behavior of title(). Get the province name.
   */
  function title() {
    if (isset($this->view->argument['country'])) {
      $country = $this->view->argument['country']->argument;
    }
    else {
      $country = variable_get('location_default_country', 'us');
    }
    $title = "";
    $values = explode(',', $this->argument);
    foreach ($values as $value) {
      $title .= location_province_name($country, $value).", ";
    }
    return substr($title,0,-2);
  }

  function summary_name($data) {
    $value = $data->{$this->name_alias};
    if (empty($value)) {
      return t('Unknown');
    }

    if (isset($this->view->argument['country'])) {
      $country = $this->view->argument['country']->argument;
    }
    else {
      $country = variable_get('location_default_country', 'us');
    }

    return location_province_name($country, $value);
  }
}
