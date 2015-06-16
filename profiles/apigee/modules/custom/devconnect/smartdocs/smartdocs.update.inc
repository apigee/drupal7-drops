<?php
/**
 * @file
 * Creates SmartDocs-related taxonomy, content-type and fields.
 */

use Apigee\SmartDocs\Model;
use Apigee\SmartDocs\Revision;
use Apigee\SmartDocs\Resource;
use Apigee\SmartDocs\Method;

/**
 * Creates the basic smart_method content type.
 */
function _smartdocs_update_create_content_type() {
  variable_set('comment_anonymous_smart_method', 0);
  variable_set('comment_default_mode_smart_method', 1);
  variable_set('comment_default_per_page_smart_method', 50);
  variable_set('comment_form_location_smart_method', 1);
  variable_set('comment_preview_smart_method', 1);
  variable_set('comment_smart_method', 1);
  variable_set('comment_subject_field_smart_method', 1);
  variable_set('comment_subject_field_smart_method', 1);
  variable_set('context_block_rebuild_needed', TRUE);
  variable_set('menu_options_smart_method', array('main-menu'));
  variable_set('menu_parent_smart_method', 'main-menu:0');
  variable_set('node_options_smart_method', array());
  variable_set('node_preview_smart_method', 1);
  variable_set('node_submitted_smart_method', 0);
  variable_set('webform_node_smart_method', 0);

  if (!in_array('smart_method', node_type_get_names())) {
    $type = new stdClass();
    $type->type = 'smart_method';
    $type->name = 'SmartDocs Method';
    $type->base = 'node_content';
    $type->description = 'Describes all API methods handled by SmartDocs.';
    $type->help = '';
    // This content-type is handled by the node module.
    $type->custom = 1;
    $type->modified = 1;
    $type->locked = 1;
    $type->disabled = 0;
    $type->is_new = 1;
    $type->has_title = 1;
    $type->title_label = 'Name';
    $type->module = 'node';
    $type->orig_type = 'smart_method';

    node_type_save($type);
  }

  if (!field_info_instance('node', 'body', 'smart_method')) {
    $instance = array(
      'field_name' => 'body',
      'entity_type' => 'node',
      'bundle' => 'smart_method',
      'label' => 'Description',
      'widget' => array(
        'weight' => 1,
        'type' => 'text_textarea_with_summary',
        'module' => 'text',
        'active' => 1,
        'settings' => array('rows' => 20, 'summary_rows' => 5),
      ),
      'settings' => array(
        'text_processing' => TRUE,
        'display_summary' => TRUE,
        'user_register_form' => FALSE,
      ),
      'display' => array(
        'default' => array(
          'label' => 'hidden',
          'type' => 'text_default',
          'settings' => array(),
          'module' => 'text',
          'weight' => 1,
        ),
        'teaser' => array(
          'label' => 'hidden',
          'type' => 'text_summary_or_trimmed',
          'setting' => array(),
          'module' => 'text',
          'weight' => 1,
        ),
      ),
      'required' => 0,
      'description' => '',
      'default_value' => NULL,
    );
    field_create_instance($instance);
  }
}

/**
 * Creates the smartdocs_models vocabulary.
 */
function _smartdocs_update_create_model_vocab() {

  if (!taxonomy_vocabulary_machine_name_load('smartdocs_models')) {
    $vocab = new stdClass();
    $vocab->machine_name = 'smartdocs_models';
    $vocab->name = 'SmartDocs models';
    $vocab->description = 'Enumerates all API models';
    $vocab->hierarchy = 1;
    $vocab->module = 'taxonomy';
    $vocab->weight = 0;
    taxonomy_vocabulary_save($vocab);
  }

  if (!field_info_field('field_model_display_name')) {
    // Now create Display name field.
    $field = array(
      'field_name' => 'field_model_display_name',
      'type' => 'text',
      'module' => 'text',
      'active' => 1,
      'locked' => 0,
      'cardinality' => 1,
      'translatable' => 0,
      'deleted' => 0,
      'entity_types' => array(),
      'settings' => array('max_length' => 255),
      'foreign keys' => array(
        'format' => array(
          'table' => 'filter_format',
          'columns' => array('format' => 'format'),
        ),
      ),
      'indexes' => array('format' => array(0 => 'format')),
    );
    field_create_field($field);
  }

  if (!field_info_instance('taxonomy_term', 'field_model_display_name', 'smartdocs_models')) {
    $instance = array(
      'field_name' => 'field_model_display_name',
      'entity_type' => 'taxonomy_term',
      'bundle' => 'smartdocs_models',
      'label' => 'Display name',
      'widget' => array(
        'type' => 'text_textfield',
        'weight' => 31,
        'settings' => array('size' => 60),
        'module' => 'text',
      ),
      'settings' => array(
        'user_register_form' => FALSE,
        'text_processing' => 0,
      ),
      'display' => array(
        'default' => array(
          'label' => 'hidden',
          'type' => 'text_default',
          'module' => 'text',
          'settings' => array(),
          'weight' => 0,
        ),
      ),
      'required' => FALSE,
      'description' => '',
    );
    field_create_instance($instance);
  }
  variable_set('pathauto_taxonomy_term_smartdocs_models_pattern', '[term:name]/apis');
}

/**
 * Creates and populates the smartdocs_verbs vocabulary.
 */
function _smartdocs_update_create_verb_vocab() {

  if (!taxonomy_vocabulary_machine_name_load('smartdocs_verbs')) {
    $vocab = new stdClass();
    $vocab->machine_name = 'smartdocs_verbs';
    $vocab->name = 'SmartDocs verbs';
    $vocab->description = 'Enumerates all possible HTTP verbs for your API';
    $vocab->hierarchy = 0;
    $vocab->module = 'taxonomy';
    $vocab->weight = 0;
    taxonomy_vocabulary_save($vocab);
  }

  $allowed_verbs = array(
    'options',
    'get',
    'head',
    'post',
    'put',
    'delete',
    'trace',
    'connect',
    //'patch',
  );
  foreach ($allowed_verbs as $verb) {
    $verb_upper = strtoupper($verb);
    $terms = taxonomy_get_term_by_name($verb_upper, 'smartdocs_verbs');
    if (empty($terms)) {
      $term = new stdClass();
      $term->vid = $vocab->vid;
      $term->name = $verb_upper;
      $term->description = '';
      $term->format = 'plain_text';
      $term->weight = 0;
      $term->parent = 0;
      taxonomy_term_save($term);
      db_insert('url_alias')
        ->fields(array(
          'source' => 'taxonomy/term/' . $term->tid,
          'alias' => 'apis/verbs/' . $verb,
          'language' => LANGUAGE_NONE,
        ))
        ->execute();
    }
  }
}

/**
 * Creates a vocabulary for tagging SmartDocs nodes.
 */
function _smartdocs_update_create_tags_vocab() {
  if (!taxonomy_vocabulary_machine_name_load('smartdocs_tags')) {
    $vocab = new stdClass();
    $vocab->machine_name = 'smartdocs_tags';
    $vocab->name = 'SmartDocs tags';
    $vocab->description = 'Enables tagging of methods';
    $vocab->hierarchy = 0;
    $vocab->module = 'taxonomy';
    $vocab->weight = 0;
    taxonomy_vocabulary_save($vocab);
  }
}

/**
 * Creates fields and attaches them to smart_method content-type.
 */
function _smartdocs_update_add_fields() {
  // Data for field_config table.
  $fields = array();
  // Data for field_config_instance table.
  $instances = array();

  $weight = 2;

  // Verb (term), cardinality 1.
  $fields[] = array(
    'active' => 1,
    'cardinality' => 1,
    'deleted' => 0,
    'entity_types' => array(),
    'field_name' => 'field_smart_method_verb',
    'foreign keys' => array('tid' => array('columns' => array('tid' => 'tid'), 'table' => 'taxonomy_term_data')),
    'indexes' => array('tid' => array(0 => 'tid')),
    'locked' => 0,
    'module' => 'taxonomy',
    'settings' => array('allowed_values' => array(0 => array('vocabulary' => 'smartdocs_verbs', 'parent' => 0))),
    'translatable' => 0,
    'type' => 'taxonomy_term_reference',
  );
  $instances[] = array(
    'bundle' => 'smart_method',
    'default_value' => NULL,
    'deleted' => 0,
    'description' => '',
    'display' => array(
      'default' => array(
        'label' => 'above',
        'module' => 'taxonomy',
        'settings' => array(),
        'type' => 'taxonomy_term_reference_link',
        'weight' => $weight,
      ),
      'teaser' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => $weight,
      ),
    ),
    'entity_type' => 'node',
    'field_name' => 'field_smart_method_verb',
    'label' => 'HTTP verb',
    'required' => 1,
    'settings' => array('user_register_form' => FALSE),
    'widget' => array(
      'active' => 1,
      'module' => 'options',
      'settings' => array(),
      'type' => 'options_select',
      'weight' => $weight++,
    ),
  );

  // Parameters, cardinality infinite.
  $fields[] = array(
    'field_name' => 'field_smart_method_params',
    'type' => 'smartdocs_parameter',
    'module' => 'smartdocs_parameter',
    'active' => 1,
    'locked' => 0,
    'cardinality' => -1,
    'translatable' => 0,
    'deleted' => 0,
    'entity_types' => array(),
    'settings' => array(),
    'foreign keys' => array(),
    'indexes' => array(),
  );
  $instances[] = array(
    'field_name' => 'field_smart_method_params',
    'entity_type' => 'node',
    'bundle' => 'smart_method',
    'deleted' => 0,
    'label' => 'Parameters',
    'settings' => array('user_register_form' => FALSE),
    'display' => array(
      'default' => array(
        'label' => 'above',
        'type' => 'smartdocs_parameter_formatter',
        'settings' => array(),
        'module' => 'smartdocs_parameter',
        'weight' => $weight,
      ),
      'teaser' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => $weight,
      ),
    ),
    'required' => 0,
    'description' => '',
    'default_value' => NULL,
    'widget' => array(
      'weight' => $weight++,
      'type' => 'smartdocs_parameter_widget',
      'module' => 'smartdocs_parameter',
      'active' => 0,
      'settings' => array(),
    ),

  );

  // Body attachments, cardinality infinite.
  $fields[] = array(
    'field_name' => 'field_smart_method_body_attach',
    'type' => 'smartdocs_attachment',
    'module' => 'smartdocs_attachment',
    'active' => 1,
    'locked' => 0,
    'cardinality' => -1,
    'translatable' => 0,
    'deleted' => 0,
    'entity_types' => array(),
    'settings' => array(),
    'foreign keys' => array(),
    'indexes' => array(),
  );
  $instances[] = array(
    'field_name' => 'field_smart_method_body_attach',
    'entity_type' => 'node',
    'bundle' => 'smart_method',
    'deleted' => 0,
    'label' => 'Body Attachments',
    'settings' => array('user_register_form' => FALSE),
    'display' => array(
      'default' => array(
        'label' => 'above',
        'type' => 'smartdocs_attachment_formatter',
        'settings' => array(),
        'module' => 'smartdocs_attachment',
        'weight' => $weight,
      ),
      'teaser' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => $weight,
      ),
    ),
    'required' => 0,
    'description' => '',
    'default_value' => NULL,
    'widget' => array(
      'weight' => $weight++,
      'type' => 'smartdocs_attachment_widget',
      'module' => 'smartdocs_attachment',
      'active' => 0,
      'settings' => array(),
    ),
  );

  // Body parameters, cardinality infinite.
  $fields[] = array(
    'field_name' => 'field_smart_method_body_params',
    'type' => 'smartdocs_parameter',
    'module' => 'smartdocs_parameter',
    'active' => 1,
    'locked' => 0,
    'cardinality' => -1,
    'translatable' => 0,
    'deleted' => 0,
    'entity_types' => array(),
    'settings' => array(),
    'foreign keys' => array(),
    'indexes' => array(),
  );
  $instances[] = array(
    'field_name' => 'field_smart_method_body_params',
    'entity_type' => 'node',
    'bundle' => 'smart_method',
    'deleted' => 0,
    'label' => 'Body Parameters',
    'settings' => array('user_register_form' => FALSE),
    'display' => array(
      'default' => array(
        'label' => 'above',
        'type' => 'smartdocs_parameter_formatter',
        'settings' => array(),
        'module' => 'smartdocs_parameter',
        'weight' => $weight,
      ),
      'teaser' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => $weight,
      ),
    ),
    'required' => 0,
    'description' => '',
    'default_value' => NULL,
    'widget' => array(
      'weight' => $weight++,
      'type' => 'smartdocs_parameter_widget',
      'module' => 'smartdocs_parameter',
      'active' => 0,
      'settings' => array(),
    ),
  );

  // Body content-type, cardinality 1
  $fields[] = array(
    'field_name' => 'field_smart_method_mime_type',
    'type' => 'text',
    'module' => 'text',
    'active' => 1,
    'locked' => 0,
    'cardinality' => 1,
    'translatable' => 0,
    'deleted' => 0,
    'entity_types' => array(),
    'foreign keys' => array(
      'format' => array(
        'table' => 'filter_format',
        'columns' => array('format'),
      ),
    ),
    'indexes' => array('format' => array('format')),
    'settings' => array('max_length' => 255),
  );
  $instances[] = array(
    'field_name' => 'field_smart_method_mime_type',
    'entity_type' => 'node',
    'bundle' => 'smart_method',
    'deleted' => 0,
    'default_value' => NULL,
    'description' => '',
    'display' => array(
      'default' => array(
        'label' => 'above',
        'module' => 'text',
        'settings' => array(),
        'type' => 'text_default',
        'weight' => $weight,
      ),
    ),
    'label' => 'Body MIME Type',
    'required' => 0,
    'settings' => array('user_register_form' => FALSE, 'text_processing' => FALSE),
    'widget' => array(
      'active' => 1,
      'module' => 'text',
      'settings' => array('size' => 60),
      'type' => 'text_textfield',
      'weight' => $weight++,
    ),
  );

  // Body documentation, cardinality 1
  $fields[] = array(
    'field_name' => 'field_smart_method_body_doc',
    'type' => 'text_long',
    'module' => 'text',
    'active' => 1,
    'locked' => 0,
    'cardinality' => 1,
    'translatable' => 0,
    'deleted' => 0,
    'entity_types' => array(),
    'foreign keys' => array(
      'format' => array(
        'table' => 'filter_format',
        'columns' => array('format'),
      ),
    ),
    'indexes' => array('format' => array('format')),
    'settings' => array(),
  );
  $instances[] = array(
    'field_name' => 'field_smart_method_body_doc',
    'entity_type' => 'node',
    'bundle' => 'smart_method',
    'deleted' => 0,
    'default_value' => NULL,
    'description' => '',
    'display' => array(
      'default' => array(
        'label' => 'above',
        'module' => 'text',
        'settings' => array(),
        'type' => 'text_default',
        'weight' => $weight,
      ),
    ),
    'label' => 'Body documentation',
    'required' => 0,
    'settings' => array('user_register_form' => FALSE, 'text_processing' => FALSE),
    'widget' => array(
      'active' => 1,
      'module' => 'text',
      'settings' => array('rows' => 5),
      'type' => 'text_textarea',
      'weight' => $weight++,
    ),
  );


  // Body sample, cardinality 1
  $fields[] = array(
    'field_name' => 'field_smart_method_sample',
    'type' => 'text_long',
    'module' => 'text',
    'active' => 1,
    'locked' => 0,
    'cardinality' => 1,
    'translatable' => 0,
    'deleted' => 0,
    'entity_types' => array(),
    'foreign keys' => array(
      'format' => array(
        'table' => 'filter_format',
        'columns' => array('format'),
      ),
    ),
    'indexes' => array('format' => array('format')),
    'settings' => array(),
  );
  $instances[] = array(
    'field_name' => 'field_smart_method_sample',
    'entity_type' => 'node',
    'bundle' => 'smart_method',
    'deleted' => 0,
    'default_value' => NULL,
    'description' => '',
    'display' => array(
      'default' => array(
        'label' => 'above',
        'module' => 'text',
        'settings' => array(),
        'type' => 'text_default',
        'weight' => $weight,
      ),
    ),
    'label' => 'Body sample',
    'required' => 0,
    'settings' => array('user_register_form' => FALSE, 'text_processing' => FALSE),
    'widget' => array(
      'active' => 1,
      'module' => 'text',
      'settings' => array('rows' => 5),
      'type' => 'text_textarea',
      'weight' => $weight++,
    ),
  );

  // Response errors, cardinality infinite.
  $fields[] = array(
    'field_name' => 'field_smart_method_errors',
    'type' => 'smartdocs_error',
    'module' => 'smartdocs_error',
    'active' => 1,
    'locked' => 0,
    'cardinality' => -1,
    'translatable' => 0,
    'deleted' => 0,
    'entity_types' => array(),
    'settings' => array(),
    'foreign keys' => array(),
    'indexes' => array(),
  );
  $instances[] = array(
    'field_name' => 'field_smart_method_errors',
    'entity_type' => 'node',
    'bundle' => 'smart_method',
    'deleted' => 0,
    'label' => 'Response Errors',
    'settings' => array('user_register_form' => FALSE),
    'display' => array(
      'default' => array(
        'label' => 'above',
        'type' => 'smartdocs_error_formatter',
        'settings' => array(),
        'module' => 'smartdocs_error',
        'weight' => $weight,
      ),
      'teaser' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => $weight,
      ),
    ),
    'required' => 0,
    'description' => '',
    'default_value' => NULL,
    'widget' => array(
      'weight' => $weight++,
      'type' => 'smartdocs_error_widget',
      'module' => 'smartdocs_parameter',
      'active' => 0,
      'settings' => array(),
    ),
  );

  // Tags (term), cardinality infinite.
  $fields[] = array(
    'active' => 1,
    'cardinality' => -1,
    'deleted' => 0,
    'entity_types' => array(),
    'field_name' => 'field_smart_method_tags',
    'foreign keys' => array('tid' => array('columns' => array('tid' => 'tid'), 'table' => 'taxonomy_term_data')),
    'indexes' => array('tid' => array(0 => 'tid')),
    'locked' => 0,
    'module' => 'taxonomy',
    'settings' => array('allowed_values' => array(0 => array('vocabulary' => 'smartdocs_tags', 'parent' => 0))),
    'translatable' => 0,
    'type' => 'taxonomy_term_reference',
  );
  $instances[] = array(
    'bundle' => 'smart_method',
    'default_value' => NULL,
    'deleted' => 0,
    'description' => '',
    'display' => array(
      'default' => array(
        'label' => 'above',
        'module' => 'taxonomy',
        'settings' => array(),
        'type' => 'taxonomy_term_reference_link',
        'weight' => $weight,
      ),
      'teaser' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => $weight,
      ),
    ),
    'entity_type' => 'node',
    'field_name' => 'field_smart_method_tags',
    'label' => 'Tags',
    'required' => 0,
    'settings' => array('user_register_form' => FALSE),
    'widget' => array(
      'weight' => $weight++,
      'type' => 'taxonomy_autocomplete',
      'module' => 'taxonomy',
      'active' => 0,
      'settings' => array('size' => 60, 'autocomplete_path' => 'taxonomy/autocomplete'),
    ),
  );

  // Model (term), cardinality 1.
  $fields[] = array(
    'active' => 1,
    'cardinality' => 1,
    'deleted' => 0,
    'entity_types' => array(),
    'field_name' => 'field_smart_method_model',
    'foreign keys' => array('tid' => array('columns' => array('tid' => 'tid'), 'table' => 'taxonomy_term_data')),
    'indexes' => array('tid' => array(0 => 'tid')),
    'locked' => 0,
    'module' => 'taxonomy',
    'settings' => array('allowed_values' => array(0 => array('vocabulary' => 'smartdocs_models', 'parent' => 0))),
    'translatable' => 0,
    'type' => 'taxonomy_term_reference',
  );
  $instances[] = array(
    'bundle' => 'smart_method',
    'default_value' => NULL,
    'deleted' => 0,
    'description' => '',
    'display' => array(
      'default' => array(
        'label' => 'above',
        'module' => 'taxonomy',
        'settings' => array(),
        'type' => 'taxonomy_term_reference_link',
        'weight' => $weight,
      ),
      'teaser' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => $weight,
      ),
    ),
    'entity_type' => 'node',
    'field_name' => 'field_smart_method_model',
    'label' => 'Model',
    'required' => 1,
    'settings' => array('user_register_form' => FALSE),
    'widget' => array(
      'active' => 1,
      'module' => 'options',
      'settings' => array(),
      'type' => 'options_select',
      'weight' => $weight++,
    ),
  );

  // Resource-path, cardinality 1. Read-only.
  $fields[] = array(
    'field_name' => 'field_smart_method_base_url',
    'type' => 'text',
    'module' => 'text',
    'active' => 1,
    'locked' => 0,
    'cardinality' => 1,
    'translatable' => 0,
    'deleted' => 0,
    'entity_types' => array(),
    'foreign keys' => array(
      'format' => array(
        'table' => 'filter_format',
        'columns' => array('format'),
      ),
    ),
    'indexes' => array('format' => array(0 => 'format')),
    'settings' => array('max_length' => 255),
  );
  $instances[] = array(
    'field_name' => 'field_smart_method_base_url',
    'entity_type' => 'node',
    'bundle' => 'smart_method',
    'deleted' => 0,
    'default_value' => NULL,
    'description' => '',
    'display' => array(
      'default' => array(
        'label' => 'above',
        'module' => 'text',
        'settings' => array(),
        'type' => 'text_default',
        'weight' => $weight,
      ),
      'teaser' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => $weight,
      ),
    ),
    'label' => 'Resource base URL',
    'required' => 0,
    'settings' => array('user_register_form' => FALSE, 'text_processing' => FALSE),
    'widget' => array(
      'active' => 1,
      'module' => 'text',
      'settings' => array('size' => 60),
      'type' => 'text_textfield',
      'weight' => $weight++,
    ),
  );

  // Resource-path, cardinality 1. Read-only.
  $fields[] = array(
    'field_name' => 'field_smart_method_resource_path',
    'type' => 'text',
    'module' => 'text',
    'active' => 1,
    'locked' => 0,
    'cardinality' => 1,
    'translatable' => 0,
    'deleted' => 0,
    'entity_types' => array(),
    'foreign keys' => array(
      'format' => array(
        'table' => 'filter_format',
        'columns' => array('format'),
      ),
    ),
    'indexes' => array('format' => array(0 => 'format')),
    'settings' => array('max_length' => 255),
  );
  $instances[] = array(
    'field_name' => 'field_smart_method_resource_path',
    'entity_type' => 'node',
    'bundle' => 'smart_method',
    'deleted' => 0,
    'default_value' => NULL,
    'description' => '',
    'display' => array(
      'default' => array(
        'label' => 'above',
        'module' => 'text',
        'settings' => array(),
        'type' => 'text_default',
        'weight' => $weight,
      ),
      'teaser' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => $weight,
      ),
    ),
    'label' => 'Resource path',
    'required' => 0,
    'settings' => array('user_register_form' => FALSE, 'text_processing' => FALSE),
    'widget' => array(
      'active' => 1,
      'module' => 'text',
      'settings' => array('size' => 60),
      'type' => 'text_textfield',
      'weight' => $weight++,
    ),
  );

  // Method security, cardinality infinite.
  $fields[] = array(
    'field_name' => 'field_smart_method_security',
    'type' => 'list_text',
    'module' => 'list',
    'active' => 1,
    'locked' => 0,
    'cardinality' => -1,
    'translatable' => 0,
    'deleted' => 0,
    'entity_types' => array(),
    'foreign keys' => array(),
    'indexes' => array('value' => array(0 => 'value')),
    'settings' => array(
      'allowed_values' => array(),
      'allowed_values_function' => '',
    ),
  );
  $instances[] = array(
    'field_name' => 'field_smart_method_security',
    'entity_type' => 'node',
    'bundle' => 'smart_method',
    'deleted' => 0,
    'default_value' => NULL,
    'description' => '',
    'display' => array(
      'default' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'list_default',
        'weight' => $weight,
        'module' => 'list',
      ),
    ),
    'label' => 'Security schemes',
    'required' => 0,
    'settings' => array('user_register_form' => FALSE),
    'widget' => array(
      'active' => 1,
      'module' => 'options',
      'settings' => array(),
      'type' => 'options_buttons',
      'weight' => $weight++,
    ),
  );

  // Method uuid (read-only), cardinality 1.
  $fields[] = array(
    'field_name' => 'field_smart_method_uuid',
    'type' => 'text',
    'module' => 'text',
    'active' => 1,
    'locked' => 0,
    'cardinality' => 1,
    'translatable' => 0,
    'deleted' => 0,
    'entity_types' => array(),
    'foreign keys' => array(
      'format' => array(
        'table' => 'filter_format',
        'columns' => array('format'),
      ),
    ),
    'indexes' => array('format' => array(0 => 'format')),
    'settings' => array('max_length' => 255),
  );
  $instances[] = array(
    'field_name' => 'field_smart_method_uuid',
    'entity_type' => 'node',
    'bundle' => 'smart_method',
    'deleted' => 0,
    'default_value' => NULL,
    'description' => '',
    'display' => array(
      'default' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => $weight,
      ),
      'teaser' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => $weight,
      ),
    ),
    'label' => 'Method UUID',
    'required' => 0,
    'settings' => array('user_register_form' => FALSE, 'text_processing' => FALSE),
    'widget' => array(
      'active' => 1,
      'module' => 'text',
      'settings' => array('size' => 60),
      'type' => 'text_textfield',
      'weight' => $weight++,
    ),
  );

  // Resource uuid (read-only), cardinality 1.
  $fields[] = array(
    'field_name' => 'field_smart_method_resource_uuid',
    'type' => 'text',
    'module' => 'text',
    'active' => 1,
    'locked' => 0,
    'cardinality' => 1,
    'translatable' => 0,
    'deleted' => 0,
    'entity_types' => array(),
    'foreign keys' => array(
      'format' => array(
        'table' => 'filter_format',
        'columns' => array('format'),
      ),
    ),
    'indexes' => array('format' => array(0 => 'format')),
    'settings' => array('max_length' => 255),
  );
  $instances[] = array(
    'field_name' => 'field_smart_method_resource_uuid',
    'entity_type' => 'node',
    'bundle' => 'smart_method',
    'deleted' => 0,
    'default_value' => NULL,
    'description' => '',
    'display' => array(
      'default' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => $weight,
      ),
      'teaser' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => $weight,
      ),
    ),
    'label' => 'Resource UUID',
    'required' => 0,
    'settings' => array('user_register_form' => FALSE, 'text_processing' => FALSE),
    'widget' => array(
      'active' => 1,
      'module' => 'text',
      'settings' => array('size' => 60),
      'type' => 'text_textfield',
      'weight' => $weight++,
    ),
  );

  // Revision number (read-only), cardinality 1.
  $fields[] = array(
    'field_name' => 'field_smart_method_revision_num',
    'type' => 'text',
    'module' => 'text',
    'active' => 1,
    'locked' => 0,
    'cardinality' => 1,
    'translatable' => 0,
    'deleted' => 0,
    'entity_types' => array(),
    'foreign keys' => array(
      'format' => array(
        'table' => 'filter_format',
        'columns' => array('format'),
      ),
    ),
    'indexes' => array('format' => array(0 => 'format')),
    'settings' => array('max_length' => 5),
  );
  $instances[] = array(
    'field_name' => 'field_smart_method_revision_num',
    'entity_type' => 'node',
    'bundle' => 'smart_method',
    'deleted' => 0,
    'default_value' => NULL,
    'description' => '',
    'display' => array(
      'default' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => $weight,
      ),
      'teaser' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => $weight,
      ),
    ),
    'label' => 'Revision number',
    'required' => 0,
    'settings' => array('user_register_form' => FALSE, 'text_processing' => FALSE),
    'widget' => array(
      'active' => 1,
      'module' => 'text',
      'settings' => array('size' => 5),
      'type' => 'text_textfield',
      'weight' => $weight++,
    ),
  );

  foreach ($fields as $field) {
    if (!field_info_field($field['field_name'])) {
      field_create_field($field);
    }
  }

  foreach ($instances as $instance) {
    if (!field_info_instance('node', $instance['field_name'], 'smart_method')) {
      field_create_instance($instance);
    }
  }
}

/**
 * Calls each of the above methods to create the smart_method content-type.
 */
function _smartdocs_update() {
  _smartdocs_update_create_model_vocab();
  _smartdocs_update_create_verb_vocab();
  _smartdocs_update_create_tags_vocab();
  _smartdocs_update_create_content_type();
  _smartdocs_update_add_fields();
  variable_set('pathauto_node_smart_method_pattern', '[node:field-smart-method-model:name]/apis/[node:field-smart-method-verb:name]/[node:field-smart-method-resource-path]');
}

/**
 * Lists methods/resources/revisions/models for which nodes have been rendered.
 *
 * @return array
 */
function _smartdocs_update_get_existing_nodes() {
  $query = db_select('smartdata', 's');
  $query->innerJoin('node', 'n', 's.nid = n.nid');
  $result = $query->fields('s')
    ->fields('n', array('status', 'type'))
    ->condition('n.type', 'smart_method', '<>')
    ->condition('n.status', 1)
    ->orderBy('model')
    ->orderBy('revision')
    ->orderBy('resource')
    ->orderBy('method')
    ->execute();
  $nodes = array();
  foreach ($result as $row) {
    $nodes[] = $row;
  }

  return $nodes;
}

/**
 * Re-imports a given SmartDocs method into a Smart Method node.
 *
 * @param int $existing_nid
 *   Node ID of the old content-type method node.
 * @param int $status
 *   Is the existing node published? Zero or one.
 * @param Apigee\SmartDocs\Model $model
 *   Model to which the to-be-rendered method belongs.
 * @param Apigee\SmartDocs\Revision $revision
 *   Revision to which the to-be-rendered method belongs.
 * @param Apigee\SmartDocs\Resource $resource
 *   Resource to which the to-be-rendered method belongs.
 * @param Apigee\SmartDocs\Method $method
 *   The method to be rendered into a new node.
 *
 * @return bool|stdClass
 *   Returns the newly-created node, or FALSE on failure.
 */
function _smartdocs_update_process_node($existing_nid, $status, Model $model, Revision $revision, Resource $resource, Method $method) {
  $transaction = db_transaction();
  // Make sure we aren't going to have a conflict in smartdata.
  db_delete('smartdata')->condition('nid', $existing_nid)->execute();
  // Render the method into a new node.
  $context = array();
  $node = smartdocs_render_node($model, $revision, $resource, $method, $status, $context);
  if (!$node || !$node->nid) {
    $transaction->rollback();
    return FALSE;
  }
  // Remove URL alias for newly-created node.
  db_delete('url_alias')->condition('source', 'node/' . $node->nid)->execute();
  // We don't delete the old node quite yet.
  $node->old_nid = $existing_nid;
  return $node;
}

/**
 * Finds all nodes in legacy per-model content-types and re-renders them.
 *
 * If re-rendering all was successful, we then delete views and content-types
 * relating to old version. If it was unsuccessful, we throw an exception.
 *
 * @throws Exception
 */
function _smartdocs_update_process_nodes() {
  /** @var Apigee\SmartDocs\Model $model */
  $model = NULL;
  /** @var Apigee\SmartDocs\Revision $revision */
  $revision = NULL;
  /** @var Apigee\SmartDocs\Resource $resource */
  $resource = NULL;
  /** @var Apigee\SmartDocs\Method $method */
  $method = NULL;

  // Newly-created Smart Method nodes will be put in this array.
  $new_nodes = array();
  // Cache SmartDocs object identifiers so we don't do more API fetches than
  // we really need to.
  $last_model = $last_revision = $last_resource = $last_method = NULL;
  // Names of content-types that become obsolete will be put in this array.
  $old_types = array();

  // Fetch info from smartdata table.
  $node_info_list = _smartdocs_update_get_existing_nodes();

  // Control how logging will happen. (Is update run from update.php or drush?)
  $log_fn = (function_exists('drush_log') ? 'drush_log' : 'drupal_set_message');
  $t = (function_exists('dt') ? 'dt' : 't');
  $success_flag = ($t == 'dt' ? 'success' : 'info');

  $failures = $successes = 0;

  foreach ($node_info_list as $node_info) {
    // The following are used for logging in case of failures.
    $args = array(
      ':nid' => $node_info->nid,
      ':model' => $node_info->model,
      ':rev' => $node_info->revision,
      ':res' => $node_info->resource,
      ':method' => $node_info->method,
    );

    // Only load model if it's different from the last one.
    if ($node_info->model !== $last_model) {
      $model = smartdocs_model_load($node_info->model);
      if (!$model) {
        $log_fn($t('Failed to update node :nid because model :model could not be loaded.', $args), 'error');
        watchdog('smartdocs update', 'Failed to load model :model', $args, WATCHDOG_ERROR);
        $failures++;
        continue;
      }
      $last_model = $model->getName();
    }
    // Only load revision if it's different from the last one.
    if ($node_info->revision !== $last_revision) {
      $revision = smartdocs_revision_load($node_info->revision);
      if (!$revision) {
        $log_fn($t('Failed to update node :nid because revision :rev of model :model could not be loaded.', $args), 'error');
        watchdog('smartdocs update', 'Failed to load revision :rev of model :model', $args, WATCHDOG_ERROR);
        $failures++;
        continue;
      }
      $last_revision = $revision->getRevisionNumber();
    }
    // Only load resource if it's different from the last one.
    if ($node_info->resource !== $last_resource) {
      $resource = smartdocs_resource_load($node_info->resource);
      if (!$resource) {
        $log_fn($t('Failed to update node :nid because resource :res of model :model revision :rev could not be loaded.', $args), 'error');
        watchdog('smartdocs update', 'Failed to load resource :res, revision :rev, model :model', $args, WATCHDOG_ERROR);
        $failures++;
        continue;
      }
      $last_resource = $resource->getUuid();
    }
    // Only load method if it's different from the last one.
    if ($node_info->method !== $last_method) {
      $method = smartdocs_method_load($node_info->method);
      if (!$method) {
        $log_fn($t('Failed to update node :nid because method :method of model :model, revision :rev, resource :res could not be loaded.', $args), 'error');
        watchdog('smartdocs update', 'Failed to load method :method, resource :res, revision :rev, model :model', $args, WATCHDOG_ERROR);
        $failures++;
        continue;
      }
      $last_method = $method->getUuid();
    }

    if (!in_array($node_info->type, $old_types)) {
      $old_types[] = $node_info->type;
    }

    // Create the new node.
    $node = _smartdocs_update_process_node($node_info->nid, $node_info->status, $model, $revision, $resource, $method);
    if (!$node) {
      $log_fn($t('Failed to re-render node :nid.', array(':nid' => $node_info->nid)), 'error');
      watchdog('smartdocs update', 'Failed to re-render node :nid', $args, WATCHDOG_ERROR);
      $failures++;
    }
    else {
      $new_nodes[] = $node;
      $successes++;
    }
  }
  if ($failures) {
    throw new Exception($t(':fail failure(s) were encountered while updating SmartDocs nodes (there were :success successes).', array(':fail' => $failures, ':success' => $successes)));
  }
  if ($successes) {
    // Finish processing nodes.
    $has_comments = db_table_exists('comments');
    foreach ($new_nodes as $node) {
      // Update old node's URL alias entry to apply to the new node instead of
      // the old one.
      db_update('url_alias')
        ->fields(array('source', 'node/' . $node->nid))
        ->condition('source', 'node/' . $node->old_nid)
        ->execute();
      // Migrate comments (if any) from old node to new one.
      if ($has_comments) {
        db_update('comments')
          ->fields(array('nid', $node->nid))
          ->condition('nid', $node->old_nid)
          ->execute();
      }
      // Delete old node.
      node_delete($node->old_nid);
    }
  }
  // Delete old views.
  $wrappers = stream_get_wrappers();
  if (in_array('private', $wrappers)) {
    $files = file_scan_directory('private://', '/_listing\.view$/');
    foreach ($files as $file) {
      file_delete($file, TRUE);
    }
  }

  // Flush views caches.
  cache_clear_all('*', 'cache_views', TRUE);
  menu_rebuild();
  module_invoke_all('views_invalidate_cache');

  // Delete non-unified content-types.
  foreach ($old_types as $type) {
    node_type_delete($type);
  }
  if ($successes) {
    $log_fn($t(':success nodes were successfully updated.', array(':success' => $successes)), $success_flag);
  }
  else {
    $log_fn($t('No nodes were updated.'), $success_flag);
  }
  if (count($old_types)) {
    $log_fn($t(':count content types were successfully migrated to the unified Smart Method type.', array(':count' => count($old_types))), $success_flag);
  }
  else {
    $log_fn($t('No content types were migrated.'), $success_flag);
  }
}
