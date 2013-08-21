<?php
/**
 * @file
 * devconnect_default_content_structure.context.inc
 */

/**
 * Implements hook_context_default_contexts().
 */
function devconnect_default_structure_context_default_contexts() {
  $export = array();

  $context = new stdClass();
  $context->disabled = FALSE; /* Edit this to true to make a default context disabled initially */
  $context->api_version = 3;
  $context->name = 'blog';
  $context->description = '';
  $context->tag = '';
  $context->conditions = array(
    'path' => array(
      'values' => array(
        'blog' => 'blog',
      ),
    ),
  );
  $context->reactions = array(
    'block' => array(
      'blocks' => array(
        'views-hot_topics-block' => array(
          'module' => 'views',
          'delta' => 'hot_topics-block',
          'region' => 'sidebar_second',
          'weight' => '-10',
        ),
      ),
    ),
  );
  $context->condition_mode = 0;
  $export['blog'] = $context;

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
        'devconnect_homepage-homepage_header' => array(
          'module' => 'devconnect_homepage',
          'delta' => 'homepage_header',
          'region' => 'homepage_header',
          'weight' => '-10',
        ),
        'views-home_featured_forum_posts-block' => array(
          'module' => 'views',
          'delta' => 'home_featured_forum_posts-block',
          'region' => 'content',
          'weight' => '-18',
        ),
        'views-recent_blog_posts-block_1' => array(
          'module' => 'views',
          'delta' => 'recent_blog_posts-block_1',
          'region' => 'content',
          'weight' => '-18',
        ),
        'devconnect_default_structure-develop-with-these' => array(
          'module' => 'devconnect_default_structure',
          'delta' => 'develop-with-these',
          'region' => 'content',
          'weight' => '-10',
        ),
      ),
    ),
  );
  $context->condition_mode = 0;
  $export['home'] = $context;

  $context = new stdClass();
  $context->disabled = FALSE; /* Edit this to true to make a default context disabled initially */
  $context->api_version = 3;
  $context->name = 'signin';
  $context->description = '';
  $context->tag = '';
  $context->conditions = array(
    'path' => array(
      'values' => array(
        'user' => 'user',
        'user/*' => 'user/*',
      ),
    ),
    'user' => array(
      'values' => array(
        'anonymous user' => 'anonymous user',
      ),
    ),
  );
  $context->reactions = array();
  $context->condition_mode = 1;
  $export['signin'] = $context;

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
  $export['sitewide'] = $context;

  return $export;
}
