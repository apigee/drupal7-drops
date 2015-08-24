<?php

/**
 * Allows a module to take some action immediately after a SmartDocs model
 * or one of its child objects (revision, resource, method) has been created,
 * updated or deleted.
 *
 * @param string $model_uuid
 */
function hook_smartdocs_model_update($model_uuid) {
  drupal_set_message('Model ' . $model_uuid . ' has just been updated.');
}
