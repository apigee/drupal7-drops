<?php

/**
 * @file
 * The callbacks during Apigee install profile are located in this file.
 */

/**
 * Batch Operation Callback
 */
function apigee_batch_smartdocs_create_model($model_machine_name, $model_display_name, $model_description) {
  // Enable SmartDocs module.
  if (!module_exists('smartdocs')) {
    module_enable(array('smartdocs'), TRUE);
  }

  // Create sample SmartDocs  model.
  $model = new Apigee\SmartDocs\Model(devconnect_default_org_config());
  try {
    $model->load($model_machine_name);
    $update = TRUE;
  } catch (Apigee\Exceptions\ResponseException $e) {
    $update = FALSE;
  }
  $model->setName($model_machine_name);
  $model->setDisplayName($model_display_name);
  $model->setDescription($model_description);
  try {
    $model->save($update);
  } catch (Apigee\Exceptions\ResponseException $e) {
    $message = $e->getResponse();
    $messageObj = @json_decode($message, TRUE);
    if (is_array($messageObj) && array_key_exists('message', $messageObj)) {
      $err_msg = $messageObj['message'];
    }
    else {
      $err_msg = $e->getMessage();
    }
    drupal_set_message($err_msg, 'error');
  }
}

/**
 * Batch Operation Callback
 */
function apigee_batch_smartdocs_import_model($model_machine_name, $model_import_file, $document_format, $content_type) {
  // Create sample SmartDocs  model.
  $model = new Apigee\SmartDocs\Model(devconnect_default_org_config());
  try {
    $model->load($model_machine_name);
  } catch (Apigee\Exceptions\ResponseException $e) {
    drupal_set_message($e->getMessage(), 'error');
  }
  if ($model->getLatestRevisionNumber() <= 0) {
    try {
      // Import the model using the Swagger file.
      $file_contents = file_get_contents($model_import_file);
      $model->importFile($file_contents, $document_format, $content_type);
      drupal_set_message('Sample model %model imported into SmartDocs.', array('%model' => $model->getDisplayName()), 'status');
    } catch (Apigee\Exceptions\ResponseException $e) {
      $message = $e->getResponse();
      $messageObj = @json_decode($message, TRUE);
      if (is_array($messageObj) && array_key_exists('message', $messageObj)) {
        $err_msg = $messageObj['message'];
      }
      else {
        $err_msg = $e->getMessage();
      }
      drupal_set_message($err_msg, 'error');
    }
  }
}
