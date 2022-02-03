<?php

/**
 * @file
 * Custom Views integration for the Metatag module.
 */

/**
 * Implements hook_views_data().
 */
function metatag_views_data() {
  $data = array();

  $data['metatag']['table']['group'] = 'Metatag';

  $entity_types = array();

  // @todo Is it really a good idea to limit the entity types to those with
  // enabled Metatag defaults?
  foreach (entity_get_info() as $entity_type => $entity_info) {
    if (metatag_entity_supports_metatags($entity_type)) {
      $entity_types[$entity_type] = $entity_info;
    }
  }

  foreach ($entity_types as $entity_type => $info) {
    $support_revisions = !empty($info['entity keys']['revision']);
    $data['metatag']['table']['join'][$info['base table']] = array(
      'field' => $support_revisions ? 'revision_id' : 'entity_id',
      'left_field' => $info['entity keys'][$support_revisions ? 'revision' : 'id'],
      'extra' => array(
        array('field' => 'entity_type', 'value' => $entity_type),
        // @todo Replace with real language support.
        // @code
        // array('field' => 'language', 'value' => LANGUAGE_NONE),
        // @endcode
      ),
    );
  }

  // Tracks how often each index is used to generate suffixes for duplicate
  // names.
  $blacklist = array_fill_keys(array_keys($data['metatag']), 1);

  $handler_default = 'metatag_handler_field_serialized';

  // Assign a field handler based on the tag class.
  $handlers = array(
    'DrupalListMetaTag' => 'metatag_handler_field_serialized_list',
  );

  // Add a field for each known meta tag.
  $metatag_info = metatag_get_info();
  foreach ($metatag_info['tags'] as $name => $options) {
    $field = array(
      'title' => $options['label'],
      'help' => !empty($options['description']) ? $options['description'] : ' ',
    );
    if (isset($options['group'])) {
      $group = isset($metatag_info['groups'][$options['group']]) ? $metatag_info['groups'][$options['group']]['label'] : $options['group'];
      $field['group'] = $data['metatag']['table']['group'] . " ($group)";
    }

    $field['field'] = array(
      'real field' => 'data',
      'path' => array($name, 'value'),
    );
    $field['field']['handler'] = isset($handlers[$options['class']]) ? $handlers[$options['class']] : $handler_default;
    $field['field']['metatag'] = $name;

    // Create a valid field name from the tag name.
    // @todo Might have to check leading characters.
    $safe_name = preg_replace('/[^a-z0-9_]/', '_', $name);
    if (!isset($blacklist[$safe_name])) {
      // Name not yet used, add to blacklist with a count of 1.
      $blacklist[$safe_name] = 1;
    }
    else {
      // Name already in use, append a suffix and increase count.
      $safe_name .= '_' . $blacklist[$safe_name]++;
    }

    $data['metatag'][$safe_name] = $field;
  }

  return $data;
}
