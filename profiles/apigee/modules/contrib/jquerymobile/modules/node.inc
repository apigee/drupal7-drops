<?php
/**
 * @file
 */
 
/**
 * Construct a drupal_render() style array from an array of loaded nodes.
 *
 * @param $nodes
 *   An array of nodes as returned by node_load_multiple().
 * @param $view_mode
 *   View mode, e.g. 'full', 'teaser'...
 * @param $weight
 *   An integer representing the weight of the first node in the list.
 * @param $langcode
 *   (optional) A language code to use for rendering. Defaults to the global
 *   content language of the current request.
 *
 * @return
 *   An array in the format expected by drupal_render().
 */
function _jquerymobile_view_multiple($nodes, $view_mode = 'teaser', $weight = 0, $langcode = NULL) {
  global $theme_key;

  field_attach_prepare_view('node', $nodes, $view_mode, $langcode);
  entity_prepare_view('node', $nodes, $langcode);
  $build = array();
  foreach ($nodes as $node) {
    $build['nodes'][$node->nid] = _jquerymobile_node_view($node, $view_mode, $langcode);
    $build['nodes'][$node->nid]['#weight'] = $weight;
    $weight++;
  }
  $build['nodes']['#sorted'] = TRUE;

  $use_global = theme_get_setting('use_global', $theme_key);
  
  $attributes['data-role'] = 'listview'; 
  $attributes['data-inset'] = $use_global ? theme_get_setting('global_inset', $theme_key) : theme_get_setting('menu_item_inset', $theme_key); 
  $attributes['data-theme'] = $use_global ? theme_get_setting('global_theme') : theme_get_setting('menu_item_theme', $theme_key);
  $attributes['data-divider-theme'] = $use_global ? theme_get_setting('global_theme') : theme_get_setting('menu_item_dividertheme', $theme_key);
  $attributes['data-count-theme'] = $use_global ? theme_get_setting('global_theme') : theme_get_setting('menu_item_counttheme', $theme_key);
  $attributes['data-split-theme'] = $use_global ? theme_get_setting('global_theme') : theme_get_setting('menu_item_splittheme', $theme_key);
  $attributes['data-split-icon'] = $use_global ? theme_get_setting('global_icon') : theme_get_setting('menu_item_spliticon', $theme_key);
  if (_jquerymobile_get_setting($theme_key, 'front')) {  
    $build['nodes']['#prefix'] = '<ul ' . drupal_attributes($attributes) . '>';
    $build['nodes']['#suffix'] = '</ul>';
  }
  return $build;
}

function _jquerymobile_node_view($node, $view_mode = 'full', $langcode = NULL) {
  global $theme_key;

  $prefix = '';
  $suffix = ''; 
    
  if (!isset($langcode)) {
    $langcode = $GLOBALS['language_content']->language;
  }

  if (_jquerymobile_get_setting($theme_key, 'front')) {
    $prefix = '<li>';
    $suffix = '</li>';
  }


  // Populate $node->content with a render() array.
  node_build_content($node, $view_mode, $langcode);
  
  $build = $node->content;
  // We don't need duplicate rendering info in node->content.
  unset($node->content);

  $build += array(
    '#theme' => 'node', 
    '#node' => $node,
    '#view_mode' => $view_mode,
    '#language' => $langcode,
    '#prefix' => $prefix,
    '#suffix' => $suffix,
  );

  // Add contextual links for this node, except when the node is already being
  // displayed on its own page. Modules may alter this behavior (for example,
  // to restrict contextual links to certain view modes) by implementing
  // hook_node_view_alter().
  if (!empty($node->nid) && !($view_mode == 'full' && node_is_page($node))) {
    $build['#contextual_links']['node'] = array('node', array($node->nid));
  }

  // Allow modules to modify the structured node.
  $type = 'node';
  drupal_alter(array('_jquerymobile_node_view', 'entity_view'), $build, $type);

  return $build;
}

