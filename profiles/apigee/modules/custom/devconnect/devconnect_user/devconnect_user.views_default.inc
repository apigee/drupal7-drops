<?php

function devconnect_user_views_default_views() {

  $view = new view();
  $view->name = 'developer_save_queue';
  $view->description = '';
  $view->tag = 'default';
  $view->base_table = 'queue';
  $view->human_name = 'Developer Save Queue';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');

  $handler->display->display_options = array(
    'title' => 'Developer Save Queue',
    'use_more_always' => FALSE,
    'access' => array('type' => 'perm', 'perm' => 'administer devconnect_user settings'),
    'cache' => array('type' => 'none'),
    'query' => array('type' => 'views_query'),
    'exposed_form' => array('type' => 'basic'),
    'pager' => array('type' => 'full', 'options' => array('items_per_page' => 50)),
    'style_plugin' => 'table',

    'empty' => array(
      'area' => array(
        'id' => 'area',
        'table' => 'views',
        'field' => 'area',
        'empty' => TRUE,
        'content' => 'There are no pending developers in the queue.',
        'format' => 'full_html'
      ),
    ),

    'fields' => array(
      'item_id' => array(
        'id' => 'item_id',
        'table' => 'queue',
        'field' => 'item_id',
        'label' => '',
        'exclude' => TRUE,
        'element_label_colon' => FALSE,
      ),
      'username' => array(
        'id' => 'username',
        'table' => 'queue',
        'field' => 'data',
        'ui_name' => 'Username',
        'label' => 'Developer Username',
        'format' => 'key',
        'key' => 'userName',
      ),
      'firstname' => array(
        'id' => 'firstname',
        'table' => 'queue',
        'field' => 'data',
        'ui_name' => 'First Name',
        'label' => '',
        'exclude' => TRUE,
        'element_label_colon' => FALSE,
        'format' => 'key',
        'key' => 'firstName',
      ),
      'lastname' => array(
        'id' => 'lastname',
        'table' => 'queue',
        'field' => 'data',
        'ui_name' => 'Last Name',
        'label' => 'Full Name',
        'element_label_colon' => FALSE,
        'format' => 'key',
        'key' => 'lastName',
        'alter' => array('alter_text' => TRUE, 'text' => '[firstname] [lastname]'),
      ),
      'created' => array(
        'id' => 'created',
        'table' => 'queue',
        'field' => 'created',
        'label' => 'In Queue Since',
        'date_format' => 'short',
        'second_date_format' => 'long'
      ),
      'retries' => array(
        'id' => 'retries',
        'table' => 'views',
        'field' => 'nothing',
        'label' => 'Retries',
        'alter' => array('text' => '[item_id]')
      ),
      'actions' => array(
        'id' => 'actions',
        'table' => 'views',
        'field' => 'nothing',
        'label' => 'Retries',
        'alter' => array('text' => '<a href="' . base_path() . 'developer-queue/retry/[item_id]">Resubmit</a> | <a href="' . base_path() . 'developer-queue/delete/[item_id]">Remove</a>')
      ),
    ),

    'filters' => array(
      'name' => array(
        'id' => 'name',
        'table' => 'queue',
        'field' => 'name',
        'value' => 'developer'
      ),
    ),
  );

  /* Display: Page */
  $handler = $view->new_display('page', 'Page', 'page');
  $handler->display->display_options = array(
    'path' => 'admin/config/people/developer-save-queue',
    'menu' => array(
      'type' => 'normal',
      'title' => 'Developer Save Queue',
      'weight' => 0,
      'name' => 'management',
      'context' => 0,
      'context_only_inline' => 0,
    ),
  );

  return array($view->name => $view);
}