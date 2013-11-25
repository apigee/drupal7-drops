<?php

function devconnect_developer_apps_views_data_alter(&$data) {
  $data['entity_developer_app']['table']['base'] = array(
    'field' => 'appId',
    'title' => t('Developer Apps'),
    'help' => t('List developer_app entities'),
    'query class' => 'DevconnectDeveloperAppsViewsQueryPlugin'
  );
}

function devconnect_developer_apps_views_plugins() {
  return array(
    'query' => array(
      'DevconnectDeveloperAppsViewsQueryPlugin' => array(
        'title' => t('Developer Apps views query plugin'),
        'help' => t('Loads developer_app entities.'),
        'handler' => 'DevconnectDeveloperAppsViewsQueryPlugin'
      )
    )
  );
}

class DevconnectDeveloperAppsViewsQueryPlugin extends views_plugin_query {

  public $conditions = array();

  public function build(&$view) {
    $this->view = $view;
  }

  public function execute(&$view) {
    $start = microtime(TRUE);
    $conditions = array();

    $view->items_per_page = $view->display_handler->default_display->options['pager']['options']['items_per_page'];
    $conditions['items_per_page'] = $view->items_per_page;

    if (isset($_GET['search'])) {
      $conditions['mail'] = $_GET['search'];
    }
    if (isset($_GET['page'])) {
      $conditions['page'] = $_GET['page'];
    }
    if (isset($_GET['sort'])) {
      $conditions['sort'] = $_GET['sort'];
    }

    $view->result = entity_load('developer_app', FALSE, $conditions);
    $view->total_rows = DeveloperAppController::getAppCount();
    $view->execute_time = microtime(TRUE) - $start;
  }

  public function get_result_entities($results, $relationship = NULL, $field = NULL) {
    return array('developer_app', $results);
  }

  public function get_result_wrappers($results, $relationship = NULL, $field = NULL) {
    $wrappers = array();
    foreach ($results as $id => $entity) {
      $wrappers[$id] = entity_metadata_wrapper('developer_app', $entity);
    }

    // Apply the relationship, if necessary.
    $selector_suffix = '';
    if ($field && ($pos = strrpos($field, ':'))) {
      $selector_suffix = substr($field, 0, $pos);
    }
    if ($selector_suffix || ($relationship && !empty($this->view->relationship[$relationship]))) {
      // Use EntityFieldHandlerHelper to compute the correct data selector for
      // the relationship.
      $handler = (object) array(
        'view' => $this->view,
        'relationship' => $relationship,
        'real_field' => '',
      );
      $selector = EntityFieldHandlerHelper::construct_property_selector($handler);
      $selector .= ($selector ? ':' : '') . $selector_suffix;
      return EntityFieldHandlerHelper::extract_property_multiple($wrappers, $selector);
    }

    // If no relationship is given, just return the entities.
    return array('developer_app', $wrappers);
  }

}