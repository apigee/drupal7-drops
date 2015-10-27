<?php

/**
 * Allows a module to take some action immediately after a SmartDocs model
 * or one of its child objects (revision, resource, method) has been created,
 * updated or deleted.
 *
 * @param string $model_uuid
 *    The unique identifier for the model being updated.
 */
function hook_smartdocs_model_update($model_uuid) {
  drupal_set_message('Model ' . $model_uuid . ' has just been updated.');
}

/**
 * Allows a module to alter the rendered output of a SmartDocs node right
 * before it is cached and served.
 *
 * @param string $content
 *    The rendered HTML output for the SmartDocs method.
 * @param stdClass $node
 *    The SmartDocs method node which is being rendered.
 */
function hook_smartdocs_model_alter(&$content, stdClass $node) {
  $content = str_replace('%node-title%', $node->title, $content);
}
