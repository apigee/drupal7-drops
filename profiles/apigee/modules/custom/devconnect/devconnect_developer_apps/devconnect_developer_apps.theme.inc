<?php

function _devconnect_developer_app_hash_link(array $variables) {
  return '<a data-toggle="tab" href="' . $variables['path'] . '">' . check_plain($variables['text']) . '</a>';
}

/**
 * @param array $variables
 */
function theme_developer_app_tabs(array $variables) {
  $output = '<ul class="nav nav-tabs">';
  $first_tab = TRUE;
  foreach ($variables['tab_links'] as $tab_link) {
    $output .= '<li';
    if ($first_tab) {
      $output .= ' class="active"';
      $first_tab = FALSE;
    }
    $output .= ' data-link-type="' . (substr($tab_link['path'], 0, 1) == '#' ? 'local' : 'external') . '"';
    $output .= '>';
    $tab_link += array('text' => '', 'path' => '#', 'options' => array());

    if (substr($tab_link['path'], 0, 1) == '#') {
      $output .= _devconnect_developer_app_hash_link($tab_link);
    }
    else {
      $output .= l($tab_link['text'], $tab_link['path'], $tab_link['options']);
    }
    $output .=  '</li>';
  }
  $output .= '</ul>';
  return $output;
}

function theme_developer_app_panes(array $variables) {
  // Open pane-set wrapper div
  $output = '<div class="tab-content app-details">';
  $first_pane = TRUE;
  foreach ($variables['panes'] as $pane) {
    $id = $pane['#id'];

    // Open pane wrapper div
    $output .= '<div class="tab-pane';
    if ($first_pane) {
      $output .= ' active';
      $first_pane = FALSE;
    }
    $output .= '" id="' . $id . '">';

    // Create pane content
    $output .= drupal_render($pane);

    // Close pane wrapper div
    $output .= '</div>';
  }
  // Close pane-set wrapper div
  $output .= '</div>';

  return $output;
}

function devconnect_developer_apps_keys_pane($variables) {
  $pane = array('#id' => 'keys', '#weight' => 0);

  $pane['pane_title'] = array(
    '#type' => 'markup',
    '#markup' => t('Keys'),
    '#prefix' => '<h3>',
    '#suffix' => '</h3>',
    '#weight' => 0
  );
  $pane['pane_subtitle'] = array(
    '#type' => 'markup',
    '#markup' => t('These are the keys to your app kingdom.'),
    '#prefix' => '<p>',
    '#suffix' => '</p>',
    '#weight' => 1
  );
  $pane['details'] = array('#weight' => 2);
  if (isset($variables['credentials'][0])) {
    $pane['#credential_status'] = $variables['overallStatus'];
    $key_classes = array('key', 'key-status-' . $variables['overallStatus']);
    if (!devconnect_user_developer_is_active($variables['account'])) {
      $key_classes[] = 'developer-disabled';
    }
    $pane['details']['consumer_key'] = array(
      '#prefix' => '<div class="' . implode(' ', $key_classes) . '">',
      '#suffix' => '</div>',
      'label' => array(
        '#type' => 'markup',
        '#markup' => t('Consumer Key:'),
        '#prefix' => '<span class="key-label">',
        '#suffix' => '</span> '
      ),
      'content' => array(
        '#type' => 'markup',
        '#markup' => check_plain($variables['credentials'][0]['consumer_key']),
        '#prefix' => '<span class="key-value">',
        '#suffix' => '</span>'
      ),
      '#weight' => 0
    );
    $pane['details']['consumer_secret'] = array(
      '#prefix' => '<div class="' . implode(' ', $key_classes) . '">',
      '#suffix' => '</div>',
      'label' => array(
        '#type' => 'markup',
        '#markup' => t('Consumer Secret:'),
        '#prefix' => '<span class="key-label">',
        '#suffix' => '</span> '
      ),
      'content' => array(
        '#type' => 'markup',
        '#markup' => check_plain($variables['credentials'][0]['consumer_secret']),
        '#prefix' => '<span class="key-value">',
        '#suffix' => '</span>'
      ),
      '#weight' => 1
    );
  }
  if (!empty($variables['callback_url']) && variable_get('devconnect_callback_handling', 'require') != 'none') {
    $pane['details']['callback_url'] = array(
      '#prefix' => '<div class="key">',
      '#suffix' => '</div>',
      'label' => array(
        '#type' => 'markup',
        '#markup' => t('Callback URL:'),
        '#prefix' => '<span class="key-label">',
        '#suffix' => '</span> '
      ),
      'content' => array(
        '#type' => 'markup',
        '#markup' => check_plain($variables['callback_url'])
      ),
      '#weight' => 2
    );
  }
  return $pane;
}

function devconnect_developer_apps_products_pane($variables) {
  $pane = array('#id' => 'products', '#weight' => 1);

  $pane['pane_title'] = array(
    '#type' => 'markup',
    '#markup' => t('Products'),
    '#prefix' => '<h3>',
    '#suffix' => '</h3>',
    '#weight' => 0
  );
  if (empty($variables['credentials']) || !isset($variables['credentials'][0]['apiproducts'])) {
    $variables['credentials'][0]['apiproducts'] = array();
  }
  $apiproducts = array();
  foreach ($variables['credentials'][0]['apiproducts'] as $i => $apiproduct) {
    $this_product = array(
      '#prefix' => '<div class="app-content">',
      '#suffix' => '</div><br>',
      '#weight' => $i,
      'info' => array(
        'title' => array(
          '#type' => 'markup',
          '#markup' => check_plain($apiproduct['display_name']),
          '#prefix' => '<h4 class="app-product-title">',
          '#suffix' => '</h4>',
          '#weight' => 0
        ),
        '#weight' => 0
      )
    );
    if (strlen($apiproduct['description']) > 0) {
      $this_product['info']['description'] = array(
        '#type' => 'markup',
        '#markup' => check_plain($apiproduct['description']),
        '#prefix' => '<div class="app-desc">',
        '#suffix' => '</div>',
        '#weight' => 1
      );
    }
    if (!empty($apiproduct['status'])) {
      $this_product['status'] = array(
        '#weight' => 1,
        '#prefix' => '<div class="apiproduct-status">',
        '#suffix' => '</div>',
        'label' => array(
          '#type' => 'markup',
          '#markup' => t('Approval Status') . ': ',
          '#weight' => 0
        ),
        'status' => array(
          '#type' => 'markup',
          '#markup' => check_plain(ucfirst($apiproduct['status'])),
          '#weight' => 1
        )
      );
    }
    $apiproducts[] = $this_product;
  }
  if (empty($apiproducts)) {
    $pane['no_products'] = array(
      '#type' => 'markup',
      '#prefix' => '<p>',
      '#suffix' => '</p>',
      '#markup' => t('This app is not associated with any API Products.'),
      '#weight' => 1
    );
  }
  else {
    $pane['api_products'] = $apiproducts + array('#weight' => 1);
  }
  return $pane;
}

function devconnect_developer_apps_detail_pane($variables) {
  $pane = array('#id' => 'details', '#weight' => 2);
  $pane['pane_title'] = array(
    '#type' => 'markup',
    '#markup' => t('App Details'),
    '#prefix' => '<h3>',
    '#suffix' => '</h3>',
    '#weight' => 0
  );
  $pane['details'] = array(
    '#prefix' => '<div class="well">',
    '#suffix' => '</div>',
    '#weight' => 1
  );

  $pane['details']['name'] = array(
    '#prefix' => '<div class="control-group app-name">',
    '#suffix' => '</div>',
    'label' => array(
      '#prefix' => '<strong>',
      '#suffix' => '</strong>',
      '#type' => 'markup',
      '#markup' => t('App Name') . ':',
      '#weight' => 0
    ),
    'value' => array(
      '#prefix' => '<div>',
      '#suffix' => '</div>',
      '#type' => 'markup',
      '#markup' => check_plain($variables['name']),
      '#weight' => 1
    ),
    '#weight' => 0
  );
  if (!empty($variables['callback_url']) && variable_get('devconnect_callback_handling', 'require') != 'none') {
    $pane['details']['callback_url'] = array(
      '#prefix' => '<div class="control-group callback-url">',
      '#suffix' => '</div>',
      'label' => array(
        '#prefix' => '<strong>',
        '#suffix' => '</strong>',
        '#type' => 'markup',
        '#markup' => t('Callback URL') . ':',
        '#weight' => 0
      ),
      'value' => array(
        '#prefix' => '<div>',
        '#suffix' => '</div>',
        '#type' => 'markup',
        '#markup' => check_plain($variables['callback_url']),
        '#weight' => 1
      ),
      '#weight' => 1
    );
  }
  if (isset($variables['credentials'][0]['apiproducts'][0]) && variable_get('devconnect_api_product_handling', 'multiple_required') != 'no_api_product') {
    $field_title = (count($variables['credentials'][0]['apiproducts']) > 1 ? 'API Products' : 'API Product');
    $pane['details']['api_products'] = array(
      '#prefix' => '<div class="control-group api-products">',
      '#suffix' => '</div>',
      '#weight' => 2,
      'title' => array(
        '#prefix' => '<strong>',
        '#suffix' => '</strong>',
        '#type' => 'markup',
        '#markup' => t($field_title) . ':',
        '#weight' => -1
      )
    );
    foreach ($variables['credentials'][0]['apiproducts'] as $i => $apiproduct) {
      $this_product = array(
        '#prefix' => '<div class="api-product">',
        '#suffix' => '</div>',
        '#weight' => $i,
        'display_name' => array(
          '#type' => 'markup',
          '#markup' => check_plain($apiproduct['display_name']),
          '#weight' => 0
        )
      );
      if (!empty($apiproduct['description'])) {
        $this_product['description'] = array(
          '#type' => 'markup',
          '#weight' => 1,
          '#prefix' => '&nbsp;&ndash;&nbsp;',
          '#markup' => check_plain($apiproduct['description'])
        );
      }
      $pane['details']['api_products'][$i] = $this_product;
    }
  }

  if (!empty($variables['app_attributes'])) {
    $pane['details']['attributes'] = array('#weight' => 3);
    $curr_weight = 0;
    foreach ($variables['app_attributes'] as $label => $value) {
      $machine_name = str_replace(' ', '_', strtolower($label));
      $machine_name = preg_replace('![^a-z0-9_-]!', '', $machine_name);
      $pane['details']['attributes'][$machine_name] = array(
        '#prefix' => '<div class="control-group app-attribute app-attribute-' . $machine_name . '">',
        '#suffix' => '</div>',
        'label' => array(
          '#type' => 'markup',
          '#markup' => check_plain("$label:"),
          '#prefix' => '<strong>',
          '#suffix' => '</strong> '
        ),
        'content' => array(
          '#type' => 'markup',
          '#markup' => check_plain($value),
          '#prefix' => '<div>',
          '#suffix' => '</div>'
        ),
        '#weight' => $curr_weight++
      );
    }
  }

  $pane['details']['status'] = array(
    '#weight' => 4,
    '#prefix' => '<div class="control-group app-status">',
    '#suffix' => '</div>',
    'label' => array(
      '#prefix' => '<strong>',
      '#suffix' => '</strong>',
      '#type' => 'markup',
      '#markup' => t('Status') . ':',
      '#weight' => 0
    ),
    'value' => array(
      '#prefix' => '<div>',
      '#suffix' => '</div>',
      '#type' => 'markup',
      '#markup' => check_plain($variables['overallStatus']),
      '#weight' => 1
    )
  );

  return $pane;
}

function devconnect_developer_apps_performance_pane($variables) {

  $pane = array('#id' => 'performance', '#weight' => 3);

  $pane['analytics_form'] = drupal_get_form('devconnect_developer_apps_analytics_form');
  $pane['analytics_form']['#prefix'] = '<div class="span24 well">';
  $pane['analytics_form']['#suffix'] = '</div>';
  $pane['analytics_form']['#weight'] = 0;

  if (empty($variables['analytics_chart'])) {
    $pane['no_data'] = array(
      '#type' => 'markup',
      '#markup' => t('No performance data is available for the criteria you supplied.'),
      '#prefix' => '<p>',
      '#suffix' => '</p>',
      '#weight' => 1
    );
  }
  else {
    $download_base = 'user/' . arg(1) . '/app-performance/' . arg(3);
    $query = drupal_static('devconnect_developer_apps_analytics_performance');

    $pane['download_buttons'] = array(
      '#prefix' => '<div class="btn-group pull-right">',
      '#suffix' => '</div>',
      '#type' => 'markup',
      '#markup' => '<a class="export btn dropdown-toggle" data-toggle="dropdown" href="#">' . t('Export') . '<span class="caret"></span></a>'
                 . '<ul class="dropdown-menu">'
                 . '<li><a href="' . url($download_base . '/xml', array('query' => $query)) . '">XML</a></li>'
                 . '<li><a href="' . url($download_base . '/csv', array('query' => $query)) . '">CSV</a></li>'
                 . '</ul>',
      '#weight' => 1
    );

    $pane['analytics_chart'] = array(
      '#prefix' => '<div style="clear: both; margin-top:20px">', // TODO: make this a CSS class
      '#suffix' => '</div>',
      '#type' => 'markup',
      '#markup' => $variables['analytics_chart'],
      '#weight' => 2
    );
  }
  return $pane;
}
