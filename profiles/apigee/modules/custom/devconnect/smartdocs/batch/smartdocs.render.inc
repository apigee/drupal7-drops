<?php

use Apigee\SmartDocs\Method;
use Apigee\SmartDocs\Model;
use Apigee\SmartDocs\Resource;
use Apigee\SmartDocs\Revision;
use Apigee\SmartDocs\Template;

use Apigee\Exceptions\ResponseException;

/**
 * Batch API Function to import nodes.
 *
 * @param Apigee\SmartDocs\Model $model
 * @param Apigee\SmartDocs\Revision $revision
 * @param array $selected
 * @param array $options
 * @param bool $render_template
 *
 * @return array
 */
function smartdocs_render(Model $model, Revision $revision, $selected, $options, $render_template = TRUE) {
  $display_name = $model->getDisplayName();

  $model_array = $model->toArray();
  $revision_array = $revision->toArray();

  $operations = array();
  if ($render_template) {
    $operations[] = array('smartdocs_template', array($model_array));
  }
  $operations[] = array('smartdocs_taxonomy_vocab', array($model_array));
  $operations[] = array('smartdocs_taxonomy_term', array($model_array, $revision_array, $selected));
  $operations[] = array('smartdocs_content_type', array($model_array));
  $operations[] = array('smartdocs_node_fields', array($model_array, $revision_array));
  $operations[] = array('smartdocs_node_paths', array($model_array, $options));
  $publish = (bool) $options['publish'];
  if (count($revision->getResources()) > 0) {
    foreach ($selected as $select) {
      if ($select == '0') {
        continue;
      }
      /** @var Apigee\SmartDocs\Resource $resource */
      foreach ($revision->getResources() as $resource) {
        $resource_array = $resource->toArray();
        /** @var Apigee\SmartDocs\Method $method */
        foreach ($resource->getMethods() as $method) {
          if ($select == $method->getUuid()) {
            $operations[] = array('smartdocs_render_nodes', array($model_array, $revision_array, $resource_array, $method->toArray(), $publish));
          }
        }
      }
    }
  }
  $operations[] = array('smartdocs_views', array($model_array));
  $operations[] = array('smartdocs_clear_caches_flush', array());
  $operations[] = array('smartdocs_clear_caches_css', array());
  $operations[] = array('smartdocs_clear_caches_js', array());
  $operations[] = array('smartdocs_clear_caches_theme', array());
  $operations[] = array('smartdocs_clear_caches_nodes', array());
  $operations[] = array('smartdocs_clear_caches_menu', array());

  $batch = array(
    'title' => t('Rendering nodes from the %model model', array('%model' => $display_name)),
    'operations' => $operations,
    'finished' => 'smartdocs_render_finished',
    'init_message' => t('Importing has started...'),
    'progress_message' => t('Processed @current out of @total.'),
    'error_message' => t('Importing has encountered an error.'),
    'file' => drupal_get_path('module', 'smartdocs') . '/batch/smartdocs.render.inc',
  );
  return $batch;
}

/**
 * Ensures the model uses the correct template.
 *
 * @param array $model_array
 * @param array $context
 */
function smartdocs_template(array $model_array, array &$context) {
  $model = new Model(devconnect_default_org_config());
  Model::fromArray($model, $model_array);
  if (!in_array($model->getName(), array_keys(node_type_get_names()))) {
    $context['message'] = t('Correct model template verified');
    
    $path = drupal_get_path('module', 'smartdocs') . '/templates/smartdocs.hbr';
    $custom_template_file = variable_get('custom_template_file', NULL);
    
    if(!empty($custom_template_file)){
      $file = file_load($custom_template_file);
      $path = file_create_url($file->uri);
    }
    $html = file_get_contents($path);

    try {
      $template = new Template(devconnect_default_org_config(), $model->getUuid());
      $template->save('drupal-cms', 'method', $html, TRUE);
    } catch (ResponseException $e) {
      $current_set = &_batch_current_set();
      $current_set['success'] = FALSE;
      $context['results'][] = t('Unable to save the model template. The modeling API says: :response', array(':response' => $e->getCode() . ' ' , $e->getMessage()));
      $context['finished'] = 1.0;
    }
  }
}

/**
 * Created SmartDocs Taxonomy Vocab
 *
 * @param array $model_array
 * @param array $context
 */
function smartdocs_taxonomy_vocab(array $model_array, array &$context) {
  $model = new Model(devconnect_default_org_config());
  Model::fromArray($model, $model_array);
  $model_name = $model->getName();
  $display_name = $model->getDisplayName();
  $smart_mid = smartdocs_model_name($model_name);

  $context['message'] = t('Taxonomy vocabularies created');
  $vocabs = array(
    'Tags' => 'tags',
    'Auth Schemes' => 'auth_schemes',
    'Methods' => 'methods',
    'Revisions' => 'revisions',
    'Types' => 'types',
  );
  foreach ($vocabs as $name => $machine) {
    if (empty(taxonomy_vocabulary_machine_name_load($smart_mid . '_' . $machine)->vid)) {
      taxonomy_vocabulary_save((object) array(
        'name' => $display_name . ' ' . $name,
        'machine_name' => $smart_mid . '_' . $machine,
      ));
    }
  }
}

/**
 * Creates SmartDocs Taxonomy Terms
 *
 * @param array $model_array
 * @param array $revision_array
 * @param array $selected
 * @param array $context
 */
function smartdocs_taxonomy_term(array $model_array, array $revision_array, array $selected, array &$context) {
  $model = new Model(devconnect_default_org_config());
  Model::fromArray($model, $model_array);

  $revision = new Revision($model->getConfig(), $model->getUuid());
  Revision::fromArray($revision, $revision_array);

  $model_name = $model->getName();
  $smart_mid = smartdocs_model_name($model_name);

  // important to redirect to correct rev
  $context['results'][] = $revision->getRevisionNumber();
  $context['message'] = t('Taxonomy terms created');
  // Tags for the method vocabulary
  $vid = taxonomy_vocabulary_machine_name_load($smart_mid . '_methods')->vid;
  if ($vid) {
    $methods = array('GET', 'PATCH', 'POST', 'PUT', 'DELETE',);
    $created = array();
    $path = variable_get('pathauto_taxonomy_term_' . $smart_mid . '_methods_pattern', NULL);
    if (is_null($path)) {
      $pattern = $model_name . '/apis/[term:name]';
      variable_set('pathauto_taxonomy_term_' . $smart_mid . '_methods_pattern', $pattern);
    }
    foreach ($methods as $method) {
      if (!in_array($method, $created)) {
        $created[] = $method;
        $term = taxonomy_get_term_by_name($method, $smart_mid . '_methods');
        if (empty($term)) {
          taxonomy_term_save((object) array(
            'name' => $method,
            'vid' => $vid,
          ));
        }
      }
    }
  }
  $path = variable_get('pathauto_taxonomy_term_' . str_replace('_', '-', $smart_mid) . '_revisions_pattern', NULL);
  if (is_null($path)) {
    $pattern = $model_name . '/apis/[term:name]';
    variable_set('pathauto_taxonomy_term_' . $smart_mid . '_revisions_pattern', $pattern);
  }
  // Task to figure out which version to create
  $revision_vid = taxonomy_vocabulary_machine_name_load($smart_mid . '_revisions')->vid;
  $term = taxonomy_get_term_by_name($revision->getRevisionNumber(), $smart_mid . '_revisions');
  if (empty($term)) {
    if ($revision_vid) {
      taxonomy_term_save((object) array(
        'name' => $revision->getRevisionNumber(),
        'vid' => $revision_vid,
      ));
    }
  }
  // Task to figure out which tags to create
  $types_vid = taxonomy_vocabulary_machine_name_load($smart_mid . '_types')->vid;
  $tag_vid = taxonomy_vocabulary_machine_name_load($smart_mid . '_tags')->vid;
  $auth_vid = taxonomy_vocabulary_machine_name_load($smart_mid . '_auth_schemes')->vid;
  $tags = array();
  $types = array();
  $schemes = array();
  /** @var Apigee\SmartDocs\Resource $resource */
  foreach ($revision->getResources() as $resource) {
    /** @var Apigee\SmartDocs\Method $operation */
    foreach ($resource->getMethods() as $operation) {
      if (in_array($operation->getUuid(), $selected)) {
        $path = variable_get('pathauto_taxonomy_term_' . $smart_mid . '_types_pattern', NULL);
        if (is_null($path)) {
          $pattern = $model_name . '/apis/[term:name]';
          variable_set('pathauto_taxonomy_term_' . $smart_mid . '_types_pattern', $pattern);
        }
        // Parameters
        foreach ($operation->getParameters() as $parameter) {
          if (strtolower($parameter['name']) == 'content-type') {
            $types[] = $parameter['defaultValue'];
            $term = taxonomy_get_term_by_name($parameter['defaultValue'], $smart_mid . '_types');
            if (empty($term) && $types_vid) {
              taxonomy_term_save((object) array('name' => $parameter['defaultValue'], 'vid' => $types_vid));
            }
          }
        }
        if (!in_array('n/a', $types)) {
          $types[] = 'n/a';
          $term = taxonomy_get_term_by_name('n/a', $smart_mid . '_types');
          if (empty($term)) {
            if ($types_vid) {
              taxonomy_term_save((object) array(
                'name' => 'n/a',
                'vid' => $types_vid,
              ));
            }
          }
        }
        // Tags (taxonomy)
        $path = variable_get('pathauto_taxonomy_term_' . $smart_mid . '_tags_pattern', NULL);
        if (!isset($path)) {
          $pattern = $model_name . '/api/[term:name]';
          variable_set('pathauto_taxonomy_term_' . $smart_mid . '_tags_pattern', $pattern);
        }
        foreach ($operation->getTags() as $tag) {
          if (!in_array($tag, $tags)) {
            $tags[] = $tag;
            $term = taxonomy_get_term_by_name($tag, $smart_mid . '_tags');
            if (empty($term) && $tag_vid) {
              taxonomy_term_save((object) array('name' => $tag, 'vid' => $tag_vid));
            }
          }
        }
        if (!in_array('n/a', $tags)) {
          $tags[] = 'n/a';
          $term = taxonomy_get_term_by_name('n/a', $smart_mid . '_tags');
          if (empty($term) && $tag_vid) {
            taxonomy_term_save((object) array('name' => 'n/a', 'vid' => $tag_vid));
          }
        }
        // Authschemes
        $path = variable_get('pathauto_taxonomy_term_' . $smart_mid . '_auth_schemes_pattern', NULL);
        if (is_null($path)) {
          $pattern = $model_name . '/apis/[term:name]';
          variable_set('pathauto_taxonomy_term_' . $smart_mid . '_auth_schemes_pattern', $pattern);
        }
        foreach ($operation->getAuthSchemes() as $scheme) {
          if (!in_array($scheme, $schemes)) {
            $schemes[] = $scheme;
            $term = taxonomy_get_term_by_name($scheme, $smart_mid . '_auth_schemes');
            if (empty($term) && $auth_vid) {
              taxonomy_term_save((object) array('name' => $scheme, 'vid' => $auth_vid));
            }
          }
        }
        if (!in_array('n/a', $schemes)) {
          $schemes[] = 'n/a';
          $term = taxonomy_get_term_by_name('n/a', $smart_mid . '_auth_schemes');
          if (empty($term) && $auth_vid) {
            taxonomy_term_save((object) array('name' => 'n/a', 'vid' => $auth_vid));
          }
        }
        // ensure OAUTH2WEBSERVER is created
        if (!in_array('OAUTH2WEBSERVER', $schemes)) {
          $schemes[] = 'OAUTH2WEBSERVER';
          $term = taxonomy_get_term_by_name('OAUTH2WEBSERVER', $smart_mid . '_auth_schemes');
          if (empty($term) && $auth_vid) {
            taxonomy_term_save((object) array('name' => 'OAUTH2WEBSERVER', 'vid' => $auth_vid));
          }
        }
      }
    }
  }
}

/**
 * Batch operation for creating the main content type housing the method nodes.
 *
 * @param array $model_array
 * @param array $context
 */
function smartdocs_content_type(array $model_array, array &$context) {
  $model = new Model(devconnect_default_org_config());
  Model::fromArray($model, $model_array);

  $model_name = $model->getName();
  $display_name = $model->getDisplayName();
  $smart_mid = smartdocs_model_name($model_name);

  $context['message'] = t('Content type created');
  if (!in_array($smart_mid, node_type_get_names())) {
    $t = get_t();
    $type = array(
      'type' => $smart_mid,
      'name' => $t($display_name),
      'base' => 'node_content',
      'description' => '',
      'help' => '',
      'custom' => 1,
      'modified' => 0,
      'locked' => 1,
      'disabled' => 0,
      'has_title' => 1,
      'title_label' => 'Name',
      'module' => 'node',
      'orig_type' => $smart_mid,
    );
    $type = node_type_set_defaults($type);
    node_type_save($type);
    node_add_body_field($type);
  }
}

/**
 * Batch operation for creating all of the necessary fields for the content type.
 *
 * @param array $model_array
 * @param array $revision_array
 * @param array $context
 */
function smartdocs_node_fields(array $model_array, array $revision_array, array &$context) {
  $model = new Model(devconnect_default_org_config());
  Model::fromArray($model, $model_array);

  $revision = new Revision($model->getConfig(), $model->getUuid());
  Revision::fromArray($revision, $revision_array);

  $model_name = $model->getName();
  $smart_mid = smartdocs_model_name($model_name);
  $smart_field = smartdocs_field_name($model_name);

  $context['message'] = t('Necessary fields created');

  $fields = array();
  $instances = array();
  $widget = array(
    'active' => 1,
    'module' => 'options',
    'settings' => array(),
    'type' => 'options_buttons',
    'weight' => 32,
  );
  if (!field_info_field('field_' . $smart_field . '_model_revisions')) {
    $vocab = $smart_mid . '_revisions';
    $cardinality = 1;
    smartdocs_field_info($cardinality, $vocab, $smart_field, 'revisions', $fields);
    smartdocs_field_instance($widget, $smart_mid, $smart_field, 'revisions', $instances);
  }
  if (!field_info_field('field_' . $smart_field . '_model_types')) {
    $vocab = $smart_mid . '_types';
    $cardinality = -1;
    smartdocs_field_info($cardinality, $vocab, $smart_field, 'types', $fields);
    smartdocs_field_instance($widget, $smart_mid, $smart_field, 'types', $instances);
  }
  /** @var Apigee\SmartDocs\Resource $resource */
  foreach ($revision->getResources() as $resource) {
    /** @var Apigee\SmartDocs\Method $operation */
    foreach ($resource->getMethods() as $operation) {
      if (!field_info_field('field_' . $smart_field . '_model_verb')) {
        smartdocs_field_info(1, $smart_mid . '_methods', $smart_field, 'verb', $fields);
        smartdocs_field_instance($widget, $smart_mid, $smart_field, 'verb', $instances);
      }
      if (!field_info_field('field_' . $smart_field . '_model_authschemes')) {
        smartdocs_field_info(-1, $smart_mid . '_auth_schemes', $smart_field, 'authschemes', $fields);
        smartdocs_field_instance($widget, $smart_mid, $smart_field, 'authschemes', $instances);
      }
      if (!field_info_field('field_' . $smart_field . '_model_tags')) {
        smartdocs_field_info(-1, $smart_mid . '_tags', $smart_field, 'tags', $fields);
        smartdocs_field_instance($widget, $smart_mid, $smart_field, 'tags', $instances);
      }
      if (!field_info_field('field_' . $smart_field . '_model_path')) {
        $fields['field_' . $smart_field . '_model_path'] = array(
          'active' => 1,
          'cardinality' => 1,
          'deleted' => 0,
          'entity_types' => array(),
          'field_name' => 'field_' . $smart_field . '_model_path',
          'foreign keys' => array('format' => array('columns' => array('format' => 'format'),'table' => 'filter_format')),
          'indexes' => array('format' => array(0 => 'format')),
          'locked' => 0,
          'module' => 'text',
          'settings' => array('max_length' => 255),
          'translatable' => 0,
          'type' => 'text',
        );
        $instances['node-' . $smart_mid . '-field_' . $smart_field . '_model_path'] = array(
          'bundle' => $smart_mid,
          'default_value' => NULL,
          'deleted' => 0,
          'description' => '',
          'display' => array(
            'default' => array(
              'label' => 'above',
              'module' => 'text',
              'settings' => array(),
              'type' => 'text_default',
              'weight' => 5,
            ),
            'teaser' => array(
              'label' => 'above',
              'settings' => array(),
              'type' => 'hidden',
              'weight' => 0,
            ),
          ),
          'entity_type' => 'node',
          'field_name' => 'field_' . $smart_field . '_model_path',
          'label' => 'Path',
          'required' => 1,
          'settings' => array('text_processing' => 0, 'user_register_form' => FALSE),
          'widget' => array(
            'active' => 1,
            'module' => 'text',
            'settings' => array('size' => 60),
            'type' => 'text_textfield',
            'weight' => 36,
          ),
        );
      }
      if (!field_info_field('field_' . $smart_field . '_model_uuid')) {
        $fields['field_' . $smart_field . '_model_uuid'] = array(
          'active' => 1,
          'cardinality' => 1,
          'deleted' => 0,
          'entity_types' => array(),
          'field_name' => 'field_' . $smart_field . '_model_uuid',
          'foreign keys' => array('format' => array('columns' => array('format' => 'format'), 'table' => 'filter_format')),
          'indexes' => array('format' => array(0 => 'format')),
          'locked' => 0,
          'module' => 'text',
          'settings' => array('max_length' => 255),
          'translatable' => 0,
          'type' => 'text',
        );
        $instances['node-' . $smart_mid . '-field_' . $smart_field . '_model_uuid'] = array(
          'bundle' => $smart_mid,
          'default_value' => NULL,
          'deleted' => 0,
          'description' => '',
          'display' => array(
            'default' => array(
              'label' => 'above',
              'module' => 'text',
              'settings' => array(),
              'type' => 'text_default',
              'weight' => 5,
            ),
            'teaser' => array(
              'label' => 'above',
              'settings' => array(),
              'type' => 'hidden',
              'weight' => 0,
            ),
          ),
          'entity_type' => 'node',
          'field_name' => 'field_' . $smart_field . '_model_uuid',
          'label' => 'UUID',
          'required' => 1,
          'settings' => array('text_processing' => 0, 'user_register_form' => FALSE),
          'widget' => array(
            'active' => 1,
            'module' => 'text',
            'settings' => array('size' => 60),
            'type' => 'text_textfield',
            'weight' => 36,
          ),
        );
      }
    }
  }
  if (!empty($fields)) {
    foreach($fields as $field) {
      field_create_field($field);
    }
  }
  if (!empty($instances)) {
    foreach($instances as $instance) {
      field_create_instance($instance);
    }
  }
}

/**
 * Generates path aliases for a given node.
 *
 * @param array $model_array
 * @param array $options
 * @param array $context
 */
function smartdocs_node_paths(array $model_array, array $options, array &$context) {
  $model = new Model(devconnect_default_org_config());
  Model::fromArray($model, $model_array);

  $model_name = $model->getName();
  $smart_mid = smartdocs_model_name($model_name);
  $smart_field = smartdocs_field_name($model_name);

  $context['message'] = t('Path aliases generated');
  switch($options['path']) {
    //case 'uuid':
    //  $path = variable_get('pathauto_node_' . $smart_mid . '_pattern', NULL);
    //  if (is_null($path)) {
    //    $pattern = $model_name . '/apis/[node:field-' . str_replace('_', '-', $smart_field) . '-model-verb:name]/[node:field-'.str_replace('_', '-', $smart_field).'-model-uuid]';
    //    variable_set('pathauto_node_' . $smart_mid . '_pattern', $pattern);
    //  }
    //  break;
    case 'path':
        $path = variable_get('pathauto_node_' . $smart_mid . '_pattern', NULL);
        if (is_null($path)) {
          $pattern = $model_name . '/apis/[node:field-' . str_replace('_', '-', $smart_field) . '-model-verb:name]/[node:field-'.str_replace('_', '-', $smart_field).'-model-path]';
          variable_set('pathauto_node_' . $smart_mid . '_pattern', $pattern);
        }
        break;
    default:
        $path = variable_get('pathauto_node_' . $smart_mid . '_pattern', NULL);
        if (is_null($path)) {
          $pattern = $model_name . '/apis/[node:field-' . str_replace('_', '-', $smart_field) . '-model-verb:name]/[node:title]';
          variable_set('pathauto_node_' . $smart_mid . '_pattern', $pattern);
        }
  }
}

/**
 * Function that creates field instances for taxonomy terms
 *
 * @param $widget
 * @param $mid
 * @param $smart_mid
 * @param $smart_field
 * @param $title
 * @param $instances
 */
function smartdocs_field_instance($widget, $smart_mid, $smart_field, $title, &$instances) {
  $instances['node-' . $smart_mid . '-field_' . $smart_field . '_model_' . $title] = array(
    'bundle' => $smart_mid,
    'default_value' => NULL,
    'deleted' => 0,
    'description' => '',
    'display' => array(
      'default' => array(
        'label' => 'above',
        'module' => 'taxonomy',
        'settings' => array(),
        'type' => 'taxonomy_term_reference_link',
        'weight' => 3,
      ),
      'teaser' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
    ),
    'entity_type' => 'node',
    'field_name' => 'field_' . $smart_field . '_model_' . $title,
    'label' => $title,
    'required' => 1,
    'settings' => array('user_register_form' => FALSE),
    'widget' => $widget,
  );
}

/**
 * Function that creates the field_info for taxonomy terms.
 *
 * @param $cardinality
 * @param $vocab
 * @param $mid
 * @param $smart_mid
 * @param $smart_field
 * @param $title
 * @param $fields
 */
function smartdocs_field_info($cardinality, $vocab, $smart_field, $title, &$fields) {
  // this is where we want to add a method field to the content type we created
  $fields['field_' . $smart_field . '_model_' . $title] = array(
    'active' => 1,
    'cardinality' => $cardinality,
    'deleted' => 0,
    'entity_types' => array(),
    'field_name' => 'field_' . $smart_field . '_model_' . $title,
    'foreign keys' => array('tid' => array('columns' => array('tid' => 'tid'), 'table' => 'taxonomy_term_data')),
    'indexes' => array('tid' => array(0 => 'tid')),
    'locked' => 0,
    'module' => 'taxonomy',
    'settings' => array('allowed_values' => array(0 => array('vocabulary' => $vocab, 'parent' => 0))),
    'translatable' => 0,
    'type' => 'taxonomy_term_reference',
  );
}

/**
 * Batch operation for creating all of the detected taxonomy terms.
 *
 * @param array $model_array
 * @param array $revision_array
 * @param array $resource_array
 * @param array $method_array
 * @param bool $publish
 * @param array $context
 */
function smartdocs_render_nodes(array $model_array, array $revision_array, array $resource_array, array $method_array, $publish, array &$context) {
  $model = new Model(devconnect_default_org_config());
  Model::fromArray($model, $model_array);

  $revision = new Revision($model->getConfig(), $model->getUuid());
  Revision::fromArray($revision, $revision_array);

  $resource = new Resource($model->getConfig(), $model->getUuid(), $revision->getUuid());
  Resource::fromArray($resource, $resource_array);

  $method = new Method($model->getConfig(), $model->getUuid(), $revision->getUuid(), $resource->getUuid());
  Method::fromArray($method, $method_array);

  $exists = db_select('smartdata', 's')
    ->fields('s', array('nid'))
    ->condition('method', $method->getUuid())
    ->execute()
    ->fetchField();

  if (!$exists) {
    drupal_load('module', 'smartdocs');
    smartdocs_render_node($model, $revision, $resource, $method, $publish, $context);
    $context['message'] = t('Node: ' . $method->getDisplayName() . ' created');
  }
  else {
    $context['message'] = t('Node: ' . $method->getDisplayName() . ' already created');
  }
}

/**
 * Batch operation for creating views.
 *
 * @param array $model_array
 * @param array $context
 * @throws Exception
 */
function smartdocs_views(array $model_array, &$context) {
  $model = new Model(devconnect_default_org_config());
  Model::fromArray($model, $model_array);

  $mid = $model->getName();
  $smart_mid = smartdocs_model_name($mid);
  $smart_field = smartdocs_field_name($mid);
  $display_name = $model->getDisplayName();

  $wrappers = stream_get_wrappers();
  if (in_array('private', $wrappers)) {
    $url_base = 'private://';
  }
  else {
    $url_base = variable_get('file_private_path', NULL);
    if (empty($url_base) || !is_dir($url_base)) {
      $url_base = 'sites/default/files/private';
      if (!is_dir($url_base)) {
        $url_base = 'sites/default/private';
        if (!is_dir($url_base)) {
          $current_set = &_batch_current_set();
          $current_set['success'] = FALSE;
          $context['results'][] = t('Cannot find private filesystem.');
          $context['finished'] = 1.0;
          return;
        }
      }
    }
    $url_base .= '/';
  }
  if (!is_writable($url_base)) {
    $current_set = &_batch_current_set();
    $current_set['success'] = FALSE;
    $context['results'][] = t('Unable to write views to the private filesystem, because it is not writable.');
    $context['finished'] = 1.0;
    return;
  }
  $context['message'] = t('Views created');
  module_load_include('inc', 'smartdocs', 'templates/smartdocs.views');
  if (!views_get_view($smart_mid . '_listing')) {
    $view = smartdocs_view($mid, $smart_mid, $smart_field, $display_name);
    file_save_data($view, $url_base . $smart_mid . '_listing.view', FILE_EXISTS_REPLACE);
  }
  cache_clear_all('*', 'cache_views', TRUE);
  menu_rebuild();
  module_invoke_all('views_invalidate_cache');
}

/**
 * Batch operation for flushing the JS/CSS cache.
 *
 * @param array $context
 */
function smartdocs_clear_caches_flush(array &$context) {
  _drupal_flush_css_js();
  $context['results'][] = 'cache_flush';
  $context['message'] = t('CSS & JS flushed');
}

/**
 * Batch operation for flushing the Registry cache.
 *
 * @param array $context
 */
function smartdocs_rebuild_registry(array &$context) {
  registry_rebuild();
  $context['results'][] = "cache_registry";
  $context['message'] = t('Registry rebuilt');
}

/**
 * Batch operation for flushing the Registry cache.
 *
 * @param array $context
 */
function smartdocs_clear_caches_css(array &$context) {
  drupal_clear_css_cache();
  $context['results'][] = "cache_css";
  $context['message'] = t('CSS caches cleared');
}

/**
 * Batch operation for flushing the JS cache.
 *
 * @param array $context
 */
function smartdocs_clear_caches_js(array &$context) {
  drupal_clear_js_cache();
  $context['results'][] = "cache_js";
  $context['message'] = t('JS caches cleared');
}

/**
 * Batch operation for flushing the Theme cache.
 *
 * @param array $context
 */
function smartdocs_clear_caches_theme(array &$context) {
  system_rebuild_theme_data();
  drupal_theme_rebuild();
  $context['results'][] = "cache_theme";
  $context['message'] = t('Theme caches cleared');
}

/**
 * Batch operation for flushing the Node cache.
 *
 * @param array $context
 */
function smartdocs_clear_caches_nodes(array &$context) {
  node_types_rebuild();
  $context['results'][] = "cache_node";
  $context['message'] = t('Node caches cleared');
}

/**
 * Batch operation for flushing the Menu cache.
 *
 * @param array $context
 */
function smartdocs_clear_caches_menu(array &$context) {
  menu_rebuild();
  $context['results'][] = "cache_menu";
  $context['message'] = t('Menu caches cleared');
}

/**
 * Finish function for when SmartDocs Renders
 *
 * @param $success
 * @param $results
 * @param $operations
 */
function smartdocs_render_finished($success, $results, $operations) {
  if (!$success) {
    drupal_set_message('The render failed, consult the logs for more information.', 'error');
  }
}
