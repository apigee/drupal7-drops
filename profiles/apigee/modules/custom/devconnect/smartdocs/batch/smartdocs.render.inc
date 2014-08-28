<?php

/**
 * Batch API Function to import nodes.
 *
 * @param $model
 * @param $verbose
 * @param $selected
 * @param $options
 * @param bool $render_template
 * @return array
 */
function smartdocs_render($model, $verbose, $selected, $options, $render_template = TRUE) {
  $model['smart_mid'] = smartdocs_model_name($model['name']);
  $model['smart_field'] = smartdocs_field_name($model['name']);
  if (!array_key_exists('displayName', $model)) {
    $model['displayName'] = $model['name'];
  }
  if ($render_template) {
    $operations[] = array('smartdocs_template', array($model['name']));
  }
  $operations[] = array('smartdocs_taxonomy_vocab', array($model['smart_mid'], $model['displayName']));
  $operations[] = array('smartdocs_taxonomy_term', array($model['name'], $model['smart_mid'], $verbose, $selected));
  $operations[] = array('smartdocs_content_type', array($model['name'], $model['smart_mid'], $model['displayName'], $verbose));
  $operations[] = array('smartdocs_node_fields', array($model['name'], $model['smart_mid'], $model['smart_field'], $verbose));
  $operations[] = array('smartdocs_node_paths', array($model['name'], $model['smart_mid'], $model['smart_field'], $options, $verbose));
  $publish = ($options['publish']) ? TRUE : FALSE;
  if (is_array($verbose['resources'])) {
    foreach ($selected as $select) {
      if ($select == '0') {
        continue;
      }
      foreach ($verbose['resources'] as $resource) {
        if (!array_key_exists('methods', $resource) || !is_array($resource['methods'])) {
          continue;
        }
        foreach ($resource['methods'] as $method) {
          if ($select == $method['id']) {
            $operations[] = array(
              'smartdocs_render_nodes',
              array(
                $model['name'],
                $model['smart_mid'],
                $model['smart_field'],
                $verbose,
                $method,
                $resource,
                $publish
              )
            );
          }
        }
      }
    }
  }
  $operations[] = array('smartdocs_views', array($model['name'], $model['smart_mid'], $model['smart_field'], $model['displayName']));
  $operations[] = array('smartdocs_clear_caches_flush', array());
  $operations[] = array('smartdocs_clear_caches_css', array());
  $operations[] = array('smartdocs_clear_caches_js', array());
  $operations[] = array('smartdocs_clear_caches_theme', array());
  $operations[] = array('smartdocs_clear_caches_entity', array());
  $operations[] = array('smartdocs_clear_caches_nodes', array());
  $operations[] = array('smartdocs_clear_caches_menu', array());

  $batch = array(
    'title' => t('Rendering nodes from the %model model', array('%model' => $model['displayName'])),
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
 * @param $mid
 * @param $context
 */
function smartdocs_template($mid, &$context) {
  if (!in_array($mid, array_keys(node_type_get_names()))) {
    $context['message'] = t('Ensuring correct model template');
    $html = file_get_contents(drupal_get_path('module', 'smartdocs') . '/templates/smartdocs.hbr');
    entity_get_controller('smartdocs_template')->updateTemplate($mid, 'method', $html);
  }
}

/**
 * Created SmartDocs Taxonomy Vocab
 *
 * @param $smart_mid
 * @param $display
 * @param $context
 */
function smartdocs_taxonomy_vocab($smart_mid, $display, &$context) {
  $context['message'] = t('Creating taxonomy vocabs...');
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
        'name' => $display . ' ' . $name,
        'machine_name' => $smart_mid . '_' . $machine,
      ));
    }
  }
}

/**
 * Creates SmartDocs Taxonomy Terms
 *
 * @param $mid
 * @param $smart_mid
 * @param $verbose
 * @param $selected
 * @param $context
 */
function smartdocs_taxonomy_term($mid, $smart_mid, $verbose, $selected, &$context) {
  // important to redirect to correct rev
  $context['results'] = $verbose;
  $context['results']['mid'] = $mid;
  $context['message'] = t('Creating taxonomy terms...');
  // Tags for the method vocabulary
  $vid = taxonomy_vocabulary_machine_name_load($smart_mid . '_methods')->vid;
  if ($vid) {
    $methods = array('GET', 'PATCH', 'POST', 'PUT', 'DELETE',);
    $created = array();
    $path = variable_get('pathauto_taxonomy_term_' . $smart_mid . '_methods_pattern', NULL);
    if (is_null($path)) {
      $pattern = $mid . '/apis/[term:name]';
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
    $pattern = $mid . '/apis/[term:name]';
    variable_set('pathauto_taxonomy_term_' . $smart_mid . '_revisions_pattern', $pattern);
  }
  // Task to figure out which version to create
  $revision_vid = taxonomy_vocabulary_machine_name_load($smart_mid . '_revisions')->vid;
  $term = taxonomy_get_term_by_name($verbose['revisionNumber'], $smart_mid . '_revisions');
  if (empty($term)) {
    if ($revision_vid) {
      taxonomy_term_save((object) array(
        'name' => $verbose['revisionNumber'],
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
  if (is_array($verbose['resources'])) {
    foreach ($verbose['resources'] as $resource) {
      if (isset($resource['methods']) && is_array($resource['methods'])) {
        foreach ($resource['methods'] as $operation) {
          if (in_array($operation['id'], $selected)) {
            $path = variable_get('pathauto_taxonomy_term_' . $smart_mid . '_types_pattern', NULL);
            if (is_null($path)) {
              $pattern = $mid . '/apis/[term:name]';
              variable_set('pathauto_taxonomy_term_' . $smart_mid . '_types_pattern', $pattern);
            }
            if (isset($operation['parameters']) && (count($operation['parameters']))) {
              if (isset($operation['parameters']) && (count($operation['parameters']) > 0)) {
                foreach($operation['parameters'] as $parameter) {
                  if (strtolower($parameter['name']) == 'content-type') {
                    $types[] = $parameter['defaultValue'];
                    $term = taxonomy_get_term_by_name($parameter['defaultValue'], $smart_mid . '_types');
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
            $path = variable_get('pathauto_taxonomy_term_' . $smart_mid . '_tags_pattern', NULL);
            if (is_null($path)) {
              $pattern = $mid . '/api/[term:name]';
              variable_set('pathauto_taxonomy_term_' . $smart_mid . '_tags_pattern', $pattern);
            }
            if (is_array($operation['tags']) && !empty($operation['tags'])) {
              foreach($operation['tags'] as $tag) {
                if (!in_array($tag, $tags)) {
                  $tags[] = $tag;
                  $term = taxonomy_get_term_by_name($tag, $smart_mid . '_tags');
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
              $term = taxonomy_get_term_by_name('n/a', $smart_mid . '_tags');
              if (empty($term)) {
                if ($tag_vid) {
                  taxonomy_term_save((object) array(
                    'name' => 'n/a',
                    'vid' => $tag_vid,
                  ));
                }
              }
            }
            $path = variable_get('pathauto_taxonomy_term_' . $smart_mid . '_auth_schemes_pattern', NULL);
            if (is_null($path)) {
              $pattern = $mid . '/apis/[term:name]';
              variable_set('pathauto_taxonomy_term_' . $smart_mid . '_auth_schemes_pattern', $pattern);
            }
            if (is_array($operation['authSchemes']) && !empty($operation['authSchemes'])) {
              foreach($operation['authSchemes'] as $scheme) {
                if (!in_array($scheme, $schemes)) {
                  $schemes[] = $scheme;
                  $term = taxonomy_get_term_by_name($scheme, $smart_mid . '_auth_schemes');
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
              $term = taxonomy_get_term_by_name('n/a', $smart_mid . '_auth_schemes');
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
              $term = taxonomy_get_term_by_name('OAUTH2WEBSERVER', $smart_mid . '_auth_schemes');
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
 * @param $smart_mid
 * @param $display
 * @param $verbose
 * @param $context
 */
function smartdocs_content_type($mid, $smart_mid, $display, $verbose, &$context) {
  $context['message'] = t('Creating content type...');
  if (!in_array($smart_mid, node_type_get_names())) {
    $t = get_t();
    $type = array(
      'type' => $smart_mid,
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
}

/**
 * Batch operation for creating all of the necessary fields for the content type.
 *
 * @param $mid
 * @param $smart_mid
 * @param $smart_field
 * @param $verbose
 * @param $context
 */
function smartdocs_node_fields($mid, $smart_mid, $smart_field, $verbose, &$context) {
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
  if (!field_info_field('field_' . $smart_field . '_model_revisions')) {
    $vocab = $smart_mid . '_revisions';
    $cardinality = 1;
    smartdocs_field_info($cardinality, $vocab, $mid, $smart_mid, $smart_field, 'revisions', $fields);
    smartdocs_field_instance($widget, $mid, $smart_mid, $smart_field, 'revisions', $instances);
  }
  if (!field_info_field('field_' . $smart_field . '_model_types')) {
    $vocab = $smart_mid . '_types';
    $cardinality = -1;
    smartdocs_field_info($cardinality, $vocab, $mid, $smart_mid, $smart_field, 'types', $fields);
    smartdocs_field_instance($widget, $mid, $smart_mid, $smart_field, 'types', $instances);
  }
  if (is_array($revisions['resources'])) {
    foreach ($revisions['resources'] as $resource) {
      if (isset($resource['methods']) && is_array($resource['methods'])) {
        foreach ($resource['methods'] as $operation) {
          foreach($operation as $title => $field) {
            switch(strtolower($title)) {
              case 'verb':
                if (!field_info_field('field_' . $smart_field . '_model_' . strtolower($title))) {
                  $vocab = $smart_mid . '_methods';
                  $cardinality = 1;
                  smartdocs_field_info($cardinality, $vocab, $mid, $smart_mid, $smart_field, strtolower($title), $fields);
                  smartdocs_field_instance($widget, $mid, $smart_mid, $smart_field, strtolower($title), $instances);
                }
                break;
              case 'authschemes':
                if (!field_info_field('field_' . $smart_field . '_model_' . strtolower($title))) {
                  $vocab = $smart_mid . '_auth_schemes';
                  $cardinality = -1;
                  smartdocs_field_info($cardinality, $vocab, $mid, $smart_mid, $smart_field, strtolower($title), $fields);
                  smartdocs_field_instance($widget, $mid, $smart_mid, $smart_field, strtolower($title), $instances);
                }
                break;
              case 'tags':
                if (!field_info_field('field_' . $smart_field . '_model_' . strtolower($title))) {
                  $vocab = $smart_mid . '_tags';
                  $cardinality = -1;
                  smartdocs_field_info($cardinality, $vocab, $mid, $smart_mid, $smart_field, strtolower($title), $fields);
                  smartdocs_field_instance($widget, $mid, $smart_mid, $smart_field, strtolower($title), $instances);
                }
                break;
              default:
                // current version doesn't support additional query and header params as drupal fields
                break;
            }
            if (!field_info_field('field_' . $smart_field . '_model_path')) {
              $fields['field_' . $smart_field . '_model_path'] = array(
                'active' => 1,
                'cardinality' => 1,
                'deleted' => 0,
                'entity_types' => array(),
                'field_name' => 'field_' . $smart_field . '_model_path',
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
						if (!field_info_field('field_' . $smart_field . '_model_uuid')) {
							$fields['field_' . $smart_field . '_model_uuid'] = array(
								'active' => 1,
								'cardinality' => 1,
								'deleted' => 0,
								'entity_types' => array(),
								'field_name' => 'field_' . $smart_field . '_model_uuid',
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
}

/**
 * Generates path aliases for a given node.
 *
 * @param $mid
 * @param $smart_mid
 * @param $smart_field
 * @param $options
 * @param $verbose
 * @param $context
 */
function smartdocs_node_paths($mid, $smart_mid, $smart_field, $options, $verbose, &$context) {
	switch($options['path']) {
		case 'uuid':
			$path = variable_get('pathauto_node_' . $smart_mid . '_pattern', NULL);
			if (is_null($path)) {
				$pattern = $mid . '/apis/[node:field-' . str_replace('_', '-', $smart_field) . '-model-verb:name]/[node:field-'.str_replace('_', '-', $smart_field).'-model-uuid]';
				variable_set('pathauto_node_' . $smart_mid . '_pattern', $pattern);
			}
			break;
		case 'path':
			$path = variable_get('pathauto_node_' . $smart_mid . '_pattern', NULL);
			if (is_null($path)) {
				$pattern = $mid . '/apis/[node:field-' . str_replace('_', '-', $smart_field) . '-model-verb:name]/[node:field-'.str_replace('_', '-', $smart_field).'-model-path]';
				variable_set('pathauto_node_' . $smart_mid . '_pattern', $pattern);
			}
			break;
		default:
			$path = variable_get('pathauto_node_' . $smart_mid . '_pattern', NULL);
			if (is_null($path)) {
				$pattern = $mid . '/apis/[node:field-' . str_replace('_', '-', $smart_field) . '-model-verb:name]/[node:title]';
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
function smartdocs_field_instance($widget, $mid, $smart_mid, $smart_field, $title, &$instances) {
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
 * @param $mid
 * @param $smart_mid
 * @param $smart_field
 * @param $title
 * @param $fields
 */
function smartdocs_field_info($cardinality, $vocab, $mid, $smart_mid, $smart_field, $title, &$fields) {
  // this is where we want to add a method field to the content type we created
  $fields['field_' . $smart_field . '_model_' . $title] = array(
    'active' => 1,
    'cardinality' => $cardinality,
    'deleted' => 0,
    'entity_types' => array(),
    'field_name' => 'field_' . $smart_field . '_model_' . $title,
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
 * @param $smart_mid
 * @param $smart_field
 * @param $verbose
 * @param $method
 * @param $resource
 * @param $publish
 * @param $context
 */
function smartdocs_render_nodes($mid, $smart_mid, $smart_field, $verbose, $method, $resource, $publish, &$context) {
  $exists = db_query('SELECT nid FROM {smartdata} WHERE method = :uuid', array(
    ':uuid' => $method['id'],
  ))->fetchField();
  if (!$exists) {
    smartdocs_render_node($method, $mid, $smart_mid, $smart_field, $publish, $verbose, $resource, $context);
  } else {
    if (isset($operation['displayName'])) {
      $context['message'] = t('Node: ' . $method['displayName'] . ' already created');
    } else {
      $context['message'] = t('Node: ' . $method['name'] . ' already created');
    }
  }
}

/**
 * Renders nodes from SmartDocs Methods
 *
 * @param $method
 * @param $mid
 * @param $smart_mid
 * @param $smart_field
 * @param $publish
 * @param $verbose
 * @param $resource
 * @param $context
 */
function smartdocs_render_node($method, $mid, $smart_mid, $smart_field, $publish, $verbose, $resource, &$context) {
  global $user;

  $values = array(
    'type' => $smart_mid,
    'uid' => $user->uid,
    'status' => ($publish) ? 1 : 0,
    'comment' => 1,
    'promote' => 0,
  );

  $query = array();
  if (!empty($method['parameters'])) {
    foreach($method['parameters'] as $parameter) {
      if (strtolower($parameter['type']) == 'query') {
        $query[] = $parameter['name'];
      }
    }
  }
  if (!empty($query)) {
    $mpid = $mid . $method['path'] . '/' . strtoupper($method['verb']) . '?' . implode('&', $query);
  } else {
    $mpid = $mid . $method['path'] . '/' . strtoupper($method['verb']);
  }

  $revision = db_query('SELECT revision FROM {smartdata} WHERE mpid = :mpid AND model = :mid', array(
    ':mpid' => $mpid,
    ':mid' => $mid,
  ))->fetchField();

  $exists = db_query('SELECT nid FROM {smartdata} WHERE mpid = :mpid AND model = :mid', array(
    ':mpid' => $mpid,
    ':mid' => $mid,
  ))->fetchField();

  $loaded = FALSE;

  if ($exists) {
    if ($revision < $verbose['revisionNumber'] || $revision > $verbose['revisionNumber']) {
      $entities = entity_load('node', FALSE, array('type' => $smart_mid, 'nid' => $exists));
      $loaded = TRUE;
      $entity = current($entities);
      $entity->revision = 1;
      $entity->log = t('Revision: @rev - Method: @title - @method was synced from SmartDocs.', array(
        '@rev' => $verbose['revisionNumber'],
        '@title' => $method['displayName'],
        '@method' => $method['id']
      ));
      db_update('smartdata')->fields(array('revision' => $verbose['revisionNumber']))
        ->condition('model', $mid)
        ->condition('mpid', $mpid)
        ->execute();
    } else {
      drupal_set_message(t('ERROR: you have duplicate methods for: %p , %v (%m)' , array(
          '%p' => $method['path'],
          '%v' => $method['verb'],
          '%m' => $mpid)
      ), 'error');
      return;
    }
  } else {
    $entity = entity_create('node', $values);
    $entity->revision = 1;
    $entity->log = t('Revision: @rev - Method: @title - @method was created from SmartDocs.', array(
      '@rev' => $verbose['revisionNumber'],
      '@title' => $method['displayName'],
      '@method' => $method['id']
    ));
  }

  $wrapper = entity_metadata_wrapper('node', $entity);
  if (isset($method['displayName'])) {
    $wrapper->title->set($method['displayName']);
    if ($loaded) {
      $context['message'] = t('Syncing node: ' . $method['displayName']);
    } else {
      $context['message'] = t('Creating node: ' . $method['displayName']);
    }
  } else {
    $wrapper->title->set($method['name']);
    if ($loaded) {
      $context['message'] = t('Syncing node: ' . $method['name']);
    } else {
      $context['message'] = t('Creating node: ' . $method['name']);
    }
  }
  if (isset($method['description'])) {
    $wrapper->body->set(array('value' => $method['description']));
  } else {
    $wrapper->body->set(array('value' => ''));
  }
  $field_name = 'field_' . $smart_field . '_model_revisions';
  $terms = taxonomy_get_term_by_name($verbose['revisionNumber'], $smart_mid . '_revisions');
  foreach ($terms as $term) {
    if ($term->vocabulary_machine_name == $smart_mid . '_revisions') {
      $field =& $wrapper->$field_name;
      $field->set(intval($term->tid));
    }
  }
  $types = array();
  if (isset($method['parameters']) && (count($method['parameters']) > 0)) {
    foreach($method['parameters'] as $parameter) {
      if (strtolower($parameter['name']) == 'content-type') {
        if (isset($parameter['defaultValue'])) {
          $terms = taxonomy_get_term_by_name($parameter['defaultValue'], $smart_mid . '_types');
          foreach ($terms as $term) {
            $types[] = $term->tid;
          }
        }
      }
    }
  }
  $field_name = 'field_' . $smart_field . '_model_types';
  $i = 0;
  if (!empty($types)) {
    foreach($types as $type) {
      $field =& $wrapper->$field_name;
      $field[$i]->set(intval($type));
      $i++;
    }
  } else {
    $terms = taxonomy_get_term_by_name('n/a', $smart_mid . '_types');
    foreach ($terms as $term) {
      $field =& $wrapper->$field_name;
      $field[$i]->set(intval($term->tid));
      $i++;
    }
  }
  foreach($method as $title => $field) {
    $tags = array();
    $auths = array();
    if (is_array($method['tags']) && !empty($method['tags'])) {
      foreach($method['tags'] as $tag) {
        $terms = taxonomy_get_term_by_name($tag, $smart_mid . '_tags');
        foreach ($terms as $term) {
          if ($term->vocabulary_machine_name == $smart_mid . '_tags') {
            $tags[] = $term->tid;
          }
        }
      }
    }
    if (array_key_exists('authSchemes', $method) && is_array($method['authSchemes']) && !empty($method['authSchemes'])) {
      foreach($method['authSchemes'] as $scheme) {
        $terms = taxonomy_get_term_by_name($scheme, $smart_mid . '_auth_schemes');
        foreach ($terms as $term) {
          if ($term->vocabulary_machine_name == $smart_mid . '_auth_schemes') {
            $auths[] = $term->tid;
          }
        }
      }
    }
    switch($title) {
      case 'verb':
        $field_name = 'field_' . $smart_field . '_model_' . strtolower($title);
        $terms = taxonomy_get_term_by_name($method['verb'], $smart_mid . '_methods');
        foreach ($terms as $term) {
          if ($term->vocabulary_machine_name == $smart_mid . '_methods') {
            $field =& $wrapper->$field_name;
            $field->set(intval($term->tid));
          }
        }
        break;
      case 'authSchemes':
        $field_name = 'field_' . $smart_field . '_model_' . strtolower($title);
        $i = 0;
        foreach($auths as $auth) {
          $field =& $wrapper->$field_name;
          $field[$i]->set(intval($auth));
          $i++;
        }
        break;
      case 'tags':
        $field_name = 'field_' . $smart_field . '_model_' . strtolower($title);
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
  // path
	$field_name = 'field_' . $smart_field . '_model_path';
  $wrapper->$field_name = $resource['path'];
	// uuid
	$field_name = 'field_' . $smart_field . '_model_uuid';
	$wrapper->$field_name = $resource['id'];
  if ($wrapper->save()) {
    $node = $wrapper->value();
    if ($node->nid) {
      cache_clear_all($node->nid, 'cache_smartdocs');
      $sid = db_query('SELECT sid FROM {smartdata} WHERE nid = :nid', array(':nid' => $node->nid))->fetchField();
      $record = array (
        'nid' =>  $node->nid,
        'revision' => $verbose['revisionNumber'],
        'resource' => $resource['id'],
        'method' => $method['id'],
        'model' => $mid,
        'synced' => 1,
        'mpid' => $mpid,
      );
      if ($sid) {
        $record['sid'] = $sid;
        drupal_write_record('smartdata', $record, 'sid');
      } else {
        drupal_write_record('smartdata', $record);
      }
    }
  }
}

/**
 * Batch operation for creating views.
 *
 * @param $mid
 * @param $smart_mid
 * @param $smart_field
 * @param $display
 * @param $context
 * @throws Exception
 */
function smartdocs_views($mid, $smart_mid, $smart_field, $display, &$context) {
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
  module_load_include('inc', 'smartdocs', 'templates/smartdocs.views');
  if (!views_get_view($smart_mid . '_listing')) {
    $view = smartdocs_view($mid, $smart_mid, $smart_field, $display);
    file_save_data($view, $url_base . $smart_mid . '_listing.view', FILE_EXISTS_REPLACE);
  }
  cache_clear_all('*', 'cache_views', TRUE);
  module_invoke_all('views_invalidate_cache');
}

/**
 * Batch operation for flushing the JS/CSS cache.
 *
 * @param $context
 */
function smartdocs_clear_caches_flush(&$context) {
  _drupal_flush_css_js();
  $context['results'][] = "cache_flush";
  $context['message'] = t('CSS & JS flushed');
}

/**
 * Batch operation for flushing the Registry cache.
 *
 * @param $context
 */
function smartdocs_rebuild_registry(&$context) {
  registry_rebuild();
  $context['results'][] = "cache_registry";
  $context['message'] = t('Registry Rebuilt');
}

/**
 * Batch operation for flushing the Registry cache.
 *
 * @param $context
 */
function smartdocs_clear_caches_css(&$context) {
  drupal_clear_css_cache();
  $context['results'][] = "cache_css";
  $context['message'] = t('CSS Caches Cleared');
}

/**
 * Batch operation for flushing the JS cache.
 *
 * @param $context
 */
function smartdocs_clear_caches_js(&$context) {
  drupal_clear_js_cache();
  $context['results'][] = "cache_js";
  $context['message'] = t('JS Caches Cleared');
}

/**
 * Batch operation for flushing the Theme cache.
 *
 * @param $context
 */
function smartdocs_clear_caches_theme(&$context) {
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
function smartdocs_clear_caches_entity(&$context) {
  entity_info_cache_clear();
  $context['results'][] = "cache_entity";
  $context['message'] = t('Entity Caches Cleared');
}

/**
 * Batch operation for flushing the Node cache.
 *
 * @param $context
 */
function smartdocs_clear_caches_nodes(&$context) {
  node_types_rebuild();
  $context['results'][] = "cache_node";
  $context['message'] = t('Node Caches Cleared');
}

/**
 * Batch operation for flushing the Menu cache.
 *
 * @param $context
 */
function smartdocs_clear_caches_menu(&$context) {
  menu_rebuild();
  $context['results'][] = "cache_menu";
  $context['message'] = t('Menu Caches Cleared');
}

/**
 * Finish function for when SmartDocs Renders
 *
 * @param $success
 * @param $results
 * @param $operations
 */
function smartdocs_render_finished($success, $results, $operations) {
  if ($success) {

  } else {
    drupal_set_message('The render failed, consult the logs for more information.', 'error');
  }
}
