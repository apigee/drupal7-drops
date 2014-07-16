<?php

/**
 * Provides authentication with Apigee when trying to access the API.  User must a valid org admin user.
 *
 * @param $user
 * @param $pass
 * @param int $timeout
 * @return bool
 */
function _devconnect_docgen_validate_credentials($user, $pass, $timeout = 4) {
  $config =& devconnect_get_org_settings();
  $org = $config['org'];
  $endpoint = $config['endpoint'];
  $timeout = intval(min($timeout, 8));
  if ($timeout < 1) {
    $timeout = 4;
  }
  $options = array(
    'http_options' => array(
      'connection_timeout' => $timeout,
      'follow_location' => FALSE,
    ),
    'user_agent' => 'devconnect_docgen/validate_credentials'
  );
  $config = new Apigee\Util\OrgConfig($org, $endpoint, $user, $pass, $options);
  $org = new Apigee\ManagementAPI\Organization($config);
  try {
    $org->load();
    return true;
  }
  catch (Exception $e) {
    return false;
  }
}

/**
 * Provides an API for updating Drupal nodes after editMode submissions
 *
 * @param $method
 */
function devconnect_docgen_api_update($method) {
  if (isset($_SERVER["PHP_AUTH_USER"]) && isset($_SERVER["PHP_AUTH_PW"])) {
    $user = $_SERVER["PHP_AUTH_USER"];
    $pass = $_SERVER["PHP_AUTH_PW"];
    if (_devconnect_docgen_validate_credentials($user, $pass)) {
      $postdata = file_get_contents("php://input");
      $query = db_select('smartdocs', 's');
      $query->condition('s.method', $method);
      $query->fields('s', array('sid', 'nid', 'revision', 'resource', 'method', 'model'));
      $result = $query->execute();
      $data = new stdClass();
      foreach ($result as $record) {
        $data = $record;
      }
      if ($data->method == $method) {
        if (smartdocs_is_json($postdata)) {
          $updates = drupal_json_decode($postdata);
          $nid = $data->nid;
          $mid = $data->model;
          $node = node_load($nid);
          if (is_array($updates)) {
            foreach ($updates as $title => $update) {
              switch($title) {
                case 'title':
                  if (isset($updates['title'])) {
                    $node->title = $updates['title'];
                  }
                  break;
                case 'description':
                  if (isset($updates['description'])) {
                    $node->body[LANGUAGE_NONE][0]['value'] = $updates['description'];
                    $node->body[LANGUAGE_NONE][0]['summary'] = $updates['description'];
                    $node->body[LANGUAGE_NONE][0]['format'] = 'full_html';
                  }
                  break;
                case 'path':
                  if (isset($updates['path'])) {
                    $field_name = 'field_' . _devconnect_docgen_model_field_name($mid) . '_model_path';
                    $field =& $node->$field_name;
                    $field[LANGUAGE_NONE][0]['value'] = $updates['path'];
                    $field[LANGUAGE_NONE][0]['safe_value'] = $updates['path'];
                  }
                  break;
                case 'types':
                  if (isset($updates['types']) && is_array($updates['types'])) {
                    $field_name = 'field_' . _devconnect_docgen_model_field_name($mid) . '_model_types';
                    $field =& $node->$field_name;
                    $i = 0;
                    foreach ($updates['types'] as $type) {
                      $term = taxonomy_get_term_by_name($type, _devconnect_docgen_model_name($mid) . '_types');
                      if (isset($term->tid)) {
                        $field[$i]->set(intval($term->tid));
                      }
                      $i++;
                    }
                  }
                  break;
                case 'verbs':
                  if (isset($updates['verbs']) && is_array($updates['verbs'])) {
                    $field_name = 'field_' . _devconnect_docgen_model_field_name($mid) . '_model_verb';
                    $field =& $node->$field_name;
                    $i = 0;
                    foreach ($updates['verbs'] as $verb) {
                      $term = taxonomy_get_term_by_name(strtoupper($verb), _devconnect_docgen_model_name($mid) . '_methods');
                      if (isset($term->tid)) {
                        $field[$i]->set(intval($term->tid));
                      }
                      $i++;
                    }
                  }
                  break;
                case 'auths':
                  if (isset($updates['auth']) && is_array($updates['auths'])) {
                    $field_name = 'field_' . _devconnect_docgen_model_field_name($mid) . '_model_authschemes';
                    $field =& $node->$field_name;
                    $i = 0;
                    foreach ($updates['auths'] as $auth) {
                      $term = taxonomy_get_term_by_name(strtoupper($auth), _devconnect_docgen_model_name($mid) . '_auth_schemes');
                      if (isset($term->tid)) {
                        $field[$i]->set(intval($term->tid));
                      }
                      $i++;
                    }
                  }
                  break;
                case 'tags':
                  if (isset($updates['tags']) && is_array($updates['tag'])) {
                    $field_name = 'field_' . _devconnect_docgen_model_field_name($mid) . '_model_tags';
                    $field =& $node->$field_name;
                    $i = 0;
                    foreach ($updates['tags'] as $tag) {
                      $term = taxonomy_get_term_by_name(strtoupper($tag), _devconnect_docgen_model_name($mid) . '_auth_schemes');
                      if (isset($term->tid)) {
                        $field[$i]->set(intval($term->tid));
                      }
                      $i++;
                    }
                  }
                  break;
                default:
                  break;
              }
            }
          }
          node_save($node);
          drupal_add_http_header('status', 200);
          $data = array(
            'code' => 200,
            'message' => 'Linked node has been saved successfully',
          );
        } else {
          drupal_add_http_header('status', 500);
          $data = array(
            'error' => (json_last_error() == 4) ? 'malformed JSON request' : json_last_error(),
          );
        }
      } else {
        drupal_add_http_header('status', 500);
        $data = array(
          'code' => 500,
          'message' => 'Method doesn\'t exist on the Drupal installation.'
        );
      }
    } else {
      drupal_add_http_header('status', 401);
      $data = array(
        'code' => 401,
        'message' => 'Authentication Failed'
      );
    }
  } else {
    drupal_add_http_header('status', 500);
    $data = array(
      'code' => 500,
      'message' => 'Credentials are missing'
    );
  }
  drupal_json_output($data);
}


/**
 * Provides an API for updating "syncing" Drupal nodes from Smartdocs UI
 *
 * @param $method
 */
function devconnect_docgen_sync_api_update($method) {
  $query = db_select('smartdocs', 's');
  $query->condition('s.method', $method);
  $query->fields('s', array('sid', 'nid', 'revision', 'resource', 'method', 'model'));
  $result = $query->execute();
  foreach ($result as $record) {
    $data_json = $record;
  }
  $data = (array) $data_json;
  $data['operation'] = $method;

  $drupal_update = 1;
  $templates = entity_get_controller('docgen_doc');
  $raw_data = $templates->loadOperation($data, $data['model'], '', $drupal_update);

  $updates = drupal_json_decode($raw_data);
  $nid = $data['nid'];
  $mid = $data['model'];
  $node = node_load($nid);

  if (is_array($updates)) {
    foreach ($updates as $title => $update) {

      switch($title) {
        case 'displayName':
          if (isset($updates['displayName'])) {
            $node->title = $updates['displayName'];
          }
          break;
        case 'description':
          if (isset($updates['description'])) {
            $node->body[LANGUAGE_NONE][0]['value'] = $updates['description'];
            $node->body[LANGUAGE_NONE][0]['summary'] = $updates['description'];
            $node->body[LANGUAGE_NONE][0]['format'] = 'full_html';
          }
          break;
        case 'path':
          if (isset($updates['path'])) {
            $field_name = 'field_' . _devconnect_docgen_model_field_name($mid) . '_model_path';
            $field =& $node->$field_name;
            $field[LANGUAGE_NONE][0]['value'] = $updates['path'];
            $field[LANGUAGE_NONE][0]['safe_value'] = $updates['path'];
          }
          break;
        case 'types':
          // TODO: need to update types...
          break;
        case 'verb':
          // Making assumption each method contains only one VERB
          if (isset($updates['verb'])) {
            $field_name = 'field_' . _devconnect_docgen_model_field_name($mid) . '_model_verb';
            $field =& $node->$field_name;
            $verb = $updates['verb'];
            $term = taxonomy_get_term_by_name(strtoupper($verb), _devconnect_docgen_model_name($mid) . '_methods');
            if (isset(current($term)->tid)) {
              $field[LANGUAGE_NONE][0]['tid'] = intval(current($term)->tid);
            }
          }
          break;
        case 'authSchemes':
          if (isset($updates['authSchemes'])) {
            $field_name = 'field_' . _devconnect_docgen_model_field_name($mid) . '_model_authschemes';
            $field =& $node->$field_name;
            $i = 0;
            foreach ($updates['authSchemes'] as $auth) {
              $term = taxonomy_get_term_by_name(strtoupper($auth), _devconnect_docgen_model_name($mid) . '_auth_schemes');
              if (isset(current($term)->tid)) {
                $field[LANGUAGE_NONE][$i]['tid'] = intval(current($term)->tid);
              }
              $i++;
            }
          }
          break;
        case 'tags':
          if (isset($updates['tags'])) {
            $field_name = 'field_' . _devconnect_docgen_model_field_name($mid) . '_model_tags';
            $field =& $node->$field_name;
            $i = 0;
            foreach ($updates['tags'] as $tag) {
              $term = taxonomy_get_term_by_name(strtoupper($tag), _devconnect_docgen_model_name($mid) . '_tags');
              if (isset(current($term)->tid)) {
                $field[LANGUAGE_NONE][$i]['tid'] = intval(current($term)->tid);
              }
              $i++;
            }
          }
          break;
        default:
          break;
      }
    }

    // Make this change a new revision
    $node->revision = 1;
    $node->log = t('Method @title - @method was synced from SmartDocs.', array('@title' => $updates['displayName'], '@method' => $method));
    node_save($node);
  } else {
    drupal_set_message(t('Method @method doesn\'t exist on the Drupal installation.', array('@method' => $method)),'error');
  }
}


/**
 * Implements hook_form_alter()
 *
 * @param $form
 * @param $form_state
 * @param $form_id
 * @param array $parameters
 */
function devconnect_docgen_form_alter(&$form, &$form_state, $form_id) {
  $models = variable_get('devconnect_docgen_content_types', array());
  if (isset($form['#node']) && in_array($form['#node']->type, array_keys($models))) {
    $field = variable_get($form['#node']->type . '_field_type');
    // disable revision to avoid sync mismatch
    $form['field_' . $field . '_model_revisions']['#disabled'] = TRUE;
    $form['field_' . $field . '_model_path']['#disabled'] = TRUE;
    $form['actions']['submit']['#submit'][] = '_devconnect_docgen_flush_node_redirect';
    $form['#validate'][] = '_devconnect_docgen_smartdoc_update';
  }
}


/**
 * Custom validate function for all smartdocs node edit forms
 */
function _devconnect_docgen_smartdoc_update($form, $form_state) {
  if (strtolower($form_state['triggering_element']['#value']) == t('delete')) {
    return;
  }
  $node = (object)$form_state['values'];
  $model = variable_get($node->type, $node->type);
  $field = variable_get($model . '_field_type');
  if ($field) {
    $element = array();
    $revisions = 'field_' . $field . '_model_revisions';
    $rev =& $node->$revisions;
    $types = 'field_' . $field . '_model_types';
    $ty =& $node->$types;
    $path = 'field_' . $field . '_model_path';
    $pa =& $node->$path;
    $verbs = 'field_' . $field . '_model_verb';
    $ve =& $node->$verbs;
    $auth = 'field_' . $field . '_model_authschemes';
    $au =& $node->$auth;
    $tags = 'field_' . $field . '_model_tags';
    $ta =& $node->$tags;
    if (!empty($rev[LANGUAGE_NONE])) {
      foreach($rev[LANGUAGE_NONE] as $key => $revision) {
        $element['revision'] = taxonomy_term_load($rev[LANGUAGE_NONE][$key]['tid'])->name;
      }
    }
    if (!empty($ty[LANGUAGE_NONE])) {
      foreach($ty[LANGUAGE_NONE] as $key => $type) {
        $element['type'][] = taxonomy_term_load($ty[LANGUAGE_NONE][$key]['tid'])->name;
      }
    }
    $element['path'] = $pa[LANGUAGE_NONE][0]['value'];
    if (!empty($ve[LANGUAGE_NONE])) {
      foreach($ve[LANGUAGE_NONE] as $key => $verb) {
        $element['verb'] = taxonomy_term_load($ve[LANGUAGE_NONE][$key]['tid'])->name;
      }
    }
    if (!empty($au[LANGUAGE_NONE])) {
      foreach($au[LANGUAGE_NONE] as $key => $auth) {
        $element['auth'][] = taxonomy_term_load($au[LANGUAGE_NONE][$key]['tid'])->name;
      }
    }
    if (!empty($ta[LANGUAGE_NONE])) {
      foreach($ta[LANGUAGE_NONE] as $key => $tag) {
        $element['tags'][] = taxonomy_term_load($ta[LANGUAGE_NONE][$key]['tid'])->name;
      }
    }
    $mid = _devconnect_docgen_model_name($node->type);
    $revision = entity_get_controller('docgen_revision')->loadVerbose($mid, $element['revision']);
    $nodes = array_flip(variable_get($mid . '_nodes', array()));
    $id = $nodes[$node->nid];
    foreach ($revision['resources'] as $resource) {
      if (isset($resource['methods'])) {
        foreach($resource['methods'] as $method) {
          if ($method['id'] == $id) {
            $element['rid'] = $resource['id'];
            $element['method_id'] = $method['id'];
            $element['method_full'] = $method;
          }
        }
      }
    }
    $element['title'] = $node->title;
    $element['body'] = $node->body[LANGUAGE_NONE][0]['value'];
    entity_get_controller('docgen_operation')->updateMethod($mid, $element);
  }
}

/**
 * Custom submit callback for smartdocs nodes, ensures cache is flushed
 */
function _devconnect_docgen_flush_node_redirect($form, &$form_state) {
  if (strtolower($form_state['triggering_element']['#value']) == 'delete') {
    $form_state['redirect'] = array(
      'admin/content/smartdocs',
    );
    $node = $form['#node'];
    try {
      node_delete($node->nid);
      $mid = _devconnect_docgen_model_name($node->type);
      $nodes = array_flip(variable_get($mid . '_nodes', array()));
      $id = $nodes[$node->nid];
      _devconnect_docgen_sync_nodes($node->nid, $id, $mid, 'delete');
      $text = t('%t has been deleted', array('%t' => $node->title));
      drupal_set_message($text, 'success');
    } catch (Exception $e) {
      $text = t('%t could not be been deleted.', array('%t' => $node->title));
      drupal_set_message($text, 'error');
    }
  } else {
    $form_state['redirect'] = array(
      'node/' . $form['#node']->nid,
      array(
        'query' => array(
          'flush' => '1',
          'drupal_edit' => 1,
        ),
      ),
    );
  }
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
 * Implements hooks preprocess html().
 *
 * @param $vars
 */
function devconnect_docgen_preprocess_html(&$vars) {
  if ($node = menu_get_object()) {
    $models = variable_get('devconnect_docgen_content_types', array());
    if (in_array($node->type, array_keys($models))) {
      $vars['classes_array'][] = 'smartdocs';
    }
  }
}

/**
 * Implements preprocess_page()
 *
 * @param $vars
 */
function devconnect_docgen_preprocess_page(&$vars) {
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
  $models = variable_get('devconnect_docgen_content_types', array());
  if (isset($vars['node']) && in_array($vars['node']->type, array_keys($models))) {
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
    $field_name = 'field_' . _devconnect_docgen_model_field_name($vars['node']->type) . '_model_verb';
    $verb =& $vars['node']->$field_name;
    if (isset($verb[LANGUAGE_NONE][0]['taxonomy_term'])) {
      $breadcrumb[] = l($verb[LANGUAGE_NONE][0]['taxonomy_term']->name, 'taxonomy/term/' . $verb[LANGUAGE_NONE][0]['taxonomy_term']->tid);
    }
    drupal_set_breadcrumb($breadcrumb);
  }
}


/**
 * Implements preprocess_node()
 *
 * @param $vars
 */
function devconnect_docgen_preprocess_node(&$vars) {
  global $base_url;
  $models = variable_get('devconnect_docgen_content_types', array());
  if (in_array($vars['type'], array_keys($models))) {
    $model = variable_get('use_docgen_templates', 1);
    if (!(bool)$model) {
      $schemes = array();
      if (isset($vars['field_' . _devconnect_docgen_model_field_name($vars['type']) . '_model_authschemes']) && !empty($vars['field_' . _devconnect_docgen_model_field_name($vars['type']) . '_model_authschemes'])) {
        foreach ($vars['field_' . _devconnect_docgen_model_field_name($vars['type']) . '_model_authschemes'] as $scheme) {
          $schemes[] = $scheme['taxonomy_term']->name;
        }
      }
      if (!empty($schemes)) {
        $vars['schemes'] = implode(', ', $schemes);
      }
      $types = array();
      if (isset($vars['field_' . _devconnect_docgen_model_field_name($vars['type']) . '_model_types']) && !empty($vars['field_' . _devconnect_docgen_model_field_name($vars['type']) . '_model_types'])) {
        foreach ($vars['field_' . _devconnect_docgen_model_field_name($vars['type']) . '_model_types'] as $type) {
          $types[] = $type['taxonomy_term']->name;
        }
      }
      if (!empty($types)) {
        $vars['types'] = implode(', ', $types);
      }
      $tags = array();
      if (isset($vars['field_' . _devconnect_docgen_model_field_name($vars['type']) . '_model_tags']) && !empty($vars['field_' . _devconnect_docgen_model_field_name($vars['type']) . '_model_tags'])) {
        foreach ($vars['field_' . _devconnect_docgen_model_field_name($vars['type']) . '_model_tags'] as $tag) {
          $tags[] = $tag['taxonomy_term']->name;
        }
      }
      if (!empty($tags)) {
        $vars['terms'] = implode(', ', $tags);
      }
      $vars['date'] = date('j F, Y', $vars['node']->created);
      if (isset($vars['field_' . _devconnect_docgen_model_field_name($vars['type']) . '_model_verb']) && !empty($vars['field_' . _devconnect_docgen_model_field_name($vars['type']) . '_model_verb'])) {
        $vars['verb'] = $vars['field_' . _devconnect_docgen_model_field_name($vars['type']) . '_model_verb'][0]['taxonomy_term']->name;
      }
      $vars['description'] = (isset($vars['body'][0]['value']) && !empty($vars['body'][0]['value'])) ? $vars['body'][0]['value'] : '';
      if (isset($vars['field_' . _devconnect_docgen_model_field_name($vars['type']) . '_model_path']) && !empty($vars['field_' . _devconnect_docgen_model_field_name($vars['type']) . '_model_path'][0]['value'])) {
        $vars['path'] = $vars['field_' . _devconnect_docgen_model_field_name($vars['type']) . '_model_path'][0]['value'];
      }

      $vars['content']['#markup'] = theme('operation_template', $vars);
    } else {
      if (!$vars['teaser']) {
        $templates = entity_get_controller('docgen_doc');
        $config =& devconnect_get_org_settings();

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

        if (!(bool)variable_get('use_local_assets', 0)) {
          $ver = variable_get(_devconnect_docgen_model_name($model) . '_bootstrap_ver', '0');
          switch($ver) {
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
          $js = preg_split("/\\r\\n|\\r|\\n/", variable_get(_devconnect_docgen_model_name($model) . '_js', $def_js));
          $css = preg_split("/\\r\\n|\\r|\\n/", variable_get(_devconnect_docgen_model_name($model) . '_css', $def_css));
          foreach($js as $file) {
            drupal_add_js($file);
          }
          foreach($css as $file) {
            drupal_add_css($file);
          }
        } else {
          $path = drupal_get_path('module', 'devconnect_docgen');
          $css = array(
            $path . '/css_apigee/main.css',
            $path . '/css_apigee/codemirror.css',
            $path . '/css_apigee/prism.css',
          );
          $js = array(
            $path . '/js_apigee/codemirror.js',
            $path . '/js_apigee/codemirror_javascript.js',
            $path . '/js_apigee/codemirror_xml.js',
            $path . '/js_apigee/prism.js',
            $path . '/js_apigee/base64_min.js',
            $path . '/js_apigee/model.js',
            $path . '/js_apigee/controller.js',
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
        $regex = '!(\[node:([0-9]+)\])!';
        $replacements = array();
        if (preg_match_all($regex, $data, $matches, PREG_SET_ORDER)) {
          foreach ($matches as $match) {
            $replacements[$match[1]] = $match[2];
          }
        }
        foreach($replacements as $token => $nid) {
          $nodes = db_query('SELECT nid, title FROM {node} WHERE nid = :nid', array(':nid' => $nid));
          foreach($nodes as $node) {
            $link = l($node->title, 'node/' . $node->nid);
            $data = str_replace($token, $link, $data);
          }
        }
        $vars['content']['#markup'] = $data;
      } else {
        $vars['content']['#markup'] = '<div class="col-sm-12">' . theme('operation_template', $vars) . '</div>';
        if (isset($vars['content']['links'])) {
          unset($vars['content']['links']);
        }
      }
    }
    if ($vars['teaser']) {
      $vars['title'] = '';
    }
  }
}