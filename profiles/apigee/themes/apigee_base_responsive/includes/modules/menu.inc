<?php

/**
 * Overrides theme_menu_local_tasks().
 * Change classes used by <ul> elements.
 */
function apigee_base_responsive_menu_local_tasks(&$vars) {
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
function apigee_base_responsive_menu_local_task($variables) {
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
function apigee_base_responsive_menu_tree(&$variables) {
  return '<ul class="menu nav">' . $variables['tree'] . '</ul>';
}

/**
 * Overrides theme_menu_link().
 *
 * Prints sub-items with the menu link.
 */
function apigee_base_responsive_menu_link(array $variables) {
  $element = $variables['element'];
  $sub_menu = '';

  if ($element['#below']) {
    // Add our own wrapper
    unset($element['#below']['#theme_wrappers']);
    $sub_menu = '<ul>' . drupal_render($element['#below']) . '</ul>';

    $element['#localized_options']['html'] = TRUE;

    $element['#href'] = "";
  }

  $output = l($element['#title'], $element['#href'], $element['#localized_options']);
  return '<li' . drupal_attributes($element['#attributes']) . '>' . $output . $sub_menu . "</li>\n";
}

/**
 * Get all primary tasks including subsets
 * @todo: phase this out. It doesn't check if the item is active and shouldn't
 * be doing its own query in the theme layer.
 */
function _apigee_base_responsive_local_tasks($tabs = FALSE) {
  if ($tabs == '') {
    return $tabs;
  }

  if (!$tabs) {
    $tabs = menu_primary_local_tasks();
  }

  foreach ($tabs as $key => $element) {
    $result = db_select('menu_router', NULL, array('fetch' => PDO::FETCH_ASSOC))
      ->fields('menu_router')
      ->condition('tab_parent', $element['#link']['path'])
      ->condition('context', MENU_CONTEXT_INLINE, '<>')
      ->condition('type', array(MENU_DEFAULT_LOCAL_TASK, MENU_LOCAL_TASK), 'IN')
      ->orderBy('weight')
      ->orderBy('title')
      ->execute();

    $router_item = menu_get_item($element['#link']['path']);
    $map = $router_item['original_map'];

    $i = 0;
    foreach ($result as $item) {
      _menu_translate($item, $map, TRUE);

      //only add items that we have access to
      if ($item['tab_parent'] && $item['access']) {
      //set path to that of parent for the first item
      if ($i === 0) {
        $item['href'] = $item['tab_parent'];
      }

      if (current_path() == $item['href']) {
        $tabs[$key][] = array(
        '#theme' => 'menu_local_task',
        '#link' => $item,
        '#active' => TRUE,
        );
      }
      else {
        $tabs[$key][] = array(
        '#theme' => 'menu_local_task',
        '#link' => $item,
        );
      }

      //only count items we have access to.
      $i++;
      }
    }
  }

  return $tabs;
}
