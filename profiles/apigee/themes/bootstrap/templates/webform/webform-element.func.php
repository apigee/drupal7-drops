<?php
/**
 * @file
 * Stub file for bootstrap_webform_element().
 */

/**
 * Returns HTML for a webform element.
 *
 * @see theme_webform_element()
 * @see bootstrap_form_element()
 */
function bootstrap_webform_element(&$variables) {
  $element = &$variables['element'];

  // Inline title.
  if ($element['#title_display'] === 'inline') {
    $element['#title_display'] = 'before';
    $element['#wrapper_attributes']['class'][] = 'form-inline';
  }

  // Description above field.
  if (!empty($element['#webform_component']['extra']['description_above'])) {
    $element['#description_display'] = 'before';
  }

  // If field prefix or suffix is present, make this an input group.
  if (!empty($element['#field_prefix']) || !empty($element['#field_suffix'])) {
    $element['#input_group'] = TRUE;
  }

  // Render with bootstrap_form_element().
  bootstrap_include('bootstrap', 'templates/system/form-element.func.php');
  return bootstrap_form_element($variables);
}
