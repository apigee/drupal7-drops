<?php
/**
 * @file
 * Supplies administrative functions for SmartDocs.
 */

// Uploaded templates > 160k will generate a warning.
define('SMARTDOCS_TEMPLATE_SIZE_THRESHOLD', 163840);

use Apigee\Exceptions\ResponseException;
use Apigee\SmartDocs\Method;
use Apigee\SmartDocs\Model;
use Apigee\SmartDocs\Resource;
use Apigee\SmartDocs\Revision;
use Apigee\SmartDocs\Security;
use Apigee\SmartDocs\Template;
use Apigee\SmartDocs\TemplateAuth;
use Apigee\SmartDocs\Security\ApiKeyScheme;
use Apigee\SmartDocs\Security\ApiKeyTemplateAuthScheme;
use Apigee\SmartDocs\Security\BasicScheme;
use Apigee\SmartDocs\Security\Oauth2Scheme;
use Apigee\SmartDocs\Security\Oauth2TemplateAuthScheme;
use Apigee\SmartDocs\Security\SecurityScheme;
use Apigee\SmartDocs\Security\TemplateAuthScheme;

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

  $form['smartdocs_disable_ssl_warning'] = array(
    '#title' => t("Disable SSL warning message"),
    '#type' => 'checkbox',
    '#default_value' => variable_get('smartdocs_disable_ssl_warning', 0),
    '#description' => t("Select this option if you would like the warning message shown, if SSL is not enabled."),
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
      $start = microtime(TRUE);
      $models = $model_obj->listModels();
      if (variable_get('smartdocs_log_transaction_time', 0)) {
        $args = array('!time' => number_format(microtime(TRUE) - $start, 4));
        watchdog('smartdocs', 'Listed all models in !time seconds', $args, WATCHDOG_INFO);
      }
    }
    catch (ResponseException $e) {
      $models = array();
    }

    $options = array();
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
      $default_value = variable_get('smartdocs_' . $machine . '_docs', array());

      if (!empty($default_value)) {
        $form['products']['#collapsed'] = FALSE;
      }

      $form['products']['smartdocs_' . $machine . '_docs'] = array(
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

  // Do not collapse advanced settings if an advanced setting is set.
  if (
    (user_access('administer smartdocs templates') && variable_get('smartdocs_custom_template_file', FALSE))
    || variable_get('smartdocs_local_asset_version', 6) != 6
    || variable_get('smartdocs_use_prism', 1) != 1
    || variable_get('smartdocs_proxy_url', FALSE)
  ) {
    $is_advanced_settings_collapsed = FALSE;
  }
  else {
    $is_advanced_settings_collapsed = TRUE;
  }

  $form['settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Advanced settings'),
    '#collapsed' => $is_advanced_settings_collapsed,
    '#collapsible' => TRUE,
  );

  $form['settings']['advanced_settings_help'] = array(
    '#type' => 'markup',
    '#markup' => t('For additional help in configuring these advanced settings, consult our <a href="@url">online documentation</a>.', array('@url' => 'http://apigee.com/docs/developer-services/content/using-smartdocs-document-apis')),
    '#prefix' => '<div style="margin-bottom:1.5em">',
    '#suffix' => '</div>',
  );

  if (user_access('administer smartdocs templates')) {
    $form['settings']['smartdocs_custom_template_file'] = array(
      '#title' => t("Upload customized method template"),
      '#type' => 'managed_file',
      '#description' => t(<<<EOF
Upload a .hbr method template file.
This template will be used instead of the default model template when creating new models.
It is recommended to keep template sizes under 160k bytes.
Embedding base64-encoded images is not allowed;
instead, consider referencing such images from external URLs.
EOF
      ),
      '#upload_location' => 'public://smartdocs_templates',
      '#upload_validators' => array(
        'file_validate_extensions' => array('hbr'),
      ),
      '#default_value' => variable_get('smartdocs_custom_template_file', 0),
      '#suffix' => '<br/>',
    );

    $form['#validate'][] = 'smartdocs_settings_template_validate';
  }

  // Add javascript to the form to warn user when uploading template to click
  // save on page.
  $form['#attached']['js'] = array(
    drupal_get_path('module', 'smartdocs') . '/js/admin_settings.js',
  );

  $form['settings']['smartdocs_node_cache_ttl'] = array(
    '#title' => t('Method HTML cache lifetime'),
    '#type' => 'textfield',
    '#description' => t('The HTML for SmartDocs method pages is cached for this length of time before re-fetching from the Modeling API. A value of zero indicates that method pages are cached indefinitely.'),
    '#default_value' => intval(variable_get('smartdocs_node_cache_ttl', SMARTDOCS_DEFAULT_METHOD_CACHE_TTL)),
    '#post_render' => array('_smartdocs_node_cache_ttl_postrender'),
  );

  $form['settings']['overrides'] = array(
    '#type' => 'fieldset',
    '#title' => t('JS/CSS Assets'),
    '#collapsed' => FALSE,
    '#collapsible' => TRUE,
  );

  $form['settings']['overrides']['smartdocs_local_asset_version'] = array(
    '#type' => 'select',
    '#title' => t('SmartDocs JS/CSS version'),
    '#default_value' => variable_get('smartdocs_local_asset_version', '6'),
    '#description' => t('Determines which version of JavaScript and CSS assets will work with your templates.'),
    '#options' => array(
      '1' => 'v1',
      '2' => 'v2',
      '3' => 'v3',
      '4' => 'v4',
      '5' => 'v5',
      '6' => 'v6',
    ),
  );

  $form['settings']['overrides']['smartdocs_use_prism'] = array(
    '#type' => 'checkbox',
    '#title' => t('Include Prism syntax highlighter'),
    '#default_value' => variable_get('smartdocs_use_prism', 1),
    '#description' => t('When checked, JSON and XML syntax of request and response bodies will be highlighted using the Prism JavaScript library.'),
  );

  $config = devconnect_get_org_settings();
  $endpoint_parts = parse_url($config['endpoint']);
  if ($endpoint_parts['host'] == 'api.enterprise.apigee.com') {
    $description = t('You should not need to edit this value unless one or more of your APIs is on a private network.');
  }
  else {
    if (isset($endpoint_parts['port'])) {
      $port = $endpoint_parts['port'];
      // Commence black magick.
      // If this is a default on-prem install (as indicated by an 8080
      // endpoint), the default port for the SmartDocs proxy is 59001.
      if ($port == 8080) {
        $port = 59001;
      }
      $port = ':' . $port;
    }
    else {
      $port = '';
    }
    $proxy_url = $endpoint_parts['scheme'] . '://'
      . $endpoint_parts['host'] . $port . '/'
      . preg_replace('!/v1$!', '', $endpoint_parts['path']);

    $proxy_url = rtrim($proxy_url, '/') . '/smartdocs/v1/sendrequest';

    $args = array(
      '%proxy' => $proxy_url,
      '%default' => 'https://apiconsole-prod.apigee.net/smartdocs/v1/sendrequest',
    );
    $description = t('If your dev portal will be exposed to users in an internal network who do not have access to the external internet, or if one or more of your APIs is on a private network, you should populate this field with the value %proxy (after making sure that the hostname resolves correctly for your users). If you leave it blank, a default value of %default will be used.', $args);
  }

  $form['settings']['smartdocs_proxy_url'] = array(
    '#type' => 'textfield',
    '#title' => t('SmartDocs proxy URL'),
    '#description' => $description,
    '#default_value' => variable_get('smartdocs_proxy_url'),
  );

  $form = system_settings_form($form);

  // Make sure the array is flattened before system_settings_form_submit() is
  // called.
  array_unshift($form['#submit'], 'smartdocs_settings_flatten_array');
  array_unshift($form['#submit'], 'smartdocs_settings_empty_proxy_url');
  // Additional submit handler to be invoked *after* calling
  // system_settings_form_submit().
  if (user_access('administer smartdocs templates')) {
    $form['#submit'][] = 'smartdocs_settings_submit';
  }

  return $form;
}

/**
 * Post-render callback for the Cache TTL number field.
 *
 * @param string $content
 *   The already-rendered HTML for this element.
 * @param array $element
 *   The array representation of the element.
 *
 * @return string
 *   The modified HTML for this element.
 */
function _smartdocs_node_cache_ttl_postrender($content, $element) {
  $input_element = '<input type="number" style="width:7em !important" min="0" '
    . 'id="' . $element['#id'] . '" name="' . $element['#name'] . '" '
    . 'value="' . $element['#value'] . '" class="form-text" /> ' . t('minutes');
  return preg_replace('!<input [^>]+>!Ums', $input_element, $content);
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
 * Early submit handler for smartdocs_settings().
 *
 * Makes sure that if smartdocs_proxy_url is empty, the variable is deleted
 * from the database, causing default-value fallbacks to come into play.
 *
 * @param array $form
 *   Descriptor of the form being submitted.
 * @param array $form_state
 *   State of the form being submitted.
 */
function smartdocs_settings_empty_proxy_url($form, &$form_state) {
  if (empty($form_state['values']['smartdocs_proxy_url'])) {
    unset($form_state['values']['smartdocs_proxy_url']);
    variable_del('smartdocs_proxy_url');
  }
}

/**
 * Validator for smartdocs_settings form.
 *
 * Ensures that a custom template (if any exist) conforms to the prescribed
 * limitations, namely that it does not contain embedded base64-encoded images.
 * Also issues a warning if an uploaded template is excessively large.
 *
 * @param array $form
 *   Descriptor of the form being validated.
 * @param array $form_state
 *   State of the form being validated.
 */
function smartdocs_settings_template_validate(array $form, array &$form_state) {
  if (!empty($form_state['values']['smartdocs_custom_template_file'])) {
    $file = file_load($form_state['values']['smartdocs_custom_template_file']);
    if (!$file) {
      return;
    }
    if (file_exists($file->uri) && _smartdocs_template_contains_base64(file_get_contents($file->uri))) {
      form_error(
        $form['settings']['smartdocs_custom_template_file'],
        t('Uploaded templates cannot contain base64-encoded images.')
      );
    }
    elseif ($file->filesize > SMARTDOCS_TEMPLATE_SIZE_THRESHOLD) {
      drupal_set_message(
        t('The template you have uploaded is very large, and may cause performance issues when rendering.'),
        'warning'
      );
    }
  }
}

/**
 * Check to see if an uploaded template contains base64-encoded images.
 *
 * @param string $contents
 *   Contents of the template.
 *
 * @return bool
 *   True if the template contains base64-encoded images; false otherwise.
 */
function _smartdocs_template_contains_base64($contents) {
  return (bool) preg_match('!data:image/[^;]{1,10};base64,!', $contents);
}

/**
 * Submit callback for the smartdocs_settings(); handles template file upload.
 *
 * @param array $form
 *   Descriptor of the form being submitted.
 * @param array $form_state
 *   State of the form being submitted.
 */
function smartdocs_settings_submit(array $form, array &$form_state) {
  // Get the managed file if it exists.
  $custom_template_file_current = (!empty($form['smartdocs_custom_template_file']['#default_value'])) ? $form['smartdocs_custom_template_file']['#default_value'] : 0;

  // If a file has been submitted, mark it as in use.
  if (isset($form_state['values']['smartdocs_custom_template_file']) && $form_state['values']['smartdocs_custom_template_file']) {
    // If the uploaded file is different than the current file, remove the
    // current file and replace it.
    if (!empty($custom_template_file_current) && $custom_template_file_current != $form_state['values']['smartdocs_custom_template_file']) {
      smartdocs_template_remove_managed_file($custom_template_file_current);
    }
    smartdocs_template_add_managed_file($form_state['values']['smartdocs_custom_template_file']);
  }
}

/**
 * Remove a SmartDoc template from the managed file system.
 *
 * @param int $managed_file
 *   A file ID.
 */
function smartdocs_template_remove_managed_file($managed_file) {
  // Retrieve the old file's id.
  $file = $managed_file ? file_load($managed_file) : FALSE;
  if ($file) {
    // When a module is managing a file, it must manage the usage count.
    // Here we decrement the usage count with file_usage_delete().
    file_usage_delete($file, 'smartdocs', 'smartdocs_global_template_file', $file->fid);

    // The file_delete() function takes a file object and checks to see if
    // the file is being used by any other modules. If it is the delete
    // operation is cancelled, otherwise the file is deleted.
    file_delete($file);
    drupal_set_message(t('The smartdocs template @filename was removed.', array('@filename' => $file->filename)));
  }
}

/**
 * Add a SmartDoc template to the managed file system.
 *
 * @param int $managed_file
 *   A file ID.
 */
function smartdocs_template_add_managed_file($managed_file) {
  // If our file is already marked as used we do not need to increase the count.
  $count = db_select('file_usage', 'f')
    ->fields('f', array('count'))
    ->condition('fid', $managed_file)
    ->execute()
    ->fetchField();
  if (empty($count)) {
    // Load the file via file id.
    $file = file_load($managed_file);
    // Change the file status to permanent.
    $file->status = FILE_STATUS_PERMANENT;
    file_save($file);
    // Mark file as in use by SmartDocs.
    file_usage_add($file, 'smartdocs', 'smartdocs_global_template_file', $managed_file);
    unset($file);
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

  $model = new Model(devconnect_default_org_config());
  try {
    $models = $model->listModels();
  }
  catch (ResponseException $e) {
    $source = ($e->getCode() < 200 ? 'CURL' : 'Edge');
    return t(
      'Unable to fetch list of models; !source responded with a status of @status.',
      array(
        '@status' => $e->getCode(),
        '!source' => $source,
      )
    );
  }

  $table_rows = array();
  /** @var Model $model */
  foreach ($models as $model) {
    $node_count = $published_count = 0;
    $latest_rev = $model->getLatestRevisionNumber();
    if ($latest_rev > 0) {
      $model_name = l($model->getDisplayName(), 'admin/content/smartdocs/models/' . $model->getUuid() . '/revisions/' . $model->getLatestRevisionNumber());

      // Make a best guess to see if there are nodes, and if so, if any are
      // published. This is not foolproof.
      $query = db_select('smartdata', 's');
      $query->innerJoin('node', 'n', 's.nid = n.nid');
      $query->fields('n', array('nid'))
        ->condition('s.model', $model->getName())
        ->condition('s.revision', $latest_rev);
      $node_count = $query->execute()->rowCount();
      if ($node_count) {
        $query->condition('n.status', 1);
        $published_count = $query->execute()->rowCount();
      }
    }
    else {
      $model_name = l($model->getDisplayName(), 'admin/content/smartdocs/models/' . $model->getUuid());
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
      $ops['list-' . $op] = array(
        'title' => $disp,
        'href' => 'admin/content/smartdocs/models/' . $model->getUuid() . '/' . $op,
        'attributes' => array(
          'title' => $disp,
          'class' => array($op . '-' . $model->getName(), 'views-ajax-link'),
        ),
      );
    }

    $table_rows[$model->getUuid()] = array(
      $model_name,
      $model->getDescription(),
      $node_count,
      $published_count,
      theme('links__ctools_dropbutton', array('links' => $ops)),
    );
  }
  $header = array(
    'name' => t('Name'),
    'description' => t('Description'),
    'node_count' => t('Rendered nodes'),
    'published_count' => t('Published nodes'),
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
  ));

  $output['table'] = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $table_rows,
    '#empty' => $empty_msg,
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
  module_invoke_all('smartdocs_model_presave', $model);
  $success = FALSE;
  try {
    $model->save();
    module_invoke_all('smartdocs_model_postsave', $model);
    // Get the content of the custom template.
    $custom_template_file = variable_get('smartdocs_custom_template_file', NULL);
    $template_content = FALSE;
    if (!empty($custom_template_file)) {
      $file = file_load($custom_template_file);
      $path = FALSE;
      if ($file === FALSE) {
        // The custom template file no longer exists in db, show error and
        // remove setting.
        drupal_set_message(t('Custom model template could not be loaded. Standard model template is being used instead.'), 'warning');
        watchdog('smartdocs', 'Custom model template could not be loaded, smartdocs_custom_template_file variable was set to %smartdocs_custom_template_file.', array('%smartdocs_custom_template_file' => $custom_template_file), WATCHDOG_ERROR);
        variable_del('smartdocs_custom_template_file');
      }
      else {
        $path = drupal_realpath($file->uri);
      }

      // Get custom template content if path is valid.
      if ($path) {
        $template_content = file_get_contents($path);
        if ($template_content === FALSE) {
          // The custom template file no longer exists in filesystem, show error
          // and remove setting.
          drupal_set_message(t('Custom model template could not be loaded. Standard model template is being used instead.'), 'warning');
          watchdog('smartdocs', 'Custom model template could not be loaded, could not get contents of file %path.', array('path' => $path), WATCHDOG_ERROR);
          variable_del('smartdocs_custom_template_file');
        }
      }
    }

    // Check to see if successfully loaded the custom template and if not get
    // standard template content to add to model.
    if ($template_content === FALSE) {
      $path = drupal_get_path('module', 'smartdocs') . '/templates/smartdocs.hbr';
      $template_content = file_get_contents($path);
    }

    module_invoke_all('smartdocs_template_presave', $model->getName(), $template_content);
    $template = new Template(devconnect_default_org_config(), $model->getUuid());
    $template->save(SMARTDOCS_TEMPLATE_NAME, 'method', $template_content, TRUE);

    $success = TRUE;
    module_invoke_all('smartdocs_template_postsave', $model->getName(), $template);
    module_invoke_all('smartdocs_model_update', $model->getUuid());
    $form_state['values']['model_id'] = $model->getUuid();

    // Create taxonomy term for model.
    $term = taxonomy_get_term_by_name($model->getName(), 'smartdocs_models');
    if (empty($term)) {
      $vocab = taxonomy_vocabulary_machine_name_load('smartdocs_models');
      if (!empty($vocab)) {
        $term = new stdClass();
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
    $start = microtime(TRUE);
    $models = $model_obj->listModels();
    if (variable_get('smartdocs_log_transaction_time', 0)) {
      $args = array('!time' => number_format(microtime(TRUE) - $start, 4));
      watchdog('smartdocs', 'Listed all models in !time seconds', $args, WATCHDOG_INFO);
    }
  }
  catch (ResponseException $e) {
    watchdog_exception('smartdocs', $e, 'Edge connection to check model name uniqueness failed: !message', array('!message' => $e->__toString()));
    drupal_set_message(t('Failed to connect to Edge backend to ensure that your model name is unique.'), 'error');
    // Must return true here because we cannot ensure model-name uniqueness.
    return TRUE;
  }
  /** @var Model $model */
  foreach ($models as $model) {
    if (strtolower($model->getName()) == strtolower($name)) {
      return TRUE;
    }
  }
  return FALSE;
}

/**
 * Page callback to display model detail.
 *
 * @param array $model_array
 *   Model (in array form) whose detail is to be displayed.
 *
 * @return array
 *   Render-array of data to be displayed.
 */
function smartdocs_show_model_detail(array $model_array) {

  $model = new Model(devconnect_default_org_config());
  Model::fromArray($model, $model_array);

  $breadcrumb = array();
  $breadcrumb[] = l(t('Home'), '<front>');
  $breadcrumb[] = l(t('Content'), 'admin/content');
  $breadcrumb[] = l(t('SmartDocs'), 'admin/content/smartdocs/models');
  $breadcrumb[] = l(t('!model', array('!model' => $model->getDisplayName())), 'admin/content/smartdocs/models/' . $model->getUuid());
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
    foreach ($revision->getResources() as $resource) {
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
 * @param Model|array $model
 *   The model whose view path is to be determined.
 *
 * @return string|bool
 *   Path of view, or FALSE if not found.
 */
function _smartdocs_get_revision_view_path($model) {
  if ($model instanceof Model) {
    $model = $model->toArray();
  }
  $terms = taxonomy_get_term_by_name($model['name'], 'smartdocs_models');
  if (!empty($terms)) {
    $term = reset($terms);
    return 'taxonomy/term/' . $term->tid;
  }
  return FALSE;
}

/**
 * Imports WADL/Swagger/SmartDocs JSON data into SmartDocs.
 *
 * @param array $form
 *   Descriptor of the form being created.
 * @param array $form_state
 *   State of the form being created.
 * @param array $model_array
 *   Model (in array form) into which data is to be imported.
 */
function smartdocs_import_model(array $form, array $form_state, array $model_array) {
  $model = new Model(devconnect_default_org_config());
  Model::fromArray($model, $model_array);

  drupal_set_title('Import into ‘' . $model->getDisplayName() . '’');

  $breadcrumb = array();
  $breadcrumb[] = l(t('Home'), '<front>');
  $breadcrumb[] = l(t('Content'), 'admin/content');
  $breadcrumb[] = l(t('SmartDocs'), 'admin/content/smartdocs/models');
  $breadcrumb[] = l(t('!model', array('!model' => $model->getDisplayName())), 'admin/content/smartdocs/models/' . $model->getUuid());
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
      'swagger_json' => 'OpenAPI (Swagger) - JSON',
      'swagger_yaml' => 'OpenAPI (Swagger) - YAML',
      'apigee_json' => 'SmartDocs - JSON',
    ),
    '#default_value' => array('swagger_yaml'),
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
    // If file was uploaded and claims to be OpenAPI, make sure it has a
    // top-level 'swagger' member.
    elseif (
      substr($form_state['values']['type'], 0, 8) == 'swagger_'
      && !empty($_FILES['files']['tmp_name']['file'])
      && file_exists($_FILES['files']['tmp_name']['file'])
    ) {
      $contents = file_get_contents($_FILES['files']['tmp_name']['file']);

      // Is this a YAML file?
      if (substr($form_state['values']['type'], -4) == 'yaml') {
        $is_openapi = (strpos($contents, 'swagger:') !== FALSE);
      }
      else {
        // Nope, must be JSON.
        $openapi = @json_decode($contents, TRUE);
        $is_openapi = is_array($openapi) && array_key_exists('swagger', $openapi);
      }
      if (!$is_openapi) {
        form_set_error('file', t('Your uploaded file does not appear to be a valid OpenAPI document.'));
      }
    }
  }
  else {
    // Validate URL is valid.
    if (isset($form_state['values']['url'])) {
      $url = trim($form_state['values']['url']);
      if (!valid_url($url, TRUE)) {
        form_set_error('url', t('The URL %url is invalid.', array('%url' => $form_state['values']['url'])));
      }
    }
  }
}

/**
 * Imports WADL/Swagger/SmartDocs JSON data into SmartDocs.
 *
 * @param array $form
 *   Descriptor of the form being created.
 * @param array $form_state
 *   State of the form being created.
 */
function smartdocs_import_model_submit(array $form, array &$form_state) {
  $log_transaction_time = variable_get('smartdocs_log_transaction_time', 0);

  $type = $form_state['values']['type'];
  $model = new Model(devconnect_default_org_config());
  $start = microtime(TRUE);
  $model->load($form_state['values']['model_uuid']);
  if ($log_transaction_time) {
    $args = array(
      '!model' => $model->getUuid(),
      '!time' => number_format(microtime(TRUE) - $start, 4),
      '!function' => __FUNCTION__,
    );
    watchdog('smartdocs', '!function: Loaded model !model in !time seconds.', $args, WATCHDOG_INFO);
  }

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

  $file_contents = NULL;

  if ($form_state['values']['upload_type'] == 'url') {
    // Importing a URL.
    $url = trim($form_state['values']['url']);
    try {
      $start = microtime(TRUE);
      $revision = $model->importUrl($url, $document_format);
      if ($log_transaction_time) {
        $args = array(
          '!model' => $model->getUuid(),
          '!revision' => $revision,
          '!time' => number_format(microtime(TRUE) - $start, 4),
        );
        watchdog('smartdocs', 'Imported URL into revision !revision of model !model in !time seconds.', $args, WATCHDOG_INFO);
      }

      $response = drupal_http_request($url);
      if ($response->code == 200) {
        $file_contents = $response->data;
      }

      // Parameters to use in success message to user.
      $message_vars = array(
        '%model_name' => $model->getName(),
        '%revision' => $revision,
        '@source_name' => "URL",
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
      $start = microtime(TRUE);
      $revision = $model->importFile($file_contents, $document_format, $content_type);
      if ($log_transaction_time) {
        $args = array(
          '!model' => $model->getUuid(),
          '!revision' => $revision,
          '!time' => number_format(microtime(TRUE) - $start, 4),
        );
        watchdog('smartdocs', 'Imported file into revision !revision of model !model in !time seconds.', $args, WATCHDOG_INFO);
      }

      // Parameters to use in success message to user.
      $message_vars = array(
        '%model_name' => $model->getName(),
        '%revision' => $revision,
        '@source_name' => 'file',
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
  // Danger, Will Robinson! Hack incoming!
  // Work around Modeling API bugs regarding parameter enums. See MGMT-2664.
  if (isset($file_contents) && $document_format == 'swagger') {
    $swagger_obj = FALSE;
    if (substr($content_type, -4) == 'yaml') {
      // Don't bother chewing through the Swagger if enums do not appear to be
      // present. This is not foolproof, but it eliminates false negatives.
      if (strpos($file_contents, 'enum:') !== FALSE) {
        require_once libraries_get_path('spyc') . '/Spyc.php';
        try {
          $swagger_obj = spyc_load($file_contents);
        }
        catch (Exception $e) {
          // Spyc may throw "Too many keys" exception in edge cases where it
          // fails to parse valid well-formed YAML. This occurs when an object
          // or array is written in "short syntax" and spans more than one line.
          // See https://github.com/mustangostang/spyc/issues/30
          // This same bug also afflicts Symfony\Yaml, though PECL yaml reads
          // the data correctly (using libyaml).
          watchdog_exception('smartdocs', $e);
          drupal_set_message(t('Warning: If your model contains parameter enums, they may not have imported correctly.'), 'warning');
        }
      }
    }
    else {
      $swagger_obj = json_decode($file_contents, TRUE);
    }
    if (!empty($swagger_obj) && intval($swagger_obj['swagger']) >= 2) {
      $resource_enums = array();
      $method_enums = array();
      $verbs = array('get', 'post', 'put', 'patch', 'options', 'head', 'delete');
      foreach ($swagger_obj['paths'] as $path => $resource) {
        // Handle resource parameters.
        if (array_key_exists('parameters', $resource) && is_array($resource['parameters'])) {
          foreach ($resource['parameters'] as $parameter) {
            if (array_key_exists('enum', $parameter)) {
              $resource_enums[$path][$parameter['name']] = $parameter['enum'];
            }
          }
        }
        // Handle method parameters.
        foreach ($verbs as $verb) {
          if (array_key_exists($verb, $resource)) {
            if (array_key_exists('parameters', $resource[$verb]) && is_array($resource[$verb]['parameters'])) {
              foreach ($resource[$verb]['parameters'] as $parameter) {
                if (array_key_exists('enum', $parameter)) {
                  $method_enums[$path][$verb][$parameter['name']] = $parameter['enum'];
                }
              }
            }
          }
        }
      }

      if (count($resource_enums) > 0 || count($method_enums) > 0) {
        // Reload the model to make sure we get the latest revision number.
        $model->load();
        $revision_id = $model->getLatestRevisionNumber();
        // Load our active revision.
        $rev = new Revision($model->getConfig(), $model->getUuid());
        $rev->load($revision_id);
        module_invoke_all('smartdocs_import_revision_resources_presave', $model, $rev, $resource_enums);
        module_invoke_all('smartdocs_import_revision_methods_presave', $model, $rev, $method_enums);
        $resources = $rev->getResources();

        foreach ($resources as $resource) {
          $path = $resource->getPath();
          // See if this resource has any resource-level params with an enum
          // member; if so, apply fixes.
          if (array_key_exists($path, $resource_enums)) {
            $resource_modified = FALSE;
            $params = $resource->getParameters();
            foreach ($resource_enums[$path] as $param_name => $param_enum) {
              $index = _smartdocs_find_named_param($params, $param_name);
              if ($index >= 0) {
                $params[$index]['options'] = $param_enum;
                $resource->setParameters($params);
                $resource_modified = TRUE;
              }
            }
            if ($resource_modified) {
              module_invoke_all('smartdocs_resource_presave', $resource, $revision, $model, TRUE);
              $resource->save(TRUE);
              module_invoke_all('smartdocs_resource_postsave', $resource, $revision, $model, TRUE);
            }
          }

          // Now handle any method-level params.
          if (array_key_exists($path, $method_enums)) {
            foreach ($method_enums[$path] as $verb => $params) {
              $verb = strtolower($verb);
              // Find the corresponding method.
              $target_method = NULL;
              foreach ($resource->getMethods() as $method) {
                if (strtolower($method->getVerb()) == $verb) {
                  $target_method = $method;
                  break;
                }
              }
              // Did we find our method? Answer should always be yes.
              if ($target_method) {
                $method_modified = FALSE;
                $target_params = $target_method->getParameters();
                foreach ($params as $param_name => $param_enum) {
                  $index = _smartdocs_find_named_param($target_params, $param_name);
                  if ($index >= 0) {
                    $target_params[$index]['options'] = $param_enum;
                    $target_method->setParameters($target_params);
                    $method_modified = TRUE;
                  }
                }
                if ($method_modified) {
                  module_invoke_all('smartdocs_method_presave', $target_method, $model->getName(), $rev->getUuid(), $resource->getUuid(), TRUE);
                  $target_method->save(TRUE);
                  module_invoke_all('smartdocs_method_postsave', $target_method, $model->getName(), $rev->getUuid(), $resource->getUuid(), TRUE);
                }
              }
            }
          }
        }
        // Finished cycling through resources.
        module_invoke_all('smartdocs_import_revision_resources_postsave', $model, $rev, $resource_enums);
        module_invoke_all('smartdocs_import_revision_methods_postsave', $model, $rev, $method_enums);
      }
    }
    // Finished twiddling Swagger file import. End of horrible hack.
  }

  module_invoke_all('smartdocs_model_import', $file_contents, $content_type, $document_format, $model, $form_state['values']['upload_type']);

  // If we are updating, we need to invalidate cache.
  if ($model->getLatestRevisionNumber() > 1) {
    _smartdocs_flush_cache();
  }

  drupal_set_message(t('Revision %revision has been created from @source_name from %source.', $message_vars), 'status');
  watchdog('smartdocs', 'Model %model_name revision %revision has been created from @source_name from %source.', $message_vars, WATCHDOG_INFO);
  $form_state['redirect'] = 'admin/content/smartdocs/models/' . $model->getUuid() . '/revisions/' . $revision;
}

/**
 * Finds index of parameter in an array, matching on name.
 *
 * @param array $params
 *   Params , either attached to a Resource or a Method.
 * @param string $param_name
 *   Name of the param to be matched.
 *
 * @return int
 *   Array offset of parameter.
 */
function _smartdocs_find_named_param(array $params, $param_name) {
  foreach ($params as $i => $param) {
    if ($param['name'] == $param_name) {
      return $i;
    }
  }
  return -1;
}

/**
 * Handles exceptions encountered when importing a model.
 *
 * @param ResponseException $e
 *   The exception that was thrown.
 * @param Model $model
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
 * @param array $model_array
 *   The Model to be exported.
 * @param array|null $revision_array
 *   The Revision to be exported (in array form). If NULL, we export the latest
 *   revision.
 */
function smartdocs_model_export(array $form, array &$form_state, array $model_array, array $revision_array = NULL) {
  $model = new Model(devconnect_default_org_config());
  Model::fromArray($model, $model_array);
  $revision = new Revision($model->getConfig(), $model->getUuid());

  if (is_array($revision_array)) {
    Revision::fromArray($revision, $revision_array);
    drupal_set_title('Export Revision #' . $revision->getRevisionNumber());
  }
  else {
    $revision_id = $model->getLatestRevisionNumber();
    if ($revision_id < 1) {
      drupal_set_message(t('You have no revisions in the model %model. You cannot export a model until you create a revision.', array('%model' => $model->getDisplayName())), 'error');
      drupal_goto('admin/content/smartdocs/models/' . $model->getUuid());
    }
    $start = microtime(TRUE);
    // TODO: wrap this in a try/catch block.
    $revision->load($revision_id);
    if (variable_get('smartdocs_log_transaction_time', 0)) {
      $args = array(
        '!model' => $model,
        '!revision' => $revision->getRevisionNumber(),
        '!time' => number_format(microtime(TRUE) - $start, 4),
        '!function' => __FUNCTION__,
      );
      watchdog('smartdocs', '!function: Loaded revision !revision of model !model in !time seconds.', $args, WATCHDOG_INFO);
    }
    drupal_set_title('Export Revision #' . $revision->getRevisionNumber());
    $revision_array = $revision->toArray();
  }

  $breadcrumb = array();
  $breadcrumb[] = l(t('Home'), '<front>');
  $breadcrumb[] = l(t('Content'), 'admin/content');
  $breadcrumb[] = l(t('SmartDocs'), 'admin/content/smartdocs/models');
  $breadcrumb[] = l(t('!model', array('!model' => $model->getDisplayName())), 'admin/content/smartdocs/models/' . $model->getUuid());
  $breadcrumb[] = l(t('Revision #@rev', array('@rev' => $revision->getRevisionNumber())), 'admin/content/smartdocs/models/' . $model->getUuid() . '/revision/' . $revision->getRevisionNumber());
  drupal_set_breadcrumb($breadcrumb);

  $form = array();
  $form['revision'] = array(
    '#type' => 'value',
    '#value' => $revision_array,
  );
  $form['model'] = array(
    '#type' => 'value',
    '#value' => $model_array,
  );
  $options = array(
    'json' => 'SmartDocs JSON',
    // DEVSOL-1434: Edge API cannot handle WADL exports for now.
  );
  $form['file_type'] = array(
    '#type' => 'select',
    '#title' => t('Select export file type'),
    '#options' => $options,
    '#default_value' => '',
    '#weight' => 1,
    '#suffix' => '<br>',
  );
  // Don't show export selectbox if there is only one option.
  if (count($options) == 1) {
    $form['file_type']['#access'] = FALSE;
    $form['file_type']['#default_value'] = 'json';

    $form['caption'] = array(
      '#type' => 'markup',
      '#markup' => t('Your model will be exported in SmartDocs’s internal JSON format.'),
      '#prefix' => '<p>',
      '#suffix' => '</p>',
      '#weight' => 0,
    );
  }

  $form['options']['submit_export'] = array(
    '#type' => 'submit',
    '#value' => t('Export'),
    '#weight' => 2,
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
  $model = new Model(devconnect_default_org_config());
  Model::fromArray($model, $form_state['values']['model']);
  $revision = new Revision($model->getConfig(), $model->getUuid());
  Revision::fromArray($revision, $form_state['values']['revision']);
  $revision_num = $revision->getRevisionNumber();

  $export_type = $form_state['values']['file_type'];
  $filename = $model->getName() . '.rev' . $revision_num . '.' . $export_type;

  try {
    $start = microtime(TRUE);
    $data = $revision->export($export_type, $revision_num);
    if (variable_get('smartdocs_log_transaction_time', 0)) {
      $args = array(
        '!model' => $model->getUuid(),
        '!revision' => $revision_num,
        '!time' => number_format(microtime(TRUE) - $start, 4),
      );
      watchdog('smartdocs', 'Exported revision !revision of model !model in !time seconds.', $args, WATCHDOG_INFO);
    }
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
 * @param array $model_array
 *   The model (in array form) whose settings are to be edited.
 *
 * @return string
 *   The html for the settings page.
 */
function smartdocs_model_settings_page(array $model_array) {
  drupal_set_title(t('Template settings for !model', array('!model' => $model_array['displayName'])));
  $breadcrumb = array();
  $breadcrumb[] = l(t('Home'), '<front>');
  $breadcrumb[] = l(t('Content'), 'admin/content');
  $breadcrumb[] = l(t('SmartDocs'), 'admin/content/smartdocs/models');
  $breadcrumb[] = l(t('!model', array('!model' => $model_array['displayName'])), 'admin/content/smartdocs/models/' . $model_array['id']);
  drupal_set_breadcrumb($breadcrumb);

  $output = '';
  if (user_access('administer smartdocs templates')) {
    $output .= drupal_render(drupal_get_form('smartdocs_model_settings', $model_array));
  }
  $output .= drupal_render(drupal_get_form('smartdocs_model_template_auth', $model_array));

  return $output;
}

/**
 * Allows configuration of a custom template for a specific model.
 *
 * @param array $form
 *   Descriptor of the form being created.
 * @param array $form_state
 *   State of the form being created.
 * @param array $model_array
 *   The Model (in array form) whose settings are to be altered.
 *
 * @return array
 *   The newly-created form.
 */
function smartdocs_model_settings(array $form, array &$form_state, array $model_array) {
  try {
    $template_obj = new Template(devconnect_default_org_config(), $model_array['id']);
    $start = microtime(TRUE);
    $template = $template_obj->load(SMARTDOCS_TEMPLATE_NAME, 'method');
    if (variable_get('smartdocs_log_transaction_time', 0)) {
      $args = array(
        '!model' => $model_array['id'],
        '!time' => number_format(microtime(TRUE) - $start, 4),
      );
      watchdog('smartdocs', 'Loaded template for model !model in !time seconds.', $args, WATCHDOG_INFO);
    }
  }
  catch (ResponseException $e) {
    $form['error'] = array(
      '#type' => 'markup',
      '#markup' => t('Unable to load method template from model !model. This may happen if you have not imported a revision for the model. Please !import a model and try again.',
        array(
          '!model' => '<em>' . $model_array['displayName'] . '</em>',
          '!import' => l(t('import'), 'admin/content/smartdocs/models/' . $model_array['id'] . '/import'),
        )),
      '#prefix' => '<div class="messages error">',
      '#suffix' => '</div>',
    );
    return $form;
  }

  drupal_set_title('‘' . $model_array['displayName'] . '’' . ' Settings');

  $form['model_uuid'] = array(
    '#type' => 'value',
    '#value' => $model_array['id'],
  );
  $form['model_name'] = array(
    '#type' => 'value',
    '#value' => $model_array['name'],
  );

  $form['method_template'] = array(
    '#type' => 'textarea',
    '#title' => t('Method Template'),
    '#rows' => 10,
    '#default_value' => $template,
    '#description' => t(<<<EOF
This template will be used for the ‘@model’ model, overriding any other default template.
It is recommended to keep template sizes under 160k bytes.
Embedding base64-encoded images is not allowed;
instead, consider referencing such images from external URLs.
EOF
      , array('@model' => $model_array['displayName'])),
  );

  $form['action'] = array(
    '#type' => 'value',
    '#value' => 'save',
  );

  $form['actions'] = array(
    '#type' => 'actions',
    'submit' => array(
      '#type' => 'submit',
      '#value' => t('Save template'),
    ),
    'revert' => array(
      '#type' => 'markup',
      '#markup' => l(
        t('Revert template to default'),
        'admin/content/smartdocs/models/' . $model_array['id'] . '/revert-template',
        array('attributes' => array('class' => array('button')))
      ),
    ),
  );

  $form['#validate'][] = 'smartdocs_method_settings_validate';
  return $form;
}

/**
 * Validator for smartdocs_model_settings form.
 *
 * @param array $form
 *   Descriptor of the form being validated.
 * @param array $form_state
 *   State of the form being validated.
 */
function smartdocs_method_settings_validate(array $form, array &$form_state) {
  $contents = $form_state['values']['method_template'];
  if (_smartdocs_template_contains_base64($contents)) {
    form_error($form['method_template'], t('Uploaded templates cannot contain base64-encoded images.'));
  }
  elseif (strlen($contents) > SMARTDOCS_TEMPLATE_SIZE_THRESHOLD) {
    drupal_set_message(
      t('The template you have uploaded is very large, and may cause performance issues when rendering.'),
      'warning'
    );
  }
}

/**
 * Form constructor for confirm-template-revert action.
 *
 * @param array $confirm_form
 *   Form being constructed.
 * @param array $form_state
 *   State of form being constructed.
 * @param array $model
 *   Array representation of model whose template is being reverted.
 *
 * @return array
 *   Newly-created form.
 */
function smartdocs_model_revert_template(array $confirm_form, array &$form_state, array $model) {

  $confirm_form['model_uuid'] = array(
    '#type' => 'value',
    '#value' => $model['id'],
  );
  $confirm_form['model_name'] = array(
    '#type' => 'value',
    '#value' => $model['name'],
  );
  $confirm_form['action'] = array(
    '#type' => 'value',
    '#value' => 'revert',
  );
  $confirm_form['method_template'] = array(
    '#type' => 'value',
    '#value' => '',
  );

  $confirm_form = confirm_form(
    $confirm_form,
    t('Revert this template to the default?'),
    'admin/content/smartdocs/models/' . $model['id'] . '/settings',
    t('Are you sure you want to revert this template to the default? Any changes you have made will be lost.'),
    t('Go ahead and revert'),
    t('Cancel')
  );

  $confirm_form['#submit'] = array('smartdocs_model_settings_submit');
  return $confirm_form;
}

/**
 * Submit Callback for Model Template.
 *
 * @param array $form
 *   Descriptor of the form being submitted.
 * @param array $form_state
 *   State of the form being submitted.
 */
function smartdocs_model_settings_submit($form, &$form_state) {

  $model_uuid = $form_state['values']['model_uuid'];
  $model_name = $form_state['values']['model_name'];
  $html = $form_state['values']['method_template'];
  $action = $form_state['values']['action'];

  if ($action == 'revert') {
    // Get the path of the default template.
    $path = drupal_get_path('module', 'smartdocs') . '/templates/smartdocs.hbr';
    // Get the path of the custom global template if it exists.
    $custom_template_file = variable_get('smartdocs_custom_template_file', NULL);
    if (!empty($custom_template_file)) {
      $file = file_load($custom_template_file);
      if ($file !== FALSE) {
        $custom_path = drupal_realpath($file->uri);
        if (file_exists($custom_path) && is_readable($custom_path) && filesize($custom_path) > 0) {
          $path = $custom_path;
        }
      }
    }
    $html = file_get_contents($path);
    $status_message = t('Template reverted.');
    $form_state['redirect'] = 'admin/content/smartdocs/models/' . $model_uuid . '/settings';
  }
  else {
    $status_message = t('Template saved.');
  }

  try {
    $template_obj = new Template(devconnect_default_org_config(), $model_name);
    module_invoke_all('smartdocs_template_presave', $model_name, $html);
    $start = microtime(TRUE);
    $template_obj->save(SMARTDOCS_TEMPLATE_NAME, 'method', $html, TRUE);
    if (variable_get('smartdocs_log_transaction_time', 0)) {
      $args = array(
        '!model' => $model_name,
        '!time' => number_format(microtime(TRUE) - $start, 4),
      );
      watchdog('smartdocs', 'Saved method template for model !model in !time seconds', $args, WATCHDOG_INFO);
    }
    module_invoke_all('smartdocs_template_postsave', $model_name, $html);
    module_invoke_all('smartdocs_template_reverted', $model_name);
    drupal_set_message($status_message);
  }
  catch (Exception $e) {
    $msg_args = array('%model_name' => $model_name, '%error' => $e->getMessage());
    drupal_set_message(t('Cannot save SmartDocs template for model %model_name. %error', $msg_args));
    watchdog('smartdocs', 'Cannot save SmartDocs template for model %model_name. %error', $msg_args, WATCHDOG_ERROR);
  }
}

/**
 * Form for adding template auth schemes.
 *
 * @param array $form
 *   The form array.
 * @param array $form_state
 *   The form state array.
 * @param array $model_array
 *   The model object (in array form) for which to add/edit the template auth
 *   schemes.
 *
 * @return array
 *   The template auth form.
 */
function smartdocs_model_template_auth($form, &$form_state, array $model_array) {

  $model = new Model(devconnect_default_org_config());
  Model::fromArray($model, $model_array);

  $log_transaction_time = variable_get('smartdocs_log_transaction_time', 0);

  $rev = $model->getLatestRevisionNumber();
  if ($rev == -1) {
    // If there is no revision for a model there won't be any security schemes
    // and corresponding template auth scheme so we return.
    $form['error_message'] = array(
      '#type' => 'markup',
      '#markup' => t('Model ‘!model’ does not yet contain any revisions.', array('!model' => $model->getDisplayName())),
    );
    return $form;
  }
  $revision = new Revision(devconnect_default_org_config(), $model->getUuid());
  // TODO: Wrap this in a try/catch.
  $revision->load($rev);
  $security = new Security(devconnect_default_org_config(), $model->getUuid(), $revision->getUuid());
  $start = microtime(TRUE);
  $schemes = $security->loadAllSchemes();
  if ($log_transaction_time) {
    $args = array(
      '!model' => $model->getUuid(),
      '!revision' => $revision->getRevisionNumber(),
      '!time' => number_format(microtime(TRUE) - $start, 4),
    );
    watchdog('smartdocs', 'Loaded all security schemes for revision !revision of model !model in !time seconds', $args, WATCHDOG_INFO);
  }

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
    '#markup' => t('This section allows you to set default authentication values for SmartDocs methods when sending a request. Leave these blank to have the user set the values on the method page.  If you want to For additional help in configuring template authentication schemes, consult the <a href="@doc_url">online documentation</a>. To manage the security objects, go to the <a href="@sec_url">security settings</a>.', $markup_args),
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

  $start = microtime(TRUE);
  $template_auth_schemes = $template_auth->loadAllSchemes();
  if ($log_transaction_time) {
    $args = array(
      '!model' => $model->getUuid(),
      '!time' => number_format(microtime(TRUE) - $start, 4),
    );
    watchdog('smartdocs', 'Loaded all template auth schemes for model !model in !time seconds', $args, WATCHDOG_INFO);
  }
  /** @var TemplateAuthScheme $scheme */
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

    if ($scheme instanceof Oauth2Scheme) {
      $form['auth_schemes']['template_auth_schemes'][$scheme_name]['type_display'] = array(
        '#type' => 'markup',
        '#markup' => '<div class="templateauth-type"><span>Type:</span> Oauth2 </div>',
      );
      $form['auth_schemes']['template_auth_schemes'][$scheme_name]['clientId'] = array(
        '#type' => 'textfield',
        '#title' => t('Client Id'),
        '#required' => FALSE,
        '#default_value' => $template_auth_scheme instanceof Oauth2TemplateAuthScheme ? $template_auth_scheme->getClientId() : '',
      );
      $form['auth_schemes']['template_auth_schemes'][$scheme_name]['clientSecret'] = array(
        '#type' => 'textfield',
        '#title' => t('Client Secret'),
        '#required' => FALSE,
        '#default_value' => !empty($template_auth_scheme) ? $template_auth_scheme->getClientSecret() : '',
      );
      if ($template_auth_scheme instanceof Oauth2TemplateAuthScheme) {
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
    }
    elseif ($scheme instanceof ApiKeyScheme) {
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
        '#default_value' => $template_auth_scheme instanceof ApiKeyTemplateAuthScheme ? $template_auth_scheme->getKeyValue() : '',
        '#required' => FALSE,
      );
    }
    elseif ($scheme instanceof BasicScheme) {
      $form['auth_schemes']['template_auth_schemes'][$scheme_name]['type_display'] = array(
        '#type' => 'markup',
        '#markup' => '<div class="templateauth-type"><span>' . t('Type:') . '</span> ' . t('Basic') . ' </div>',
      );
      $form['auth_schemes']['template_auth_schemes'][$scheme_name]['noconfig'] = array(
        '#type' => 'markup',
        '#markup' => '<div class="form-item">' . t('No configuration options.') . '</div>',
      );
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
      module_invoke_all('smartdocs_template_auth_scheme_presave', $template_auth, $scheme, $model_id, $is_update);
      $start = microtime(TRUE);
      $template_auth->save($scheme, $is_update);
      if (variable_get('smartdocs_log_transaction_time', 0)) {
        $args = array(
          '!model' => $model_id,
          '!scheme' => $scheme->getName(),
          '!time' => number_format(microtime(TRUE) - $start, 4),
        );
        watchdog('smartdocs', 'Saved template auth scheme !scheme to model !model in !time seconds', $args, WATCHDOG_INFO);
      }
      module_invoke_all('smartdocs_template_auth_scheme_postsave', $template_auth, $scheme, $model_id, $is_update);
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
  return $form['templates'][$name]['method_template'];
}


/**
 * Model delete confirmation form.
 *
 * @param array $form
 *   Descriptor of the form being created.
 * @param array $form_state
 *   State of the form being created.
 * @param array $model_array
 *   The Model (in array form) to be deleted.
 *
 * @return array
 *   The newly-created form.
 */
function smartdocs_model_delete(array $form, array &$form_state, array $model_array) {

  $breadcrumb = array();
  $breadcrumb[] = l(t('Home'), '<front>');
  $breadcrumb[] = l(t('Content'), 'admin/content');
  $breadcrumb[] = l(t('SmartDocs'), 'admin/content/smartdocs/models');
  $breadcrumb[] = l(t('!model', array('!model' => $model_array['displayName'])), 'admin/content/smartdocs/models/' . $model_array['id']);
  drupal_set_breadcrumb($breadcrumb);

  $form['model'] = array(
    '#type' => 'value',
    '#value' => $model_array,
  );
  $ques = t('Are you sure you want to delete the model %name?', array('%name' => $model_array['displayName']));
  $desc = t('Deleting the model %name will delete the all content from Dev Portal and remove the model from the Edge Management server.  This cannot be undone.  If you want make this documentation unavailable to end users, unpublish the nodes instead.', array('%name' => $model_array['displayName']));
  return confirm_form($form, $ques, 'admin/content/smartdocs/models/' . $model_array['id'], $desc, t('Delete'), t('Cancel'));
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
  $model_uuid = $model->getUuid();
  module_invoke_all('smartdocs_model_predelete', $model_name);

  try {
    $start = microtime(TRUE);
    $model->delete();
    if (variable_get('smartdocs_log_transaction_time', 0)) {
      $args = array(
        '!model' => $model_uuid,
        '!time' => number_format(microtime(TRUE) - $start, 4),
      );
      watchdog('smartdocs', 'Deleted model !model in !time seconds', $args, WATCHDOG_INFO);
    }
    module_invoke_all('smartdocs_model_postdelete', $model_name);

    // Delete all nodes in this model.
    $model_terms = taxonomy_get_term_by_name($model_name, 'smartdocs_models');
    if (!empty($model_terms)) {
      $model_term = reset($model_terms);

      $query = new EntityFieldQuery();
      $result = $query->entityCondition('entity_type', 'node')
        ->entityCondition('bundle', 'smart_method')
        ->fieldCondition('field_smart_method_model', 'tid', $model_term->tid, '=')
        ->execute();

      if (!empty($result['node'])) {
        $nids = array_keys($result['node']);
        db_delete('smartdata')
          ->condition('nid', $nids)
          ->execute();
        node_delete_multiple($nids);
      }

      taxonomy_term_delete($model_term->tid);
    }

    // Remove any lingering views.
    $view_name = $model_name . '_methods';
    $view = views_get_view($view_name);
    if ($view) {
      views_delete_view($view);
    }
    // Make sure ctools is not caching the old view.
    db_delete('ctools_object_cache')->condition('name', $view_name)->execute();

    menu_rebuild();
    drupal_flush_all_caches();
    drupal_set_message(t('%model has been deleted along with all of its associated content.', array('%model' => $model->getDisplayName())), 'status');
    $form_state['redirect'] = 'admin/content/smartdocs/models';
  }
  catch (Exception $e) {
    drupal_set_message('There was an error deleting the model(s).', 'error');
  }
}

/**
 * Page callback to display list of revisions for a given model.
 *
 * @param array $model_array
 *   The Model (in array form) containing the revisions to be listed.
 *
 * @return mixed
 *   HTML string or render-array.
 */
function smartdocs_revision_list(array $model_array) {
  try {
    $revision_obj = new Revision(devconnect_default_org_config(), $model_array['id']);
    $start = microtime(TRUE);
    $revisions = $revision_obj->listRevisions();
    if (variable_get('smartdocs_log_transaction_time', 0)) {
      $args = array(
        '!model' => $model_array['id'],
        '!time' => number_format(microtime(TRUE) - $start, 4),
      );
      watchdog('smartdocs', 'Listed revisions for model !model in !time seconds', $args, WATCHDOG_INFO);
    }
  }
  catch (ResponseException $e) {
    return array(
      'error' => array(
        '#type' => 'markup',
        '#markup' => t('An error occurred while fetching all revisions for model !model.', array('!model' => $model_array['displayName'])),
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
  $breadcrumb[] = l(t('!model', array('!model' => $model_array['displayName'])), 'admin/content/smartdocs/models/' . $model_array['id']);
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
  foreach ($revisions as $revision) {
    $links = array();
    foreach ($operations as $key => $op) {
      $links[$key . '-' . $revision->getUuid()] = array(
        'title' => $op,
        'href' => 'admin/content/smartdocs/models/' . $model_array['id'] . '/revisions/' . $revision->getRevisionNumber() . $key,
        'attributes' => array(
          'title' => $op,
          'class' => array(
            $key . '-' . $revision->getUuid(),
            'views-ajax-link',
          ),
        ),
      );
      if ($op == 'Security Settings') {
        // Removed 'models' from URL since menu routing can only be 9 items.
        // See MENU_MAX_PARTS in menu.inc.
        $links[$key . '-' . $revision->getUuid()]['href'] = 'admin/content/smartdocs/' . $model_array['id'] . '/revisions/' . $revision->getRevisionNumber() . $key;
      }
    }

    $caption = 'Revision #@rev';
    if ($is_latest_revision) {
      $caption .= ' (latest)';
      $is_latest_revision = FALSE;
    }

    $rows[] = array(
      l(t($caption, array('@rev' => $revision->getRevisionNumber())), 'admin/content/smartdocs/models/' . $model_array['id'] . '/revisions/' . $revision->getRevisionNumber()),
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
 * @param array $model_array
 *   The Model (in array form) to which a new revision is to be added.
 */
function smartdocs_add_revision(array $form, array $form_state, array $model_array) {
  drupal_set_title(t('Add Revision to ‘!model’', array('!model' => $model_array['displayName'])));
  $breadcrumb = array();
  $breadcrumb[] = l(t('Home'), '<front>');
  $breadcrumb[] = l(t('Content'), 'admin/content');
  $breadcrumb[] = l(t('SmartDocs'), 'admin/content/smartdocs/models');
  $breadcrumb[] = l(t('!model', array('!model' => $model_array['displayName'])), 'admin/content/smartdocs/models/' . $model_array['id']);
  drupal_set_breadcrumb($breadcrumb);

  $form['model'] = array(
    '#type' => 'value',
    '#value' => $model_array,
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
  module_invoke_all('smartdocs_revision_presave', $revision, $model);
  try {
    $start = microtime(TRUE);
    $revision->save();
    if (variable_get('smartdocs_log_transaction_time', 0)) {
      $args = array(
        '!model' => $model->getUuid(),
        '!revision' => $revision->getRevisionNumber(),
        '!time' => number_format(microtime(TRUE) - $start, 4),
      );
      watchdog('smartdocs', 'Created revision !revision for model !model in !time seconds', $args, WATCHDOG_INFO);
    }
    module_invoke_all('smartdocs_revision_postsave', $revision, $model);
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
 * @param array $model_array
 *   Model (in array form) containing the revision to be edited.
 * @param array $revision_array
 *   Revision (in array form) to be edited.
 *
 * @return array
 *   Descriptor of the newly-created form.
 */
function smartdocs_revision_detail(array $form, array &$form_state, array $model_array, array $revision_array) {

  $form['#attached'] = array(
    'css' => array(
      drupal_get_path('module', 'views') . '/css/views-admin.ctools.css',
      drupal_get_path('module', 'smartdocs') . '/css/smartdocs.css',
    ),
    'js' => array(
      drupal_get_path('module', 'smartdocs') . '/js/check-uncheck-all.js',
      'misc/tableselect.js',
    ),
    'library' => array(
      array('system', 'drupal.collapse'),
    ),
  );
  $rev_number = $revision_array['revisionNumber'];

  try {
    $security = new Security(devconnect_default_org_config(), $model_array['id'], $revision_array['id']);
    $start = microtime(TRUE);
    $security_schemes = $security->loadAllSchemes();
    if (variable_get('smartdocs_log_transaction_time', 0)) {
      $args = array(
        '!model' => $model_array['id'],
        '!revision' => $rev_number,
        '!time' => number_format(microtime(TRUE) - $start, 4),
      );
      watchdog('smartdocs', 'Loaded all security schemes for revision !revision of model !model in !time seconds', $args, WATCHDOG_INFO);
    }
  }
  catch (ResponseException $e) {
    $security_schemes = array();
  }
  $security_scheme_display = array();
  foreach ($security_schemes as $security_scheme) {
    $security_scheme_display[$security_scheme->getName()] = $security_scheme->getName() . ' (' . $security_scheme->getType(TRUE) . ')';
  }

  // TODO: find way to move these page-specific items out of form constructor.
  drupal_set_title(t('‘!model’ API Revision #@rev', array('!model' => $model_array['displayName'], '@rev' => $rev_number)));
  $breadcrumb = array();
  $breadcrumb[] = l(t('Home'), '<front>');
  $breadcrumb[] = l(t('Content'), 'admin/content');
  $breadcrumb[] = l(t('SmartDocs'), 'admin/content/smartdocs/models');
  $breadcrumb[] = l(t('!model', array('!model' => $model_array['displayName'])), 'admin/content/smartdocs/models/' . $model_array['id']);
  $breadcrumb[] = l(t('Revisions'), 'admin/content/smartdocs/models/' . $model_array['id'] . '/revisions');
  $breadcrumb[] = l(t('Revision #@rev', array('@rev' => $rev_number)), 'admin/content/smartdocs/models/' . $model_array['id'] . '/revisions/' . $rev_number);
  drupal_set_breadcrumb($breadcrumb);

  $nids = db_select('smartdata', 's')
    ->condition('model', $model_array['name'])
    ->fields('s', array('nid'))
    ->execute();
  $nodes = array();
  foreach ($nids as $nid) {
    $nodes[$nid->nid] = $model_array;
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

    if ($op == 'Security Settings') {
      // Removed 'models' from URL since menu routing can only be 9 items.
      // See MENU_MAX_PARTS in menu.inc.
      $links[$key]['href'] = 'admin/content/smartdocs/' . $model_array['id'] . '/' . $key;
    }
    else {
      $links[$key]['href'] = 'admin/content/smartdocs/models/' . $model_array['id'] . '/' . $key;
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
    $entity['displayName'] = $model_array['displayName'];
    $entity['name'] = $model_array['name'];

    $resources = $revision_array['resources'];
    if (empty($resources)) {
      unset($form['options']);
      $form['resources'] = array(
        '#markup' => t('There is currently no data in the model.'),
      );
      return $form;
    }
    $form['resources'] = array();
    foreach ($resources as $resource) {
      $resource_id = $resource['id'];
      $links = array();
      $links['add-' . $resource_id] = array(
        'title' => t('Add method'),
        'href' => 'node/add/smart-method',
        'query' => array(
          'model' => $model_array['name'],
          'revision' => $revision_array['revisionNumber'],
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
        'href' => 'admin/content/smartdocs/models/' . $model_array['id'] . '/resource/edit/' . $rev_number . '/' . $resource_id,
        'attributes' => array(
          'title' => t('Edit resource'),
          'class' => array(
            'edit-' . $resource_id,
            'views-ajax-link',
          ),
        ),
      );
      $links['delete-' . $resource_id] = array(
        'title' => t('Delete resource'),
        'href' => 'admin/content/smartdocs/models/' . $model_array['id'] . '/resource/delete/' . $rev_number . '/' . $resource_id,
        'attributes' => array(
          'title' => t('Delete resource'),
          'class' => array(
            'delete-' . $resource_id,
            'views-ajax-link',
          ),
        ),
      );

      $resource_set = array(
        'ops' => array('#markup' => theme('links__ctools_dropbutton', array('links' => $links))),
        'methods' => array('#tree' => TRUE),
        '#title' => $resource['baseUrl'] . $resource['path'],
      );

      $has_tax = FALSE;

      foreach ($resource['methods'] as $method) {
        $method_count++;

        $tags = $method['tags'];
        if (!empty($tags)) {
          $has_tax = TRUE;
        }

        $query = db_select('smartdata', 's');
        $query->innerJoin('node', 'n', 's.nid = n.nid');
        $row = $query->fields('s', array('nid', 'synced'))
          ->fields('n', array('status'))
          ->condition('s.method', $method['id'])
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
        else {
          $link = NULL;
          $edit_method_url = NULL;
        }

        if ($has_node && $published) {
          $published_count++;
        }

        $ops = array();
        if ($has_node) {
          $ops['edit-method-' . $method['id']] = array(
            'title' => t('Edit method'),
            'href' => $edit_method_url,
            'attributes' => array(
              'title' => t('Edit method'),
              'class' => array(
                'edit-method-' . $method['id'],
                'views-ajax-link',
              ),
            ),
          );

          $op_title = $published ? t('Unpublish node') : t('Publish node');

          $ops['toggle-status-method-' . $method['id']] = array(
            'title' => $op_title,
            'href' => 'admin/content/smartdocs/method-toggle-status/' . $model_array['id'] . '/' . $rev_number . '/' . $resource['id'] . '/' . $method['id'],
            'attributes' => array(
              'title' => $op_title,
              'class' => array(
                'toggle-status-method-' . $method['id'],
                'views-ajax-link',
              ),
            ),
          );
        }
        $ops['delete-method-' . $method['id']] = array(
          'title' => t('Delete method'),
          'href' => 'admin/content/smartdocs/method-delete/' . $model_array['id'] . '/' . $rev_number . '/' . $resource['id'] . '/' . $method['id'],
          'attributes' => array(
            'title' => t('Delete method'),
            'class' => array(
              'delete-method-' . $method['id'],
              'views-ajax-link',
            ),
          ),
        );

        $auth = array();
        foreach ($method['security'] as $security_scheme_name) {
          if (array_key_exists($security_scheme_name, $security_scheme_display)) {
            $auth[] = $security_scheme_display[$security_scheme_name];
          }
        }

        if (empty($method['displayName'])) {
          $method['displayName'] = $method['name'];
        }

        $resource_set['methods'][$method['id']] = array(
          '#type' => 'checkbox',
          '#return_value' => $method['id'],
          '#data' => array(
            'tax' => (!empty($tags) ? implode(', ', $tags) : ''),
            'name' => check_plain($method['displayName']),
            'description' => filter_xss($method['description']),
            'method' => $method['verb'],
            'auth' => implode(', ', $auth),
            'node' => ($has_node ? $link : '<span style="color:red;">No</span>'),
            'status' => ($published ? '<span style="color:green;">Yes</span>' : '<span style="color:red;">No</span>'),
            'synced' => ($synced ? '<span style="color:green;">Yes</span>' : '<span style="color:red;">No</span>'),
            'operations' => theme('links__ctools_dropbutton', array('links' => $ops)),
          ),
        );
      }
      if (count($resource['methods']) > 0) {
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

  if ($published_count && $view_path = _smartdocs_get_revision_view_path($model_array)) {
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
    '#value' => smartdocs_prepare_revision($revision_array),
  );
  $form['model'] = array(
    '#type' => 'value',
    '#value' => array(
      'id' => $model_array['id'],
      'name' => $model_array['name'],
      'displayName' => $model_array['displayName'],
      'description' => $model_array['description'],
    ),
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
  $model_array = $form_state['values']['model'];

  switch ($form_state['values']['operation']) {
    case 'render_nodes':
    case 'render_publish_nodes':
      $revision_array = $form_state['values']['revision'];
      $selected = $form_state['values']['methods'];
      $options = array('publish' => ($form_state['values']['operation'] == 'render_publish_nodes'));

      module_load_include('inc', 'smartdocs', 'batch/smartdocs.render');
      batch_set(smartdocs_render($model_array, $revision_array, $selected, $options, FALSE));
      // Start batch process manually so FAPI will not include form state, which may be very large.
      batch_process(drupal_get_destination());
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
 * @param array $model
 *   Model to which the resource belongs.
 * @param array $revision
 *   Revision to which the resource belongs.
 * @param array|null $resource
 *   Resource being edited, or NULL if a new resource is being created.
 *
 * @return array
 *   The newly-created form.
 */
function smartdocs_resource_edit(array $form, array &$form_state, array $model, array $revision, array $resource = NULL) {

  $display_name = $model['displayName'];
  // TODO: find way to move these page-specific items out of form constructor.
  if (isset($resource)) {
    drupal_set_title(t('Edit resource from ‘!model’', array('!model' => $display_name)));
  }
  else {
    drupal_set_title(t('Add resource to ‘!model’', array('!model' => $display_name)));
  }
  $breadcrumb = array();
  $breadcrumb[] = l(t('Home'), '<front>');
  $breadcrumb[] = l(t('Content'), 'admin/content');
  $breadcrumb[] = l(t('SmartDocs'), 'admin/content/smartdocs/models');
  $breadcrumb[] = l($display_name, 'admin/content/smartdocs/models/' . $model['id']);
  $breadcrumb[] = l(t('API Revisions'), 'admin/content/smartdocs/models/' . $model['id'] . '/revisions/');
  $breadcrumb[] = l(t('Revision #!revision', array('!revision' => $revision['revisionNumber'])), 'admin/content/smartdocs/models/' . $model['id'] . '/revisions/' . $revision['revisionNumber']);
  drupal_set_breadcrumb($breadcrumb);

  $version = variable_get('smartdocs_local_asset_version', '6');
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
    '#value' => $model,
  );
  $form['revision'] = array(
    '#type' => 'value',
    '#value' => $revision,
  );
  $form['resource'] = array(
    '#type' => 'value',
    '#value' => (isset($resource) ? $resource : NULL),
  );
  $form['display_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Display Name'),
    '#default_value' => isset($resource) ? $resource['displayName'] : NULL,
    '#size' => 60,
    '#maxlength' => 128,
  );
  $form['path'] = array(
    '#type' => 'textfield',
    '#title' => t('Path'),
    '#default_value' => isset($resource) ? $resource['path'] : NULL,
    '#description' => t('Provide a path with dynamic values, ex: %e', array('%e' => '/consoles/{name}')),
    '#size' => 60,
    '#maxlength' => 128,
  );
  $form['description'] = array(
    '#type' => 'textarea',
    '#default_value' => isset($resource) ? $resource['description'] : NULL,
    '#title' => t('Description'),
    '#rows' => 5,
  );

  $form['parameters'] = array(
    '#type' => 'textarea',
    '#title' => t('Parameters'),
    '#default_value' => isset($resource) && count($resource['parameters']) > 0 ? smartdocs_format_json($resource['parameters']) : NULL,
  );

  $example = <<<EOM
<pre>[
  {
    &quot;dataType&quot;:&quot;string&quot;,
    &quot;type&quot;:&quot;query&quot;,
    &quot;name&quot;:&quot;shirtsize&quot;,
    &quot;default&quot;:&quot;lower&quot;,
    &quot;description&quot;:&quot;Shirt size.&quot;,
    &quot;options&quot;:[&quot;s&quot;,&quot;m&quot;,&quot;l&quot;]
  }
]</pre>
EOM;

  $form['examples'] = array(
    '#type' => 'fieldset',
    '#title' => t('Examples'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['examples']['ex'] = array(
    '#markup' => $example,
  );
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
  module_invoke_all('smartdocs_resource_presave', $resource, $revision, $model, $update);

  try {
    $start = microtime(TRUE);
    $resource->save($update);
    if (variable_get('smartdocs_log_transaction_time', 0)) {
      $args = array(
        '!model' => $model->getUuid(),
        '!revision' => $revision->getRevisionNumber(),
        '!resource' => $resource->getUuid(),
        '!time' => number_format(microtime(TRUE) - $start, 4),
      );
      watchdog('smartdocs', 'Saved resource !resource of revision !revision of model !model in !time seconds', $args, WATCHDOG_INFO);
    }
    if ($update) {
      drupal_set_message(t('Resource has been updated successfully.'));
    }
    else {
      drupal_set_message(t('Resource has been created successfully.'));
    }
    module_invoke_all('smartdocs_resource_postsave', $resource, $revision, $model, $update);
    module_invoke_all('smartdocs_model_update', $model->getUuid());
    $form_state['redirect'] = 'admin/content/smartdocs/models/' . $model->getUuid() . '/revisions/' . $revision->getRevisionNumber();
  }
  catch (Exception $e) {
    drupal_set_message(t('Resource failed to save, please consult the logs.'), 'error');
  }
}

/**
 * Constructor for delete-resource confirmation form.
 *
 * @param array $form
 *   The form being constructed.
 * @param array $form_state
 *   The state of the form being constructed.
 * @param array $model
 *   The model from which a resource is being deleted.
 * @param array $revision
 *   The revision from which a resource is being deleted.
 * @param array $resource
 *   The resource to be deleted.
 *
 * @return array
 *   Render-array of the form.
 */
function smartdocs_resource_delete(array $form, array &$form_state, array $model, array $revision, array $resource) {

  $method_uuids = $method_nids = array();
  foreach ($resource['methods'] as $method) {
    $method_uuids[] = $method['id'];
  }
  $description = NULL;
  if (count($method_uuids) > 0) {
    $query = db_select('smartdata', 's');
    $query->innerJoin('node', 'n', 's.nid = n.nid');
    $query->fields('n', array('nid'));
    $query->fields('s', array('method'));
    $query->condition('s.model', $model['name'])
      ->condition('s.revision', $revision['revisionNumber'])
      ->condition('s.resource', $resource['id'])
      ->condition('s.method', $method_uuids);
    $method_nids = $query->execute()->fetchAllKeyed();
    $method_uuids = array_flip($method_nids);
    if (count($method_nids) == 0) {
      $description = t('This resource contains %methods methods.', array('%methods' => count($method_uuids)));
    }
    else {
      $description = t('This resource contains %methods methods, of which %nids are rendered as Drupal nodes. Deleting this resource will also delete these methods and their Drupal content.',
          array('%methods' => count($method_uuids), '%nids' => count($method_nids))
      );
    }
  }
  $question = t('Are you sure you wish to delete the %resourcename resource?', array('%resourcename' => $resource['displayName']));
  $cancel_path = 'admin/content/smartdocs/models/' . $model['id'] . '/revisions/' . $revision['revisionNumber'];

  $form['model'] = array(
    '#type' => 'value',
    '#value' => array('uuid' => $model['id'], 'name' => $model['name']),
  );
  $form['revision'] = array(
    '#type' => 'value',
    '#value' => array('uuid' => $revision['id'], 'num' => $revision['revisionNumber']),
  );
  $form['resource'] = array(
    '#type' => 'value',
    '#value' => array('uuid' => $resource['id'], 'name' => $resource['displayName']),
  );

  $form['method_nids'] = array(
    '#type' => 'value',
    '#value' => $method_nids,
  );

  $form['method_uuids'] = array(
    '#type' => 'value',
    '#value' => $method_uuids,
  );

  return confirm_form($form, $question, $cancel_path, $description);
}

/**
 * Submit handler for the smartdocs_resource_delete form.
 *
 * Deletes the specified resource from its revision, and deletes method nodes
 * pertaining thereunto, if any exist.
 *
 * @param array $form
 *   The form being submitted.
 * @param array $form_state
 *   The state of the form being submitted.
 */
function smartdocs_resource_delete_submit($form, &$form_state) {
  $model_uuid = $form_state['values']['model']['uuid'];
  $model_name = $form_state['values']['model']['name'];
  $revision_uuid = $form_state['values']['revision']['uuid'];
  $revision_number = $form_state['values']['revision']['num'];
  $resource_uuid = $form_state['values']['resource']['uuid'];
  $resource_name = $form_state['values']['resource']['name'];

  $resource = new Resource(devconnect_default_org_config(), $model_uuid, $revision_uuid);
  module_invoke_all('smartdocs_resource_predelete', $resource, $revision_uuid, $model_name);
  $deleted_nodes = 0;
  try {
    $resource->delete($resource_uuid);
    foreach ($form_state['values']['method_nids'] as $nid) {
      module_invoke_all('smartdocs_method_node_predelete', $form_state['values']['method_uuids'][$nid], $nid, $resource, $revision_uuid, $model_name);
      node_delete($nid);
      module_invoke_all('smartdocs_method_node_postdelete', $form_state['values']['method_uuids'][$nid], $nid, $resource, $revision_uuid, $model_name);
      $deleted_nodes++;
    }

    db_delete('smartdata')
      ->condition('model', $model_name)
      ->condition('revision', $revision_number)
      ->condition('resource', $resource_uuid)
      ->execute();

    $message = t('Resource %resource has been deleted.', array('%resource' => $resource_name));
    if ($deleted_nodes) {
      $message .= ' ' . t('%count nodes were also deleted.', array('%count' => $deleted_nodes));
    }
    drupal_set_message($message);
  }
  catch (Exception $e) {
    drupal_set_message(t('Failed to delete resource %resource. Please examine the error logs for details.', array('%resource' => $resource_name)), 'error');
  }
  module_invoke_all('smartdocs_resource_postdelete', $resource, $revision_uuid, $model_name);
  $form_state['redirect'] = 'admin/content/smartdocs/models/' . $model_uuid . '/revisions/' . $revision_number;
}

/**
 * Page callback to publish unpublished method nodes, and vice versa.
 *
 * @param array $model
 *   The Model to which the node belongs.
 * @param array $revision
 *   The Revision to which the node belongs.
 * @param array $resource
 *   The Resource to which the node belongs.
 * @param array $method
 *   The Method corresponding to the node whose status is to be toggled.
 */
function smartdocs_toggle_node_status(array $model, array $revision, array $resource, array $method) {
  $nid = _smartdocs_get_method_node($method);
  if ($nid) {
    $node = node_load($nid);
    if ($node) {
      $node->status = 1 - $node->status;
      node_save($node);
      drupal_set_message(t('The node associated with the method ‘@method’ has been @action.', array(
        '@method' => $method['displayName'],
        '@action' => $node->status ? t('published') : t('unpublished'),
      )));
    }
    else {
      drupal_set_message(t('The node associated with method ‘@method’ could not be found.', array('@method' => $method['displayName'])), 'error');
    }
  }
  else {
    drupal_set_message(t('No node is associated with method ‘@method’.', array('@method' => $method['displayName'])), 'error');
  }
  drupal_goto('admin/content/smartdocs/models/' . $model['id'] . '/revisions/' . $revision['revisionNumber']);
}

/**
 * Form constructor confirming deletion of a method.
 *
 * @param array $form
 *   Descriptor of the form being created.
 * @param array $form_state
 *   State of the form being created.
 * @param array $model
 *   Model from which a method will be deleted.
 * @param array $revision
 *   Revision from which a method will be deleted.
 * @param array $resource
 *   Resource from which a method will be deleted.
 * @param array $method
 *   The method which is to be deleted.
 *
 * @return array
 *   The form structure to be presented to the user.
 */
function smartdocs_method_delete(array $form, array &$form_state, array $model, array $revision, array $resource, array $method) {
  // TODO: find way to move these page-specific items out of form constructor.
  drupal_set_title(t('About to delete method ‘@method’ of ‘!model’ model', array(
    '@method' => $method['displayName'],
    '!model' => $model['displayName'],
  )));

  $breadcrumb = array();
  $breadcrumb[] = l(t('Home'), '<front>');
  $breadcrumb[] = l(t('Administration'), 'admin');
  $breadcrumb[] = l(t('SmartDocs'), 'admin/content/smartdocs/models');
  $breadcrumb[] = l($model['displayName'], 'admin/content/smartdocs/models/' . $model['id']);
  $breadcrumb[] = l(t('API Revisions'), 'admin/content/smartdocs/models/' . $model['id'] . '/revisions/');
  $breadcrumb[] = l(t('Revision #@revision', array('@revision' => $revision['revisionNumber'])), 'admin/content/smartdocs/models/' . $model['id'] . '/revisions/' . $revision['revisionNumber']);
  drupal_set_breadcrumb($breadcrumb);

  $form['revision_url'] = array(
    '#type' => 'value',
    '#value' => 'admin/content/smartdocs/models/' . $model['id'] . '/revisions/' . $revision['revisionNumber'],
  );
  $form['model_uuid'] = array(
    '#type' => 'value',
    '#value' => $model['id'],
  );
  $form['revision_uuid'] = array(
    '#type' => 'value',
    '#value' => $revision['id'],
  );
  $form['resource_uuid'] = array(
    '#type' => 'value',
    '#value' => $resource['id'],
  );
  $form['method'] = array(
    '#type' => 'value',
    '#value' => $method,
  );
  $form['nid'] = array(
    '#type' => 'value',
    '#value' => NULL,
  );
  $form['warning'] = array(
    '#type' => 'markup',
    '#markup' => '<p><strong>' . t('Warning:') . '</strong> ' . t('You are about to remove the method ‘@method’ from the Modeling API.', array('@method' => $method['displayName'])) . '</p>',
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
  $action = $form_state['values']['node_action'];
  $model_uuid = $form_state['values']['model_uuid'];
  $revision_uuid = $form_state['values']['revision_uuid'];
  $resource_uuid = $form_state['values']['resource_uuid'];

  $nid = $form_state['values']['nid'];
  if ($nid) {
    module_invoke_all('smartdocs_method_node_preaction', $method, $nid, $model_uuid, $revision_uuid, $resource_uuid, $action);
    switch ($action) {
      case 'unpublish':
      case 'keep':
        db_delete('smartdata')->condition('nid', $nid)->execute();
        if ($action == 'unpublish') {
          $node = node_load($nid);
          $node->status = 0;
          node_save($node);
        }
        break;

      case 'delete':
        module_invoke_all('smartdocs_method_node_predelete', $method, $nid, $model_uuid, $revision_uuid, $resource_uuid);
        db_delete('smartdata')->condition('nid', $nid)->execute();
        node_delete($nid);
        module_invoke_all('smartdocs_method_node_postdelete', $method, $nid, $model_uuid, $revision_uuid, $resource_uuid);
        break;
    }
    module_invoke_all('smartdocs_method_node_postaction', $method, $nid, $model_uuid, $revision_uuid, $resource_uuid, $action);
  }
  try {
    module_invoke_all('smartdocs_method_predelete', $method, $model_uuid, $revision_uuid, $resource_uuid);
    $start = microtime(TRUE);
    $method->delete();
    if (variable_get('smartdocs_log_transaction_time', 0)) {
      $args = array(
        '!method' => $method->getUuid(),
        '!resource' => $form_state['values']['resource_uuid'],
        '!revision' => $form_state['values']['revision_uuid'],
        '!model' => $form_state['values']['model_uuid'],
        '!time' => number_format(microtime(TRUE) - $start, 4),
      );
      watchdog('smartdocs', 'Deleted method !method from resource !resource of revision !revision of model !model in !time seconds', $args, WATCHDOG_INFO);
    }
    module_invoke_all('smartdocs_model_update', $form_state['values']['model_uuid']);
    module_invoke_all('smartdocs_method_postdelete', $method, $form_state['values']['model_uuid'], $form_state['values']['revision_uuid'], $form_state['values']['resource_uuid']);
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
 * @param array $model
 *   The model object to add the scheme for.
 * @param array $revision
 *   The revision object of the model to add the scheme for.
 * @param string $scheme_name
 *   The name of the security scheme, this is passed in while editing a scheme.
 *
 * @return array
 *   The form array.
 */
function smartdocs_model_security_scheme(array $form, array &$form_state, array $model, array $revision, $scheme_name = NULL) {
  $form['model'] = array(
    '#type' => 'value',
    '#value' => $model,
  );
  $form['revision'] = array(
    '#type' => 'value',
    '#value' => $revision,
  );
  $form['#attached'][] = 'misc/machine-name.js';

  $type = !empty($form_state['values']['type']) ? $form_state['values']['type'] : NULL;

  // If we are editing a scheme then we set the type to the type of the scheme
  // being edited.
  if (!empty($scheme_name)) {
    $security = new Security(devconnect_default_org_config(), $model['id'], $revision['id']);
    $start = microtime(TRUE);
    $scheme = $security->load($scheme_name);
    if (variable_get('smartdocs_log_transaction_time', 0)) {
      $args = array(
        '!model' => $model['id'],
        '!revision' => $revision['revisionNumber'],
        '!scheme' => $scheme->getName(),
        '!time' => number_format(microtime(TRUE) - $start, 4),
      );
      watchdog('smartdocs', "Loaded security scheme !scheme for revision !revision of model !model in !time seconds", $args, WATCHDOG_INFO);
    }
    $type = $scheme->getType();
  }
  else {
    $scheme = NULL;
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
          '#default_value' => $scheme instanceof Oauth2Scheme ? $scheme->getGrantType() : 'AUTHORIZATION_CODE',
        );
        $form['auth_fields']['authorizationUrl'] = array(
          '#type' => 'textfield',
          '#title' => t('Authorization URL'),
          '#required' => TRUE,
          '#default_value' => $scheme instanceof Oauth2Scheme ? $scheme->getAuthorizationUrl() : '',
        );
        $form['auth_fields']['accessTokenUrl'] = array(
          '#type' => 'textfield',
          '#title' => t('Access token url'),
          '#default_value' => $scheme instanceof Oauth2Scheme ? $scheme->getAccessTokenUrl() : '',
        );
        $form['auth_fields']['advanced_settings']['authorizationVerb'] = array(
          '#type' => 'radios',
          '#title' => t('Authorization verb'),
          '#options' => array(
            'GET' => t('GET'),
            'POST' => t('POST'),
          ),
          '#required' => TRUE,
          '#default_value' => $scheme instanceof Oauth2Scheme ? $scheme->getAuthorizationVerb() : 'GET',
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
          '#default_value' => $scheme instanceof Oauth2Scheme ? $scheme->getIn() : 'HEADER',
        );
        $form['auth_fields']['advanced_settings']['accessTokenParamName'] = array(
          '#type' => 'textfield',
          '#title' => t('Access token parameter name'),
          '#default_value' => $scheme instanceof Oauth2Scheme ? $scheme->getAccessTokenParamName() : 'access_token',
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
          '#default_value' => $scheme instanceof Oauth2Scheme ? json_encode((object) $scheme->getScopes()) : '{}',
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
          '#default_value' => $scheme instanceof ApiKeyScheme ? $scheme->getParamName() : '',
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
          '#default_value' => $scheme instanceof ApiKeyScheme ? $scheme->getIn() : 'HEADER',
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

  $payload = array();
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
    drupal_alter('smartdocs_model_security_scheme', $payload);
    $scheme = SecurityScheme::fromArray($payload);
    module_invoke_all('smartdocs_model_security_scheme_presave', $scheme, $model, $revision, $is_update);
    $start = microtime(TRUE);
    $security->save($scheme, $is_update);
    if (variable_get('smartdocs_log_transaction_time', 0)) {
      $args = array(
        '!model' => $model->getUuid(),
        '!revision' => $revision->getRevisionNumber(),
        '!scheme' => $scheme->getName(),
        '!time' => number_format(microtime(TRUE) - $start, 4),
      );
      watchdog('smartdocs', 'Saved security scheme !scheme to revision !revision of model !model in !time seconds', $args, WATCHDOG_INFO);
    }
    module_invoke_all('smartdocs_model_security_scheme_postsave', $scheme, $model, $revision, $is_update);
    drupal_set_message(t("The security scheme has been saved"), "status");
  }
  catch (Exception $e) {
    drupal_set_message(t('There was an error saving the security scheme.'));
  }
  $form_state['redirect'] = 'admin/content/smartdocs/' . $model->getUuid() . '/revisions/' . $revision->getUuid() . '/security';
}

/**
 * Page callback for the security scheme list page.
 *
 * @param array $model
 *   The model to list the security schemes for.
 * @param array $revision
 *   The revision of the model to list the security scheme for.
 *
 * @return string
 *   Themed table of security schemes for the given model and revision.
 */
function smartdocs_security_scheme_list(array $model, array $revision) {

  $rev = $revision['revisionNumber'];
  drupal_set_title(t('Security Settings for API Revision #@rev', array('@rev' => $rev)));

  $breadcrumb = array();
  $breadcrumb[] = l(t('Home'), '<front>');
  $breadcrumb[] = l(t('Content'), 'admin/content');
  $breadcrumb[] = l(t('SmartDocs'), 'admin/content/smartdocs/models');
  $breadcrumb[] = l(t('!model', array('!model' => $model['displayName'])), 'admin/content/smartdocs/models/' . $model['id']);
  $breadcrumb[] = l(t('Revisions'), 'admin/content/smartdocs/models/' . $model['id'] . '/revisions');
  $breadcrumb[] = l(t('Revision #@rev', array('@rev' => $rev)), 'admin/content/smartdocs/models/' . $model['id'] . '/revisions/' . $rev);
  drupal_set_breadcrumb($breadcrumb);

  drupal_add_css(drupal_get_path('module', 'views') . '/css/views-admin.ctools.css');
  $output['add_scheme'] = array(
    '#type' => 'link',
    '#href' => 'admin/content/smartdocs/' . $model['id'] . '/revisions/' . $revision['id'] . '/security/add',
    '#title' => t('Add Security Scheme'),
    '#prefix' => '<div class="templateauth-help" style="margin-bottom: 1.5em">',
    '#suffix' => '</div>',
  );
  $settings_url = url('admin/content/smartdocs/models/' . $model['id'] . '/settings', array('fragment' => 'template_auth_settings'));
  $output['template_auth_link'] = array(
    '#markup' => t('To manage the template auth schemes, go to the <a href="@url">template auth settings</a>.', array('@url' => $settings_url)),
    '#prefix' => '<div class="templateauth-help">',
    '#suffix' => '</div>',
  );

  $security = new Security(devconnect_default_org_config(), $model['id'], $revision['id']);
  $start = microtime(TRUE);
  $schemes = $security->loadAllSchemes();
  if (variable_get('smartdocs_log_transaction_time', 0)) {
    $args = array(
      '!model' => $model['id'],
      '!revision' => $revision['revisionNumber'],
      '!time' => number_format(microtime(TRUE) - $start, 4),
    );
    watchdog('smartdocs', 'Loaded all security schemes for revision !revision of model !model in !time seconds', $args, WATCHDOG_INFO);
  }
  $rows = array();
  $header = array(
    t('Name'),
    t('Type'),
    t('Operations'),
  );
  /** @var SecurityScheme $scheme */
  foreach ($schemes as $scheme) {
    $operations = array(
      'edit' => 'Edit',
      'delete' => 'Delete',
    );
    $ops = array();
    foreach ($operations as $op => $disp) {
      $ops['list-' . $op] = array(
        'title' => $disp,
        'href' => 'admin/content/smartdocs/' . $model['id'] . '/revisions/' . $revision['id'] . '/security/' . $scheme->getName() . '/' . $op,
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
 * Form API callback: Confirmation form for deleting a security scheme.
 *
 * @param array $form
 *   The form array.
 * @param array $form_state
 *   The form state array.
 * @param array $model
 *   The model for which the scheme is being deleted.
 * @param array $revision
 *   The revision of the model for which the scheme is being deleted.
 * @param string $scheme_name
 *   The name of the scheme to be deleted.
 *
 * @return array
 *   The confirmation form array.
 */
function smartdocs_model_security_delete(array $form, array &$form_state, array $model, array $revision, $scheme_name) {

  $form['model_uuid'] = array(
    '#type' => 'value',
    '#value' => $model['id'],
  );
  $form['revision_uuid'] = array(
    '#type' => 'value',
    '#value' => $revision['id'],
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
  $path = 'admin/content/smartdocs/' . $model['id'] . '/revisions/' . $revision['id'] . '/security';
  $description = t('You will be deleting this security scheme permanently and this action cannot be undone.');

  return confirm_form($form, t('Are you sure that you want to delete the security scheme %scheme_name?', array('%scheme_name' => $scheme_name)), $path, $description, t('Delete'));
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
  $log_transaction_time = variable_get('smartdocs_log_transaction_time', 0);

  $model_id = $form_state['values']['model_uuid'];
  $revision_id = $form_state['values']['revision_uuid'];
  $scheme_name = $form_state['values']['scheme_name'];
  $delete_templateauth = $form_state['values']['delete_templateauth'];

  $security = new Security(devconnect_default_org_config(), $model_id, $revision_id);
  module_invoke_all('smartdocs_security_predelete', $security, $model_id, $revision_id);
  try {
    $start = microtime(TRUE);
    $security->delete($scheme_name);
    if ($log_transaction_time) {
      $args = array(
        '!model' => $model_id,
        '!revision' => $revision_id,
        '!scheme' => $scheme_name,
        '!time' => number_format(microtime(TRUE) - $start, 4),
      );
      watchdog('smartdocs', 'Deleted security scheme !scheme from revision !revision of model !model in !time seconds', $args, WATCHDOG_INFO);
    }
    module_invoke_all('smartdocs_security_postdelete', $security, $model_id, $revision_id);
    if ($delete_templateauth) {
      $templateauth = new TemplateAuth(devconnect_default_org_config(), $model_id);
      module_invoke_all('smartdocs_template_auth_scheme_predelete', $scheme_name, $model_id);
      $start = microtime(TRUE);
      $templateauth->delete($scheme_name);
      if ($log_transaction_time) {
        $args['!time'] = number_format(microtime(TRUE) - $start, 4);
        unset($args['!revision']);
        watchdog('smartdocs', 'Deleted templateauth !scheme from model !model in !time seconds', $args, WATCHDOG_INFO);
      }
      module_invoke_all('smartdocs_template_auth_scheme_postdelete', $scheme_name, $model_id);
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
 * @param array $model
 *   Model to which the revision being edited belongs.
 * @param array $revision
 *   The revision being edited.
 */
function smartdocs_edit_revision(array $form, array $form_state, array $model, array $revision) {
  drupal_set_title(t('Edit API Revision #@rev of model ‘!model’', array(
    '@rev' => $revision['revision'],
    '!model' => $model['displayName'],
  )));
  $breadcrumb = array();
  $breadcrumb[] = l(t('Home'), '<front>');
  $breadcrumb[] = l(t('Content'), 'admin/content');
  $breadcrumb[] = l(t('SmartDocs'), 'admin/content/smartdocs/models');
  $breadcrumb[] = l(t('!model', array('!model' => $model['displayName'])), 'admin/content/smartdocs/models/' . $model['id']);
  $breadcrumb[] = l(t('Revisions'), 'admin/content/smartdocs/models/' . $model['id'] . '/revisions');
  $breadcrumb[] = l(t('Revision #@rev', array('@rev' => $revision['revisionNumber'])), 'admin/content/smartdocs/models/' . $model['id'] . '/revisions/' . $revision['revisionNumber']);
  drupal_set_breadcrumb($breadcrumb);

  $form['model'] = array(
    '#type' => 'value',
    '#value' => $model,
  );
  $form['revision'] = array(
    '#type' => 'value',
    '#value' => $revision,
  );
  $form['display_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Display Name'),
    '#size' => 60,
    '#default_value' => $revision['displayName'],
    '#maxlength' => 128,
  );
  $form['version'] = array(
    '#type' => 'textfield',
    '#title' => t('Version ID'),
    '#size' => 60,
    '#default_value' => $revision['releaseVersion'],
    '#maxlength' => 128,
  );
  $form['description'] = array(
    '#type' => 'textarea',
    '#title' => t('Description'),
    '#default_value' => $revision['description'],
    '#rows' => 5,
  );
  $form['base_url'] = array(
    '#type' => 'textfield',
    '#title' => t('Base URL'),
    '#default_value' => $revision['baseUrl'],
    '#size' => 60,
    '#maxlength' => 128,
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
  module_invoke_all('smartdocs_revision_presave', $revision, $model);

  try {
    $start = microtime(TRUE);
    $revision->save(TRUE);
    if (variable_get('smartdocs_log_transaction_time', 0)) {
      $args = array(
        '!model' => $model->getUuid(),
        '!revision' => $revision->getRevisionNumber(),
        '!time' => number_format(microtime(TRUE) - $start, 4),
      );
      watchdog('smartdocs', 'Saved revision !revision of model !model in !time seconds', $args, WATCHDOG_INFO);
    }
    module_invoke_all('smartdocs_revision_postsave', $revision, $model);
    module_invoke_all('smartdocs_model_update', $model->getUuid());

    $rev = $revision->getRevisionNumber();
    drupal_set_message(t('Revision #@rev has been updated.', array('@rev' => $rev)));
    $form_state['redirect'] = 'admin/content/smartdocs/models/' . $model->getUuid() . '/revisions/' . $rev;
  }
  catch (Exception $e) {
    drupal_set_message(t('Revision #@rev failed to update.', array('@rev' => $revision->getRevisionNumber())), 'error');
  }
}

/**
 * Form constructor to dismiss warning that appears on status report.
 *
 * @return array
 *   The newly-created form.
 */
function smartdocs_dismiss_custom_template_warning() {
  $form = array();
  $ques = t('Are you sure you want to dismiss the custom template update warning?');
  $desc = t('This implies that you have reviewed the default template at %path and made sure your SmartDocs custom method template is up-to-date.', array('%path' => base_path() . drupal_get_path('module', 'smartdocs') . '/templates/smartdocs.hbr'));
  return confirm_form($form, $ques, 'admin/config/smartdocs', $desc, t('Dismiss warning'), t('Cancel'));
}

/**
 * Submit handler to dismiss warning that appears on status report.
 *
 * @param array $form
 *   The form being submitted.
 * @param array $form_state
 *   State of the form being submitted.
 */
function smartdocs_dismiss_custom_template_warning_submit($form, &$form_state) {
  variable_del('smartdocs_custom_template_warning');
  $form_state['redirect'] = 'admin/config/smartdocs';
}
