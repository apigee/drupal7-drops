<?php

/**
 * Returns navigational links based on a menu tree
 */
function apigee_base_menu_navigation_links($tree, $lvl = 0) {
  $result = array();

  if (count($tree) > 0) {
    foreach ($tree as $id => $item) {
      // Skip disabled menu items.
      if ($item['link']['hidden']) {
        continue;
      }
      $new_item = array(
        'title' => $item['link']['title'],
        'link_path' => $item['link']['link_path'],
        'href' => $item['link']['href']
      );

      // Don't do drugs and don't do any levels deeper than 1
      if ($lvl < 1) {
        $new_item['below'] = apigee_base_menu_navigation_links($item['below'], $lvl + 1);
      }

      $result['menu-' . $item['link']['mlid']] = $new_item;
    }
  }

  return $result;
}


/*
 * Custom theme functions
 *
 */

/**
 * Preprocess function for theme_apigee_base_btn_dropdown
 */
function apigee_base_preprocess_apigee_base_btn_dropdown(&$variables) {
  // Add default class
  $variables['attributes']['class'][] = 'btn-group';

  // Check if its a array of links so we need to theme it first here.
  if (is_array($variables['links'])) {
    $variables['links'] = theme('links', array(
      'links' => $variables['links'],
      'attributes' => array(
        'class' => array('dropdown-menu'),
      ),
    ));
  }
}

/**
 * theme_apigee_base_btn_dropdown
 */
function apigee_base_apigee_base_btn_dropdown($variables) {
  $type_class = '';

  // Type class
  if (isset($variables['type'])) {
    $type_class = ' btn-' . $variables['type'];
  }

  // Start markup
  $output = '<div' . drupal_attributes($variables['attributes']) . '>';

  // Add as string if it's not a link
  if (is_array($variables['label'])) {
    $output .= l($variables['label']['title'], $variables['label']['href'], $variables['label']);
  }

  $output .= '<a class="btn' . $type_class . ' dropdown-toggle" data-toggle="dropdown" href="#">';

  // It's a link so create one
  if (is_string($variables['label'])) {
    $output .= check_plain($variables['label']);
  }

  // Finish markup
  $output .= '
  <span class="caret"></span>
  </a>
  ' . $variables['links'] . '
  </div>';

  return $output;
}
