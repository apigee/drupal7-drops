<?php
$context = new stdClass();
$context->disabled = FALSE; /* Edit this to true to make a default context disabled initially */
$context->api_version = 3;
$context->name = 'apigee_base_theme_blocks';
$context->description = '';
$context->tag = '';
$context->conditions = array(
  'theme' => array(
    'values' => array(
      'apigee_base' => 'apigee_base',
    ),
  ),
);
$context->reactions = array(
  'block' => array(
    'blocks' => array(
      'forum-active' => array(
        'module' => 'forum',
        'delta' => 'active',
        'region' => 'header',
        'weight' => '-10',
      ),
      'forum-new' => array(
        'module' => 'forum',
        'delta' => 'new',
        'region' => 'header',
        'weight' => '-9',
      ),
      'blog-recent' => array(
        'module' => 'blog',
        'delta' => 'recent',
        'region' => 'header',
        'weight' => '-8',
      ),
      'comment-recent' => array(
        'module' => 'comment',
        'delta' => 'recent',
        'region' => 'header',
        'weight' => '-7',
      ),
      'node-recent' => array(
        'module' => 'node',
        'delta' => 'recent',
        'region' => 'header',
        'weight' => '-6',
      ),
      'search-form' => array(
        'module' => 'search',
        'delta' => 'form',
        'region' => 'header',
        'weight' => '-5',
      ),
      'user-new' => array(
        'module' => 'user',
        'delta' => 'new',
        'region' => 'header',
        'weight' => '-4',
      ),
      'user-online' => array(
        'module' => 'user',
        'delta' => 'online',
        'region' => 'header',
        'weight' => '-3',
      ),
      'system-help' => array(
        'module' => 'system',
        'delta' => 'help',
        'region' => 'help',
        'weight' => '-10',
      ),
    ),
  ),
);
$context->condition_mode = 0;