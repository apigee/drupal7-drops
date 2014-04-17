<?php

class dda_handler_filter_status extends views_handler_filter_boolean_operator {
  public function construct() {
    parent::construct();
    if ($this->value_value == t('True')) {
      $this->value_value = t('Pending');
    }
  }
  public function get_value_options() {
    $this->value_options = array(-1 => t('Revoked'), 0 => t('Pending'), 1 => t('Approved'));
  }

  function query() {
    $this->ensure_my_table();
    $field = "$this->table_alias.$this->real_field";
    $this->query->add_where($this->options['group'], $field, $this->value, '=');
  }
}
