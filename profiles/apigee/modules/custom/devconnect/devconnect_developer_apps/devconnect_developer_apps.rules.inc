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
}
