<?php

/**
 * @file
 */


/**
 * Footermap template preprocess function.
 * @param &$variables
 */
function template_preprocess_footermap(&$variables) {
  global $user;
  global $language;
  
  drupal_add_css(drupal_get_path('module', 'footermap') . '/footermap.css', array('type' => 'file', 'every_page' => TRUE, 'group' => CSS_DEFAULT));

  // Bail early if we have cached the footermap.
  if ($variables['cached']) {
    return;
  }
  
  // We need to remove any menu item that does not match LANGUAGE_NONE or our language.
  $lang = !empty($user->language) ? $user->language : (!empty($language->language) ? $language->language : LANGUAGE_NONE);
  
  foreach ($variables['footermap'] as $name => $header) {
    if (!empty($header['#items'])) {
      foreach ($header['#items'] as $key => $item) {
        if ($item['#language'] <> $lang && $item['#language'] <> LANGUAGE_NONE) {
          unset($variables['footermap'][$name]['#items'][$key]);
        }
      }
    }
  }
  
  // set cache for language
  cache_set('footermap-' . $lang, $variables['footermap'], 'cache', CACHE_TEMPORARY);
}

/**
 * Footermap Item theme function
 * @param &$variables
 * @return HTML output
 */
function theme_footermap_item($variables) {
  $o = '<li ' . drupal_attributes($variables['attributes']) . '>';
  $o .= l($variables['title'], $variables['href']);
  $o .= '</li>';

  if (isset($variables['children'])) {
    foreach (element_children($variables['children']) as $child) {
      $o .= theme('footermap_item', $variables['children'][$child]);
    }
  }

  return $o;
}

/**
 * Footermap Header theme function
 * @param &$variables
 * @return HTML output
 */
function theme_footermap_header($variables) {
  $o = '';

  if (variable_get('footermap_heading', 0)) {
    $o .= '<h3>' . $variables['title'] . '</h3>';
  }

  $o .= '<ul ' . drupal_attributes($variables['attributes']) . '>';

  if (isset($variables['items'])) {
    foreach (element_children($variables['items']) as $child) {
      $o .= theme('footermap_item', $variables['items'][$child]);
    }
  }

  $o .= '</ul>';

  return $o;
}
