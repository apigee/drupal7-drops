<?php
/**
 * @file
 * Theme-related functions for SmartDocs nodes and terms.
 */

use Apigee\Exceptions\ResponseException;

/**
 * Sets breadcrumbs for taxonomy term pages related to SmartDocs vocabularies.
 *
 * @param object $term
 *   A taxonomy term.
 */
function _smartdocs_set_term_breadcrumb($term) {
  if (!$term || $term->vocabulary_machine_name != 'smartdocs_models') {
    return;
  }
  if (substr($term->vocabulary_machine_name, -8) != '_methods') {
    return;
  }
  $model_name = (empty($term->description) ? $term->name : $term->description);

  $breadcrumb = array(
    l(t('Home'), '<front>'),
    l(t('@model_name Documentation', array('@model_name' => $model_name)), $term->name . '/apis'),
  );
  drupal_set_breadcrumb($breadcrumb);
}

/**
 * Properly decorates SmartDocs node pages.
 *
 * @param array $vars
 *   Variables to be preprocessed for themeing.
 */
function _smartdocs_preprocess_node_page(&$vars) {
  $node = $vars['node'];

  $model_term = $node->field_smart_method_model[LANGUAGE_NONE][0]['taxonomy_term'];
  $model_id = $model_term->name;
  $revision_id = $node->field_smart_method_revision_num[LANGUAGE_NONE][0]['value'];
  if (property_exists($model_term, 'field_model_display_name') && !empty($model_term->field_model_display_name[LANGUAGE_NONE][0]['safe_value'])) {
    $model_name = $model_term->field_model_display_name[LANGUAGE_NONE][0]['safe_value'];
  }
  else {
    $model_name = $model_id;
  }

  // Set 'dna' var to let templates know this is Smartdocs doing the talking.
  // This is only used by the legacy theme.
  $vars['dna'] = TRUE;

  // If appropriate, set edit revision tab(s).
  if (array_key_exists('#primary', $vars['tabs']) && is_array($vars['tabs']['#primary']) && user_access('administer smartdocs content')) {
    $vars['tabs']['#primary'][] = array(
      '#theme' => 'menu_local_task',
      '#active' => FALSE,
      '#link' => array(
        'title' => t('View model revision'),
        'href' => 'admin/content/smartdocs/models/' . $model_id . '/revisions/' . $revision_id,
        'localized_options' => array(),
      ),
    );
  }

  // Set breadcrumbs.
  $breadcrumb = array(
    l(t('Home'), '<front>'),
    l(t('@model_name Documentation', array('@model_name' => $model_name)), 'taxonomy/term/' . $model_term->tid),
  );
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
  $arg = arg();
  if (count($arg) > 2 && $arg[0] == 'taxonomy' && $arg[1] == 'term' && is_numeric($arg[2])) {
    _smartdocs_set_term_breadcrumb(menu_get_object('taxonomy_term', 2));
  }
  elseif (count($arg) == 2 && $arg[0] == 'node' && is_numeric($arg[1]) && array_key_exists('node', $vars) && $vars['node']->type == 'smart_method') {
    _smartdocs_preprocess_node_page($vars);
  }

  // If SSL is not enabled, then show warning on the smartdocs node and
  // smartdocs admin pages.
  if ((array_key_exists('node', $vars) && $vars['node']->type == 'smart_method') || strstr(current_path(), 'admin/content/smartdocs') ||  strstr(current_path(), 'admin/content/smartdocs')) {
    if (user_access('administer smartdocs content') && !isset($_SERVER['HTTPS']) && !variable_get('smartdocs_disable_ssl_warning', FALSE)) {
      $path = current_path();
      $prologue = $message = t('SmartDocs pages are not secured with SSL.');
      if (array_key_exists('PANTHEON_ENVIRONMENT', $_SERVER)) {
        $message .= ' <em>' . l(t('Learn more.'), 'http://apigee.com/docs/developer-services/content/using-ssl-portal', array('attributes' => array('target' => '_blank'))) . '</em>.';
      }
      $message .= ' ' . t('It is strongly advised to use SmartDocs with HTTPS.') . '<br>';
      if ($path == 'admin/config/smartdocs') {
        $message .= t('You can disable this message below.');
      }
      else {
        $message .= t('You can disable this message at the !settings_page.', array(
          '!settings_page' => l(t('SmartDocs settings page'),
              'admin/config/smartdocs/settings',
              array(
                'attributes' => array(
                  'target' => '_blank',
                ),
              )),
          ));
      }
      // Avoid duplicating message, the hard way.
      if (isset($_SESSION['messages']['warning'])) {
        $prologue_len = strlen($prologue);
        foreach ($_SESSION['messages']['warning'] as $warning) {
          if (substr($warning, 0, $prologue_len) == $prologue) {
            return;
          }
        }
      }
      drupal_set_message($message, 'warning', FALSE);
    }
  }
}

/**
 * Implements hook_preprocess_node().
 */
function smartdocs_preprocess_node(&$vars) {

  if ($vars['type'] != 'smart_method') {
    return;
  }

  $model_term = $vars['field_smart_method_model'][0]['taxonomy_term'];

  if (!$vars['teaser']) {

    // Make SmartDocs work in themes that don't include bootstrap.
    $js = drupal_get_js();
    if (strpos($js, 'bootstrap') === FALSE) {
      drupal_add_js('//netdna.bootstrapcdn.com/twitter-bootstrap/3.0.2/js/bootstrap.min.js');
    }

    $org_settings = devconnect_get_org_settings();
    $js_settings = array(
      'oauth2AuthUrl' => rtrim(base_path(), '/') . '/smartdocs/oauth/callback',
      'apiModelBaseUrl' => preg_replace('!v1$!', '', rtrim($org_settings['endpoint'], '/')),
      'authUrl' => $org_settings['endpoint'] . '/users/{user}/authenticate',
      'proxyUrl' => variable_get('smartdocs_proxy_url', 'https://apiconsole-prod.apigee.net/smartdocs/v1/sendrequest'),
    );
    drupal_add_js(array('smartdocs' => $js_settings), 'setting');

    $script = 'var Apigee=Apigee||{};Apigee.APIModel=Apigee.APIModel||{};';
    $options = array(
      'type' => 'inline',
      'group' => JS_LIBRARY,
      'requires_jquery' => FALSE,
      'weight' => -9999,
    );
    drupal_add_js($script, $options);

    $version = variable_get('smartdocs_local_asset_version', '6');
    $path = drupal_get_path('module', 'smartdocs');
    $css = array(
      $path . '/local/css/main.css',
      $path . '/local/css/codemirror.css',
    );
    if ($version < 3) {
      // These versions declared element widths in px rather than %.
      $css[] = $path . '/local/css/fixed-widths.css';
    }
    elseif ($version == 3) {
      // This version declared h2 widths as a narrow percentage.
      $css[] = $path . '/local/css/narrow-h2.css';
    }

    $js = array(
      $path . '/local/js/codemirror.js',
      $path . '/local/js/codemirror_javascript.js',
      $path . '/local/js/codemirror_xml.js',
      $path . '/local/js/base64_min.js',
    );

    if (variable_get('smartdocs_use_prism', 1)) {
      $js[] = $path . '/local/js/prism.js';
      $css[] = $path . '/local/css/prism.css';
    }
    $js[] = $path . '/local/js/base64_min.js';
    $js[] = $path . '/local/js/v' . $version . '/model.js';
    $js[] = $path . '/local/js/v' . $version . '/controller.js';

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

    drupal_add_css(drupal_get_path('module', 'smartdocs') . '/css/smartdocs.css', array(
      'group' => CSS_THEME,
      'weight' => 200,
    ));

    $vars['content'] = array('#type' => 'markup');

    $content_markup = NULL;
    $write_cache = FALSE;
    // Pull markup from cache if it exists and is not expired.
    // Don't fail if updates haven't been run yet!
    try {
      $cached_data = cache_get('node:' . $vars['nid'], 'cache_smartdocs');
      if ($cached_data) {
        $content_markup = trim($cached_data->data);
      }
    }
    catch (Exception $e) {
      // Do nothing.
      // If we are using the default caching backend, and the cache_smartdocs
      // table does not yet exist, it will throw a PDOException which we catch
      // here.
    }

    if (empty($content_markup)) {
      // Cache miss (or no cache at all), so fetch from Modeling API.
      $model_id = $model_term->name;
      // Set default values.
      $revision_num = $resource_uuid = $uuid = NULL;
      foreach (array('revision_num', 'resource_uuid', 'uuid') as $field_name) {
        // Under some circumstances, there may be a LANGUAGE_NONE array level
        // in the field definition; in other circumstances, there may not be.
        $extended_field_name = 'field_smart_method_' . $field_name;
        if (array_key_exists(LANGUAGE_NONE, $vars[$extended_field_name])) {
          $$field_name = $vars[$extended_field_name][LANGUAGE_NONE][0]['value'];
        }
        else {
          $$field_name = $vars[$extended_field_name][0]['value'];
        }
      }
      $doc = new Apigee\SmartDocs\Doc(devconnect_default_org_config(), $model_id, $revision_num, $resource_uuid, $uuid);

      try {
        $content_markup = trim($doc->getHtml(SMARTDOCS_TEMPLATE_NAME));
        drupal_alter('smartdocs_method', $content_markup, $vars['node']);
        $write_cache = TRUE;
      }
      catch (ResponseException $e) {
        // Fall back to Teaser behavior, which doesn't require Modeling API
        // roundtrip. This is icky, but at least it shows SOMETHING to the end
        // user.
        $content_markup = theme('smartdocs_method', $vars);
        $vars['content']['#prefix'] = '<div class="col-sm-12">';
        $vars['content']['#suffix'] = '</div>';
      }
    }

    if ($write_cache && !empty($content_markup)) {
      $expire_timestamp = REQUEST_TIME + intval(variable_get('smartdocs_node_cache_ttl', SMARTDOCS_DEFAULT_METHOD_CACHE_TTL) * 60);
      // Don't fail if updates haven't been run yet!
      try {
        cache_set('node:' . $vars['nid'], $content_markup, 'cache_smartdocs', $expire_timestamp);
      }
      catch (Exception $e) {
        // Do nothing.
        // If we are using the default caching backend, and the cache_smartdocs
        // table does not yet exist, it will throw a PDOException which we
        // catch here.
      }
    }
    $vars['content']['#markup'] = $content_markup;
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

/**
 * Implements hook_preprocess_HOOK() for devconnect_developer_apps_list().
 *
 * If an API Product is configured to correspond to one or more models, adds a
 * link to each model's documentation on the developer app listing page.
 */
function smartdocs_preprocess_devconnect_developer_apps_list(&$vars) {
  // Initialize a cache of already-loaded terms.
  $model_terms = array();
  // Initialize a cache of already-queried models containing published nodes.
  $model_has_nodes = array();
  foreach ($vars['applications'] as &$app) {
    foreach ($app['entity']->apiProducts as $product) {
      // This variable may be set on the main SmartDocs config admin screen.
      $models = variable_get('smartdocs_' . $product . '_docs', array());
      foreach ($models as $model) {
        // Get taxonomy term corresponding to this model.
        if (array_key_exists($model, $model_terms)) {
          $model_term = $model_terms[$model];
        }
        else {
          $terms = taxonomy_get_term_by_name($model, 'smartdocs_models');
          if (empty($terms)) {
            // This can happen if the model has never been rendered.
            continue;
          }
          $model_term = reset($terms);
          $model_terms[$model] = $model_term;
        }
        // Are there any published nodes associated with this model?
        if (array_key_exists($model, $model_has_nodes)) {
          // We already queried for this model.
          $has_nodes = $model_has_nodes[$model];
        }
        else {
          // Hit the database to find at least one published node.
          $query = new EntityFieldQuery();
          $result = $query->entityCondition('entity_type', 'node')
            ->entityCondition('bundle', 'smart_method')
            ->fieldCondition('field_smart_method_model', 'tid', $model_term->tid, '=')
            ->propertyCondition('status', NODE_PUBLISHED)
            ->range(0, 1)
            ->execute();
          $has_nodes = (isset($result['node']) && !empty($result['node']));
          $model_has_nodes[$model] = $has_nodes;
        }
        if ($has_nodes) {
          // Yes, at least one published node exists.
          if (property_exists($model_term, 'field_model_display_name') && !empty($model_term->field_model_display_name[LANGUAGE_NONE][0]['value'])) {
            $title = $model_term->field_model_display_name[LANGUAGE_NONE][0]['value'];
          }
          else {
            $title = $model_term->name;
          }
          $link_title = t('@model documentation', array('@model' => $title));
          $app['smartdocs'][$product][] = l($link_title, 'taxonomy/term/' . $model_term->tid, array('attributes' => array('target' => '_blank')));
        }
      }
    }
  }
}

/**
 * Implements hook_preprocess_HOOK() for smartdocs_method().
 */
function smartdocs_preprocess_smartdocs_method(&$vars) {
  $terms = &drupal_static(__FUNCTION__, array());
  if (empty($terms)) {
    $vocab = taxonomy_vocabulary_machine_name_load('smartdocs_verbs');
    if ($vocab) {
      $terms = taxonomy_get_tree($vocab->vid);
      $terms = array_reduce($terms, function ($result, $item) {
        $result[$item->tid] = $item;
        return $result;
      }, array());
    }
  }
  $tid = $vars['field_smart_method_verb'][LANGUAGE_NONE][0]['tid'];
  $vars['path'] = $vars['field_smart_method_resource_path'][LANGUAGE_NONE][0]['value'];
  $vars['verb'] = array_key_exists($tid, $terms) ? $terms[$tid]->name : '';
  $vars['body'] = $vars['content']['body']['#items'][0]['value'];
}
