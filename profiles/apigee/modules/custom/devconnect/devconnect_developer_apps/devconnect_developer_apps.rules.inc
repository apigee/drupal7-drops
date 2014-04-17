<?php

/**
 * Implements hook_rules_event_info().
 *
 *
 */
function devconnect_developer_apps_rules_event_info() {
  $items = array();

  $items['devconnect_developer_app_create'] = array(
    'label' => t('Upon creation of a developer app'),
    'group' => 'Devconnect',
    'variables' => array(
      'app' => array('type' => 'developer_app', 'label' => t('created app'))
    )
  );

  $items['devconnect_developer_app_update'] = array(
    'label' => t('Upon update of a developer app'),
    'group' => 'Devconnect',
    'variables' => array(
      'app' => array('type' => 'developer_app', 'label' => t('updated app'))
    )
  );

  $items['devconnect_developer_app_delete'] = array(
    'label' => t('Upon deletion of a developer app'),
    'group' => 'Devconnect',
    'variables' => array(
      'app' => array('type' => 'developer_app', 'label' => t('deleted app'))
    )
  );

  $statuses = array('revoked', 'pending', 'approved');
  foreach ($statuses as $from) {
    foreach ($statuses as $to) {
      if ($to == $from) {
        continue;
      }
      $items['devconnect_developer_app_status_' . $from . '_' . $to] = array(
        'label' => t("Upon change of a developer app’s overall status from $from to $to"),
        'group' => 'Devconnect',
        'variables' => array(
          'app' => array('type' => 'developer_app', 'label' => t('updated app')),
        )
      );
    }
  }

  return $items;
}

/**
 * Implements hook_rules_event_info_alter().
 *
 * Remove entity-based Developer App events because they don't work.
 * The events declared in devconnect_developer_apps_rules_event_info()
 * should be used instead.
 *
 * @param $events
 */
function devconnect_developer_apps_rules_event_info_alter(&$events) {
  foreach ($events as $key => $event) {
    if (array_key_exists('group', $event) && $event['group'] == 'Developer App') {
      unset($events[$key]);
    }
  }
}
