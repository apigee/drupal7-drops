<?php

class dda_handler_field_status extends views_handler_field {
  function render($values) {
    $value = $this->get_value($values);
    switch ($value) {
      case -1: return t('revoked'); break;
      case 1: return t('approved'); break;
      default: return t('pending'); break;
    }
  }
}
