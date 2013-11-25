<?php
/**
 * @file
 * Implements Feeds mapping API
 */

/**
 * Implements hook_feeds_processor_targets_alter().
 */
function cck_phone_feeds_processor_targets_alter(&$targets, $entity_type, $bundle_name) {
  foreach (field_info_instances($entity_type, $bundle_name) as $name => $instance) {
    $info = field_info_field($name);
    if ($info['type'] == 'phone_number') {
      $targets[$name .':country_codes'] = array(
        'name' => t('!label - country code', array('!label' => check_plain($instance['label']))),
        'callback' => 'cck_phone_feeds_set_target',
        'description' => t('The @label field.', array('@label' => $instance['label'])),
      );
      $targets[$name .':number'] = array(
        'name' => t('!label - phone number', array('!label' => check_plain($instance['label']))),
        'callback' => 'cck_phone_feeds_set_target',
        'description' => t('The @label field.', array('@label' => $instance['label'])),
      );
      $targets[$name .':extension'] = array(
        'name' => t('!label - extension', array('!label' => check_plain($instance['label']))),
        'callback' => 'cck_phone_feeds_set_target',
        'description' => t('The @label field.', array('@label' => $instance['label'])),
      );
    }
  }
}

/**
 * Callback for feed mapping.
 */
function cck_phone_feeds_set_target($source, $entity, $target, $value) {
  if (empty($value)) {
    return;
  }

  // Handle non-multiple value fields.
  if (!is_array($value)) {
    $value = array($value);
  }

  // Iterate over all values.
  $i = 0;
  $info = field_info_field($target);
  list($field_name, $sub_field) = explode(':', $target);

  // We will call this multiple times, preserve existing values.
  $field = empty($entity->{$field_name}) ? array() : $entity->{$field_name};

  foreach ($value as $v) {
    if ($sub_field == 'country_codes') {
      $v = strtolower($v);
    }
    if (!is_array($v) && !is_object($v)) {
      $field['und'][$i][$sub_field] = $v;
    }
    if ($info['cardinality'] == 1) {
      break;
    }
    $i++;
  }
  $entity->{$field_name} = $field;
}
