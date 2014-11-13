<?php

/**
 * @files
 * Views resources for Views Queue module.
 */

/**
 * Implements hook_views_data().
 */
function views_queue_views_data() {
  $data = array();

  // Base table.
  $data['queue']['table']['group'] = t('SystemQueue');
  $data['queue']['table']['base'] = array(
    'field' => 'item_id',
    'title' => t('SystemQueue'),
    'help' => t('The Drupal SystemQueue table.'),
  );

  // Fields.
  $data['queue']['item_id'] = array(
    'title' => t('Item ID'),
    'help' => t('The queue serial item ID.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
    ),
  );
  $data['queue']['name'] = array(
    'title' => t('Name'),
    'help' => t('The queue name.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );
  $data['queue']['data'] = array(
    'title' => t('Data'),
    'help' => t('The stored data for the queue item.'),
    'field' => array(
      'handler' => 'views_handler_field_serialized',
      'click sortable' => FALSE,
    ),
  );
  $data['queue']['expire'] = array(
    'title' => t('Expire'),
    'help' => t('The expiration lease time.'),
    'field' => array(
      'handler' => 'views_handler_field_date',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort_date',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_date',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_date',
    ),
  );
  $data['queue']['created'] = array(
    'title' => t('Creation'),
    'help' => t('The creation time.'),
    'field' => array(
      'handler' => 'views_handler_field_date',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort_date',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_date',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_date',
    ),
  );

  return $data;
}
