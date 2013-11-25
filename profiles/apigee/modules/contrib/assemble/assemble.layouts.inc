<?php
/**
 * @file
 * assemble.layouts.inc
 */

/**
 * Implements hook_default_panels_layout().
 */
function assemble_default_panels_layout() {
  $export = array();

  $layout = new stdClass();
  $layout->disabled = FALSE; /* Edit this to true to make a default layout disabled initially */
  $layout->api_version = 1;
  $layout->name = 'assembler_demo';
  $layout->admin_title = 'Assembler Demo';
  $layout->admin_description = '';
  $layout->category = '';
  $layout->plugin = 'responsive';
  $layout->settings = array(
    'regions' => array(
      'header_a' => 'header_a',
      'header_b' => 'header_b',
      'header_c' => 'header_c',
      'subheader_a' => 'subheader_a',
      'subheader_b' => 'subheader_b',
      'subheader_c' => 'subheader_c',
      'navigation' => 'navigation',
      'title' => 'title',
      'sidebar_a' => 'sidebar_a',
      'body' => 'body',
      'sidebar_b' => 'sidebar_b',
      'sidebar_c' => 'sidebar_c',
      'footer_a' => 'footer_a',
      'footer_b' => 'footer_b',
      'footer_c' => 'footer_c',
    ),
    'overrides' => array(
      'smartphone' => array(),
      'tablet' => array(
        0 => array(
          'name' => 'header_b',
          'columns' => 3,
        ),
        1 => array(
          'name' => 'header_c',
          'columns' => 3,
        ),
        2 => array(
          'name' => 'subheader_b',
          'columns' => 3,
        ),
        3 => array(
          'name' => 'subheader_c',
          'columns' => 3,
        ),
        4 => array(
          'name' => 'footer_a',
          'columns' => 3,
        ),
        5 => array(
          'name' => 'footer_b',
          'columns' => 3,
        ),
      ),
      'standard' => array(
        0 => array(
          'name' => 'body',
          'columns' => 6,
        ),
        1 => array(
          'name' => 'sidebar_a',
          'columns' => 3,
        ),
        2 => array(
          'name' => 'sidebar_b',
          'columns' => 3,
        ),
        3 => array(
          'name' => 'subheader_a',
          'columns' => 4,
        ),
        4 => array(
          'name' => 'subheader_b',
          'columns' => 4,
        ),
        5 => array(
          'name' => 'subheader_c',
          'columns' => 4,
        ),
        6 => array(
          'name' => 'footer_a',
          'columns' => 4,
        ),
        7 => array(
          'name' => 'footer_b',
          'columns' => 4,
        ),
        8 => array(
          'name' => 'footer_c',
          'columns' => 4,
        ),
        9 => array(
          'name' => 'header_b',
          'columns' => 6,
        ),
        10 => array(
          'name' => 'header_c',
          'columns' => 6,
        ),
      ),
    ),
  );
  $export['assembler_demo'] = $layout;

  return $export;
}
