<?php

/**
 * Page callback for admin/smartdocs.
 *
 * @param $form
 * @param $form_state
 * @return array
 */
function smartdocs_models($form, &$form_state) {
  drupal_add_css(drupal_get_path('module', 'smartdocs') . '/css/smartdocs.css');
  $form = array();
  $rows = array();
  $models = entity_load('smartdocs_model');
  foreach ($models as $model) {
    $rows[strtolower($model['displayName'])] = $model;
  }
  ksort($rows);
  foreach($rows as $item) {
    $form['models'][$item['id']] = array(
      '#type' => 'row',
      '#data' => $item
    );
    $operations = array(
      'revisions' => 'API Revisions',
      'add' => 'Add API Revision',
      'import' => 'Import',
      'settings' => 'Settings',
      'export' => 'Export',
      'delete' => 'Delete'
    );
    $links = array();
    foreach($operations as $key => $op) {
      $links[$key . '-' . $item['id']] = array(
        'title' => $op,
        'href' => 'admin/smartdocs/models/' . $item['name'] . '/' . $key,
        'attributes' => array(
          'title' => $op,
          'class' => array(
            $key . '-' . $item['id'],
            'views-ajax-link'
          ),
        ),
      );
    }
    $form['models'][$item['id']]['operations'] = array(
      '#markup' => theme('links__ctools_dropbutton',
        array(
          'links' => $links,
        )
      ),
    );
  }
  $form['add_model'] = array(
    '#type' => 'button',
    '#value' => t('Add Model'),
  );
  $form['#theme'] = 'smartdocs_models_table';
  return $form;
}

/**
 * Page callback for admin/content/smartdocs/add-model.
 *
 * @param $form
 * @param $form_state
 * @return array
 */
function smartdocs_add_model($form, &$form_state) {
  $form['display_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Display Name'),
    '#size' => 60,
    '#maxlength' => 128,
    '#required' => TRUE,
  );
  $form['model_name'] = array(
    '#type' => 'machine_name',
    '#machine_name' => array(
      'exists' => 'smartdocs_model_exist',
    ),
  );
  $form['model_description'] = array(
    '#type' => 'textarea',
    '#title' => t('Description'),
    '#required' => TRUE,
  );
  $form['model_submit'] = array(
    '#type' => 'submit',
    '#value' => t('Create Model')
  );
  $form['#submit'][] = 'smartdocs_add_model_submit';
  $form['#redirect'][] = 'smartdocs_add_model_submit';
  return $form;
}

/**
 * Submit Callback for smartdocs_add_model()
 *
 * @param $form
 * @param $form_state
 */
function smartdocs_add_model_submit($form, &$form_state) {
  $payload = array(
    'model_name' => trim($form_state['values']['model_name']),
    'display_name' => trim($form_state['values']['display_name']),
    'model_description' => trim($form_state['values']['model_description']),
  );
  try {
    if (entity_get_controller('smartdocs_model')->create($payload)) {
      menu_rebuild();
      drupal_set_message($payload['display_name'] . ' was created successfully.', 'status');
    }
    $form_state['redirect'] = 'admin/smartdocs/models';
  } catch (Exception $e) {
    drupal_set_message(t('There was an error when trying to create the model.'), 'error');
  }
}

/**
 * Function that checks if the model exists already.
 *
 * @param $name
 * @return bool
 */
function smartdocs_model_exist($name) {
  $docgen = entity_load('smartdocs_model');
  foreach($docgen as $model) {
    if ($model['name'] == smartdocs_model_name($name)) {
      return TRUE;
    }
  }
  return FALSE;
}

/**
 * Page callback for admin/smartdocs.
 *
 * @param $form
 * @param $form_state
 * @param $mid
 * @return string
 */
function smartdocs_model_detail($form, $form_state, $mid) {

  drupal_add_css(drupal_get_path('module', 'smartdocs') . '/css/smartdocs.css');

  $form = array();
  $rows = array();

  $specific = entity_get_controller('smartdocs_model')->loadSingle($mid);

  $rows[] = $specific;

  drupal_set_title('‘' . $specific['displayName'] . '’ Details');

  foreach($rows as $item) {
    $form['models'][$item['id']] = array(
      '#type' => 'row',
      '#data' => $item
    );
    $operations = array(
      'revisions' => 'API Revisions',
      'add' => 'Add API Revision',
      'import' => 'Import',
      'settings' => 'Settings',
      'export' => 'Export',
      'delete' => 'Delete'
    );
    if (!isset($specific['latestRevisionNumber'])) {
      unset($operations['revisions']);
    }
    $links = array();
    foreach($operations as $key => $op) {
      $links[$key . '-' . $item['id']] = array(
        'title' => $op,
        'href' => 'admin/smartdocs/models/' . $item['name'] . '/' . $key,
        'attributes' => array(
          'title' => $op,
          'class' => array(
            $key . '-' . $item['id'],
            'views-ajax-link'
          ),
        ),
      );
    }
    $form['models'][$item['id']]['operations'] = array(
      '#markup' => theme('links__ctools_dropbutton',
        array(
          'links' => $links,
        )
      ),
    );
  }
  $form['#theme'] = 'smartdocs_models_table';
  return $form;
}

/**
 * Imports WADL / Swagger data into SmartDocs
 *
 * @param $form
 * @param $form_state
 * @param $mid
 */
function smartdocs_import_model($form, $form_state, $mid) {
  $form['model'] = array(
    '#type' => 'hidden',
    '#value' => entity_get_controller('smartdocs_model')->loadSingle($mid)
  );
  drupal_set_title('Import into ‘' . $form['model']['#value']['displayName'] . '’');
  $form['format'] = array(
    '#prefix' => '<div id="import-format">',
    '#suffix' => '</div>',
    '#type' => 'fieldset',
    '#title' => t('Import Options'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['format']['type'] = array(
    '#type' => 'select',
    '#title' => t('Choose format'),
    '#options' => array(
      'wadl' => 'WADL',
      'swagger' => 'Swagger',
      'apigee_json' => 'SmartDocs JSON',
    ),
    '#ajax' => array(
      'wrapper' => 'import-format',
      'callback' => 'smartdocs_import_ajax',
    ),
    '#default_value' => array('wadl'),
  );
  if (isset($form_state['values']['type']) || $form['format']['type']['#default_value'][0] == 'wadl') {
    if (isset($form_state['values']) && array_key_exists('type', $form_state['values'])) {
      switch($form_state['values']['type']) {
        case 'swagger':
          $form['format']['import']['swagger_model_import'] = array(
            '#type' => 'textfield',
            '#title' => t('Swagger URL'),
            '#size' => 60,
            '#maxlength' => 128,
            '#default_value' => '',
            '#required' => TRUE,
          );
          break;
        case 'apigee_json':
          $form['format']['import']['upload_type'] = array(
            '#type' => 'select',
            '#title' => t('Upload Type'),
            '#options' => array(
              'upload' => 'File',
              'path' => 'URL Path',
            ),
            '#ajax' => array(
              'wrapper' => 'import-format',
              'callback' => 'smartdocs_import_ajax',
            ),
            '#default_value' => array('upload'),
          );
          if (isset($form_state['values']['upload_type'])) {
            switch($form_state['values']['upload_type']) {
              case 'upload':
                $form['format']['import']['file_model_import'] = array(
                  '#type' => 'managed_file',
                  '#title' => t('File'),
                  '#description' => t('Allowed extensions: .json'),
                  '#upload_location' => 'public://jsons',
                  '#upload_validators' => array(
                    'file_validate_extensions' => array('json'),
                  ),
                  '#required' => TRUE,
                );
                break;
              case 'path':
                $form['format']['import']['path_model_import'] = array(
                  '#type' => 'textfield',
                  '#title' => t('URL Path'),
                  '#size' => 60,
                  '#maxlength' => 128,
                  '#default_value' => '',
                  '#required' => TRUE,
                );
                break;
              default:
                $form['format']['import']['file_model_import'] = array(
                  '#type' => 'managed_file',
                  '#title' => t('File'),
                  '#description' => t('Allowed extensions: .json'),
                  '#upload_location' => 'public://jsons',
                  '#upload_validators' => array(
                    'file_validate_extensions' => array('json'),
                  ),
                  '#required' => TRUE,
                );
            }
          } else {
            $form['format']['import']['file_model_import'] = array(
              '#type' => 'managed_file',
              '#title' => t('File'),
              '#description' => t('Allowed extensions: .json'),
              '#upload_location' => 'public://jsonss',
              '#upload_validators' => array(
                'file_validate_extensions' => array('json'),
              ),
              '#required' => TRUE,
            );
          }
          break;
        default:
          $form['format']['import']['upload_type'] = array(
            '#type' => 'select',
            '#title' => t('Upload Type'),
            '#options' => array(
              'upload' => 'File',
              'path' => 'URL Path',
            ),
            '#ajax' => array(
              'wrapper' => 'import-format',
              'callback' => 'smartdocs_import_ajax',
            ),
            '#default_value' => array('upload'),
          );
          if (isset($form_state['values']['upload_type'])) {
            switch($form_state['values']['upload_type']) {
              case 'upload':
                $form['format']['import']['file_model_import'] = array(
                  '#type' => 'managed_file',
                  '#title' => t('File'),
                  '#description' => t('Allowed extensions: .wadl .xml'),
                  '#upload_location' => 'public://wadls',
                  '#upload_validators' => array(
                    'file_validate_extensions' => array('wadl xml'),
                  ),
                  '#required' => TRUE,
                );
                break;
              case 'path':
                $form['format']['import']['path_model_import'] = array(
                  '#type' => 'textfield',
                  '#title' => t('URL Path'),
                  '#size' => 60,
                  '#maxlength' => 128,
                  '#default_value' => '',
                  '#required' => TRUE,
                );
                break;
              default:
                $form['format']['import']['file_model_import'] = array(
                  '#type' => 'managed_file',
                  '#title' => t('File'),
                  '#description' => t('Allowed extensions: .wadl .xml'),
                  '#upload_location' => 'public://wadls',
                  '#upload_validators' => array(
                    'file_validate_extensions' => array('wadl xml'),
                  ),
                  '#required' => TRUE,
                );
            }
          } else {
            $form['format']['import']['file_model_import'] = array(
              '#type' => 'managed_file',
              '#title' => t('File'),
              '#description' => t('Allowed extensions: .wadl .xml'),
              '#upload_location' => 'public://wadls',
              '#upload_validators' => array(
                'file_validate_extensions' => array('wadl xml'),
              ),
              '#required' => TRUE,
            );
          }
          break;
      }
    } else {
      $form['format']['import']['upload_type'] = array(
        '#type' => 'select',
        '#title' => t('Upload Type'),
        '#options' => array(
          'upload' => 'File',
          'path' => 'URL Path',
        ),
        '#ajax' => array(
          'wrapper' => 'import-format',
          'callback' => 'smartdocs_import_ajax',
        ),
        '#default_value' => array('upload'),
      );
      $form['format']['import']['file_model_import'] = array(
        '#type' => 'managed_file',
        '#title' => t('File'),
        '#description' => t('Allowed extensions: .wadl .xml'),
        '#upload_location' => 'public://wadls',
        '#upload_validators' => array(
          'file_validate_extensions' => array('wadl xml'),
        ),
        '#required' => TRUE,
      );
    }
  }
  $form['import'] = array(
    '#type' => 'submit',
    '#value' => t('Import')
  );
  return $form;
}

/**
 * Ajax callback for import format
 */
function smartdocs_import_ajax($form, &$form_state) {
  $form['#rebuild'] = TRUE;
  return $form['format'];
}

/**
 * Validate callback for smartdocs_import_model
 *
 * @param $form
 * @param $form_state
 */
function smartdocs_import_model_validate($form, &$form_state) {
  switch($form_state['values']['type']) {
    case 'swagger':
      if (isset($form_state['values']['swagger_model_import'])) {
        if (!(filter_var($form_state['values']['swagger_model_import'], FILTER_VALIDATE_URL))) {
          form_set_error('swagger_model_import', 'The Swagger URL is not a valid URL.');
        }
      }
      break;
    default:
      $format = $form_state['values']['type'] == "apigee_json"? "SmartDocs JSON": "WADL";
      $upload_type = (array_key_exists('upload_type', $form_state['values']) ? $form_state['values']['upload_type'] : NULL);
      switch($upload_type) {
        case 'path':
          if (isset($form_state['values']['path_model_import'])) {
            if (!(filter_var($form_state['values']['path_model_import'], FILTER_VALIDATE_URL))) {
              form_set_error('path_model_import', "The $format URL is not a valid URL.");
            }
          }
          break;
        default:
          break;
      }
      break;
  }
}

/**
 * Imports WADL / Swagger data into SmartDocs
 *
 * @param $form
 * @param $form_state
 * @param $mid
 */
function smartdocs_import_model_submit($form, &$form_state) {
  $type = $form_state['values']['type'];
  $entity = array(
    'apiId' => $form_state['values']['model']['name'],
  );
  $message = t('There was an error importing your data.');
  switch($type) {
    case 'wadl':
      $action = $form_state['values']['upload_type'];
      switch($action) {
        case 'upload':
          $file = file_load($form_state['values']['file_model_import']);
          $file->status = FILE_STATUS_PERMANENT;
          file_save($file);
          $entity['xml'] = file_get_contents($file->uri);
          $message = t('The WADL XML has been imported into the model.');
          break;
        case 'path':
          $entity['xml'] = file_get_contents($form_state['values']['path_model_import']);
          $message = t('The WADL XML has been imported into the model.');
          break;
      }
      break;
    case 'swagger':
      $entity['url'] = $form_state['values']['swagger_model_import'];
      $message = t('The swagger data has been imported into the model.');
      break;
    case 'apigee_json':
      $action = $form_state['values']['upload_type'];
      switch($action) {
        case 'upload':
          $file = file_load($form_state['values']['file_model_import']);
          $file->status = FILE_STATUS_PERMANENT;
          file_save($file);
          $entity['json'] = file_get_contents($file->uri);
          $message = t('The WADL XML has been imported into the model.');
          break;
        case 'path':
          $entity['json'] = file_get_contents($form_state['values']['path_model_import']);
          $message = t('The WADL XML has been imported into the model.');
          break;
      }
      break;
  }
  $import = entity_get_controller('smartdocs_model')->import($entity, $type);
  if (isset($import['revisionNumber'])) {
    drupal_set_message($message, 'status');
    $form_state['redirect'] = 'admin/smartdocs/models/' . $form_state['values']['model']['name'] . '/revisions/' . $import['revisionNumber'];
  } else {
    drupal_set_message('There was an error importing your data. Please consult the logs.', 'error');
  }
}

/**
 * Exports Model into specified format
 *
 * @param $form
 * @param $form_state
 * @param $mid
 */
function smartdocs_model_export($form, &$form_state, $mid, $rev = NULL) {
  if ($rev) {
    drupal_set_title('Export Revision #' . $rev);
  }
  $form = array();
  $form['rev'] = array(
    '#type' => 'hidden',
    '#value' => (is_null($rev)) ? 'latest' : $rev,
  );
  $form['model'] = array(
    '#type' => 'hidden',
    '#value' => $mid,
  );
  $options = array(
    'json' => 'SmartDocs JSON',
    'wadl' => 'WADL'
  );
  $form['file_type'] = array(
    '#type' => 'select',
    '#title' => t('Select export file type'),
    '#options' => $options,
    '#default_value' => '',
  );
  $form['options']['submit_export'] = array(
    '#type' => 'submit',
    '#value' => t('Export'),
    '#prefix' => '<br>',
  );
  return $form;
}

/**
 * Submit Callback for smartdocs_model_export
 *
 * @param $form
 * @param $form_state
 * @param $mid
 */
function smartdocs_model_export_submit($form, &$form_state) {
  try {
    if ($form_state['input']['file_type'] == 'json') {
      $data = entity_get_controller('smartdocs_model')->export($form_state['values']['model'], '', $form_state['values']['rev']);
      $filename = $form_state['input']['model'] . '.json';
    }
    else {
      $data = entity_get_controller('smartdocs_model')->export($form_state['values']['model'], $form_state['input']['file_type'], $form_state['values']['rev']);
      $filename = $form_state['input']['model'] . '.wadl';
    }
  } catch (Exception $e) {
    $message = t('There was an error exporting the model: @model', array('@model' => $form_state['values']['model']));
    drupal_set_message($message, 'error');
    return;
  }
  $xexport_dir = 'public://wadls_export';
  file_prepare_directory($xexport_dir, FILE_CREATE_DIRECTORY);
  $destination = $xexport_dir . '/' . $filename;
  $replace = FILE_EXISTS_REPLACE;
  $my_file_obj = file_save_data($data, $destination, $replace);
  $headers = file_get_content_headers($my_file_obj);
  $headers['Content-Disposition'] = 'attachment; filename="' . $my_file_obj->filename . '"';
  file_transfer($my_file_obj->uri, $headers);
}

/**
 * Specifies Settings for a specific model
 *
 * @param $form
 * @param $form_state
 * @param $mid
 */
function smartdocs_model_settings($form, &$form_state, $mid) {
  $model = entity_get_controller('smartdocs_model')->loadSingle($mid);
  drupal_set_title('‘' . $model['displayName'] . '’' . ' Settings');
  $form['model'] = array(
    '#type' => 'hidden',
    '#value' => $model,
  );
  $def_css = 'https://smartdocs.apigee.com/3/static/css/main.css
https://smartdocs.apigee.com/3/static/css/codemirror.css
https://smartdocs.apigee.com/3/static/css/prism.css';
  $def_js = "https://smartdocs.apigee.com/3/static/js/codemirror.js
https://smartdocs.apigee.com/3/static/js/codemirror_javascript.js
https://smartdocs.apigee.com/3/static/js/codemirror_xml.js
https://smartdocs.apigee.com/3/static/js/prism.js
https://smartdocs.apigee.com/3/static/js/base64_min.js
https://smartdocs.apigee.com/3/static/js/model.js
https://smartdocs.apigee.com/3/static/js/controller.js";
  $form[smartdocs_model_name($mid) . '_css'] = array(
    '#type' => 'textarea',
    '#title' => t('CSS Style Sheets'),
    '#rows' => 3,
    '#default_value' => variable_get(smartdocs_model_name($mid) . '_css', $def_css),
  );
  $form[smartdocs_model_name($mid) . '_js'] = array(
    '#type' => 'textarea',
    '#title' => t('JS Style Sheets'),
    '#rows' => 7,
    '#default_value' => variable_get(smartdocs_model_name($mid) . '_js', $def_js),
  );
  $form['templates'] = array(
    '#type' => 'fieldset',
    '#title' => t('Template'),
    '#collapsed' => FALSE,
    '#collapsible' => TRUE,
  );
  $template = entity_get_controller('smartdocs_template')->loadTemplate($mid, 'method');
  $form['templates'][smartdocs_model_name($mid)][smartdocs_model_name($mid) . '_method_template'] = array(
    '#type' => 'textarea',
    '#title' => t('Method Template'),
    '#rows' => 10,
    '#default_value' => $template,
    '#prefix' => '<div id="' . smartdocs_model_name($mid) . '-wrapper">',
    '#suffix' => '</div>',
  );
  $form['templates'][smartdocs_model_name($mid)]['method_save'] = array(
    '#type' => 'submit',
    '#value' => 'Save ' . $model['displayName'] . ' Template',
    '#submit' => array('smartdocs_method_save'),
    '#model' => smartdocs_model_name($mid),
    '#ajax' => array(
      'callback' => 'smartdocs_method_callback',
      'wrapper' => $mid . '-wrapper',
      'method' => 'replace',
      'effect' => 'fade',
      'progress' => array(
        'type' =>'throbber',
      )
    ),
  );
  return system_settings_form($form);
}

/**
 * Ajax Callback for Model Template.
 *
 * @param $form
 * @param $form_state
 * @return mixed
 */
function smartdocs_method_callback($form, &$form_state) {
  $name = $form_state['clicked_button']['#model'];
  return $form['templates'][$name][$name . '_method_template'];
}

/**
 * Submit Callback for Model Template.
 *
 * @param $form
 * @param $form_state
 */
function smartdocs_method_save($form, &$form_state) {
  try {
    $mid = $form_state['values']['model']['name'];
    $display = $form_state['values']['model']['displayName'];
    $html = $form_state['values'][$mid . '_method_template'];
    if ($save = entity_get_controller('smartdocs_template')->updateTemplate($mid, 'method', $html)) {
      drupal_set_message('Template Saved. Cache cleared for the ' . $display . ' model.');
      $query = db_select('smartdata', 's');
      $query->condition('s.model', $mid);
      $query->fields('s', array('nid'));
      $results = $query->execute();
      foreach($results as $result) {
        cache_clear_all($result->nid, 'cache_smartdocs');
      }
    }
  } catch (Exception $e) {
    drupal_set_message('There was an error saving your template.', 'error');
  }
  $form_state['rebuild'] = TRUE;
}

/**
 * Exports Model into specified format
 *
 * @param $form
 * @param $form_state
 * @param $mid
 */
function smartdocs_model_delete($form, &$form_state, $mid) {
  $form['model'] = array(
    '#type' => 'hidden',
    '#value' => $mid,
  );
  $ques = 'Are you sure?';
  $desc = 'Deleting any model will also delete it\'s corresponding content type and all of it\'s data.';
  return confirm_form($form, $ques, 'admin/content/smartdocs', $desc);
}

/**
 * Exports Model into specified format
 *
 * @param $form
 * @param $form_state
 * @param $mid
 */
function smartdocs_model_delete_submit($form, &$form_state) {
  $model = $form_state['values']['model'];
  try {
    if (entity_get_controller('smartdocs_model')->delete($model)) {
      if (variable_get(smartdocs_model_name($model) . '_nodes', null)) {
        variable_del(smartdocs_model_name($model) . '_nodes');
        variable_del(smartdocs_model_name($model));
      }
      if (variable_get('smartdocs_content_types', array())) {
        $models = variable_get('smartdocs_content_types', array());
        $types = array();
        foreach ($models as $display => $type) {
          if ($type != smartdocs_model_name($model)) {
            $types[$display] = $type;
          }
        }
        variable_set('smartdocs_content_types', $types);
      }
      $results = db_select('node', 'n')
        ->fields('n', array('nid'))
        ->condition('type', smartdocs_model_name($model))
        ->execute();
      foreach ($results as $result) {
        $nids[] = $result->nid;
        db_delete('smartdata')
          ->condition('nid', $result->nid)
          ->execute();
      }
      (!empty($nids)) ? node_delete_multiple($nids) : null;
      $update = db_update('node_type')->fields(array('custom' => 1,'modified' => 1,'locked' => 0))
        ->condition('type', smartdocs_model_name($model))->execute();
      ($update) ? node_type_delete(smartdocs_model_name($model)) : null;
      $vocabs = array( 'tags', 'auth_schemes', 'methods', 'revisions', 'types');
      foreach ($vocabs as $machine) {
        if ($vid = taxonomy_vocabulary_machine_name_load(smartdocs_model_name($model) . '_' . $machine)->vid) {
          foreach (taxonomy_get_tree($vid) as $term) {
            taxonomy_term_delete($term->tid);
          }
          taxonomy_vocabulary_delete($vid);
        }
      }
      $fid = db_query("SELECT fid FROM {file_managed} WHERE filename=:name", array(':name' => smartdocs_model_name($model) . '_listing.view'))->fetchField();
      if ($fid) {
        $file = file_load($fid);
        file_delete($file);
      }
      menu_rebuild();
      drupal_flush_all_caches();
      drupal_set_message(t('%model has been deleted and all of it\'s associated content.', array('%model' => $model)), 'status');
      $form_state['redirect'] = 'admin/smartdocs/models';
    } else {
      drupal_set_message('There was an error deleting the model(s).', 'error');
    }
  } catch (Exception $e) {
    drupal_set_message('There was an error deleting the model(s).', 'error');
  }
}
