<?php

/**
 * Page Callback for admin/smartdocs/model/%name
 *
 * @param $form
 * @param $form_state
 * @param $mid
 * @return mixed
 */
function smartdocs_revision_detail($form, $form_state, $mid) {
  $model = entity_get_controller('smartdocs_model')->loadSingle($mid);
  $revisions = entity_get_controller('smartdocs_revision')->getAllRevisions($mid);

  drupal_set_title('‘' . $model['displayName'] . '’ API Revisions');

  $rows = array();
  foreach($revisions as $revision) {
    $rows[$revision['revisionNumber']] = $revision;
  }
  ksort($rows);
  $form['model'] = array(
    '#data' => $model
  );
  foreach($rows as $item) {
    $form['revisions'][$item['id']] = array(
      '#type' => 'row',
      '#data' => $item
    );
    $operations = array(
      '' => 'Revision Details',
      '/edit' => 'Edit Revision',
      '/auth' => 'Auth Settings',
    );
    $links = array();
    foreach($operations as $key => $op) {
      $links[$key . '-' . $item['id']] = array(
        'title' => $op,
        'href' => 'admin/smartdocs/models/' . $model['name'] . '/revisions/' . $item['revisionNumber'] . $key,
        'attributes' => array(
          'title' => $op,
          'class' => array(
            $key . '-' . $item['id'],
            'views-ajax-link'
          ),
        ),
      );
    }
    $form['revisions'][$item['id']]['operations'] = array(
      '#markup' => theme('links__ctools_dropbutton',
        array(
          'links' => $links,
        )
      ),
    );
  }
  $form['#theme'] = 'smartdocs_revisions_table';
  return $form;
}

/**
 * Page Callback for admin/smartdocs/model/%name
 *
 * @param $form
 * @param $form_state
 * @param $mid
 * @return mixed
 */
function smartdocs_add_revision($form, $form_state, $mid) {
  $model = entity_get_controller('smartdocs_model')->loadSingle($mid);
  drupal_set_title('Add API Revision to ‘' . $model['displayName'] . '’');
  $form['model'] = array(
    '#type' => 'hidden',
    '#value' => $model,
  );
  $form['display_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Display Name'),
    '#size' => 60,
    '#maxlength' => 128,
    '#required' => TRUE,
    '#attributes' => array(
      'placeholder' => t('New Revision'),
    )
  );
  $form['version'] = array(
    '#type' => 'textfield',
    '#title' => t('Version ID'),
    '#size' => 60,
    '#maxlength' => 128,
    '#required' => TRUE,
    '#attributes' => array(
      'placeholder' => t('v1'),
    )
  );
  $form['description'] = array(
    '#type' => 'textarea',
    '#title' => t('Description'),
    '#rows' => 5,
    '#required' => TRUE,
  );
  $form['base_url'] = array(
    '#type' => 'textfield',
    '#title' => t('Base URL'),
    '#size' => 60,
    '#maxlength' => 128,
    '#required' => TRUE,
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Add API Revision'),
  );
  return $form;
}

/**
 * Validation Callback for smartdocs_add_revision
 *
 * @param $form
 * @param $form_state
 * @param $mid
 * @return mixed
 */
function smartdocs_add_revision_validate($form, &$form_state, $mid) {
  if (!(filter_var($form_state['values']['base_url'], FILTER_VALIDATE_URL))){
    form_set_error('base_url', 'Base URL needs to be a valid URL.');
  }
}

/**
 * Submit Callback for smartdocs_add_revision
 *
 * @param $form
 * @param $form_state
 * @param $mid
 * @return mixed
 */
function smartdocs_add_revision_submit($form, &$form_state) {
  $payload = array(
    'displayName' => $form_state['values']['display_name'],
    'description' => $form_state['values']['description'],
    'releaseVersion' => $form_state['values']['version'],
    'baseUrl' => $form_state['values']['base_url'],
  );
  $payload = drupal_json_encode($payload);
  $mid = $form_state['values']['model']['name'];
  if ($revision = entity_get_controller('smartdocs_revision')->newRevision($mid, $payload)) {
    drupal_set_message('Revision was created successfully');
    $form_state['redirect'] = 'admin/smartdocs/models/' . $mid .'/revisions/' . $revision['revisionNumber'];
  }
}

/**
 * Authentication Settings for Specific Revision
 *
 * @param $form
 * @param $form_state
 * @param $mid
 * @param $rev
 * @return array
 */
function smartdocs_revision_auth($form, &$form_state, $mid, $rev) {

  drupal_set_title('Authentication Settings for API Revision #' . $rev);

  $crumb = drupal_get_breadcrumb();
  array_pop($crumb);
  $crumb[] = l('#' . $rev, 'admin/smartdocs/models/' . $mid .'/revisions/' . $rev);
  $crumb[] = 'Authentication Settings';
  drupal_set_breadcrumb($crumb);

  $form['model'] = array(
    '#type' => 'hidden',
    '#value' => $mid,
  );
  $form['rev'] = array(
    '#type' => 'hidden',
    '#value' => $rev,
  );

  // OAuth
  $OAuth = entity_get_controller('smartdocs_revision')->getOAuthCredentials($mid, $rev);

  $form['oauth_action'] = array(
    '#type' => 'hidden',
    '#value' => 'edit',
  );

  if (empty($OAuth)) {
    $form['oauth_action'] = array(
      '#type' => 'hidden',
      '#value' => 'add',
    );
  }

  $form['oauth'] = array(
    '#type' => 'fieldset',
    '#title' => t('OAuth 2.0 Web Server Flow'),
    '#prefix' => '<div id="oauth-config">',
    '#suffix' => '</div>',
    '#collapsed' => TRUE,
    '#collapsible' => TRUE,
    '#tree' => TRUE,
  );
  $form['oauth']['clientId'] = array(
    '#type' => 'textfield',
    '#title' => t('Client ID'),
    '#default_value' => (isset($OAuth['clientId'])) ? $OAuth['clientId'] : '',
  );
  $form['oauth']['clientSecret'] = array(
    '#type' => 'textfield',
    '#title' => t('Client Secret'),
    '#default_value' => (isset($OAuth['clientSecret'])) ? $OAuth['clientSecret'] : '',
  );
  $form['oauth']['authorizationUrl'] = array(
    '#type' => 'textfield',
    '#title' => t('Authorization URL'),
    '#default_value' => (isset($OAuth['authorizationUrl'])) ? $OAuth['authorizationUrl'] : '',
  );
  $form['oauth']['authorizationVerb'] = array(
    '#type' => 'radios',
    '#title' => t('Authorization Verb'),
    '#default_value' => (isset($OAuth['authorizationVerb'])) ? $OAuth['authorizationVerb'] : '',
    '#options' => array(
      'GET' => 'GET',
      'POST' => 'POST',
    ),
  );
  $form['oauth']['accessTokenUrl'] = array(
    '#type' => 'textfield',
    '#title' => t('Access Token URL'),
    '#default_value' => (isset($OAuth['accessTokenUrl'])) ? $OAuth['accessTokenUrl'] : '',
  );
  $form['oauth']['accessTokenType'] = array(
    '#type' => 'radios',
    '#title' => t('Access Token Type'),
    '#default_value' => (isset($OAuth['accessTokenType'])) ? $OAuth['accessTokenType'] : '',
    '#options' => array(
      'header' => 'Header',
      'query' => 'Query',
    ),
  );
  $form['oauth']['accessTokenParamName'] = array(
    '#type' => 'textfield',
    '#title' => t('Access Token param name'),
    '#default_value' => (isset($OAuth['accessTokenParamName'])) ? $OAuth['accessTokenParamName'] : '',
  );
  $form['oauth']['clientAuthScheme'] = array(
    '#type' => 'radios',
    '#title' => t('Client Authentication Name'),
    '#default_value' => (isset($OAuth['clientAuthScheme'])) ? $OAuth['clientAuthScheme'] : '',
    '#options' => array(
      'header' => 'Header',
      'body' => 'Body',
    ),
  );
  $callback = variable_get($mid . '_' . $rev . '_callbackURL', null);
  if (isset($callback['callback'])) {
    $form['oauth']['callbackURL'] = array(
      '#type' => 'textfield',
      '#title' => t('Callback URL'),
      '#default_value' => $callback['callback'],
      '#disabled' => TRUE,
    );
  }

  $token = entity_get_controller('smartdocs_revision')->getTokenCredentials($mid, $rev);

  $form['token_action'] = array(
    '#type' => 'hidden',
    '#value' => 'edit',
  );

  if (empty($token)) {
    $form['token_action'] = array(
      '#type' => 'hidden',
      '#value' => 'add',
    );
  }

  $form['token'] = array(
    '#type' => 'fieldset',
    '#title' => t('Custom Token'),
    '#prefix' => '<div id="custom-token">',
    '#suffix' => '</div>',
    '#collapsed' => TRUE,
    '#collapsible' => TRUE,
    '#tree' => TRUE,
  );
  $form['token']['tokenType'] = array(
    '#type' => 'radios',
    '#title' => t('Token Type'),
    '#default_value' => (isset($token['tokenType'])) ? $token['tokenType'] : '',
    '#options' => array(
      'header' => 'Header',
      'query' => 'Query',
    ),
  );
  $form['token']['tokenMap'] = array(
    '#prefix' => '<div id="custom-token-map">',
    '#suffix' => '</div>',
    '#collapsed' => FALSE,
    '#collapsible' => TRUE,
    '#tree' => TRUE,
  );
  $button = TRUE;
  if (empty($form_state['num_tokens'])) {
    if (!empty($token['tokenMap'])) {
      foreach($token['tokenMap'] as $name => $value) {
        $form_state['num_tokens'][uniqid()] = array(
          'name' => $name,
          'value' => $value
        );
      }
    } else {
      $form_state['num_tokens'][uniqid()] = array(
        'name' => '',
        'value' => ''
      );
    }
  }
  $i = 0;
  foreach ($form_state['num_tokens'] as $uuid => $params) {
    if (is_array($params) && isset($params['name'])) {
      $form['token']['tokenMap'][$uuid] = array(
        '#title' => t('Default Token Value #@num (optional)', array('@num' => ($i + 1))),
        '#type' => 'fieldset',
        '#collapsed' => FALSE,
        '#collapsible' => TRUE,
      );
      $form['token']['tokenMap'][$uuid]['name'] = array(
        '#type' => 'textfield',
        '#title' => t('Default Name'),
        '#default_value' => (!empty($params['name'])) ? $params['name'] : '',
      );
      $form['token']['tokenMap'][$uuid]['value'] = array(
        '#type' => 'textfield',
        '#title' => t('Default Value'),
        '#default_value' => (!empty($params['value'])) ? $params['value'] : '',
      );
      if ($button) {
        $form['token']['tokenMap'][$uuid]['remove'] = array(
          '#type' => 'submit',
          '#attributes' => array(
            'data' => $uuid,
          ),
          '#value' => t('Remove token #' . ($i + 1)),
          '#submit' => array('smartdocs_token_auth_remove'),
          '#limit_validation_errors' => array(),
          '#ajax' => array(
            'callback' => 'smartdocs_token_auth_callback',
            'wrapper' => 'custom-token-map',
          ),
          '#suffix' => '<br/><br/>'
        );
      }
      $i++;
    }
  }
  $form['token']['tokenMap']['add'] = array(
    '#type' => 'submit',
    '#value' => t('Add Token'),
    '#submit' => array('smartdocs_token_auth_add'),
    '#limit_validation_errors' => array(),
    '#ajax' => array(
      'callback' => 'smartdocs_token_auth_callback',
      'wrapper' => 'custom-token-map',
    ),
    '#suffix' => '<br/><br/>'
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Save Authentication Settings'
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
function smartdocs_token_auth_callback($form, $form_state) {
  return $form['token']['tokenMap'];
}

/**
 * Adds parameter to the form
 *
 * @param $form
 * @param $form_state
 */
function smartdocs_token_auth_remove($form, &$form_state) {
  $form_state['num_tokens'] = array();
  $remove = $form_state['clicked_button']['#attributes']['data'];
  foreach($form_state['input']['token']['tokenMap'] as $uuid => $token) {
    if (!($remove == $uuid)) {
      $form_state['num_tokens'][$uuid] = array(
        'name' => $token['name'],
        'value' => $token['value']
      );
    }
  }
  $form_state['rebuild'] = TRUE;
}

/**
 * Adds parameter to the form
 *
 * @param $form
 * @param $form_state
 */
function smartdocs_token_auth_add($form, &$form_state) {
  foreach($form_state['input']['token']['tokenMap'] as $uuid => $token) {
    $form_state['num_tokens'][$uuid] = array(
      'name' => $token['name'],
      'value' => $token['value']
    );
  }
  $form_state['num_tokens'][uniqid()] = array(
    'name' => '',
    'value' => ''
  );
  $form_state['rebuild'] = TRUE;
}

/**
 * Authentication Submit Form
 *
 * @param $form
 * @param $form_state
 */
function smartdocs_revision_auth_submit($form, &$form_state) {
  $model = $form_state['values']['model'];
  $rev = $form_state['values']['rev'];
  if (!empty($form_state['values']['oauth']['clientId'])) {
    if ($form_state['values']['oauth_action'] == 'add') {
      $form_state['values']['oauth']['name'] = 'oauth2WebServerFlow';
      $payload = $form_state['values']['oauth'];
      if ($callback = entity_get_controller('smartdocs_revision')->addAuth($model, $rev, $payload)) {
        drupal_set_message('OAuth 2 Web Server Flow has been added to the revision.', 'status');
        variable_set($model . '_' . $rev . '_callbackURL', $callback);
      } else {
        drupal_set_message('There was a problem adding ‘OAuth 2 Web Server Flow’ to the current revision.', 'error');
      }
    } else {
      $form_state['values']['oauth']['name'] = 'oauth2WebServerFlow';
      $payload = $form_state['values']['oauth'];
      if (entity_get_controller('smartdocs_revision')->updateAuth($model, $rev, $payload)) {
        drupal_set_message('OAuth 2 Web Server Flow has been updated.', 'status');
      } else {
        drupal_set_message('There was a problem updating the ‘OAuth 2 Web Server Flow’ for the current revision.', 'error');
      }
    }
  } else {
    drupal_set_message('No configuration settings for OAuth 2 Web Server Flow found.', 'info');
  }
  if (!empty($form_state['values']['token']['tokenType'])) {
    $map = array();
    foreach($form_state['values']['token']['tokenMap'] as $key => $value) {
      if (is_array($value)) {
        if (isset($value['name']) && !empty($value['name'])) {
          $map[$value['name']] = $value['value'];
        }
      }
    }
    $payload = array(
      'name' => 'custom',
      'tokenType' => $form_state['values']['token']['tokenType'],
    );
    if (!empty($map)) {
      $payload['tokenMap'] = $map;
    }
    if ($form_state['values']['token_action'] == 'add') {
      if (entity_get_controller('smartdocs_revision')->addAuth($model, $rev, $payload)) {
        drupal_set_message('Custom Token has been add to the revision.', 'status');
      } else {
        drupal_set_message('There was a problem adding ‘Custom Token’ to the revision.', 'error');
      }
    } else {
      if (entity_get_controller('smartdocs_revision')->updateAuth($model, $rev, $payload)) {
        drupal_set_message('Custom Token has been updated.', 'status');
      } else {
        drupal_set_message('There was a problem updating the ‘Custom Token’ information to the revision.', 'error');
      }
    }
  } else {
    drupal_set_message('No configuration settings for Custom Token found.', 'info');
  }
}

/**
 * Edit Revision Form
 *
 * @param $form
 * @param $form_state
 * @param $mid
 * @param $rev
 * @return mixed
 */
function smartdocs_edit_revision($form, $form_state, $mid, $rev) {
  $rev = entity_get_controller('smartdocs_revision')->getRevision($mid, $rev);
  drupal_set_title('Edit API Revision ‘' . (isset($rev['displayName'])) ? $rev['displayName'] : $rev['name'] . '’');
  $form['model'] = array(
    '#type' => 'hidden',
    '#value' => $mid,
  );
  $form['rev'] = array(
    '#type' => 'hidden',
    '#value' => $rev['revisionNumber'],
  );
  $form['display_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Display Name'),
    '#size' => 60,
    '#default_value' => (isset($rev['displayName'])) ? $rev['displayName'] : $rev['name'],
    '#maxlength' => 128,
    '#required' => TRUE,
  );
  $form['version'] = array(
    '#type' => 'textfield',
    '#title' => t('Version ID'),
    '#size' => 60,
    '#default_value' => (isset($rev['releaseVersion'])) ? $rev['releaseVersion'] : '',
    '#maxlength' => 128,
    '#required' => TRUE,
  );
  $form['description'] = array(
    '#type' => 'textarea',
    '#title' => t('Description'),
    '#default_value' => (isset($rev['description'])) ? $rev['description'] : '',
    '#rows' => 5,
    '#required' => TRUE,
  );
  $form['base_url'] = array(
    '#type' => 'textfield',
    '#title' => t('Base URL'),
    '#default_value' => (isset($rev['baseUrl'])) ? $rev['baseUrl'] : '',
    '#size' => 60,
    '#maxlength' => 128,
    '#required' => TRUE,
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#submit' => array('smartdocs_edit_revision_submit'),
    '#value' => t('Save API Revision'),
  );
  return $form;
}

/**
 * Edit Revision Form Submit
 *
 * @param $form
 * @param $form_state
 */
function smartdocs_edit_revision_submit($form, &$form_state) {
  $payload = array(
    'displayName' => $form_state['values']['display_name'],
    'description' => $form_state['values']['description'],
    'releaseVersion' => $form_state['values']['version'],
    'baseUrl' => $form_state['values']['base_url'],
  );
  $payload = drupal_json_encode($payload);
  $mid = $form_state['values']['model'];
  $rev = $form_state['values']['rev'];
  if ($revision = entity_get_controller('smartdocs_revision')->updateRevision($mid, $rev, $payload)) {
    cache_clear_all('*', 'cache_smartdocs', TRUE);
    drupal_set_message('Revision #' . $rev .' has been updated.');
    $form_state['redirect'] = 'admin/smartdocs/models/' . $mid .'/revisions/' . $revision['revisionNumber'];
  }
}
