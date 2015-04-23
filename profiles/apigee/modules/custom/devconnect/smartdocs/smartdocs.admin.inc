<?php

use Apigee\SmartDocs\AuthScheme;
use Apigee\SmartDocs\Method;
use Apigee\SmartDocs\Model;
use Apigee\SmartDocs\Oauth2Credentials;
use Apigee\SmartDocs\Resource;
use Apigee\SmartDocs\Revision;
use Apigee\SmartDocs\Template;
use Apigee\SmartDocs\TokenCredentials;

use Apigee\Exceptions\ResponseException;

/**
 * SmartDocs Settings Form
 *
 * @param array $form
 * @param array $form_state
 * @return array
 */
function smartdocs_settings($form, $form_state) {
  $form['get_help'] = array(
    '#type' => 'markup',
    '#markup' => t('For additional help in configuring SmartDocs, please consult the <a href="@url">online documentation</a>.', array('@url' => 'http://apigee.com/docs/developer-services/content/using-smartdocs-document-apis')),
    '#prefix' => '<div style="margin-bottom:1.5em">',
    '#suffix' => '</div>'
  );

  $form['disable_ssl_warning'] = array(
    '#title' => t("Disable SSL warning message"),
    '#type' => 'checkbox',
    '#default_value' => variable_get('disable_ssl_warning', 0),
    '#description' => t("Select this option if you would like the warning message shown, if SSL is not enabled.")
  );
  $form['cache_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Cache settings for SmartDocs'),
    '#collapsed' => FALSE,
    '#collapsible' => FALSE,
  );
  $form['cache_settings']['clear_cache'] = array(
    '#type' => 'submit',
    '#value' => t('Rebuild SmartDocs model caches'),
    '#submit' => array('smartdocs_rebuild_cache_table')
  );
  $form['products'] = array(
    '#type' => 'fieldset',
    '#title' => t('API Products / SmartDocs Settings'),
    '#collapsed' => TRUE,
    '#collapsible' => TRUE,
  );
  $form['products']['info'] = array(
    '#markup' => '<p>This section creates a correlation between your products and SmartDocs internally.</p><br/>'
  );
  try {
    $modelObj = new Model(devconnect_default_org_config());
    $models = $modelObj->listModels();
  } catch (ResponseException $e) {
    $models = array();
  }

  $options = array();
  /** @var Apigee\SmartDocs\Model $model */
  foreach ($models as $model) {
    $model_name = $model->getDisplayName();
    if (empty($model_name)) {
      $model_name = $model->getName();
    }

    $options[$model_name] = $model->getName();
  }
  $options = array_flip($options);
  natcasesort($options);

  $api_products = entity_load('api_product', FALSE, array('show_private' => TRUE));
  $products = array();
  foreach ($api_products as $product) {
    $products[$product->displayName] = $product->name;
  }
  $products = array_flip($products);
  natcasesort($products);

  foreach ($products as $machine => $product) {
    $form['products'][$machine . '_docs'] = array(
      '#type' => 'select',
      '#title' => t($product . ' models'),
      '#default_value' => variable_get($machine . '_docs', array()),
      '#description' => t('Specify the SmartDocs model that relates to the product.'),
      '#options' => $options,
      '#multiple' => TRUE,
      '#attributes' => array(
        'style' => 'height:200px;',
      ),
      '#suffix' => '<br/>',
    );
  }

  $form['smartdocs_path_gen_type'] = array(
    '#type' => 'radios',
    '#title' => t('Path generation'),
    '#description' => t('Specify the path generation type.'),
    '#default_value' => variable_get('smartdocs_path_gen_type', 'path'),
    '#options' => array(
      'path' => t('Resource Path with Method Type'),
      'title' => t('Method Title'),
      // 'uuid' => t('Method UUID'),
    ),
  );
  $form['smartdocs_publish_on_render'] = array(
    '#type' => 'checkbox',
    '#title' => t('Publish rendered nodes immediately'),
    '#description' => t('If selected, this will publish SmartDocs nodes as soon as they are rendered.'),
    '#default_value' => variable_get('smartdocs_publish_on_render', 1),
    '#return_value' => 1,
  );

  $form['settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Advanced settings'),
    '#collapsed' => TRUE,
    '#collapsible' => TRUE,
  );
  $form['settings']['use_docgen_templates'] = array(
    '#type' => 'checkbox',
    '#disabled' => TRUE,
    '#title' => t('Use modeling template system'),
    '#default_value' => variable_get('use_docgen_templates', 1),
    '#description' => t('By enabling this setting, you will override all node templates with model templates.'),
    '#suffix' => '<br/>',
  );
  $form['settings']['custom_template_file'] = array(
    '#title' => t("Upload customized model template"),
    '#type' => 'managed_file',
    '#description' => t('Upload a .hbr template file. This template file will be used to render the methods instead of the default template'),
    '#upload_location' => 'public://smartdocs_templates',
    '#upload_validators' => array(
      'file_validate_extensions' => array('hbr')
    ),
    '#default_value' => variable_get('custom_template_file', 0),
    '#suffix' => '<br/>',
  );
  $form['settings']['data_proxy_url'] = array(
    '#type' => 'textfield',
    '#title' => t('Set custom proxy URL'),
    '#default_value' => variable_get('data_proxy_url', 'https://apiconsole-prod.apigee.net/smartdocs/v1'),
  );
  $form['settings']['data_auth_url'] = array(
    '#type' => 'textfield',
    '#title' => t('Set custom auth URL'),
    '#default_value' => variable_get('data_auth_url', 'https://api.enterprise.apigee.com/v1/users/{user}/authenticate'),
  );
  $form['settings']['api_proxy'] = array(
    '#type' => 'fieldset',
    '#title' => t('Management API override settings'),
    '#collapsed' => TRUE,
    '#collapsible' => TRUE,
  );
  $form['settings']['api_proxy']['use_local_assets'] = array(
    '#type' => 'checkbox',
    '#title' => t('Use local SmartDocs JS/CSS'),
    '#default_value' => variable_get('use_local_assets', 0),
    '#description' => t('By enabling this setting, you will use local assets hosted by DevPortal.'),
    '#suffix' => '<br/>',
  );
  $form['settings']['api_proxy']['local_asset_version'] = array(
    '#type' => 'select',
    '#title' => t('Local SmartDocs JS/CSS version'),
    '#default_value' => variable_get('local_asset_version', '5'),
    '#description' => t('By enabling this setting, you will use local assets hosted by DevPortal.'),
    '#options' => array(
      '1' => 'v1',
      '2' => 'v2',
      '3' => 'v3',
      '4' => 'v4',
      '5' => 'v5',
    ),
    '#suffix' => '<br/>',
  );
  $form['settings']['api_proxy']['use_api_proxy_url'] = array(
    '#type' => 'checkbox',
    '#title' => t('Use Management API URL override'),
    '#default_value' => variable_get('use_api_proxy_url', 0),
    '#description' => t('By enabling this setting, you will override the default devportal endpoint for SmartDocs calls.'),
    '#suffix' => '<br/>',
  );
  $form['settings']['api_proxy']['api_proxy_url'] = array(
    '#type' => 'textfield',
    '#title' => t('Management API URL override'),
    '#default_value' => variable_get('api_proxy_url', 'https://api.enterprise.apigee.com/v1'),
  );
  $url = l('SmartDocs documentation', 'http://apigee.com/docs/developer-services/content/using-smartdocs-document-apis', array(
    'html' => TRUE,
    'attributes' => array(
      'target' => '_blank',
    )
  ));
  $learn = 'For more information, including configuration of the Management API URL Override, please consult: ';
  $form['settings']['api_proxy_info'] = array(
    '#markup' => $learn . $url,
  );
  $form['#submit'][] = 'smartdocs_settings_submit';
  return system_settings_form($form);
}

/**
 * Submit callback for the smartdocs_settings(). To handle the template file upload.
 *
 * @param array $form
 * @param array $form_state
 */
function smartdocs_settings_submit($form, &$form_state) {

  $current_template = variable_get('custom_template_file', NULL);
  $template = $form_state['values']['custom_template_file'];
  if ($template == $current_template) {
    return;
  }

  if ($current_template != NULL) {
    $current_file = file_load($current_template);
    file_usage_delete($current_file, 'smartdocs');
    file_delete($current_file, TRUE);
  }
  if (!empty($template)) {
    $file = file_load($template);
    if ($file) {
      $file->status = FILE_STATUS_PERMANENT;
      file_save($file);
      file_usage_add($file, 'smartdocs', 'smartdocs', 1);
      variable_set('custom_template_file', $file->fid);
    }
  }
}

/**
 * Page callback to list all models.
 */
function smartdocs_model_list() {

  drupal_add_css(drupal_get_path('module', 'views') . '/css/views-admin.ctools.css');

  $output['add_model'] = array(
    '#type' => 'link',
    '#title' => t('Add a new model'),
    '#href' => 'admin/smartdocs/models/add',
    '#prefix' => '<div style="margin-bottom:1.5em">',
    '#suffix' => '</div>'
  );

  $models = _smartdocs_get_methods_details();
  if (empty($models)) {
    return $output;
  }

  $table_rows = array();
  /** @var Apigee\SmartDocs\Model $model */
  foreach ($models as $model) {
    $latest_rev = $model->getLatestRevisionNumber();
    if ($latest_rev > 0) {
      $model_name = l($model->getDisplayName(), 'admin/smartdocs/models/' . $model->getUuid() . '/revisions/' . $model->getLatestRevisionNumber());
    }
    else {
      $model_name = l($model->getDisplayName(), 'admin/smartdocs/models/' . $model->getUuid());
    }

    $method_count = $node_count = $published_count = 0;
    /** @var Apigee\SmartDocs\Revision $revision */
    $revision = $model->getActiveRevision();
    if (!empty($revision)) {
      /** @var Apigee\SmartDocs\Resource $resource */
      foreach ($revision->getResources() as $resource) {
        /** @var Apigee\SmartDocs\Method $method */
        foreach ($resource->getMethods() as $method) {
          $method_count++;
          if ($method->getMetadata('nid')) {
            $node_count++;
          }
          if ($method->getMetadata('status')) {
            $published_count++;
          }
        }
      }
    }

    $ops = array();
    if ($published_count && $view_path = _smartdocs_get_revision_view_path($model)) {
      $ops['list-' . $model->getName()] = array(
        'title' => 'View Published Docs',
        'href' => $view_path,
        'attributes' => array(
          'title' => 'View Published Documentation',
          'class' => array('list-' . $model->getName(), 'views-ajax-link')
        )
      );
    }
    $operations = array(
      'revisions' => 'API Revisions',
      'add' => 'Add API Revision',
      'import' => 'Import',
      'settings' => 'Settings',
      'export' => 'Export',
      'delete' => 'Delete'
    );
    foreach ($operations as $op => $disp) {
      $ops['list-' . $op] = array(
        'title' => $disp,
        'href' => 'admin/smartdocs/models/' . $model->getUuid() . '/' . $op,
        'attributes' => array(
          'title' => $disp,
          'class' => array($op . '-' . $model->getName(), 'views-ajax-link')
        )
      );
    }
    $table_rows[] = array(
      $model_name,
      $model->getDescription(),
      $method_count,
      $node_count,
      $published_count,
      theme('links__ctools_dropbutton', array('links' => $ops))
    );
  }
  $header = array(
    'name' => t('Name'),
    'description' => t('Description'),
    'total' => t('Total methods'),
    'rendered' => t('Rendered'),
    'published' => t('Published'),
    'operations' => array(
      'data' => t('Operations'),
      'class' => array(
        'smartdocs-operations'
      ),
    ),
  );

  $output['table'] = array(
    '#markup' => theme('table', array('header' => $header, 'rows' => $table_rows))
  );

  return $output;
}

/**
 * Page callback for admin/content/smartdocs/add-model.
 *
 * @param $form
 * @param $form_state
 * @return array
 */
function smartdocs_add_model($form, &$form_state) {
  $form['#attached'][] = 'misc/machine-name.js';

  $form['display_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#size' => 60,
    '#maxlength' => 128,
    '#required' => TRUE,
  );
  $form['model_name'] = array(
    '#type' => 'machine_name',
    '#title' => t('Internal name'),
    '#description' => t('A unique machine-readable name. Can only contain letters, digits, underscores and hyphens.'),
    '#machine_name' => array(
      'exists' => 'smartdocs_model_exist',
      'source' => array('display_name'),
      'label' => t('Internal name'),
      'replace_pattern' => '[^A-Za-z0-9_-]+',
      'replace' => '-',
      'error' => 'The internal name must contain only letters, digits, underscores, or hyphens.'
    ),
    '#required' => TRUE,
    '#access' => TRUE,
  );
  $form['model_description'] = array(
    '#type' => 'textarea',
    '#title' => t('Description'),
  );
  $form['model_submit'] = array(
    '#type' => 'submit',
    '#value' => t('Create Model')
  );
  return $form;
}

/**
 * Validator callback for smartdocs_add_model().
 *
 * Ensures that the magic name 'add' is not used as a model name.
 *
 * @param array $form
 * @param array $form_state
 */
function smartdocs_add_model_validate(array $form, array &$form_state) {
  if ($form_state['values']['model_name'] == 'add') {
    form_error($form['model_name'], t('The internal name %name is a reserved word. Please select a different internal name.', array('%name' => $form_state['values']['model_name'])));
  }
}

/**
 * Submit Callback for smartdocs_add_model().
 *
 * Attempts to save a model entity based on form values.
 *
 * @param array $form
 * @param array $form_state
 */
function smartdocs_add_model_submit(array $form, array &$form_state) {

  $model = new Model(devconnect_default_org_config());
  $model->setName(trim($form_state['values']['model_name']));
  $model->setDisplayName(trim($form_state['values']['display_name']));
  $model->setDescription(trim($form_state['values']['model_description']));

  $success = FALSE;
  try {
    $model->save();
    $success = TRUE;
    cache_clear_all('model:' . $model->getUuid(), 'cache_smartdocs');
    module_invoke_all('smartdocs_model_update', $model->getUuid());
  } catch (Exception $e) {
    drupal_set_message(t('There was an error when trying to create the model.'), 'error');
    watchdog_exception('smartdocs', $e);
  }
  if ($success) {
    menu_rebuild();
    drupal_set_message($model->getDisplayName() . ' was created successfully.', 'status');
  }
  $form_state['redirect'] = 'admin/smartdocs/models/' . $model->getUuid();
}

/**
 * Function that checks if the model exists already.
 *
 * @param $name
 * @return bool
 */
function smartdocs_model_exist($name) {
  try {
    $modelObj = new Model(devconnect_default_org_config());
    $models = $modelObj->listModels();
  } catch (ResponseException $e) {
    return FALSE;
  }
  $scrubbed_name = smartdocs_model_name($name);
  /** @var Apigee\Smartdocs\Model $model */
  foreach ($models as $model) {
    if ($model->getName() == $scrubbed_name) {
      return TRUE;
    }
  }
  return FALSE;
}


function smartdocs_show_model_detail(Model $model) {

  drupal_add_css(drupal_get_path('module', 'views') . '/css/views-admin.ctools.css');
  $output = array();
  $model_name = $model->getName();
  $model_uuid = $model->getUuid();

  $operations = array();
  $revision = $model->getActiveRevision();
  if ($revision) {
    $operations['revisions'] = 'API Revisions';
  }
  drupal_set_title('‘' . $model->getDisplayName() . '’ Details');

  $count_nids = 0;
  if ($revision) {
    /** @var Apigee\SmartDocs\Resource $resource */
    foreach ($revision->getResources() as $resource) {
      /** @var Apigee\SmartDocs\Method $method */
      foreach ($resource->getMethods() as $method) {
        if ($method->getMetadata('status')) {
          $count_nids++;
        }
      }
    }
  }

  $operations = array(
      'add' => 'Add API Revision',
      'import' => 'Import',
      'settings' => 'Settings',
      'export' => 'Export',
      'delete' => 'Delete'
    ) + $operations;

  $links = array();
  foreach ($operations as $key => $op) {
    $links[$key . '-' . $model_name] = array(
      'title' => $op,
      'href' => 'admin/smartdocs/models/' . $model_uuid . '/' . $key,
      'attributes' => array(
        'title' => $op,
        'class' => array(
          $key . '-' . $model_name,
          'views-ajax-link'
        ),
      ),
    );
  }
  if ($count_nids && $view_path = _smartdocs_get_revision_view_path($model)) {
    $link = array(
      'title' => 'View Published Docs',
      'href' => $view_path,
      'attributes' => array(
        'title' => 'View Published Documentation',
        'class' => array('list-' . $model_name, 'views-ajax-link')
      )
    );
    $links = array('list-' . $model_name => $link) + $links;
  }
  $output['name'] = array(
    '#item',
    '#title' => t('Name'),
    '#markup' => $model->getDisplayName(),
  );
  $output['description'] = array(
    '#type' => 'item',
    '#title' => t('Description'),
    '#markup' => $model->getDescription(),
  );
  if ($revision) {
    $output['revision'] = array(
      '#type' => 'item',
      '#title' => t('Latest Revision'),
      '#markup' => l(t('Revision #@rev', array('@rev' => $revision)), 'admin/smartdocs/models/' . $model_uuid . '/revisions/' . $revision->getRevisionNumber()),
    );
  }
  $output['created'] = array(
    '#type' => 'item',
    '#title' => t('Created'),
    '#markup' => format_date($model->getCreatedTime(), 'custom', 'j F Y'),
  );
  if ($mod_time = $model->getModifiedTime()) {
    $output['modified'] = array(
      '#type' => 'item',
      '#title' => t('Updated'),
      '#markup' => format_date($mod_time, 'custom', 'j F Y'),
    );
  }
  $output['operations'] = array(
    '#type' => 'item',
    '#title' => t('Operations'),
    '#markup' => theme('links__ctools_dropbutton', array('links' => $links)),
  );

  return $output;
}

/**
 * Finds the listing view path associated with a revision, if any.
 *
 * @param array $revision
 * @return string|bool
 */
function _smartdocs_get_revision_view_path(Model $model) {
  $view = views_get_view($model->getName() . '_listing');
  if ($view) {
    if (array_key_exists('page', $view->display) && array_key_exists('path', $view->display['page']->display_options)) {
      return $view->display['page']->display_options['path'];
    }
  }
  return FALSE;
}


/**
 * Imports WADL / Swagger data into SmartDocs
 *
 * @param array $form
 * @param array $form_state
 * @param array $model
 */
function smartdocs_import_model(array $form, array $form_state, Model $model) {
  drupal_set_title('Import into ‘' . $model->getDisplayName() . '’');

  $form['model'] = array(
    '#type' => 'value',
    '#value' => $model->toArray()
  );
  $form['format'] = array(
    '#prefix' => '<div id="import-format">',
    '#suffix' => '</div>',
    '#type' => 'fieldset',
    '#title' => t('Import Options'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['format']['type'] = array(
    '#type' => 'select',
    '#title' => t('Choose format'),
    '#options' => array(
      'wadl' => 'WADL',
      'swagger' => 'Swagger',
      'apigee_json' => 'SmartDocs JSON',
    ),
    '#ajax' => array(
      'wrapper' => 'import-format',
      'callback' => 'smartdocs_import_ajax',
    ),
    '#default_value' => array('wadl'),
  );
  if (isset($form_state['values']['type']) || $form['format']['type']['#default_value'][0] == 'wadl') {
    if (isset($form_state['values']) && array_key_exists('type', $form_state['values'])) {
      switch ($form_state['values']['type']) {
        case 'swagger':
          $form['format']['import']['swagger_model_import'] = array(
            '#type' => 'textfield',
            '#title' => t('Swagger URL'),
            '#size' => 60,
            '#maxlength' => 128,
            '#default_value' => '',
            '#required' => TRUE,
          );
          break;
        case 'apigee_json':
          $form['format']['import']['upload_type'] = array(
            '#type' => 'select',
            '#title' => t('Upload Type'),
            '#options' => array(
              'upload' => 'File',
              'path' => 'URL Path',
            ),
            '#ajax' => array(
              'wrapper' => 'import-format',
              'callback' => 'smartdocs_import_ajax',
            ),
            '#default_value' => array('upload'),
          );
          if (isset($form_state['values']['upload_type'])) {
            switch ($form_state['values']['upload_type']) {
              case 'upload':
                $form['format']['import']['file_model_import'] = array(
                  '#type' => 'managed_file',
                  '#title' => t('File'),
                  '#description' => t('Allowed extensions: .json'),
                  '#upload_location' => 'public://jsons',
                  '#upload_validators' => array(
                    'file_validate_extensions' => array('json'),
                  ),
                  '#required' => TRUE,
                );
                break;
              case 'path':
                $form['format']['import']['path_model_import'] = array(
                  '#type' => 'textfield',
                  '#title' => t('URL Path'),
                  '#size' => 60,
                  '#maxlength' => 128,
                  '#default_value' => '',
                  '#required' => TRUE,
                );
                break;
              default:
                $form['format']['import']['file_model_import'] = array(
                  '#type' => 'managed_file',
                  '#title' => t('File'),
                  '#description' => t('Allowed extensions: .json'),
                  '#upload_location' => 'public://jsons',
                  '#upload_validators' => array(
                    'file_validate_extensions' => array('json'),
                  ),
                  '#required' => TRUE,
                );
            }
          }
          else {
            $form['format']['import']['file_model_import'] = array(
              '#type' => 'managed_file',
              '#title' => t('File'),
              '#description' => t('Allowed extensions: .json'),
              '#upload_location' => 'public://jsonss',
              '#upload_validators' => array(
                'file_validate_extensions' => array('json'),
              ),
              '#required' => TRUE,
            );
          }
          break;
        default:
          $form['format']['import']['upload_type'] = array(
            '#type' => 'select',
            '#title' => t('Upload Type'),
            '#options' => array(
              'upload' => 'File',
              'path' => 'URL Path',
            ),
            '#ajax' => array(
              'wrapper' => 'import-format',
              'callback' => 'smartdocs_import_ajax',
            ),
            '#default_value' => array('upload'),
          );
          if (isset($form_state['values']['upload_type'])) {
            switch ($form_state['values']['upload_type']) {
              case 'upload':
                $form['format']['import']['file_model_import'] = array(
                  '#type' => 'managed_file',
                  '#title' => t('File'),
                  '#description' => t('Allowed extensions: .wadl .xml'),
                  '#upload_location' => 'public://wadls',
                  '#upload_validators' => array(
                    'file_validate_extensions' => array('wadl xml'),
                  ),
                  '#required' => TRUE,
                );
                break;
              case 'path':
                $form['format']['import']['path_model_import'] = array(
                  '#type' => 'textfield',
                  '#title' => t('URL Path'),
                  '#size' => 60,
                  '#maxlength' => 128,
                  '#default_value' => '',
                  '#required' => TRUE,
                );
                break;
              default:
                $form['format']['import']['file_model_import'] = array(
                  '#type' => 'managed_file',
                  '#title' => t('File'),
                  '#description' => t('Allowed extensions: .wadl .xml'),
                  '#upload_location' => 'public://wadls',
                  '#upload_validators' => array(
                    'file_validate_extensions' => array('wadl xml'),
                  ),
                  '#required' => TRUE,
                );
            }
          }
          else {
            $form['format']['import']['file_model_import'] = array(
              '#type' => 'managed_file',
              '#title' => t('File'),
              '#description' => t('Allowed extensions: .wadl .xml'),
              '#upload_location' => 'public://wadls',
              '#upload_validators' => array(
                'file_validate_extensions' => array('wadl xml'),
              ),
              '#required' => TRUE,
            );
          }
          break;
      }
    }
    else {
      $form['format']['import']['upload_type'] = array(
        '#type' => 'select',
        '#title' => t('Upload Type'),
        '#options' => array(
          'upload' => 'File',
          'path' => 'URL Path',
        ),
        '#ajax' => array(
          'wrapper' => 'import-format',
          'callback' => 'smartdocs_import_ajax',
        ),
        '#default_value' => array('upload'),
      );
      $form['format']['import']['file_model_import'] = array(
        '#type' => 'managed_file',
        '#title' => t('File'),
        '#description' => t('Allowed extensions: .wadl .xml'),
        '#upload_location' => 'public://wadls',
        '#upload_validators' => array(
          'file_validate_extensions' => array('wadl xml'),
        ),
        '#required' => TRUE,
      );
    }
  }
  $form['import'] = array(
    '#type' => 'submit',
    '#value' => t('Import')
  );
  return $form;
}

/**
 * Ajax callback for import format
 */
function smartdocs_import_ajax($form, &$form_state) {
  $form['#rebuild'] = TRUE;
  return $form['format'];
}

/**
 * Validate callback for smartdocs_import_model
 *
 * @param $form
 * @param $form_state
 */
function smartdocs_import_model_validate($form, &$form_state) {
  switch ($form_state['values']['type']) {
    case 'swagger':
      if (isset($form_state['values']['swagger_model_import'])) {
        if (!(filter_var($form_state['values']['swagger_model_import'], FILTER_VALIDATE_URL))) {
          form_set_error('swagger_model_import', 'The Swagger URL is not a valid URL.');
        }
      }
      break;
    default:
      $format = $form_state['values']['type'] == 'apigee_json' ? 'SmartDocs JSON' : 'WADL';
      $upload_type = (array_key_exists('upload_type', $form_state['values']) ? $form_state['values']['upload_type'] : NULL);
      switch ($upload_type) {
        case 'path':
          if (isset($form_state['values']['path_model_import'])) {
            if (!(filter_var($form_state['values']['path_model_import'], FILTER_VALIDATE_URL))) {
              form_set_error('path_model_import', "The $format URL is not a valid URL.");
            }
          }
          break;
        default:
          break;
      }
      break;
  }
}

/**
 * Imports WADL / Swagger data into SmartDocs
 *
 * @param array $form
 * @param array $form_state
 */
function smartdocs_import_model_submit(array $form, array &$form_state) {
  $type = $form_state['values']['type'];
  /** @var Apigee\SmartDocs\Model $model */
  $model = new Model(devconnect_default_org_config());
  Model::fromArray($model, $form_state['values']['model']);

  $message = t('There was an error importing your data.');
  $revision = new Revision($model->getConfig(), $model->getUuid());
  switch ($type) {
    case 'wadl':
      $action = $form_state['values']['upload_type'];
      if ($action == 'upload') {
        $file = file_load($form_state['values']['file_model_import']);
        $file->status = FILE_STATUS_PERMANENT;
        file_save($file);
        $xml = file_get_contents($file->uri);
      }
      else {
        $xml = file_get_contents($form_state['values']['path_model_import']);
      }
      try {
        $revision->importWadl($xml);
        $message = t('The WADL XML has been imported into the model.');
      } catch (Exception $e) {

      }

      break;
    case 'swagger':
      $url = $form_state['values']['swagger_model_import'];
      try {
        $revision->importSwagger($url);
        $message = t('The Swagger data has been imported into the model.');
      } catch (Exception $e) {

      }

      break;
    case 'apigee_json':
      $action = $form_state['values']['upload_type'];
      if ($action == 'upload') {
        $file = file_load($form_state['values']['file_model_import']);
        $file->status = FILE_STATUS_PERMANENT;
        file_save($file);
        $json = file_get_contents($file->uri);
        $message = t('The WADL XML has been imported into the model.');
      }
      else {
        $json = file_get_contents($form_state['values']['path_model_import']);
      }
      try {
        $revision->importApigeeJson($json);
        $message = t('The JSON data has been imported into the model.');
      } catch (Exception $e) {

      }
      break;
  }
  cache_clear_all('model:' . $model->getUuid(), 'cache_smartdocs');
  if ($revision->getRevisionNumber() > 0) {
    drupal_set_message($message, 'status');
    $form_state['redirect'] = 'admin/smartdocs/models/' . $model->getUuid() . '/revisions/' . $revision->getRevisionNumber();
  }
  else {
    drupal_set_message('There was an error importing your data. Please consult the logs.', 'error');
  }
}

/**
 * Exports Model into specified format
 *
 * @param array $form
 * @param array $form_state
 * @param Apigee\SmartDocs\Model $model
 * @param Apigee\SmartDocs\Revision|null $revision
 */
function smartdocs_model_export(array $form, array &$form_state, Model $model, Revision $revision = NULL) {
  if ($revision) {
    drupal_set_title('Export Revision #' . $revision->getRevisionNumber());
  }
  else {
    $revision_id = $model->getLatestRevisionNumber();
    $revision = new Revision($model->getConfig(), $model->getUuid());
    $revision->load($revision_id);
    drupal_set_title('Export Revision #' . $revision->getRevisionNumber());
  }
  $form = array();
  $form['revision'] = array(
    '#type' => 'value',
    '#value' => $revision->toArray(),
  );
  $form['model'] = array(
    '#type' => 'value',
    '#value' => $model->toArray(),
  );
  $options = array(
    'json' => 'SmartDocs JSON',
    'wadl' => 'WADL'
  );
  $form['file_type'] = array(
    '#type' => 'select',
    '#title' => t('Select export file type'),
    '#options' => $options,
    '#default_value' => '',
  );
  $form['options']['submit_export'] = array(
    '#type' => 'submit',
    '#value' => t('Export'),
    '#prefix' => '<br>',
  );
  return $form;
}

/**
 * Submit Callback for smartdocs_model_export
 *
 * @param $form
 * @param $form_state
 * @param $mid
 */
function smartdocs_model_export_submit($form, &$form_state) {
  /** @var Apigee\SmartDocs\Model $model */
  $model = new Model(devconnect_default_org_config());
  Model::fromArray($model, $form_state['values']['model']);
  /** @var Apigee\SmartDocs\Revision $revision */
  $revision = new Revision($model->getConfig(), $model->getUuid());
  Revision::fromArray($revision, $form_state['values']['revision']);

  try {
    if ($form_state['input']['file_type'] == 'json') {
      $data = $revision->export(NULL);
      $filename = $model->getName() . '.json';
    }
    else {
      $data = $revision->export($form_state['input']['file_type']);
      $filename = $model->getName() . '.wadl';
    }
  } catch (Exception $e) {
    $message = t('There was an error exporting the ‘@model’ model.', array('@model' => $model->getDisplayName()));
    drupal_set_message($message, 'error');
    return;
  }
  $xexport_dir = 'public://wadls_export';
  file_prepare_directory($xexport_dir, FILE_CREATE_DIRECTORY);
  $destination = $xexport_dir . '/' . $filename;
  $my_file_obj = file_save_data($data, $destination, FILE_EXISTS_REPLACE);
  $headers = file_get_content_headers($my_file_obj);
  $headers['Content-Disposition'] = 'attachment; filename="' . $my_file_obj->filename . '"';
  file_transfer($my_file_obj->uri, $headers);
}


/**
 * Specifies Settings for a specific model
 *
 * @param $form
 * @param $form_state
 * @param $mid
 */
function smartdocs_model_settings(array $form, array &$form_state, Model $model) {
  try {
    $templateObj = new Template($model->getConfig(), $model->getUuid());
    $template = $templateObj->load('drupal-cms', 'method');
  } catch (ResponseException $e) {
    $form['error'] = array(
      '#type' => 'markup',
      '#markup' => t('Unable to load method template from model @model. This may happen if you have not imported a revision for the model.', array('@model' => $model->getDisplayName())),
      '#prefix' => '<div class="messages error">',
      '#suffix' => '</div>',
    );
    return $form;
  }

  $model_name = smartdocs_model_name($model->getName());
  drupal_set_title('‘' . $model->getDisplayName() . '’' . ' Settings');
  $form['model'] = array(
    '#type' => 'value',
    '#value' => $model->toArray(),
  );
  $def_css = 'https://smartdocs.apigee.com/3/static/css/main.css
https://smartdocs.apigee.com/3/static/css/codemirror.css
https://smartdocs.apigee.com/3/static/css/prism.css';
  $def_js = "https://smartdocs.apigee.com/3/static/js/codemirror.js
https://smartdocs.apigee.com/3/static/js/codemirror_javascript.js
https://smartdocs.apigee.com/3/static/js/codemirror_xml.js
https://smartdocs.apigee.com/3/static/js/prism.js
https://smartdocs.apigee.com/3/static/js/base64_min.js
https://smartdocs.apigee.com/3/static/js/model.js
https://smartdocs.apigee.com/3/static/js/controller.js";
  $form[$model_name . '_css'] = array(
    '#type' => 'textarea',
    '#title' => t('CSS Style Sheets'),
    '#rows' => 3,
    '#default_value' => variable_get($model_name . '_css', $def_css),
  );
  $form[$model_name . '_js'] = array(
    '#type' => 'textarea',
    '#title' => t('JS Style Sheets'),
    '#rows' => 7,
    '#default_value' => variable_get($model_name . '_js', $def_js),
  );
  $form['templates'] = array(
    '#type' => 'fieldset',
    '#title' => t('Template'),
    '#collapsed' => FALSE,
    '#collapsible' => TRUE,
  );
  $form['templates'][$model_name][$model_name . '_method_template'] = array(
    '#type' => 'textarea',
    '#title' => t('Method Template'),
    '#rows' => 10,
    '#default_value' => $template,
    '#prefix' => '<div id="' . $model_name . '-wrapper">',
    '#suffix' => '</div>',
  );
  $form['templates'][$model_name]['method_save'] = array(
    '#type' => 'submit',
    '#value' => 'Save ' . $model->getDisplayName() . ' Template',
    '#submit' => array('smartdocs_model_method_settings_save'),
    '#model' => $model_name,
    '#ajax' => array(
      'callback' => 'smartdocs_method_callback',
      'wrapper' => $model_name . '-wrapper',
      'method' => 'replace',
      'effect' => 'fade',
      'progress' => array(
        'type' => 'throbber',
      )
    ),
  );
  return system_settings_form($form);
}

/**
 * Ajax Callback for Model Template.
 *
 * @param $form
 * @param $form_state
 * @return mixed
 */
function smartdocs_method_callback($form, &$form_state) {
  $name = $form_state['clicked_button']['#model'];
  return $form['templates'][$name][$name . '_method_template'];
}

/**
 * Submit Callback for Model Template.
 *
 * @param $form
 * @param $form_state
 */
function smartdocs_model_method_settings_save($form, &$form_state) {
  try {
    /** @var Apigee\SmartDocs\Model $model */
    $model = new Model(devconnect_default_org_config());
    Model::fromArray($model, $form_state['values']['model']);
    $mid = smartdocs_model_name($model->getName());
    $html = $form_state['values'][$mid . '_method_template'];
    $templateObj = new Template($model->getConfig(), $model->getUuid());
    $templateObj->save('drupal-cms', 'method', $html, TRUE);

    drupal_set_message('Template saved.');

  } catch (Exception $e) {
    drupal_set_message('There was an error saving your template.', 'error');
  }
  $form_state['rebuild'] = TRUE;
}


/**
 * Model delete confirmation form.
 *
 * @param array $form
 * @param array $form_state
 * @param array $model
 */
function smartdocs_model_delete(array $form, array &$form_state, Model $model) {
  $form['model'] = array(
    '#type' => 'value',
    '#value' => $model,
  );
  $ques = 'Are you sure?';
  $desc = 'Deleting any model will also delete its corresponding content type and all of its data.';
  return confirm_form($form, $ques, 'admin/smartdocs/models/' . $model->getUuid(), $desc);
}

/**
 * Deletes a model and all its dependent data.
 *
 * @param array $form
 * @param array $form_state
 */
function smartdocs_model_delete_submit(array $form, array &$form_state) {
  $model = $form_state['values']['model'];
  $model_name = $model->getName();
  $model_name = smartdocs_model_name($model_name);

  try {
    $model->delete();
    if (variable_get($model_name . '_nodes', NULL)) {
      variable_del($model_name . '_nodes');
      variable_del($model_name);
    }
    if (variable_get('smartdocs_content_types', array())) {
      $models = variable_get('smartdocs_content_types', array());
      $types = array();
      foreach ($models as $display => $type) {
        if ($type != $model_name) {
          $types[$display] = $type;
        }
      }
      variable_set('smartdocs_content_types', $types);
    }
    $results = db_select('node', 'n')
      ->fields('n', array('nid'))
      ->condition('type', $model_name)
      ->execute();
    $nids = array();
    foreach ($results as $result) {
      $nids[] = $result->nid;
      db_delete('smartdata')
        ->condition('nid', $result->nid)
        ->execute();
    }
    if (count($nids) > 0) {
      node_delete_multiple($nids);
    }
    $update = db_update('node_type')
      ->fields(array('custom' => 1, 'modified' => 1, 'locked' => 0))
      ->condition('type', $model_name)
      ->execute();
    if ($update) {
      node_type_delete($model_name);
    }
    $vocabs = array('tags', 'auth_schemes', 'methods', 'revisions', 'types');
    foreach ($vocabs as $machine) {
      $term = taxonomy_vocabulary_machine_name_load($model_name . '_' . $machine);
      if ($term && $term->vid) {
        foreach (taxonomy_get_tree($term->vid) as $term) {
          taxonomy_term_delete($term->tid);
        }
        taxonomy_vocabulary_delete($term->vid);
      }
    }
    $fid = db_query("SELECT fid FROM {file_managed} WHERE filename=:name", array(':name' => $model_name . '_listing.view'))->fetchField();
    if ($fid) {
      $file = file_load($fid);
      file_delete($file);
    }
    menu_rebuild();
    drupal_flush_all_caches();
    drupal_set_message(t('%model has been deleted along with all of its associated content.', array('%model' => $model_name)), 'status');
    $form_state['redirect'] = 'admin/smartdocs/models';
  } catch (Exception $e) {
    drupal_set_message('There was an error deleting the model(s).', 'error');
  }
}

/**
 * Page callback to display list of revisions for a given model.
 *
 * @return string
 */
function smartdocs_revision_list(Model $model) {
  try {
    $revisionObj = new Apigee\SmartDocs\Revision($model->getConfig(), $model->getUuid());
    $revisions = $revisionObj->listRevisions();
  } catch (ResponseException $e) {
    return array(
      'error' => array(
        '#type' => 'markup',
        '#markup' => t('An error occurred while fetching all revisions for model @model.', array('@model' => $model->getDisplayName())),
        '#prefix' => '<div class="message error">',
        '#suffix' => '</div>',
      )
    );
  }

  drupal_add_css(drupal_get_path('module', 'views') . '/css/views-admin.ctools.css');

  $headers = array(
    t('Revision'),
    t('Base URL'),
    t('Created'),
    t('Operations')
  );

  usort($revisions, function (Revision $a, Revision $b) {
    $ar = $a->getRevisionNumber();
    $br = $b->getRevisionNumber();
    if ($ar == $br) {
      return 0;
    }
    return ($ar > $br) ? -1 : 1;
  });
  $rows = array();

  $operations = array(
    '' => t('Revision Details'),
    '/edit' => t('Edit Revision'),
    '/auth' => t('Auth Settings'),
  );

  $is_latest_revision = TRUE;
  /** @var Apigee\SmartDocs\Revision $revision */
  foreach ($revisions as $revision) {
    $links = array();
    foreach ($operations as $key => $op) {
      $links[$key . '-' . $revision->getUuid()] = array(
        'title' => $op,
        'href' => 'admin/smartdocs/models/' . $model->getUuid() . '/revisions/' . $revision->getRevisionNumber() . $key,
        'attributes' => array(
          'title' => $op,
          'class' => array(
            $key . '-' . $revision->getUuid(),
            'views-ajax-link'
          ),
        ),
      );
    }
    $caption = 'Revision #@rev';
    if ($is_latest_revision) {
      $caption .= ' (latest)';
      $is_latest_revision = FALSE;
    }

    $rows[] = array(
      l(t($caption, array('@rev' => $revision->getRevisionNumber())), 'admin/smartdocs/models/' . $model->getUuid() . '/revisions/' . $revision->getRevisionNumber()),
      $revision->getBaseUrl(),
      format_date($revision->getCreatedTime(), 'custom', 'j F Y'),
      theme('links__ctools_dropbutton', array('links' => $links))
    );
  }

  return theme('table', array('header' => $headers, 'rows' => $rows));
}


/**
 * Page Callback for admin/smartdocs/model/%name
 *
 * @param $form
 * @param $form_state
 * @param $model
 */
function smartdocs_add_revision(array $form, array $form_state, Model $model) {
  drupal_set_title(t('Add API Revision to ‘@model’', array('@model' => $model->getDisplayName())));
  $form['model'] = array(
    '#type' => 'value',
    '#value' => $model->toArray(),
  );
  $form['display_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Display Name'),
    '#size' => 60,
    '#maxlength' => 128,
    '#required' => TRUE,
    '#attributes' => array(
      'placeholder' => t('New Revision'),
    )
  );
  $form['version'] = array(
    '#type' => 'textfield',
    '#title' => t('Version ID'),
    '#size' => 60,
    '#maxlength' => 128,
    '#required' => TRUE,
    '#attributes' => array(
      'placeholder' => t('v1'),
    )
  );
  $form['description'] = array(
    '#type' => 'textarea',
    '#title' => t('Description'),
    '#rows' => 5,
    '#required' => TRUE,
  );
  $form['base_url'] = array(
    '#type' => 'textfield',
    '#title' => t('Base URL'),
    '#size' => 60,
    '#maxlength' => 128,
    '#required' => TRUE,
  );
  // TODO: add more fields
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Add API Revision'),
  );
  return $form;
}

/**
 * Validation Callback for smartdocs_add_revision
 *
 * @param $form
 * @param $form_state
 * @param $mid
 * @return mixed
 */
function smartdocs_add_revision_validate($form, &$form_state) {
  if (!(filter_var($form_state['values']['base_url'], FILTER_VALIDATE_URL))) {
    form_set_error('base_url', 'Base URL needs to be a valid URL.');
  }
}

/**
 * Submit Callback for smartdocs_add_revision
 *
 * @param $form
 * @param $form_state
 * @param $mid
 * @return mixed
 */
function smartdocs_add_revision_submit($form, &$form_state) {
  $model = new Model(devconnect_default_org_config());
  Model::fromArray($model, $form_state['values']['model']);
  $revision = new Revision(devconnect_default_org_config(), $model->getUuid());
  $revision->setBaseUrl($form_state['values']['base_url']);
  $revision->setDescription($form_state['values']['description']);
  $revision->setReleaseVersion($form_state['values']['version']);
  $revision->setDisplayName($form_state['values']['display_name']);
  try {
    $revision->save();
    cache_clear_all('model:' . $model->getUuid(), 'cache_smartdocs');
    module_invoke_all('smartdocs_model_update', $model->getUuid());
    drupal_set_message(t('Revision was created successfully'));
    $form_state['redirect'] = 'admin/smartdocs/models/' . $model->getUuid() . '/revisions/' . $revision->getRevisionNumber();
  } catch (Exception $e) {
    drupal_set_message(t('There was a problem creating the new revision. Please check the logs.'), 'error');
  }
}

/**
 * Page Callback for admin/smartdocs/model/%smartdocs_model/revisions/%
 *
 * @param $form
 * @param $form_state
 * @param $mid
 * @param $rev
 * @return mixed
 */
function smartdocs_revision_detail(array $form, array &$form_state, Model $model, Revision $revision) {

  $form['#attached'] = array(
    'css' => array(
      drupal_get_path('module', 'views') . '/css/views-admin.ctools.css',
      drupal_get_path('module', 'smartdocs') . '/css/smartdocs.css',
    ),
    'js' => array(
      'misc/tableselect.js',
    ),
    'library' => array(
      array('system', 'drupal.collapse'),
    ),
  );
  $rev = $revision->getRevisionNumber();

  // TODO: find a way to move these page-specific items out of a form constructor
  drupal_set_title(t('‘@model’ API Revision #@rev', array('@model' => $model->getDisplayName(), '@rev' => $rev)));
  $crumb = drupal_get_breadcrumb();
  $crumb[] = '#' . $rev;
  drupal_set_breadcrumb($crumb);

  $nids = db_select('smartdata', 's')
    ->condition('model', $model->getName())
    ->fields('s', array('nid'))
    ->execute();
  $nodes = array();
  foreach ($nids as $nid) {
    $nodes[$nid->nid] = $model;
  }

  if (variable_get('smartdocs_publish_on_render', 1)) {
    $caption = t('Render and publish nodes');
  }
  else {
    $caption = t('Render nodes');
  }
  $operations_options['render_nodes'] = $caption;
  if (!empty($nodes)) {
    $operations_options['publish_nodes'] = t('Publish nodes');
    $operations_options['unpublish_nodes'] = t('Unpublish nodes');
  }

  $operations = array(
    'resource/add/' . $rev => 'Add Resource',
    'revisions/' . $rev . '/auth' => 'Auth Settings',
    'export/' . $rev => 'Export',
    'revisions' => 'View All Revisions',
    'revisions/' . $rev . '/edit' => 'Edit API Revision',
  );
  $links = array();
  foreach ($operations as $key => $op) {
    $links[$key] = array(
      'title' => $op,
      'href' => 'admin/smartdocs/models/' . $model->getUuid() . '/' . $key,
      'attributes' => array(
        'title' => $op,
        'class' => array('views-ajax-link'),
      ),
    );
  }
  $form['revision_tools'] = array(
    '#markup' => theme('links__ctools_dropbutton', array('links' => $links)),
    '#suffix' => '<br><br>'
  );

  $form['options'] = array(
    '#type' => 'fieldset',
    '#title' => t('Update options'),
    '#attributes' => array('class' => array('container-inline')),
    '#access' => array('administer smartdocs content'),
  );
  $form['options']['operation'] = array(
    '#type' => 'select',
    '#title' => t('Operation'),
    '#title_display' => 'invisible',
    '#options' => $operations_options,
  );
  $form['options']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Update'),
  );

  $form['options']['render_desc'] = array(
    '#type' => 'container',
    '#states' => array(
      'visible' => array(
        'select[name="operation"]' => array('value' => 'render_nodes'),
      )
    ),
    '#attributes' => array('style' => 'margin-left:1em'),
    'render_markup' => array(
      '#type' => 'markup',
      '#markup' => t('Rendering nodes associates your SmartDocs content with Drupal nodes, thus making it available for Drupal to display.'),
    ),
  );

  if (!empty($nodes)) {
    $form['options']['publish_desc'] = array(
      '#type' => 'container',
      '#states' => array(
        'visible' => array(
          'select[name="operation"]' => array('value' => 'publish_nodes'),
        )
      ),
      '#attributes' => array('style' => 'margin-left:1em'),
      'publish_markup' => array(
        '#type' => 'markup',
        '#markup' => t('Publishing Drupal nodes makes their content available for viewing.'),
      ),
    );


    $form['options']['unpublish_desc'] = array(
      '#type' => 'container',
      '#states' => array(
        'visible' => array(
          'select[name="operation"]' => array('value' => 'unpublish_nodes'),
        )
      ),
      '#attributes' => array('style' => 'margin-left:1em'),
      'unpublish_markup' => array(
        '#type' => 'markup',
        '#markup' => t('Unpublishing Drupal nodes makes their content viewable only by administrators.'),
      ),
    );
  }

  $header = array(
    'tax' => t('Taxonomy'),
    'name' => t('Name'),
    'description' => t('Description'),
    'method' => t('Method'),
    'auth' => t('Authentication'),
    'node' => t('Node Association'),
    'status' => t('Published'),
    'synced' => t('Synced'),
    'operations' => t('Operations'),
  );
  if ($rev) {
    $entity['displayName'] = $model->getDisplayName();
    $entity['name'] = $model->getName();

    $resources = $revision->getResources();
    if (empty($resources)) {
      $form['resources'] = array(
        '#markup' => t('There is currently no data in the model.')
      );
      return $form;
    }
    $form['resources'] = array();
    /** @var Apigee\SmartDocs\Resource $resource */
    foreach ($resources as $resource) {
      $resourceId = $resource->getUuid();
      $links = array();
      $links['add-' . $resourceId] = array(
        'title' => t('Add method'),
        'href' => 'admin/smartdocs/models/' . $model->getUuid() . '/method/add/' . $rev . '/' . $resourceId,
        'attributes' => array(
          'title' => t('Add method'),
          'class' => array(
            'add-' . $resourceId,
            'views-ajax-link'
          ),
        ),
      );
      $links['edit-' . $resourceId] = array(
        'title' => t('Edit resource'),
        'href' => 'admin/smartdocs/models/' . $model->getUuid() . '/resource/edit/' . $rev . '/' . $resourceId,
        'attributes' => array(
          'title' => t('Edit resource'),
          'class' => array(
            'edit-' . $resourceId,
            'views-ajax-link'
          ),
        ),
      );

      $resource_set = array(
        'ops' => array('#markup' => theme('links__ctools_dropbutton', array('links' => $links))),
        'methods' => array('#tree' => TRUE),
        '#title' => $resource->getBaseUrl() . $resource->getPath(),
      );

      $has_tax = FALSE;
      foreach ($resource->getMethods() as $method) {
        /** @var Apigee\SmartDocs\Method $method */
        $tags = $method->getTags();
        if (!empty($tags)) {
          $has_tax = TRUE;
        }

        $query = db_select('smartdata', 's');
        $query->innerJoin('node', 'n', 's.nid = n.nid');
        $row = $query->fields('s', array('nid', 'synced'))
          ->fields('n', array('status'))
          ->condition('s.method', $method->getUuid())
          ->execute()
          ->fetchAssoc();
        $has_node = !empty($row);
        $synced = $published = FALSE;
        if ($has_node) {
          $link = l(t('NID: %nid', array('%nid' => $row['nid'])), 'node/' . $row['nid'], array(
            'html' => TRUE,
            'attributes' => array('target' => '_blank'),
          ));
          $published = (bool) $row['status'];
          $synced = (bool) $row['synced'];
        }

        $ops = array();
        $ops['edit-method-' . $method->getUuid()] = array(
          'title' => t('Edit method'),
          'href' => 'admin/smartdocs/models/' . $model->getUuid() . '/method/edit/' . $rev . '/' . $resource->getUuid() . '/' . $method->getUuid(),
          'attributes' => array(
            'title' => t('Edit method'),
            'class' => array(
              'edit-method-' . $method->getUuid(),
              'views-ajax-link'
            ),
          ),
        );
        if ($has_node) {
          $op_title = t($published ? 'Unpublish node' : 'Publish node');

          $ops['toggle-status-method-' . $method->getUuid()] = array(
            'title' => $op_title,
            'href' => 'admin/smartdocs/models/' . $model->getUuid() . '/method/toggle-status/' . $rev . '/' . $resource->getUuid() . '/' . $method->getUuid(),
            'attributes' => array(
              'title' => $op_title,
              'class' => array(
                'toggle-status-method-' . $method->getUuid(),
                'views-ajax-link'
              ),
            ),
          );
        }
        $ops['delete-method-' . $method->getUuid()] = array(
          'title' => t('Delete method'),
          'href' => 'admin/smartdocs/models/' . $model->getUuid() . '/method/delete/' . $rev . '/' . $resource->getUuid() . '/' . $method->getUuid(),
          'attributes' => array(
            'title' => t('Delete method'),
            'class' => array(
              'delete-method-' . $method->getUuid(),
              'views-ajax-link'
            ),
          ),
        );

        $resource_set['methods'][$method->getUuid()] = array(
          '#type' => 'checkbox',
          '#return_value' => $method->getUuid(),
          '#data' => array(
            'tax' => (!empty($tags) ? implode(', ', $tags) : ''),
            'name' => check_plain($method->getDisplayName()),
            'description' => filter_xss($method->getDescription()),
            'method' => $method->getVerb(),
            'auth' => implode(', ', $method->getAuthSchemes()),
            'node' => ($has_node ? $link : '<span style="color:red;">No</span>'),
            'status' => ($published ? '<span style="color:green;">Yes</span>' : '<span style="color:red;">No</span>'),
            'synced' => ($synced ? '<span style="color:green;">Yes</span>' : '<span style="color:red;">No</span>'),
            'operations' => theme('links__ctools_dropbutton', array('links' => $ops)),
          )
        );
      }
      if (count($resource->getMethods()) > 0) {
        $header_copy = $header;
        if (!$has_tax) {
          unset($header_copy['tax']);
          foreach ($resource_set['methods'] as &$method_row) {
            if (is_array($method_row) && array_key_exists('#data', $method_row) && array_key_exists('tax', $method_row['#data'])) {
              unset($method_row['#data']['tax']);
            }
          }
        }
        $resource_set['#headers'] = $header_copy;
      }
      $form['resources'][] = $resource_set;
    }
  }

  $form['revision'] = array(
    '#type' => 'value',
    '#value' => $revision->toArray(),
  );
  $form['model'] = array(
    '#type' => 'value',
    '#value' => $model->toArray(),
  );

  $form['#theme'] = 'smartdocs_resources_table';
  return $form;
}

/**
 * Resource, Method table submit
 *
 * @param $form
 * @param $form_state
 */
function smartdocs_revision_detail_submit($form, &$form_state) {
  switch ($form_state['values']['operation']) {
    case 'render_nodes':
      $model = new Model(devconnect_default_org_config());
      Model::fromArray($model, $form_state['values']['model']);
      $revision = new Revision($model->getConfig(), $model->getUuid());
      Revision::fromArray($revision, $form_state['values']['revision']);
      $selected = $form_state['values']['methods'];
      $options = array(
        'publish' => variable_get('smartdocs_publish_on_render', 1),
        'path' => variable_get('smartdocs_path_gen_type', 'path')
      );
      module_load_include('inc', 'smartdocs', 'batch/smartdocs.render');
      batch_set(smartdocs_render($model, $revision, $selected, $options));
      cache_clear_all('model:' . $model->getUuid(), 'cache_smartdocs');
      break;
    case 'publish_nodes':
      foreach ($form_state['input']['methods'] as $uuid) {
        _smartdocs_set_node_status($uuid, 1);
      }
      break;
    case 'unpublish_nodes':
      foreach ($form_state['input']['methods'] as $uuid) {
        _smartdocs_set_node_status($uuid, 0);
      }
      break;
  }
}

/**
 * Attempts to set a status (zero or one) on a SmartDocs node.
 *
 * @param string $method_uuid
 * @param int $status
 * @throws Exception (may be thrown by node_save)
 */
function _smartdocs_set_node_status($method_uuid, $status) {
  $nid = db_select('smartdata', 's')
    ->condition('method', $method_uuid)
    ->fields('s', array('nid'))
    ->execute()
    ->fetchField();
  $node = FALSE;
  if ($nid) {
    $node = node_load($nid);
  }
  if ($node) {
    $node->status = intval($status);
    node_save($node);
  }
  else {
    // Clean up any bad data in smartdata. That is to say, if smartdata
    // has a reference to a node that doesn't exist, delete that reference.
    if (isset($nid)) {
      db_delete('smartdata')->condition('nid', $nid)->execute();
    }
    drupal_set_message(t('The method you selected doesn’t have an associated node.  Please render it first.'), 'error', FALSE);
  }
}

/**
 * Edit Resource Form
 *
 * @param array $form
 * @param array $form_state
 * @param Apigee\SmartDocs\Model $model
 * @param Apigee\SmartDocs\Revision $revision
 * @param Apigee\SmartDocs\Resource|null $resource
 * @return array
 */
function smartdocs_resource_edit(array $form, array &$form_state, Model $model, Revision $revision, Resource $resource = NULL) {

  // TODO: find a way to move these page-specific items out of a form constructor
  if (isset($resource)) {
    $title = 'Edit resource from ‘@model’';
  }
  else {
    $title = 'Add resource to ‘@model’';
  }
  drupal_set_title(t($title, array('@model' => $model->getDisplayName())));
  $breadcrumb = array();
  $breadcrumb[] = l('Home', '<front>');
  $breadcrumb[] = l('Administration', 'admin');
  $breadcrumb[] = l('SmartDocs', 'admin/smartdocs');
  $breadcrumb[] = l('Models', 'admin/smartdocs/models');
  $breadcrumb[] = l($model->getDisplayName(), 'admin/smartdocs/models/' . $model->getUuid());
  $breadcrumb[] = l('API Revisions', 'admin/smartdocs/models/' . $model->getUuid() . '/revisions/');
  $breadcrumb[] = l('#' . $revision->getRevisionNumber(), 'admin/smartdocs/models/' . $model->getUuid() . '/revisions/' . $revision->getRevisionNumber());
  drupal_set_breadcrumb($breadcrumb);

  $version = variable_get('local_asset_version', '3');
  $my_path = drupal_get_path('module', 'smartdocs');
  $form['#attached'] = array(
    'js' => array(
      $my_path . '/local/js/v' . $version . '/codemirror.js',
      $my_path . '/local/js/v' . $version . '/codemirror_javascript.js',
      array(
        'data' => array('smartdocs' => array('parameters' => 'parameters')),
        'type' => 'setting'
      ),
      $my_path . '/js/smartdocs.js',
    ),
    'css' => array(
      $my_path . '/local/css/v' . $version . '/codemirror.css',
    ),
  );

  $form['model'] = array(
    '#type' => 'value',
    '#value' => $model->toArray(),
  );
  $form['revision'] = array(
    '#type' => 'value',
    '#value' => $revision->toArray(),
  );
  $form['resource'] = array(
    '#type' => 'value',
    '#value' => (is_object($resource) ? $resource->toArray() : NULL),
  );
  $form['display_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Display Name'),
    '#default_value' => isset($resource) ? $resource->getDisplayName() : NULL,
    '#size' => 60,
    '#maxlength' => 128,
  );
  $form['path'] = array(
    '#type' => 'textfield',
    '#title' => t('Path'),
    '#default_value' => isset($resource) ? $resource->getPath() : NULL,
    '#description' => t('Provide a path with dynamic values, ex: %e', array('%e' => '/consoles/{name}')),
    '#size' => 60,
    '#maxlength' => 128,
  );
  $form['description'] = array(
    '#type' => 'textarea',
    '#default_value' => isset($resource) ? $resource->getDescription() : NULL,
    '#title' => t('Description'),
    '#rows' => 5,
  );

  $form['parameters'] = array(
    '#type' => 'textarea',
    '#title' => t('Parameters'),
    '#default_value' => isset($resource) && count($resource->getParameters()) > 0 ? smartdocs_format_json($resource->getParameters()) : NULL,
  );

  $example = smartdocs_method_examples('parameters');
  if (!empty($example)) {
    $form['examples'] = array(
      '#type' => 'fieldset',
      '#title' => t('Examples'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );
    $form['examples']['ex'] = array(
      '#markup' => $example,
    );
  }
  $form['submit'] = array(
    '#type' => 'submit',
    '#submit' => array('smartdocs_resource_edit_submit'),
    '#value' => t((isset($resource) ? 'Save' : 'Add') . ' Resource'),
  );
  return $form;
}


/**
 * Validator for smartdocs_resource_edit() form.
 */
function smartdocs_resource_edit_validate($form, &$form_state) {
  if (!empty($form_state['values']['parameters'])) {
    if (smartdocs_json_validate($form_state['values']['parameters'])) {
      form_set_error('parameters', t('Please specify proper JSON values for the parameters.'));
    }
  }
}

/**
 * Submit callback for Edit Resource form
 *
 * @param $form
 * @param $form_state
 */
function smartdocs_resource_edit_submit($form, &$form_state) {
  $parameters = drupal_json_decode($form_state['values']['parameters']);
  if (empty($parameters)) {
    $parameters = array();
  }

  $model = new Model(devconnect_default_org_config());
  Model::fromArray($model, $form_state['values']['model']);
  $revision = new Revision($model->getConfig(), $model->getUuid());
  Revision::fromArray($revision, $form_state['values']['revision']);
  $resource = new Resource($model->getConfig(), $model->getUuid(), $revision->getUuid());
  if (empty($form_state['values']['resource'])) {
    $update = FALSE;
  }
  else {
    $update = TRUE;
    Resource::fromArray($resource, $form_state['values']['resource']);
  }
  $resource->setName(preg_replace('/[^A-Za-z0-9]/', '', $form_state['values']['display_name']));
  $resource->setDisplayName($form_state['values']['display_name']);
  $resource->setDescription($form_state['values']['description']);
  $resource->setPath($form_state['values']['path']);
  $resource->setParameters($parameters);

  try {
    $resource->save($update);
    drupal_set_message(t('Resource has been ' . ($update ? 'updated' : 'created') . ' successfully.'));
    cache_clear_all('model:' . $model->getUuid(), 'cache_smartdocs');
    module_invoke_all('smartdocs_model_update', $model->getUuid());
    $form_state['redirect'] = 'admin/smartdocs/models/' . $model->getUuid() . '/revisions/' . $revision->getRevisionNumber();
  } catch (Exception $e) {
    drupal_set_message(t('Resource failed to save, please consult the logs.'), 'error');
  }
}

/**
 * Adds a method to a given resource
 *
 * @param array $form
 * @param array $form_state
 * @param Apigee\SmartDocs\Model $model
 * @param Apigee\SmartDocs\Revision $revision
 * @param Apigee\SmartDocs\Resource $resource
 * @param Apigee\SmartDocs\Method|null $method
 * @return mixed
 */
function smartdocs_method_edit(array $form, array &$form_state, Model $model, Revision $revision, Resource $resource, Method $method = NULL) {
  $version = variable_get('local_asset_version', '3');

  $my_path = drupal_get_path('module', 'smartdocs');
  $form['#attached'] = array(
    'css' => array(
      $my_path . '/local/css/v' . $version . '/codemirror.css',
    ),
    'js' => array(
      $my_path . '/local/js/v' . $version . '/codemirror.js',
      $my_path . '/local/js/v' . $version . '/codemirror_javascript.js',
      $my_path . '/js/smartdocs.js',
    ),
  );

  // TODO: find a way to move these page-specific items out of a form constructor
  if (isset($method)) {
    drupal_set_title(t('Edit method ‘@method’ of ‘@model’ model', array(
      '@method' => $method->getDisplayName(),
      '@model' => $model->getDisplayName()
    )));
  }
  else {
    drupal_set_title(t('Add method to ‘@model’ model', array('@model' => $model->getDisplayName())));
  }
  $breadcrumb = array();
  $breadcrumb[] = l(t('Home'), '<front>');
  $breadcrumb[] = l(t('Administration'), 'admin');
  $breadcrumb[] = l('SmartDocs', 'admin/smartdocs');
  $breadcrumb[] = l(t('Models'), 'admin/smartdocs/models');
  $breadcrumb[] = l($model->getDisplayName(), 'admin/smartdocs/models/' . $model->getUuid());
  $breadcrumb[] = l(t('API Revisions'), 'admin/smartdocs/models/' . $model->getUuid() . '/revisions/');
  $breadcrumb[] = l('#' . $revision->getRevisionNumber(), 'admin/smartdocs/models/' . $model->getUuid() . '/revisions/' . $revision->getRevisionNumber());
  drupal_set_breadcrumb($breadcrumb);

  if (isset($method)) {
    // We need to evaluate if there is node information.
    $nid = db_select('smartdata', 's')
      ->fields('s', array('nid'))
      ->condition('method', $method->getUuid())
      ->execute()
      ->fetchField();

    if (isset($nid)) {
      $form['node'] = array(
        '#type' => 'value',
        '#value' => node_load($nid)
      );
    }
  }
  $form['model'] = array(
    '#type' => 'value',
    '#value' => $model->toArray(),
  );
  $form['revision'] = array(
    '#type' => 'value',
    '#value' => $revision->toArray(),
  );
  $form['resource'] = array(
    '#type' => 'value',
    '#value' => $resource->toArray(),
  );
  $form['method'] = array(
    '#type' => 'value',
    '#value' => (is_object($method) ? $method->toArray() : NULL),
  );

  $form['display_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Display Name'),
    '#size' => 60,
    '#maxlength' => 128,
    '#default_value' => isset($method) ? $method->getDisplayName() : NULL,
  );
  $form['description'] = array(
    '#type' => 'text_format',
    '#title' => t('Description'),
    '#rows' => 5,
    '#default_value' => isset($method) ? $method->getDescription() : NULL,
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
    '#default_value' => isset($method) ? $method->getVerb() : NULL,
  );
  $schemes = array();
  if (isset($method)) {
    foreach ($method->getAuthSchemes() as $scheme) {
      $schemes[] = $scheme;
    }
    if (empty($schemes)) {
      $schemes[] = 'NOAUTH';
    }
  }

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
    '#default_value' => $schemes,
    '#required' => TRUE,
  );

  $content_type = 'application/json';
  if (isset($method)) {
    $body = $method->getBody();
    if (!empty($body['contentType'])) {
      $content_type = $body['contentType'];
    }
  }
  $form['contentType'] = array(
    '#type' => 'select',
    '#title' => t('Content Type'),
    '#options' => array(
      'application/jsonp' => 'application/jsonp',
      'application/json' => 'application/json',
      'application/json, text/xml' => 'application/json, text/xml',
      'text/xml' => 'text/xml'
    ),
    '#default_value' => $content_type,
  );
  $types = array(
    'parameters' => t('Parameters'),
    'body-doc' => t('Request Body Documentation'),
    'body-example' => t('Request Body Example'),
    'tags' => t('Tags'),
  );

  foreach ($types as $type => $display) {
    $form['field_' . $type] = array(
      '#type' => 'fieldset',
      '#title' => $display,
      '#prefix' => '<div id="' . $type . '">',
      '#suffix' => '</div>',
    );
    switch ($type) {
      case 'body-doc':
        $body = ($method ? $method->getBody() : array());
        $form['field_body-doc']['body-doc'] = array(
          '#type' => 'text_format',
          '#title' => t('Description'),
          '#rows' => 5,
          '#default_value' => (array_key_exists('doc', $body)) ? $body['doc'] : '',
        );
        break;
      case 'body-example':
        $body = ($method ? $method->getBody() : array());
        $form['field_body-example']['body-example'] = array(
          '#type' => 'textarea',
          '#title' => $display,
          '#default_value' => (array_key_exists('sample', $body)) ? $body['sample'] : '',
          '#attached' => array(
            'js' => array(
              'data' => array('smartdocs' => array('body-example' => 'body-example')),
              'type' => 'setting',
            )
          )
        );
        break;
      case 'parameters':
        if ($method) {
          $method_params = $method->getParameters();
          foreach ($resource->getParameters() as $key => $resource_param) {
            if (array_key_exists($key, $method_params)) {
              unset($method_params[$key]);
            }
          }
          // Re-number the params array.
          $method_params = array_values($method_params);
        }
        else {
          $method_params = NULL;
        }
        $form['field_parameters']['parameters'] = array(
          '#type' => 'textarea',
          '#title' => $display,
          '#default_value' => empty($method_params) ? '' : smartdocs_format_json($method_params),
          '#attached' => array(
            'js' => array(
              'data' => array('smartdocs' => array('parameters' => 'parameters')),
              'type' => 'setting',
            )
          )
        );
        break;
      case 'tags':
        $tags = $method ? $method->getTags() : NULL;
        $form['field_tags']['tags'] = array(
          '#type' => 'textarea',
          '#title' => $display,
          '#default_value' => empty($tags) ? '' : smartdocs_format_json($tags),
          '#attached' => array(
            'js' => array(
              'data' => array('smartdocs' => array('tags' => 'tags')),
              'type' => 'setting',
            )
          )
        );
    }
    $example = smartdocs_method_examples($type);
    if (!empty($example)) {
      $form['field_' . $type]['examples'] = array(
        '#type' => 'fieldset',
        '#title' => t('Examples'),
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
      );
      $form['field_' . $type]['examples']['ex'] = array(
        '#markup' => $example,
      );
    }
  }
  $form['submit'] = array(
    '#type' => 'submit',
    '#submit' => array('smartdocs_method_edit_submit'),
    '#value' => t((isset($method) ? 'Save' : 'Add') . ' Method'),
  );

  return $form;
}


/**
 * Validator for smartdocs_method_edit() form.
 */
function smartdocs_method_edit_validate($form, &$form_state) {
  if (!empty($form_state['values']['parameters'])) {
    if (smartdocs_json_validate($form_state['values']['parameters'])) {
      form_set_error('parameters', t('Please specify proper JSON values for the parameters.'));
    }
  }
  if (!empty($form_state['values']['parameterGroups'])) {
    if (smartdocs_json_validate($form_state['values']['parameterGroups'])) {
      form_set_error('parameterGroups', t('Please specify proper JSON values for the parameter groups.'));
    }
  }
  if (!empty($form_state['values']['tags'])) {
    if (smartdocs_json_validate($form_state['values']['tags'])) {
      form_set_error('tags', t('Please specify proper JSON values for the tags.'));
    }
  }
}

/**
 * Submit form handler for smartdocs_method_edit().
 *
 * @param $form
 * @param $form_state
 */
function smartdocs_method_edit_submit($form, &$form_state) {

  $schemes = array();
  foreach ($form_state['values']['authschemes'] as $key => $auth) {
    if (is_string($auth)) {
      $schemes[] = $key;
    }
  }

  $parameters = drupal_json_decode($form_state['values']['parameters']);
  if (empty($parameters)) {
    $parameters = array();
  }
  $tags = drupal_json_decode($form_state['values']['tags']);
  if (empty($tags)) {
    $tags = array();
  }

  $model = new Model(devconnect_default_org_config());
  Model::fromArray($model, $form_state['values']['model']);
  $revision = new Revision($model->getConfig(), $model->getUuid());
  Revision::fromArray($revision, $form_state['values']['revision']);
  $resource = new Resource($model->getConfig(), $model->getUuid(), $revision->getUuid());
  Resource::fromArray($resource, $form_state['values']['resource']);
  $method = new Method($model->getConfig(), $model->getUuid(), $revision->getUuid(), $resource->getUuid());
  if (empty($form_state['values']['method'])) {
    $update = FALSE;
  }
  else {
    Method::fromArray($method, $form_state['values']['method']);
    $update = TRUE;
  }

  $method->setName(preg_replace('/[^A-Za-z0-9]/', '', $form_state['values']['display_name']));
  $method->setDisplayName($form_state['values']['display_name']);
  $method->setDescription(trim($form_state['values']['description']['value']));
  $method->setVerb($form_state['values']['verb']);
  $method->setParameters($parameters);
  $method->setAuthSchemes($schemes);
  $method->setTags($tags);

  $body = array();
  if (!empty($form_state['values']['contentType'])) {
    $body['contentType'] = $form_state['values']['contentType'];
  }
  if (!empty($form_state['values']['body-example'])) {
    $body['sample'] = $form_state['values']['body-example'];
  }
  if (!empty($form_state['values']['body-doc']['value'])) {
    $body['doc'] = trim($form_state['values']['body-doc']['value']);
  }
  $method->setBody($body);

  if (array_key_exists('parameterGroups', $form_state['values'])) {
    $param_groups = $form_state['values']['parameterGroups'];
    if (!empty($param_groups)) {
      $param_groups = drupal_json_decode($param_groups);
    }
    if (empty($param_groups)) {
      $param_groups = array();
    }
    $method->setParameterGroups($param_groups);
  }

  try {
    $method->save($update);
    cache_clear_all('model:' . $model->getUuid(), 'cache_smartdocs');
    module_invoke_all('smartdocs_model_update', $model->getUuid());
    $message_set = FALSE;

    if ($update) {
      $node = $form_state['values']['node'];
      if (is_object($node) && $node->nid) {
        $context = array();
        smartdocs_render_node($model, $revision, $resource, $method, $node->status, $context);

        drupal_set_message('The method ‘' . $method->getDisplayName() . '’ has been updated along with the associated node.', 'status');
        $form_state['redirect'] = drupal_get_path_alias('node/' . $node->nid);
        $message_set = TRUE;
      }
    }
    if (!$message_set) {
      drupal_set_message('Method ' . ($update ? 'updated' : 'created') . ' successfully', 'status');
      $form_state['redirect'] = 'admin/smartdocs/models/' . $model->getUuid() . '/revisions/' . $revision->getRevisionNumber();
    }
  } catch (Exception $e) {
    drupal_set_message('Method ' . ($update ? 'update' : 'creation') . ' failed.', 'error');
  }
}

/**
 * Page callback to publish unpublished method nodes, and vice versa.
 *
 * @param Model $model
 * @param Revision $revision
 * @param Resource $resource
 * @param Method $method
 * @throws Exception
 */
function smartdocs_toggle_node_status(Model $model, Revision $revision, Resource $resource, Method $method) {
  $nid = _smartdocs_get_method_node($method);
  if ($nid) {
    $node = node_load($nid);
    if ($node) {
      $node->status = 1 - $node->status;
      node_save($node);
      drupal_set_message(t('The node associated with the method ‘@method’ has been @action.', array('@method' => $method->getDisplayName(), '@action' => $node->status ? t('published') : t('unpublished'))));
    }
    else {
      drupal_set_message(t('The node associated with method ‘@method’ could not be found.', array('@method' => $method->getDisplayName())), 'error');
    }
  }
  else {
    drupal_set_message(t('No node is associated with method ‘@method’.', array('@method' => $method->getDisplayName())), 'error');
  }
  drupal_goto('admin/smartdocs/models/' . $model->getUuid() . '/revisions/' . $revision->getRevisionNumber());
}

/**
 * Form constructor confirming deletion of a method.
 *
 * @param array $form
 * @param array $form_state
 * @param Model $model
 * @param Revision $revision
 * @param Resource $resource
 * @param Method $method
 * @return array
 */
function smartdocs_method_delete(array $form, array &$form_state, Model $model, Revision $revision, Resource $resource, Method $method) {
  // TODO: find a way to move these page-specific items out of a form constructor
  drupal_set_title(t('About to delete method ‘@method’ of ‘@model’ model', array(
    '@method' => $method->getDisplayName(),
    '@model' => $model->getDisplayName()
  )));

  $breadcrumb = array();
  $breadcrumb[] = l(t('Home'), '<front>');
  $breadcrumb[] = l(t('Administration'), 'admin');
  $breadcrumb[] = l('SmartDocs', 'admin/smartdocs');
  $breadcrumb[] = l(t('Models'), 'admin/smartdocs/models');
  $breadcrumb[] = l($model->getDisplayName(), 'admin/smartdocs/models/' . $model->getUuid());
  $breadcrumb[] = l(t('API Revisions'), 'admin/smartdocs/models/' . $model->getUuid() . '/revisions/');
  $breadcrumb[] = l('#' . $revision->getRevisionNumber(), 'admin/smartdocs/models/' . $model->getUuid() . '/revisions/' . $revision->getRevisionNumber());
  drupal_set_breadcrumb($breadcrumb);

  $form['revision_url'] = array(
    '#type' => 'value',
    '#value' => 'admin/smartdocs/models/' . $model->getUuid() . '/revisions/' . $revision->getRevisionNumber()
  );
  $form['model_uuid'] = array(
    '#type' => 'value',
    '#value' => $model->getUuid(),
  );
  $form['revision_uuid'] = array(
    '#type' => 'value',
    '#value' => $revision->getUuid(),
  );
  $form['resource_uuid'] = array(
    '#type' => 'value',
    '#value' => $resource->getUuid(),
  );
  $form['method'] = array(
    '#type' => 'value',
    '#value' => $method->toArray(),
  );
  $form['nid'] = array(
    '#type' => 'value',
    '#value' => NULL
  );
  $form['warning'] = array(
    '#type' => 'markup',
    '#markup' => '<p><strong>' . t('Warning:') . '</strong> ' . t('You are about to remove the method ‘@method’ from the Modeling API.', array('@method' => $method->getDisplayName())) . '</p>'
  );
  $nid = _smartdocs_get_method_node($method);
  if ($nid) {
    $status = db_select('node', 'n')
      ->fields('n', array('status'))
      ->condition('nid', $nid)
      ->execute()
      ->fetchField();
    if ($status !== FALSE) {
      $form['nid']['#value'] = $nid;
      $options['delete'] = t('Delete associated Drupal node');
      if ($status) {
        $options['unpublish'] = t('Keep associated Drupal node, but unpublish it and unlink it from the Modeling API');
        $options['keep'] = t('Keep associated Drupal node in a published state, but unlink it from the Modeling API');
      }
      else {
        $options['keep'] = t('Keep associated Drupal node, but unlink it from the Modeling API');
      }
      $form['node_action'] = array(
        '#type' => 'radios',
        '#title' => t('What should be done with the Drupal node associated with this method?'),
        '#options' => $options,
        '#default_value' => 'delete',
        '#suffix' => '<p><strong>' . t('Note:') . '</strong> ' . t('Unlinking a node from the Modeling API will change the appearance of the node. In particular, users will no longer be able to test API calls from the node’s page.') . '</p>',
      );
    }
  }
  $form['actions'] = array(
    '#type' => 'actions',
    'cancel' => array(
      '#type' => 'submit',
      '#value' => t('Get me out of here!'),
      '#submit' => array('smartdocs_method_delete_cancel')
    ),
    'ok' => array(
      '#type' => 'submit',
      '#value' => t('Delete this method'),
      '#submit' => array('smartdocs_method_delete_submit')
    )
  );

  return $form;
}

/**
 * Cancel action for method deletion form.
 *
 * @param $form
 * @param $form_state
 */
function smartdocs_method_delete_cancel($form, &$form_state) {
  $form_state['redirect'] = $form_state['values']['revision_url'];
}

/**
 * Submit action for method deletion form.
 *
 * @param $form
 * @param $form_state
 * @throws Exception
 * @throws \Apigee\Exceptions\ParameterException
 */
function smartdocs_method_delete_submit($form, &$form_state) {
  $method = new Method(devconnect_default_org_config(), $form_state['values']['model_uuid'], $form_state['values']['revision_uuid'], $form_state['values']['resource_uuid']);
  Method::fromArray($method, $form_state['values']['method']);
  $method_name = $method->getDisplayName();
  $nid = $form_state['values']['nid'];
  if ($nid) {
    db_delete('smartdata')
      ->condition('nid', $nid)
      ->execute();
    switch ($form_state['values']['node_action']) {
      case 'delete':
        node_delete($nid);
        break;
      case 'unpublish':
        $node = node_load($nid);
        $node->status = 0;
        node_save($node);
        break;
    }
  }
  try {
    $method->delete();
    $cid = 'model:' . $form_state['values']['model_uuid'];
    cache_clear_all($cid, 'cache_smartdocs');
    module_invoke_all('smartdocs_model_update', $form_state['values']['model_uuid']);
    drupal_set_message(t('Method ‘@method’ was successfully deleted.', array('@method' => $method_name)));
  } catch (Apigee\Exceptions\ResponseException $e) {
    drupal_set_message(t('Deleting method ‘@method’ failed. Please check the logs.', array('@method' => $method_name)), 'error');
  }
  $form_state['redirect'] = $form_state['values']['revision_url'];
}

/**
 * Authentication settings for specific revision
 *
 * @param array $form
 * @param array $form_state
 * @param Apigee\SmartDocs\Model $model
 * @param Apigee\SmartDocs\Revision $rev
 *
 * @return array
 */
function smartdocs_revision_auth(array $form, array &$form_state, Model $model, Revision $revision) {

  $rev = $revision->getRevisionNumber();
  drupal_set_title(t('Authentication Settings for API Revision #@rev', array('@rev' => $rev)));

  $crumb = drupal_get_breadcrumb();
  array_pop($crumb);
  $crumb[] = l('#' . $rev, 'admin/smartdocs/models/' . $model->getUuid() . '/revisions/' . $rev);
  $crumb[] = t('Authentication Settings');
  drupal_set_breadcrumb($crumb);

  $form['model'] = array(
    '#type' => 'value',
    '#value' => $model->toArray(),
  );
  $form['revision'] = array(
    '#type' => 'value',
    '#value' => $revision->toArray(),
  );

  $auth_scheme = new AuthScheme($model->getConfig(), $model->getUuid(), $revision->getUuid());
  try {
    $oauth = $auth_scheme->loadOauth2Credentials();
  } catch (Exception $e) {
    $oauth = NULL;
  }

  $form['oauth_action'] = array(
    '#type' => 'value',
    '#value' => (empty($oauth) ? 'add' : 'edit'),
  );

  $form['oauth'] = array(
    '#type' => 'fieldset',
    '#title' => t('OAuth 2.0 Web Server Flow'),
    '#prefix' => '<div id="oauth-config">',
    '#suffix' => '</div>',
    '#collapsed' => TRUE,
    '#collapsible' => TRUE,
    '#tree' => TRUE,
  );
  $form['oauth']['clientId'] = array(
    '#type' => 'textfield',
    '#title' => t('Client ID'),
    '#default_value' => isset($oauth) ? $oauth->getClientId() : NULL,
  );
  $form['oauth']['clientSecret'] = array(
    '#type' => 'textfield',
    '#title' => t('Client Secret'),
    '#default_value' => isset($oauth) ? $oauth->getClientSecret() : NULL,
  );
  $form['oauth']['authorizationUrl'] = array(
    '#type' => 'textfield',
    '#title' => t('Authorization URL'),
    '#default_value' => isset($oauth) ? $oauth->getAuthUrl() : NULL,
  );
  $form['oauth']['authorizationVerb'] = array(
    '#type' => 'radios',
    '#title' => t('Authorization Verb'),
    '#default_value' => isset($oauth) ? $oauth->getAuthVerb() : NULL,
    '#options' => array(
      'GET' => 'GET',
      'POST' => 'POST',
    ),
  );
  $form['oauth']['accessTokenUrl'] = array(
    '#type' => 'textfield',
    '#title' => t('Access Token URL'),
    '#default_value' => isset($oauth) ? $oauth->getAccessTokenUrl() : NULL,
  );
  $form['oauth']['accessTokenType'] = array(
    '#type' => 'radios',
    '#title' => t('Access Token Type'),
    '#default_value' => isset($oauth) ? $oauth->getAccessTokenType() : NULL,
    '#options' => array(
      'header' => 'Header',
      'query' => 'Query',
    ),
  );
  $form['oauth']['accessTokenParamName'] = array(
    '#type' => 'textfield',
    '#title' => t('Access Token param name'),
    '#default_value' => isset($oauth) ? $oauth->getAccessTokenParamName() : NULL,
  );
  $form['oauth']['clientAuthScheme'] = array(
    '#type' => 'radios',
    '#title' => t('Client Authentication Name'),
    '#default_value' => isset($oauth) ? $oauth->getClientAuthScheme() : NULL,
    '#options' => array(
      'header' => 'Header',
      'body' => 'Body',
    ),
  );
  $callback = variable_get($model->getName() . '_' . $rev . '_callbackURL', NULL);
  if (isset($callback['callback'])) {
    $form['oauth']['callbackURL'] = array(
      '#type' => 'textfield',
      '#title' => t('Callback URL'),
      '#default_value' => $callback['callback'],
      '#disabled' => TRUE,
    );
  }

  try {
    $token = $auth_scheme->loadTokenCredentials();
  } catch (Exception $e) {
    $token = NULL;
  }

  $form['token_action'] = array(
    '#type' => 'value',
    '#value' => (empty($token) ? 'add' : 'edit'),
  );

  $form['token'] = array(
    '#type' => 'fieldset',
    '#title' => t('Custom Token'),
    '#prefix' => '<div id="custom-token">',
    '#suffix' => '</div>',
    '#collapsed' => TRUE,
    '#collapsible' => TRUE,
    '#tree' => TRUE,
  );
  $form['token']['tokenType'] = array(
    '#type' => 'radios',
    '#title' => t('Token Type'),
    '#default_value' => isset($token) ? $token->getTokenType() : NULL,
    '#options' => array(
      'header' => 'Header',
      'query' => 'Query',
    ),
  );
  $form['token']['tokenMap'] = array(
    '#prefix' => '<div id="custom-token-map">',
    '#suffix' => '</div>',
    '#collapsed' => FALSE,
    '#collapsible' => TRUE,
    '#tree' => TRUE,
  );
  $button = TRUE;
  // This is black magic from the AJAX handler used to add/remove fields.
  if (empty($form_state['num_tokens'])) {
    if (isset($token) && count($token->getTokenMap()) > 0) {
      foreach ($token->getTokenMap() as $name => $value) {
        $form_state['num_tokens'][uniqid()] = array(
          'name' => $name,
          'value' => $value
        );
      }
    }
    else {
      $form_state['num_tokens'][uniqid()] = array(
        'name' => '',
        'value' => ''
      );
    }
  }
  $i = 0;
  foreach ($form_state['num_tokens'] as $unique_id => $params) {
    if (is_array($params) && isset($params['name'])) {
      $form['token']['tokenMap'][$unique_id] = array(
        '#title' => t('Default Token Value #@num (optional)', array('@num' => ($i + 1))),
        '#type' => 'fieldset',
        '#collapsed' => FALSE,
        '#collapsible' => TRUE,
      );
      $form['token']['tokenMap'][$unique_id]['name'] = array(
        '#type' => 'textfield',
        '#title' => t('Default Name'),
        '#default_value' => (!empty($params['name'])) ? $params['name'] : '',
      );
      $form['token']['tokenMap'][$unique_id]['value'] = array(
        '#type' => 'textfield',
        '#title' => t('Default Value'),
        '#default_value' => (!empty($params['value'])) ? $params['value'] : '',
      );
      if ($button) {
        $form['token']['tokenMap'][$unique_id]['remove'] = array(
          '#type' => 'submit',
          '#attributes' => array(
            'data' => $unique_id,
          ),
          '#value' => t('Remove token #' . ($i + 1)),
          '#submit' => array('smartdocs_token_auth_remove'),
          '#limit_validation_errors' => array(),
          '#ajax' => array(
            'callback' => 'smartdocs_token_auth_callback',
            'wrapper' => 'custom-token-map',
          ),
          '#suffix' => '<br/><br/>'
        );
      }
      $i++;
    }
  }
  $form['token']['tokenMap']['add'] = array(
    '#type' => 'submit',
    '#value' => t('Add Token'),
    '#submit' => array('smartdocs_token_auth_add'),
    '#limit_validation_errors' => array(),
    '#ajax' => array(
      'callback' => 'smartdocs_token_auth_callback',
      'wrapper' => 'custom-token-map',
    ),
    '#suffix' => '<br/><br/>'
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Save Authentication Settings'
  );
  return $form;
}

/**
 * Ajax Callback to rebuild form
 *
 * @param $form
 * @param $form_state
 * @return mixed
 */
function smartdocs_token_auth_callback($form, $form_state) {
  return $form['token']['tokenMap'];
}

/**
 * Adds parameter to the form
 *
 * @param $form
 * @param $form_state
 */
function smartdocs_token_auth_remove($form, &$form_state) {
  $form_state['num_tokens'] = array();
  $remove = $form_state['clicked_button']['#attributes']['data'];
  foreach ($form_state['input']['token']['tokenMap'] as $unique_id => $token) {
    if (!($remove == $unique_id)) {
      $form_state['num_tokens'][$unique_id] = array(
        'name' => $token['name'],
        'value' => $token['value']
      );
    }
  }
  $form_state['rebuild'] = TRUE;
}

/**
 * Adds parameter to the form
 *
 * @param $form
 * @param $form_state
 */
function smartdocs_token_auth_add($form, &$form_state) {
  foreach ($form_state['input']['token']['tokenMap'] as $unique_id => $token) {
    $form_state['num_tokens'][$unique_id] = array(
      'name' => $token['name'],
      'value' => $token['value']
    );
  }
  $form_state['num_tokens'][uniqid()] = array(
    'name' => '',
    'value' => ''
  );
  $form_state['rebuild'] = TRUE;
}

/**
 * Authentication Submit Form
 *
 * @param $form
 * @param $form_state
 */
function smartdocs_revision_auth_submit($form, &$form_state) {
  $model = new Model(devconnect_default_org_config());
  Model::fromArray($model, $form_state['values']['model']);
  $revision = new Revision($model->getConfig(), $model->getUuid());
  Revision::fromArray($revision, $form_state['values']['revision']);

  $auth_scheme = new AuthScheme($model->getConfig(), $model->getUuid(), $revision->getUuid());
  if (!empty($form_state['values']['oauth']['clientId'])) {
    $update = ($form_state['values']['oauth_action'] == 'edit');
    $payload = $form_state['values']['oauth'];
    $payload['name'] = 'oauth2WebServerFlow';
    try {
      $auth_scheme->saveOauth2Credentials(new Oauth2Credentials($payload));
      drupal_set_message(t('OAuth 2 Web Server Flow has been ' . ($update ? 'updated' : 'added to the revision') . '.'), 'status');
    } catch (Exception $e) {
      drupal_set_message(t('There was a problem ' . ($update ? 'updating the ‘OAuth2 Web Server Flow’ for' : 'adding the ‘OAuth2 Web Server Flow’ to') . ' the current revision.'), 'error');
    }
  }
  else {
    drupal_set_message(t('No configuration settings for OAuth 2 Web Server Flow found.'), 'notice');
  }

  if (!empty($form_state['values']['token']['tokenType'])) {
    $map = array();
    foreach ($form_state['values']['token']['tokenMap'] as $key => $value) {
      if (is_array($value)) {
        if (isset($value['name']) && !empty($value['name'])) {
          $map[$value['name']] = $value['value'];
        }
      }
    }
    $payload = array(
      'name' => 'custom',
      'tokenType' => $form_state['values']['token']['tokenType'],
    );
    if (!empty($map)) {
      $payload['tokenMap'] = $map;
    }
    $update = ($form_state['values']['token_action'] == 'edit');
    try {
      $auth_scheme->saveTokenCredentials(new TokenCredentials($payload));
      drupal_set_message(t('Custom Token has been ' . ($update ? 'updated' : 'added to the revision') . '.'), 'status');
    } catch (Exception $e) {
      drupal_set_message(t('There was a problem ' . ($update ? 'updating ‘Custom Token’ for' : 'adding ‘Custom Token’ to') . ' the current revision.'), 'error');
    }
  }
  else {
    drupal_set_message(t('No configuration settings for Custom Token found.'), 'info');
  }
}


/**
 * Edit Revision Form
 *
 * @param array $form
 * @param array $form_state
 * @param Apigee\SmartDocs\Model $model
 * @param Apigee\SmartDocs\Revision $revision
 */
function smartdocs_edit_revision(array $form, array $form_state, Model $model, Revision $revision) {
  drupal_set_title(t('Edit API Revision #@rev of model ‘@model’', array(
    '@rev' => $revision->getRevisionNumber(),
    '@model' => $model->getDisplayName()
  )));
  $form['model'] = array(
    '#type' => 'value',
    '#value' => $model->toArray(),
  );
  $form['revision'] = array(
    '#type' => 'value',
    '#value' => $revision->toArray(),
  );
  $form['display_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Display Name'),
    '#size' => 60,
    '#default_value' => $revision->getDisplayName(),
    '#maxlength' => 128,
    '#required' => TRUE,
  );
  $form['version'] = array(
    '#type' => 'textfield',
    '#title' => t('Version ID'),
    '#size' => 60,
    '#default_value' => $revision->getReleaseVersion(),
    '#maxlength' => 128,
    '#required' => TRUE,
  );
  $form['description'] = array(
    '#type' => 'textarea',
    '#title' => t('Description'),
    '#default_value' => $revision->getDescription(),
    '#rows' => 5,
    '#required' => TRUE,
  );
  $form['base_url'] = array(
    '#type' => 'textfield',
    '#title' => t('Base URL'),
    '#default_value' => $revision->getBaseUrl(),
    '#size' => 60,
    '#maxlength' => 128,
    '#required' => TRUE,
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#submit' => array('smartdocs_edit_revision_submit'),
    '#value' => t('Save API Revision'),
  );
  return $form;
}

/**
 * Edit Revision Form Submit
 *
 * @param $form
 * @param $form_state
 */
function smartdocs_edit_revision_submit($form, &$form_state) {
  $model = new Model(devconnect_default_org_config());
  Model::fromArray($model, $form_state['values']['model']);
  $revision = new Revision($model->getConfig(), $model->getUuid());
  Revision::fromArray($revision, $form_state['values']['revision']);

  $revision->setDisplayName($form_state['values']['display_name']);
  $revision->setDescription($form_state['values']['description']);
  $revision->setReleaseVersion($form_state['values']['version']);
  $revision->setBaseUrl($form_state['values']['base_url']);

  try {
    $revision->save(TRUE);
    cache_clear_all('model:' . $model->getUuid(), 'cache_smartdocs');
    module_invoke_all('smartdocs_model_update', $model->getUuid());

    $rev = $revision->getRevisionNumber();
    drupal_set_message(t('Revision #@rev has been updated.', array('@rev' => $rev)));
    $form_state['redirect'] = 'admin/smartdocs/models/' . $model->getUuid() . '/revisions/' . $rev;
  } catch (Exception $e) {
    drupal_set_message(t('Revision #@rev failed to update.', array('@rev' => $revision->getRevisionNumber())), 'error');
  }
}

