<?php
/**
 * @file
 * uuid features hooks on behalf of the state_flow module.
 */

/**
 * Implements hook_uuid_entity_features_export_render_alter().
 */
function state_flow_uuid_entity_features_export_render_alter($entity_type, &$export, &$entity, $module) {
  // The state flow module adds a copy of the entity object which leads to an
  // endless loop in the export.
  if (isset($export->state_flow)) {
    unset($export->state_flow);
  }
}
