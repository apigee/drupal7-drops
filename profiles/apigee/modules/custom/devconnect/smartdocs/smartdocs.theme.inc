<?php

/**
 * Implements theme_hook().
 *
 * @param $form
 * @return string
 */
function theme_smartdocs_models_table($form) {
  drupal_add_css(drupal_get_path('module', 'views') . '/css/views-admin.ctools.css');
  $form = $form['form'];
  $specific = arg(3);
  if (isset($form['models'])) {
    foreach ($form['models'] as $model) {
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
                'data' => (isset($model['latestRevisionNumber'])) ? l('Revision #' . $model['latestRevisionNumber'], 'admin/smartdocs/models/' . $model['name'] . '/revisions/' . $model['latestRevisionNumber']) : 'n/a'
              ),
              'created' => array(
                'data' => date('F d, Y', floor($model['createdTime'] / 1000))
              ),
              'updated' => array(
                'data' => (isset($model['modifiedTime'])) ? date('F d, Y', floor($model['modifiedTime'] / 1000)) : 'n/a'
              ),
              'operations' => array(
                'data' => $operations,
              )
            ),
          );
        }
        else {
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
    }
    else {
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
      }
      else {
        $ops = theme('links__ctools_dropbutton', array(
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
  }
  else {
    return 'Make a model';
  }
}

/**
 * Implements theme_hook().
 *
 * @param $form
 * @return string
 */
function theme_smartdocs_revisions_table($form) {
  drupal_add_css(drupal_get_path('module', 'views') . '/css/views-admin.ctools.css');
  $form = $form['form'];
  if (!empty($form['revisions'])) {
    foreach ($form['revisions'] as $revision) {
      if (isset($revision['#type']) && $revision['#type'] == 'row') {
        $rows[] = array(
          'data' => array(
            'revision' => array(
              'data' => l('Revision #' . $revision['#data']['revisionNumber'], 'admin/smartdocs/models/' . $form['model']['#data']['name'] . '/revisions/' . $revision['#data']['revisionNumber']),
            ),
            'base' => array(
              'data' => $revision['#data']['baseUrl'],
            ),
            'created' => array(
              'data' => date('F d, Y', floor($revision['#data']['createdTime'] / 1000)),
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
      'href' => 'admin/smartdocs/models/' . $form['model']['#data']['name'] . '/add',
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
      $ops = theme('links__ctools_dropbutton', array(
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
      }
      else {
        return $ops . '<br/><br/>' . $latest . theme('table', array(
              'header' => $header,
              'rows' => $rows,
                )
        );
      }
    }
    else {
      return theme('links__ctools_dropbutton', array(
        'links' => $link,
          )
      );
    }
  }
  else {
    $link['add_model'] = array(
      'title' => t('Add API Revision'),
      'href' => 'admin/smartdocs/models/' . $form['model']['#data']['name'] . '/add',
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
    return theme('links__ctools_dropbutton', array(
      'links' => $link,
        )
    );
  }
}

/**
 * Sets breadcrumbs for taxonomy term pages related to SmartDocs vocabularies.
 *
 * @param $term
 */
function _smartdocs_set_term_breadcrumb($term) {
  if (!$term) {
    drupal_set_message('no term!');
    return;
  }
  if (substr($term->vocabulary_machine_name, -8) != '_methods') {
    drupal_set_message(substr($term->vocabulary_machine_name, -8));
    return;
  }
  $type = node_type_get_type(substr($term->vocabulary_machine_name, 0, -8));
  if (!$type) {
    return;
  }
  $breadcrumb = array(
    l(t('Home'), '<front>'),
    l(t('@node_name Documentation', array('@node_name' => $type->name)), $type->type . '/apis'),
  );
  drupal_set_breadcrumb($breadcrumb);
}

/**
 * Properly decorates SmartDocs node pages.
 *
 * @param array $vars
 */
function _smartdocs_preprocess_node_page(&$vars) {
  $node = $vars['node'];

  $types = db_select('smartdata', 's')
    ->fields('s', array('model'))
    ->condition('nid', $node->nid)
    ->execute()
    ->fetchCol();
  if (empty($types)) {
    return;
  }
  $type = NULL;
  foreach ($types as $this_type) {
    $this_type = smartdocs_model_name($this_type);
    if ($this_type == $node->type) {
      $type = $this_type;
      break;
    }
  }
  if (!isset($type)) {
    return;
  }

  // Set 'dna' var to let templates know this is Smartdocs doing the talking.
  $vars['dna'] = TRUE;

  // If appropriate, set edit revision tab(s).
  if (array_key_exists('#primary', $vars['tabs']) && !user_is_anonymous()) {
    $edit = array();
    foreach ($vars['tabs']['#primary'] as $key => $tab) {
      if (strtolower($tab['#link']['title']) == 'view' || strtolower($tab['#link']['title']) == 'view published') {
        $edit = $tab;
        break;
      }
    }
    $edit['#active'] = FALSE;
    $result = db_select('smartdata', 's')
      ->fields('s', array('model', 'revision'))
      ->condition('nid', $node->nid)
      ->execute();
    foreach ($result as $record) {
      $edit['#link']['title'] = 'View SmartDocs Revision';
      $edit['#link']['href'] = url(base_path() . '/admin/smartdocs/models/' . $record->model . '/revisions/' . $record->revision);
      $vars['tabs']['#primary'][] = $edit;
    }
  }

  // Set breadcrumbs
  $breadcrumb = array(
    l(t('Home'), '<front>'),
    l(t('@node_type Documentation', array('@node_type' => node_type_get_name($node))), $type . '/apis')
  );
  $field_name = 'field_' . smartdocs_field_name($type) . '_model_verb';
  $verb =& $node->$field_name;
  if (isset($verb[LANGUAGE_NONE][0]['taxonomy_term'])) {
    $verb_term =& $verb[LANGUAGE_NONE][0]['taxonomy_term'];
    $breadcrumb[] = l($verb_term->name, 'taxonomy/term/' . $verb_term->tid);
  }
  drupal_set_breadcrumb($breadcrumb);
}

/**
 * Implements hook_preprocess_page().
 *
 * If this is a vocab term page related to smartdocs, the breadcrumb is set.
 *
 * If this is a node related to a smartdocs model, the breadcrumb is set,
 * the 'dna' page variable is set to TRUE, and the "View SmartDocs Revision"
 * tab(s) is/are created for users with appropriate permissions.
 */
function smartdocs_preprocess_page(&$vars) {
  if (arg(0) == 'taxonomy' && arg(1) == 'term' && is_numeric(arg(2))) {
    _smartdocs_set_term_breadcrumb(menu_get_object('taxonomy_term', 2));
  }
  elseif (array_key_exists('node', $vars)) {
    _smartdocs_preprocess_node_page($vars);
  }
}

/**
 * Implements hook_form_alter().
 */
function smartdocs_form_alter(&$form, &$form_state, $form_id) {
  if (strpos($form_id, 'node_form') !== FALSE) {
    $arg = arg();
    $query = db_select('smartdata', 's');
    $query->fields('s', array('model', 'nid', 'method', 'resource', 'revision'));
    $result = $query->execute();
    $type = array();
    foreach ($result as $record) {
      $mid = strtolower(preg_replace(array(
        '/[^a-zA-Z0-9]+/',
        '/-+/',
        '/^-+/',
        '/-+$/',
              ), array('_', '_', '', ''), $record->model));
      $mid = substr($mid, 0, 32);
      $type[$mid][$record->nid] = array(
        'model' => $record->model,
        'revision' => $record->revision,
        'resource' => (isset($record->resource)) ? $record->resource : NULL,
        'method' => (isset($record->method)) ? $record->method : NULL,
        'nid' => $record->nid,
        'smart_model' => $mid,
      );
    }
    if (!empty($type)) {
      if (array_key_exists($form['type']['#value'], $type)) {
        if (is_numeric($arg[1]) && $arg[2] == 'edit') {
          $info = $type[$form['type']['#value']][$form['nid']['#value']];
          $path = 'admin/smartdocs/models/' . $info['model'] . '/method/edit/' . $info['revision'] .
              '/' . $info['resource'] . '/' . $info['method'];
          unset($_GET['destination']);
          unset($_REQUEST['edit']['destination']);
          $redirect = (isset($_GET['rawNode'])) ? $_GET['rawNode'] : 0;
          if (!(bool) check_plain($redirect)) {
            drupal_goto($path);
          }
        }
        else {
          $revision = 1;
          $mid = '';
          foreach ($type[$form['type']['#value']] as $model) {
            if (!isset($revision)) {
              $revision = (int) $model['revision'];
            }
            else {
              if ($revision < (int) $model['revision']) {
                $revision = (int) $model['revision'];
              }
            }
            $mid = $model['model'];
          }
          // go to the latest rendered revision
          $path = 'admin/smartdocs/models/' . $mid . '/revisions/' . $revision;
          unset($_GET['destination']);
          unset($_REQUEST['edit']['destination']);
          $message = 'In order to create a ‘' . $mid . '’ node, you must first create a method. Then render it into a node.';
          drupal_set_message($message, 'status');
          if (!(bool) check_plain($_GET['rawNode'])) {
            drupal_goto($path);
          }
        }
      }
    }
  }
}

/**
 * Implements hook_preprocess_node().
 */
function smartdocs_preprocess_node(&$vars) {
  global $base_url;
  $models = array();
  $types = db_query('SELECT model FROM {smartdata} WHERE nid = :nid', array(':nid' => $vars['nid']));
  foreach ($types as $type) {
    $models[smartdocs_model_name($type->model)] = $type->model;
  }

  if (in_array($vars['type'], array_keys($models))) {

    if (!$vars['teaser']) {
      $templates = entity_get_controller('smartdocs_doc');
      $config = devconnect_get_org_settings();

      if ((bool) variable_get('use_api_proxy_url', 0)) {
        drupal_add_js(
            array(
          'devconnect_docgen' => array(
            'isDrupal' => TRUE,
            'apiModelBaseUrl' => variable_get('api_proxy_url'),
            'oauth2AuthUrl' => $base_url . '/smartdocs/oauth/callback',
          )
            ), 'setting');
      }
      else {
        drupal_add_js(
            array(
          'devconnect_docgen' => array(
            'isDrupal' => TRUE,
            'apiModelBaseUrl' => str_replace('/v1', '', $config['endpoint']),
            'oauth2AuthUrl' => $base_url . '/smartdocs/oauth/callback',
            'dataProxyUrl' => variable_get('data_proxy_url', 'https://apiconsole-prod.apigee.net/smartdocs/v1'),
            'dataAuthUrl' => variable_get('data_auth_url', 'https://api.enterprise.apigee.com/v1/users/{user}/authenticate'),
          )
            ), 'setting');
      }

      $query = db_select('smartdata', 's');
      $query->condition('s.nid', $vars['node']->nid);
      $query->fields('s', array('nid', 'revision', 'resource', 'method', 'model'));

      $results = $query->execute();
      $data = array();
      foreach ($results as $result) {
        $data = array(
          'nid' => $result->nid,
          'revision' => $result->revision,
          'resource' => $result->resource,
          'operation' => $result->method,
        );
        $model = $result->model;
      }

      drupal_add_css(drupal_get_path('module', 'smartdocs') . '/css/smartdocs.css');

      if (!(bool) variable_get('use_local_assets', 0)) {
        $def_css = 'https://smartdocs.apigee.com/5/static/css/main.css
https://smartdocs.apigee.com/5/static/css/codemirror.css
https://smartdocs.apigee.com/5/static/css/prism.css';
        $def_js = "https://smartdocs.apigee.com/5/static/js/codemirror.js
https://smartdocs.apigee.com/5/static/js/codemirror_javascript.js
https://smartdocs.apigee.com/5/static/js/codemirror_xml.js
https://smartdocs.apigee.com/5/static/js/prism.js
https://smartdocs.apigee.com/5/static/js/base64_min.js
https://smartdocs.apigee.com/5/static/js/model.js
https://smartdocs.apigee.com/5/static/js/controller.js";

        $smart_mid = smartdocs_model_name($models[$vars['type']]);

        $js = preg_split("/\\r\\n|\\r|\\n/", variable_get($smart_mid . '_js', $def_js));
        $css = preg_split("/\\r\\n|\\r|\\n/", variable_get($smart_mid . '_css', $def_css));
        $i = 100;
        foreach ($js as $file) {
          $options = array(
            'group' => JS_THEME,
            'weight' => $i,
            'type' => 'external',
          );
          drupal_add_js($file, $options);
          $i++;
        }
        $i = 100;
        foreach ($css as $file) {
          $options = array(
            'group' => CSS_THEME,
            'weight' => $i,
            'type' => 'external',
          );
          drupal_add_css($file, $options);
          $i++;
        }
      }
      else {
        $version = variable_get('local_asset_version', '5');
        $path = drupal_get_path('module', 'smartdocs');
        $css = array(
          $path . '/local/css/v' . $version . '/main.css',
          $path . '/local/css/v' . $version . '/codemirror.css',
          $path . '/local/css/v' . $version . '/prism.css',
        );
        $js = array(
          $path . '/local/js/v' . $version . '/codemirror.js',
          $path . '/local/js/v' . $version . '/codemirror_javascript.js',
          $path . '/local/js/v' . $version . '/codemirror_xml.js',
          $path . '/local/js/v' . $version . '/prism.js',
          $path . '/local/js/v' . $version . '/base64_min.js',
          $path . '/local/js/v' . $version . '/model.js',
          $path . '/local/js/v' . $version . '/controller.js',
        );
        $i = 100;
        foreach ($js as $file) {
          $options = array(
            'group' => JS_THEME,
            'weight' => $i,
          );
          drupal_add_js($file, $options);
          $i++;
        }
        $i = 100;
        foreach ($css as $file) {
          $options = array(
            'group' => CSS_THEME,
            'weight' => $i,
          );
          drupal_add_css($file, $options);
          $i++;
        }
      }
      $data = $templates->loadOperation((array) $data, $model, 'drupal-cms');
      $vars['content'] = array();
      $vars['content']['#markup'] = $data;
    }
    else {
      $vars['content']['#markup'] = '<div class="col-sm-12">' . theme('smartdocs_method', $vars) . '</div>';
      if (isset($vars['content']['links'])) {
        unset($vars['content']['links']);
      }
    }
    if ($vars['teaser']) {
      $vars['title'] = '';
    }
  }
}

/**
 * Implements hook_preprocess_devconnect_developer_apps_list().
 */
function smartdocs_preprocess_devconnect_developer_apps_list(&$vars) {
  foreach ($vars['applications'] as $key => $app) {
    $products = array();
    foreach ($app['entity']->apiProducts as $product) {
      $products[$product] = $product;
    }
    foreach ($products as $product) {
      $models = variable_get($product . '_docs', array());
      foreach ($models as $model) {
        $query = db_select('smartdata', 's');
        $query->condition('s.model', $model);
        $query->fields('s', array('nid'));
        $results = $query->execute();
        $nodes = array();
        foreach ($results as $result) {
          $status = db_query('SELECT status FROM {node} WHERE nid = :nid', array(':nid' => $result->nid . $model))->fetchField();
          if ((bool) $status) {
            $nodes[$result->nid] = $result->nid;
          }
        }
        if (!empty($nodes)) {
          $display = db_query('SELECT title FROM {menu_router} WHERE path = :path', array(':path' => 'admin/smartdocs/models/' . $model))->fetchField();
          $vars['applications'][$key]['smartdocs'][$product][] = l($display . ' Documentation', $model . '/apis', array(
            'html' => TRUE,
            'attributes' => array(
              'target' => '_blank',
            )
          ));
        }
      }
    }
  }
}
