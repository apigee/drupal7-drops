<?php

/**
 * @file
 * Provides views theme override functions for Bootstrap themes.
 */

/**
 * Implements template_preprocess_views_view_table().
 * Add a class to table displays of views.
 */
function apigee_base_preprocess_views_view_table(&$vars) {
  $vars['classes_array'][] = 'table';
}

/**
 * Implements template_preprocess_views_view_grid().
 * Add a table class to grid displays of views.
 */
function apigee_base_preprocess_views_view_grid(&$vars) {
  $vars['class'] .= ' table';
}
