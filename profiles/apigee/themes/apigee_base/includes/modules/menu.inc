<?php

/**
 * Overrides theme_menu_local_tasks().
 * Change classes used by <ul> elements.
 */
function apigee_base_menu_local_tasks(&$vars) {
  $output = '';

  if (!empty($vars['primary']) ) {
    // Include invisible label for non-visual users.
    $vars['primary']['#prefix'] = '<h2 class="element-invisible">' . t('Primary tabs') . '</h2>';
    $vars['primary']['#prefix'] .= '<ul class="nav nav-tabs">';
    $vars['primary']['#suffix'] = '</ul>';
    $output .= drupal_render($vars['primary']);
  }

  if (!empty($vars['secondary']) ) {
    $vars['secondary']['#prefix'] = '<ul class="nav nav-pills">';
    $vars['secondary']['#suffix'] = '</ul>';
    $output .= drupal_render($vars['secondary']);
  }

  return $output;
}


/**
 * Overrides theme_menu_local_task().
 *
 * Returns HTML for primary and secondary local tasks. In addition to what the
 * core does, this function includes the child tasks.
 *
 * @ingroup themeable
 */
function apigee_base_menu_local_task($variables) {
  $link = $variables['element']['#link'];
  $link_text = $link['title'];
  $classes = array();

  if (!empty($variables['element']['#active'])) {
    // Add text to indicate active tab for non-visual users.
    $active = '<span class="element-invisible">' . t('(active tab)') . '</span>';

    // If the link does not contain HTML already, check_plain() it now.
    // After we set 'html'=TRUE the link will not be sanitized by l().
    if (empty($link['localized_options']['html'])) {
      $link['title'] = check_plain($link['title']);
    }
    $link['localized_options']['html'] = TRUE;
    $link_text = t('!local-task-title!active', array('!local-task-title' => $link['title'], '!active' => $active));

    $classes[] = 'active';
  }

  // Render child tasks if available.
  $children = '';
  if (element_children($variables['element'])) {
    $link['localized_options']['attributes']['class'][] = 'dropdown-toggle';
    $link['localized_options']['attributes']['data-toggle'][] = 'dropdown';
    $classes[] = 'dropdown';

    $children = drupal_render_children($variables['element']);
    $children = '</b><ul class="secondary-tabs dropdown-menu">' . $children . "</ul>";

    return '<li class="' . implode(' ', $classes) . '"><a href="#"' . drupal_attributes($link['localized_options']['attributes']) .'>' . $link_text . '<b class="caret"></a>' . $children . "</li>\n";
  }
  else{
    return '<li class="' . implode(' ', $classes) . '">' . l($link_text, $link['href'], $link['localized_options']) . $children . "</li>\n";
  }
}

/**
 * Overrides theme_menu_tree().
 * Includes the nav class on the menu <ul>
 */
function apigee_base_menu_tree(&$variables) {
  return '<ul class="menu nav">' . $variables['tree'] . '</ul>';
}
