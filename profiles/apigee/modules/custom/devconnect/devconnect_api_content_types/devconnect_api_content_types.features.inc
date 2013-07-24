<?php
/**
 * @file
 * devconnect_api_content_types.features.inc
 */

/**
 * Implements hook_node_info().
 */
function devconnect_api_content_types_node_info() {
  $items = array(
    'api' => array(
      'name' => t('API'),
      'base' => 'node_content',
      'description' => t('General description of broad functionality. It usually doesn’t contain technical details.'),
      'has_title' => '1',
      'title_label' => t('API Name'),
      'help' => '',
    ),
    'api_method' => array(
      'name' => t('API Method'),
      'base' => 'node_content',
      'description' => t('The API method, with long description (with full technical details) and test console. Eg. Vertical method (under “Top Sellers” class, under Search API) contains short description, long description (that includes URL, parameters for API method call, sample request and response) etc. It also includes a test console which can be used to make requests to the API method.'),
      'has_title' => '1',
      'title_label' => t('Method'),
      'help' => '',
    ),
    'class_name' => array(
      'name' => t('API Class Name'),
      'base' => 'node_content',
      'description' => t('A way to group API methods (defined next). There is no content associated with a class name apart from the name of class itself. This is used as a ‘separator’ when listing API methods.'),
      'has_title' => '1',
      'title_label' => t('API Class Name'),
      'help' => '',
    ),
  );
  return $items;
}
