<?php
/**
 * @file
 * Contains batch functions for rendering nodes.
 */

use Apigee\SmartDocs\Method;
use Apigee\SmartDocs\Model;
use Apigee\SmartDocs\Revision;
use Apigee\SmartDocs\Template;

use Apigee\Exceptions\ResponseException;

/**
 * Batch API Function to import nodes.
 *
 * @param array|Model $model
 *   The model from which nodes will be rendered.
 * @param array|Revision $revision
 *   The revision from which nodes will be rendered.
 * @param array $selected
 *   An associative array whose keys are Method UUIDs and whose value is either
 *   zero or one. One indicates that the method is to be rendered.
 * @param array $options
 *   An associative array of configuration options, whose key is the option
 *   name and whose value is the option value. At this time the only supported
 *   option is 'publish'.
 * @param bool $render_template
 *   If TRUE, configure the method template before doing anything else.
 *
 * @return array
 *   An array of batch operations to be performed.
 */
function smartdocs_render($model, $revision, array $selected, array $options, $render_template = TRUE) {
  if ($model instanceof Model) {
    $model = array(
      'id' => $model->getUuid(),
      'name' => $model->getName(),
      'displayName' => $model->getDisplayName(),
      'description' => $model->getDescription(),
    );
  }
  if ($revision instanceof Revision) {
    $revision = smartdocs_prepare_revision($revision);
  }
  $this_model = array(
    'id' => $model['id'],
    'name' => $model['name'],
    'displayName' => $model['displayName'],
    'description' => $model['description'],
  );
  $this_revision = array(
    'id' => $revision['id'],
    'revisionNumber' => $revision['revisionNumber'],
  );
  $resources = $revision['resources'];

  $operations = array();
  if ($render_template) {
    $operations[] = array('smartdocs_template', array($this_model));
  }
  $publish = (bool) $options['publish'];
  if (count($resources) > 0) {
    foreach ($selected as $select) {
      if ($select == '0') {
        continue;
      }
      foreach ($resources as $resource) {
        foreach ($resource['methods'] as $method) {
          if ($select == $method['id']) {
            $operations[] = array(
              'smartdocs_render_nodes',
              array(
                $this_model,
                $this_revision,
                array(
                  'id' => $resource['id'],
                  'name' => $resource['name'],
                  'displayName' => empty($resource['displayName']) ? $resource['name'] : $resource['displayName'],
                  'baseUrl' => $resource['baseUrl'],
                  'path' => $resource['path'],
                ),
                array(
                  'id' => $method['id'],
                ),
                $publish,
              ),
            );
          }
        }
      }
    }
  }

  $batch = array(
    'title' => t('Rendering nodes from the %model model', array('%model' => $this_model['displayName'])),
    'operations' => $operations,
    'finished' => 'smartdocs_render_finished',
    'init_message' => t('Rendering has started...'),
    'progress_message' => t('Processed @current out of @total.'),
    'error_message' => t('Rendering SmartDocs nodes for %model model has encountered an error.', array('%model' => $this_model['displayName'])),
    'file' => drupal_get_path('module', 'smartdocs') . '/batch/smartdocs.render.inc',
  );
  return $batch;
}

/**
 * Ensures the model uses the correct template.
 *
 * @param array $model_array
 *   A Model, persisted as an associative array.
 * @param array $context
 *   The current batch context.
 */
function smartdocs_template(array $model_array, array &$context) {
  if (!in_array($model_array['name'], array_keys(node_type_get_names()))) {
    $context['message'] = t('Correct model template verified');

    $path = drupal_get_path('module', 'smartdocs') . '/templates/smartdocs.hbr';
    $custom_template_file = variable_get('smartdocs_custom_template_file', NULL);

    if (!empty($custom_template_file)) {
      $file = file_load($custom_template_file);
      $path = file_create_url($file->uri);
    }
    $html = file_get_contents($path);

    try {
      $template = new Template(devconnect_default_org_config(), $model_array['id']);
      $start = microtime(TRUE);
      $template->save(SMARTDOCS_TEMPLATE_NAME, 'method', $html, TRUE);
      if (variable_get('smartdocs_log_transaction_time', 0)) {
        $args = array(
          '!model' => $model_array['id'],
          '!time' => number_format(microtime(TRUE) - $start, 4),
        );
        watchdog('smartdocs', 'Saved method template for model !model in !time seconds', $args, WATCHDOG_INFO);
      }
    }
    catch (ResponseException $e) {
      $current_set = &_batch_current_set();
      $current_set['success'] = FALSE;
      $context['results'][] = t('Unable to save the model template. The modeling API says: :response', array(
          ':response' => $e->getCode() . ' ' . $e->getMessage(),
        ));
      $context['finished'] = 1.0;
    }
  }
}

/**
 * Batch operation for creating all of the detected taxonomy terms.
 *
 * @param array $model_array
 *   A Model, persisted as an associative array.
 * @param array $revision_array
 *   A Revision, persisted as an associative array.
 * @param array $resource_array
 *   A Resource, persisted as an associative array.
 * @param array $method_array
 *   A Method, persisted as an associative array.
 * @param bool $publish
 *   If TRUE, the resulting nodes will be published.
 * @param array $context
 *   The current batch context.
 */
function smartdocs_render_nodes(array $model_array, array $revision_array, array $resource_array, array $method_array, $publish, array &$context) {
  try {
    $start = microtime(TRUE);
    // Reload method in order to pick up tags.
    $method = new Method(devconnect_default_org_config(), $model_array['id'], $revision_array['id'], $resource_array['id']);
    $method->load($method_array['id']);
    $method_array = $method->toArray(TRUE);
    $method_array['tags'] = $method->getTags();
    $method_array['body'] = $method->getBody();
    if (variable_get('smartdocs_log_transaction_time', 0)) {
      $args = array(
        '!method' => $method_array['name'],
        '!model' => $model_array['displayName'],
        '!time' => number_format(microtime(TRUE) - $start, 4),
      );
      watchdog('smartdocs', __FUNCTION__ . ': Loaded method !method from model !model in !time seconds.', $args, WATCHDOG_INFO);
    }

  }
  catch (Exception $e) {
    $message = t('Failed to load method :title from the Modeling API.', array(':title' => $method_array['displayName']));
    $context['message'] = $message;
    $context['results'][] = $message;

    $args = array(
      '!method' => $method_array['displayName'],
      '!model' => $model_array['name'],
      '!revision' => $revision_array['revisionNumber'],
      '!resource' => $resource_array['displayName'],
      '!exception' => $e->getMessage(),
    );
    watchdog('smartdocs', "Exception loading method !method from model !model, revision !revision, resource !resource:\n!exception", $args, WATCHDOG_ERROR);
    return;
  }

  $exists = db_select('smartdata', 's')
    ->fields('s', array('nid'))
    ->condition('method', $method_array['id'])
    ->execute()
    ->fetchField();

  if (!$exists) {
    drupal_load('module', 'smartdocs');
    smartdocs_render_node($model_array, $revision_array, $resource_array, $method_array, $publish, $context);
    $context['message'] = t('Node: :title created', array(':title' => $method_array['displayName']));
  }
  else {
    $context['message'] = t('Node: :title already created', array(':title' => $method_array['displayName']));
  }
}

/**
 * Finish function for when SmartDocs Renders.
 *
 * @param bool $success
 *   TRUE if the batch finished successfully, FALSE if errors occurred.
 * @param array $results
 *   Status details for each item in the batch.
 * @param array $operations
 *   The operations that remained unprocessed in case of failure.
 */
function smartdocs_render_finished($success, $results, $operations) {
  if (!$success) {
    drupal_set_message(t('The render failed, consult the logs for more information.'), 'error');
  }
  elseif (!empty($results)) {
    foreach ($results as $message) {
      drupal_set_message($message, 'warning');
    }
  }
}
