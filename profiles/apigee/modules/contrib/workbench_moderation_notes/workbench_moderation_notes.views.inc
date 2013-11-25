<?php

/**
 * @file
 *  Content moderation views integration for Workbench Moderation Notes.
 */

/**
 * Implements hook_views_data().
 */
function workbench_moderation_notes_views_data() {
  $data = array();

  $data['workbench_moderation_node_history']['note'] = array(
    'title' => t('Moderation Note'),
    'help' => t('Note about the moderation action'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  return $data;
}
