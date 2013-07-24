<?php

/**
 * Describes custom developer app attributes.
 *
 * @return array
 *
 * The return value should be an associative array.
 * The key of this array should be the attribute's machine name, and the value
 * should be another associative array with the following members:
 *   name (string - required)
 *   type (string - required) Form element for this attribute
 *   required (bool - default false)
 *   pattern (string - optional) Regex which values must match
 *   tooltip (string - optional) Form fields will have this as a "title" attribute.
 *   description (string - optional)
 *   default (string - optional)
 *   maxlength (integer - optional)
 */
function hook_devconnect_app_attributes() {
  return array(
    'description' => array(
      'name' => t('Description'),
      'type' => 'textarea',
      'required' => FALSE
    )
  );
}

/**
 * Returns a list of attributes whose values should be displayed on the app
 * detail page.
 *
 * @return array
 *   Keys are attribute machine names and values are displayable titles.
 *
 */
function hook_devconnect_attributes_display_list() {
  return array(
    'description' => t('Description')
  );
}