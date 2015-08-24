<?php

/**
 * Batch constructor to rebuild and repair app attributes.
 *
 * We process the batch one developer at a time to prevent timeouts in any
 * single step of the batch.
 *
 * @return array
 *   The batch definition.
 */
function devconnect_developer_apps_rebuild_apps_batch() {
  $batch = array(
    'title' => t('Rebuilding attributes of all developer apps'),
    'finished' => 'devconnect_developer_apps_rebuild_apps_batch_finish',
    'init_message' => t('Rebuilding app attributes has started...'),
    'progress_message' => t('Processed @current out of @total developers.'),
    'error_message' => t('An error occurred while rebuilding developer app attributes.'),
    'file' => drupal_get_path('module', 'devconnect_developer_apps') . '/devconnect_developer_apps.batch.inc',
  );

  $devel_controller = entity_get_controller('developer');
  $developers = $devel_controller->listEmails();
  $operations = array();
  foreach ($developers as $developer_email) {
    $operations[] = array('devconnect_developer_apps_rebuild_apps_batch_item', array($developer_email));
  }
  $batch['operations'] = $operations;

  return $batch;
}

/**
 * Finished-batch callback for rebuilding/repairing app attributes.
 *
 * @param bool $success
 *   Did batch succeed?
 */
function devconnect_developer_apps_rebuild_apps_batch_finish($success) {
  if ($success) {
    variable_set('devconnect_developer_apps_unescaped', 1);
    menu_rebuild();
  }
  else {
    drupal_set_message('An error occurred while rebuilding developer app attributes.', 'error');
  }
}

/**
 * Individual batch item callback for rebuilding a developer's app attributes.
 *
 * @param string $developer_email
 *   Identifier of the developer being updated.
 * @param array $context
 *   Current context of the batch.
 */
function devconnect_developer_apps_rebuild_apps_batch_item($developer_email, array &$context) {
  $apps = entity_load('developer_app', FALSE, array('mail' => $developer_email));

  $changed = $unchanged = 0;
  foreach ($apps as $app) {
    $needs_save = FALSE;
    foreach ($app->attributes as $key => $val) {
      if (strpos($val, '&') !== FALSE) {
        $app->attributes[$key] = html_entity_decode($val, ENT_QUOTES, 'UTF-8');
        $needs_save = TRUE;
      }
    }
    if ($needs_save) {
      entity_save('developer_app', $app);
      $changed++;
    }
    else {
      $unchanged++;
    }
  }
  $context['message'] = t('!changed apps changed, !unchanged left unchanged for developer @developer', array('!changed' => $changed, '!unchanged' => $unchanged, '@developer' => $developer_email));
}