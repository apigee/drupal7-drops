<?php

function smartdocs_views_default_views() {
  $views = array();

  // SmartDocs method view.
  // Contains displays for Frontpage Block, Model index page and TOC block.
  $view = new view();
  $view->name = 'smartdocs_methods';
  $view->description = '';
  $view->tag = 'smartdocs';
  $view->base_table = 'node';
  $view->human_name = 'Smartdocs Method Listing';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE;

  $verbose_display = <<< EOM
<div class="method_details clearfix">
  <div class="method_data title">
    <div class="verb-auth"><p data-role="verb" class="[verb]">[verb]</p>[security]</div>
    <div class="title-description">[title]<p title="[base_url][resource_path]" class="resource_path" data-role="resource_path">[base_url][resource_path]</p></div>
  </div>
  <div class="method_data description">[body]</div>
</div>
EOM;

  $verbose_tags = <<< EOM
<h3 class="method-tag-header">[tags]</h3>
<div class="clearfix method_details">
  <div class="method_data title-header">Method</div>
  <div class="method_data description-header">Description</div>
</div>
EOM;

  $terse_display = <<< EOM
<div class="method_details clearfix">
  <div class="method_data title">
    <div class="verb-auth"><p data-role="verb" class="[verb]">[verb]</p>[security]</div>
    <div class="title-description">[title]<p title="[resource_path]" class="resource_path" data-role="resource_path">[resource_path]</p></div>
  </div>
</div>
EOM;

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');

  $master_fields = array(
    'title' => array(
      'id' => 'title',
      'table' => 'node',
      'field' => 'title',
      'label' => '',
      'exclude' => TRUE,
      'alter' => array('word_boundary' => FALSE, 'ellipsis' => FALSE),
      'element_label_colon' => FALSE,
    ),
    'security' => array(
      'id' => 'security',
      'table' => 'field_data_field_smart_method_security',
      'field' => 'field_smart_method_security',
      'label' => '',
      'exclude' => TRUE,
      'alter' => array(
        'alter_text' => TRUE,
        'text' => '<span title="This method needs authentication." class="icon_lock"></span>',
      ),
      'element_label_colon' => FALSE,
      'delta_limit' => 1,
      'delta_offset' => 0,
    ),
    'body' => array(
      'id' => 'body',
      'table' => 'field_data_body',
      'field' => 'body',
      'label' => '',
      'exclude' => TRUE,
      'element_label_colon' => FALSE,
      'type' => 'text_plain',
    ),
    'base_url' => array(
      'id' => 'base_url',
      'table' => 'field_data_field_smart_method_base_url',
      'field' => 'field_smart_method_base_url',
      'label' => '',
      'exclude' => TRUE,
      'element_label_colon' => FALSE,
      'type' => 'text_plain',
    ),
    'resource_path' => array(
      'id' => 'resource_path',
      'table' => 'field_data_field_smart_method_resource_path',
      'field' => 'field_smart_method_resource_path',
      'label' => '',
      'exclude' => TRUE,
      'element_label_colon' => FALSE,
      'type' => 'text_plain',
    ),
    'tags' => array(
      'id' => 'tags',
      'table' => 'field_data_field_smart_method_tags',
      'field' => 'field_smart_method_tags',
      'label' => '',
      'exclude' => TRUE,
      'alter' => array(
        'alter_text' => TRUE,
        'text' => $verbose_tags,
      ),
      'element_label_colon' => FALSE,
      'type' => 'taxonomy_term_reference_plain',
      'delta_offset' => 0,
    ),
    'verb' => array(
      'id' => 'verb',
      'table' => 'field_data_field_smart_method_verb',
      'field' => 'field_smart_method_verb',
      'label' => '',
      'exclude' => TRUE,
      'element_label_colon' => FALSE,
      'type' => 'taxonomy_term_reference_plain',
    ),
    'display' => array(
      'id' => 'display',
      'table' => 'views',
      'field' => 'nothing',
      'label' => '',
      'alter' => array(
        'text' => $verbose_display,
      ),
      'element_label_colon' => FALSE,
    ),
  );

  $handler->display->display_options = array(
    'use_more_always' => FALSE,
    'access' => array('type' => 'perm'),
    'cache' => array('type' => 'none'),
    'query' => array('type' => 'views_query'),
    'exposed_form' => array('type' => 'basic'),
    'pager' => array('type' => 'none', 'options' => array('offset' => 0)),
    'style_plugin' => 'default',
    'style_options' => array(
      'grouping' => array(
        0 => array(
          'field' => 'tags',
          'rendered' => 1,
          'rendered_strip' => 0,
        ),
      ),
    ),
    'row_plugin' => 'fields',
    'fields' => $master_fields,
    'sorts' => array(
      'created' => array(
        'id' => 'created',
        'table' => 'node',
        'field' => 'created',
        'order' => 'DESC'
      ),
    ),
    'filters' => array(
      'status' => array(
        'id' => 'status',
        'table' => 'node',
        'field' => 'status',
        'value' => 1,
        'group' => 1,
        'expose' => array('operator' => FALSE),
      ),
      'type' => array(
        'id' => 'type',
        'table' => 'node',
        'field' => 'type',
        'value' => array('smart_method' => 'smart_method'),
      ),
    ),
  );

  /* Display: Front-page Block */
  $handler = $view->new_display('block', 'Front-page Block', 'frontpage');

  $frontpage_fields = $master_fields;
  unset($frontpage_fields['body']);
  unset($frontpage_fields['base_url']);
  unset($frontpage_fields['tags']);
  $frontpage_fields['display']['alter']['text'] = $terse_display;

  $handler->display->display_options = array(
    'defaults' => array(
      'title' => FALSE,
      'pager' => FALSE,
      'style_options' => FALSE,
      'row_plugin' => FALSE,
      'row_options' => FALSE,
      'fields' => FALSE,
    ),
    'title' => 'APIs',
    'pager' => array(
      'type' => 'some',
      'options' => array('items_per_page' => 5, 'offset' => 0),
    ),
    'row_plugin' => 'fields',
    'fields' => $frontpage_fields,
  );

  /* Display: Model index page */
  $handler = $view->new_display('block', 'Model index page', 'modelindex');
  $handler->display->display_options = array(
    'defaults' => array('arguments' => FALSE),
    'arguments' => array(
      'model_tid' => array(
        'id' => 'model_tid',
        'table' => 'field_data_field_smart_method_model',
        'field' => 'field_smart_method_model_tid',
        'default_action' => 'not found',
        'default_argument_type' => 'fixed',
        'summary' => array('number_of_records' => 0, 'format' => 'default_summary'),
        'summary_options' => array('items_per_page' => 25),
      ),
    ),
  );

  /* Display: Method TOC */
  $handler = $view->new_display('block', 'Method TOC', 'methodtoc');
  $toc_fields = $master_fields;
  unset($toc_fields['body']);
  unset($toc_fields['base_url']);
  $toc_fields['tags']['alter']['text'] = '<h3 class="method-tag-header">[tags]</h3>';
  $toc_fields['display']['alter']['text'] = $terse_display;

  $handler->display->display_options = array(
    'defaults' => array('fields' => FALSE, 'arguments' => FALSE),
    'fields' => $toc_fields,
    'arguments' => array(
      'model_tid' => array(
        'id' => 'model_tid',
        'table' => 'field_data_field_smart_method_model',
        'field' => 'field_smart_method_model_tid',
        'default_action' => 'not found',
        'default_argument_type' => 'fixed',
        'summary' => array('number_of_records' => 0, 'format' => 'default_summary'),
        'summary_options' => array('items_per_page' => 25),
      ),
    ),
  );

  // Add the above view object to the array to be returned at the end.
  $views[$view->name] = $view;

  // SmartDocs model view.
  // Contains page display to list all the models that have at least one
  // published node.
  $view = new view();
  $view->name = 'smartdocs_models';
  $view->description = '';
  $view->tag = 'smartdocs';
  $view->base_table = 'taxonomy_term_data';
  $view->human_name = 'SmartDocs Models';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options = array(
    'title' => 'API Documentation',
    'use_more_always' => FALSE,
    'group_by' => TRUE,
    'access' => array('type' => 'perm'),
    'cache' => array('type' => 'none'),
    'query' => array('type' => 'views_query'),
    'exposed_form' => array('type' => 'basic'),
    'pager' => array(
      'type' => 'full',
      'options' => array('items_per_page' => 10),
    ),
    'style_plugin' => 'default',
    'style_options' => array('row_class' => 'model_info'),
    'row_plugin' => 'fields',
    'empty' => array(
      'area' => array(
        'id' => 'area',
        'table' => 'views',
        'field' => 'area',
        'empty' => TRUE,
        'content' => '<div class="smartdocs-no-models">We are currently working on putting together our API documentation; we will be publishing them shortly. Please check back again soon. Thanks for your patience!</div>',
        'format' => 'full_html',
      ),
    ),
    'relationships' => array(
      'reverse_field_smart_method_model_node' => array(
        'id' => 'reverse_field_smart_method_model_node',
        'table' => 'taxonomy_term_data',
        'field' => 'reverse_field_smart_method_model_node',
        'required' => TRUE,
      ),
    ),
     'fields' => array(
      'tid' => array(
        'id' => 'tid',
        'table' => 'taxonomy_term_data',
        'field' => 'tid',
        'label' => '',
        'exclude' => TRUE,
        'element_label_colon' => FALSE,
        'separator' => '',
      ),
      'name' => array(
        'id' => 'name',
        'table' => 'field_data_field_model_display_name',
        'field' => 'field_model_display_name',
        'label' => '',
        'alter' => array('make_link' => TRUE, 'path' => 'taxonomy/term/[tid]'),
        'element_type' => 'span',
        'element_class' => 'model_title',
        'element_label_colon' => FALSE,
      ),
      'description' => array(
        'id' => 'description',
        'table' => 'taxonomy_term_data',
        'field' => 'description',
        'label' => '',
        'element_class' => 'model_description',
        'element_label_colon' => FALSE,
      ),
    ),
    'filters' => array(
      'machine_name' => array(
        'id' => 'machine_name',
        'table' => 'taxonomy_vocabulary',
        'field' => 'machine_name',
        'value' => array('smartocs_models' => 'smartdocs_models'),
      ),
      'count_published' => array(
        'id' => 'count_published',
        'table' => 'node',
        'field' => 'status',
        'relationship' => 'reverse_field_smart_method_model_node',
        'group_type' => 'count',
        'operator' => '>=',
        'value' => array('value' => 1),
      ),
      'status' => array(
        'id' => 'status',
        'table' => 'node',
        'field' => 'status',
        'relationship' => 'reverse_field_smart_method_model_node',
        'value' => 1,
      ),
    ),
  );

  /* Display: Page */
  $handler = $view->new_display('page', 'Page', 'page');
  $handler->display->display_options['path'] = 'apis';

  $views[$view->name] = $view;
  return $views;
}
