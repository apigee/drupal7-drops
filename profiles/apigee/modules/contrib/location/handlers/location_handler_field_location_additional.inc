<?php
// $Id: location_handler_field_location_country.inc,v 1.1 2008/09/10 22:41:52 bdragon Exp $

/**
 * @file
 * Additional field handler.
 */

class location_handler_field_location_additional extends views_handler_field {

  function option_definition() {
    $options = parent::option_definition();
    $options['style'] = array('default' => 'name');
    return $options;
  }

  function render($values) {
    return check_plain(trim($values->{$this->aliases['additional']}));
  }
}
