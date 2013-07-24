<?php
/**
 * @file
 * devconnect_api_views.views_default.inc
 */

/**
 * Implements hook_views_default_views().
 */
function devconnect_api_views_views_default_views() {
  $export = array();

  $view = new view;
  $view->name = 'apimethodoverviews';
  $view->description = '';
  $view->tag = 'default';
  $view->base_table = 'node';
  $view->human_name = 'APIMethodOverviews';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['access']['type'] = 'perm';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['query']['options']['distinct'] = TRUE;
  $handler->display->display_options['query']['options']['query_comment'] = FALSE;
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'none';
  $handler->display->display_options['style_plugin'] = 'default';
  $handler->display->display_options['row_plugin'] = 'node';
  /* Field: Content: Select API */
  $handler->display->display_options['fields']['field_api']['id'] = 'field_api';
  $handler->display->display_options['fields']['field_api']['table'] = 'field_data_field_api';
  $handler->display->display_options['fields']['field_api']['field'] = 'field_api';
  $handler->display->display_options['fields']['field_api']['label'] = '';
  $handler->display->display_options['fields']['field_api']['exclude'] = TRUE;
  $handler->display->display_options['fields']['field_api']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['field_api']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['field_api']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['field_api']['alter']['external'] = 0;
  $handler->display->display_options['fields']['field_api']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['field_api']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['field_api']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['field_api']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['field_api']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['field_api']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['field_api']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['field_api']['alter']['html'] = 0;
  $handler->display->display_options['fields']['field_api']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['field_api']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['field_api']['hide_empty'] = 0;
  $handler->display->display_options['fields']['field_api']['empty_zero'] = 0;
  $handler->display->display_options['fields']['field_api']['hide_alter_empty'] = 1;
  $handler->display->display_options['fields']['field_api']['type'] = 'node_reference_plain';
  $handler->display->display_options['fields']['field_api']['field_api_classes'] = 0;
  /* Field: Content: Select Class */
  $handler->display->display_options['fields']['field_class_name']['id'] = 'field_class_name';
  $handler->display->display_options['fields']['field_class_name']['table'] = 'field_data_field_class_name';
  $handler->display->display_options['fields']['field_class_name']['field'] = 'field_class_name';
  $handler->display->display_options['fields']['field_class_name']['label'] = '';
  $handler->display->display_options['fields']['field_class_name']['exclude'] = TRUE;
  $handler->display->display_options['fields']['field_class_name']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['field_class_name']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['field_class_name']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['field_class_name']['alter']['external'] = 0;
  $handler->display->display_options['fields']['field_class_name']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['field_class_name']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['field_class_name']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['field_class_name']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['field_class_name']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['field_class_name']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['field_class_name']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['field_class_name']['alter']['html'] = 0;
  $handler->display->display_options['fields']['field_class_name']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['field_class_name']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['field_class_name']['hide_empty'] = 0;
  $handler->display->display_options['fields']['field_class_name']['empty_zero'] = 0;
  $handler->display->display_options['fields']['field_class_name']['hide_alter_empty'] = 1;
  $handler->display->display_options['fields']['field_class_name']['type'] = 'node_reference_plain';
  $handler->display->display_options['fields']['field_class_name']['field_api_classes'] = 0;
  /* Field: Content: Nid */
  $handler->display->display_options['fields']['nid']['id'] = 'nid';
  $handler->display->display_options['fields']['nid']['table'] = 'node';
  $handler->display->display_options['fields']['nid']['field'] = 'nid';
  $handler->display->display_options['fields']['nid']['label'] = '';
  $handler->display->display_options['fields']['nid']['exclude'] = TRUE;
  $handler->display->display_options['fields']['nid']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['nid']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['nid']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['nid']['alter']['external'] = 0;
  $handler->display->display_options['fields']['nid']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['nid']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['nid']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['nid']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['nid']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['nid']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['nid']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['nid']['alter']['html'] = 0;
  $handler->display->display_options['fields']['nid']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['nid']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['nid']['hide_empty'] = 0;
  $handler->display->display_options['fields']['nid']['empty_zero'] = 0;
  $handler->display->display_options['fields']['nid']['hide_alter_empty'] = 1;
  $handler->display->display_options['fields']['nid']['link_to_node'] = 0;
  /* Field: Content: Title */
  $handler->display->display_options['fields']['title']['id'] = 'title';
  $handler->display->display_options['fields']['title']['table'] = 'node';
  $handler->display->display_options['fields']['title']['field'] = 'title';
  $handler->display->display_options['fields']['title']['label'] = '';
  $handler->display->display_options['fields']['title']['exclude'] = TRUE;
  $handler->display->display_options['fields']['title']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['title']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['title']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['title']['alter']['external'] = 0;
  $handler->display->display_options['fields']['title']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['title']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['title']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['title']['alter']['word_boundary'] = 0;
  $handler->display->display_options['fields']['title']['alter']['ellipsis'] = 0;
  $handler->display->display_options['fields']['title']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['title']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['title']['alter']['html'] = 0;
  $handler->display->display_options['fields']['title']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['title']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['title']['hide_empty'] = 0;
  $handler->display->display_options['fields']['title']['empty_zero'] = 0;
  $handler->display->display_options['fields']['title']['hide_alter_empty'] = 1;
  $handler->display->display_options['fields']['title']['link_to_node'] = 0;
  /* Field: Content: Short Description */
  $handler->display->display_options['fields']['field_sdesc']['id'] = 'field_sdesc';
  $handler->display->display_options['fields']['field_sdesc']['table'] = 'field_data_field_sdesc';
  $handler->display->display_options['fields']['field_sdesc']['field'] = 'field_sdesc';
  $handler->display->display_options['fields']['field_sdesc']['label'] = '';
  $handler->display->display_options['fields']['field_sdesc']['alter']['alter_text'] = 1;
  $handler->display->display_options['fields']['field_sdesc']['alter']['text'] = '<strong style="color:#000000">[title]</strong>  -  <span>[field_sdesc-value]</span>';
  $handler->display->display_options['fields']['field_sdesc']['alter']['make_link'] = 1;
  $handler->display->display_options['fields']['field_sdesc']['alter']['path'] = '!1/[nid]';
  $handler->display->display_options['fields']['field_sdesc']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['field_sdesc']['alter']['external'] = 0;
  $handler->display->display_options['fields']['field_sdesc']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['field_sdesc']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['field_sdesc']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['field_sdesc']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['field_sdesc']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['field_sdesc']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['field_sdesc']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['field_sdesc']['alter']['html'] = 0;
  $handler->display->display_options['fields']['field_sdesc']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['field_sdesc']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['field_sdesc']['hide_empty'] = 0;
  $handler->display->display_options['fields']['field_sdesc']['empty_zero'] = 0;
  $handler->display->display_options['fields']['field_sdesc']['hide_alter_empty'] = 1;
  $handler->display->display_options['fields']['field_sdesc']['field_api_classes'] = 0;
  /* Sort criterion: Content: Post date */
  $handler->display->display_options['sorts']['created']['id'] = 'created';
  $handler->display->display_options['sorts']['created']['table'] = 'node';
  $handler->display->display_options['sorts']['created']['field'] = 'created';
  $handler->display->display_options['sorts']['created']['order'] = 'DESC';
  /* Contextual filter: Content: Select API (field_api) */
  $handler->display->display_options['arguments']['field_api_nid']['id'] = 'field_api_nid';
  $handler->display->display_options['arguments']['field_api_nid']['table'] = 'field_data_field_api';
  $handler->display->display_options['arguments']['field_api_nid']['field'] = 'field_api_nid';
  $handler->display->display_options['arguments']['field_api_nid']['default_action'] = 'default';
  $handler->display->display_options['arguments']['field_api_nid']['exception']['value'] = '';
  $handler->display->display_options['arguments']['field_api_nid']['default_argument_type'] = 'php';
  $handler->display->display_options['arguments']['field_api_nid']['default_argument_options']['code'] = 'if (arg(0) == \'api-overview\' && is_string(arg(1))) {
   $api_id = arg(1, drupal_get_normal_path(\'api/\'.urldecode(arg(1)))); 
    $res = db_select(\'node\', \'n\')
    							->fields(\'n\',array(\'nid\'))
    							->condition(\'nid\', $api_id, \'=\')
    							->condition(\'type\', \'api\', \'=\')
    							->condition(\'status\', 1, \'=\')
    							->execute()
    							->fetchObject();
   return $res->nid;
}';
  $handler->display->display_options['arguments']['field_api_nid']['default_argument_skip_url'] = 0;
  $handler->display->display_options['arguments']['field_api_nid']['summary']['number_of_records'] = '0';
  $handler->display->display_options['arguments']['field_api_nid']['summary']['format'] = 'default_summary';
  $handler->display->display_options['arguments']['field_api_nid']['summary_options']['items_per_page'] = '25';
  $handler->display->display_options['arguments']['field_api_nid']['break_phrase'] = 0;
  $handler->display->display_options['arguments']['field_api_nid']['not'] = 0;
  /* Filter criterion: Content: Published */
  $handler->display->display_options['filters']['status']['id'] = 'status';
  $handler->display->display_options['filters']['status']['table'] = 'node';
  $handler->display->display_options['filters']['status']['field'] = 'status';
  $handler->display->display_options['filters']['status']['value'] = 1;
  $handler->display->display_options['filters']['status']['group'] = 0;
  $handler->display->display_options['filters']['status']['expose']['operator'] = FALSE;
  /* Filter criterion: Content: Type */
  $handler->display->display_options['filters']['type']['id'] = 'type';
  $handler->display->display_options['filters']['type']['table'] = 'node';
  $handler->display->display_options['filters']['type']['field'] = 'type';
  $handler->display->display_options['filters']['type']['value'] = array(
    'api_method' => 'api_method',
  );

  /* Display: Block */
  $handler = $view->new_display('block', 'Block', 'block');
  $handler->display->display_options['defaults']['style_plugin'] = FALSE;
  $handler->display->display_options['style_plugin'] = 'default';
  $handler->display->display_options['defaults']['style_options'] = FALSE;
  $handler->display->display_options['defaults']['row_plugin'] = FALSE;
  $handler->display->display_options['row_plugin'] = 'fields';
  $handler->display->display_options['defaults']['row_options'] = FALSE;
  $export['apimethodoverviews'] = $view;

  $view = new view;
  $view->name = 'apimethods';
  $view->description = '';
  $view->tag = 'default';
  $view->base_table = 'node';
  $view->human_name = 'APIMethods';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = 'APIMethods';
  $handler->display->display_options['access']['type'] = 'perm';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['query']['options']['distinct'] = TRUE;
  $handler->display->display_options['query']['options']['query_comment'] = FALSE;
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'none';
  $handler->display->display_options['pager']['options']['offset'] = '0';
  $handler->display->display_options['style_plugin'] = 'default';
  $handler->display->display_options['style_options']['grouping'] = 'field_class_name';
  $handler->display->display_options['style_options']['group_rendered'] = 1;
  $handler->display->display_options['row_plugin'] = 'fields';
  /* Field: Content: Nid */
  $handler->display->display_options['fields']['nid']['id'] = 'nid';
  $handler->display->display_options['fields']['nid']['table'] = 'node';
  $handler->display->display_options['fields']['nid']['field'] = 'nid';
  $handler->display->display_options['fields']['nid']['label'] = '';
  $handler->display->display_options['fields']['nid']['exclude'] = TRUE;
  $handler->display->display_options['fields']['nid']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['nid']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['nid']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['nid']['alter']['external'] = 0;
  $handler->display->display_options['fields']['nid']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['nid']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['nid']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['nid']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['nid']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['nid']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['nid']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['nid']['alter']['html'] = 0;
  $handler->display->display_options['fields']['nid']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['nid']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['nid']['hide_empty'] = 0;
  $handler->display->display_options['fields']['nid']['empty_zero'] = 0;
  $handler->display->display_options['fields']['nid']['hide_alter_empty'] = 1;
  $handler->display->display_options['fields']['nid']['link_to_node'] = 0;
  /* Field: Content: Select API */
  $handler->display->display_options['fields']['field_api']['id'] = 'field_api';
  $handler->display->display_options['fields']['field_api']['table'] = 'field_data_field_api';
  $handler->display->display_options['fields']['field_api']['field'] = 'field_api';
  $handler->display->display_options['fields']['field_api']['label'] = '';
  $handler->display->display_options['fields']['field_api']['exclude'] = TRUE;
  $handler->display->display_options['fields']['field_api']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['field_api']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['field_api']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['field_api']['alter']['external'] = 0;
  $handler->display->display_options['fields']['field_api']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['field_api']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['field_api']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['field_api']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['field_api']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['field_api']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['field_api']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['field_api']['alter']['html'] = 0;
  $handler->display->display_options['fields']['field_api']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['field_api']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['field_api']['hide_empty'] = 0;
  $handler->display->display_options['fields']['field_api']['empty_zero'] = 0;
  $handler->display->display_options['fields']['field_api']['hide_alter_empty'] = 1;
  $handler->display->display_options['fields']['field_api']['type'] = 'node_reference_plain';
  $handler->display->display_options['fields']['field_api']['field_api_classes'] = 0;
  /* Field: Content: Select Class */
  $handler->display->display_options['fields']['field_class_name']['id'] = 'field_class_name';
  $handler->display->display_options['fields']['field_class_name']['table'] = 'field_data_field_class_name';
  $handler->display->display_options['fields']['field_class_name']['field'] = 'field_class_name';
  $handler->display->display_options['fields']['field_class_name']['label'] = '';
  $handler->display->display_options['fields']['field_class_name']['exclude'] = TRUE;
  $handler->display->display_options['fields']['field_class_name']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['field_class_name']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['field_class_name']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['field_class_name']['alter']['external'] = 0;
  $handler->display->display_options['fields']['field_class_name']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['field_class_name']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['field_class_name']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['field_class_name']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['field_class_name']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['field_class_name']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['field_class_name']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['field_class_name']['alter']['html'] = 0;
  $handler->display->display_options['fields']['field_class_name']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['field_class_name']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['field_class_name']['hide_empty'] = 0;
  $handler->display->display_options['fields']['field_class_name']['empty_zero'] = 0;
  $handler->display->display_options['fields']['field_class_name']['hide_alter_empty'] = 1;
  $handler->display->display_options['fields']['field_class_name']['type'] = 'node_reference_plain';
  $handler->display->display_options['fields']['field_class_name']['field_api_classes'] = 0;
  /* Field: Content: Title */
  $handler->display->display_options['fields']['title']['id'] = 'title';
  $handler->display->display_options['fields']['title']['table'] = 'node';
  $handler->display->display_options['fields']['title']['field'] = 'title';
  $handler->display->display_options['fields']['title']['label'] = '';
  $handler->display->display_options['fields']['title']['alter']['alter_text'] = 1;
  $handler->display->display_options['fields']['title']['alter']['text'] = '[title]';
  $handler->display->display_options['fields']['title']['alter']['make_link'] = 1;
  $handler->display->display_options['fields']['title']['alter']['path'] = '!1/[nid]';
  $handler->display->display_options['fields']['title']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['title']['alter']['external'] = 0;
  $handler->display->display_options['fields']['title']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['title']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['title']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['title']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['title']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['title']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['title']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['title']['alter']['html'] = 0;
  $handler->display->display_options['fields']['title']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['title']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['title']['hide_empty'] = 0;
  $handler->display->display_options['fields']['title']['empty_zero'] = 0;
  $handler->display->display_options['fields']['title']['hide_alter_empty'] = 1;
  $handler->display->display_options['fields']['title']['link_to_node'] = 0;
  /* Sort criterion: Content: Post date */
  $handler->display->display_options['sorts']['created']['id'] = 'created';
  $handler->display->display_options['sorts']['created']['table'] = 'node';
  $handler->display->display_options['sorts']['created']['field'] = 'created';
  $handler->display->display_options['sorts']['created']['order'] = 'DESC';
  /* Contextual filter: Content: Select API (field_api) */
  $handler->display->display_options['arguments']['field_api_nid']['id'] = 'field_api_nid';
  $handler->display->display_options['arguments']['field_api_nid']['table'] = 'field_data_field_api';
  $handler->display->display_options['arguments']['field_api_nid']['field'] = 'field_api_nid';
  $handler->display->display_options['arguments']['field_api_nid']['default_action'] = 'default';
  $handler->display->display_options['arguments']['field_api_nid']['default_argument_type'] = 'php';
  $handler->display->display_options['arguments']['field_api_nid']['default_argument_options']['code'] = 'if (arg(0) == \'api\' && is_string(arg(1))) { 
		$api_id = arg(1, drupal_get_normal_path(\'api/\'.urldecode(arg(1)))); 
    $res = db_select(\'node\', \'n\')
    							->fields(\'n\',array(\'nid\'))
    							->condition(\'nid\', $api_id, \'=\')
    							->condition(\'type\', \'api\', \'=\')
    							->condition(\'status\', 1, \'=\')
    							->execute()
    							->fetchObject();
   return $res->nid;
} else { 
   $result = db_query("SELECT n.nid FROM {node} n WHERE n.type=:ntype AND n.status=:nstatus ORDER BY n.sticky DESC, n.title ASC, n.changed DESC", array(\':ntype\'=>\'api\', \':nstatus\' => 1));
    	while ($row = $result->fetchObject()) {
			 $method_id = db_query("SELECT DISTINCT(node.nid) AS nid,
		                     node_data_field_api.entity_id As entity_id,
		                     node.changed AS node_changed
		                     FROM node node
		                     LEFT JOIN field_data_field_api node_data_field_api ON node.vid = node_data_field_api.entity_id
		                     WHERE node.type = \'api_method\' AND node_data_field_api.field_api_nid = :nid
		                     GROUP BY nid
		                     ORDER BY node.sticky DESC, node.title ASC, node.changed DESC", array(\':nid\' => $row->nid));
			 $met_id = $method_id->fetchObject();
			 if ($met_id->nid) { $nid = $row->nid; break; } 
		}
		return $nid;
}';
  $handler->display->display_options['arguments']['field_api_nid']['default_argument_skip_url'] = 0;
  $handler->display->display_options['arguments']['field_api_nid']['summary']['number_of_records'] = '0';
  $handler->display->display_options['arguments']['field_api_nid']['summary']['format'] = 'default_summary';
  $handler->display->display_options['arguments']['field_api_nid']['summary_options']['items_per_page'] = '25';
  $handler->display->display_options['arguments']['field_api_nid']['break_phrase'] = 0;
  $handler->display->display_options['arguments']['field_api_nid']['not'] = 0;
  /* Filter criterion: Content: Type */
  $handler->display->display_options['filters']['type']['id'] = 'type';
  $handler->display->display_options['filters']['type']['table'] = 'node';
  $handler->display->display_options['filters']['type']['field'] = 'type';
  $handler->display->display_options['filters']['type']['value'] = array(
    'api_method' => 'api_method',
  );
  /* Filter criterion: Content: Published */
  $handler->display->display_options['filters']['status']['id'] = 'status';
  $handler->display->display_options['filters']['status']['table'] = 'node';
  $handler->display->display_options['filters']['status']['field'] = 'status';
  $handler->display->display_options['filters']['status']['value'] = '1';

  /* Display: Block */
  $handler = $view->new_display('block', 'Block', 'block');
  $handler->display->display_options['defaults']['title'] = FALSE;
  $export['apimethods'] = $view;

  return $export;
}
