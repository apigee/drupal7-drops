<?php

function d8cmi_drush_command() {
  $items = array();

  $items['d8cmi-variable-get'] = array(
    'callback' => 'd8cmi_drush_vget',
    'description' => dt('Gets a particular variable from a module/theme YAML config file.'),
    'arguments' => array(
      'object' => dt('The module or theme from which to pull the variable.'),
      'key' => dt('The key or variable name whose value is to be returned.')
    ),
    'options' => array(
      'format' => array(
        'description' => 'Format to output the object. Use "print_r" for print_r (default), "export" for var_export, and "json" for JSON.',
        'example-value' => 'export',
      ),
    ),

    'aliases' => array('d8-vget')
  );
  $items['d8cmi-variable-set'] = array(
    'callback' => 'd8cmi_drush_vset',
    'description' => dt('Writes a particular variable to a module/theme YAML config file.'),
    'arguments' => array(
      'object' => dt('The module or theme to which to write the variable.'),
      'key' => dt('The key or variable name whose value is to be written.'),
      'value' => dt('The value to be written to the key.')
    ),
    'options' => array(
      'format' => array(
        'description' => 'Format to parse the object. Use "auto" to detect format from value (default), "string", "integer" or "boolean" for corresponding primitive type, and "json" for JSON.',
        'example-value' => 'boolean',
      ),
    ),

    'aliases' => array('d8-vset')
  );

  return $items;
}

function d8cmi_drush_vget() {
  $args = func_get_args();
  if (count($args) < 2) {
    drush_set_error(dt('Object and key required'));
    return;
  }
  list($object, $key) = $args;
  try {
    $config = Drupal::config($object);
  }
  catch (Exception $e) {
    drush_set_error(dt('No such module or theme @obj.', array('@obj' => $object)));
    return;
  }
  $value = $config->get($key);
  drush_print(drush_format($value, $key));
}

function d8cmi_drush_vset() {
  $args = func_get_args();
  if (count($args) < 2) {
    drush_set_error(dt('Object, key and value required'));
    return '';
  }
  list($object, $key, $value) = $args;
  try {
    $config = Drupal::config($object);
  }
  catch (Exception $e) {
    drush_set_error(dt('No such module or theme @obj.', array('@obj' => $object)));
    return '';
  }

  // If the value is a string (usual case, unless we are called from code),
  // then format the input
  if (is_string($value)) {
    $value = _drush_variable_format($value, drush_get_option('format', 'auto'));
  }
  $config->set($key, $value);
  $config->save();

  // Format the output for display
  if (is_array($value)) {
    $display = "\n" . var_export($value, TRUE);
  }
  elseif (is_integer($value)) {
    $display = $value;
  }
  elseif (is_bool($value)) {
    $display = $value ? "TRUE" : "FALSE";
  }
  else {
    $display = '"' . $value . '"';
  }
  drush_log(dt('!name was set to !value.', array('!name' => $key, '!value' => $display)), 'success');
  return '';
}