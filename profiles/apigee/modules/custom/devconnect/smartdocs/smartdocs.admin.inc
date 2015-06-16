<?php
/**
 * @file
 * Supplies administrative functions for SmartDocs.
 */

use Apigee\SmartDocs\Method;
use Apigee\SmartDocs\Model;
use Apigee\SmartDocs\Resource;
use Apigee\SmartDocs\Revision;
use Apigee\SmartDocs\Template;
use Apigee\SmartDocs\Security;
use Apigee\SmartDocs\Security\SecurityScheme;
use Apigee\SmartDocs\TemplateAuth;
use Apigee\SmartDocs\Security\TemplateAuthScheme;

use Apigee\Exceptions\ResponseException;

/**
 * SmartDocs Settings Form.
 *
 * @param array $form
 *   Descriptor of the form being created.
 * @param array $form_state
 *   State of the form being created.
 *
 * @return array
 *   Descriptor of the newly-created form.
 */
function smartdocs_settings($form, $form_state) {
  $form['get_help'] = array(
    '#type' => 'markup',
    '#markup' => t('For additional help in configuring SmartDocs, consult the <a href="@url">online documentation</a>.', array('@url' => 'http://apigee.com/docs/developer-services/content/using-smartdocs-document-apis')),
    '#prefix' => '<div style="margin-bottom:1.5em">',
    '#suffix' => '</div>',
  );

  $form['disable_ssl_warning'] = array(
    '#title' => t("Disable SSL warning message"),
    '#type' => 'checkbox',
    '#default_value' => variable_get('disable_ssl_warning', 0),
    '#description' => t("Select this option if you would like the warning message shown, if SSL is not enabled."),
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
    '#submit' => array('smartdocs_rebuild_cache_table'),
  );

  // Don't bother with API Product correlation if devconnect_developer_apps is
  // not enabled; otherwise we'll encounter failure when trying to load
  // api_product entities.
  if (module_exists('devconnect_developer_apps')) {
    $form['products'] = array(
      '#type' => 'fieldset',
      '#title' => t('API Products/SmartDocs Settings'),
      '#collapsed' => TRUE,
      '#collapsible' => TRUE,
    );
    $form['products']['info'] = array(
      '#markup' => t('This section creates a correlation between your products and SmartDocs internally.'),
      '#prefix' => '<p>',
      '#suffix' => '</p><br>',
    );
    try {
      $model_obj = new Model(devconnect_default_org_config());
      $models = $model_obj->listModels();
    }
    catch (ResponseException $e) {
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
      $default_value = variable_get($machine . '_docs', array());

      if (!empty($default_value)) {
        $form['products']['#collapsed'] = FALSE;
      }

      $form['products'][$machine . '_docs'] = array(
        '#type' => 'checkboxes',
        '#title' => t(':product models', array(':product' => $product)),
        '#default_value' => $default_value,
        '#description' => t('Specify the SmartDocs model that relates to the product.'),
        '#options' => $options,
        '#suffix' => '<br/>',
      );
    }
  }
  $form['smartdocs_publish_on_render'] = array(
    '#type' => 'checkbox',
    '#title' => t('Publish rendered nodes immediately'),
    '#description' => t('If selected, this will publish SmartDocs nodes as soon as they are rendered.'),
    '#default_value' => variable_get('smartdocs_publish_on_render', 1),
    '#return_value' => 1,
  );

  $form['smartdocs_path_gen_type'] = array(
    '#type' => 'markup',
    '#markup' => t('To configure auto-generated paths for Smart Method nodes, <a href="@url">go to the PathAuto configuration page</a>.', array(
      '@url' => url('admin/config/search/path/patterns', array('fragment' => 'edit-node')),
    )),
    '#prefix' => '<p>',
    '#suffix' => '</p>',
  );

  $form['settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Advanced settings'),
    '#collapsed' => TRUE,
    '#collapsible' => TRUE,
  );

  $form['settings']['advanced_settings_help'] = array(
    '#type' => 'markup',
    '#markup' => t('For additional help in configuring these advanced settings, consult our <a href="@url">online documentation</a>.', array('@url' => 'http://apigee.com/docs/developer-services/content/using-smartdocs-document-apis')),
    '#prefix' => '<div style="margin-bottom:1.5em">',
    '#suffix' => '</div>',
  );

  $form['settings']['custom_template_file'] = array(
    '#title' => t("Upload customized model template"),
    '#type' => 'managed_file',
    '#description' => t('Upload a .hbr template file. This template file will be used to render the methods instead of the default template.'),
    '#upload_location' => 'public://smartdocs_templates',
    '#upload_validators' => array(
      'file_validate_extensions' => array('hbr'),
    ),
    '#default_value' => variable_get('custom_template_file', 0),
    '#suffix' => '<br/>',
  );

  $form['settings']['api_proxy'] = array(
    '#type' => 'fieldset',
    '#title' => t('Management API override settings'),
    '#collapsed' => FALSE,
    '#collapsible' => TRUE,
  );

  $form['settings']['api_proxy']['is_edge_mgmt_firewalled'] = array(
    '#type' => 'checkbox',
    '#title' => t('Edge Management server is firewalled'),
    '#default_value' => variable_get('is_edge_mgmt_firewalled', 0),
    '#description' => t('For on premise Edge installations, configure settings due to the Edge server being not available through the internet.'),
  );

  $form['settings']['api_proxy']['is_edge_mgmt_firewalled_settings'] = array(
    '#type' => 'container',
    '#states' => array(
      // Hide the additional firewall settings if is_edge_mgmt_firewalled is
      // not set.
      'invisible' => array(
        'input[name="is_edge_mgmt_firewalled"]' => array('checked' => FALSE),
      ),
    ),
  );

  $form['settings']['api_proxy']['is_edge_mgmt_firewalled_settings']['data_proxy_url'] = array(
    '#type' => 'textfield',
    '#title' => t('Send Request Proxy'),
    '#description' => t('If the send request backend is not discoverable, pre-supply it here.'),
    '#default_value' => variable_get('data_proxy_url', 'https://apiconsole-prod.apigee.net/smartdocs/v1'),
  );

  $form['settings']['api_proxy']['is_edge_mgmt_firewalled_settings']['data_auth_url'] = array(
    '#type' => 'textfield',
    '#title' => t('Management CORS Proxy (legacy)'),
    '#description' => t('If the authUrl is not discoverable, pre-supply it here.'),
    '#default_value' => variable_get('data_auth_url', 'https://api.enterprise.apigee.com/v1/users/{user}/authenticate'),
  );

  $form['settings']['api_proxy']['use_local_assets'] = array(
    '#type' => 'checkbox',
    '#title' => t('Use local SmartDocs JS/CSS'),
    '#default_value' => variable_get('use_local_assets', 0),
    '#description' => t('By enabling this setting, you will use local assets hosted by Dev Portal.'),
  );

  $form['settings']['api_proxy']['local_assets'] = array(
    '#type' => 'container',
    '#states' => array(
      // Hide the additional local asset settings when use local SmartDocs
      // JS/CSS is disabled.
      'invisible' => array(
        'input[name="use_local_assets"]' => array('checked' => FALSE),
      ),
    ),
  );

  $form['settings']['api_proxy']['local_assets']['local_asset_version'] = array(
    '#type' => 'select',
    '#title' => t('Local SmartDocs JS/CSS version'),
    '#default_value' => variable_get('local_asset_version', '6'),
    '#description' => t('By enabling this setting, you will use local assets hosted by Dev Portal.'),
    '#options' => array(
      '1' => 'v1',
      '2' => 'v2',
      '3' => 'v3',
      '4' => 'v4',
      '5' => 'v5',
      '6' => 'v6',
    ),
  );
  $form['settings']['api_proxy']['use_api_proxy_url'] = array(
    '#type' => 'checkbox',
    '#title' => t('Use Management API URL override'),
    '#default_value' => variable_get('use_api_proxy_url', 0),
    '#description' => t('By enabling this setting, you will override the default Dev Portal endpoint for SmartDocs calls.'),
  );

  $form['settings']['api_proxy']['api_proxy_settings'] = array(
    '#type' => 'container',
    '#states' => array(
      // Hide the api proxy URL if use management API URL override is not
      // enabled.
      'invisible' => array(
        'input[name="use_api_proxy_url"]' => array('checked' => FALSE),
      ),
    ),
  );

  $form['settings']['api_proxy']['api_proxy_settings']['api_proxy_url'] = array(
    '#type' => 'textfield',
    '#title' => t('Management API URL override'),
    '#default_value' => variable_get('api_proxy_url', 'https://api.enterprise.apigee.com/v1'),
  );

  $form = system_settings_form($form);

  // Make sure the array is flattened before system_settings_form_submit() is
  // called.
  array_unshift($form['#submit'], 'smartdocs_settings_flatten_array');
  // Additional submit handler to be invoked *after* calling
  // system_settings_form_submit().
  $form['#submit'][] = 'smartdocs_settings_submit';
  return $form;
}

/**
 * Early submit handler for smartdocs_settings().
 *
 * Flattens arrays of models to which API Products might correspond. Since the
 * incoming array is generated by a checkboxes element, it is by nature very
 * ugly. Here we convert it to a simple numerically-indexed array.
 *
 * @param array $form
 *   Descriptor of the form being submitted.
 * @param array $form_state
 *   State of the form being submitted.
 */
function smartdocs_settings_flatten_array($form, &$form_state) {
  // Short circuit if devconnect_developer_apps is not enabled.
  if (!array_key_exists('products', $form)) {
    return;
  }
  foreach ($form_state['values'] as $key => $val) {
    if (substr($key, -5) == '_docs' && is_array($val)) {
      $values = array();
      foreach ($val as $name => $value) {
        if ($value !== 0) {
          $values[] = $value;
        }
      }
      $form_state['values'][$key] = $values;
    }
  }
}

/**
 * Submit callback for the smartdocs_settings(); handles template file upload.
 *
 * @param array $form
 *   Descriptor of the form being submitted.
 * @param array $form_state
 *   State of the form being submitted.
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
    '#title' => t('New model'),
    '#href' => 'admin/content/smartdocs/models/add',
    '#prefix' => '<div style="margin-bottom:1.5em">',
    '#suffix' => '</div>',
  );

  $models = _smartdocs_get_methods_details();

  $table_rows = array();
  /** @var Apigee\SmartDocs\Model $model */
  foreach ($models as $model) {
    $latest_rev = $model->getLatestRevisionNumber();
    if ($latest_rev > 0) {
      $model_name = l($model->getDisplayName(), 'admin/content/smartdocs/models/' . $model->getUuid() . '/revisions/' . $model->getLatestRevisionNumber());
    }
    else {
      $model_name = l($model->getDisplayName(), 'admin/content/smartdocs/models/' . $model->getUuid());
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
          'class' => array('list-' . $model->getName(), 'views-ajax-link'),
        ),
      );
    }
    $operations = array(
      'revisions' => 'API Revisions',
      'add' => 'Add Revision',
      'import' => 'Import',
      'settings' => 'Settings',
      'export' => 'Export',
      'delete' => 'Delete',
    );
    foreach ($operations as $op => $disp) {
      // Don't show settings if there are no methods yet.
      if ($op == 'settings' && $method_count == 0) {
        continue;
      }

      $ops['list-' . $op] = array(
        'title' => $disp,
        'href' => 'admin/content/smartdocs/models/' . $model->getUuid() . '/' . $op,
        'attributes' => array(
          'title' => $disp,
          'class' => array($op . '-' . $model->getName(), 'views-ajax-link'),
        ),
      );
    }
    $table_rows[] = array(
      $model_name,
      $model->getDescription(),
      $method_count,
      $node_count,
      $published_count,
      theme('links__ctools_dropbutton', array('links' => $ops)),
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
        'smartdocs-operations',
      ),
    ),
  );

  $empty_msg = t('No models have been created. <a href="@add_model">Create a new model</a> or <a href="@read_docs">read the documentation</a> to get started.', array(
      '@add_model' => url('admin/content/smartdocs/models/add'),
      '@read_docs' => url('http://apigee.com/docs/developer-services/content/using-smartdocs-document-apis'),
    )
  );

  $output['table'] = array(
    '#markup' => theme('table', array(
      'header' => $header,
      'rows' => $table_rows,
      'empty' => $empty_msg,
    )),
  );

  return $output;
}

/**
 * Page callback for admin/content/smartdocs/add-model.
 *
 * @param array $form
 *   Descriptor of the form being submitted.
 * @param array $form_state
 *   State of the form being submitted.
 *
 * @return array
 *   Descriptor of newly-created form.
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
      'error' => t('The internal name must contain only letters, digits, underscores, or hyphens.'),
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
    '#value' => t('Create Model'),
  );
  return $form;
}

/**
 * Validator callback for smartdocs_add_model().
 *
 * Ensures that the magic name 'add' is not used as a model name.
 *
 * @param array $form
 *   Descriptor of the form being validated.
 * @param array $form_state
 *   State of the form being validated.
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
 *   Descriptor of the form being submitted.
 * @param array $form_state
 *   State of the form being submitted.
 */
function smartdocs_add_model_submit(array $form, array &$form_state) {

  $model = new Model(devconnect_default_org_config());
  $model->setName(trim($form_state['values']['model_name']));
  $model->setDisplayName(trim($form_state['values']['display_name']));
  $model->setDescription(trim($form_state['values']['model_description']));

  $success = FALSE;
  try {
    $model->save();

    $path = drupal_get_path('module', 'smartdocs') . '/templates/smartdocs.hbr';
    $custom_template_file = variable_get('custom_template_file', NULL);

    if (!empty($custom_template_file)) {
      $file = file_load($custom_template_file);
      $path = file_create_url($file->uri);
    }
    $html = file_get_contents($path);
    $template = new Template(devconnect_default_org_config(), $model->getUuid());
    $template->save(SMARTDOCS_TEMPLATE_NAME, 'method', $html, TRUE);

    $success = TRUE;
    cache_clear_all('model:' . $model->getUuid(), 'cache_smartdocs');
    module_invoke_all('smartdocs_model_update', $model->getUuid());

    // Create taxonomy term for model.
    $term = taxonomy_get_term_by_name($model->getName(), 'smartdocs_models');
    if (empty($term)) {
      $vocab = taxonomy_vocabulary_machine_name_load('smartdocs_models');
      if (!empty($vocab)) {
        $term = new stdClass;
        $term->vid = $vocab->vid;
        $term->name = $model->getName();
        $term->description = $model->getDescription();
        $term->field_model_display_name[LANGUAGE_NONE][0]['value'] = $model->getDisplayName();
        taxonomy_term_save($term);
      }
    }
  }
  catch (Exception $e) {
    drupal_set_message(t('There was an error when trying to create the model.'), 'error');
    watchdog_exception('smartdocs', $e);
  }
  if ($success) {
    menu_rebuild();
    drupal_set_message($model->getDisplayName() . ' was created successfully.', 'status');
    $form_state['redirect'] = 'admin/content/smartdocs/models/' . $model->getUuid();
  }
}

/**
 * Checks to see if a given model exists already.
 *
 * @param string $name
 *   Name of the model whose existence is to be determined.
 *
 * @return bool
 *   TRUE if the model exists, FALSE if it doesn't.
 */
function smartdocs_model_exist($name) {
  try {
    $model_obj = new Model(devconnect_default_org_config());
    $models = $model_obj->listModels();
  }
  catch (ResponseException $e) {
    return FALSE;
  }
  /** @var Apigee\Smartdocs\Model $model */
  foreach ($models as $model) {
    if ($model->getName() == $name) {
      return TRUE;
    }
  }
  return FALSE;
}

/**
 * Page callback to display model detail.
 *
 * @param Model $model
 *   Model whose detail is to be displayed.
 *
 * @return array
 *   Render-array of data to be displayed.
 */
function smartdocs_show_model_detail(Model $model) {

  $breadcrumb = array();
  $breadcrumb[] = l(t('Home'), '<front>');
  $breadcrumb[] = l(t('Content'), 'admin/content');
  $breadcrumb[] = l(t('SmartDocs'), 'admin/content/smartdocs/models');
  $breadcrumb[] = l(t('@model', array('@model' => $model->getDisplayName())), 'admin/content/smartdocs/models/' . $model->getUuid());
  drupal_set_breadcrumb($breadcrumb);
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
    'import' => 'Import',
    'add' => 'Add Revision',
    'settings' => 'Settings',
    'export' => 'Export',
    'delete' => 'Delete',
  ) + $operations;

  $links = array();
  foreach ($operations as $key => $op) {
    $links[$key . '-' . $model_name] = array(
      'title' => $op,
      'href' => 'admin/content/smartdocs/models/' . $model_uuid . '/' . $key,
      'attributes' => array(
        'title' => $op,
        'class' => array(
          $key . '-' . $model_name,
          'views-ajax-link',
        ),
      ),
    );
  }
  // TODO fixme.
  if ($count_nids && $view_path = _smartdocs_get_revision_view_path($model)) {
    $link = array(
      'title' => 'View Published Docs',
      'href' => $view_path,
      'attributes' => array(
        'title' => 'View Published Documentation',
        'class' => array('list-' . $model_name, 'views-ajax-link'),
      ),
    );
    $links = array('list-' . $model_name => $link) + $links;
  }
  $output['name'] = array(
    '#type' => 'item',
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
      '#markup' => l(t('Revision #@rev', array('@rev' => $revision->getRevisionNumber())), 'admin/content/smartdocs/models/' . $model_uuid . '/revisions/' . $revision->getRevisionNumber()),
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
 * @param Model $model
 *   The model whose view path is to be determined.
 *
 * @return string|bool
 *   Path of view, or FALSE if not found.
 */
function _smartdocs_get_revision_view_path(Model $model) {
  $terms = taxonomy_get_term_by_name($model->getName(), 'smartdocs_models');
  if (!empty($terms)) {
    $term = reset($terms);
    return 'taxonomy/term/' . $term->tid;
  }
  return FALSE;
}

/**
 * Imports WADL / Swagger data into SmartDocs.
 *
 * @param array $form
 *   Descriptor of the form being created.
 * @param array $form_state
 *   State of the form being created.
 * @param Model $model
 *   Model into which data is to be imported.
 */
function smartdocs_import_model(array $form, array $form_state, Model $model) {
  drupal_set_title('Import into ‘' . $model->getDisplayName() . '’');

  $breadcrumb = array();
  $breadcrumb[] = l(t('Home'), '<front>');
  $breadcrumb[] = l(t('Content'), 'admin/content');
  $breadcrumb[] = l(t('SmartDocs'), 'admin/content/smartdocs/models');
  $breadcrumb[] = l(t('@model', array('@model' => $model->getDisplayName())), 'admin/content/smartdocs/models/' . $model->getUuid());
  drupal_set_breadcrumb($breadcrumb);

  $form['model_uuid'] = array(
    '#type' => 'value',
    '#value' => $model->getUuid(),
  );
  $form['format'] = array(
    '#prefix' => '<div id="import-format">',
    '#suffix' => '</div>',
    '#type' => 'fieldset',
    '#description' => t('<strong>Note:</strong> Swagger 1.2 formats can only be imported by URL.'),
    '#title' => t('Import Options'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['format']['type'] = array(
    '#type' => 'select',
    '#title' => t('Choose format'),
    '#options' => array(
      'wadl' => 'WADL',
      'swagger_json' => 'Swagger - JSON',
      'swagger_yaml' => 'Swagger - YAML',
      'apigee_json' => 'SmartDocs - JSON',
    ),
    '#default_value' => array('wadl'),
  );

  $form['format']['upload_type'] = array(
    '#type' => 'select',
    '#title' => t('Upload Type'),
    '#options' => array(
      'file' => 'File',
      'url' => 'URL',
    ),
    '#ajax' => array(
      'wrapper' => 'import-format',
      'callback' => 'smartdocs_import_ajax',
    ),
    '#default_value' => array('file'),
  );

  // Either show URL or File upload depending on upload_type.
  if (!isset($form_state['values']) || $form_state['values']['upload_type'] == 'file') {
    // Show file upload field.
    $form['format']['import']['file'] = array(
      '#type' => 'file',
      '#description' => t('To clear this field, <a href="!reset">reset the form</a>.', array('!reset' => url($_GET['q']))),
      '#title' => t('File'),
    );
  }
  else {
    // Show URL upload field.
    $form['format']['import']['url'] = array(
      '#type' => 'textfield',
      '#title' => t('URL'),
      '#size' => 60,
      '#maxlength' => 128,
      '#default_value' => '',
      '#required' => TRUE,
    );
  }

  $form['import'] = array(
    '#type' => 'submit',
    '#value' => t('Import'),
  );
  return $form;
}

/**
 * Ajax callback for import format.
 *
 * @param array $form
 *   Descriptor of the form being altered.
 * @param array $form_state
 *   State of the form being altered.
 *
 * @return array
 *   Altered fragment of the form.
 */
function smartdocs_import_ajax($form, &$form_state) {
  $form['#rebuild'] = TRUE;
  return $form['format'];
}

/**
 * Validate callback for smartdocs_import_model.
 *
 * @param array $form
 *   Descriptor of the form being validated.
 * @param array $form_state
 *   State of the form being validated.
 */
function smartdocs_import_model_validate($form, &$form_state) {

  if ($form_state['values']['upload_type'] == 'file') {
    // Validate file is not null.
    if (empty($_FILES['files']['name']['file'])) {
      form_set_error('file', t('You must select a file to upload.'));
    }
  }
  else {
    // Validate URL is valid.
    if (isset($form_state['values']['url']) && !valid_url($form_state['values']['url'], TRUE)) {
      form_set_error('url', t('The URL %url is invalid.', array('%url' => $form_state['values']['url'])));
    }
  }
}

/**
 * Imports WADL / Swagger data into SmartDocs.
 *
 * @param array $form
 *   Descriptor of the form being created.
 * @param array $form_state
 *   State of the form being created.
 */
function smartdocs_import_model_submit(array $form, array &$form_state) {
  $type = $form_state['values']['type'];
  /** @var Apigee\SmartDocs\Model $model */
  $model = new Model(devconnect_default_org_config());
  $model->load($form_state['values']['model_uuid']);

  $content_type = NULL;
  switch ($type) {
    case 'wadl':
      $content_type = 'application/xml';
      $document_format = 'wadl';
      break;

    case 'swagger_json':
      $content_type = 'application/json';
      $document_format = 'swagger';
      break;

    case 'swagger_yaml':
      $content_type = 'application/yaml';
      $document_format = 'swagger';
      break;

    case 'apigee_json':
      $content_type = 'application/json';
      $document_format = 'apimodel';
      break;

    default:
      $content_type = 'application/yaml';
      $document_format = 'swagger';
  }

  if ($form_state['values']['upload_type'] == 'url') {
    // Importing a URL.
    $url = $form_state['values']['url'];
    try {
      $revision = $model->importUrl($url, $document_format);

      // Parameters to use in success message to user.
      $message_vars = array(
        '%model_name' => $model->getName(),
        '%revision' => $revision,
        '%source_name' => "URL",
        '%source' => $url,
      );
    }
    catch (ResponseException $e) {
      smartdocs_admin_handle_import_exception($e, $model, 'URL', $url);
      // Prevent Drupal from cleaning the form.
      $form_state['redirect'] = FALSE;
      return;
    }
  }
  else {
    // Importing a file.
    $file_obj = file_save_upload('file', array('file_validate_extensions' => array()));
    if (!$file_obj) {
      drupal_set_message(t('Error: SmartDocs could not save file.'), 'error');
      $form_state['redirect'] = FALSE;
      return;
    }

    if (!file_exists($file_obj->uri)) {
      file_delete($file_obj);
      drupal_set_message(t('Error: SmartDocs could not get the contents of the file %filename from URI: %file_uri.', array('%filename' => $file_obj->filename, '%file_uri' => $file_obj->uri)), 'error');
      $form_state['redirect'] = FALSE;
      return;
    }
    $file_contents = file_get_contents($file_obj->uri);
    // Delete the file form managed file table and file system.
    file_delete($file_obj);

    try {
      $revision = $model->importFile($file_contents, $document_format, $content_type);

      // Parameters to use in success message to user.
      $message_vars = array(
        '%model_name' => $model->getName(),
        '%revision' => $revision,
        '%source_name' => 'file',
        '%source' => $file_obj->filename,
      );
    }
    catch (ResponseException $e) {
      smartdocs_admin_handle_import_exception($e, $model, 'file', $file_obj->filename . ' (URI: ' . $file_obj->uri . ')');
      // Prevent Drupal from cleaning the form.
      $form_state['redirect'] = FALSE;
      return;
    }
  }

  $form_state['redirect'] = 'admin/content/smartdocs/models/' . $model->getUuid() . '/revisions/' .  $revision;
  drupal_set_message(t('Revision %revision has been created from from %source_name %source', $message_vars), 'status');
  watchdog('smartdocs', 'Model %model_name revision %revision has been created from from @source_name %source', $message_vars, WATCHDOG_INFO);
  cache_clear_all('model:' . $model->getUuid(), 'cache_smartdocs');
}

/**
 * Handles exceptions encountered when importing a model.
 *
 * @param Apigee\Exceptions\ResponseException $e
 *   The exception that was thrown.
 * @param Apigee\SmartDocs\Model $model
 *   The model being imported.
 * @param string $source_name
 *   The type of import ('file' or 'URL').
 * @param string $source
 *   The filename or URL from which the import was taking place.
 */
function smartdocs_admin_handle_import_exception(ResponseException $e, Model $model, $source_name, $source) {
  // See if there is a response body in the exception to give info to user.
  $error_code = '';
  $error_message = '';
  $error_response = $e->getResponse();
  if (!empty($error_response)) {
    $response_body = json_decode($error_response);
    if (!empty($response_body->code)) {
      $error_code = $response_body->code;
      if (!empty($response_body->message)) {
        $error_message = $response_body->message;
      }
      else {
        $error_message = '';
      }
    }
    else {
      // The error code and message is not coming back from server, just
      // display the response.
      $error_code = '';
      $error_message = $error_response;
    }
  }

  $message_vars = array(
    '%model_name' => $model->getName(),
    '@source_name' => $source_name,
    '%source' => $source,
    '%error_code' => $error_code,
    '%error_message' => $error_message,
    '@url' => 'https://apigee.com/docs/developer-services/content/using-smartdocs-document-apis',
  );
  drupal_set_message(t('There was an error importing @source_name %source. %error_code: %error_message Please consult the logs or <a href="@url" target="_blank">refer to our documentation</a>.', $message_vars), 'error');
  watchdog('smartdocs', 'Model %model_name import failure. @source_name %source %error_code: %error_message', $message_vars, WATCHDOG_ERROR);
}

/**
 * Handles exceptions thrown by model export.
 *
 * @param Exception $e
 *   The exception that was thrown.
 * @param Model $model
 *   The model which was being exported.
 * @param string $export_type
 *   Identifier for the type of export being done.
 */
function smartdocs_model_export_exception_handler(Exception $e, Model $model, $export_type) {
  // See if there is a response body in the exception to give info to user.
  $error_code = '';
  $error_message = '';
  $error_response = $e->getMessage();
  if (!empty($error_response)) {
    $error_message = $error_response;
    $error_code = $e->getCode();
  }

  $message_vars = array(
    '%model_name' => $model->getName(),
    '%export_type' => $export_type,
    '%error_code' => $error_code,
    '%error_message' => $error_message,
  );
  drupal_set_message(t('There was an export importing %export_type format. Edge API Response: %error_code: %error_message', $message_vars), 'error');
  watchdog('smartdocs', 'Model %model_name export to %export_type failure. Edge API Response: %error_code %error_message', $message_vars, WATCHDOG_ERROR);
}

/**
 * Exports Model into specified format.
 *
 * @param array $form
 *   Descriptor of the form being created.
 * @param array $form_state
 *   State of the form being created.
 * @param Apigee\SmartDocs\Model $model
 *   The Model to be exported.
 * @param Apigee\SmartDocs\Revision|null $revision
 *   The Revision to be exported. If NULL, we export the latest revision.
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

  $breadcrumb = array();
  $breadcrumb[] = l(t('Home'), '<front>');
  $breadcrumb[] = l(t('Content'), 'admin/content');
  $breadcrumb[] = l(t('SmartDocs'), 'admin/content/smartdocs/models');
  $breadcrumb[] = l(t('@model', array('@model' => $model->getDisplayName())), 'admin/content/smartdocs/models/' . $model->getUuid());
  $breadcrumb[] = l(t('Revision #@rev', array('@rev' => $revision->getRevisionNumber())), 'admin/content/smartdocs/models/' . $model->getUuid() . '/revision/' . $revision->getRevisionNumber());
  drupal_set_breadcrumb($breadcrumb);

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
    // DEVSOL-1434: Edge API cannot handle WADL exports for now.
    // 'wadl' => 'WADL',
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
 * Submit Callback for smartdocs_model_export.
 *
 * @param array $form
 *   Descriptor of the form being submitted.
 * @param array $form_state
 *   State of the form being submitted.
 */
function smartdocs_model_export_submit($form, &$form_state) {
  /** @var Apigee\SmartDocs\Model $model */
  $model = new Model(devconnect_default_org_config());
  Model::fromArray($model, $form_state['values']['model']);
  /** @var Apigee\SmartDocs\Revision $revision */
  $revision = new Revision($model->getConfig(), $model->getUuid());
  Revision::fromArray($revision, $form_state['values']['revision']);

  $export_type = $form_state['input']['file_type'];
  $filename = $model->getName() . '.' . $export_type;

  try {
    $data = $revision->export($export_type);
  }
  catch (Exception $e) {
    smartdocs_model_export_exception_handler($e, $model, $export_type);
    $form_state['redirect'] = FALSE;
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
 * Menu callback for the model template settings page.
 *
 * @param \Apigee\SmartDocs\Model $model
 *   The model whose settings are to be edited.
 *
 * @return string
 *   The html for the settings page.
 */
function smartdocs_model_settings_page(Model $model) {
  drupal_set_title(t('Template settings for @model', array('@model' => $model->getDisplayName())));
  $breadcrumb = array();
  $breadcrumb[] = l(t('Home'), '<front>');
  $breadcrumb[] = l(t('Content'), 'admin/content');
  $breadcrumb[] = l(t('SmartDocs'), 'admin/content/smartdocs/models');
  $breadcrumb[] = l(t('@model', array('@model' => $model->getDisplayName())), 'admin/content/smartdocs/models/' . $model->getUuid());
  drupal_set_breadcrumb($breadcrumb);

  $settings_form = drupal_get_form('smartdocs_model_settings', $model);
  $template_auth_form = drupal_get_form('smartdocs_model_template_auth', $model);

  return drupal_render($settings_form) . drupal_render($template_auth_form);
}

/**
 * Specifies Settings for a specific model.
 *
 * @param array $form
 *   Descriptor of the form being created.
 * @param array $form_state
 *   State of the form being created.
 * @param Model $model
 *   The Model whose settings are to be altered.
 *
 * @return array
 *   The newly-created form.
 */
function smartdocs_model_settings(array $form, array &$form_state, Model $model) {
  try {
    $template_obj = new Template($model->getConfig(), $model->getUuid());
    $template = $template_obj->load(SMARTDOCS_TEMPLATE_NAME, 'method');
  }
  catch (ResponseException $e) {
    $form['error'] = array(
      '#type' => 'markup',
      '#markup' => t('Unable to load method template from model !model. This may happen if you have not imported a revision for the model. Please !import a model and try again.',
        array(
          '!model' => '<em>' . $model->getDisplayName() . '</em>',
          '!import' => l(t('import'), 'admin/content/smartdocs/models/' . $model->getUuid() . '/import'),
        )),
      '#prefix' => '<div class="messages error">',
      '#suffix' => '</div>',
    );
    return $form;
  }

  $model_name = $model->getName();
  drupal_set_title('‘' . $model->getDisplayName() . '’' . ' Settings');
  $form['model'] = array(
    '#type' => 'value',
    '#value' => $model->toArray(),
  );
  $def_css = 'https://smartdocs.apigee.com/6/static/css/main.css
https://smartdocs.apigee.com/6/static/css/codemirror.css
https://smartdocs.apigee.com/6/static/css/prism.css';
  $def_js = "https://smartdocs.apigee.com/6/static/js/codemirror.js
https://smartdocs.apigee.com/6/static/js/codemirror_javascript.js
https://smartdocs.apigee.com/6/static/js/codemirror_xml.js
https://smartdocs.apigee.com/6/static/js/prism.js
https://smartdocs.apigee.com/6/static/js/base64_min.js
https://smartdocs.apigee.com/6/static/js/model.js
https://smartdocs.apigee.com/6/static/js/controller.js";
  $form[$model_name . '_css'] = array(
    '#type' => 'textarea',
    '#title' => t('CSS'),
    '#rows' => 3,
    '#default_value' => variable_get($model_name . '_css', $def_css),
  );
  $form['css_js'][$model_name . '_js'] = array(
    '#type' => 'textarea',
    '#title' => t('Javascript'),
    '#rows' => 7,
    '#default_value' => variable_get($model_name . '_js', $def_js),
  );

  $form['css_js']['actions'] = array(
    '#type' => 'actions',
    'submit' => array(
      '#type' => 'submit',
      '#value' => t('Save CSS & JavaScript'),
    ),
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
    '#value' => t('Save template'),
    '#submit' => array('smartdocs_model_method_settings_save'),
    '#model' => $model_name,
    '#ajax' => array(
      'callback' => 'smartdocs_method_callback',
      'wrapper' => $model_name . '-wrapper',
      'method' => 'replace',
      'effect' => 'fade',
      'progress' => array(
        'type' => 'throbber',
      ),
    ),
  );
  return $form;
}

/**
 * Handles the submit of the template auth settings form.
 *
 * @param array $form
 *   The form array.
 * @param array $form_state
 *   The form state array.
 */
function smartdocs_model_settings_submit($form, &$form_state) {
  $model_name = $form_state['values']['model']['name'];
  $model_css_name = $model_name . '_css';
  $model_js_name = $model_name . '_js';

  variable_set($model_css_name, $form_state['values'][$model_css_name]);
  variable_set($model_js_name, $form_state['values'][$model_js_name]);

  drupal_set_message(t('The CSS and JavaScript settings have been saved.'));
}

/**
 * Form for adding template auth schemes.
 *
 * @param array $form
 *   The form array.
 * @param array $form_state
 *   The form state array.
 * @param \Apigee\SmartDocs\Model $model
 *   The model object for which to add/edit the template auth schemes.
 *
 * @return array
 *   The template auth form.
 */
function smartdocs_model_template_auth($form, &$form_state, Model $model) {

  $rev = $model->getLatestRevisionNumber();
  if ($rev == -1) {
    // If there is no revision for a model there won't be any security schemes
    // and corresponding template auth scheme so we return.
    $form['error_message'] = array(
      '#type' => 'markup',
      '#markup' => t('Model ‘@model’ does not yet contain any revisions.', array('@model' => $model->getDisplayName())),
    );
    return $form;
  }
  $revision = smartdocs_revision_load($rev);
  $security = new Security(devconnect_default_org_config(), $model->getUuid(), $revision->getUuid());
  $schemes = $security->loadAllSchemes();

  $form['auth_schemes'] = array(
    '#type' => 'fieldset',
    '#title' => t('Template authentication schemes'),
    '#collapsed' => FALSE,
    '#collapsible' => TRUE,
    '#prefix' => '<div id="template_auth_settings"></div>',
  );

  $markup_args = array(
    '@doc_url' => 'http://apigee.com/docs/developer-services/content/using-smartdocs-document-apis',
    '@sec_url' => url('admin/content/smartdocs/' . $model->getUuid() . '/revisions/' . $revision->getUuid() . '/security'),
  );
  $form['auth_schemes']['help_docs'] = array(
    '#type' => 'markup',
    '#markup' => t('For additional help in configuring template authentication schemes, consult the <a href="@doc_url">online documentation</a>.<br>
       To manage the security objects, go to the <a href="@sec_url">security settings</a>.', $markup_args),
    '#prefix' => '<div class="templateauth-help"><div>',
    '#suffix' => '</div></div>',
  );

  $form['auth_schemes']['model'] = array(
    '#type' => 'value',
    '#value' => $model->toArray(),
  );

  $form['auth_schemes']['template_auth_schemes'] = array(
    '#type' => 'container',
    '#tree' => TRUE,
  );

  $template_auth = new TemplateAuth(devconnect_default_org_config(), $model->getUuid());
  $template_auth_schemes = $template_auth->loadAllSchemes();
  /** @var Apigee\SmartDocs\Security\SecurityScheme $scheme */
  foreach ($schemes as $scheme) {
    $template_auth_scheme = NULL;
    $scheme_name = $scheme->getName();
    if (!empty($template_auth_schemes) && isset($template_auth_schemes[$scheme->getName()])) {
      $template_auth_scheme = $template_auth_schemes[$scheme_name];
    }
    $form['auth_schemes']['template_auth_schemes'][$scheme_name] = array(
      '#type' => 'fieldset',
      '#title' => t('@scheme_name', array('@scheme_name' => $scheme_name)),
      '#tree' => TRUE,
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
    );
    $form['auth_schemes']['template_auth_schemes'][$scheme_name]['name'] = array(
      '#type' => 'value',
      '#value' => $scheme_name,
    );
    $form['auth_schemes']['template_auth_schemes'][$scheme_name]['type'] = array(
      '#type' => 'value',
      '#value' => $scheme->getType(),
    );

    switch ($scheme->getType()) {

      case 'OAUTH2':
        $form['auth_schemes']['template_auth_schemes'][$scheme_name]['type_display'] = array(
          '#type' => 'markup',
          '#markup' => '<div class="templateauth-type"><span>Type:</span> Oauth2 </div>',
        );
        $form['auth_schemes']['template_auth_schemes'][$scheme_name]['clientId'] = array(
          '#type' => 'textfield',
          '#title' => t('Client Id'),
          '#required' => TRUE,
          '#default_value' => !empty($template_auth_scheme) ? $template_auth_scheme->getClientId() : '',
        );
        $form['auth_schemes']['template_auth_schemes'][$scheme_name]['clientSecret'] = array(
          '#type' => 'textfield',
          '#title' => t('Client Secret'),
          '#required' => TRUE,
          '#default_value' => !empty($template_auth_scheme) ? $template_auth_scheme->getClientSecret() : '',
        );
        if (!empty($template_auth_scheme)) {
          // @TODO: Use $template_auth_scheme->getCallbackUrl(); when the
          // backend return the callback url in the response for
          // loadAllSchemes() call.
          $config = devconnect_default_org_config();
          $callback_url = $config->endpoint . '/o/' . $config->orgName . '/apimodels/' . $model->getName() . '/templateauths/' . $template_auth_scheme->getName() . '/callback';
          if (isset($callback_url)) {
            $form['auth_schemes']['template_auth_schemes'][$scheme_name]['callbackUrl'] = array(
              '#type' => 'markup',
              '#markup' => '<div class="templateauth-type"><span>Callback Url:</span>' . $callback_url . '</div>',
            );
          }
        }

        break;

      case 'APIKEY':
        $form['auth_schemes']['template_auth_schemes'][$scheme_name]['type'] = array(
          '#type' => 'markup',
          '#markup' => '<div class="templateauth-type"><span>Type:</span>  Apikey </div>',
        );
        $form['auth_schemes']['template_auth_schemes'][$scheme_name]['keyName'] = array(
          '#type' => 'value',
          '#value' => $scheme->getParamName(),
        );
        $form['auth_schemes']['template_auth_schemes'][$scheme_name]['keyValue'] = array(
          '#type' => 'textfield',
          '#title' => t('Value for the apikey parameter !param', array('!param' => '<em>' . $scheme->getParamName() . '</em>')),
          '#default_value' => !empty($template_auth_scheme) ? $template_auth_scheme->getKeyValue() : '',
          '#required' => TRUE,
        );
        break;

      case 'BASIC':
        $form['auth_schemes']['template_auth_schemes'][$scheme_name]['type_display'] = array(
          '#type' => 'markup',
          '#markup' => '<div class="templateauth-type"><span>Type:</span> Basic </div>',
        );
        $form['auth_schemes']['template_auth_schemes'][$scheme_name]['noconfig'] = array(
          '#type' => 'markup',
          '#markup' => '<div class="form-item">No configuration options.</div>',
        );
        break;
    }
    $form['auth_schemes']['template_auth_schemes'][$scheme_name]['is_update'] = array(
      '#type' => 'value',
      '#value' => !empty($template_auth_scheme) ? TRUE : FALSE,
    );
  }
  $form['auth_schemes']['actions'] = array(
    '#type' => 'actions',
    'submit' => array(
      '#type' => 'submit',
      '#value' => t('Save template authentication schemes'),
    ),
  );

  return $form;
}

/**
 * Handles the submit of the template auth settings form.
 *
 * @param array $form
 *   The form array.
 * @param array $form_state
 *   The form state array.
 */
function smartdocs_model_template_auth_submit($form, &$form_state) {
  $model_id = $form_state['values']['model']['id'];
  $template_auth_schemes = $form_state['values']['template_auth_schemes'];
  $template_auth = new TemplateAuth(devconnect_default_org_config(), $model_id);
  $success = TRUE;
  foreach ($template_auth_schemes as $payload) {
    try {
      $is_update = $payload['is_update'];
      unset($payload['is_update']);
      $scheme = TemplateAuthScheme::fromArray($payload);
      $template_auth->save($scheme, $is_update);
    }
    catch (Exception $e) {
      $success = FALSE;
    }
  }
  if ($success) {
    drupal_set_message(t('The template authentication schemes have been saved.'));
  }
  else {
    drupal_set_message(t('There was an error saving the template auth scheme, please try again.'));
  }
}

/**
 * Ajax Callback for Model Template.
 *
 * @param array $form
 *   Descriptor of the form being altered.
 * @param array $form_state
 *   State of the form being altered.
 *
 * @return array
 *   The altered fragment of the form.
 */
function smartdocs_method_callback($form, &$form_state) {
  $name = $form_state['clicked_button']['#model'];
  return $form['templates'][$name][$name . '_method_template'];
}

/**
 * Submit Callback for Model Template.
 *
 * @param array $form
 *   Descriptor of the form being submitted.
 * @param array $form_state
 *   State of the form being submitted.
 */
function smartdocs_model_method_settings_save($form, &$form_state) {
  try {
    /** @var Apigee\SmartDocs\Model $model */
    $model = new Model(devconnect_default_org_config());
    Model::fromArray($model, $form_state['values']['model']);
    $html = $form_state['values'][$model->getName() . '_method_template'];
    $template_obj = new Template($model->getConfig(), $model->getUuid());
    $template_obj->save(SMARTDOCS_TEMPLATE_NAME, 'method', $html, TRUE);

    drupal_set_message('Template saved.');
  }
  catch (Exception $e) {
    drupal_set_message('There was an error saving your template.', 'error');
  }
  $form_state['rebuild'] = TRUE;
}

/**
 * Model delete confirmation form.
 *
 * @param array $form
 *   Descriptor of the form being created.
 * @param array $form_state
 *   State of the form being created.
 * @param Model $model
 *   The Model to be deleted.
 *
 * @return array
 *   The newly-created form.
 */
function smartdocs_model_delete(array $form, array &$form_state, Model $model) {
  $breadcrumb = array();
  $breadcrumb[] = l(t('Home'), '<front>');
  $breadcrumb[] = l(t('Content'), 'admin/content');
  $breadcrumb[] = l(t('SmartDocs'), 'admin/content/smartdocs/models');
  $breadcrumb[] = l(t('@model', array('@model' => $model->getDisplayName())), 'admin/content/smartdocs/models/' . $model->getUuid());
  drupal_set_breadcrumb($breadcrumb);

  $form['model'] = array(
    '#type' => 'value',
    '#value' => $model->toArray(),
  );
  $ques = t('Are you sure you want to delete the model %name?', array('%name' => $model->getName()));
  $desc = t('Deleting the model %name will delete the all content from Dev Portal and remove the model from the Edge Management server.  This cannot be undone.  If you want make this documentation unavailable to end users, unpublish the nodes instead.', array('%name' => $model->getName()));
  return confirm_form($form, $ques, 'admin/content/smartdocs/models/' . $model->getUuid(), $desc, t('Delete'), t('Cancel'));
}

/**
 * Deletes a model and all its dependent data.
 *
 * @param array $form
 *   Descriptor of the form being submitted.
 * @param array $form_state
 *   State of the form being submitted.
 */
function smartdocs_model_delete_submit(array $form, array &$form_state) {
  $model = new Model(devconnect_default_org_config());
  Model::fromArray($model, $form_state['values']['model']);
  $model_name = $model->getName();

  try {
    $model->delete();

    // Delete all nodes in this model.
    $model_terms = taxonomy_get_term_by_name($model_name, 'smartdocs_models');
    if (!empty($model_terms)) {
      $model_term = reset($model_terms);

      $query = new EntityFieldQuery();
      $result = $query->entityCondition('entity_type', 'node')
        ->entityCondition('bundle', 'smart_method')
        ->fieldCondition('field_smart_method_model', 'tid', $model_term->tid, '=')
        ->execute();
      $nids = array_keys($result['node']);

      if (!empty($nids)) {
        db_delete('smartdata')
          ->condition('nid', $nids)
          ->execute();
        node_delete_multiple($nids);
      }

      taxonomy_term_delete($model_term->tid);
    }

    menu_rebuild();
    drupal_flush_all_caches();
    drupal_set_message(t('%model has been deleted along with all of its associated content.', array('%model' => $model_name)), 'status');
    $form_state['redirect'] = 'admin/content/smartdocs/models';
  }
  catch (Exception $e) {
    drupal_set_message('There was an error deleting the model(s).', 'error');
  }
}

/**
 * Page callback to display list of revisions for a given model.
 *
 * @param Model $model
 *   The Model containing the revisions to be listed.
 *
 * @return mixed
 *   HTML string or render-array.
 */
function smartdocs_revision_list(Model $model) {
  try {
    $revision_obj = new Revision($model->getConfig(), $model->getUuid());
    $revisions = $revision_obj->listRevisions();
  }
  catch (ResponseException $e) {
    return array(
      'error' => array(
        '#type' => 'markup',
        '#markup' => t('An error occurred while fetching all revisions for model @model.', array('@model' => $model->getDisplayName())),
        '#prefix' => '<div class="message error">',
        '#suffix' => '</div>',
      ),
    );
  }

  drupal_add_css(drupal_get_path('module', 'views') . '/css/views-admin.ctools.css');

  $breadcrumb = array();
  $breadcrumb[] = l(t('Home'), '<front>');
  $breadcrumb[] = l(t('Content'), 'admin/content');
  $breadcrumb[] = l(t('SmartDocs'), 'admin/content/smartdocs/models');
  $breadcrumb[] = l(t('@model', array('@model' => $model->getDisplayName())), 'admin/content/smartdocs/models/' . $model->getUuid());
  drupal_set_breadcrumb($breadcrumb);

  $headers = array(
    t('Revision'),
    t('Base URL'),
    t('Created'),
    t('Operations'),
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
    '/security' => t('Security Settings'),
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
            'views-ajax-link',
          ),
        ),
      );
    }
    if($op == 'Security Settings') {
      // Removed 'models' from URL since menu routing can only be 9 items.
      // See MENU_MAX_PARTS in menu.inc.
      $links[$key . '-' . $revision->getUuid()]['href'] = 'admin/content/smartdocs/' . $model->getUuid() . '/revisions/' . $revision->getRevisionNumber() . $key;
    }

    $caption = 'Revision #@rev';
    if ($is_latest_revision) {
      $caption .= ' (latest)';
      $is_latest_revision = FALSE;
    }

    $rows[] = array(
      l(t($caption, array('@rev' => $revision->getRevisionNumber())), 'admin/content/smartdocs/models/' . $model->getUuid() . '/revisions/' . $revision->getRevisionNumber()),
      $revision->getBaseUrl(),
      format_date($revision->getCreatedTime(), 'custom', 'j F Y'),
      theme('links__ctools_dropbutton', array('links' => $links)),
    );
  }

  return theme('table', array('header' => $headers, 'rows' => $rows));
}


/**
 * Page Callback for admin/content/smartdocs/model/%name.
 *
 * @param array $form
 *   Descriptor of the form being submitted.
 * @param array $form_state
 *   State of the form being submitted.
 * @param Model $model
 *   The Model to which a new revision is to be added.
 */
function smartdocs_add_revision(array $form, array $form_state, Model $model) {
  drupal_set_title(t('Add Revision to ‘@model’', array('@model' => $model->getDisplayName())));
  $breadcrumb = array();
  $breadcrumb[] = l(t('Home'), '<front>');
  $breadcrumb[] = l(t('Content'), 'admin/content');
  $breadcrumb[] = l(t('SmartDocs'), 'admin/content/smartdocs/models');
  $breadcrumb[] = l(t('@model', array('@model' => $model->getDisplayName())), 'admin/content/smartdocs/models/' . $model->getUuid());
  drupal_set_breadcrumb($breadcrumb);

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
    ),
  );
  $form['version'] = array(
    '#type' => 'textfield',
    '#title' => t('Version ID'),
    '#size' => 60,
    '#maxlength' => 128,
    '#required' => TRUE,
    '#attributes' => array(
      'placeholder' => t('v1'),
    ),
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
  // TODO: add more fields.
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Add Revision'),
  );
  return $form;
}

/**
 * Validation Callback for smartdocs_add_revision.
 *
 * @param array $form
 *   Descriptor of the form being validated.
 * @param array $form_state
 *   State of the form being validated.
 */
function smartdocs_add_revision_validate($form, &$form_state) {
  if (!(filter_var($form_state['values']['base_url'], FILTER_VALIDATE_URL))) {
    form_set_error('base_url', 'Base URL needs to be a valid URL.');
  }
}

/**
 * Submit Callback for smartdocs_add_revision.
 *
 * @param array $form
 *   Descriptor of the form being submitted.
 * @param array $form_state
 *   State of the form being submitted.
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
    $form_state['redirect'] = 'admin/content/smartdocs/models/' . $model->getUuid() . '/revisions/' . $revision->getRevisionNumber();
  }
  catch (Exception $e) {
    drupal_set_message(t('There was a problem creating the new revision. Please check the logs.'), 'error');
  }
}

/**
 * Page Callback for admin/content/smartdocs/model/%smartdocs_model/revisions/%.
 *
 * @param array $form
 *   Descriptor of the form being created.
 * @param array $form_state
 *   State of the form being created.
 * @param Model $model
 *   Model containing the revision to be edited.
 * @param Revision $revision
 *   The Revision to be edited.
 *
 * @return array
 *   Descriptor of the newly-created form.
 */
function smartdocs_revision_detail(array $form, array &$form_state, Model $model, Revision $revision) {

  $form['#attached'] = array(
    'css' => array(
      drupal_get_path('module', 'views') . '/css/views-admin.ctools.css',
      drupal_get_path('module', 'smartdocs') . '/css/smartdocs.css',
      drupal_get_path('theme', 'apigee_responsive') . '/css/bootstrap.min.css',
    ),
    'js' => array(
      drupal_get_path('module', 'smartdocs') . '/js/check-uncheck-all.js',
      'misc/tableselect.js',
    ),
    'library' => array(
      array('system', 'drupal.collapse'),
    ),
  );
  $rev_number = $revision->getRevisionNumber();

  try {
    $security = new Security(devconnect_default_org_config(), $model->getUuid(), $revision->getUuid());
    $security_schemes = $security->loadAllSchemes();
  }
  catch (ResponseException $e) {
    $security_schemes = array();
  }
  $security_scheme_display = array();
  /** @var Apigee\SmartDocs\Security\SecurityScheme $security_scheme */
  foreach ($security_schemes as $security_scheme) {
    $security_scheme_display[$security_scheme->getName()] = $security_scheme->getName() . ' (' . $security_scheme->getType(TRUE) . ')';
  }

  // TODO: find way to move these page-specific items out of form constructor.
  drupal_set_title(t('‘@model’ API Revision #@rev', array('@model' => $model->getDisplayName(), '@rev' => $rev_number)));
  $breadcrumb = array();
  $breadcrumb[] = l(t('Home'), '<front>');
  $breadcrumb[] = l(t('Content'), 'admin/content');
  $breadcrumb[] = l(t('SmartDocs'), 'admin/content/smartdocs/models');
  $breadcrumb[] = l(t('@model', array('@model' => $model->getDisplayName())), 'admin/content/smartdocs/models/' . $model->getUuid());
  $breadcrumb[] = l(t('Revisions'), 'admin/content/smartdocs/models/' . $model->getUuid() . '/revisions');
  $breadcrumb[] = l(t('Revision #@rev', array('@rev' => $rev_number)), 'admin/content/smartdocs/models/' . $model->getUuid() . '/revisions/' . $rev_number);
  drupal_set_breadcrumb($breadcrumb);

  $nids = db_select('smartdata', 's')
    ->condition('model', $model->getName())
    ->fields('s', array('nid'))
    ->execute();
  $nodes = array();
  foreach ($nids as $nid) {
    $nodes[$nid->nid] = $model;
  }

  $operations_options = array();
  // Set default behavior based on configuration preference.
  if (variable_get('smartdocs_publish_on_render', 1)) {
    $operations_options['render_publish_nodes'] = t('Render and publish nodes');
    $operations_options['render_nodes'] = t('Render nodes');
  }
  else {
    $operations_options['render_nodes'] = t('Render nodes');
    $operations_options['render_publish_nodes'] = t('Render and publish nodes');
  }
  if (!empty($nodes)) {
    $operations_options['publish_nodes'] = t('Publish nodes');
    $operations_options['unpublish_nodes'] = t('Unpublish nodes');
  }

  $operations = array(
    'resource/add/' . $rev_number => 'Add Resource',
    'revisions/' . $rev_number . '/security' => 'Security Settings',
    'export/' . $rev_number => 'Export',
    'revisions' => 'View All Revisions',
    'revisions/' . $rev_number . '/edit' => 'Edit API Revision',
  );
  $links = array();
  foreach ($operations as $key => $op) {
    $links[$key] = array(
      'title' => $op,
      'attributes' => array(
        'title' => $op,
        'class' => array('views-ajax-link'),
      ),
    );

    if($op == 'Security Settings') {
      // Removed 'models' from URL since menu routing can only be 9 items.
      // See MENU_MAX_PARTS in menu.inc.
      $links[$key]['href'] = 'admin/content/smartdocs/' . $model->getUuid() . '/' . $key;
    }
    else {
      $links[$key]['href'] = 'admin/content/smartdocs/models/' . $model->getUuid() . '/' . $key;
    }
  }
  $form['revision_tools'] = array(
    '#markup' => theme('links__ctools_dropbutton', array('links' => $links)),
    '#suffix' => '<br><br>',
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
      ),
    ),
    '#attributes' => array('style' => 'margin-left:1em'),
    'render_markup' => array(
      '#type' => 'markup',
      '#markup' => t("Rendering nodes builds pages in Dev Portal from Edge's generated SmartDocs HTML. Publishing makes the pages viewable to developers."),
    ),
  );

  $form['options']['checkall'] = array(
    '#type' => 'checkbox',
    '#title' => t('Select / Unselect all'),
    '#attributes' => array('onclick' => 'checkUncheckAll(this);'),
    '#weight' => 10,
    '#prefix' => '<br/><br/>',
  );

  if (!empty($nodes)) {
    $form['options']['publish_desc'] = array(
      '#type' => 'container',
      '#states' => array(
        'visible' => array(
          'select[name="operation"]' => array('value' => 'publish_nodes'),
        ),
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
        ),
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

  // Get total of published methods.
  $published_count = 0;
  $method_count = 0;

  if ($rev_number) {
    $entity['displayName'] = $model->getDisplayName();
    $entity['name'] = $model->getName();

    $resources = $revision->getResources();
    if (empty($resources)) {
      unset($form['options']);
      $form['resources'] = array(
        '#markup' => t('There is currently no data in the model.'),
      );
      return $form;
    }
    $form['resources'] = array();
    /** @var Apigee\SmartDocs\Resource $resource */
    foreach ($resources as $resource) {
      $resource_id = $resource->getUuid();
      $links = array();
      $links['add-' . $resource_id] = array(
        'title' => t('Add method'),
        'href' => 'node/add/smart-method',
        'query' => array(
          'model' => $model->getName(),
          'revision' => $revision->getRevisionNumber(),
          'resource' => $resource_id,
        ),
        'attributes' => array(
          'title' => t('Add method'),
          'class' => array(
            'add-' . $resource_id,
            'views-ajax-link',
          ),
        ),
      );
      $links['edit-' . $resource_id] = array(
        'title' => t('Edit resource'),
        'href' => 'admin/content/smartdocs/models/' . $model->getUuid() . '/resource/edit/' . $rev_number . '/' . $resource_id,
        'attributes' => array(
          'title' => t('Edit resource'),
          'class' => array(
            'edit-' . $resource_id,
            'views-ajax-link',
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
        $method_count++;

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
          $edit_method_url = 'node/' . $row['nid'] . '/edit';
        }

        if ($has_node && $published) {
          $published_count++;
        }

        $ops = array();
        if ($has_node) {
          $ops['edit-method-' . $method->getUuid()] = array(
            'title' => t('Edit method'),
            'href' => $edit_method_url,
            'attributes' => array(
              'title' => t('Edit method'),
              'class' => array(
                'edit-method-' . $method->getUuid(),
                'views-ajax-link',
              ),
            ),
          );

          $op_title = $published ? t('Unpublish node') : t('Publish node');

          $ops['toggle-status-method-' . $method->getUuid()] = array(
            'title' => $op_title,
            'href' => 'admin/content/smartdocs/models/' . $model->getUuid() . '/method/toggle-status/' . $rev_number . '/' . $resource->getUuid() . '/' . $method->getUuid(),
            'attributes' => array(
              'title' => $op_title,
              'class' => array(
                'toggle-status-method-' . $method->getUuid(),
                'views-ajax-link',
              ),
            ),
          );
        }
        $ops['delete-method-' . $method->getUuid()] = array(
          'title' => t('Delete method'),
          'href' => 'admin/content/smartdocs/models/' . $model->getUuid() . '/method/delete/' . $rev_number . '/' . $resource->getUuid() . '/' . $method->getUuid(),
          'attributes' => array(
            'title' => t('Delete method'),
            'class' => array(
              'delete-method-' . $method->getUuid(),
              'views-ajax-link',
            ),
          ),
        );

        $auth = array();
        foreach ($method->getSecurity() as $security_scheme_name) {
          if (array_key_exists($security_scheme_name, $security_scheme_display)) {
            $auth[] = $security_scheme_display[$security_scheme_name];
          }
        }

        $resource_set['methods'][$method->getUuid()] = array(
          '#type' => 'checkbox',
          '#return_value' => $method->getUuid(),
          '#data' => array(
            'tax' => (!empty($tags) ? implode(', ', $tags) : ''),
            'name' => check_plain($method->getDisplayName()),
            'description' => filter_xss($method->getDescription()),
            'method' => $method->getVerb(),
            'auth' => implode(', ', $auth),
            'node' => ($has_node ? $link : '<span style="color:red;">No</span>'),
            'status' => ($published ? '<span style="color:green;">Yes</span>' : '<span style="color:red;">No</span>'),
            'synced' => ($synced ? '<span style="color:green;">Yes</span>' : '<span style="color:red;">No</span>'),
            'operations' => theme('links__ctools_dropbutton', array('links' => $ops)),
          ),
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

  if ($published_count && $view_path = _smartdocs_get_revision_view_path($model)) {
    $count_args = array('%published_count' => $published_count, '%total_count' => $method_count);
    $published_docs_message = l(t('View API Documentation'), $view_path)
      . ' '
      . t('(%published_count out of %total_count methods published)', $count_args);

  }
  else {
    $published_docs_message = t('No documentation created. Render and publish one or more methods to view documentation.');
  }
  $form['published_docs_link'] = array(
    '#markup' => $published_docs_message,
    '#prefix' => '<div class="smartdocs-revision-view-published-docs">',
    '#suffix' => '</div><br><br>',
  );

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
 * Validates the revision detail form.
 *
 * @param array $form
 *   Descriptor of the form being submitted.
 * @param array $form_state
 *   State of the form being submitted.
 */
function smartdocs_revision_detail_validate($form, &$form_state) {
  $selection = array_filter($form_state['values']['methods']);
  if (empty($selection)) {
    form_set_error('', "Please select at least one method to render.");
  }
}

/**
 * Resource, Method table submit.
 *
 * @param array $form
 *   Descriptor of the form being submitted.
 * @param array $form_state
 *   State of the form being submitted.
 */
function smartdocs_revision_detail_submit($form, &$form_state) {
  $model = new Model(devconnect_default_org_config());
  Model::fromArray($model, $form_state['values']['model']);

  switch ($form_state['values']['operation']) {
    case 'render_nodes':
    case 'render_publish_nodes':
      $revision = new Revision($model->getConfig(), $model->getUuid());
      Revision::fromArray($revision, $form_state['values']['revision']);
      $selected = $form_state['values']['methods'];
      $options = array('publish' => ($form_state['values']['operation'] == 'render_publish_nodes'));
      module_load_include('inc', 'smartdocs', 'batch/smartdocs.render');
      batch_set(smartdocs_render($model, $revision, $selected, $options));
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
  cache_clear_all('model:' . $model->getUuid(), 'cache_smartdocs');
}

/**
 * Attempts to set a status (zero or one) on a SmartDocs node.
 *
 * @param string $method_uuid
 *   UUID (from the Modeling API, not from Drupal) of the method to be altered.
 * @param int $status
 *   Zero to unpublish, or one to publish.
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
 * Edit Resource Form.
 *
 * @param array $form
 *   Descriptor of the form being created.
 * @param array $form_state
 *   State of the form being created.
 * @param Apigee\SmartDocs\Model $model
 *   Model to which the resource belongs.
 * @param Apigee\SmartDocs\Revision $revision
 *   Revision to which the resource belongs.
 * @param Apigee\SmartDocs\Resource|null $resource
 *   Resource being edited, or NULL if a new resource is being created.
 *
 * @return array
 *   The newly-created form.
 */
function smartdocs_resource_edit(array $form, array &$form_state, Model $model, Revision $revision, Resource $resource = NULL) {

  // TODO: find way to move these page-specific items out of form constructor.
  if (isset($resource)) {
    drupal_set_title(t('Edit resource from ‘@model’', array('@model' => $model->getDisplayName())));
  }
  else {
    drupal_set_title(t('Add resource to ‘@model’', array('@model' => $model->getDisplayName())));
  }
  $breadcrumb = array();
  $breadcrumb[] = l(t('Home'), '<front>');
  $breadcrumb[] = l(t('Content'), 'admin/content');
  $breadcrumb[] = l(t('SmartDocs'), 'admin/content/smartdocs/models');
  $breadcrumb[] = l($model->getDisplayName(), 'admin/content/smartdocs/models/' . $model->getUuid());
  $breadcrumb[] = l(t('API Revisions'), 'admin/content/smartdocs/models/' . $model->getUuid() . '/revisions/');
  $breadcrumb[] = l(t('Revision #!revision', array('!revision' => $revision->getRevisionNumber())), 'admin/content/smartdocs/models/' . $model->getUuid() . '/revisions/' . $revision->getRevisionNumber());
  drupal_set_breadcrumb($breadcrumb);

  $version = variable_get('local_asset_version', '6');
  $my_path = drupal_get_path('module', 'smartdocs');
  $form['#attached'] = array(
    'js' => array(
      $my_path . '/local/js/v' . $version . '/codemirror.js',
      $my_path . '/local/js/v' . $version . '/codemirror_javascript.js',
      array(
        'data' => array('smartdocs' => array('parameters' => 'parameters')),
        'type' => 'setting',
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
    '#value' => isset($resource) ? t('Save Resource') : t('Add Resource'),
  );
  return $form;
}


/**
 * Validator for smartdocs_resource_edit() form.
 *
 * @param array $form
 *   Descriptor of the form being validated.
 * @param array $form_state
 *   State of the form being validated.
 */
function smartdocs_resource_edit_validate($form, &$form_state) {
  if (!empty($form_state['values']['parameters'])) {
    if (smartdocs_json_validate($form_state['values']['parameters'])) {
      form_set_error('parameters', t('Please specify proper JSON values for the parameters.'));
    }
  }
}

/**
 * Submit callback for Edit Resource form.
 *
 * @param array $form
 *   Descriptor of the form being submitted.
 * @param array $form_state
 *   State of the form being submitted.
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
    if ($update) {
      drupal_set_message(t('Resource has been updated successfully.'));
    }
    else {
      drupal_set_message(t('Resource has been created successfully.'));
    }
    cache_clear_all('model:' . $model->getUuid(), 'cache_smartdocs');
    module_invoke_all('smartdocs_model_update', $model->getUuid());
    $form_state['redirect'] = 'admin/content/smartdocs/models/' . $model->getUuid() . '/revisions/' . $revision->getRevisionNumber();
  }
  catch (Exception $e) {
    drupal_set_message(t('Resource failed to save, please consult the logs.'), 'error');
  }
}

/**
 * Page callback to publish unpublished method nodes, and vice versa.
 *
 * @param Model $model
 *   The Model to which the node belongs.
 * @param Revision $revision
 *   The Revision to which the node belongs.
 * @param Resource $resource
 *   The Resource to which the node belongs.
 * @param Method $method
 *   The Method corresponding to the node whose status is to be toggled.
 */
function smartdocs_toggle_node_status(Model $model, Revision $revision, Resource $resource, Method $method) {
  $nid = _smartdocs_get_method_node($method);
  if ($nid) {
    $node = node_load($nid);
    if ($node) {
      $node->status = 1 - $node->status;
      node_save($node);
      drupal_set_message(t('The node associated with the method ‘@method’ has been @action.', array(
        '@method' => $method->getDisplayName(),
        '@action' => $node->status ? t('published') : t('unpublished'),
      )));
    }
    else {
      drupal_set_message(t('The node associated with method ‘@method’ could not be found.', array('@method' => $method->getDisplayName())), 'error');
    }
  }
  else {
    drupal_set_message(t('No node is associated with method ‘@method’.', array('@method' => $method->getDisplayName())), 'error');
  }
  drupal_goto('admin/content/smartdocs/models/' . $model->getUuid() . '/revisions/' . $revision->getRevisionNumber());
}

/**
 * Form constructor confirming deletion of a method.
 *
 * @param array $form
 *   Descriptor of the form being created.
 * @param array $form_state
 *   State of the form being created.
 * @param Model $model
 *   Model from which a method will be deleted.
 * @param Revision $revision
 *   Revision from which a method will be deleted.
 * @param Resource $resource
 *   Resource from which a method will be deleted.
 * @param Method $method
 *   The method which is to be deleted.
 *
 * @return array
 *   The form structure to be presented to the user.
 */
function smartdocs_method_delete(array $form, array &$form_state, Model $model, Revision $revision, Resource $resource, Method $method) {
  // TODO: find way to move these page-specific items out of form constructor.
  drupal_set_title(t('About to delete method ‘@method’ of ‘@model’ model', array(
    '@method' => $method->getDisplayName(),
    '@model' => $model->getDisplayName(),
  )));

  $breadcrumb = array();
  $breadcrumb[] = l(t('Home'), '<front>');
  $breadcrumb[] = l(t('Administration'), 'admin');
  $breadcrumb[] = l(t('SmartDocs'), 'admin/content/smartdocs/models');
  $breadcrumb[] = l($model->getDisplayName(), 'admin/content/smartdocs/models/' . $model->getUuid());
  $breadcrumb[] = l(t('API Revisions'), 'admin/content/smartdocs/models/' . $model->getUuid() . '/revisions/');
  $breadcrumb[] = l(t('Revision #@revision', array('@revision' => $revision->getRevisionNumber())), 'admin/content/smartdocs/models/' . $model->getUuid() . '/revisions/' . $revision->getRevisionNumber());
  drupal_set_breadcrumb($breadcrumb);

  $form['revision_url'] = array(
    '#type' => 'value',
    '#value' => 'admin/content/smartdocs/models/' . $model->getUuid() . '/revisions/' . $revision->getRevisionNumber(),
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
    '#value' => NULL,
  );
  $form['warning'] = array(
    '#type' => 'markup',
    '#markup' => '<p><strong>' . t('Warning:') . '</strong> ' . t('You are about to remove the method ‘@method’ from the Modeling API.', array('@method' => $method->getDisplayName())) . '</p>',
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
      '#submit' => array('smartdocs_method_delete_cancel'),
    ),
    'ok' => array(
      '#type' => 'submit',
      '#value' => t('Delete this method'),
      '#submit' => array('smartdocs_method_delete_submit'),
    ),
  );

  return $form;
}

/**
 * Cancel action for method deletion form.
 *
 * @param array $form
 *   Descriptor of the form being submitted.
 * @param array $form_state
 *   State of the form being submitted.
 */
function smartdocs_method_delete_cancel($form, &$form_state) {
  $form_state['redirect'] = $form_state['values']['revision_url'];
}

/**
 * Submit action for method deletion form.
 *
 * @param array $form
 *   Descriptor of the form being submitted.
 * @param array $form_state
 *   State of the form being submitted.
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
  }
  catch (ResponseException $e) {
    drupal_set_message(t('Deleting method ‘@method’ failed. Please check the logs.', array('@method' => $method_name)), 'error');
  }
  $form_state['redirect'] = $form_state['values']['revision_url'];
}

/**
 * Form for adding security scheme to a model.
 *
 * @param array $form
 *   The form array.
 * @param array $form_state
 *   The form state array.
 * @param \Apigee\SmartDocs\Model $model
 *   The model object to add the scheme for.
 * @param \Apigee\SmartDocs\Revision $revision
 *   The revision object of the model to add the scheme for.
 * @param string $scheme_name
 *   The name of the security scheme, this is passed in while editing a scheme.
 *
 * @return array
 *   The form array.
 */
function smartdocs_model_security_scheme(array $form, array &$form_state, Model $model, Revision $revision, $scheme_name = NULL) {
  $form['model'] = array(
    '#type' => 'value',
    '#value' => $model->toArray(),
  );
  $form['revision'] = array(
    '#type' => 'value',
    '#value' => $revision->toArray(),
  );
  $form['#attached'][] = 'misc/machine-name.js';

  $type = !empty($form_state['values']['type']) ? $form_state['values']['type'] : NULL;

  // If we are editing a scheme then we set the type to the type of the scheme
  // being edited.
  if (!empty($scheme_name)) {
    $security = new Security(devconnect_default_org_config(), $model->getUuid(), $revision->getUuid());
    $scheme = $security->load($scheme_name);
    $type = $scheme->getType();
  }

  $form['scheme_action'] = array(
    '#type' => 'value',
    '#value' => is_null($scheme_name) ? 'Add' : 'Update',
  );

  $form['display_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#size' => 60,
    '#maxlength' => 128,
    '#required' => TRUE,
    '#default_value' => isset($scheme) ? $scheme->getName() : '',
    '#disabled' => isset($scheme) ? TRUE : FALSE,
  );
  $form['scheme_name'] = array(
    '#type' => 'machine_name',
    '#title' => t('Internal name'),
    '#description' => t('A unique machine-readable name. Can only contain letters, digits, underscores and hyphens.'),
    '#machine_name' => array(
      'exists' => 'smartdocs_model_security_scheme_exists',
      'source' => array('display_name'),
      'label' => t('Internal name'),
      'replace_pattern' => '[^A-Za-z0-9_-]+',
      'replace' => '-',
      'error' => 'The internal name must contain only letters, digits, underscores, or hyphens.',
    ),
    '#required' => TRUE,
    '#access' => TRUE,
    '#default_value' => isset($scheme) ? $scheme->getName() : '',
    '#disabled' => isset($scheme) ? TRUE : FALSE,
  );
  $form['type'] = array(
    '#type' => 'select',
    '#title' => t('Type'),
    '#options' => array(
      'OAUTH2' => t('OAuth 2.0'),
      'APIKEY' => t('Apikey'),
      'BASIC' => t('Basic'),
    ),
    '#ajax' => array(
      'callback' => 'smartdocs_model_security_ajax_callback',
      'wrapper' => 'auth-type-field-container',
    ),
    '#default_value' => isset($scheme) ? $scheme->getType() : 'OAUTH2',
    '#disabled' => isset($scheme) ? TRUE : FALSE,
  );
  $form['auth_fields'] = array(
    '#type' => 'container',
    '#prefix' => '<div id="auth-type-field-container">',
    '#suffix' => '</div>',
    '#tree' => TRUE,
  );
  $form['auth_fields']['advanced_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Advanced Settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#weight' => 10,
  );
  if (!empty($type)) {
    switch ($type) {
      case 'OAUTH2':
        $form['auth_fields']['grantType'] = array(
          '#type' => 'radios',
          '#title' => t('Grant Type'),
          '#options' => array(
            'IMPLICIT' => t('Implicit'),
            'PASSWORD' => t('Password'),
            'CLIENT_CREDENTIALS' => t('Client Credentials'),
            'AUTHORIZATION_CODE' => t('Authorization Code'),
          ),
          '#required' => TRUE,
          '#default_value' => isset($scheme) ? $scheme->getGrantType() : 'AUTHORIZATION_CODE',
        );
        $form['auth_fields']['authorizationUrl'] = array(
          '#type' => 'textfield',
          '#title' => t('Authorization URL'),
          '#required' => TRUE,
          '#default_value' => isset($scheme) ? $scheme->getAuthorizationUrl() : '',
        );
        $form['auth_fields']['accessTokenUrl'] = array(
          '#type' => 'textfield',
          '#title' => t('Access token url'),
          '#default_value' => isset($scheme) ? $scheme->getAccessTokenUrl() : '',
        );
        $form['auth_fields']['advanced_settings']['authorizationVerb'] = array(
          '#type' => 'radios',
          '#title' => t('Authorization verb'),
          '#options' => array(
            'GET' => t('GET'),
            'POST' => t('POST'),
          ),
          '#required' => TRUE,
          '#default_value' => !isset($scheme) ? $scheme->getAuthorizationVerb() : 'GET',
        );
        $form['auth_fields']['advanced_settings']['in'] = array(
          '#type' => 'select',
          '#title' => t('Authorization Request Method'),
          '#options' => array(
            'HEADER' => t('Header'),
            'QUERY' => t('Query'),
          ),
          '#description' => t('How the access token will be passed when using the send request feature.'),
          '#required' => TRUE,
          '#default_value' => isset($scheme) ? $scheme->getIn() : 'HEADER',
        );
        $form['auth_fields']['advanced_settings']['accessTokenParamName'] = array(
          '#type' => 'textfield',
          '#title' => t('Access token parameter name'),
          '#default_value' => isset($scheme) ? $scheme->getAccessTokenParamName() : 'access_token',
          '#description' => t('Override the OAuth specification\'s "access_token" name in the query string (non-standard).'),
          '#states' => array(
            'visible' => array(
              ':input[name="auth_fields[advanced_settings][in]"]' => array('value' => 'QUERY'),
            ),
          ),
        );
        $form['auth_fields']['advanced_settings']['clientAuthenticationMethod'] = array(
          '#type' => 'select',
          '#title' => t('When making the call to get an access token, pass the client id and secret in the:'),
          '#description' => t('How the client authenticates with the authorization server.'),
          '#options' => array(
            'HEADER' => t('Header'),
            'BODY' => t('Body'),
            'NONE' => t('None'),
          ),
        );
        $form['auth_fields']['advanced_settings']['scopes'] = array(
          '#type' => 'textarea',
          '#title' => t('Scopes'),
          '#default_value' => isset($scheme) ? json_encode((object) $scheme->getScopes()) : '{}',
          '#description' => t('Please enter a JSON object of scopes, e.g:
                            {
                              "write:pets": "modify pets in your account",
                              "read:pets": "read your pets"
                            }'),
        );
        break;

      case 'APIKEY':
        unset($form['auth_fields']['advanced_settings']);
        $form['auth_fields']['paramName'] = array(
          '#type' => 'textfield',
          '#title' => t('Param Name'),
          '#required' => TRUE,
          '#default_value' => isset($scheme) ? $scheme->getParamName() : '',
        );
        $form['auth_fields']['in'] = array(
          '#type' => 'select',
          '#title' => t('In'),
          '#options' => array(
            'HEADER' => t('Header'),
            'QUERY' => t('Query'),
            'BODY' => t('Body'),
          ),
          '#required' => TRUE,
          '#default_value' => isset($scheme) ? $scheme->getIn() : 'HEADER',
        );
        break;

      case 'BASIC':
        unset($form['auth_fields']['advanced_settings']);
        break;
    }
  }
  else {
    $form['auth_fields']['grantType'] = array(
      '#type' => 'radios',
      '#title' => t('Grant Type'),
      '#options' => array(
        'IMPLICIT' => t('Implicit'),
        'PASSWORD' => t('Password'),
        'CLIENT_CREDENTIALS' => t('Client Credentials'),
        'AUTHORIZATION_CODE' => t('Authorization Code'),
      ),
      '#default_value' => 'AUTHORIZATION_CODE',
      '#required' => TRUE,
    );
    $form['auth_fields']['authorizationUrl'] = array(
      '#type' => 'textfield',
      '#title' => t('Authorization URL'),
      '#required' => TRUE,
    );
    $form['auth_fields']['accessTokenUrl'] = array(
      '#type' => 'textfield',
      '#title' => t('Access token url'),
    );
    $form['auth_fields']['advanced_settings']['authorizationVerb'] = array(
      '#type' => 'radios',
      '#title' => t('Authorization verb'),
      '#options' => array(
        'GET' => t('GET'),
        'POST' => t('POST'),
      ),
      '#required' => TRUE,
      '#default_value' => 'GET',
    );
    $form['auth_fields']['advanced_settings']['in'] = array(
      '#type' => 'select',
      '#title' => t('Authorization Request Method'),
      '#options' => array(
        'HEADER' => t('Header'),
        'QUERY' => t('Query'),
      ),
      '#description' => t('How the access token will be passed when using the send request feature.'),
      '#required' => TRUE,
      '#default_value' => 'HEADER',
    );
    $form['auth_fields']['advanced_settings']['accessTokenParamName'] = array(
      '#type' => 'textfield',
      '#title' => t('Access token parameter name'),
      '#default_value' => 'access_token',
      '#description' => t('Override the OAuth specification\'s "access_token" name in the query string (non-standard).'),
      '#states' => array(
        'visible' => array(
          ':input[name="auth_fields[advanced_settings][in]"]' => array('value' => 'QUERY'),
        ),
      ),
    );
    $form['auth_fields']['advanced_settings']['clientAuthenticationMethod'] = array(
      '#type' => 'select',
      '#title' => t('When making the call to get an access token, pass the client id and secret in the:'),
      '#description' => t('How the client authenticates with the authorization server.'),
      '#options' => array(
        'HEADER' => t('Header'),
        'BODY' => t('Body'),
        'NONE' => t('None'),
      ),
    );
    $form['auth_fields']['advanced_settings']['scopes'] = array(
      '#type' => 'textarea',
      '#title' => t('Scopes'),
      '#default_value' => '{}',
      '#description' => t('Please enter a JSON object of scopes, e.g:
                            {
                              "write:pets": "modify pets in your account",
                              "read:pets": "read your pets"
                            }'),
    );
  }
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
  );
  return $form;
}

/**
 * Machine name field callback; checks if scheme machine name already exists.
 *
 * @param string $name
 *   The machine name of the security scheme.
 *
 * @return bool
 *   TRUE if the machine name already exists, FALSE otherwise.
 */
function smartdocs_model_security_scheme_exists($name) {
  // We don't do the validation here as we don't have the model and the revision
  // available here. And to check the if the machine name exists for a scheme
  // we need to invoke and instantiate the security class that requires the
  // model and the revision.
  return FALSE;
}

/**
 * Ajax callback for Security scheme form; updates fields based on scheme type.
 *
 * @param array $form
 *   The security scheme form.
 * @param array $form_state
 *   The corresponding form state.
 *
 * @return array
 *   The subset of the form to be updated via AJAX.
 */
function smartdocs_model_security_ajax_callback($form, $form_state) {
  return $form['auth_fields'];
}

/**
 * Validate handler for the security scheme form.
 *
 * @param array $form
 *   The form being validated.
 * @param array $form_state
 *   The state of form being validated.
 */
function smartdocs_model_security_scheme_validate($form, &$form_state) {
  if ($form_state['values']['type'] == 'OAUTH2') {
    $scopes = $form_state['values']['auth_fields']['advanced_settings']['scopes'];
    $scope_object = json_decode($scopes);
    if ((!is_object($scope_object))) {
      form_set_error('auth_fields][advanced_settings][scopes', t('Please enter a valid JSON object.'));
    }
    elseif (is_object($scope_object)) {
      $vars = get_object_vars($scope_object);
      foreach ($vars as $scope_values) {
        if (!is_scalar($scope_values)) {
          form_set_error('auth_fields][advanced_settings][scopes', t('The Scopes JSON object should consist of key-value pairs. Each key and each value must be a string.'));
          break;
        }
      }
    }
  }
}

/**
 * Submit handler for security scheme form.
 *
 * @param array $form
 *   Form being submitted.
 * @param array $form_state
 *   State of form being submitted.
 */
function smartdocs_model_security_scheme_submit($form, &$form_state) {
  $model = new Model(devconnect_default_org_config());
  Model::fromArray($model, $form_state['values']['model']);
  $revision = new Revision($model->getConfig(), $model->getUuid());
  Revision::fromArray($revision, $form_state['values']['revision']);

  $type = $form_state['values']['type'];
  $security = new Security($model->getConfig(), $model->getUuid(), $revision->getUuid(), $type);
  $is_update = ($form_state['values']['scheme_action'] == 'Update') ? TRUE : FALSE;

  // Construct the payload depending on the type of the scheme.
  switch ($type) {
    case 'OAUTH2':
      $payload = $form_state['values']['auth_fields'];
      unset($payload['advanced_settings']);
      $payload += $form_state['values']['auth_fields']['advanced_settings'];
      $payload['name'] = $form_state['values']['scheme_name'];
      $payload['type'] = $type;
      $payload['scopes'] = json_decode($payload['scopes'], TRUE);
      break;

    case 'APIKEY':
      $payload = $form_state['values']['auth_fields'];
      $payload['name'] = $form_state['values']['scheme_name'];
      $payload['type'] = $type;
      break;

    case 'BASIC':
      $payload = array();
      $payload['name'] = $form_state['values']['scheme_name'];
      $payload['type'] = $type;
      break;
  }
  try {
    $scheme = SecurityScheme::fromArray($payload);
    $security->save($scheme, $is_update);
    drupal_set_message(t("The security scheme has been saved"), "status");
  }
  catch (Exception $e) {
    drupal_set_message(t('There was an error saving the security scheme.'));
  }
  drupal_goto('admin/content/smartdocs/' . $model->getUuid() . '/revisions/' . $revision->getUuid() . '/security');
}

/**
 * Page callback for the security scheme list page.
 *
 * @param \Apigee\SmartDocs\Model $model
 *   The model to list the security schemes for.
 * @param \Apigee\SmartDocs\Revision $revision
 *   The revision of the model to list the security scheme for.
 *
 * @return string
 *   Themed table of security schemes for the given model and revision.
 */
function smartdocs_security_scheme_list(Model $model, Revision $revision) {

  $rev = $revision->getRevisionNumber();
  drupal_set_title(t('Security Settings for API Revision #@rev', array('@rev' => $rev)));

  $breadcrumb = array();
  $breadcrumb[] = l(t('Home'), '<front>');
  $breadcrumb[] = l(t('Content'), 'admin/content');
  $breadcrumb[] = l(t('SmartDocs'), 'admin/content/smartdocs/models');
  $breadcrumb[] = l(t('@model', array('@model' => $model->getDisplayName())), 'admin/content/smartdocs/models/' . $model->getUuid());
  $breadcrumb[] = l(t('Revisions'), 'admin/content/smartdocs/models/' . $model->getUuid() . '/revisions');
  $breadcrumb[] = l(t('Revision #@rev', array('@rev' => $rev)), 'admin/content/smartdocs/models/' . $model->getUuid() . '/revisions/' . $rev);
  drupal_set_breadcrumb($breadcrumb);

  drupal_add_css(drupal_get_path('module', 'views') . '/css/views-admin.ctools.css');
  $output['add_scheme'] = array(
    '#type' => 'link',
    '#href' => 'admin/content/smartdocs/' . $model->getUuid() . '/revisions/' . $revision->getUuid() . '/security/add',
    '#title' => t('Add Security Scheme'),
    '#prefix' => '<div class="templateauth-help" style="margin-bottom: 1.5em">',
    '#suffix' => '</div>',
  );
  $settings_url = url('admin/content/smartdocs/models/' . $model->getUuid() . '/settings', array('fragment' => 'template_auth_settings'));
  $output['template_auth_link'] = array(
    '#markup' => t('To manage the template auth schemes, go to the <a href="@url">template auth settings</a>.', array('@url' => $settings_url)),
    '#prefix' => '<div class="templateauth-help">',
    '#suffix' => '</div>',
  );

  $security = new Security(devconnect_default_org_config(), $model->getUuid(), $revision->getUuid());
  $schemes = $security->loadAllSchemes();
  $rows = array();
  $header = array(
    t('Name'),
    t('Type'),
    t('Operations'),
  );
  foreach ($schemes as $scheme) {
    $operations = array(
      'edit' => 'Edit',
      'delete' => 'Delete',
    );
    foreach ($operations as $op => $disp) {
      $ops['list-' . $op] = array(
        'title' => $disp,
        'href' => 'admin/content/smartdocs/' . $model->getUuid() . '/revisions/' . $revision->getUuid() . '/security/' . $scheme->getName() . '/' . $op,
        'attributes' => array(
          'title' => $disp,
          'class' => array($op . '-' . $scheme->getName(), 'views-ajax-link'),
        ),
      );
    }

    $rows[] = array(
      $scheme->getName(),
      $scheme->getType(),
      theme('links__ctools_dropbutton', array('links' => $ops)),
    );
  }

  $vars = array(
    'header' => $header,
    'rows' => $rows,
    'empty' => t('No Security scheme available'),
  );
  $output['table'] = array(
    '#markup' => theme('table', $vars),
  );
  return $output;
}

/**
 * Form API callback: Confirmation form for deleting a security sheme.
 *
 * @param array $form
 *   The form array.
 * @param array $form_state
 *   The form state array.
 * @param \Apigee\SmartDocs\Model $model
 *   The model for which the scheme is being deleted.
 * @param \Apigee\SmartDocs\Revision $revision
 *   The revision of the model for which the scheme is being deleted.
 * @param string $scheme_name
 *   The name of the scheme to be deleted.
 *
 * @return array
 *   The confirmation form array.
 */
function smartdocs_model_security_delete($form, &$form_state, Model $model, Revision $revision, $scheme_name) {

  $form['model_uuid'] = array(
    '#type' => 'value',
    '#value' => $model->getUuid(),
  );
  $form['revision_uuid'] = array(
    '#type' => 'value',
    '#value' => $revision->getUuid(),
  );
  $form['scheme_name'] = array(
    '#type' => 'value',
    '#value' => $scheme_name,
  );
  $form['delete_templateauth'] = array(
    '#type' => 'checkbox',
    '#title' => t('Delete associated template auth scheme?'),
    '#description' => t('Selecting this option will delete the associated template auth scheme with name matching the name of this security scheme.'),
    '#weight' => 5,
  );
  $path = 'admin/content/smartdocs/' . $model->getUuid() . '/revisions/' . $revision->getUuid() . '/security';
  $description = t('You will be deleting this security scheme permanently and this action cannot be undone.');

  return confirm_form($form, t('Are you sure, that you want to delete the security scheme !scheme_name', array('!scheme_name' => $scheme_name)), $path, $description, t('Delete'));
}

/**
 * Form API callback: handles submit of security scheme delete confirm form.
 *
 * @param array $form
 *   The form array.
 * @param array $form_state
 *   The form state array.
 */
function smartdocs_model_security_delete_submit($form, &$form_state) {
  $model_id = $form_state['values']['model_uuid'];
  $revision_id = $form_state['values']['revision_uuid'];
  $scheme_name = $form_state['values']['scheme_name'];
  $delete_templateauth = $form_state['values']['delete_templateauth'];

  $security = new Security(devconnect_default_org_config(), $model_id, $revision_id);
  try{
    $security->delete($scheme_name);
    if ($delete_templateauth) {
      $templateauth = new TemplateAuth(devconnect_default_org_config(), $model_id);
      $templateauth->delete($scheme_name);
    }
    drupal_set_message(t('Successfully deleted scheme !scheme_name', array('!scheme_name' => $scheme_name)));
  }
  catch (Exception $e) {
    drupal_set_message(t('An error occured when trying to delete the scheme !scheme_name', array('!scheme_name' => $scheme_name)), 'error');
  }
  $form_state['redirect'] = 'admin/content/smartdocs/' . $model_id . '/revisions/' . $revision_id . '/security';
}

/**
 * Edit Revision Form.
 *
 * @param array $form
 *   The form being created.
 * @param array $form_state
 *   Current state of the form being created.
 * @param Apigee\SmartDocs\Model $model
 *   Model to which the revision being edited belongs.
 * @param Apigee\SmartDocs\Revision $revision
 *   The revision being edited.
 */
function smartdocs_edit_revision(array $form, array $form_state, Model $model, Revision $revision) {
  drupal_set_title(t('Edit API Revision #@rev of model ‘@model’', array(
    '@rev' => $revision->getRevisionNumber(),
    '@model' => $model->getDisplayName(),
  )));
  $breadcrumb = array();
  $breadcrumb[] = l(t('Home'), '<front>');
  $breadcrumb[] = l(t('Content'), 'admin/content');
  $breadcrumb[] = l(t('SmartDocs'), 'admin/content/smartdocs/models');
  $breadcrumb[] = l(t('@model', array('@model' => $model->getDisplayName())), 'admin/content/smartdocs/models/' . $model->getUuid());
  $breadcrumb[] = l(t('Revisions'), 'admin/content/smartdocs/models/' . $model->getUuid() . '/revisions');
  $breadcrumb[] = l(t('Revision #@rev', array('@rev' => $revision->getRevisionNumber())), 'admin/content/smartdocs/models/' . $model->getUuid() . '/revisions/' . $revision->getRevisionNumber());
  drupal_set_breadcrumb($breadcrumb);

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
 * Edit Revision Form Submit.
 *
 * @param array $form
 *   The descriptor of the form being submitted.
 * @param array $form_state
 *   The state of the form being submitted.
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
    $form_state['redirect'] = 'admin/content/smartdocs/models/' . $model->getUuid() . '/revisions/' . $rev;
  }
  catch (Exception $e) {
    drupal_set_message(t('Revision #@rev failed to update.', array('@rev' => $revision->getRevisionNumber())), 'error');
  }
}
