<?php
$context = new stdClass();
$context->disabled = TRUE; /* Edit this to true to make a default context disabled initially */
$context->api_version = 3;
$context->name = 'smartdocs_sidebar_toc';
$context->description = 'Places a TOC sidebar on method nodes';
$context->tag = 'smartdocs';
$context->conditions = array(
  'node' => array(
    'values' => array(
      'smart_method' => 'smart_method',
    ),
    'options' => array(
      'node_form' => '1',
    ),
  ),
);
$context->reactions = array(
  'block' => array(
    'blocks' => array(
      'smartdocs-method_toc' => array(
        'module' => 'smartdocs',
        'delta' => 'method_toc',
        'region' => 'sidebar_second',
        'weight' => '-10',
      ),
    ),
  ),
);
$context->condition_mode = 0;
