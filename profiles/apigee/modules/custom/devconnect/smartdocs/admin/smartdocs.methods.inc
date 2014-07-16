<?php

/**
 * Adds a method to a given resource
 *
 * @param $form
 * @param $form_state
 * @param $mid
 * @param $rev
 * @param $res
 * @return mixed
 */
function smartdocs_method_add($form, $form_state, $mid, $rev, $res) {
  module_load_include('inc', 'smartdocs', 'admin/smartdocs.methods.examples');
  $model = entity_get_controller('smartdocs_model')->loadSingle($mid);
  drupal_set_title('Add Method to `' . $model['displayName'] . '`');
  $form['model'] = array(
    '#type' => 'hidden',
    '#value' => $model,
  );
  $form['rev'] = array(
    '#type' => 'hidden',
    '#value' => $rev,
  );
  $form['res'] = array(
    '#type' => 'hidden',
    '#value' => $res,
  );
  $form['display_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Display Name'),
    '#size' => 60,
    '#maxlength' => 128,
  );
  $form['description'] = array(
    '#type' => 'textarea',
    '#title' => t('Description'),
    '#rows' => 5,
  );
  $form['verb'] = array(
    '#type' => 'select',
    '#title' => t('Method Verb'),
    '#options' => array(
      'DELETE' => 'DELETE',
      'GET' => 'GET',
      'PATCH' => 'PATCH',
      'POST' => 'POST',
      'PUT' => 'PUT'
    ),
  );
  $form['authschemes'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Authentication'),
    '#options' => array(
      'NOAUTH' => 'NOAUTH',
      'BASICAUTH' => 'BASICAUTH',
      'CUSTOM' => 'CUSTOM',
      'OAUTH2WEBSERVER' => 'OAUTH2WEBSERVER'
    ),
    '#multiple' => TRUE,
  );
  $types = array(
    'parameters' => t('Parameters'),
    'parameterGroups' => t('Parameter Groups'),
    'tags' => t('Tags'),
    'samples' => t('Samples'),
    'response' => t('Response'),
    'customAttributes' => t('Custom Attributes'),
  );
  foreach ($types as $type => $display) {
    $form['field_' . $type] = array(
      '#type' => 'fieldset',
      '#title' => t($display),
      '#prefix' => '<div id="' . $type .'">',
      '#suffix' => '</div>',
    );
    $form['field_' . $type][$type] = array(
      '#type' => 'textarea',
      '#title' => t($display),
    );
    $form['field_' . $type]['examples'] = array(
      '#type' => 'fieldset',
      '#title' => t('Examples'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );
    $form['field_' . $type]['examples']['ex'] = array(
      '#markup' => smartdocs_method_examples($type),
    );
  }
  $form['submit'] = array(
    '#type' => 'submit',
    '#submit' => array('smartdocs_method_add_submit'),
    '#value' => t('Add Method'),
  );
  return $form;
}

/**
 * Submit callback for Add Resource form
 *
 * @param $form
 * @param $form_state
 */
function smartdocs_method_add_submit($form, &$form_state) {
  $schemes = array();
  foreach($form_state['values']['authschemes'] as $key => $auth) {
    if (is_string($auth)) {
      $schemes[] = $key;
    }
  }
  $payload = array(
    'name' => preg_replace('/[^A-Za-z0-9]/', "", $form_state['values']['display_name']),
    'displayName' => $form_state['values']['display_name'],
    'description' => $form_state['values']['description'],
    'verb' => $form_state['values']['verb'],
    'parameters' => drupal_json_decode($form_state['values']['parameters']),
    'authSchemes' => $schemes
  );
  $mid = $form_state['values']['model']['name'];
  $rev = $form_state['values']['rev'];
  $res = $form_state['values']['res'];
  if (entity_get_controller('smartdocs_method')->createMethod($mid, $rev, $res, $payload)) {
    drupal_set_message('Method created successfully', 'status');
  } else {
    drupal_set_message('Method creation failed.', 'error');
  }
}