<?php
$context = new stdClass();
$context->disabled = FALSE; /* Edit this to true to make a default context disabled initially */
$context->api_version = 3;
$context->name = 'home';
$context->description = '';
$context->tag = '';
$context->conditions = array(
  'path' => array(
    'values' => array(
      'home' => 'home',
    ),
  ),
);
$context->reactions = array(
  'block' => array(
    'blocks' => array(
      'views-devconnect_blog-block_1' => array(
        'module' => 'views',
        'delta' => 'devconnect_blog-block_1',
        'region' => 'content',
        'weight' => '-10',
      ),
      'views-home_featured_forum_posts-block' => array(
        'module' => 'views',
        'delta' => 'home_featured_forum_posts-block',
        'region' => 'content',
        'weight' => '-9',
      ),
      'views-smartdocs_methods-frontpage' => array(
        'module' => 'views',
        'delta' => 'smartdocs_methods-frontpage',
        'region' => 'content',
        'weight' => '-8',
      ),
      'devconnect_homepage-homepage_header' => array(
        'module' => 'devconnect_homepage',
        'delta' => 'homepage_header',
        'region' => 'homepage_header',
        'weight' => '-10',
      ),
    ),
  ),
);
$context->condition_mode = 0;
