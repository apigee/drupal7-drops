<?php

/**
 * Implements hook_views_data().
 *
 * @return array
 */
function devconnect_developer_apps_views_data() {

  $base_table = 'dc_dev_app';
  $join_table = 'dc_dev_app_api_products';
  $right_table = 'dc_api_product';
  $attrib_table = 'dc_dev_app_attributes';

  $data[$base_table]= array(
    'table' => array(
      'group' => t('Developer Apps'),
      'base' => array(
        'field' => 'app_id', // primary key
        'title' => t('Developer Apps'),
        'help' => t('Contains cached data related to apps owned by developers.')
      )
    ),
    'name' => array(
      'title' => t('App Name'),
      'help' => t('Internal or machine-referenceable name for the app'),
      'field' => array(
        'handler' => 'views_handler_field',
        'click sortable' => TRUE, // This is use by the table display plugin.
      ),
      'sort' => array('handler' => 'views_handler_sort'),
      'filter' => array('handler' => 'views_handler_filter_string'),
      'argument' => array('handler' => 'views_handler_argument_string')
    ),
    'app_id' => array(
      'title' => t('App UUID'),
      'help' => t('Universally unique identifier for an app'),
      'field' => array(
        'handler' => 'views_handler_field',
        'click sortable' => TRUE, // This is use by the table display plugin.
      ),
      'sort' => array('handler' => 'views_handler_sort'),
      'filter' => array('handler' => 'views_handler_filter_string'),
      'argument' => array('handler' => 'views_handler_argument_string')
    ),
    'uid' => array(
      'title' => t('User ID'),
      'help' => t('User ID of the Drupal user owning an app'),
      'field' => array(
        'handler' => 'views_handler_field_numeric',
        'click sortable' => TRUE, // This is use by the table display plugin.
      ),
      'sort' => array('handler' => 'views_handler_sort'),
      'filter' => array('handler' => 'views_handler_filter_numeric'),
      'argument' => array('handler' => 'views_handler_argument_numeric'),
      'relationship' => array(
        'base' => 'users', // The name of the table to join with.
        'base field' => 'uid', // The name of the field on the joined table.
        // 'field' => 'nid' -- see hook_views_data_alter(); not needed here.
        'handler' => 'views_handler_relationship',
        'label' => t('Developers'),
        'title' => t('Developers'),
        'help' => t('Relates developer apps to users'),
      ),
    ),
    'display_name' => array(
      'title' => t('App Display Name'),
      'help' => t('Human-readable name for the app'),
      'field' => array(
        'handler' => 'views_handler_field',
        'click sortable' => TRUE, // This is use by the table display plugin.
      ),
      'sort' => array('handler' => 'views_handler_sort'),
      'filter' => array('handler' => 'views_handler_filter_string'),
      'argument' => array('handler' => 'views_handler_argument_string')
    ),
    'description' => array(
      'title' => t('App Description'),
      'help' => t('Short descriptive text about the app'),
      'field' => array(
        'handler' => 'views_handler_field',
        'click sortable' => TRUE, // This is use by the table display plugin.
      ),
      'sort' => array('handler' => 'views_handler_sort'),
      'filter' => array('handler' => 'views_handler_filter_string'),
      'argument' => array('handler' => 'views_handler_argument_string')
    ),
    'consumer_key' => array(
      'title' => t('API Key'),
      'help' => t('Consumer key used when authenticating with the API'),
      'field' => array(
        'handler' => 'views_handler_field',
        'click sortable' => TRUE, // This is use by the table display plugin.
      ),
      'sort' => array('handler' => 'views_handler_sort'),
      'filter' => array('handler' => 'views_handler_filter_string'),
      'argument' => array('handler' => 'views_handler_argument_string')
    ),
    'consumer_secret' => array(
      'title' => t('Consumer Secret'),
      'help' => t('Consumer secret key used when authenticating with the API'),
      'field' => array(
        'handler' => 'views_handler_field',
        'click sortable' => TRUE, // This is use by the table display plugin.
      ),
      'sort' => array('handler' => 'views_handler_sort'),
      'filter' => array('handler' => 'views_handler_filter_string'),
      'argument' => array('handler' => 'views_handler_argument_string')
    ),
    'overall_status' => array(
      'title' => t('Overall App Status'),
      'help' => t('Overall status (App, Credential and API Product)'),
      'field' => array(
        'handler' => 'dda_handler_field_status',
        'click sortable' => TRUE, // This is use by the table display plugin.
      ),
      'sort' => array('handler' => 'views_handler_sort'),
      'filter' => array('handler' => 'dda_handler_filter_status'),
      'argument' => array('handler' => 'views_handler_argument_numeric')
    ),
    'app_status' => array(
      'title' => t('App Status'),
      'help' => t('Indicates whether an App is in pending, approved or revoked status'),
      'field' => array(
        'handler' => 'dda_handler_field_status',
        'click sortable' => TRUE, // This is use by the table display plugin.
      ),
      'sort' => array('handler' => 'views_handler_sort'),
      'filter' => array('handler' => 'dda_handler_filter_status'),
      'argument' => array('handler' => 'views_handler_argument_numeric')
    ),
    'credential_status' => array(
      'title' => t('Credential Status'),
      'help' => t('Indicates whether an app\'s Credential is in pending, approved or revoked status'),
      'field' => array(
        'handler' => 'dda_handler_field_status',
        'click sortable' => TRUE, // This is use by the table display plugin.
      ),
      'sort' => array('handler' => 'views_handler_sort'),
      'filter' => array('handler' => 'dda_handler_filter_status'),
      'argument' => array('handler' => 'views_handler_argument_numeric')
    ),
    'callback_url' => array(
      'title' => t('Callback URI'),
      'help' => t('Callback URI registered for the app'),
      'field' => array(
        'handler' => 'views_handler_field',
        'click sortable' => TRUE, // This is use by the table display plugin.
      ),
      'sort' => array('handler' => 'views_handler_sort'),
      'filter' => array('handler' => 'views_handler_filter_string'),
      'argument' => array('handler' => 'views_handler_argument_string')
    ),
    'created_at' => array(
      'title' => t('Date/Time Created'),
      'help' => t('Indicates when the app was first created'),
      'field' => array(
        'handler' => 'views_handler_field_date',
        'click sortable' => TRUE, // This is use by the table display plugin.
      ),
      'sort' => array('handler' => 'views_handler_sort_date'),
      'filter' => array('handler' => 'views_handler_filter_date'),
      'argument' => array('handler' => 'views_handler_argument_date')
    ),
    'modified_at' => array(
      'title' => t('Date/Time Modified'),
      'help' => t('Indicates when the app was last modified'),
      'field' => array(
        'handler' => 'views_handler_field_date',
        'click sortable' => TRUE, // This is use by the table display plugin.
      ),
      'sort' => array('handler' => 'views_handler_sort_date'),
      'filter' => array('handler' => 'views_handler_filter_date'),
      'argument' => array('handler' => 'views_handler_argument_date')
    ),
    'access_type' => array(
      'title' => t('Access Type'),
      'help' => t('Indicates whether an app has read or write access, or both'),
      'field' => array(
        'handler' => 'views_handler_field',
        'click sortable' => TRUE, // This is use by the table display plugin.
      ),
      'sort' => array('handler' => 'views_handler_sort'),
      'filter' => array('handler' => 'views_handler_filter_string'),
      'argument' => array('handler' => 'views_handler_argument_string')
    ),
    'app_family' => array(
      'title' => t('App Family'),
      'help' => t('Indicates to which app family this app belongs, if any'),
      'field' => array(
        'handler' => 'views_handler_field',
        'click sortable' => TRUE, // This is use by the table display plugin.
      ),
      'sort' => array('handler' => 'views_handler_sort'),
      'filter' => array('handler' => 'views_handler_filter_string'),
      'argument' => array('handler' => 'views_handler_argument_string')
    )
  );

  $data[$right_table] = array(
    'table' => array(
      'group' => t('Developer Apps'),
    ),
    'api_product_display_name' => array(
      'real field' => 'display_name',
      'title' => t('API Product display name'),
      'help' => t('Human-readable name of the API Product'),
      'field' => array(
        'handler' => 'views_handler_field',
        'click sortable' => TRUE,
      ),
      'sort' => array('handler' => 'views_handler_sort'),
      'filter' => array('handler' => 'views_handler_filter_string'),
      'argument' => array('handler' => 'views_handler_argument_string')
    ),
    'api_product_approval_type' => array(
      'real field' => 'approval_type',
      'title' => t('API Product Approval Type'),
      'help' => t('Determines if apps are approved manually or automatically'),
      'field' => array(
        'handler' => 'dda_handler_field_approval_type',
        'click sortable' => TRUE,
      ),
      'sort' => array('handler' => 'views_handler_sort'),
      'filter' => array('handler' => 'dda_handler_filter_approval_type'),
      'argument' => array('handler' => 'views_handler_argument_string')
    ),
    'api_product_description' => array(
      'real field' => 'description',
      'title' => t('API Product Description'),
      'help' => t('Short descriptive text about API Product'),
      'field' => array(
        'handler' => 'views_handler_field',
        'click sortable' => TRUE,
      ),
      'sort' => array('handler' => 'views_handler_sort'),
      'filter' => array('handler' => 'views_handler_filter_string'),
      'argument' => array('handler' => 'views_handler_argument_string')
    ),
  );

  $data[$join_table] = array(
    'table' => array(
      'group' => t('Developer Apps'),
    ),
    'status' => array(
      'title' => t('API Product Status'),
      'help' => t('Credential status as it applies to an individual API Product'),
      'field' => array(
        'handler' => 'dda_handler_field_status',
        'click sortable' => TRUE, // This is use by the table display plugin.
      ),
      'sort' => array('handler' => 'views_handler_sort'),
      'filter' => array('handler' => 'dda_handler_filter_status'),
      'argument' => array('handler' => 'views_handler_argument_numeric')
    ),
    'apiproduct_name' => array(
      'real field' => 'name',
      'title' => t('API Product internal name'),
      'help' => t('Internal, machine-parseable unique name of the API Product'),
      'field' => array(
        'handler' => 'views_handler_field',
        'click sortable' => TRUE,
      ),
      'sort' => array('handler' => 'views_handler_sort'),
      'filter' => array('handler' => 'views_handler_filter_string'),
      'argument' => array('handler' => 'views_handler_argument_string')
    ),
  );

  $data[$attrib_table] = array(
    'table' => array('group' => t('Developer Apps')),
    'attribute_name' => array(
      'real field' => 'name',
      'title' => t('Attribute Name'),
      'help' => t('Name of the developer app attribute'),
      'field' => array('handler' => 'views_handler_field'),
      'sort' => array('handler' => 'views_handler_sort'),
      'filter' => array('handler' => 'views_handler_filter_string'),
      'argument' => array('handler' => 'views_handler_argument_string')

    ),
    'attribute_value' => array(
      'real field' => 'value',
      'title' => t('Attribute Value'),
      'help' => t('Value of the developer app attribute'),
      'field' => array('handler' => 'views_handler_field'),
      'sort' => array('handler' => 'views_handler_sort'),
      'filter' => array('handler' => 'views_handler_filter_string'),
      'argument' => array('handler' => 'views_handler_argument_string')
    ),
    'attribute_filter' => array(
      'real field' => 'value',
      'title' => t('Attribute'),
      'help' => t('Value for a named attribute'),
      'filter' => array('handler' => 'dda_handler_filter_attribute')
    )
  );

  $data[$join_table]['table']['join'][$base_table] = array(
    'left_field' => 'app_id',
    'field' => 'app_id'
  );
  $data[$attrib_table]['table']['join'][$base_table] = array(
    'left_field' => 'app_id',
    'field' => 'app_id'
  );
  $data[$right_table]['table']['join'][$base_table] = array(
    'left_table' => $join_table,
    'left_field' => 'name',
    'field' => 'name'
  );

  return $data;
}

/**
 * Implements hook_views_pre_execute().
 *
 * Ensures that caches are populated.
 *
 * @param $view
 */
function devconnect_developer_apps_views_pre_execute(&$view) {
  if ($view->base_table == 'dc_dev_app') {
    if (_cache_get_object('developer_app')->isEmpty()) {
      devconnect_developer_apps_sync_apps('default');
    }
    if (_cache_get_object('api_product')->isEmpty()) {
      devconnect_developer_apps_sync_products('default');
    }
  }
}
