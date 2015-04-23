<?php

/**
 * Allows a module to take some action immediately after a SmartDocs model
 * cache has been invalidated.
 *
 * @param string $model_uuid
 */
function hook_smartdocs_model_update($model_uuid) {
  drupal_set_message('Cache for model ' . $model_uuid . ' has just been invalidated.');
}
