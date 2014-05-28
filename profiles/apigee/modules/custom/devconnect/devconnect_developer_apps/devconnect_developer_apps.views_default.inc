<?php

/**
 * Implements hook_views_default_views().
 */
function devconnect_developer_apps_views_default_views() {
  $view = new view();
  $view->name = 'developer_apps';
  $view->description = '';
  $view->tag = 'default';
  $view->base_table = 'dc_dev_app';
  $view->human_name = 'Developer Apps';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options = array(
    'title' => 'Developer Apps',
    'use_more_always' => FALSE,
    'access' => array('type' => 'perm', 'perm' => 'administer developer_apps'),
    'cache' => array('type' => 'none'),
    'query' => array('type' => 'views_query'),
    'exposed_form' => array('type' => 'basic'),
    'pager' => array('type' => 'full', 'options' => array('items_per_page' => 40)),
    'style_plugin' => 'table',
    'style_options' => array(
      'columns' => array(
        'uid' => 'uid',
        'field_first_name' => 'field_first_name',
        'field_last_name' => 'field_last_name',
        'app_name' => 'app_name',
        'username' => 'username',
        'api_key' => 'api_key',
        'api_product' => 'api_product',
        'status' => 'status',
        'edit_link' => 'edit_link',
        'delete_link' => 'delete_link',
      ),
      'default' => 'username',
      'info' => array(
        'uid' => array(
          'sortable' => 0,
          'default_sort_order' => 'asc',
          'align' => '',
          'separator' => '',
          'empty_column' => 0,
        ),
        'field_first_name' => array(
          'sortable' => 0,
          'default_sort_order' => 'asc',
          'align' => '',
          'separator' => '',
          'empty_column' => 0,
        ),
        'field_last_name' => array(
          'sortable' => 0,
          'default_sort_order' => 'asc',
          'align' => '',
          'separator' => '',
          'empty_column' => 0,
        ),
        'app_name' => array(
          'sortable' => 1,
          'default_sort_order' => 'asc',
          'align' => '',
          'separator' => '',
          'empty_column' => 0,
        ),
        'username' => array(
          'sortable' => 1,
          'default_sort_order' => 'asc',
          'align' => '',
          'separator' => '',
          'empty_column' => 0,
        ),
        'api_key' => array(
          'sortable' => 0,
          'default_sort_order' => 'asc',
          'align' => '',
          'separator' => '',
          'empty_column' => 0,
        ),
        'api_product' => array(
          'sortable' => 1,
          'default_sort_order' => 'asc',
          'align' => '',
          'separator' => '',
          'empty_column' => 0,
        ),
        'status' => array(
          'sortable' => 1,
          'default_sort_order' => 'asc',
          'align' => '',
          'separator' => '',
          'empty_column' => 0,
        ),
        'edit_link' => array(
          'align' => '',
          'separator' => '',
          'empty_column' => 0,
        ),
        'delete_link' => array(
          'align' => '',
          'separator' => '',
          'empty_column' => 0,
        )
      )
    ),
    'relationships' => array(
      'uid' => array(
        'id' => 'uid',
        'table' => 'dc_dev_app',
        'field' => 'uid'
      )
    ),
    'fields' => array(
      'uid' => array(
        'id' => 'uid',
        'table' => 'dc_dev_app',
        'field' => 'uid',
        'exclude' => TRUE,
        'separator' => ''
      ),
      'field_first_name' => array(
        'id' => 'field_first_name',
        'table' => 'field_data_field_first_name',
        'field' => 'field_first_name',
        'relationship' => 'uid',
        'exclude' => TRUE
      ),
      'field_last_name' => array(
        'id' => 'field_last_name',
        'table' => 'field_data_field_last_name',
        'field' => 'field_last_name',
        'relationship' => 'uid',
        'exclude' => TRUE
      ),
      'app_name' => array(
        'id' => 'app_name',
        'table' => 'dc_dev_app',
        'field' => 'name',
        'alter' => array('make_link' => TRUE, 'path' => 'user/[uid]/apps/[app_name]')
      ),
      'username' => array(
        'id' => 'username',
        'table' => 'users',
        'field' => 'name',
        'relationship' => 'uid',
        'label' => 'Developer',
        'alter' => array('alter_text' => TRUE, 'text' => '[username] ([field_first_name] [field_last_name])')
      ),
      'api_key' => array(
        'id' => 'api_key',
        'table' => 'dc_dev_app',
        'field' => 'consumer_key'
      ),
      'api_product' => array(
        'id' => 'api_product',
        'table' => 'dc_api_product',
        'field' => 'api_product_display_name',
        'label' => 'API Products'
      ),
      'status' => array(
        'id' => 'status',
        'table' => 'dc_dev_app',
        'field' => 'overall_status',
        'label' => 'Status'
      ),
      'edit_link' => array(
        'id' => 'edit_link',
        'table' => 'views',
        'field' => 'nothing',
        'label' => '',
        'alter' => array('text' => 'Edit', 'make_link' => TRUE, 'path' => 'user/[uid]/apps/[app_name]/edit-app'),
        'element_label_colon' => FALSE
      ),
      'delete_link' => array(
        'id' => 'delete_link',
        'table' => 'views',
        'field' => 'nothing',
        'label' => '',
        'alter' => array('text' => 'Delete', 'make_link' => TRUE, 'path' => 'user/[uid]/apps/[app_name]/delete'),
        'element_label_colon' => FALSE
      ),
    ),
  );

  /* Display: Page */
  $handler = $view->new_display('page', 'Page', 'page');
  $handler->display->display_options = array(
    'path' => 'admin/reports/developer-apps',
    'menu' => array(
      'type' => 'normal',
      'title' => 'Developer Apps',
      'weight' => 0,
      'name' => 'management',
      'context' => 0,
      'context_only_inline' => 0
    )
  );

  return array($view->name => $view);
}
