<?php

/**
 * Standard Template for Views generation
 *
 * @param $mid
 * @param $smart_mid
 * @param $smart_field
 * @param $display
 * @return string
 */
function smartdocs_view($mid, $smart_mid, $smart_field, $display) {
  $view = <<<EOF
<?php
\$view = new view();
\$view->name = "${smart_mid}_listing";
\$view->description = "${smart_mid} Listing Description";
\$view->tag = "default, smartdocs";
\$view->base_table = "node";
\$view->human_name = "$display Model Overview";
\$view->core = 7;
\$view->api_version = "3.0";
\$view->disabled = FALSE;

/* Display: Master */
\$handler = \$view->new_display("default", "Master", "default");
\$handler->display->display_options = array(
  'title' => "$display Documentation",
  'use_more_always' => FALSE,
  'access' => array('type' => 'perm'),
  'cache' => array('type' => 'none'),
  'query' => array('type' => 'views_query'),
  'exposed_form' => array('type' => 'basic'),
  'pager' => array('type' => 'none', 'options' => array('offset' => '0')),
  'style_plugin' => 'default',
  'style_options' => array(
    'grouping' => array(
      0 => array(
        'field' => 'field_${smart_field}_model_tags',
        'rendered' => 1,
        'rendered_strip' => 0,
      ),
    ),
  ),
  'row_plugin' => 'fields',
  'fields' => array(
    'title' => array(
      'id' => 'title',
      'table' => 'node',
      'field' => 'title',
      'label' => '',
      'exclude' => TRUE,
      'alter' => array('word_boundary' => FALSE, 'ellipsis' => FALSE),
      'element_label_colon' => FALSE,
    ),
    'authschemes' => array(
      'id' => 'authschemes',
      'table' => 'field_data_field_${smart_field}_model_authschemes',
      'field' => 'field_${smart_field}_model_authschemes',
      'label' => '',
      'exclude' => TRUE,
      'alter' => array('alter_text' => TRUE, 'text' => '<span title="This method needs authentication." class="icon_lock"></span>'),
      'element_label_colon' => FALSE,
      'type' => 'taxonomy_term_reference_plain',
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
    'path' => array(
      'id' => 'path',
      'table' => 'field_data_field_${smart_field}_model_path',
      'field' => 'field_${smart_field}_model_path',
      'label' => '',
      'exclude' => TRUE,
      'element_label_colon' => FALSE,
    ),
    'tags' => array(
      'id' => 'tags',
      'table' => 'field_data_field_${smart_field}_model_tags',
      'field' => 'field_${smart_field}_model_tags',
      'label' => '',
      'exclude' => TRUE,
      'alter' => array(
        'alter_text' => TRUE,
        'text' => '<h3 class="method-tag-header">[tags]</h3><div class="clearfix method_details"><div class="method_data title-header">Method</div><div class="method_data description-header">Description</div></div>',
      ),
      'element_label_colon' => FALSE,
      'type' => 'taxonomy_term_reference_plain',
      'group_rows' => FALSE,
      'delta_offset' => 0,
    ),
    'verb' => array(
      'id' => 'verb',
      'table' => 'field_data_field_${smart_field}_model_verb',
      'field' => 'field_${smart_field}_model_verb',
      'label' => '',
      'exclude' => TRUE,
      'element_label_colon' => FALSE,
      'type' => 'taxonomy_term_reference_plain',
    ),
    'method_detail' => array(
      'id' => 'method_detail',
      'table' => 'views',
      'field' => 'nothing',
      'label' => '',
      'alter' => array(
        'text' => '<div class="method_details clearfix">'
          .   '<div class="method_data title">'
          .     '<div class="verb-auth">'
          .       '<p data-role="verb" class="[verb]">[verb]</p>'
          .       '[authschemes]'
          .     '</div>'
          .     '<div class="title-description">'
          .       '[title]'
          .       '<p title="[path]" class="resource_path" data-role="resource_path">[path]</p>'
          .     '</div>'
          .   '</div>'
          .   '<div class="method_data description">[body]</div>'
          . '</div>',
      ),
      'element_label_colon' => FALSE,
    ),
  ),
  'sorts' => array(
    'created' => array(
      'id' => 'created',
      'table' => 'node',
      'field' => 'created',
      'order' => 'DESC',
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
      'value' => array('$smart_mid' => '$smart_mid'),
    ),
  ),
);

/* Display: Page */
\$handler = \$view->new_display("page", "Page", "page");
\$handler->display->display_options["path"] = "$mid/apis";

\$handler = \$view->new_display("block", "Block", "block_1");
\$handler->display->display_options = array(
  'defaults' => array('title' => FALSE, 'pager' => FALSE, 'fields' => FALSE),
  'title' => t('APIs'),
  'pager' => array('type' => 'some', 'options' => array('items_per_page' => 5, 'offset' => 0)),
  'fields' => array(
    'title' => array(
      'id' => 'title',
      'table' => 'node',
      'field' => 'title',
      'label' => '',
      'exclude' => TRUE,
      'alter' => array('word_boundary' => FALSE, 'ellipsis' => FALSE),
      'element_label_colon' => FALSE,
    ),
    'authschemes' => array(
      'id' => 'authschemes',
      'table' => 'field_data_field_${smart_field}_model_authschemes',
      'field' => 'field_${smart_field}_model_authschemes',
      'label' => '',
      'exclude' => TRUE,
      'alter' => array('alter_text' => TRUE, 'text' => '<span title="This method needs authentication." class="icon_lock"></span>'),
      'element_label_colon' => FALSE,
      'type' => 'taxonomy_term_reference_plain',
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
    'path' => array(
      'id' => 'path',
      'table' => 'field_data_field_${smart_field}_model_path',
      'field' => 'field_${smart_field}_model_path',
      'label' => '',
      'exclude' => TRUE,
      'element_label_colon' => FALSE,
    ),
    'verb' => array(
      'id' => 'verb',
      'table' => 'field_data_field_${smart_field}_model_verb',
      'field' => 'field_${smart_field}_model_verb',
      'label' => '',
      'exclude' => TRUE,
      'element_label_colon' => FALSE,
      'type' => 'taxonomy_term_reference_plain',
    ),
    'method_details' => array(
      'id' => 'method_details',
      'table' => 'views',
      'field' => 'nothing',
      'label' => '',
      'alter' => array(
        'text' => '<div class="method_details clearfix">'
          .   '<div class="method_data title">'
          .     '<div class="verb-auth">'
          .       '<p data-role="verb" class="[verb]">[verb]</p>'
          .       '[authschemes]'
          .     '</div>'
          .     '<div class="title-description">'
          .       '[title]'
          .       '<p title="[path]" class="resource_path" data-role="resource_path">[path]</p>'
          .     '</div>'
          .   '</div>'
          .   '<div class="method_data description">[body]</div>'
          . '</div>',
      ),
      'element_label_colon' => FALSE,
    ),
  ),
);
EOF;
  return trim($view);
}
