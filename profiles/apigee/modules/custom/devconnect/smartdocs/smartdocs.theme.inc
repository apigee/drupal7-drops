<?php

/**
 * Implements theme_hook
 *
 * @param $vars
 */
function theme_smartdocs_models_table($form) {
  drupal_add_css(drupal_get_path('module', 'views') . '/css/views-admin.ctools.css');
  $form = $form['form'];
  $specific = arg(3);
  if (isset($form['models'])) {
    foreach($form['models'] as $model) {
      if (isset($model['#type']) && $model['#type'] == 'row') {
        if (isset($specific)) {
          $operations = $model['operations']['#markup'];
          $model = entity_get_controller('smartdocs_model')->loadSingle($specific);
          if (!isset($model['#data']['id'])) {
            $model['#data']['id'] = '';
          }
          $rows[] = array(
            'data' => array(
              'name' => array(
                'data' => $model['displayName'],
              ),
              'description' => array(
                'data' => '<p>' . $model['description'] . '</p><br/>',
              ),
              'revision' => array(
                'data' => (isset($model['latestRevisionNumber'])) ? l('Revision #' . $model['latestRevisionNumber'],
                    'admin/smartdocs/models/' . $model['name'] . '/revisions/' . $model['latestRevisionNumber']) : 'n/a'
              ),
              'created' => array(
                'data' => date('F d, Y',  floor($model['createdTime'] / 1000))
              ),
              'updated' => array(
                'data' => (isset($model['modifiedTime'])) ? date('F d, Y',  floor($model['modifiedTime'] / 1000)) : 'n/a'
              ),
              'operations' => array(
                'data' => $operations,
              )
            ),
          );
        } else {
          $rows[] = array(
            'data' => array(
              'name' => array(
                'data' => l($model['#data']['displayName'], 'admin/smartdocs/models/' . $model['#data']['name']),
              ),
              'description' => array(
                'data' => '<p>' . $model['#data']['description'] . '</p><br/>',
              ),
              'operations' => array(
                'data' => $model['operations']['#markup'],
              )
            ),
          );
        }
      }
    }
    $link['add_model'] = array(
      'title' => t('Add a New Model'),
      'href' => 'admin/smartdocs/models/add',
      'attributes' => array(
        'title' => t('Add a New Model'),
        'class' => array(
          'views-ajax-link'
        ),
      ),
    );
    if (isset($specific)) {
      $header = array(
        'name' => t('Name'),
        'description' => t('Description'),
        'revision' => t('Latest Revision'),
        'created' => t('Created'),
        'updated' => t('Updated'),
        'operations' => array(
          'data' => t('Operations'),
          'class' => array(
            'smartdocs-operations'
          ),
        ),
      );
    } else {
      $header = array(
        'name' => t('Name'),
        'description' => t('Description'),
        'operations' => array(
          'data' => t('Operations'),
          'class' => array(
            'smartdocs-operations'
          ),
        ),
      );
    }
    if (isset($rows)) {
      if (isset($specific)) {
        return theme('table', array(
            'header' => $header,
            'rows' => $rows,
          )
        );
      } else {
        $ops = theme('links__ctools_dropbutton',
          array(
            'links' => $link,
          )
        );
        return $ops . '<br/><br/>' . theme('table', array(
            'header' => $header,
            'rows' => $rows,
          )
        );
      }
    }
  } else {
    return 'Make a model';
  }
}

/**
 * Implements theme_hook
 *
 * @param $vars
 */
function theme_smartdocs_revisions_table($form) {
  drupal_add_css(drupal_get_path('module', 'views') . '/css/views-admin.ctools.css');
  $form = $form['form'];
  if (!empty($form['revisions'])) {
    foreach($form['revisions'] as $revision) {
      if (isset($revision['#type']) && $revision['#type'] == 'row') {
        $rows[] = array(
          'data' => array(
            'revision' => array(
              'data' => l('Revision #' . $revision['#data']['revisionNumber'],
                'admin/smartdocs/models/' . $form['model']['#data']['name'] . '/revisions/' . $revision['#data']['revisionNumber']),
            ),
            'base' => array(
              'data' => $revision['#data']['baseUrl'],
            ),
            'created' => array(
              'data' => date('F d, Y',  floor($revision['#data']['createdTime'] / 1000)),
            ),
            'operations' => array(
              'data' => $revision['operations']['#markup']
            )
          ),
        );
      }
    }
    $link['add_model'] = array(
      'title' => t('Add API Revision'),
      'href' => 'admin/smartdocs/models/' . $form['model']['#data']['name'] .'/add',
      'attributes' => array(
        'title' => t('Add API Revision'),
        'class' => array(
          'views-ajax-link'
        ),
      ),
    );
    $link['import_model'] = array(
      'title' => t('Import'),
      'href' => 'admin/smartdocs/models/' . $form['model']['#data']['name'] . '/import',
      'attributes' => array(
        'title' => t('Import'),
        'class' => array(
          'views-ajax-link'
        ),
      ),
    );
    $header = array(
      'revision' => t('Revision'),
      'base' => t('Base URL'),
      'created' => t('Created'),
      'operations' => array(
        'data' => t('Operations'),
        'class' => array(
          'smartdocs-operations'
        ),
      )
    );
    if (isset($rows)) {
      $ops = theme('links__ctools_dropbutton',
        array(
          'links' => $link,
        )
      );
      $recent = $header;
      $recent['revision'] = t('Latest Revision');
      $latest = theme('table', array(
        'header' => $recent,
        'rows' => array_pop($rows),
      ));
      if (empty($rows)) {
        return $ops . '<br/><br/>' . $latest;
      } else {
        return $ops . '<br/><br/>' . $latest . theme('table', array(
            'header' => $header,
            'rows' => $rows,
          )
        );
      }
    } else {
      return theme('links__ctools_dropbutton',
        array(
          'links' => $link,
        )
      );
    }
  } else {
    $link['add_model'] = array(
      'title' => t('Add API Revision'),
      'href' => 'admin/smartdocs/models/' . $form['model']['#data']['name'] .'/add',
      'attributes' => array(
        'title' => t('Add API Revision'),
        'class' => array(
          'views-ajax-link'
        ),
      ),
    );
    $link['import_model'] = array(
      'title' => t('Import'),
      'href' => 'admin/smartdocs/models/' . $form['model']['#data']['name'] .'/import',
      'attributes' => array(
        'title' => t('Import'),
        'class' => array(
          'views-ajax-link'
        ),
      ),
    );
    return theme('links__ctools_dropbutton',
      array(
        'links' => $link,
      )
    );
  }
}

/**
 * Implements preprocess_page()
 *
 * @param $vars
 */
function smartdocs_preprocess_page(&$vars) {
  $term = menu_get_object('taxonomy_term', 2);
  if ($term) {
    $show_crumb = FALSE;
    $types = node_type_get_types();
    foreach ($types as $type) {
      if ($term->vocabulary_machine_name == $type->type . '_methods') {
        $show_crumb = TRUE;
        $node_type = $type->type;
        $node_name = $type->name;
      }
    }
    if ($show_crumb && isset($node_type) && isset($node_name)) {
      $breadcrumb = array();
      $breadcrumb[] = l('Home', '<front>');
      $breadcrumb[] = l($node_name . ' Documentation', variable_get($node_type) . '/apis');
      drupal_set_breadcrumb($breadcrumb);
    }
  }
  if (isset($vars['node'])) {
    $models = array();
    $types = db_query('SELECT model FROM {smartdocs} WHERE nid = :nid', array(':nid' => $vars['node']->nid));
    foreach ($types as $type) {
      $models[smartdocs_model_name($type->model)] = $type->model;
    }
    if (in_array($vars['node']->type, array_keys($models))) {
      global $base_url;
      if (isset($vars['tabs']['#primary']) && !user_is_anonymous()) {
        $edit = array();
        foreach($vars['tabs']['#primary'] as $key => $tab) {
          if (strtolower($tab['#link']['title']) == 'view' || strtolower($tab['#link']['title']) == 'view published') {
            $edit_mode = (isset($_GET['editMode'])) ? $_GET['editMode'] : 0;
            if ($edit_mode == '1') {
              $edit = $tab;
              $edit['#active'] = TRUE;
              $vars['tabs']['#primary'][$key]['#active'] = FALSE;
            } else {
              $edit = $tab;
              $edit['#active'] = FALSE;
            }
          }
          if (strtolower($tab['#link']['title']) == 'edit') {
            $vars['tabs']['#primary'][$key]['#link']['title'] = 'Edit Drupal Node';
          }
        }
        $edit['#link']['title'] = 'Edit SmartDoc';
        $edit['#link']['href'] = url($base_url . '/' . drupal_get_path_alias(current_path()), array('query' => array('editMode' => '1')));
        $vars['tabs']['#primary'][] = $edit;
      }
      $vars['dna'] = TRUE;
      $breadcrumb = array();
      $breadcrumb[] = l('Home', '<front>');
      $breadcrumb[] = l($models[$vars['node']->type] . ' Documentation', variable_get($vars['node']->type) . '/apis');
      $field_name = 'field_' . smartdocs_field_name($models[$vars['node']->type]) . '_model_verb';
      $verb =& $vars['node']->$field_name;
      if (isset($verb[LANGUAGE_NONE][0]['taxonomy_term'])) {
        $breadcrumb[] = l($verb[LANGUAGE_NONE][0]['taxonomy_term']->name, 'taxonomy/term/' . $verb[LANGUAGE_NONE][0]['taxonomy_term']->tid);
      }
      drupal_set_breadcrumb($breadcrumb);
    }
  }
}


/**
 * Implements preprocess_node()
 *
 * @param $vars
 */
function smartdocs_preprocess_node(&$vars) {
  global $base_url;
  $models = array();
  $types = db_query('SELECT model FROM {smartdocs} WHERE nid = :nid', array(':nid' => $vars['nid']));
  foreach ($types as $type) {
    $models[smartdocs_model_name($type->model)] = $type->model;
  }
  $smart_mid = smartdocs_model_name($models[$vars['type']]);
  if (in_array($vars['type'], array_keys($models))) {
    $templates = entity_get_controller('smartdocs_doc');
    $config = devconnect_get_org_settings();

    if ((bool)variable_get('use_api_proxy_url', 0)) {
      drupal_add_js(
        array(
          'devconnect_docgen' => array(
            'isDrupal' => TRUE,
            'apiModelBaseUrl' => variable_get('api_proxy_url'),
            'oauth2AuthUrl' => $base_url . '/smartdocs/oauth/callback',
          )
        ), 'setting');
    } else {
      drupal_add_js(
        array(
          'devconnect_docgen' => array(
            'isDrupal' => TRUE,
            'apiModelBaseUrl' => str_replace('/v1', '', $config['endpoint']),
            'oauth2AuthUrl' => $base_url . '/smartdocs/oauth/callback',
          )
        ), 'setting');
    }

    $query = db_select('smartdocs', 's');
    $query->condition('s.nid', $vars['node']->nid);
    $query->fields('s', array('nid', 'revision', 'resource', 'method', 'model'));

    $results = $query->execute();
    $data = array();
    foreach($results as $result) {
      $data = array(
        'nid' => $result->nid,
        'revision' => $result->revision,
        'resource' => $result->resource,
        'operation' => $result->method,
      );
      $model = $result->model;
    }

    drupal_add_css(drupal_get_path('module', 'smartdocs') . '/css/smartdocs.css');

    if (!(bool)variable_get('use_local_assets', 0)) {
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
      $js = preg_split("/\\r\\n|\\r|\\n/", variable_get($smart_mid . '_js', $def_js));
      $css = preg_split("/\\r\\n|\\r|\\n/", variable_get($smart_mid . '_css', $def_css));
      foreach($js as $file) {
        drupal_add_js($file);
      }
      foreach($css as $file) {
        drupal_add_css($file);
      }
    } else {
      $path = drupal_get_path('module', 'smartdocs');
      $css = array(
        $path . '/local/css/main.css',
        $path . '/local/css/codemirror.css',
        $path . '/local/css/prism.css',
      );
      $js = array(
        $path . '/local_js/codemirror.js',
        $path . '/local_js/codemirror_javascript.js',
        $path . '/local_js/codemirror_xml.js',
        $path . '/local_js/prism.js',
        $path . '/local_js/base64_min.js',
        $path . '/local_js/model.js',
        $path . '/local_js/controller.js',
      );
      $i = 100;
      foreach($js as $file) {
        $options = array(
          'group' => JS_THEME,
          'weight' => $i,
        );
        drupal_add_js($file, $options);
        $i++;
      }
      $i = 100;
      foreach($css as $file) {
        $options = array(
          'group' => CSS_THEME,
          'weight' => $i,
        );
        drupal_add_css($file, $options);
        $i++;
      }
    }
    $data = $templates->loadOperation((array)$data, $model, 'drupal-cms');
    $vars['content'] = array();
    $vars['content']['#markup'] = $data;
  }
}