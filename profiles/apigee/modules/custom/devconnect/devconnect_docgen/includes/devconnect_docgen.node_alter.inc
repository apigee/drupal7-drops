<?php

/**
 * Implements hook_form_alter()
 *
 * @param $form
 * @param $form_state
 * @param $form_id
 * @param array $parameters
 */
function devconnect_docgen_form_alter(&$form, &$form_state, $form_id, $parameters = array()) {

  $models = variable_get('devconnect_docgen_content_types', array());

  if (isset($form['#node']) && in_array($form['#node']->type, array_keys($models))) {
    $type = $form['#node']->type;
    $mid = $type;
    $header = array();
    if (isset($form['field_' . $mid . '_model_requestjson'])) {
      $form['field_' . $mid . '_model_requestjson']['#type'] = 'hidden';

      $form['parameter_container'] = array(
        '#type' => 'fieldset',
        '#title' => t('Query Parameters'),
        '#prefix' => '<div id="parameters-container">',
        '#suffix' => '</div><br/>',
        '#weight' => 50,
      );
      $form['parameter_container']['parameters'] = array(
        '#prefix' => '<div id="api-parameters">',
        '#suffix' => '</div>',
        '#tree' => TRUE,
        '#theme' => 'devconnect_docgen_param_table',
        '#header' => array(),
      );

      $form['apigee_content_container'] = array(
        // '#type' => 'fieldset', will remain commented out until we figure out what to do with the Apigee Content
        '#title' => t('Apigee Content'),
        '#prefix' => '<div id="apigee-content-container">',
        '#suffix' => '</div><br/>',
        '#weight' => 50,
      );
      $form['apigee_content_container']['parameters'] = array(
        '#type' => 'hidden',
        '#multiple' => TRUE,
        '#default_value' => '',
      );
      if (empty($form['field_' . $mid . '_model_requestjson'][LANGUAGE_NONE][0]['value']['#default_value'])) {
        $default = array();
        $default['params'][] = array(
          'name' => '',
          'required' => '',
          'type' => '',
          'style' => '',
          'description' => '',
        );
        $form['field_' . $mid . '_model_requestjson'][LANGUAGE_NONE][0]['value']['#default_value'] = drupal_json_encode($default);
      }
      if (!empty($form['field_' . $mid . '_model_requestjson'][LANGUAGE_NONE][0]['value']['#default_value'])) {
        $parameters = drupal_json_decode($form['field_' . $mid . '_model_requestjson'][LANGUAGE_NONE][0]['value']['#default_value']);

        if (isset($form_state['storage']['ajaxed'])) {
          foreach ($parameters['params'] as $key => $params) {
            foreach($params as $title => $param) {
              $header[$title] = '';
            }
          }
          if (empty($header)) {
            $header = array(
              'name' => '',
              'required' => '',
              'type' => '',
              'style' => '',
              'description' => '',
            );
          }
          $count = $form_state['storage']['num_parameters'];
          for ($i = count($parameters['params']); $i <= $count; $i++) {
            foreach ($header as $title => $value) {
              if (empty($parameters['params'][$i][$title])) {
                $parameters['params'][$i][$title] = $value;
              }
            }
          }
          if (isset($form_state['storage']['remove'])) {
            unset($parameters['params'][$form_state['storage']['remove']]);
          }
        }

        if (!empty($parameters['params'])) {
          _devconnect_docgen_make_params($parameters['params'], $form, $header, $form_state);
        } else {
          _devconnect_docgen_make_params($parameters, $form, $header, $form_state, TRUE);
        }
      } else {
        _devconnect_docgen_make_params($parameters, $form, $header, $form_state, TRUE);
      }
      if (isset($form['parameter_container'])) {
        $form['parameter_container']['parameters']['#header'] = $header;

        $form['parameter_container']['add_parameter'] = array(
          '#type' => 'submit',
          '#value' => t('Add Parameter'),
          '#submit' => array('devconnect_docgen_add_parameter_node_add_one'),
          '#ajax' => array(
            'callback' => 'devconnect_docgen_add_parameter_node_callback',
            'wrapper' => 'api-parameters',
            'progress' => array(
              'type' => 'none'
            )
          ),
          '#limit_validation_errors' => array(array('title')),
          '#attributes' => array(
            'class' => array(
              'add-parameter'
            )
          ),
        );
        $form['parameter_container']['remove_parameter'] = array(
          '#type' => 'submit',
          '#value' => t('Remove Parameter'),
          '#submit' => array('devconnect_docgen_remove_parameter_node_remove_one'),
          '#ajax' => array(
            'callback' => 'devconnect_docgen_remove_parameter_node_callback',
            'wrapper' => 'api-parameters',
            'progress' => array(
              'type' => 'none'
            )
          ),
          '#limit_validation_errors' => array(array('title')),
          '#attributes' => array(
            'class' => array(
              'add-parameter'
            )
          ),
        );
      }
    }
    if (!empty($form['field_' . $mid . 'payload'][LANGUAGE_NONE][0]['value']['#default_value'])) {
      $header_params = drupal_json_decode($form['field_' . $mid . 'payload'][LANGUAGE_NONE][0]['value']['#default_value']);
      foreach($header_params['representation'] as $hparams) {
        if (isset($hparams['payload'])) {
          $form['apigee_content_container']['parameters']['#default_value'] = trim($hparams['payload']['content']);
        }
      }
    }
  }
}

/**
 * Makes parameters array
 */
function _devconnect_docgen_make_params(&$parameters, &$form, &$header, &$form_state, $empty = FALSE) {
  if ($empty) {
    $parameters = array(
      array(
        'name' => '',
        'required' => '',
        'type' => '',
        'style' => '',
        'description' => '',
      )
    );
  }
  foreach ($parameters as $key => $params) {
    foreach($params as $title => $param) {
      $header[$title] = t($title);
      $form['parameter_container']['parameters'][$key][$title] = array(
        '#type' => 'textarea',
        '#default_value' => $param,
        '#size' => 30,
        '#rows' => 5,
      );
    }
  }
}

/*
 * AJAX Callback for remove_parameter
 */
function devconnect_docgen_remove_parameter_node_callback($form, &$form_state) {
  return $form['parameter_container']['parameters'];
}

/**
 * Submit Callback for remove_parameter
 *
 * @param $form
 * @param $form_state
 * @return mixed
 */
function devconnect_docgen_remove_parameter_node_remove_one($form, &$form_state) {
  $form_state['storage']['ajaxed'] = TRUE;
  foreach($form['parameter_container']['parameters'] as $key => $value) {
    if (is_numeric($key)) {
      $form_state['storage']['remove']++;
    }
  }
  $form_state['storage']['num_parameters']--;
  $form_state['storage']['remove'] = $form_state['storage']['remove'] - 1;
  if ($form_state['storage']['remove'] == 0) {
    unset($form_state['storage']['remove']);
  }
  $form_state['rebuild'] = TRUE;
}

/**
 * Ajax Callback for add_parameter
 *
 * @param $form
 * @param $form_state
 * @return mixed
 */
function devconnect_docgen_add_parameter_node_callback($form, $form_state) {
  return $form['parameter_container']['parameters'];
}

/*
 * Submit Callback for add_parameter
 */
function devconnect_docgen_add_parameter_node_add_one($form, &$form_state) {
  $form_state['storage']['ajaxed'] = TRUE;
  $form_state['storage']['num_parameters']++;
  if (isset($form_state['storage']['remove'])) {
    unset($form_state['storage']['remove']);
  }
  $form_state['rebuild'] = TRUE;
}

/**
 * Implements theme_hook_theme()
 */
function theme_devconnect_docgen_param_table($vars) {
  $element = $vars['element'];
  $rows = array();
  foreach (element_children($element) as $key) {
    $row = array();
    $row['data'] = array();
    foreach ($element['#header'] as $fieldname => $title) {
      $row['data'][] = drupal_render($element[$key][$fieldname]);
    }
    $rows[] = $row;
  }
  return theme(
    'table', array(
      'header' => $element['#header'],
      'rows' => $rows,
    )
  );
}

/**
 * Implements hook_node_validate()
 */
function devconnect_docgen_node_validate($node, $form, &$form_state) {
  $models = variable_get('devconnect_docgen_content_types', array());
  if (in_array($node->type, array_keys($models))) {
    $json = array(
      'params' => $form_state['values']['parameters'],
    );
    foreach($form_state['values']['parameters'] as $key => $parameter) {
      if (empty($parameter['name'])) {
        unset($form_state['values']['parameters'][$key]);
      }
    }
    if (empty($form_state['values']['parameters'])) {
      $form_state['values']['field_' . $node->type . '_model_requestjson']['und'][0]['value'] = '';
    } else {
      $form_state['values']['field_' . $node->type . '_model_requestjson']['und'][0]['value'] = drupal_json_encode($json);
    }
  }
}

/**
 * Implements preprocess_page()
 *
 * @param $vars
 */
function devconnect_docgen_preprocess_page(&$vars) {
  $models = variable_get('devconnect_docgen_content_types', array());
  if (isset($vars['node']) && in_array($vars['node']->type, array_keys($models))) {
    $vars['dna'] = TRUE;

    $breadcrumb = array();
    $breadcrumb[] = l('Home', '<front>');
    $breadcrumb[] = l($models[$vars['node']->type] . ' Documentation', $vars['node']->type . '/apis');

    drupal_set_breadcrumb($breadcrumb);
  }
}


/**
 * Implements preprocess_node()
 *
 * @param $vars
 */
function devconnect_docgen_preprocess_node(&$vars) {

  $models = variable_get('devconnect_docgen_content_types', array());
  if (in_array($vars['type'], array_keys($models))) {
    $mid = $vars['type'];
    if (isset($vars['field_' . $mid . '_model_requestjson'])) {
      if (!$vars['teaser']) {
        $parameters = drupal_json_decode('[]');
        if (isset($vars['field_' . $mid . '_model_requestjson'][0])) {
          if (!is_array($vars['field_' . $mid . '_model_requestjson'][0]['value'])) {
            $parameters = drupal_json_decode($vars['field_' . $mid . '_model_requestjson'][0]['value']);
          }
        }
        if (!empty($parameters)) {
          $header = array();
          $rows = array();
          $data = array();
          if (isset($parameters['params'])) {
            foreach($parameters['params'] as $params) {
              foreach($params as $type => $param) {
                $header[$type] = $type;
                $data[$type] = $param;
              }
              $rows[] = $data;
            }
            $header = array_keys($header);
            $vars['content']['field_' . $mid . '_model_requestjson'][0]['#markup'] = theme(
              'table', array(
                'header' => $header,
                'rows'=> $rows
              )
            );
          } else {
            $vars['content']['field_' . $mid . '_model_requestjson'][0]['#markup'] = '';
          }
        } else {
          $vars['content']['field_' . $mid . '_model_requestjson'][0]['#markup'] = '';
        }
      }
    }
    $vars['display_submitted'] = FALSE;
    $model = variable_get('use_docgen_templates', 1);
    if (!(bool)$model) {
      // make schemes a pretty var
      $schemes = array();
      if (isset($vars['field_' . $vars['type'] . '_model_authschemes']) && !empty($vars['field_' . $vars['type'] . '_model_authschemes'])) {
        foreach ($vars['field_' . $vars['type'] . '_model_authschemes'] as $scheme) {
          $schemes[] = $scheme['taxonomy_term']->name;
        }
      }
      if (!empty($schemes)) {
        $vars['schemes'] = implode(', ', $schemes);
      }
      // make types a pretty var
      $types = array();
      if (isset($vars['field_' . $vars['type'] . '_model_types']) && !empty($vars['field_' . $vars['type'] . '_model_types'])) {
        foreach ($vars['field_' . $vars['type'] . '_model_types'] as $type) {
          $types[] = $type['taxonomy_term']->name;
        }
      }
      if (!empty($types)) {
        $vars['types'] = implode(', ', $types);
      }

      // make tags a pretty var
      $tags = array();
      if (isset($vars['field_' . $vars['type'] . '_model_tags']) && !empty($vars['field_' . $vars['type'] . '_model_tags'])) {
        foreach ($vars['field_' . $vars['type'] . '_model_tags'] as $tag) {
          $tags[] = $tag['taxonomy_term']->name;
        }
      }
      if (!empty($tags)) {
        $vars['terms'] = implode(', ', $tags);
      }

      // Friendly Date
      $vars['date'] = date('j F, Y', $vars['node']->created);

      // Verb
      if (isset($vars['field_' . $vars['type'] . '_model_verb']) && !empty($vars['field_' . $vars['type'] . '_model_verb'])) {
        $vars['verb'] = $vars['field_' . $vars['type'] . '_model_verb'][0]['taxonomy_term']->name;
      }

      // Description
      $vars['description'] = (isset($vars['body'][0]['value']) && !empty($vars['body'][0]['value'])) ? $vars['body'][0]['value'] : '';

      // Path
      if (isset($vars['field_' . $vars['type'] . '_model_path']) && !empty($vars['field_' . $vars['type'] . '_model_path'][0]['value'])) {
        $vars['path'] = $vars['field_' . $vars['type'] . '_model_path'][0]['value'];
      }

      $vars['content']['#markup'] = theme('operation_template', $vars);
    } else {
      if (!$vars['teaser']) {
        $templates = entity_get_controller('docgen_doc');

        $config =& Drupal::config('devconnect.settings');
        $endpoint = $config->get('endpoint');
        $url = parse_url($endpoint);

        drupal_add_js(
          array(
            'devconnect_docgen' => array(
              'isDrupal' => TRUE,
              'apiModelBaseUrl' => $url['scheme'] . '://' . $url['host'],
            )
          ), 'setting');

        $cache = cache_get($vars['type'] . '_' . $vars['node']->nid, 'cache_docgen');
        $data = drupal_json_decode($cache->data);

        $data = $templates->loadOperation($data, $vars['type']);

        $vars['content'] = array();
        $vars['content']['#markup'] = $data;
      } else {
        $vars['content']['#markup'] = theme('operation_template', $vars);
      }
    }
    if ($vars['teaser']) {
      $vars['title'] = '';
    }
  }
}