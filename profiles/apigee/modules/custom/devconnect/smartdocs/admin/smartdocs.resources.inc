<?php

/**
 * Page Callback for admin/smartdocs/model/%name
 *
 * @param $form
 * @param $form_state
 * @param $mid
 * @return mixed
 */
function smartdocs_resource_detail($form, &$form_state, $mid, $rev) {

  drupal_add_css(drupal_get_path('module', 'views') . '/css/views-admin.ctools.css');
  drupal_add_css(drupal_get_path('module', 'smartdocs') . '/css/smartdocs.css');

  $model = entity_get_controller('smartdocs_model')->loadSingle($mid);

  if (isset($form_state['render_options']) && $form_state['render_options']) {
    $selected = $form_state['values']['api_model_table'];
    $verbose = entity_get_controller('smartdocs_revision')->loadVerbose($mid, $rev);
    return smartdocs_render_confirmation_form($form, $form_state, $selected, $verbose, $model);
  }

  $model['revisions'][$rev] = entity_get_controller('smartdocs_revision')->loadVerbose($mid, $rev);

  drupal_set_title('`' . $model['displayName'] . '` API Revision #' . $rev);
  $crumb = drupal_get_breadcrumb();
  $crumb[] = '#' . $rev;
  drupal_set_breadcrumb($crumb);

  $operations_options['render_nodes'] = t('Render Nodes');
  if (!empty($nodes)) {
    $operations_options['publish_nodes'] = t('Publish Nodes');
    $operations_options['unpublish_nodes'] = t('Unpublish Nodes');
    $operations_options['sync_nodes'] = t('Sync Nodes to SmartDocs');
  }

  $operations = array(
    'resource/add/' . $rev => 'Add Resource',
    'revisions/' . $rev  . '/auth' => 'Auth Settings',
    'export/' . $rev => 'Export',
    'revisions' => 'View All Revisions',
    'revisions/' . $rev . '/edit' => 'Edit API Revision',
  );
  $links = array();
  foreach($operations as $key => $op) {
    $links[$key] = array(
      'title' => $op,
      'href' => 'admin/smartdocs/models/' . $mid .'/' . $key,
      'attributes' => array(
        'title' => $op,
        'class' => array(
          'views-ajax-link'
        ),
      ),
    );
  }
  $form['revision_tools'] = array(
    '#markup' => theme('links__ctools_dropbutton',
        array(
          'links' => $links,
        )
      ) . '<br/><br/>',
  );

  $form['options'] = array(
    '#type' => 'fieldset',
    '#title' => t('Update options'),
    '#attributes' => array('class' => array('container-inline')),
    '#access' => array('administer smartdocs content'),
  );
  $form['options']['operation'] = array(
    '#type' => 'select',
    '#title' => t('Operation'),
    '#title_display' => 'invisible',
    '#options' => $operations_options,
    '#default_value' => 'approve',
  );
  $form['options']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Update'),
  );
  $header = array(
    'path' => t('Resource Path'),
    'tax' => t('Taxonomy'),
    'name' => t('Name'),
    'description' => t('Description'),
    'method' => t('Method'),
    'auth' => t('Authentication'),
    'node' => t('Node Association'),
    'status' => t('Published'),
    'synced' => t('Synced'),
    'operations' => array(
      'data' => t('Operations'),
      'class' => array(
        'api-model-table-operations'
      )
    )
  );
  if (isset($model['revisions'])) {
    if (isset($model['displayName'])) {
      $entity['displayName'] = $model['displayName'];
    }
    $entity['name'] = $model['name'];
    $form['verbose'] = array(
      '#type' => 'hidden',
      '#value' => drupal_json_encode($model['revisions'][$rev]),
    );
    $form['mid'] = array(
      '#type' => 'hidden',
      '#value' => $mid,
    );
    $resources = $model['revisions'][$rev]['resources'];
    $options = array();
    $resource_header = array(
      'name' => t('Name'),
      'base' => t('Base URL'),
      'path' => t('Path'),
      'ops' => t('Operations')
    );
    if (!empty($resources)) {
      $form['resources'] = array(
        '#type' => 'fieldset',
        '#title' => 'Resources',
        '#collapsed' => TRUE,
        '#collapsible' => TRUE,
      );
    }
    foreach($resources as $resource) {
      $resource_row = array();
      $resource_row['name'] = $resource['name'];
      $resource_row['base'] = $resource['baseUrl'];
      $resource_row['path'] = $resource['path'];
      $operations = array(
        'resource/edit/' . $rev . '/' . $resource['id'] => 'Edit Resource',
        'method/add/' . $rev . '/' . $resource['id'] => 'Add Method',
      );
      $links = array();
      foreach($operations as $key => $op) {
        $links[$resource['id'] . '-' . $key] = array(
          'title' => $op,
          'href' => 'admin/smartdocs/models/' . $mid .'/' . $key,
          'attributes' => array(
            'title' => $op,
            'class' => array(
              'views-ajax-link'
            ),
          ),
        );
      }
      $resource_row['ops'] = theme('links__ctools_dropbutton',
        array(
          'links' => $links,
        )
      );
      $resource_rows[] = $resource_row;
      if (isset($resource['methods']) && is_array($resource['methods'])) {
        foreach ($resource['methods'] as $uuid => $method) {
          $nid = db_query("SELECT nid FROM {smartdocs} WHERE method = :uuid", array(':uuid' => $method['id']))->fetchField();
          $synced = 0;
          $node = 0;
          $status = 0;
          if (!empty($nid)) {
            $text = t('NID: %nid', array('%nid' => $nid));
            $link = l(t('NID: %nid', array('%nid' => $nid)), 'node/' . $nid, array(
              'html' => TRUE,
              'attributes' => array(
                'target' => '_blank',
              )
            ));
            $node = 1;
            $status = db_query("SELECT status FROM {node} WHERE nid = :nid", array(':nid' => $nid))->fetchField();
            if(isset($nid)) {
              $synced = db_query("SELECT synced FROM {smartdocs} WHERE nid = :nid", array(':nid' => $nid))->fetchField();
            }
          }
          $operations = array(
            'method/edit/' . $rev . '/' . $resource['id'] . '/' . $method['id'] => 'Edit Method',
            'resource/edit/' . $rev . '/' . $resource['id'] => 'Edit Parent Resource',
          );
          $tablelinks = array();
          foreach($operations as $key => $op) {
            $tablelinks[$key] = array(
              'title' => $op,
              'href' => 'admin/smartdocs/models/' . $model['name'] . '/' . $key,
              'attributes' => array(
                'title' => $op,
                'class' => array(
                  $key,
                  'views-ajax-link'
                ),
              ),
            );
          }
          $options[$method['id']] = array(
            'path' => array(
              'data' => (isset($method['path'])) ? $method['path'] : '',
              'class' => 'word-wrap'
            ),
            'tax' => array(
              'data' => (isset($method['tags']) && !empty($method['tags'])) ? implode(', ', $method['tags']) : '',
              'class' => 'word-wrap'
            ),
            'name' => array(
              'data' => (isset($method['displayName'])) ? check_plain($method['displayName']) : check_plain($method['name']),
              'class' => 'word-wrap'
            ),
            'description' => array(
              'data' => (isset($method['description'])) ? check_plain($method['description']) : '',
              'class' => 'word-wrap'
            ),
            'method' => array(
              'data' => (isset($method['verb'])) ? check_plain($method['verb']) : '',
              'class' => 'word-wrap'
            ),
            'auth' => array(
              'data' => (isset($method['authSchemes']) && !empty($method['authSchemes'])) ? implode(', ', $method['authSchemes']) : '',
              'class' => 'word-wrap'
            ),
            'node' => array(
              'data' => ((bool)$node) ? $link : '<span style="color:red;">No</span>',
              'class' => 'word-wrap'
            ),
            'status' => array(
              'data' => ((bool)$status) ? '<span style="color:green;">Yes</span>' : '<span style="color:red;">No</span>',
              'class' => 'word-wrap'
            ),
            'synced' => array(
              'data' => ((bool)$synced) ? '<span style="color:green;">Yes</span>' : '<span style="color:red;">No</span>',
              'class' => 'word-wrap'
            ),
            'operations' => array(
              'data' => theme('links__ctools_dropbutton',
                array(
                  'links' => $tablelinks,
                )
              ),
              'class' => 'word-wrap'
            )
          );
        }
      }
    }
    if (!empty($resource_rows)) {
      $form['resources'][$rev] = array(
        '#markup' => theme('table', array(
            'header' => $resource_header,
            'rows' => $resource_rows )
        )
      );
    }
    $form['methods'] = array(
      '#type' => 'fieldset',
      '#title' => 'Methods',
      '#collapsed' => FALSE,
      '#collapsible' => TRUE,
    );
    $form['methods']['api_model_table'] = array(
      '#type' => 'tableselect',
      '#header' => $header,
      '#options' => $options,
      '#empty' => (empty($resources)) ? t('No Methods or Resources found') : t('No Methods found'),
      '#attributes' => array(
        'class' => array(
          'api-model-table'
        )
      )
    );
  } else {
    $form['resources'] = array(
      '#type' => 'fieldset',
      '#title' => 'Resources',
      '#collapsed' => TRUE,
      '#collapsible' => TRUE,
    );
    $form['methods'] = array(
      '#type' => 'fieldset',
      '#title' => 'Methods',
      '#collapsed' => FALSE,
      '#collapsible' => TRUE,
    );
    $form['methods']['api_model_table'] = array(
      '#type' => 'tableselect',
      '#header' => $header,
      '#options' => array(),
      '#empty' => 'There is currently no data into the model.'
    );
  }

  return $form;
}

/**
 * Resource, Method table submit
 *
 * @param $form
 * @param $form_state
 */
function smartdocs_resource_detail_submit($form, &$form_state) {
  if (isset($form_state['values']['operation'])) {
    switch($form_state['values']['operation']) {
      case 'render_nodes':
        $form_state['render_options'] = TRUE;
        $form_state['rebuild'] = TRUE;
        break;
      default:
        break;
    }
  } else {
    module_load_include('inc', 'smartdocs', 'batch/smartdocs.render');
    $model = $form_state['values']['model'];
    $verbose = $form_state['values']['verbose'];
    $table = $form_state['values']['selected'];
    $options = $form_state['values']['options'];
    $selected = array();
    foreach ($table as $import) {
      if (!is_null($import)) {
        $selected[] = $import;
      }
    }
    batch_set(smartdocs_render($model, $verbose, $selected, $options));
  }
}

/**
 * SmartDocs Render Confirmation Form
 *
 * @param $form
 * @param $form_state
 * @param $selected
 * @param $verbose
 * @return mixed
 */
function smartdocs_render_confirmation_form($form, $form_state, $selected, $verbose, $model) {
  $form['selected'] = array(
    '#type' => 'hidden',
    '#value' => $selected,
  );
  $form['verbose'] = array(
    '#type' => 'hidden',
    '#value' => $verbose,
  );
  $form['model'] = array(
    '#type' => 'hidden',
    '#value' => $model,
  );
  $form['options'] = array(
    '#type' => 'fieldset',
    '#title' => t('Options'),
    '#description' => t('Please configure your rendering options.'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#tree' => TRUE,
  );
  $form['options']['publish'] = array(
    '#type' => 'checkbox',
    '#title' => t('Publish Immediately'),
    '#description' => t('If selected, this will publish the node immediately'),
  );
  $form['options']['path'] = array(
    '#type' => 'radios',
    '#title' => t('Path Generation'),
    '#description' => t('Specify the path generation type.'),
    '#default_value' => 'path',
    '#options' => array(
      'path' => t('Resource Path w/ Method Type %d', array('%d' => '(default)')),
      'title' => 'Method Title',
      'uuid' => 'Method UUID',
    )
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Render Nodes')
  );
  return $form;
}

/**
 * Form that adds a resource to a given revision
 *
 * @param $form
 * @param $form_state
 * @param $mid
 * @param $rev
 */
function smartdocs_resource_add($form, $form_state, $mid, $rev) {
  $model = entity_get_controller('smartdocs_model')->loadSingle($mid);
  drupal_set_title('Add Resource to `' . $model['displayName'] . '`');
  $form['model'] = array(
    '#type' => 'hidden',
    '#value' => $model,
  );
  $form['rev'] = array(
    '#type' => 'hidden',
    '#value' => $rev,
  );
  $form['display_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Display Name'),
    '#size' => 60,
    '#maxlength' => 128,
  );
  $form['path'] = array(
    '#type' => 'textfield',
    '#title' => t('Path'),
    '#description' => t('Provide a path with dynamic values, ex: %e' , array('%e' => '/consoles/{name}')),
    '#size' => 60,
    '#maxlength' => 128,
  );
  $form['description'] = array(
    '#type' => 'textarea',
    '#title' => t('Description'),
    '#rows' => 5,
  );
  $form['parameters'] = array(
    '#prefix' => '<div id="parameters">',
    '#suffix' => '</div>',
    '#tree' => TRUE,
  );
  if (empty($form_state['num_parameters'])) {
    $form_state['num_parameters'] = 0;
  }
  for ($i = 0; $i <= $form_state['num_parameters']; $i++) {
    $form['parameters'][$i] = array(
      '#type' => 'fieldset',
      '#title' => t('Parameter #' . ($i + 1)),
    );
    $form['parameters'][$i]['required'] = array(
      '#type' => 'checkbox',
      '#title' => t('Required'),
      '#suffix' => '<br/>'
    );
    $form['parameters'][$i]['name'] = array(
      '#type' => 'textfield',
      '#title' => t('Name'),
      '#description' => t('Provide the name for this parameter.')
    );
    $form['parameters'][$i]['defaultvalue'] = array(
      '#type' => 'textfield',
      '#title' => t('Default Value'),
      '#description' => t('Provide the default value for this parameter.')
    );
    $form['parameters'][$i]['datatype'] = array(
      '#type' => 'radios',
      '#title' => t('Data Type'),
      '#options' => array(
        'int' => 'Int',
        'string' => 'String',
      ),
    );
    $form['parameters'][$i]['type'] = array(
      '#type' => 'radios',
      '#title' => t('Type'),
      '#options' => array(
        'header' => 'Header',
        'body' => 'Body',
        'query' => 'Query',
        'template' => 'Template',
      ),
    );
  }
  $form['parameters']['add'] = array(
    '#type' => 'submit',
    '#value' => t('Add Parameter'),
    '#submit' => array('smartdocs_resource_add_parameter'),
    '#limit_validation_errors' => array(),
    '#ajax' => array(
      'callback' => 'smartdocs_resource_callback',
      'wrapper' => 'parameters',
    ),
    '#suffix' => '<br/><br/>'
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#submit' => array('smartdocs_resource_add_submit'),
    '#value' => t('Add Resource'),
  );
  return $form;
}

/**
 * Edit Resource Form
 *
 * @param $form
 * @param $form_state
 * @param $mid
 * @param $rev
 * @param $res
 * @return mixed
 */
function smartdocs_resource_edit($form, $form_state, $mid, $rev, $res) {
  $resource = entity_get_controller('smartdocs_resource')->loadResource($mid, $rev, $res);
  $model = entity_get_controller('smartdocs_model')->loadSingle($mid);
  drupal_set_title('Edit Resource from `' . $model['displayName'] . '`');
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
    '#default_value' => (isset($resource['displayName'])) ? $resource['displayName'] : $resource['name'],
    '#size' => 60,
    '#maxlength' => 128,
  );
  $form['path'] = array(
    '#type' => 'textfield',
    '#title' => t('Path'),
    '#default_value' => (isset($resource['path'])) ? $resource['path'] : '',
    '#description' => t('Provide a path with dynamic values, ex: %e' , array('%e' => '/consoles/{name}')),
    '#size' => 60,
    '#maxlength' => 128,
  );
  $form['description'] = array(
    '#type' => 'textarea',
    '#default_value' => (isset($resource['description'])) ? $resource['description'] : '',
    '#title' => t('Description'),
    '#rows' => 5,
  );
  $form['parameters'] = array(
    '#prefix' => '<div id="parameters">',
    '#suffix' => '</div>',
    '#tree' => TRUE,
  );
  $count = count($resource['parameters']);
  if (empty($form_state['num_parameters'])) {
    $form_state['num_parameters'] = $count - 1;
  }
  for ($i = 0; $i <= $form_state['num_parameters']; $i++) {
    $form['parameters'][$i] = array(
      '#type' => 'fieldset',
      '#title' => t('Parameter #' . ($i + 1)),
    );
    $form['parameters'][$i]['required'] = array(
      '#type' => 'checkbox',
      '#title' => t('Required'),
      '#default_value' => (bool)$resource['parameters'][$i]['required'],
      '#suffix' => '<br/>'
    );
    $form['parameters'][$i]['name'] = array(
      '#type' => 'textfield',
      '#title' => t('Name'),
      '#default_value' => $resource['parameters'][$i]['name'],
      '#description' => t('Provide the name for this parameter.')
    );
    $form['parameters'][$i]['defaultValue'] = array(
      '#type' => 'textfield',
      '#title' => t('Default Value'),
      '#default_value' => (isset($resource['parameters'][$i]['defaultValue'])) ? $resource['parameters'][$i]['defaultValue'] : '',
      '#description' => t('Provide the default value for this parameter.')
    );
    $form['parameters'][$i]['dataType'] = array(
      '#type' => 'radios',
      '#title' => t('Data Type'),
      '#default_value' => (isset($resource['parameters'][$i]['dataType'])) ? $resource['parameters'][$i]['dataType'] : '',
      '#options' => array(
        'int' => 'Int',
        'string' => 'String',
      ),
    );
    $form['parameters'][$i]['type'] = array(
      '#type' => 'radios',
      '#title' => t('Type'),
      '#default_value' => (isset($resource['parameters'][$i]['type'])) ? $resource['parameters'][$i]['type'] : '',
      '#options' => array(
        'HEADER' => 'Header',
        'BODY' => 'Body',
        'QUERY' => 'Query',
        'TEMPLATE' => 'Template',
      ),
    );
  }
  $form['parameters']['add'] = array(
    '#type' => 'submit',
    '#value' => t('Add Parameter'),
    '#submit' => array('smartdocs_resource_add_parameter'),
    '#limit_validation_errors' => array(),
    '#ajax' => array(
      'callback' => 'smartdocs_resource_callback',
      'wrapper' => 'parameters',
    ),
    '#suffix' => '<br/><br/>'
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#submit' => array('smartdocs_resource_edit_submit'),
    '#value' => t('Save Resource'),
  );
  return $form;
}

/**
 * Ajax Callback to rebuild form
 *
 * @param $form
 * @param $form_state
 * @return mixed
 */
function smartdocs_resource_callback($form, $form_state) {
  return $form['parameters'];
}

/**
 * Adds parameter to the form
 *
 * @param $form
 * @param $form_state
 */
function smartdocs_resource_add_parameter($form, &$form_state) {
  $form_state['num_parameters']++;
  $form_state['rebuild'] = TRUE;
}

/**
 * Submit callback for Add Resource form
 *
 * @param $form
 * @param $form_state
 */
function smartdocs_resource_add_submit($form, &$form_state) {
  unset($form_state['values']['parameters']['add']);
  foreach ($form_state['values']['parameters'] as $key => $parameter) {
    if ((bool)$parameter['required']) {
      $form_state['values']['parameters'][$key]['required'] = 'true';
    } else {
      $form_state['values']['parameters'][$key]['required'] = 'false';
    }
  }
  $payload = array(
    'name' => preg_replace('/[^A-Za-z0-9]/', "", $form_state['values']['display_name']),
    'displayName' => $form_state['values']['display_name'],
    'description' => $form_state['values']['description'],
    'path' => $form_state['values']['path'],
    'parameters' => $form_state['values']['parameters'],
  );
  $mid = $form_state['values']['model']['name'];
  $rev = $form_state['values']['rev'];
  if (entity_get_controller('smartdocs_resource')->createResource($mid, $rev, $payload)) {
    drupal_set_message('Resource has been created successfully');
    $form_state['redirect'] = 'admin/smartdocs/models/' . $mid .'/revisions/' . $rev;
  } else {
    drupal_set_message('Resource failed to save, please consult the logs.', 'error');
  }
}

/**
 * Submit callback for Edit Resource form
 *
 * @param $form
 * @param $form_state
 */
function smartdocs_resource_edit_submit($form, &$form_state) {
  unset($form_state['values']['parameters']['add']);
  foreach ($form_state['values']['parameters'] as $key => $parameter) {
    if ((bool)$parameter['required']) {
      $form_state['values']['parameters'][$key]['required'] = 'true';
    } else {
      $form_state['values']['parameters'][$key]['required'] = 'false';
    }
  }
  $payload = array(
    'name' => preg_replace('/[^A-Za-z0-9]/', "", $form_state['values']['display_name']),
    'displayName' => $form_state['values']['display_name'],
    'description' => $form_state['values']['description'],
    'path' => $form_state['values']['path'],
    'parameters' => $form_state['values']['parameters'],
  );
  $mid = $form_state['values']['model']['name'];
  $rev = $form_state['values']['rev'];
  $res = $form_state['values']['res'];
  if (entity_get_controller('smartdocs_resource')->updateResource($mid, $rev, $res, $payload)) {
    drupal_set_message('Resource has been created successfully');
    $form_state['redirect'] = 'admin/smartdocs/models/' . $mid .'/revisions/' . $rev;
  } else {
    drupal_set_message('Resource failed to save, please consult the logs.', 'error');
  }
}