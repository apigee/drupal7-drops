<?php
$view = new view();
$view->name = 'home_featured_forum_posts';
$view->description = '';
$view->tag = 'default';
$view->base_table = 'node';
$view->human_name = 'Home Featured Forum Posts';
$view->core = 7;
$view->api_version = '3.0';
$view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

/* Display: Master */
$handler = $view->new_display('default', 'Master', 'default');
$handler->display->display_options = array(
  'title' => 'Forum Discussions',
  'use_more_always' => FALSE,
  'access' => array('type' => 'perm'),
  'cache' => array('type' => 'none'),
  'query' => array('options' => array('query_comment' => FALSE)),
  'exposed_form' => array('type' => 'basic'),
  'pager' => array('type' => 'some', 'options' => array('items_per_page' => 5)),
  'style_plugin' => 'default',
  'row_plugin' => 'fields',
  'fields' => array(
    'path' => array(
      'id' => 'path',
      'table' => 'node',
      'field' => 'path',
      'label' => '',
      'exclude' => TRUE,
      'element_label_colon' => FALSE
    ),
    'title' => array(
      'id' => 'title',
      'table' => 'node',
      'field' => 'title',
      'label' => '',
      'exclude' => TRUE,
      'alter' => array('word_boundary' => FALSE, 'ellipsis' => FALSE),
      'element_label_colon' => FALSE
    ),
    'last_updated' => array(
      'id' => 'last_updated',
      'table' => 'node_comment_statistics',
      'field' => 'last_updated',
      'label' => '',
      'alter' => array('alter_text' => TRUE, 'text' => "[title]<br>\n[last_updated]"),
      'element_label_colon' => FALSE,
      'date_format' => 'time ago'
    ),
    'comment_count' => array(
      'id' => 'comment_count',
      'table' => 'node_comment_statistics',
      'field' => 'comment_count',
      'label' => '',
      'alter' => array(
        'alter_text' => TRUE,
        'text' => "<div class=\"comment-count\">\n[comment_count]\n</div>",
        'make_link' => TRUE,
        'path' => '[path]',
        'absolute' => TRUE
      ),
      'element_label_colon' => FALSE
    )
  ),
  'sorts' => array(
    'status' => array(
      'id' => 'status',
      'table' => 'node',
      'field' => 'status',
      'order' => 'DESC'
    ),
    'promote' => array(
      'id' => 'promote',
      'table' => 'node',
      'field' => 'promote',
      'order' => 'DESC'
    )
  ),
  'filters' => array(
    'status' => array(
      'id' => 'status',
      'table' => 'node',
      'field' => 'status',
      'value' => 1,
      'group' => 0,
      'expose' => array('operator' => FALSE)
    ),
    'type' => array(
      'id' => 'type',
      'table' => 'node',
      'field' => 'type',
      'value' => array('forum' => 'forum')
    )
  )
);

/* Display: Block */
$handler = $view->new_display('block', 'Block', 'block');