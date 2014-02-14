<?php
$context = new stdClass();
$context->disabled = FALSE; /* Edit this to true to make a default context disabled initially */
$context->api_version = 3;
$context->name = 'sitewide';
$context->description = '';
$context->tag = '';
$context->conditions = array(
  'sitewide' => array(
    'values' => array(
      1 => 1,
    ),
  ),
);
$context->reactions = array(
  'block' => array(
    'blocks' => array(
      'devconnect_default_structure-footer' => array(
        'module' => 'devconnect_default_structure',
        'delta' => 'footer',
        'region' => 'footer',
        'weight' => '-10',
      ),
    ),
  ),
);
$context->condition_mode = 0;