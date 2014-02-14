<?php
$view = new view();
$view->name = 'hot_topics';
$view->description = '';
$view->tag = 'default';
$view->base_table = 'node';
$view->human_name = 'Hot Topics';
$view->core = 7;
$view->api_version = '3.0';
$view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

/* Display: Master */
$handler = $view->new_display('default', 'Master', 'default');
$handler->display->display_options = array(
  'title' => 'Hot Topics',
  'use_more_always' => FALSE,
  'access' => array('type' => 'perm'),
  'cache' => array('type' => 'none'),
  'query' => array('type' => 'views_query', 'options' => array('query_comment' => FALSE)),
  'exposed_form' => array('type' => 'basic'),
  'pager' => array('type' => 'some', 'options' => array('items_per_page' => 10, 'offset' => 0)),
  'style_plugin' => 'default',
  'row_plugin' => 'fields',
  'fields' => array(
    'term_node_tid' => array(
      'id' => 'term_node_tid',
      'table' => 'node',
      'field' => 'term_node_tid',
      'label' => '',
      'element_label_colon' => FALSE,
      'type' => 'ul',
      'vocabularies' => array('forums' => 0, 'blog' => 0, 'tags' => 0)
    )
  ),
  'filters' => array(
    'status' => array(
      'id' => 'status',
      'table' => 'node',
      'field' => 'status',
      'value' => 1,
      'group' => 1,
      'expose' => array('operator' => FALSE)
    ),
    'type' => array(
      'id' => 'type',
      'table' => 'node',
      'field' => 'type',
      'value' => array('blog' => 'blog', 'article' => 'article')
    )
  )
);


/* Display: Block */
$handler = $view->new_display('block', 'Block', 'block');
$handler->display->display_options['block_description'] = 'Hot Topics';