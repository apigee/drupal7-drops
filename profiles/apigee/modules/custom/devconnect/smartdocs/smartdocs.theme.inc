<?php
/**
 * @file
 * Theme-related functions for SmartDocs nodes and terms.
 */

use Apigee\Exceptions\ResponseException;

/**
 * Sets breadcrumbs for taxonomy term pages related to SmartDocs vocabularies.
 *
 * @param stdClass $term
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
  $vars['dna'] = TRUE;

  // If appropriate, set edit revision tab(s).
  if (array_key_exists('#primary', $vars['tabs']) && user_access('administer smartdocs content')) {
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
    if (user_access('administer smartdocs content') && !isset($_SERVER['HTTPS']) && !variable_get('disable_ssl_warning', FALSE)) {
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
    $org_settings = devconnect_get_org_settings();

    if ((bool) variable_get('use_api_proxy_url', 0)) {
      drupal_add_js(
        array(
          'smartdocs' => array(
            'isDrupal' => TRUE,
            'apiModelBaseUrl' => variable_get('api_proxy_url'),
            'oauth2AuthUrl' => base_path() . '/smartdocs/oauth/callback',
          ),
        ), 'setting');
    }
    else {
      drupal_add_js(
        array(
          'smartdocs' => array(
            'isDrupal' => TRUE,
            'apiModelBaseUrl' => str_replace('/v1', '', $org_settings['endpoint']),
            'oauth2AuthUrl' => base_path() . '/smartdocs/oauth/callback',
            'dataProxyUrl' => variable_get('data_proxy_url', 'https://apiconsole-prod.apigee.net/smartdocs/v1'),
            'dataAuthUrl' => variable_get('data_auth_url', 'https://api.enterprise.apigee.com/v1/users/{user}/authenticate'),
          ),
        ), 'setting');
    }

    if (!(bool) variable_get('use_local_assets', 0)) {
      $def_css = 'https://smartdocs.apigee.com/6/static/css/main.css
https://smartdocs.apigee.com/6/static/css/codemirror.css
https://smartdocs.apigee.com/6/static/css/prism.css';
      $def_js = "https://smartdocs.apigee.com/6/static/js/codemirror.js
https://smartdocs.apigee.com/6/static/js/codemirror_javascript.js
https://smartdocs.apigee.com/6/static/js/codemirror_xml.js
https://smartdocs.apigee.com/6/static/js/prism.js
https://smartdocs.apigee.com/6/static/js/base64_min.js
https://smartdocs.apigee.com/6/static/js/model.js
https://smartdocs.apigee.com/6/static/js/controller.js";

      $js = preg_split('/\\r\\n|\\r|\\n/', variable_get($model_term->name . '_js', $def_js));
      $css = preg_split('/\\r\\n|\\r|\\n/', variable_get($model_term->name . '_css', $def_css));
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
      $version = variable_get('local_asset_version', '6');
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

    drupal_add_css(drupal_get_path('module', 'smartdocs') . '/css/smartdocs.css', array(
      'group' => CSS_THEME,
      'weight' => 200,
    ));
    $model_id = $model_term->name;
    $revision_num = $vars['field_smart_method_revision_num'][LANGUAGE_NONE][0]['value'];
    $resource_uuid = $vars['field_smart_method_resource_uuid'][LANGUAGE_NONE][0]['value'];
    $method_uuid = $vars['field_smart_method_uuid'][LANGUAGE_NONE][0]['value'];

    $doc = new Apigee\SmartDocs\Doc(devconnect_default_org_config(), $model_id, $revision_num, $resource_uuid, $method_uuid);

    try {
      $content_markup = $doc->getHtml(SMARTDOCS_TEMPLATE_NAME);
      $vars['content'] = array('#markup' => $content_markup);
    }
    catch (ResponseException $e) {
      // Fall back to Teaser behavior, which doesn't require Modeling API
      // roundtrip. This is icky, but at least it shows SOMETHING to the end
      // user.
      $vars['content'] = array(
        '#type' => 'markup',
        '#markup' => theme('smartdocs_method', $vars),
        '#prefix' => '<div class="col-sm-12">',
        '#suffix' => '</div>',
      );
    }
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
      $models = variable_get($product . '_docs', array());
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
          $link_title = t(':model documentation', array(':model' => smartdocs_taxonomy_term_title($model_term)));
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
  $vars['path'] = $vars['field_smart_method_resource_path'][0]['value'];
  $vars['verb'] = $vars['field_smart_method_verb'][0]['taxonomy_term']->name;
  $vars['body'] = $vars['content']['body']['#items'][0]['value'];
}
