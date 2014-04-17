<?php
class dda_handler_field_approval_type extends views_handler_field {
  function render($values) {
    $value = $this->get_value($values);
    return $value ? t('auto') : t('manual');
  }
}
