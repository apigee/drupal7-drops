<?php
/**
 * @file
 * assemble.default_layout_region.inc
 */

/**
 * Implements hook_default_layout_region().
 */
function assemble_default_layout_region() {
  $export = array();

  $layout_region = new stdClass();
  $layout_region->disabled = FALSE; /* Edit this to true to make a default layout_region disabled initially */
  $layout_region->api_version = 1;
  $layout_region->name = 'footer_d';
  $layout_region->admin_title = 'Footer D';
  $export['footer_d'] = $layout_region;

  $layout_region = new stdClass();
  $layout_region->disabled = FALSE; /* Edit this to true to make a default layout_region disabled initially */
  $layout_region->api_version = 1;
  $layout_region->name = 'footer_e';
  $layout_region->admin_title = 'Footer E';
  $export['footer_e'] = $layout_region;

  $layout_region = new stdClass();
  $layout_region->disabled = FALSE; /* Edit this to true to make a default layout_region disabled initially */
  $layout_region->api_version = 1;
  $layout_region->name = 'footer_f';
  $layout_region->admin_title = 'Footer F';
  $export['footer_f'] = $layout_region;

  $layout_region = new stdClass();
  $layout_region->disabled = FALSE; /* Edit this to true to make a default layout_region disabled initially */
  $layout_region->api_version = 1;
  $layout_region->name = 'sidebar_d';
  $layout_region->admin_title = 'Sidebar D';
  $export['sidebar_d'] = $layout_region;

  $layout_region = new stdClass();
  $layout_region->disabled = FALSE; /* Edit this to true to make a default layout_region disabled initially */
  $layout_region->api_version = 1;
  $layout_region->name = 'subheader_d';
  $layout_region->admin_title = 'Subheader D';
  $export['subheader_d'] = $layout_region;

  return $export;
}
