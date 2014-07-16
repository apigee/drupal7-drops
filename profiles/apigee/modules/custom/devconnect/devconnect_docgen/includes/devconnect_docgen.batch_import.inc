<?php

/**
 * @file: DevConnect DocGen Batch Import Include
 * @about: A batch job that imports the modeling APIs methods as nodes.
 *
 * @author: Brian Hasselbeck <bhasselbeck@apigee.com>
 * @version: v1.0
 */

/**
 * Batch API Function to import nodes.
 *
 * @param $mid
 * @param $verbose
 * @param $selected
 * @param $options
 * @param $bootstrap_ver
 * @return array
 */
function _devconnect_docgen_import_nodes($mid, $verbose, $selected, $options, $bootstrap_ver) {
  $operations = array();
  if (!in_array(_devconnect_docgen_model_name($mid), node_type_get_names())) {
    switch($bootstrap_ver) {
      case '2':
        $operations[] = array('devconnect_docgen_ensure_model_cms_template', array($mid, '2'));
        variable_set(_devconnect_docgen_model_name($mid) . '_bootstrap_ver', '2');
        variable_set(_devconnect_docgen_model_name($mid) . '_css', SMARTDOCS_CSS_B2);
        variable_set(_devconnect_docgen_model_name($mid) . '_js', SMARTDOCS_JS_B2);
        break;
      case '3':
        $operations[] = array('devconnect_docgen_ensure_model_cms_template', array($mid, '3'));
        variable_set(_devconnect_docgen_model_name($mid) . '_bootstrap_ver', '3');
        variable_set(_devconnect_docgen_model_name($mid) . '_css', SMARTDOCS_CSS_B3);
        variable_set(_devconnect_docgen_model_name($mid) . '_js', SMARTDOCS_JS_B3);
        break;
      default:
        $operations[] = array('devconnect_docgen_ensure_model_cms_template', array($mid, '2'));
        variable_set(_devconnect_docgen_model_name($mid) . '_bootstrap_ver', '2');
        variable_set(_devconnect_docgen_model_name($mid) . '_css', SMARTDOCS_CSS_B2);
        variable_set(_devconnect_docgen_model_name($mid) . '_js', SMARTDOCS_JS_B2);
        break;
    }
  }
  if (isset($verbose['displayName'])) {
    $operations[] = array('devconnect_docgen_import_create_taxonomy_vocab', array($mid, $verbose['displayName']));
  } else if (isset($verbose['name'])) {
    $display = $verbose['name'];
    $operations[] = array('devconnect_docgen_import_create_taxonomy_vocab', array($mid, $display));
  } else {
    $operations[] = array('devconnect_docgen_import_create_taxonomy_vocab', array($mid, $mid));
  }
  $operations[] = array('devconnect_docgen_import_create_taxonomy_term', array($mid, $verbose, $selected));
  if (isset($verbose['displayName'])) {
    $operations[] = array('devconnect_docgen_import_create_content_type', array($mid, $verbose['displayName'], $verbose));
  } else if (isset($verbose['name'])) {
    $display = $verbose['name'];
    $operations[] = array('devconnect_docgen_import_create_content_type', array($mid, $display, $verbose));
  } else {
    $operations[] = array('devconnect_docgen_import_create_content_type', array($mid, $mid, $verbose));
  }
  $operations[] = array('devconnect_docgen_import_create_necessary_fields', array($mid, $verbose));
  $publish = ($options['publish']) ? TRUE : FALSE;
  foreach ($selected as $key => $select) {
    if ($select != '0') {
      if (is_array($verbose['resources'])) {
        foreach ($verbose['resources'] as $resource) {
          if (isset($resource['methods']) && is_array($resource['methods'])) {
            foreach ($resource['methods'] as $operation) {
              if ($select == $operation['id']) {
                $operations[] = array(
                  'devconnect_docgen_import_map_node_fields',
                  array($mid, $verbose, $operation, $resource, $selected, $publish)
                );
              }
            }
          }
        }
      }
    }
  }
  $operations[] = array('devconnect_docgen_import_create_views', array($mid, $verbose, $selected));
  $operations[] = array('devconnect_docgen_clear_caches_flush', array());
  $operations[] = array('devconnect_docgen_clear_caches_css', array());
  $operations[] = array('devconnect_docgen_clear_caches_js', array());
  $operations[] = array('devconnect_docgen_clear_caches_theme', array());
  $operations[] = array('devconnect_docgen_clear_caches_entity', array());
  $operations[] = array('devconnect_docgen_clear_caches_nodes', array());
  $operations[] = array('devconnect_docgen_clear_caches_menu', array());
  $operations[] = array('devconnect_docgen_clear_caches_menu', array());
  $batch = array(
    'title' => t('Rendering nodes from the %model model', array('%model' => $verbose['displayName'])),
    'operations' => $operations,
    'finished' => '_devconnect_docgen_import_nodes_finished',
    'init_message' => t('Importing has started...'),
    'progress_message' => t('Processed @current out of @total.'),
    'error_message' => t('Importing has encountered an error.'),
    'file' => drupal_get_path('module', 'devconnect_docgen') . '/includes/devconnect_docgen.batch_import.inc',
  );
  return $batch;
}

/**
 * Ensures that this vocabulary uses the correct html/css formatting for display in devconnect.
 *
 * @param $mid
 * @param $update
 * @param $context
 */
function devconnect_docgen_ensure_model_cms_template($mid, $bootstrap_ver, &$context) {
  if (!in_array($mid, array_keys(node_type_get_names()))) {
    $context['message'] = t('Ensuring correct model template');
    switch($bootstrap_ver) {
      case '2':
        $def_js = SMARTDOCS_JS_B2;
        $def_css = SMARTDOCS_CSS_B2;
        break;
      case '3':
        $def_js = SMARTDOCS_JS_B3;
        $def_css = SMARTDOCS_CSS_B3;
        break;
      default:
        $def_js = SMARTDOCS_JS_B2;
        $def_css = SMARTDOCS_CSS_B2;
        break;
    }
    switch($bootstrap_ver) {
      case '3':
        $html = file_get_contents(drupal_get_path('module', 'devconnect_docgen') . '/includes/bootstrap.3.html');
        break;
      default:
        $html = entity_get_controller('docgen_template')->loadDefaultTemplate($mid, 'method');;
        break;
    }
    variable_set($mid . '_css', $def_css);
    variable_set($mid . '_js', $def_js);
    entity_get_controller('docgen_template')->updateTemplate($mid, 'method', $html);
  }
}

/**
 * Batch operation for creating all of the detected taxonomy vocabs.
 *
 * @param $mid
 * @param $display
 * @param $context
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
    if (empty(taxonomy_vocabulary_machine_name_load(_devconnect_docgen_model_name($mid) . '_' . $machine)->vid)) {
      taxonomy_vocabulary_save((object) array(
        'name' => $display . ' ' . $name,
        'machine_name' => _devconnect_docgen_model_name($mid) . '_' . $machine,
      ));
    }
  }
}

/**
 * Batch operation for creating all of the detected taxonomy terms.
 *
 * @param $mid
 * @param $verbose
 * @param $selected
 * @param $context
 */
function devconnect_docgen_import_create_taxonomy_term($mid, $verbose, $selected, &$context) {
  // important to redirect to correct rev
  $context['results'] = $verbose;
  $context['results']['mid'] = $mid;
  $context['message'] = t('Creating taxonomy terms...');
  // Tags for the method vocabulary
  $vid = taxonomy_vocabulary_machine_name_load(_devconnect_docgen_model_name($mid) . '_methods')->vid;
  if ($vid) {
    $methods = array('GET', 'POST', 'PUT', 'DELETE');
    $created = array();
    $path = variable_get('pathauto_taxonomy_term_' . _devconnect_docgen_model_name($mid) . '_methods_pattern', NULL);
    if (is_null($path)) {
      $pattern = $mid . '/apis/[term:name]';
      variable_set('pathauto_taxonomy_term_' . _devconnect_docgen_model_name($mid) . '_methods_pattern', $pattern);
    }
    foreach ($methods as $method) {
      if (!in_array($method, $created)) {
        $created[] = $method;
        $term = taxonomy_get_term_by_name($method, _devconnect_docgen_model_name($mid) . '_methods');
        if (empty($term)) {
          taxonomy_term_save((object) array(
            'name' => $method,
            'vid' => $vid,
          ));
        }
      }
    }
  }
  $path = variable_get('pathauto_taxonomy_term_' . str_replace('_', '-', _devconnect_docgen_model_name($mid)) . '_revisions_pattern', NULL);
  if (is_null($path)) {
    $pattern = $mid . '/apis/[term:name]';
    variable_set('pathauto_taxonomy_term_' . _devconnect_docgen_model_name($mid) . '_revisions_pattern', $pattern);
  }
  // Task to figure out which version to create
  $revision_vid = taxonomy_vocabulary_machine_name_load(_devconnect_docgen_model_name($mid) . '_revisions')->vid;
  $term = taxonomy_get_term_by_name($verbose['revisionNumber'], _devconnect_docgen_model_name($mid) . '_revisions');
  if (empty($term)) {
    if ($revision_vid) {
      taxonomy_term_save((object) array(
        'name' => $verbose['revisionNumber'],
        'vid' => $revision_vid,
      ));
    }
  }
  // Task to figure out which tags to create
  $types_vid = taxonomy_vocabulary_machine_name_load(_devconnect_docgen_model_name($mid) . '_types')->vid;
  $tag_vid = taxonomy_vocabulary_machine_name_load(_devconnect_docgen_model_name($mid) . '_tags')->vid;
  $auth_vid = taxonomy_vocabulary_machine_name_load(_devconnect_docgen_model_name($mid) . '_auth_schemes')->vid;
  $tags = array();
  $types = array();
  $schemes = array();
  if (is_array($verbose['resources'])) {
    foreach ($verbose['resources'] as $resource) {
      if (isset($resource['methods']) && is_array($resource['methods'])) {
        foreach ($resource['methods'] as $operation) {
          if (in_array($operation['id'], $selected)) {
            $path = variable_get('pathauto_taxonomy_term_' . _devconnect_docgen_model_name($mid) . '_types_pattern', NULL);
            if (is_null($path)) {
              $pattern = $mid . '/apis/[term:name]';
              variable_set('pathauto_taxonomy_term_' . _devconnect_docgen_model_name($mid) . '_types_pattern', $pattern);
            }
            if (isset($operation['parameters']) && (count($operation['parameters']))) {
              if (isset($operation['parameters']) && (count($operation['parameters']) > 0)) {
                foreach($operation['parameters'] as $parameter) {
                  if (strtolower($parameter['name']) == 'content-type') {
                    $types[] = $parameter['defaultValue'];
                    $term = taxonomy_get_term_by_name($parameter['defaultValue'], _devconnect_docgen_model_name($mid) . '_types');
                    if (empty($term)) {
                      if ($types_vid) {
                        taxonomy_term_save((object) array(
                          'name' => $parameter['defaultValue'],
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
              $term = taxonomy_get_term_by_name('n/a', _devconnect_docgen_model_name($mid) . '_types');
              if (empty($term)) {
                if ($types_vid) {
                  taxonomy_term_save((object) array(
                    'name' => 'n/a',
                    'vid' => $types_vid,
                  ));
                }
              }
            }
            $path = variable_get('pathauto_taxonomy_term_' . _devconnect_docgen_model_name($mid) . '_tags_pattern', NULL);
            if (is_null($path)) {
              $pattern = $mid . '/api/[term:name]';
              variable_set('pathauto_taxonomy_term_' . _devconnect_docgen_model_name($mid) . '_tags_pattern', $pattern);
            }
            if (is_array($operation['tags']) && !empty($operation['tags'])) {
              foreach($operation['tags'] as $tag) {
                if (!in_array($tag, $tags)) {
                  $tags[] = $tag;
                  $term = taxonomy_get_term_by_name($tag, _devconnect_docgen_model_name($mid) . '_tags');
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
              $term = taxonomy_get_term_by_name('n/a', _devconnect_docgen_model_name($mid) . '_tags');
              if (empty($term)) {
                if ($tag_vid) {
                  taxonomy_term_save((object) array(
                    'name' => 'n/a',
                    'vid' => $tag_vid,
                  ));
                }
              }
            }
            $path = variable_get('pathauto_taxonomy_term_' . _devconnect_docgen_model_name($mid) . '_auth_schemes_pattern', NULL);
            if (is_null($path)) {
              $pattern = $mid . '/apis/[term:name]';
              variable_set('pathauto_taxonomy_term_' . _devconnect_docgen_model_name($mid) . '_auth_schemes_pattern', $pattern);
            }
            if (is_array($operation['authSchemes']) && !empty($operation['authSchemes'])) {
              foreach($operation['authSchemes'] as $scheme) {
                if (!in_array($scheme, $schemes)) {
                  $schemes[] = $scheme;
                  $term = taxonomy_get_term_by_name($scheme, _devconnect_docgen_model_name($mid) . '_auth_schemes');
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
              $term = taxonomy_get_term_by_name('n/a', _devconnect_docgen_model_name($mid) . '_auth_schemes');
              if (empty($term)) {
                if ($auth_vid) {
                  taxonomy_term_save((object) array(
                    'name' => 'n/a',
                    'vid' => $auth_vid,
                  ));
                }
              }
            }
            // ensure OAUTH2WEBSERVER is created
            if (!in_array('OAUTH2WEBSERVER', $schemes)) {
              $schemes[] = 'n/a';
              $term = taxonomy_get_term_by_name('OAUTH2WEBSERVER', _devconnect_docgen_model_name($mid) . '_auth_schemes');
              if (empty($term)) {
                if ($auth_vid) {
                  taxonomy_term_save((object) array(
                    'name' => 'OAUTH2WEBSERVER',
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
 *
 * @param $mid
 * @param $display
 * @param $verbose
 * @param $context
 */
function devconnect_docgen_import_create_content_type($mid, $display, $verbose, &$context) {
  $context['message'] = t('Creating content type...');

  if (!in_array(_devconnect_docgen_model_name($mid), node_type_get_names())) {
    $t = get_t();
    $type = array(
      'type' => _devconnect_docgen_model_name($mid),
      'name' => $t($display),
      'base' => 'node_content',
      'custom' => 0,
      'modified' => 0,
      'locked' => 1,
      'title_label' => 'Name'
    );
    $type = node_type_set_defaults($type);
    node_type_save($type);
    node_add_body_field($type);
  }

  $models = variable_get('devconnect_docgen_content_types', array());
  $models[_devconnect_docgen_model_name($mid)] = $display;
  variable_set('devconnect_docgen_content_types', $models);
  variable_set(_devconnect_docgen_model_name($mid), $mid); // important step
}

/**
 * Batch operation for creating all of the necessary fields for the content type.
 *
 * @param $mid
 * @param $verbose
 * @param $context
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
  if (!field_info_field('field_' . _devconnect_docgen_model_field_name($mid) . '_model_revisions')) {
    $vocab = _devconnect_docgen_model_name($mid) . '_revisions';
    $cardinality = 1;
    devconnect_docgen_create_field_info($cardinality, $vocab, $mid, 'revisions', $fields);
    devconnect_docgen_create_field_instance($widget, $mid, 'revisions', $instances);
  }
  if (!field_info_field('field_' . _devconnect_docgen_model_field_name($mid) . '_model_types')) {
    $vocab = _devconnect_docgen_model_name($mid) . '_types';
    $cardinality = -1;
    devconnect_docgen_create_field_info($cardinality, $vocab, $mid, 'types', $fields);
    devconnect_docgen_create_field_instance($widget, $mid, 'types', $instances);
  }
  if (is_array($revisions['resources'])) {
    foreach ($revisions['resources'] as $resource) {
      if (isset($resource['methods']) && is_array($resource['methods'])) {
        foreach ($resource['methods'] as $operation) {
          foreach($operation as $title => $field) {
            switch(strtolower($title)) {
              case 'verb':
                if (!field_info_field('field_' . _devconnect_docgen_model_field_name($mid) . '_model_' . strtolower($title))) {
                  $vocab = _devconnect_docgen_model_name($mid) . '_methods';
                  $cardinality = 1;
                  devconnect_docgen_create_field_info($cardinality, $vocab, $mid, strtolower($title), $fields);
                  devconnect_docgen_create_field_instance($widget, $mid, strtolower($title), $instances);
                }
                break;
              case 'authschemes':
                if (!field_info_field('field_' . _devconnect_docgen_model_field_name($mid) . '_model_' . strtolower($title))) {
                  $vocab = _devconnect_docgen_model_name($mid) . '_auth_schemes';
                  $cardinality = -1;
                  devconnect_docgen_create_field_info($cardinality, $vocab, $mid, strtolower($title), $fields);
                  devconnect_docgen_create_field_instance($widget, $mid, strtolower($title), $instances);
                }
                break;
              case 'tags':
                if (!field_info_field('field_' . _devconnect_docgen_model_field_name($mid) . '_model_' . strtolower($title))) {
                  $vocab = _devconnect_docgen_model_name($mid) . '_tags';
                  $cardinality = -1;
                  devconnect_docgen_create_field_info($cardinality, $vocab, $mid, strtolower($title), $fields);
                  devconnect_docgen_create_field_instance($widget, $mid, strtolower($title), $instances);
                }
                break;
              default:
                // current version doesn't support additional query and header params as drupal fields
                break;
            }
            if (!field_info_field('field_' . _devconnect_docgen_model_field_name($mid) . '_model_path')) {
              $fields['field_' . _devconnect_docgen_model_field_name($mid) . '_model_path'] = array(
                'active' => 1,
                'cardinality' => 1,
                'deleted' => 0,
                'entity_types' => array(),
                'field_name' => 'field_' . _devconnect_docgen_model_field_name($mid) . '_model_path',
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
              $instances['node-' . _devconnect_docgen_model_name($mid) . '-field_' . _devconnect_docgen_model_field_name($mid) . '_model_path'] = array(
                'bundle' => _devconnect_docgen_model_name($mid),
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
                'field_name' => 'field_' . _devconnect_docgen_model_field_name($mid) . '_model_path',
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
  $path = variable_get('pathauto_node_' . _devconnect_docgen_model_name($mid) . '_pattern', NULL);
  if (is_null($path)) {
    $pattern = $mid . '/apis/[node:field-' . str_replace('_', '-', _devconnect_docgen_model_field_name($mid)) . '-model-verb:name]/[node:field-'.str_replace('_', '-', _devconnect_docgen_model_field_name($mid)).'-model-path]';
    variable_set('pathauto_node_' . _devconnect_docgen_model_name($mid) . '_pattern', $pattern);
  }
}

/**
 * Function that creates field instances for taxonomy terms
 *
 * @param $widget
 * @param $mid
 * @param $title
 * @param $instances
 */
function devconnect_docgen_create_field_instance($widget, $mid, $title, &$instances) {
  $instances['node-' . _devconnect_docgen_model_name($mid) . '-field_' . _devconnect_docgen_model_field_name($mid) . '_model_' . $title] = array(
    'bundle' => _devconnect_docgen_model_name($mid),
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
    'field_name' => 'field_' . _devconnect_docgen_model_field_name($mid) . '_model_' . $title,
    'label' => $title,
    'required' => 1,
    'settings' => array(
      'user_register_form' => FALSE,
    ),
    'widget' => $widget,
  );
}

/**
 * Function that creates the field_info for taxonomy terms.
 *
 * @param $cardinality
 * @param $vocab
 * @param _devconnect_docgen_model_name($mid)
 * @param $title
 * @param $fields
 */
function devconnect_docgen_create_field_info($cardinality, $vocab, $mid, $title, &$fields) {
  // this is where we want to add a method field to the content type we created
  $fields['field_' . _devconnect_docgen_model_field_name($mid) . '_model_' . $title] = array(
    'active' => 1,
    'cardinality' => $cardinality,
    'deleted' => 0,
    'entity_types' => array(),
    'field_name' => 'field_' . _devconnect_docgen_model_field_name($mid) . '_model_' . $title,
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
 * Batch operation for creating all of the detected taxonomy terms.
 *
 * @param $mid
 * @param $verbose
 * @param $selected
 * @param $context
 */
function devconnect_docgen_import_map_node_fields($mid, $verbose, $operation, $resource, $selected, $publish, &$context) {
  $exists = array_keys(variable_get(_devconnect_docgen_model_name($mid) . '_nodes', array()));
  if (!in_array($operation['id'], $exists)) {
    devconnect_docgen_operation_node($operation, $mid, $publish, $verbose, $resource, $context);
  } else {
    if (isset($operation['displayName'])) {
      $context['message'] = t('Node: ' . $operation['displayName'] . ' already created');
    } else {
      $context['message'] = t('Node: ' . $operation['name'] . ' already created');
    }
  }
}

/**
 * Makes a single node from an operation in the model.
 *
 * @param $operation
 * @param $mid
 * @param $publish
 * @param $verbose
 * @param $resource
 * @param $selected
 * @param $exists
 */
function devconnect_docgen_operation_node($operation, $mid, $publish, $verbose, $resource, &$context) {
  global $user;
  global $base_url;
  $values = array(
    'type' => _devconnect_docgen_model_name($mid),
    'uid' => $user->uid,
    'status' => ($publish) ? 1 : 0,
    'comment' => 1,
    'promote' => 0,
  );

  $query = array();
  if (!empty($operation['parameters'])) {
    foreach($operation['parameters'] as $parameter) {
      if (strtolower($parameter['type']) == 'query') {
        $query[] = $parameter['name'];
      }
    }
  }
  if (!empty($query)) {
    $mpid = $mid . $operation['path'] . '/' . strtoupper($operation['verb']) . '?' . implode('&', $query);
  } else {
    $mpid = $mid . $operation['path'] . '/' . strtoupper($operation['verb']);
  }

  $revision = db_query('SELECT revision FROM {smartdocs} WHERE mpid = :mpid AND model = :mid', array(
    ':mpid' => $mpid,
    ':mid' => $mid,
  ))->fetchField();

  $exists = db_query('SELECT nid FROM {smartdocs} WHERE mpid = :mpid AND model = :mid', array(
    ':mpid' => $mpid,
    ':mid' => $mid,
  ))->fetchField();

  $loaded = FALSE;

  if ($exists) {
    if ($revision < $verbose['revisionNumber'] || $revision > $verbose['revisionNumber']) {
      $entities = entity_load('node', FALSE, array('type' => _devconnect_docgen_model_name($mid), 'nid' => $exists));
      $loaded = TRUE;
      $entity = current($entities);
      $entity->revision = 1;
      $entity->log = t('Revision: @rev - Method: @title - @method was synced from SmartDocs.', array(
        '@rev' => $verbose['revisionNumber'],
        '@title' => $operation['displayName'],
        '@method' => $operation['id']
      ));
      db_update('smartdocs')->fields(array('revision' => $verbose['revisionNumber']))
        ->condition('model', $mid)
        ->condition('mpid', $mpid)
        ->execute();
    } else {
      drupal_set_message(t('ERROR: you have duplicate methods for: %p , %v (%m)' , array(
          '%p' => $operation['path'],
          '%v' => $operation['verb'],
          '%m' => $mpid)
      ), 'error');
      return;
    }
  } else {
    $entity = entity_create('node', $values);
    $entity->revision = 1;
    $entity->log = t('Revision: @rev - Method: @title - @method was created from SmartDocs.', array(
      '@rev' => $verbose['revisionNumber'],
      '@title' => $operation['displayName'],
      '@method' => $operation['id']
    ));
  }

  $wrapper = entity_metadata_wrapper('node', $entity);
  if (isset($operation['displayName'])) {
    $wrapper->title->set($operation['displayName']);
    if ($loaded) {
      $context['message'] = t('Syncing node: ' . $operation['displayName']);
    } else {
      $context['message'] = t('Creating node: ' . $operation['displayName']);
    }
  } else {
    $wrapper->title->set($operation['name']);
    if ($loaded) {
      $context['message'] = t('Syncing node: ' . $operation['name']);
    } else {
      $context['message'] = t('Creating node: ' . $operation['name']);
    }
  }
  if (isset($operation['description'])) {
    $wrapper->body->set(array('value' => $operation['description']));
  } else {
    $wrapper->body->set(array('value' => ''));
  }
  $field_name = 'field_' . _devconnect_docgen_model_field_name($mid) . '_model_revisions';
  $terms = taxonomy_get_term_by_name($verbose['revisionNumber'], _devconnect_docgen_model_name($mid) . '_revisions');
  foreach ($terms as $term) {
    if ($term->vocabulary_machine_name == _devconnect_docgen_model_name($mid) . '_revisions') {
      $field =& $wrapper->$field_name;
      $field->set(intval($term->tid));
    }
  }
  $types = array();
  if (isset($operation['parameters']) && (count($operation['parameters']) > 0)) {
    foreach($operation['parameters'] as $parameter) {
      if (strtolower($parameter['name']) == 'content-type') {
        if (isset($parameter['defaultValue'])) {
          $terms = taxonomy_get_term_by_name($parameter['defaultValue'], _devconnect_docgen_model_name($mid) . '_types');
          foreach ($terms as $term) {
            $types[] = $term->tid;
          }
        }
      }
    }
  }
  $field_name = 'field_' . _devconnect_docgen_model_field_name($mid) . '_model_types';
  $i = 0;
  if (!empty($types)) {
    foreach($types as $type) {
      $field =& $wrapper->$field_name;
      $field[$i]->set(intval($type));
      $i++;
    }
  } else {
    $terms = taxonomy_get_term_by_name('n/a', _devconnect_docgen_model_name($mid) . '_types');
    foreach ($terms as $term) {
      $field =& $wrapper->$field_name;
      $field[$i]->set(intval($term->tid));
      $i++;
    }
  }
  foreach($operation as $title => $field) {
    $tags = array();
    $auths = array();
    if (is_array($operation['tags']) && !empty($operation['tags'])) {
      foreach($operation['tags'] as $tag) {
        $terms = taxonomy_get_term_by_name($tag, _devconnect_docgen_model_name($mid) . '_tags');
        foreach ($terms as $term) {
          if ($term->vocabulary_machine_name == _devconnect_docgen_model_name($mid) . '_tags') {
            $tags[] = $term->tid;
          }
        }
      }
    }
    if (is_array($operation['authSchemes']) && !empty($operation['authSchemes'])) {
      foreach($operation['authSchemes'] as $scheme) {
        $terms = taxonomy_get_term_by_name($scheme, _devconnect_docgen_model_name($mid) . '_auth_schemes');
        foreach ($terms as $term) {
          if ($term->vocabulary_machine_name == _devconnect_docgen_model_name($mid) . '_auth_schemes') {
            $auths[] = $term->tid;
          }
        }
      }
    }
    switch($title) {
      case 'verb':
        $field_name = 'field_' . _devconnect_docgen_model_field_name($mid) . '_model_' . strtolower($title);
        $terms = taxonomy_get_term_by_name($operation['verb'], _devconnect_docgen_model_name($mid) . '_methods');
        foreach ($terms as $term) {
          if ($term->vocabulary_machine_name == _devconnect_docgen_model_name($mid) . '_methods') {
            $field =& $wrapper->$field_name;
            $field->set(intval($term->tid));
          }
        }
        break;
      case 'authSchemes':
        $field_name = 'field_' . _devconnect_docgen_model_field_name($mid) . '_model_' . strtolower($title);
        $i = 0;
        foreach($auths as $auth) {
          $field =& $wrapper->$field_name;
          $field[$i]->set(intval($auth));
          $i++;
        }
        break;
      case 'tags':
        $field_name = 'field_' . _devconnect_docgen_model_field_name($mid) . '_model_' . strtolower($title);
        $i = 0;
        foreach($tags as $tag) {
          $field =& $wrapper->$field_name;
          $field[$i]->set(intval($tag));
          $i++;
        }
        break;
      default:
        break;
    }
  }
  $field_name = 'field_' . _devconnect_docgen_model_field_name($mid) . '_model_path';
  $wrapper->$field_name = $resource['path'];
  if ($wrapper->save()) {
    $node = $wrapper->value();
    _devconnect_docgen_sync_nodes($node->nid, $operation['id'], _devconnect_docgen_model_name($mid), 'create');
    $sid = db_query('SELECT sid FROM {smartdocs} WHERE nid = :nid', array(':nid' => $node->nid))->fetchField();
    $record = array (
      'nid' =>  $node->nid,
      'revision' => $verbose['revisionNumber'],
      'resource' => $resource['id'],
      'method' => $operation['id'],
      'model' => $mid,
      'synced' => 1,
      'mpid' => $mpid,
    );
    if ($sid) {
      $record['sid'] = $sid;
      drupal_write_record('smartdocs', $record, 'sid');
    } else {
      drupal_write_record('smartdocs', $record);
    }
  }
  entity_save('node', $entity);
  $element = array(
    'revision' => $verbose['revisionNumber'],
    'rid' => $resource['id'],
    'method_id' => $operation['id'],
    'method_full' => $operation,
    'devportal' => TRUE,
  );
  entity_get_controller('docgen_operation')->updateMethod($mid, $element);
  cache_clear_all($node->nid, 'cache_docgen');
}

/**
 * Batch operation for creating views.
 *
 * @param $mid
 * @param $verbose
 * @param $selected
 * @param $context
 */
function devconnect_docgen_import_create_views($mid, $verbose, $selected, &$context) {
  // If this is somehow called before the private filesystem stream wrapper is
  // registered, we need to perform ugly extra magic to make sure we can still
  // write.
  // todo: Should we just call drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL) instead?
  $wrappers = stream_get_wrappers();
  if (in_array('private', $wrappers)) {
    $url_base = 'private://';
  }
  else {
    // Ugh. Do it the hard way!
    $url_base = variable_get('file_private_path', NULL);
    // If this is not valid, make some guesses in a best-faith effort.
    if (empty($url_base) || !is_dir($url_base)) {
      $url_base = 'sites/default/files/private';
      if (!is_dir($url_base)) {
        $url_base = 'sites/default/private';
        if (!is_dir($url_base)) {
          throw new Exception('Cannot find private filesystem.');
        }
      }
    }
    $url_base .= '/';
  }
  if (!is_writable($url_base)) {
    throw new Exception('Private filesystem is not writable.');
  }

  $context['message'] = t('Creating views...');
  module_load_include('inc', 'devconnect_docgen', 'includes/templates/views/default_view.template');
  if (!views_get_view(_devconnect_docgen_model_name($mid) . '_listing')) {
    file_save_data(_devconnect_docgen_generate_default_view($mid, $verbose['displayName']), $url_base . _devconnect_docgen_model_name($mid) . '_listing.view', FILE_EXISTS_REPLACE);
  }
  cache_clear_all('*', 'cache_views', TRUE);
  module_invoke_all('views_invalidate_cache');
}

/**
 * Batch operation for flushing the JS/CSS cache.
 *
 * @param $context
 */
function devconnect_docgen_clear_caches_flush(&$context) {
  _drupal_flush_css_js();
  $context['results'][] = "cache_flush";
  $context['message'] = t('CSS & JS flushed');
}

/**
 * Batch operation for flushing the Registry cache.
 *
 * @param $context
 */
function devconnect_docgen_rebuild_registry(&$context) {
  registry_rebuild();
  $context['results'][] = "cache_registry";
  $context['message'] = t('Registry Rebuilt');
}

/**
 * Batch operation for flushing the Registry cache.
 *
 * @param $context
 */
function devconnect_docgen_clear_caches_css(&$context) {
  drupal_clear_css_cache();
  $context['results'][] = "cache_css";
  $context['message'] = t('CSS Caches Cleared');
}

/**
 * Batch operation for flushing the JS cache.
 *
 * @param $context
 */
function devconnect_docgen_clear_caches_js(&$context) {
  drupal_clear_js_cache();
  $context['results'][] = "cache_js";
  $context['message'] = t('JS Caches Cleared');
}

/**
 * Batch operation for flushing the Theme cache.
 *
 * @param $context
 */
function devconnect_docgen_clear_caches_theme(&$context) {
  system_rebuild_theme_data();
  drupal_theme_rebuild();
  $context['results'][] = "cache_theme";
  $context['message'] = t('Theme Caches Cleared');
}

/**
 * Batch operation for flushing the Entity cache.
 *
 * @param $context
 */
function devconnect_docgen_clear_caches_entity(&$context) {
  entity_info_cache_clear();
  $context['results'][] = "cache_entity";
  $context['message'] = t('Entity Caches Cleared');
}

/**
 * Batch operation for flushing the Node cache.
 *
 * @param $context
 */
function devconnect_docgen_clear_caches_nodes(&$context) {
  node_types_rebuild();
  $context['results'][] = "cache_node";
  $context['message'] = t('Node Caches Cleared');
}

/**
 * Batch operation for flushing the Menu cache.
 *
 * @param $context
 */
function devconnect_docgen_clear_caches_menu(&$context) {
  menu_rebuild();
  $context['results'][] = "cache_menu";
  $context['message'] = t('Menu Caches Cleared');
}

/**
 * Batch operation for creating menu items.
 *
 * @param $mid
 * @param $verbose
 * @param $selected
 * @param $context
 */
function devconnect_docgen_import_create_menu_items($mid, $visible, &$context) {
  $context['message'] = t('Creating menu structure...');
  $link = array(
    'link_title' => 'APIs',
    'link_path' => $mid . '/apis',
    'menu_name' => 'main-menu',
    'weight' => 0,
    'expanded' => 1,
    'plid' => 0,
  );
  $mlid = db_query("SELECT mlid from {menu_links} WHERE link_title=:link_title AND link_path=:link_path", array(
    ':link_title' =>  $link['link_title'],
    ':link_path' => $link['link_path']
  ))->fetchField();
  if(!$mlid) {
    $item = array(
      'link_path' => $link['link_path'],
      'link_title' => $link['link_title'],
      'menu_name' => $link['menu_name'],
      'weight' => $link['weight'],
      'expanded' => $link['expanded'],
    );
    $plid = menu_link_save($item);
    if (!$visible) {
      db_update('menu_links')->fields(array('hidden'=>1))
        ->condition('link_title', $link['link_title'])
        ->condition('link_path', $link['link_path'])
        ->execute();
    }
    if ($plid) {
      _devconnect_docgen_build_tag_menu_items($mid, $plid, $visible);
    } else {
      drupal_set_message('Menu link for ' . $link['link_title'] . ' could not be saved', 'error');
    }
  } else {
    _devconnect_docgen_build_tag_menu_items($mid, $mlid, $visible);
  }
}

/**
 * Build tags into logical menu items.
 *
 * @param $mid
 * @param $plid
 */
function _devconnect_docgen_build_tag_menu_items($mid, $plid, $visible) {
  $vid = taxonomy_vocabulary_machine_name_load(_devconnect_docgen_model_name($mid) . '_tags')->vid;
  $tree = taxonomy_get_tree($vid);
  foreach ($tree as $term) {
    $link = array(
      'link_title' => $term->name,
      'link_path' => 'taxonomy/term/' . $term->tid,
      'menu_name' => 'main-menu',
      'weight' => 0,
      'expanded' => 1,
    );
    $mlid = db_query("SELECT mlid from {menu_links} WHERE link_title=:link_title AND link_path=:link_path", array(
      ':link_title' =>  $link['link_title'],
      ':link_path' => $link['link_path']
    ))->fetchField();
    if(!$mlid) {
      $item = array(
        'link_path' => $link['link_path'],
        'link_title' => $link['link_title'],
        'menu_name' => $link['menu_name'],
        'weight' => $link['weight'],
        'expanded' => $link['expanded'],
        'plid' => $plid,
      );
      $tag_plid = menu_link_save($item);
      if (!$visible) {
        db_update('menu_links')->fields(array('hidden'=>1))
          ->condition('link_title', $link['link_title'])
          ->condition('link_path', $link['link_path'])
          ->execute();
      }
      _devconnect_docgen_build_node_menu_items($tag_plid, $term->tid, $visible);
    } else {
      _devconnect_docgen_build_node_menu_items($mlid, $term->tid, $visible);
    }
  }

}

/**
 * Build tags into logical menu items.
 *
 * @param $plid
 * @param $tid
 */
function _devconnect_docgen_build_node_menu_items($plid, $tid, $visible) {
  $nodes = taxonomy_select_nodes($tid);
  foreach($nodes as $node) {
    $node = node_load($node);
    $link = array(
      'link_title' => $node->title,
      'link_path' => 'node/' . $node->nid,
      'menu_name' => 'main-menu',
      'weight' => 0,
      'expanded' => 0,
    );
    $mlid = db_query("SELECT mlid from {menu_links} WHERE link_title=:link_title AND link_path=:link_path", array(
      ':link_title' =>  $link['link_title'],
      ':link_path' => $link['link_path']
    ))->fetchField();
    if(!$mlid) {
      $item = array(
        'link_path' => $link['link_path'],
        'link_title' => $link['link_title'],
        'menu_name' => $link['menu_name'],
        'weight' => $link['weight'],
        'expanded' => $link['expanded'],
        'plid' => $plid,
      );
      menu_link_save($item);
      if (!$visible) {
        db_update('menu_links')->fields(array('hidden'=>1))
          ->condition('link_title', $link['link_title'])
          ->condition('link_path', $link['link_path'])
          ->execute();
      }
    }
  }
}

/**
 * Finished function for importing nodes from the Modeling API.
 *
 * @param $success
 * @param $results
 * @param $operations
 */
function _devconnect_docgen_import_nodes_finished($success, $results, $operations) {
  if ($success) {
    menu_rebuild();
    $rev = $results['revisionNumber'];
    drupal_set_message('The selected SmartDocs methods for revision #' . $rev . ' have been successfully rendered as nodes.', 'status');
    $mid = $results['mid'];
    drupal_goto('admin/content/smartdocs/list/' . $mid .'/detail', array(
      'query' => array(
        'rev' => $rev,
      )
    ));
  } else {
    drupal_set_message('The render failed, consult the logs for more information.', 'error');
  }
}
