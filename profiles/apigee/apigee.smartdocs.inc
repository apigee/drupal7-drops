<?php
/**
 * @file
 * Contains SmartDocs-related functionality at profile install time.
 */

define('SMARTDOCS_SAMPLE_PETSTORE_MODEL', 'petstore_example');
define('SMARTDOCS_SAMPLE_HELLOWORLD_MODEL', 'helloworld_example');

/**
 * Create model and import Swagger/WADL file to load content.
 */
function apigee_smartdocs_import_model_content() {
  global $install_state;

  _apigee_manage_memory();

  if (!$install_state['parameters']['edge_configured']) {
    return NULL;
  }

  // Create sample SmartDocs  model.
  $model_display_name = 'Pet Store Example API';
  $model_description = 'Manage inventory and users through an example REST API patterned after the classic pet store demo.';
  $model_base_url = 'http://petstore.swagger.io/v2';
  $operations[] = array(
    'apigee_batch_smartdocs_create_model',
    array(
      SMARTDOCS_SAMPLE_PETSTORE_MODEL,
      $model_display_name,
      $model_description,
      $model_base_url,
    ),
  );

  // Import pet store Swagger file into pet store model.
  $model_import_file = __DIR__ . '/modules/custom/devconnect/smartdocs/samples/petstore.swagger.json';
  $operations[] = array(
    'apigee_batch_smartdocs_import_model',
    array(
      SMARTDOCS_SAMPLE_PETSTORE_MODEL,
      $model_import_file,
      'swagger',
      'application/json',
    ),
  );

  // Create sample SmartDocs  model.
  $model_display_name = 'Hello World API';
  $model_description = 'A simple "hello world" API.';
  $model_base_url = 'http://mocktarget.apigee.net';
  $operations[] = array(
    'apigee_batch_smartdocs_create_model',
    array(
      SMARTDOCS_SAMPLE_HELLOWORLD_MODEL,
      $model_display_name,
      $model_description,
      $model_base_url,
    ),
  );

  // Import pet store Swagger file into pet store model.
  $model_import_file = __DIR__ . '/modules/custom/devconnect/smartdocs/samples/helloworld.yaml';
  $operations[] = array(
    'apigee_batch_smartdocs_import_model',
    array(
      SMARTDOCS_SAMPLE_HELLOWORLD_MODEL,
      $model_import_file,
      'swagger',
      'application/yaml',
    ),
  );

  $batch = array(
    'operations' => $operations,
    'title' => t('Creating and importing SmartDocs example models'),
    'init_message' => t('SmartDocs model creation has started...'),
    'file' => drupal_get_path('profile', 'apigee') . '/apigee.smartdocs.inc',
  );

  return $batch;
}

/**
 * Renders and publishes pet store and hello-world SmartDocs nodes.
 *
 * @return mixed
 *   The batch API array.
 */
function apigee_smartdocs_render_model_content() {
  global $install_state;

  _apigee_manage_memory();

  if (!$install_state['parameters']['edge_configured']) {
    return NULL;
  }

  drupal_set_message(t('Rendering SmartDocs example documentation pages.'), 'status');
  require_once drupal_get_path('module', 'smartdocs') . '/batch/smartdocs.render.inc';

  // Get batches to render model nodes.
  $batch_hello = get_smartdocs_render_batch(SMARTDOCS_SAMPLE_HELLOWORLD_MODEL);
  $batch_pet = get_smartdocs_render_batch(SMARTDOCS_SAMPLE_PETSTORE_MODEL);

  $operations = array();
  if ($batch_hello != NULL) {
    $operations = array_merge($operations, $batch_hello['operations']);
  }
  if ($batch_pet != NULL) {
    $operations = array_merge($operations, $batch_pet['operations']);
  }

  // Merge the pet and hello-world batch to return.
  $batch = array(
    'operations' => $operations,
    'title' => t('Rendering SmartDocs documentation pages'),
    'init_message' => t('SmartDocs rendering has started...'),
    'progress_message' => t('Processed @current out of @total.'),
    'error_message' => t('Rendering SmartDocs nodes has encountered an error.'),
    'file' => drupal_get_path('module', 'smartdocs') . '/batch/smartdocs.render.inc',
  );

  return $batch;
}

/**
 * Get batch for rendering all nodes for a model.
 *
 * @return array
 *   The batch for Drupal batch API.
 */
function get_smartdocs_render_batch($model_machine_name) {

  try {
    $model = new Apigee\SmartDocs\Model(devconnect_default_org_config());
    $model->load($model_machine_name);
    $revision = new Apigee\SmartDocs\Revision($model->getConfig(), $model->getUuid());
    $rev = max($model->getLatestRevisionNumber(), 1);
    $revision->load($rev);
  }
  catch (Apigee\Exceptions\ResponseException $e) {
    $message = $e->getResponse();
    $message_object = @json_decode($message, TRUE);
    if (is_array($message_object) && array_key_exists('message', $message_object)) {
      $err_msg = $message_object['message'];
    }
    else {
      $err_msg = $e->getMessage();
    }
    $msg_args = array('%model' => $model_machine_name, '!error_message' => $err_msg);
    drupal_set_message(t('Error rendering SmartDocs methods for %model: !error_message', $msg_args), 'error');
    watchdog('apigee', 'Error rendering SmartDocs methods for %model: !error_message', $msg_args, WATCHDOG_ERROR);
    return NULL;
  }
  catch (Exception $e) {
    $msg_args = array('%model' => $model_machine_name, '!error_message' => $e->getMessage());
    drupal_set_message(t('Error rendering SmartDocs methods for %model: !error_message', $msg_args), 'error');
    watchdog('apigee', 'Error rendering SmartDocs methods for %model: !error_message', $msg_args, WATCHDOG_ERROR);
    return NULL;
  }

  $selected = array();
  foreach ($revision->getResources() as $resource) {
    foreach ($resource->getMethods() as $method) {
      $selected[$method->getUuid()] = $method->getUuid();
    }
  }

  $model_array = array(
    'id' => $model->getUuid(),
    'name' => $model->getName(),
    'displayName' => $model->getDisplayName(),
    'description' => $model->getDescription(),
  );
  $revision_array = smartdocs_prepare_revision($revision);

  $batch = smartdocs_render($model_array, $revision_array, $selected, array('publish' => 'publish'), TRUE);
  return $batch;
}

/**
 * Batch operation callback to create a model.
 *
 * Checks to see if a given model exists, and if not, creates it.
 *
 * @param string $model_machine_name
 *   Machine name of the model to be created.
 * @param string $model_display_name
 *   Display name of the model to be created.
 * @param string $model_description
 *   Description of the model to be created.
 * @param string $model_base_url
 *   Base URL to use when creating a new revision.
 *
 * @throws Exception
 *   Thrown if the model exists, has a bogus revision number, and cannot be
 *   deleted. This prevents us from trying to import into an invalid model.
 */
function apigee_batch_smartdocs_create_model($model_machine_name, $model_display_name, $model_description, $model_base_url) {
  // Enable SmartDocs module.
  if (!module_exists('smartdocs')) {
    module_enable(array('smartdocs'), TRUE);
  }

  // Create sample SmartDocs  model.
  $model = new Apigee\SmartDocs\Model(devconnect_default_org_config());
  try {
    $model->load($model_machine_name);
    $update = TRUE;
  }
  catch (Apigee\Exceptions\ResponseException $e) {
    // Thrown if model does not yet exist.
    $update = FALSE;
  }
  $valid_revision_state = TRUE;
  if ($update) {
    $latest_revision_num = $model->getLatestRevisionNumber();
    if ($latest_revision_num < 0) {
      $valid_revision_state = FALSE;
    }
    elseif ($latest_revision_num > 0) {
      $revision = new Apigee\SmartDocs\Revision($model->getConfig(), $model->getUuid());
      try {
        $revision->load($latest_revision_num);
      }
      catch (Apigee\Exceptions\ResponseException $e) {
        $valid_revision_state = FALSE;
      }
    }

    if (!$valid_revision_state) {
      // If revision state is not valid (latestRevisionNumber is either -1 or
      // refers to a nonexistent revision) we create a new revision.
      $revision = new Apigee\SmartDocs\Revision($model->getConfig(), $model->getUuid());
      $revision->setBaseUrl($model_base_url);
      try {
        $revision->save();
      }
      catch (Apigee\Exceptions\ResponseException $e) {
        $msg_args = array('%model' => $model_display_name, '!error_message' => $e->getMessage());
        drupal_set_message(t('Error creating revision for %model: !error_message', $msg_args), 'error');
        watchdog('apigee', 'Error creating revision for %model: !error_message', $msg_args, WATCHDOG_ERROR);
      }
    }

    // If the model loaded and is not invalid, then no reason to modify it;
    // just return.
    return;
  }

  $model->setName($model_machine_name);
  $model->setDisplayName($model_display_name);
  $model->setDescription($model_description);
  try {
    $model->save($update);
    // Ignore errors in which latestRevisionNumber is -1. These will be handled
    // when the model is imported.
  }
  catch (Apigee\Exceptions\ResponseException $e) {
    $message = $e->getResponse();
    $message_obj = @json_decode($message, TRUE);
    if (is_array($message_obj) && array_key_exists('message', $message_obj)) {
      $err_msg = $message_obj['message'];
    }
    else {
      $err_msg = $e->getMessage();
    }
    $msg_args = array('%model' => $model_display_name, '!error_message' => $err_msg);

    drupal_set_message(t('Error creating %model: !error_message', $msg_args), 'error');
    watchdog('apigee', 'Error creating %model: !error_message', $msg_args, WATCHDOG_ERROR);

  }
  catch (Exception $e) {
    $msg_args = array('%model' => $model_display_name, '!error_message' => $e->getMessage());

    drupal_set_message(t('Error creating %model: !error_message', $msg_args), 'error');
    watchdog('apigee', 'Error creating %model: !error_message', $msg_args, WATCHDOG_ERROR);
  }
}

/**
 * Batch operation callback to import a revision from a file into a model.
 *
 * @param string $model_machine_name
 *   Machine name of the model to be imported.
 * @param string $model_import_file
 *   Name of the file containing import data.
 * @param string $document_format
 *   Format of the document type. May be 'wadl', 'swagger' or 'apimodel'.
 * @param string $content_type
 *   MIME type of the document.
 */
function apigee_batch_smartdocs_import_model($model_machine_name, $model_import_file, $document_format, $content_type) {
  // Create sample SmartDocs  model.
  $model = new Apigee\SmartDocs\Model(devconnect_default_org_config());
  try {
    $model->load($model_machine_name);
  }
  catch (Apigee\Exceptions\ResponseException $e) {
    $message = $e->getResponse();
    $message_obj = @json_decode($message, TRUE);
    if (is_array($message_obj) && array_key_exists('message', $message_obj)) {
      $err_msg = $message_obj['message'];
    }
    else {
      $err_msg = $e->getMessage();
    }
    $msg_args = array('%model' => $model_machine_name, '!error_message' => $err_msg);
    drupal_set_message(t('Error importing %model: !error_message', $msg_args), 'error');
    watchdog('apigee', 'Error importing %model: !error_message', $msg_args, WATCHDOG_ERROR);
  }
  catch (Exception $e) {
    $msg_args = array('%model' => $model_machine_name, '!error_message' => $e->getMessage());
    drupal_set_message(t('Error importing %model: !error_message', $msg_args), 'error');
    watchdog('apigee', 'Error importing %model: !error_message', $msg_args, WATCHDOG_ERROR);
  }
  if ($model->getLatestRevisionNumber() <= 0) {
    try {
      // Import the model using the Swagger file.
      $file_contents = file_get_contents($model_import_file);
      $model->importFile($file_contents, $document_format, $content_type);
      drupal_set_message(t('Sample model %model imported into SmartDocs.', array('%model' => $model->getDisplayName())), 'status');
    }
    catch (Apigee\Exceptions\ResponseException $e) {
      $message = $e->getResponse();
      $message_obj = @json_decode($message, TRUE);
      if (is_array($message_obj) && array_key_exists('message', $message_obj)) {
        $err_msg = $message_obj['message'];
      }
      else {
        $err_msg = $e->getMessage();
      }
      drupal_set_message(t('Error importing %model: !error_message.', array('%model' => $model_machine_name, '!error_message' => $err_msg)), 'error');
      watchdog('apigee', 'Error importing %model: !error_message.', array('%model' => $model_machine_name, '!error_message' => $err_msg), WATCHDOG_ERROR);
    }
    catch (Exception $e) {
      drupal_set_message(t('Error importing %model: !error_message.', array('%model' => $model_machine_name, '!error_message' => $e->getMessage())), 'error');
      watchdog('apigee', 'Error importing %model: !error_message.', array('%model' => $model_machine_name, '!error_message' => $e->getMessage()), WATCHDOG_ERROR);
    }
  }
}
