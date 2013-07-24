<?php
/**
 * @file
 * devconnect_api_content_types.field_group.inc
 */

/**
 * Implements hook_field_group_info().
 */
function devconnect_api_content_types_field_group_info() {
  $export = array();

  $field_group = new stdClass;
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_test|node|api_method|form';
  $field_group->group_name = 'group_test';
  $field_group->entity_type = 'node';
  $field_group->bundle = 'api_method';
  $field_group->mode = 'form';
  $field_group->parent_name = '';
  $field_group->data = array(
    'label' => ' Test',
    'weight' => '5',
    'children' => array(
      0 => 'field_test_description',
      1 => 'field_test_method_type',
      2 => 'field_test_endpoint_uri',
      3 => 'field_test_request',
    ),
    'format_type' => 'fieldset',
    'format_settings' => array(
      'formatter' => 'collapsible',
      'instance_settings' => array(
        'description' => '',
        'classes' => '',
        'required_fields' => 1,
      ),
    ),
  );
  $export['group_test|node|api_method|form'] = $field_group;

  return $export;
}
