<?php

/**
 * Adds a method to a given resource
 *
 * @param $form
 * @param $form_state
 * @param $mid
 * @param $rev
 * @param $res
 * @return mixed
 */
function smartdocs_method_add($form, $form_state, $mid, $rev, $res) {
  module_load_include('inc', 'smartdocs', 'admin/smartdocs.methods.examples');
  $model = entity_get_controller('smartdocs_model')->loadSingle($mid);
  drupal_set_title('Edit Method in ‘' . $model['displayName'] . '’ model');
  $version = variable_get('local_asset_version', '3');
  drupal_add_js(drupal_get_path('module', 'smartdocs') . '/local/js/v' . $version . '/codemirror.js');
  drupal_add_js(drupal_get_path('module', 'smartdocs') . '/local/js/v' . $version . '/codemirror_javascript.js');
  drupal_add_css(drupal_get_path('module', 'smartdocs') . '/local/css/v' . $version . '/codemirror.css');

  $breadcrumb = array();
  $breadcrumb[] = l('Home', '<front>');
  $breadcrumb[] = l('Administration', 'admin');
  $breadcrumb[] = l('SmartDocs', 'admin/smartdocs');
  $breadcrumb[] = l('Models', 'admin/smartdocs/models');
  $breadcrumb[] = l($model['displayName'], 'admin/smartdocs/models/' . $model['name']);
  $breadcrumb[] = l('API Revisions', 'admin/smartdocs/models/' . $model['name'] . '/revisions/');
  $breadcrumb[] = l('#' . $rev, 'admin/smartdocs/models/' . $model['name'] . '/revisions/' . $rev);
  drupal_set_breadcrumb($breadcrumb);

  $form['model'] = array(
    '#type' => 'hidden',
    '#value' => $model,
  );
  $form['rev'] = array(
    '#type' => 'hidden',
    '#value' => $rev,
  );
  $form['res'] = array(
    '#type' => 'hidden',
    '#value' => $res,
  );
  $form['display_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Display Name'),
    '#size' => 60,
    '#maxlength' => 128,
    '#default_value' => (isset($method['displayName'])) ? $method['displayName'] : '',
  );
  $form['description'] = array(
    '#type' => 'text_format',
    '#title' => t('Description'),
    '#rows' => 5,
    '#default_value' => (isset($method['description'])) ? $method['description'] : '',
  );
  $form['verb'] = array(
    '#type' => 'select',
    '#title' => t('Method Verb'),
    '#options' => array(
      'DELETE' => 'DELETE',
      'GET' => 'GET',
      'PATCH' => 'PATCH',
      'POST' => 'POST',
      'PUT' => 'PUT'
    ),
    '#default_value' => (isset($method['verb'])) ? array($method['verb']) : '',
  );
  $schemes = array();
  if (isset($method['authSchemes'])) {
    foreach ($method['authSchemes'] as $scheme) {
      $schemes[] = $scheme;
    }
  }
  $form['authschemes'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Authentication'),
    '#options' => array(
      'NOAUTH' => 'NOAUTH',
      'BASICAUTH' => 'BASICAUTH',
      'CUSTOM' => 'CUSTOM',
      'OAUTH2WEBSERVER' => 'OAUTH2WEBSERVER'
    ),
    '#multiple' => TRUE,
    '#default_value' => $schemes,
    '#required' => TRUE,
  );
  $form['contentType'] = array(
    '#type' => 'select',
    '#title' => t('Content Type'),
    '#options' => array(
      'application/jsonp' => 'application/jsonp',
      'application/json' => 'application/json',
      'application/json, text/xml' => 'application/json, text/xml',
      'text/xml' => 'text/xml'
    ),
  );
  $types = array(
    'parameters' => t('Parameters'),
    'body-doc' => t('Request Body Documentation'),
    'body-example' => t('Request Body Example'),
    'tags' => t('Tags'),
  );
  foreach ($types as $type => $display) {
    $form['field_' . $type] = array(
      '#type' => 'fieldset',
      '#title' => $display,
      '#prefix' => '<div id="' . $type . '">',
      '#suffix' => '</div>',
    );
    switch ($type) {
      case 'body-doc':
        $output = (isset($method['body']['doc'])) ? $method['body']['doc'] : '';
        $form['field_' . $type][$type] = array(
          '#type' => 'text_format',
          '#title' => t('Description'),
          '#rows' => 5,
          '#default_value' => $output,
        );
        break;
      case 'body-example':
        $output = (isset($method['body']['sample'])) ? $method['body']['sample'] : '';
        drupal_add_js(array('smartdocs' => array($type => strtolower($type))), 'setting');
        $form['field_' . $type][$type] = array(
          '#type' => 'textarea',
          '#title' => $display,
          '#default_value' => $output,
        );
        break;
      default:
        $output = (isset($method[$type])) ? json_encode($method[$type]) : '';
        $output = _smartdocs_prettyprint($output);
        drupal_add_js(array('smartdocs' => array($type => strtolower($type))), 'setting');
        $form['field_' . $type][$type] = array(
          '#type' => 'textarea',
          '#title' => $display,
          '#default_value' => $output,
        );
        break;
    }
    $example = smartdocs_method_examples($type);
    if (!empty($example)) {
      $form['field_' . $type]['examples'] = array(
        '#type' => 'fieldset',
        '#title' => t('Examples'),
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
      );
      $form['field_' . $type]['examples']['ex'] = array(
        '#markup' => $example,
      );
    }
  }
  $form['submit'] = array(
    '#type' => 'submit',
    '#submit' => array('smartdocs_method_add_submit'),
    '#value' => t('Add Method'),
  );

  drupal_add_js(drupal_get_path('module', 'smartdocs') . '/js/smartdocs.js');

  return $form;
}

/**
 * Implements hook_validate().
 */
function smartdocs_method_add_validate($form, &$form_state) {
  $json_error = t('Please specify proper JSON values for the ');
  if (!empty($form_state['values']['parameters'])) {
    if (smartdocs_json_validate($form_state['values']['parameters'])) {
      form_set_error('parameters', $json_error . 'parameters');
    }
  }
  if (!empty($form_state['values']['parameterGroups'])) {
    if (smartdocs_json_validate($form_state['values']['parameterGroups'])) {
      form_set_error('parameterGroups', $json_error . 'parameter groups.');
    }
  }
  if (!empty($form_state['values']['tags'])) {
    if (smartdocs_json_validate($form_state['values']['tags'])) {
      form_set_error('tags', $json_error . 'tags.');
    }
  }
}

/**
 * Submit callback for Add Resource form
 *
 * @param $form
 * @param $form_state
 */
function smartdocs_method_add_submit($form, &$form_state) {
  $schemes = array();
  foreach ($form_state['values']['authschemes'] as $key => $auth) {
    if (is_string($auth)) {
      $schemes[] = $key;
    }
  }

  $parameters = drupal_json_decode($form_state['values']['parameters']);

  $payload = array(
    'name' => preg_replace('/[^A-Za-z0-9]/', "", $form_state['values']['display_name']),
    'displayName' => $form_state['values']['display_name'],
    'description' => $form_state['values']['description']['value'],
    'verb' => $form_state['values']['verb'],
    'parameters' => (!empty($parameters)) ? $parameters : array(),
    'authSchemes' => $schemes,
    'tags' => drupal_json_decode($form_state['values']['tags']),
  );
  if (isset($form_state['values']['body-example'])) {
    $payload['body']['contentType'] = $form_state['values']['contentType'];
    $payload['body']['sample'] = $form_state['values']['body-example'];
  }
  if (isset($form_state['values']['body-doc'])) {
    $payload['body']['doc'] = $form_state['values']['body-doc']['value'];
  }
  $mid = $form_state['values']['model']['name'];
  $rev = $form_state['values']['rev'];
  $res = $form_state['values']['res'];
  if ($create = entity_get_controller('smartdocs_method')->createMethod($mid, $rev, $res, $payload)) {
    drupal_set_message('Method created successfully', 'status');
    $form_state['redirect'] = 'admin/smartdocs/models/' . $mid . '/revisions/' . $rev;
  }
  else {
    drupal_set_message('Method creation failed. ' . $create, 'error');
  }
}

/**
 * Adds a method to a given resource
 *
 * @param $form
 * @param $form_state
 * @param $mid
 * @param $rev
 * @param $res
 * @param $methodId
 * @return mixed
 */
function smartdocs_method_edit($form, $form_state, $mid, $rev, $res, $methodId) {
  module_load_include('inc', 'smartdocs', 'admin/smartdocs.methods.examples');
  $model = entity_get_controller('smartdocs_model')->loadSingle($mid);
  drupal_set_title('Edit Method in ‘' . $model['displayName'] . '’ model');
  $version = variable_get('local_asset_version', '3');
  drupal_add_js(drupal_get_path('module', 'smartdocs') . '/local/js/v' . $version . '/codemirror.js');
  drupal_add_js(drupal_get_path('module', 'smartdocs') . '/local/js/v' . $version . '/codemirror_javascript.js');
  drupal_add_css(drupal_get_path('module', 'smartdocs') . '/local/css/v' . $version . '/codemirror.css');
  $resource = entity_get_controller('smartdocs_resource')->loadResource($mid, $rev, $res);
  $method = entity_get_controller('smartdocs_method')->getMethod($mid, $rev, $res, $methodId);

  $breadcrumb = array();
  $breadcrumb[] = l('Home', '<front>');
  $breadcrumb[] = l('Administration', 'admin');
  $breadcrumb[] = l('SmartDocs', 'admin/smartdocs');
  $breadcrumb[] = l('Models', 'admin/smartdocs/models');
  $breadcrumb[] = l($model['displayName'], 'admin/smartdocs/models/' . $model['name']);
  $breadcrumb[] = l('API Revisions', 'admin/smartdocs/models/' . $model['name'] . '/revisions/');
  $breadcrumb[] = l('#' . $rev, 'admin/smartdocs/models/' . $model['name'] . '/revisions/' . $rev);
  drupal_set_breadcrumb($breadcrumb);

  // We need to evaluate if there is node information.
  $nid = db_query('SELECT nid FROM {smartdata} WHERE method = :method', array(':method' => $method['id']))->fetchField();

  if (isset($nid)) {
    $form['node'] = array(
      '#type' => 'hidden',
      '#value' => node_load($nid)
    );
  }
  $form['model'] = array(
    '#type' => 'hidden',
    '#value' => $model
  );
  $form['rev'] = array(
    '#type' => 'hidden',
    '#value' => $rev
  );
  $form['res'] = array(
    '#type' => 'hidden',
    '#value' => $res
  );
  $form['method'] = array(
    '#type' => 'hidden',
    '#value' => $method
  );
  if (isset($method['displayName'])) {
    $display = $method['displayName'];
  }
  else {
    if (isset($method['name'])) {
      $display = $method['name'];
    }
    else {
      $display = '';
    }
  }
  $form['display_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Display Name'),
    '#size' => 60,
    '#maxlength' => 128,
    '#default_value' => $display,
  );
  $form['description'] = array(
    '#type' => 'text_format',
    '#title' => t('Description'),
    '#rows' => 5,
    '#default_value' => (isset($method['description'])) ? $method['description'] : '',
  );
  $form['verb'] = array(
    '#type' => 'select',
    '#title' => t('Method Verb'),
    '#options' => array(
      'DELETE' => 'DELETE',
      'GET' => 'GET',
      'PATCH' => 'PATCH',
      'POST' => 'POST',
      'PUT' => 'PUT'
    ),
    '#default_value' => (isset($method['verb'])) ? array($method['verb']) : '',
  );
  $schemes = array();
  if (isset($method['authSchemes'])) {
    foreach ($method['authSchemes'] as $scheme) {
      $schemes[] = $scheme;
    }
  }
  if (empty($schemes)) {
    $schemes = array('NOAUTH');
  }
  $form['authschemes'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Authentication'),
    '#options' => array(
      'NOAUTH' => 'NOAUTH',
      'BASICAUTH' => 'BASICAUTH',
      'CUSTOM' => 'CUSTOM',
      'OAUTH2WEBSERVER' => 'OAUTH2WEBSERVER'
    ),
    '#multiple' => TRUE,
    '#default_value' => $schemes,
    '#required' => TRUE,
  );
  $form['contentType'] = array(
    '#type' => 'select',
    '#title' => t('Content Type'),
    '#options' => array(
      'application/jsonp' => 'application/jsonp',
      'application/json' => 'application/json',
      'application/json, text/xml' => 'application/json, text/xml',
      'text/xml' => 'text/xml'
    ),
    '#default_value' => (isset($method['body']['contentType']) && !empty($method['body']['contentType'])) ? $method['body']['contentType'] : 'application/json',
  );
  $types = array(
    'parameters' => t('Parameters'),
    'body-doc' => t('Request Body Documentation'),
    'body-example' => t('Request Body Example'),
    'tags' => t('Tags'),
  );
  foreach ($types as $type => $display) {
    $form['field_' . $type] = array(
      '#type' => 'fieldset',
      '#title' => $display,
      '#prefix' => '<div id="' . $type . '">',
      '#suffix' => '</div>',
    );
    switch ($type) {
      case 'body-doc':
        $output = (isset($method['body']['doc'])) ? $method['body']['doc'] : '';
        $form['field_' . $type][$type] = array(
          '#type' => 'text_format',
          '#title' => t('Description'),
          '#rows' => 5,
          '#default_value' => $output,
        );
        break;
      case 'body-example':
        $output = (isset($method['body']['sample'])) ? $method['body']['sample'] : '';
        $form['field_' . $type][$type] = array(
          '#type' => 'textarea',
          '#title' => $display,
          '#default_value' => $output,
        );
        drupal_add_js(array('smartdocs' => array($type => strtolower($type))), 'setting');
        break;
      default:
        if ($type == 'parameters') {
          if (isset($resource['parameters'])) {
            foreach ($resource['parameters'] as $key => $res_param) {
              unset($method['parameters'][$key]);
            }
            if (empty($method[$type])) {
              if (isset($method['parameters'])) {
                unset($method['parameters']);
              }
            }
            if (array_key_exists('parameters', $method) && is_array($method['parameters'])) {
              $method['parameters'] = array_values($method['parameters']);
            }
          }
        }
        $output = (isset($method[$type])) ? json_encode($method[$type], 128) : '';
        $form['field_' . $type][$type] = array(
          '#type' => 'textarea',
          '#title' => $display,
          '#default_value' => $output,
        );
        drupal_add_js(array('smartdocs' => array($type => strtolower($type))), 'setting');
        break;
    }
    $example = smartdocs_method_examples($type);
    if (!empty($example)) {
      $form['field_' . $type]['examples'] = array(
        '#type' => 'fieldset',
        '#title' => t('Examples'),
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
      );
      $form['field_' . $type]['examples']['ex'] = array(
        '#markup' => $example,
      );
    }
  }
  $form['submit'] = array(
    '#type' => 'submit',
    '#submit' => array('smartdocs_method_edit_submit'),
    '#value' => t('Update Method'),
  );

  drupal_add_js(drupal_get_path('module', 'smartdocs') . '/js/smartdocs.js');

  return $form;
}

/**
 * Implements hook_validate().
 */
function smartdocs_method_edit_validate($form, &$form_state) {
  $json_error = t('Please specify proper JSON values for the ');
  if (!empty($form_state['values']['parameters'])) {
    if (smartdocs_json_validate($form_state['values']['parameters'])) {
      form_set_error('parameters', $json_error . 'parameters');
    }
  }
  if (!empty($form_state['values']['parameterGroups'])) {
    if (smartdocs_json_validate($form_state['values']['parameterGroups'])) {
      form_set_error('parameterGroups', $json_error . 'parameter groups.');
    }
  }
  if (!empty($form_state['values']['tags'])) {
    if (smartdocs_json_validate($form_state['values']['tags'])) {
      form_set_error('tags', $json_error . 'tags.');
    }
  }
}

/**
 * Implements hook_form_submit().
 *
 * @param $form
 * @param $form_state
 */
function smartdocs_method_edit_submit($form, &$form_state) {
  $method = $form_state['values']['method'];
  $mid = $form_state['values']['model']['name'];
  $rev = $form_state['values']['rev'];
  $res = $form_state['values']['res'];
  $method['displayName'] = $form_state['values']['display_name'];
  $method['description'] = $form_state['values']['description']['value'];
  $method['verb'] = $form_state['values']['verb'];
  if (!empty($form_state['values']['authschemes'])) {
    $method['authSchemes'] = array();
    $i = 0;
    foreach ($form_state['values']['authschemes'] as $auth => $active) {
      $method['authSchemes'][$i] = $active;
      if ($active == '0') {
        unset($method['authSchemes'][$i]);
      }
      $i++;
    }
  }
  if (!empty($form_state['values']['parameters'])) {
    $method['parameters'] = drupal_json_decode($form_state['values']['parameters']);
  }
  else {
    $method['parameters'] = array();
  }
  if (!empty($form_state['values']['parameterGroups'])) {
    $method['parameterGroups'] = drupal_json_decode($form_state['values']['parameterGroups']);
  }
  if (!empty($form_state['values']['body-doc'])) {
    $method['body']['doc'] = $form_state['values']['body-doc']['value'];
  }
  if (!empty($form_state['values']['body-example'])) {
    $method['body']['contentType'] = $form_state['values']['contentType'];
    $method['body']['sample'] = $form_state['values']['body-example'];
  }
  if (!empty($form_state['values']['tags'])) {
    $method['tags'] = drupal_json_decode($form_state['values']['tags']);
  }
  if (!empty($method['customAttributes'])) {
    $custom = array();
    foreach ($method['customAttributes'] as $key => $attribute) {
      if (empty($method['customAttributes'][$key]['value'])) {
        $method['customAttributes'][$key]['value'] = "[ ]";
      }
    }
  }
  unset($method['createdTime']);
  unset($method['modifiedTime']);
  $update = entity_get_controller('smartdocs_method')->updateMethod($mid, $rev, $res, $method['id'], $method);
  if (isset($update['id'])) {
    $node = $form_state['values']['node'];
    if (isset($node->nid)) {
      module_load_include('inc', 'smartdocs', 'batch/smartdocs.render');
      $smart_mid = smartdocs_model_name($mid);
      $smart_field = smartdocs_field_name($mid);
      $verbose = array();
      $verbose['revisionNumber'] = $form_state['values']['rev'];
      $resource = entity_get_controller('smartdocs_resource')->loadResource($mid, $rev, $res);
      smartdocs_update_method_node($update, $mid, $smart_mid, $smart_field, $node->status, $verbose, $resource, $context);
      drupal_set_message('The method ‘' . $update['displayName'] . '’ has been updated along with the associated node.', 'status');
      $path = drupal_get_path_alias('node/' . $node->nid);
      $form_state['redirect'] = array(
        $path,
        array(
          'query' => array(
            'flush' => 1,
          )
        )
      );
    }
    else {
      drupal_set_message('The method ‘' . $update['displayName'] . '’ has been updated.', 'status');
      $form_state['redirect'] = 'admin/smartdocs/models/' . $mid . '/revisions/' . $rev;
    }
  }
  else {
    drupal_set_message('There was an error updating the method.  Please consult the logs for more details.', 'error');
  }
}

/**
 * Checks if given string is proper json.
 *
 * @param $json
 * @return bool
 */
function smartdocs_json_validate($json) {
  json_decode('{' . $json . '}');
  return (json_last_error() == JSON_ERROR_NONE);
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
function smartdocs_update_method_node($method, $mid, $smart_mid, $smart_field, $publish, $verbose, $resource, &$context) {

  $nid = db_query('SELECT nid FROM {smartdata} WHERE method = :method', array(
    ':method' => $method['id'],
  ))->fetchField();

  $entities = entity_load('node', FALSE, array('type' => $smart_mid, 'nid' => $nid));
  $loaded = TRUE;
  $entity = current($entities);
  $entity->revision = 1;
  $entity->log = t('Revision: @rev - Method: @title - @method was synced from SmartDocs.', array(
    '@rev' => $verbose['revisionNumber'],
    '@title' => $method['displayName'],
    '@method' => $method['id']
  ));

  $tag_vid = taxonomy_vocabulary_machine_name_load($smart_mid . '_tags')->vid;

  // create tag if it doesn't exist
  if (is_array($method['tags']) && !empty($method['tags'])) {
    foreach ($method['tags'] as $tag) {
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

  $wrapper = entity_metadata_wrapper('node', $entity);
  if (isset($method['displayName'])) {
    $wrapper->title->set($method['displayName']);
    if ($loaded) {
      $context['message'] = t('Syncing node: ' . $method['displayName']);
    }
    else {
      $context['message'] = t('Creating node: ' . $method['displayName']);
    }
  }
  else {
    $wrapper->title->set($method['name']);
    if ($loaded) {
      $context['message'] = t('Syncing node: ' . $method['name']);
    }
    else {
      $context['message'] = t('Creating node: ' . $method['name']);
    }
  }
  if (isset($method['description'])) {
    $wrapper->body->set(array('value' => $method['description']));
  }
  else {
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
    foreach ($method['parameters'] as $parameter) {
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
    foreach ($types as $type) {
      $field =& $wrapper->$field_name;
      $field[$i]->set(intval($type));
      $i++;
    }
  }
  else {
    $terms = taxonomy_get_term_by_name('n/a', $smart_mid . '_types');
    foreach ($terms as $term) {
      $field =& $wrapper->$field_name;
      $field[$i]->set(intval($term->tid));
      $i++;
    }
  }
  foreach ($method as $title => $field) {
    $tags = array();
    $auths = array();

    if (is_array($method['tags']) && !empty($method['tags'])) {
      foreach ($method['tags'] as $tag) {
        $terms = taxonomy_get_term_by_name($tag, $smart_mid . '_tags');
        foreach ($terms as $term) {
          if ($term->vocabulary_machine_name == $smart_mid . '_tags') {
            $tags[] = $term->tid;
          }
        }
      }
    }
    if (is_array($method['authSchemes']) && !empty($method['authSchemes'])) {
      foreach ($method['authSchemes'] as $scheme) {
        $terms = taxonomy_get_term_by_name($scheme, $smart_mid . '_auth_schemes');
        foreach ($terms as $term) {
          if ($term->vocabulary_machine_name == $smart_mid . '_auth_schemes') {
            $auths[] = $term->tid;
          }
        }
      }
    }
    switch ($title) {
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
        foreach ($auths as $auth) {
          $field =& $wrapper->$field_name;
          $field[$i]->set(intval($auth));
          $i++;
        }
        break;
      case 'tags':
        $field_name = 'field_' . $smart_field . '_model_' . strtolower($title);
        $i = 0;
        foreach ($tags as $tag) {
          $field =& $wrapper->$field_name;
          $field[$i]->set(intval($tag));
          $i++;
        }
        break;
      default:
        break;
    }
  }
  $field_name = 'field_' . $smart_field . '_model_path';
  $wrapper->$field_name = $resource['path'];
  if ($wrapper->save()) {
    $node = $wrapper->value();
    if ($node->nid) {
      db_update('smartdata')
        ->fields(array('synced' => 1))
        ->condition('nid', $node->nid)
        ->execute();
      cache_clear_all($node->nid, 'cache_smartdocs');
    }
  }
}

