<?php
/**
 * @file
 * devconnect_download.ds.inc
 */

/**
 * Implements hook_ds_field_settings_info().
 */
function devconnect_download_ds_field_settings_info() {
  $export = array();

  $ds_fieldsetting = new stdClass();
  $ds_fieldsetting->api_version = 1;
  $ds_fieldsetting->id = 'devconnect_download|devconnect_download|default';
  $ds_fieldsetting->entity_type = 'devconnect_download';
  $ds_fieldsetting->bundle = 'devconnect_download';
  $ds_fieldsetting->view_mode = 'default';
  $ds_fieldsetting->settings = array(
    'signed_download' => array(
      'weight' => '1',
      'label' => 'hidden',
      'format' => 'default',
    ),
  );
  $export['devconnect_download|devconnect_download|default'] = $ds_fieldsetting;

  return $export;
}

/**
 * Implements hook_ds_custom_fields_info().
 */
function devconnect_download_ds_custom_fields_info() {
  $export = array();

  $ds_field = new stdClass();
  $ds_field->api_version = 1;
  $ds_field->field = 'signed_download';
  $ds_field->label = 'Signed Download';
  $ds_field->field_type = 5;
  $ds_field->entities = array(
    'devconnect_download' => 'devconnect_download',
  );
  $ds_field->properties = array(
    'code' => array(
      'value' => file_get_contents(dirname(__FILE__)."/devconnect_download_signed_download.php"),
      'format' => 'ds_code',
    ),
    'use_token' => 0,
  );
  $export['signed_download'] = $ds_field;

  return $export;
}

/**
 * Implements hook_ds_layout_settings_info().
 */
function devconnect_download_ds_layout_settings_info() {
  $export = array();

  $ds_layout = new stdClass();
  $ds_layout->api_version = 1;
  $ds_layout->id = 'devconnect_download|devconnect_download|default';
  $ds_layout->entity_type = 'devconnect_download';
  $ds_layout->bundle = 'devconnect_download';
  $ds_layout->view_mode = 'default';
  $ds_layout->layout = 'ds_1col';
  $ds_layout->settings = array(
    'hide_empty_regions' => 0,
    'hide_sidebars' => 0,
    'regions' => array(
      'ds_content' => array(
        0 => 'field_title',
        1 => 'signed_download',
      ),
    ),
    'fields' => array(
      'field_title' => 'ds_content',
      'signed_download' => 'ds_content',
    ),
    'classes' => array(),
  );
  $export['devconnect_download|devconnect_download|default'] = $ds_layout;

  $ds_layout = new stdClass();
  $ds_layout->api_version = 1;
  $ds_layout->id = 'devconnect_download|devconnect_download|teaser';
  $ds_layout->entity_type = 'devconnect_download';
  $ds_layout->bundle = 'devconnect_download';
  $ds_layout->view_mode = 'teaser';
  $ds_layout->layout = 'ds_2col';
  $ds_layout->settings = array(
    'hide_empty_regions' => 0,
    'hide_sidebars' => 0,
    'regions' => array(
      'left' => array(
        0 => 'field_title',
      ),
      'right' => array(
        0 => 'signed_download',
      ),
    ),
    'fields' => array(
      'field_title' => 'left',
      'signed_download' => 'right',
    ),
    'classes' => array(),
  );
  $export['devconnect_download|devconnect_download|teaser'] = $ds_layout;

  return $export;
}
