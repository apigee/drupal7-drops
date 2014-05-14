<?php

/**
 * Returns HTML for status and/or error messages, grouped by type.
 *
 * Override of theme_status_message() to use the classes for Bootstrap alert
 * message boxes
 *
 * An invisible heading identifies the messages for assistive technology.
 * Sighted users see a colored box. See http://www.w3.org/TR/WCAG-TECHS/H69.html
 * for info.
 *
 * @param $variables
 *   An associative array containing:
 *   - display: (optional) Set to 'status' or 'error' to display only messages
 *     of that type.
 */
function apigee_base_status_messages($variables) {
  $display = $variables['display'];
  $output = '';

  // Bootstrap classes
  $alert_classes = array(
    'status' => 'alert-success',
    'error' => 'alert-error',
    'warning' => 'alert-warning',
  );

  foreach (drupal_get_messages($display) as $type => $messages) {
    $output .= '<div class="alert alert-block ' . $alert_classes[$type] . '">';
    if (!empty($status_heading[$type])) {
      $output .= '<h2 class="element-invisible">' . $status_heading[$type] . "</h2>\n";
    }
    if (count($messages) > 1) {
      $output .= " <ul>\n";
      foreach ($messages as $message) {
        $output .= '  <li>' . $message . "</li>\n";
      }
      $output .= " </ul>\n";
    }
    else {
      $output .= $messages[0];
    }
    $output .= "</div>\n";
  }
  return $output;
}


/**
 * Implements hook_css_alter().
 *
 * Check the files listed in the exclude[css] array in the theme info file and
 * remove them from the list of CSS files.
 */
function apigee_base_css_alter(&$css) {
  $excluded_files = apigee_base_theme_get_info('exclude');
  if (isset($excluded_files['css']) && is_array($excluded_files['css'])) {
    foreach ($excluded_files['css'] as $filename) {
      unset($css[$filename]);
    }
  }
}


/**
 * Implements hook_js_alter().
 *
 * Check the files listed in the exclude[js] array in the theme info file and
 * remove them from the list of JS files.
 */
function apigee_base_js_alter(&$js) {
  $excluded_files = apigee_base_theme_get_info('exclude');
  if (isset($excluded_files['js']) && is_array($excluded_files['js'])) {
    foreach ($excluded_files['js'] as $filename) {
      unset($js[$filename]);
    }
  }
}


/**
 * Get the given setting for this theme
 */
function apigee_base_theme_get_info($setting_name, $theme = NULL) {
  // If no key is given, use the current theme if we can determine it.
  if (!isset($theme)) {
    $theme = !empty($GLOBALS['theme_key']) ? $GLOBALS['theme_key'] : '';
  }

  $return = array();

  if ($theme) {
    $themes = list_themes();
    $theme_object = $themes[$theme];

    // Create a list which includes the current theme and all its base themes.
    if (isset($theme_object->base_themes)) {
      $theme_keys = array_keys($theme_object->base_themes);
      $theme_keys[] = $theme;
    }
    else {
      $theme_keys = array($theme);
    }
    foreach ($theme_keys as $theme_key) {
      if (!empty($themes[$theme_key]->info[$setting_name])) {
        $return = $themes[$theme_key]->info[$setting_name];
      }
    }
  }
  return $return;
}




/**
 * Overrides theme_links().
 * Returns HTML for a set of links. Does a lot of tweaking around classes that
 * the core implementation does not, and displays child menu links in a
 * dropdown.
 */
function apigee_base_links($variables) {
  $links = $variables['links'];
  $attributes = $variables['attributes'];
  $heading = $variables['heading'];

  global $language_url;
  $output = '';

  if (count($links) > 0) {
    $output = '';
    $output .= '<ul' . drupal_attributes($attributes) . '>';

    // Treat the heading first if it is present to prepend it to the
    // list of links.
    if (!empty($heading)) {
      if (is_string($heading)) {
        // Prepare the array that will be used when the passed heading
        // is a string.
        $heading = array(
          'text' => $heading,
          // Set the default level of the heading.
          'level' => 'li',
        );
      }
      $output .= '<' . $heading['level'];
      if (!empty($heading['class'])) {
        $output .= drupal_attributes(array('class' => $heading['class']));
      }
      $output .= '>' . check_plain($heading['text']) . '</' . $heading['level'] . '>';
    }

    $num_links = count($links);
    $i = 1;

    foreach ($links as $key => $link) {
      $children = array();

      if (isset($link['below'])) {
        $children = $link['below'];
      }

      $attributes = array('class' => array($key));

      // Add first, last and active classes to the list of links to help out themers.
      if ($i == 1) {
        $attributes['class'][] = 'first';
      }
      if ($i == $num_links) {
        $attributes['class'][] = 'last';
      }
      if (isset($link['href']) && ($link['href'] == $_GET['q'] || ($link['href'] == '<front>' && drupal_is_front_page()))
        && (empty($link['language']) || $link['language']->language == $language_url->language)) {
        $attributes['class'][] = 'active';
      }

      if (count($children) > 0) {
        $attributes['class'][] = 'dropdown';
        $link['attributes']['data-toggle'] = 'dropdown';
        $link['attributes']['class'][] = 'dropdown-toggle';
      }

      if (!isset($link['attributes'])) {
        $link['attributes'] = array();
      }

      $link['attributes'] = array_merge($link['attributes'], $attributes);

      if (count($children) > 0) {
        $link['attributes']['class'][] = 'dropdown';
      }

      $output .= '<li' . drupal_attributes($attributes) . '>';

      if (isset($link['href'])) {
        if (count($children) > 0) {
          $link['html'] = TRUE;
          $link['title'] .= ' <span class="caret"></span>';
          $output .=  '<a' . drupal_attributes($link['attributes']) . ' href="#">'. $link['title'] .'</a>';
        }
        else {
          // Pass in $link as $options, they share the same keys.
          $output .= l($link['title'], $link['href'], $link);
        }
      }
      elseif (!empty($link['title'])) {
        // Some links are actually not links, but we wrap these in <span> for adding title and class attributes.
        if (empty($link['html'])) {
          $link['title'] = check_plain($link['title']);
        }

        $span_attributes = '';
        if (isset($link['attributes'])) {
          $span_attributes = drupal_attributes($link['attributes']);
        }
        $output .= '<span' . $span_attributes . '>' . $link['title'] . '</span>';
      }

      $i++;

      if(count($children) > 0) {
        $attributes = array();
        $attributes['class'] = array('dropdown-menu');

        $output .= theme('apigee_base_links', array('links' => $children, 'attributes' => $attributes));
      }

      $output .= "</li>\n";
    }

    $output .= '</ul>';
  }

  return $output;
}

/**
 * Implements theme_preprocess_table().
 * Adds classes to the <table> to use Bootstrap styling.
 */
function apigee_base_preprocess_table(&$variables) {
  $variables['attributes']['class'][] = 'table';
  $variables['attributes']['class'][] = 'table-striped';
}


/**
 * Overrides theme_progress_bar().
 *
 * Use the Bootstrap progress bar component.
 */
function apigee_base_progress_bar($variables) {
  $variables['attributes']['class'][] = 'progress';
  $variables['attributes']['class'][] = 'progress-striped';

  return "<div". drupal_attributes($variables['attributes']) .">
  <div class=\"bar\"
       style=\"width: ". $variables['percent'] ."%;\"></div>
  </div>";
}
