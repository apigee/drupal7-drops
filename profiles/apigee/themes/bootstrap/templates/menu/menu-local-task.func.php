<?php
/**
 * @file
 * Stub file for bootstrap_menu_local_task().
 */

/**
 * Returns HTML for a single local task link.
 *
 * @param array $variables
 *   An associative array containing:
 *   - element: A render element containing:
 *     - #link: A menu link array with 'title', 'href', and 'localized_options'
 *       keys.
 *     - #active: A boolean indicating whether the local task is active.
 *
 * @return string
 *   The constructed HTML.
 *
 * @see theme_menu_local_task()
 *
 * @ingroup theme_functions
 */
function bootstrap_menu_local_task($variables) {
  $link = $variables['element']['#link'];

  $options = isset($link['localized_options']) ? $link['localized_options'] : array();

  // Filter the title if the "html" is not set, otherwise l() will automatically
  // sanitize using check_plain(), so no need to call that here.
  $title = empty($options['html']) ? filter_xss_admin($link['title']) : $link['title'];

  $href = $link['href'];
  $attributes = array();

  // Add text to indicate active tab for non-visual users.
  if (!empty($variables['element']['#active'])) {
    $options['html'] = TRUE;
    $attributes['class'][] = 'active';
    $title = t('!local-task-title!active', array(
      '!local-task-title' => $title,
      '!active' => '<span class="element-invisible">' . t('(active tab)') . '</span>',
    ));
  }

  return '<li' . drupal_attributes($attributes) . '>' . l($title, $href, $options) . "</li>\n";
}
