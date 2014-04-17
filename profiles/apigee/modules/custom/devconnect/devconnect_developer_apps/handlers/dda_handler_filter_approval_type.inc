<?php
class dda_handler_filter_approval_type extends views_handler_filter_boolean_operator {
  public function construct() {
    parent::construct();
    if ($this->value_value == t('True')) {
      $this->value_value = t('Auto');
    }
  }
  public function get_value_options() {
    $this->value_options = array(0 => t('Manual'), 1 => t('Auto'));
  }
}