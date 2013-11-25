<?php

function _devconnect_monetization_theme_tabs_defaults(array &$tabs) {
  $tabs = array_merge_recursive($tabs, array(
    'attributes' => array(),
    'class' => array('nav', 'nav-tabs'),
    'tabs' => array(),
  ));
  $tabs['class'] = array_unique($tabs['class']);
  for ($i = 0, $len = count($tabs['attributes']); $i < $len; $i++) {
    $tabs['attributes'][$i] += array(
      'name' => '',
      'value' => '',
      'weight' => 0,
    );
  }
  foreach (array_keys($tabs['tabs']) as $path) {
    $tabs['tabs'][$path] = array_merge_recursive($tabs['tabs'][$path], array(
      'li' => array(
        'attributes' => array(),
        'class' => array(),
      ),
      'pane' => array(
          'attributes' => array(),
          'class' => array(),
      ),
      'class' => array(/*'toc-filter-processed'*/),
      'attributes' => array(),
      'callback args' => array(),
      'access args' => array(),
    ));
    $tabs['tabs'][$path]['attributes'][] = array(
      'name'   => 'data-toggle',
      'value'  => 'tab',
      'weight' => 0,
    );
    for ($i = 0, $len = count($tabs['tabs'][$path]['attributes']); $i < $len; $i++) {
      $tabs['tabs'][$path]['attributes'][$i] += array(
        'name'   => 'data-toggle',
        'value'  => 'tab',
        'weight' => 0,
      );
    }

    foreach (array_keys($tabs['tabs'][$path]['li']['attributes']) as $key) {
      $tabs['tabs'][$path]['li']['attributes'][$key] += array(
        'name' => '',
        'value' => '',
        'weight' => 0,
      );
    }
    foreach (array_keys($tabs['tabs'][$path]['pane']['attributes']) as $key) {
      $tabs['tabs'][$path]['pane']['attributes'][$key] += array(
        'name' => '',
        'value' => '',
        'weight' => 0,
      );
    }
    $tabs['tabs'][$path]['pane']['class'] += array('tab-pane');
    $tabs['tabs'][$path] += array(
      'file' => '',
      'file path' => '',
      'callback' => '',
      'weight' => 0,
    );
    $tabs['tabs'][$path]['file'] .= '';
    $tabs['tabs'][$path]['file path'] .= '';
    $tabs['tabs'][$path]['weight'] += 0;
    $tabs['tabs'][$path]['callback'] .= '';
  }

  $active_tab = NULL;
  foreach (array_keys($tabs['tabs']) as $path) {
    if ($active_tab == NULL || $tabs['tabs'][$path]['active'] == TRUE) {
      $active_tab = $path;
    }
    if ($key = array_search('active', $tabs['tabs'][$path]['li']['class'])) {
      unset($tabs['tabs'][$path]['li']['class'][$key]);
    }
    $tabs['tabs'][$path]['li']['class'] = array_unique($tabs['tabs'][$path]['li']['class']);
    $tabs['tabs'][$path]['class'] = array_unique($tabs['tabs'][$path]['class']);
    $tabs['tabs'][$path]['text'] .= '';
    $tabs['tabs'][$path]['callback'] .= '';
    if (!isset($tabs['tabs'][$path]['access callback'])) {
      $tabs['tabs'][$path]['access callback'] = FALSE;
    }
    $tabs['tabs'][$path]['active'] = FALSE;
  }
  $tabs['tabs'][$active_tab]['active'] = TRUE;
}


function devconnect_monetization_theme_tabs(array $tabs) {
  _devconnect_monetization_theme_tabs_defaults($tabs);
  drupal_alter('devconnect_monetization_tabs', $tabs);
  _devconnect_monetization_theme_tabs_defaults($tabs);
  return devconnect_monetization_theme_tabs_render($tabs);
}

function devconnect_monetization_theme_attributes(array $attributes) {
  $atts = array();
  foreach ($attributes as $att) {
    if (array_key_exists($att['name'], $atts)) {
      if ($att['weight'] > $atts[$att['name']]['weight']) {
        $atts[$att['name']] = $att;
      }
    }
    else {
      $atts[$att['name']] = $att;
    }
  }
  foreach (array_keys($atts) as $name) {
    $atts[$name] = $atts[$name]['value'];
  }
  return $atts;
}

function devconnect_monetization_theme_tabs_render(array $tabs) {

  $output = '<ul class="' . implode(' ', $tabs['class']) .  '"';
  if (!empty($tabs['attributes'])) {
    $atts = devconnect_monetization_theme_attributes($tabs['attributes']);
    $output .= drupal_attributes($atts);
  }
  $output .= '>';
  $tab_contents = '';
  foreach ($tabs['tabs'] as $path => &$tab) {
    $access_callback = $tab['access callback'];
    if ($access_callback === TRUE || call_user_func_array($access_callback, $tab['access args'])) {
      $li = $tab['li'];
      $output .= '<li';
      if ($tab['active'] === TRUE && !in_array('active', $tab['li']['class'])) {
        $tab['li']['class'][] = 'active';
      }
      if (!empty($tab['li']['class'])) {
        $output .= ' class="' . implode(' ', $tab['li']['class']) . '"';
      }
      if (!empty($li['attributes'])) {
        $atts = devconnect_monetization_theme_attributes($li['attributes']);
        $output .= drupal_attributes($atts);
      }

      if (substr($path, 0, 1) != '#') {
        foreach ($tab['attributes'] as $key => $value) {
          if ($value['name'] == 'data-toggle') {
            unset($tab['attributes'][$key]);
          }
        }
      }

      $options = array(
        'attributes' => array(
          'class' => $tab['class']
        ),
        'html' => TRUE,
      );
      if (!empty($tabs['attributes'])) {
        $atts = devconnect_monetization_theme_attributes($tab['attributes']);
        $options['attributes'] += $atts;
      }

      $url = $path;
      if (substr($url, 0, 1) == '#') {
        $options['fragment'] = substr($url, 1);
        $url = '';
      }
      $anchor =  '<a href="' . check_plain(url($url, $options)) . '"' . drupal_attributes($options['attributes']) . '>' . $tab['text'] . '</a>';
      $output .= '>' . $anchor . '</li>';
      $callback = $tab['callback'];
      if (substr($path, 0, 1) == '#' || $tab['active'] /*($path == $_GET['q'] || ($path == '<front>' && drupal_is_front_page()))*/) {
        $file = $tab['file'];
        $filepath = $tab['file path'];

        if (strlen($file) > 0) {
          drupal_load('file', $filepath . '/' . $file);
          require_once $filepath . '/' . $file;
        }

        if (!is_callable($callback)) {
          throw new Exception('Invalid callback defined as content callback for tab [' . $path . ']');
        }
        $content = call_user_func_array($callback, $tab['callback args']);
        if (is_array($content)) {
          $content = drupal_render($content);
        }
        $tab_contents .= '<div id="' . (substr($path, 0, 1) == '#' ? $options['fragment'] : '') . '" class="' . implode(' ', $tab['pane']['class']) . ($tab['active'] ? ' active' : '') . ' tab-pane"';
        if (!empty($tab['pane']['attributes'])) {
          $atts = devconnect_monetization_theme_attributes($tab['pane']['attributes']);
          $tab_contents .= ' ' . drupal_attributes($atts);
        }
        $tab_contents .= '>' . $content . '</div>';
      }
    }
    // If it is active tab but user has no access to it, then redirect
    // the user to the first tab the that he has access to
    else if ($tab['active'] === TRUE) {
      foreach ($tabs['tabs'] as $path2 => &$tab2) {
        $access_callback = $tab2['access callback'];
        if ($access_callback === TRUE || call_user_func_array($access_callback, $tab['access args'])) {
          drupal_goto($path2);
          break 2;
        }
      }
    }
  }
  $output .= '</ul>';
  if (!empty($tab_contents)) {
    $output .= '<div class="tab-content">' . $tab_contents . '</div>';
  }
  return '<div class="tabbable">' . $output . '</div>';
}