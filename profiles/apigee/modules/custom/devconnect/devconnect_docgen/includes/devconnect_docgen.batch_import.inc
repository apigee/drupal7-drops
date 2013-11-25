<?php

/**
 * @file: DevConnect DocGen Batch Import Include
 * @about: A batch job that imports the modeling APIs methods as nodes.
 *
 * @author: Brian Hasselbeck <bhasselbeck@apigee.com>
 * @version: v1.0
 */

/**
 * Batch API Function to import nodes
 *
 * @description:
 *
 * @param $verbose - this is the "mega" call to the Modeling API
 * @param $selected - this is an array of UUIDs correlating to operations that need to be created into nodes.
 */
function _devconnect_docgen_import_nodes($mid, $verbose, $selected) {

  $mid = strtolower(preg_replace(array(
    '/[^a-zA-Z0-9]+/',
    '/-+/',
    '/^-+/',
    '/-+$/',
  ), array('_', '_', '', ''), $mid)); // uses drupal default machine_name preg_match config
  if (strlen($mid) > 8) {
    $mid = substr($mid, 0, 8); // only take eight characters
  }

  $batch = array(
    'title' => t('Rendering nodes from the %model model', array('%model' => $verbose['displayName'])),
    'operations' => array(
      array('devconnect_docgen_ensure_model_cms_template', array($mid)),
      array('devconnect_docgen_import_create_taxonomy_vocab', array(strtolower($mid), $verbose['displayName'])),
      array('devconnect_docgen_import_create_taxonomy_term', array(strtolower($mid), $verbose, $selected)),
      array('devconnect_docgen_import_create_content_type', array($mid, $verbose['displayName'], $verbose)),
      array('devconnect_docgen_import_create_necessary_fields', array($mid, $verbose)),
      array('devconnect_docgen_import_map_node_fields', array($mid, $verbose, $selected)),
      array('devconnect_docgen_import_create_views', array($mid, $verbose, $selected)),
    ),
    'finished' => '_devconnect_docgen_import_nodes_finished',
    'init_message' => t('Importing has started...'),
    'progress_message' => t('Processed @current out of @total.'),
    'error_message' => t('Importing has encountered an error.'),
    'file' => drupal_get_path('module', 'devconnect_docgen') . '/includes/devconnect_docgen.batch_import.inc',
  );
  return $batch;
}

/**
 * Ensures that this vocabulary uses the correct html/css formatting for display in devconnect
 */
function devconnect_docgen_ensure_model_cms_template($mid, &$context) {
  // First, we check if the nodes have been rendered before.  If they haven't, there won't be a custom content type
  if (!in_array($mid, node_type_get_names())) {
    $context['message'] = t('Ensuring correct model template');
    _devconnect_docgen_render_operation_template($mid);
  }
}
/**
 * Batch operation for creating all of the detected taxonomy vocabs
 */
function devconnect_docgen_import_create_taxonomy_vocab($mid, $display, &$context) {
  $context['message'] = t('Creating taxonomy vocabs...');
  $vocabs = array(
    'Tags' => 'tags',
    'Auth Schemes' => 'auth_schemes',
    'Methods' => 'methods',
    'Revisions' => 'revisions',
    'Types' => 'types',
  );
  foreach ($vocabs as $name => $machine) {
    $vid = taxonomy_vocabulary_machine_name_load($mid . '_' . $machine)->vid;
    if (empty($vid)) {
      taxonomy_vocabulary_save((object) array(
        'name' => $display . ' ' . $name,
        'machine_name' => $mid . '_' . $machine,
      ));
    }
  }
}

/**
 * Batch operation for creating all of the detected taxonomy terms
 */
function devconnect_docgen_import_create_taxonomy_term($mid, $verbose, $selected, &$context) {
  $context['message'] = t('Creating taxonomy terms...');

  // Tags for the method vocabulary
  $vid = taxonomy_vocabulary_machine_name_load($mid . '_methods')->vid;
  if ($vid) {
    $methods = array('GET', 'POST', 'PUT', 'DELETE');
    $created = array();
    $path = variable_get('pathauto_taxonomy_term_' . $mid . '_methods_pattern', NULL);
    if (is_null($path)) {
      $pattern = $mid . '/apis/[term:name]';
      variable_set('pathauto_taxonomy_term_' . $mid . '_methods_pattern', $pattern);
    }
    foreach ($methods as $method) {
      if (!in_array($method, $created)) {
        $created[] = $method;
        $term = taxonomy_get_term_by_name($method, $mid . '_methods');
        if (empty($term)) {
          taxonomy_term_save((object) array(
            'name' => $method,
            'vid' => $vid,
          ));
        }
      }
    }
  }
  $path = variable_get('pathauto_taxonomy_term_' . $mid . '_revisions_pattern', NULL);
  if (is_null($path)) {
    $pattern = $mid . '/apis/[term:name]';
    variable_set('pathauto_taxonomy_term_' . $mid . '_revisions_pattern', $pattern);
  }
  // Task to figure out which version to create
  $revision_vid = taxonomy_vocabulary_machine_name_load($mid . '_revisions')->vid;
  $term = taxonomy_get_term_by_name($verbose['revision'], $mid . '_revisions');
  if (empty($term)) {
    if ($revision_vid) {
      taxonomy_term_save((object) array(
        'name' => $verbose['revision'],
        'vid' => $revision_vid,
      ));
    }
  }
  // Task to figure out which tags to create
  $types_vid = taxonomy_vocabulary_machine_name_load($mid . '_types')->vid;
  $tag_vid = taxonomy_vocabulary_machine_name_load($mid . '_tags')->vid;
  $auth_vid = taxonomy_vocabulary_machine_name_load($mid . '_auth_schemes')->vid;
  $revisions = $verbose;
  $tags = array();
  $types = array();
  $schemes = array();
  if (is_array($revisions['resources'])) {
    foreach ($revisions['resources'] as $resource) {
      if (isset($resource['methods']) && is_array($resource['methods'])) {
        foreach ($resource['methods'] as $operation) {
          if (in_array($operation['id'], $selected)) {
            $path = variable_get('pathauto_taxonomy_term_' . $mid . '_types_pattern', NULL);
            if (is_null($path)) {
              $pattern = $mid . '/apis/[term:name]';
              variable_set('pathauto_taxonomy_term_' . $mid . '_types_pattern', $pattern);
            }
            if (isset($operation['parameters']) && (count($operation['parameters']))) {
              if (isset($operation['parameters']) && (count($operation['parameters']) > 0)) {
                foreach($operation['parameters'] as $parameter) {
                  if (strtolower($parameter['name']) == 'content-type') {
                    $types[] = $parameter['default'];
                    $term = taxonomy_get_term_by_name($parameter['default'], $mid . '_types');
                    if (empty($term)) {
                      if ($types_vid) {
                        taxonomy_term_save((object) array(
                          'name' => $parameter['default'],
                          'vid' => $types_vid,
                        ));
                      }
                    }
                  }
                }
              }
            }
            if (!in_array('n/a', $types)) {
              $types[] = 'n/a';
              $term = taxonomy_get_term_by_name('n/a', $mid . '_types');
              if (empty($term)) {
                if ($types_vid) {
                  taxonomy_term_save((object) array(
                    'name' => 'n/a',
                    'vid' => $types_vid,
                  ));
                }
              }
            }
            $path = variable_get('pathauto_taxonomy_term_' . $mid . '_tags_pattern', NULL);
            if (is_null($path)) {
              $pattern = $mid . '/api/[term:name]';
              variable_set('pathauto_taxonomy_term_' . $mid . '_tags_pattern', $pattern);
            }
            if (is_array($operation['tags']) && !empty($operation['tags'])) {
              foreach($operation['tags'] as $tag) {
                if (!in_array($tag, $tags)) {
                  $tags[] = $tag;
                  $term = taxonomy_get_term_by_name($tag, $mid . '_tags');
                  if (empty($term)) {
                    if ($tag_vid) {
                      taxonomy_term_save((object) array(
                        'name' => $tag,
                        'vid' => $tag_vid,
                      ));
                    }
                  }
                }
              }
            }
            if (!in_array('n/a', $tags)) {
              $tags[] = 'n/a';
              $term = taxonomy_get_term_by_name('n/a', $mid . '_tags');
              if (empty($term)) {
                if ($tag_vid) {
                  taxonomy_term_save((object) array(
                    'name' => 'n/a',
                    'vid' => $tag_vid,
                  ));
                }
              }
            }
            $path = variable_get('pathauto_taxonomy_term_' . $mid . '_auth_schemes_pattern', NULL);
            if (is_null($path)) {
              $pattern = $mid . '/apis/[term:name]';
              variable_set('pathauto_taxonomy_term_' . $mid . '_auth_schemes_pattern', $pattern);
            }
            if (is_array($operation['authSchemes']) && !empty($operation['authSchemes'])) {
              foreach($operation['authSchemes'] as $scheme) {
                if (!in_array($scheme, $schemes)) {
                  $schemes[] = $scheme;
                  $term = taxonomy_get_term_by_name($scheme, $mid . '_auth_schemes');
                  if (empty($term)) {
                    if ($auth_vid) {
                      taxonomy_term_save((object) array(
                        'name' => $scheme,
                        'vid' => $auth_vid,
                      ));
                    }
                  }
                }
              }
            }
            if (!in_array('n/a', $schemes)) {
              $schemes[] = 'n/a';
              $term = taxonomy_get_term_by_name('n/a', $mid . '_auth_schemes');
              if (empty($term)) {
                if ($auth_vid) {
                  taxonomy_term_save((object) array(
                    'name' => 'n/a',
                    'vid' => $auth_vid,
                  ));
                }
              }
            }
          }
        }
      }
    }
  }
}

/**
 * Batch operation for creating the main content type housing the method nodes.
 */
function devconnect_docgen_import_create_content_type($mid, $display, $verbose, &$context) {
  $context['message'] = t('Creating content type...');

  if (!in_array($mid, node_type_get_names())) {
    $t = get_t();
    $type = array(
      'type' => strtolower($mid),
      'name' => $t($display),
      'base' => 'node_content',
      'custom' => 1,
      'modified' => 1,
      'locked' => 0,
      'title_label' => 'Name'
    );
    $type = node_type_set_defaults($type);
    node_type_save($type);
    node_add_body_field($type);
  }

  $models = variable_get('devconnect_docgen_content_types', array());
  $models[$mid] = $display;
  variable_set('devconnect_docgen_content_types', $models);
}

/**
 * Batch operation for creating all of the necessary fields for the content type
 */
function devconnect_docgen_import_create_necessary_fields($mid, $verbose, &$context) {
  $context['message'] = t('Creating necessary fields...');

  $revisions = $verbose;
  $fields = array();
  $instances = array();
  $widget = array(
    'active' => 1,
    'module' => 'options',
    'settings' => array(),
    'type' => 'options_buttons',
    'weight' => 32,
  );
  if (!field_info_field('field_' . strtolower($mid) . '_model_revisions')) {
    $vocab = $mid . '_revisions';
    $cardinality = 1;
    devconnect_docgen_create_field_info($cardinality, strtolower($vocab), strtolower($mid), 'revisions', $fields);
    devconnect_docgen_create_field_instance($widget, strtolower($mid), 'revisions', $instances);
  }
  if (!field_info_field('field_' . strtolower($mid) . '_model_types')) {
    $vocab = $mid . '_types';
    $cardinality = -1;
    devconnect_docgen_create_field_info($cardinality, strtolower($vocab), strtolower($mid), 'types', $fields);
    devconnect_docgen_create_field_instance($widget, strtolower($mid), 'types', $instances);
  }
  if (is_array($revisions['resources'])) {
    foreach ($revisions['resources'] as $resource) {
      if (isset($resource['methods']) && is_array($resource['methods'])) {
        foreach ($resource['methods'] as $operation) {
          foreach($operation as $title => $field) {
            switch(strtolower($title)) {
              case 'verb':
                if (!field_info_field('field_' . strtolower($mid) . '_model_' . strtolower($title))) {
                  $vocab = $mid . '_methods';
                  $cardinality = 1;
                  devconnect_docgen_create_field_info($cardinality, strtolower($vocab), strtolower($mid), strtolower($title), $fields);
                  devconnect_docgen_create_field_instance($widget, strtolower($mid), strtolower($title), $instances);
                }
                break;
              case 'authschemes':
                if (!field_info_field('field_' . strtolower($mid) . '_model_' . strtolower($title))) {
                  $vocab = $mid . '_auth_schemes';
                  $cardinality = -1;
                  devconnect_docgen_create_field_info($cardinality, strtolower($vocab), strtolower($mid), strtolower($title), $fields);
                  devconnect_docgen_create_field_instance($widget, strtolower($mid), strtolower($title), $instances);
                }
                break;
              case 'tags':
                if (!field_info_field('field_' . strtolower($mid) . '_model_' . strtolower($title))) {
                  $vocab = $mid . '_tags';
                  $cardinality = -1;
                  devconnect_docgen_create_field_info($cardinality, strtolower($vocab), strtolower($mid), strtolower($title), $fields);
                  devconnect_docgen_create_field_instance($widget, strtolower($mid), strtolower($title), $instances);
                }
                break;
              default:
                // current version doesn't support additional query and header params as drupal fields
                break;
            }
            if (!field_info_field('field_' . strtolower($mid) . '_model_path')) {
              $fields['field_' . strtolower($mid) . '_model_path'] = array(
                'active' => 1,
                'cardinality' => 1,
                'deleted' => 0,
                'entity_types' => array(),
                'field_name' => 'field_' . strtolower($mid) . '_model_path',
                'foreign keys' => array(
                  'format' => array(
                    'columns' => array(
                      'format' => 'format',
                    ),
                    'table' => 'filter_format',
                  ),
                ),
                'indexes' => array(
                  'format' => array(
                    0 => 'format',
                  ),
                ),
                'locked' => 0,
                'module' => 'text',
                'settings' => array(
                  'max_length' => 255,
                ),
                'translatable' => 0,
                'type' => 'text',
              );
              $instances['node-' . strtolower($mid) . '-field_' . strtolower($mid) . '_model_path'] = array(
                'bundle' => strtolower($mid),
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
                'field_name' => 'field_' . strtolower($mid) . '_model_path',
                'label' => 'Path',
                'required' => 1,
                'settings' => array(
                  'text_processing' => 0,
                  'user_register_form' => FALSE,
                ),
                'widget' => array(
                  'active' => 1,
                  'module' => 'text',
                  'settings' => array(
                    'size' => 60,
                  ),
                  'type' => 'text_textfield',
                  'weight' => 36,
                ),
              );
            }
          }
        }
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
  $path = variable_get('pathauto_node_' . $mid . '_pattern', NULL);
  if (is_null($path)) {
    $pattern = $mid . '/apis/[node:field-' . $mid . '-model-verb:name]/[node:field-'.$mid.'-model-path]';
    variable_set('pathauto_node_' . $mid . '_pattern', $pattern);
  }
}

/**
 * Function that creates field instances for taxonomy terms
 */
function devconnect_docgen_create_field_instance($widget, $mid, $title, &$instances) {
  $instances['node-' . $mid . '-field_' . $mid . '_model_' . $title] = array(
    'bundle' => $mid,
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
    'field_name' => 'field_' . $mid . '_model_' . $title,
    'label' => $title,
    'required' => 1,
    'settings' => array(
      'user_register_form' => FALSE,
    ),
    'widget' => $widget,
  );
}

/**
 * Function that creates the field_info for taxonomy terms
 */
function devconnect_docgen_create_field_info($cardinality, $vocab, $mid, $title, &$fields) {
  // this is where we want to add a method field to the content type we created
  $fields['field_' . $mid . '_model_' . $title] = array(
    'active' => 1,
    'cardinality' => $cardinality,
    'deleted' => 0,
    'entity_types' => array(),
    'field_name' => 'field_' . $mid . '_model_' . $title,
    'foreign keys' => array(
      'tid' => array(
        'columns' => array(
          'tid' => 'tid',
        ),
        'table' => 'taxonomy_term_data',
      ),
    ),
    'indexes' => array(
      'tid' => array(
        0 => 'tid',
      ),
    ),
    'locked' => 0,
    'module' => 'taxonomy',
    'settings' => array(
      'allowed_values' => array(
        0 => array(
          'vocabulary' => $vocab,
          'parent' => 0,
        ),
      ),
    ),
    'translatable' => 0,
    'type' => 'taxonomy_term_reference',
  );
}

/**
 * Batch operation for creating all of the detected taxonomy terms
 */
function devconnect_docgen_import_map_node_fields($mid, $verbose, $selected, &$context) {
  $context['message'] = t('Creating node content...');
  global $user;
  $revisions = $verbose;
  $nodes = array();
  $exists = array_keys(variable_get($mid . '_nodes', array()));
  if (is_array($revisions['resources'])) {
    foreach ($revisions['resources'] as $resource) {
      if (isset($resource['methods']) && is_array($resource['methods'])) {
        foreach ($resource['methods'] as $operation) {
          if (in_array($operation['id'], $selected) && !in_array($operation['id'], $exists)) {
            $values = array(
              'type' => strtolower($mid),
              'uid' => $user->uid,
              'status' => 1,
              'comment' => 1,
              'promote' => 0,
            );
            $entity = entity_create('node', $values);
            $wrapper = entity_metadata_wrapper('node', $entity);

            if (isset($operation['displayName'])) {
              $wrapper->title->set($operation['displayName']);
            } else {
              $wrapper->title->set($operation['name']);
            }
            $wrapper->body->set(array('value' => $operation['description']));

            // sets the revision; should only be one revision per operation
            $field_name = 'field_' . strtolower($mid) . '_model_revisions';
            $terms = taxonomy_get_term_by_name($verbose['revision'], strtolower($mid) . '_revisions');
            foreach ($terms as $term) {
              if ($term->vocabulary_machine_name == strtolower($mid) . '_revisions') {
                $field =& $wrapper->$field_name;
                $field->set(intval($term->tid));
              }
            }

            // Set the type of application ex: application/json
            $types = array();
            if (isset($operation['parameters']) && (count($operation['parameters']) > 0)) {
              foreach($operation['parameters'] as $parameter) {
                if (strtolower($parameter['name']) == 'content-type') {
                  $types[] = $parameter['default'];
                  $terms = taxonomy_get_term_by_name($parameter['default'], $mid . '_types');
                  foreach ($terms as $term) {
                    $types[] = $term->tid;
                  }
                }
              }
            }
            $field_name = 'field_' . strtolower($mid) . '_model_types';
            $i = 0;
            foreach($types as $type) {
              $field =& $wrapper->$field_name;
              $field[$i]->set(intval($type));
              $i++;
            }

            foreach($operation as $title => $field) {
              $tags = array();
              $auths = array();
              if (is_array($operation['tags']) && !empty($operation['tags'])) {
                foreach($operation['tags'] as $tag) {
                  $terms = taxonomy_get_term_by_name($tag, strtolower($mid) . '_tags');
                  foreach ($terms as $term) {
                    if ($term->vocabulary_machine_name == strtolower($mid) . '_tags') {
                      $tags[] = $term->tid;
                    }
                  }
                }
              }
              if (is_array($operation['authSchemes']) && !empty($operation['authSchemes'])) {
                foreach($operation['authSchemes'] as $scheme) {
                  $terms = taxonomy_get_term_by_name($scheme, strtolower($mid) . '_auth_schemes');
                  foreach ($terms as $term) {
                    if ($term->vocabulary_machine_name == strtolower($mid) . '_auth_schemes') {
                      $auths[] = $term->tid;
                    }
                  }
                }
              }
              switch($title) {
                case 'verb':
                  $field_name = 'field_' . strtolower($mid) . '_model_' . strtolower($title);
                  $terms = taxonomy_get_term_by_name($operation['verb'], strtolower($mid) . '_methods');
                  foreach ($terms as $term) {
                    if ($term->vocabulary_machine_name == strtolower($mid) . '_methods') {
                      $field =& $wrapper->$field_name;
                      $field->set(intval($term->tid));
                    }
                  }
                  break;
                case 'authSchemes':
                  $field_name = 'field_' . strtolower($mid) . '_model_' . strtolower($title);
                  $i = 0;
                  foreach($auths as $auth) {
                    $field =& $wrapper->$field_name;
                    $field[$i]->set(intval($auth));
                    $i++;
                  }
                  break;
                case 'tags':
                  $field_name = 'field_' . strtolower($mid) . '_model_' . strtolower($title);
                  $i = 0;
                  foreach($tags as $tag) {
                    $field =& $wrapper->$field_name;
                    $field[$i]->set(intval($tag));
                    $i++;
                  }
                  break;
                default:
                  // currently, we don't support header or query parameters at this time
                  break;
              }
            }
            $field_name = 'field_' . strtolower($mid) . '_model_path';
            $wrapper->$field_name = $resource['path'];
            if ($wrapper->save()) {
              $node = $wrapper->value();
              $nodes[$operation['id']] = $node->nid;
              _devconnect_docgen_sync_nodes($node->nid, $operation['id'], $nodes, $mid, 'create');
              $data = array(
                'nid' => $node->nid,
                'revision' => $verbose['revision'],
                'resource' => $resource['id'],
                'operation' => $operation['id'],
              );
              $data = drupal_json_encode($data);
              $cache_name = $mid . '_' . $node->nid;
              cache_set($cache_name, $data, 'cache_docgen', CACHE_PERMANENT);
            }
            entity_save('node', $entity);
          }
        }
      }
    }
  }
}

/**
 * Batch operation for creating views
 */
function devconnect_docgen_import_create_views($mid, $verbose, $selected, &$context) {
  $context['message'] = t('Creating views...');
  module_load_include('inc', 'devconnect_docgen', 'includes/templates/views/default_view.template');
  if (!views_get_view($mid . '_listing')) {
    file_save_data(_devconnect_docgen_generate_default_view($mid, $verbose['displayName']), 'private://' . $mid . '_listing.view', FILE_EXISTS_REPLACE);
  }
}

/**
 * Finished function for importing nodes from the Modeling API
 */
function devconnect_docgen_import_nodes_finished($success, $results, $operations) {
  if ($success) {
    menu_rebuild();
    drupal_set_message('Batch Finished', 'status');
  } else {
    drupal_set_message('Batch Failed', 'error');
  }
}

