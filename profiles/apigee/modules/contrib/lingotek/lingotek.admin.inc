<?php

/**
 * @file
 * Administrative Settings for the module.
 */
include_once('lingotek.session.inc');

/**
 * Form constructor for the administration form.
 *
 * @return array
 *   A FAPI form array.
 */
function lingotek_admin_account_status_form($form, &$form_state, $show_fieldset = FALSE) {

  lingotek_is_module_setup();
  $account = LingotekAccount::instance();

  $api = LingotekApi::instance();
  $site = variable_get('site_name', 'Drupal Site');
  $is_enterprise = $account->isEnterprise();

  // Account Status

  $account_status = $account->getStatusText();
  //$account_status .= '<br>Enterprise: ' . ($is_enterprise ? 'Yes' : 'No');

  $form['status'] = array(
    '#type' => $show_fieldset ? 'fieldset' : 'item',
    '#title' => t('Account'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#group' => 'administrative_settings',
    'actions' => array(
      '#type' => 'actions',
      'submit' => array(
        '#type' => 'submit',
        '#value' => t('Update account status')
      )
    ),
    '#submit' => array('lingotek_admin_account_status_form_submit')
  );

  $form['status']['account_summary'] = array(
    '#type' => 'hidden',
    '#value' => $account_status,
    '#attributes' => array('id' => array('account_summary')),
  );

  // Account Information

  $activation_details = variable_get('lingotek_activation_first_name', '') . ' ' . variable_get('lingotek_activation_last_name', '');
  $activation_email = variable_get('lingotek_activation_email', '');
  $activation_details .= strlen($activation_email) ? ' (' . $activation_email . ')' : '';
  $activation_details = strlen(trim($activation_details)) ? $activation_details : 'NA';

  $form['status'][] = array(
    '#type' => 'item',
    '#markup' => theme('table', array('header' => array(), 'rows' => array(
        array(t('Status:'), $account_status),
        //,array('Plan:', $account->getPlanTypeText()),
        array(t('Enterprise:'), ($is_enterprise ? '<span style="color: green;">' . t('Yes') . '</span>' : '<span>' . t('No') . '</span>')),
        array(t('Activation Name:'), $activation_details),
        array(t('Community Identifier:'), variable_get('lingotek_community_identifier', '')),
        array(t('OAuth Key:'), variable_get('lingotek_oauth_consumer_id', '')),
        array(t('OAuth Secret:'), variable_get('lingotek_oauth_consumer_secret', '')),
        array(t('Workflow ID:'), variable_get('lingotek_workflow', '')),
        array(t('Integration Method ID:'), variable_get('lingotek_integration_method', '')),
        array(t('External ID:'), variable_get('lingotek_login_id', '')),
        array(t('Project ID:'), variable_get('lingotek_project', '')),
        array(t('Vault ID:'), variable_get('lingotek_vault', '')),
        array(t('Notify URL:'), variable_get('lingotek_notify_url', '')),
  )))
  );

  return $form;
}

/**
 * Account Status Submit Handler
 * @param type $form
 * @param type $form_state
 */
function lingotek_admin_account_status_form_submit($form, &$form_state) {
  $account = LingotekAccount::instance();
  $result = $account->getAccountStatus(); // Returns false on fail.

  if ($result === FALSE) { // There was a problem retrieving the account status.
    drupal_set_message(t('There was an error retrieving your account status.  Please try again later.'), 'error');
  }
  else { // We got a valid account status.  Send the user back to the Lingotek settings page, via the second cache clearer.
    drupal_set_message(t('Your account status has been updated.'));
    menu_rebuild();
  }
}

/**
 * Content translation form
 */
function lingotek_admin_node_translation_settings_form($form, &$form_state, $show_fieldset = FALSE) {

  //$setup_complete = variable_get('lingotek_setup_complete', 0);
  $setup_complete = (!lingotek_is_config_missing());
  $raw_types = node_type_get_types();
  $types = array();
  $translate = variable_get('lingotek_translate_fields', array());

  // What types of fields DO we translate?
  $included_fields = array('text', 'text_long', 'text_textfield', 'text_textarea', 'text_textarea_with_summary');
  if (module_exists('link')) {
    $included_fields[] = 'link_field';
  }
  
  $form['node_translation'] = array(
    '#type' => $show_fieldset ? 'fieldset' : 'item',
    '#title' => ($setup_complete ? t('Translate Content') : t('Which content types do you want translated?')),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#group' => 'administrative_settings',
    'actions' => array(
      '#type' => 'actions',
      'submit' => array(
        '#type' => 'submit',
        '#value' => ($setup_complete ? t('Save') : t('Next')),
      )
    ),
    '#submit' => array('lingotek_admin_node_translation_settings_form_submit'),
  );
  
  if ($setup_complete) {
    $form['node_translation'][] = array(
      '#type' => 'item',
      '#description' => t('Choose the content types to be translated:'),
    );
  }

  $rows = array();

  foreach ($raw_types as $value) {
    $fr = array();
    foreach (field_info_instances('node', $value->type) as $field) {
      $field_label = $field['label'];
      $field_machine_name = $field['field_name'];
      $field_type = $field['widget']['type'];
      if (array_search($field_type, $included_fields)) {
        $fr[$field_machine_name] = array(
          '#type' => 'checkbox',
          '#title' => $field_label,
          '#attributes' => array(
            'id' => array('edit-form-item-' . $value->type . '-seperator-' . $field_machine_name),
            'name' => $value->type . '_SEPERATOR_' . $field_machine_name,
            'class' => array('field'),
          ),
        );
        
        $is_enabled = !empty($translate[$value->type]) && array_search($field_machine_name, $translate[$value->type]) !== FALSE;
        if(!$setup_complete || $is_enabled) {
          $fr[$field_machine_name]['#attributes']['checked'] = 'checked';
        }
      }
    }
    if(!isset($fr['title_field'])) {
        $fr['title_field'] = array(
          '#type' => 'checkbox',
          '#title' => 'Title (Note: field will be created.)',
          '#attributes' => array(
            'id' => array('edit-form-item-' . $value->type . '-seperator-' . $field_machine_name),
            'name' => 'title_swap_' . $value->type,
            'class' => array('field'),
          ),
        );
        if(!$setup_complete) {
          $fr['title_field']['#attributes']['checked'] = 'checked';
        }
    }
    $rows[$value->type] = array(
      array('data' => $value->name, 'width' => '20%'),
      array('data' => drupal_render($fr), 'width' => '80%'),
    );
  }
  
  $form['node_translation']['types'] = array(
    '#type' => 'tableselect',
    '#header' => array('Content Type', 'Fields'),
    '#options' => $rows,
    '#attributes' => array('class' => array('lingotek-content-settings-table')),
  );
  
  foreach (array_keys($translate) as $content_type) {
    $form['node_translation']['types']['#default_value'][$content_type] = 1;
  }

  if(!$setup_complete) {
    $defaults = array();
    foreach ($raw_types as $content_type) {
      $defualts[$content_type->type] = 1;
    }
    $form['node_translation']['types']['#default_value'] = $defualts;
  }
  
  return $form;
}

/**
 * Node Translation Settings - Form Submit
 */
function lingotek_admin_node_translation_settings_form_submit($form, &$form_state) {
  $translate = array();
  $operations = array();
  foreach ($form_state['input'] as $key => $value) {
    
    // Look for Selected Content Types and Fields.
    if (FALSE !== strpos($key, '_SEPERATOR_')) {
      // And only if set to translate
      if ($value != 0) {
        $parts = explode('_SEPERATOR_', $key);
        $content_type = $parts[0];
        $content_field = $parts[1];
        
        //check to make sure that the content type is enabled
        if(isset($form_state['values']['types'][$content_type])) {
          $translate[$content_type][] = $content_field;

          // Set this content type to be Lingotek translated.
          variable_set('language_content_type_' . $content_type, LINGOTEK_ENABLED);
          // Set this field to 'translatable'.
          // Update the field via the Field API (Instead of the direct db_update)
          $field = field_info_field($content_field);
          $field['translatable'] = 1;
          $field['lingotek_translatable'] = 1;
          field_update_field($field);
        }
      }
    } // END:  Selected Content Types and Fields
    // Look for any nodes we need to do the Title swap for.
    if (FALSE !== strpos($key, 'title_swap_')) {
      // And only if set to swap
      if ($value != 0) {
        $content_type = substr($key, strlen('title_swap_'));
        
        //check to make sure that the content type is enabled
        if(isset($form_state['values']['types'][$content_type])) {
          // Do the actual title replacement
          $entity_type = 'node';
          $bundle = $content_type;
          $legacy_field = 'title';

          // Use the Title module to migrate the content.
          if (title_field_replacement_toggle($entity_type, $bundle, $legacy_field)) {
            //title_field_replacement_batch_set($title_entity, $title_bundle, $title_field);
            $operations[] = array('title_field_replacement_batch', array($entity_type, $bundle, $legacy_field));
            $translate[$content_type][] = 'title_field';
            $field = field_info_field('title_field');
            $field['translatable'] = 1;
            $field['lingotek_translatable'] = 1; // Ensure the title field is flagged for Lingotek translation.
            $operations[] = array('field_update_field', array($field)); //field_update_field($field);
          }
        }
      }
    }
  }
  $_SESSION['lingotek_setup_path'][] = 'admin/config/lingotek/node-translation-settings';
  variable_set('lingotek_translate_fields', $translate);
  drupal_set_message(t('Your content types have been updated.'));

  if (count($operations)) {
    $batch = array(
      'title' => t('Preparing content for translation'),
      'operations' => $operations
    );

    batch_set($batch);

    if (is_null(variable_get('lingotek_translate_comments', NULL)) || is_null(variable_get('lingotek_translate_config', NULL))) {
      $redirect = 'admin/config/lingotek/comment-translation-settings';
    }
    else {
      $redirect = LINGOTEK_MENU_LANG_BASE_URL . '/settings';
    }
    batch_process($redirect);
  }
}

/**
 * Additional translation form
 */
function lingotek_admin_comment_translation_settings_form($form, &$form_state, $show_fieldset = FALSE) {
  global $base_url;
  $api = LingotekApi::instance();
  //$setup_complete = variable_get('lingotek_setup_complete', 0);
  $setup_complete = (!lingotek_is_config_missing());
  $account = LingotekAccount::instance();
  $is_enterprise = $account->isEnterprise();
  /*
   * Comment translation
   */
  $form['comment_translation'] = array(
    '#type' => $show_fieldset ? 'fieldset' : 'item',
    '#title' => ($setup_complete ? t('Translate Comments') : t('Which comment items do you want translated?')),
    //'#description' => t('Enable/disable and set defaults for comment translation.'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#group' => 'administrative_settings',
    'actions' => array(
      '#type' => 'actions',
      'submit' => array(
        '#type' => 'submit',
        '#value' => ($setup_complete ? t('Save') : t('Next')),
      )
    ),
    '#submit' => array('lingotek_admin_additional_translation_settings_form_submit'),
  );

  $form['comment_translation'][] = array(
    '#type' => 'item',
    '#description' => t('Automatically translate comments on the selected content types.'),
  );

  $type_options = array();
  $type_defaults = array();
  foreach (node_type_get_types() as $type => $type_data) {
    $type_options[$type] = $type_data->name;
    $type_defaults[$type] = $type;
  }
  $form['comment_translation']['lingotek_translate_comments_options']['lingotek_translate_comments_node_types'] = array(
    '#type' => 'checkboxes',
    '#options' => $type_options,
    '#multiple' => TRUE,
    '#default_value' => variable_get('lingotek_translate_comments_node_types', $setup_complete ? array() : $type_defaults),
  );

  if ($is_enterprise) {
    $form['comment_translation']['lingotek_translate_comments_options']['lingotek_translate_comments_workflow_id'] = array(
      '#type' => 'select',
      '#title' => t('Workflow'),
      '#description' => t('This workflow will be used to translate comments.
        Since there is no Drupal UI for managing workflow phases for comments, it is 
        recommended that you choose a Workflow that only consists of a single machine translation Phase.'),
      '#options' => $api->listWorkflows(),
      '#default_value' => variable_get('lingotek_translate_comments_workflow_id', ''),
    );
  }

  return $form;
}

function lingotek_admin_comment_translation_settings_form_submit($form, &$form_state) {
  $form_state['values']['lingotek_translate_comments'] = FALSE;

  foreach ($form_state['values']['lingotek_translate_comments_node_types'] as $val) {
    if (!empty($val))
      $form_state['values']['lingotek_translate_comments'] = TRUE;
  }

  system_settings_form_submit($form, $form_state);

  //This code configures the content entity and it's fields so that they can be translated.
  if ($form_state['values']['lingotek_translate_comments']) {
    foreach ($form_state['values']['lingotek_translate_comments_node_types'] as $content_type => $value) {
      if (!title_field_replacement_enabled('comment', 'comment_node_' . $content_type, 'subject')) {
        title_field_replacement_toggle('comment', 'comment_node_' . $content_type, 'subject');
        title_field_replacement_batch_set('comment', 'comment_node_' . $content_type, 'subject');
      }
    }

    $field = field_info_field('subject_field');
    $field['lingotek_translatable'] = TRUE;
    $field['translatable'] = TRUE;
    field_update_field($field);

    $field = field_info_field('comment_body');
    $field['lingotek_translatable'] = TRUE;
    $field['translatable'] = TRUE;
    field_update_field($field);

    // This is needed for versions of Drupal core 7.10 and lower. See http://drupal.org/node/1380660 for details.
    drupal_static_reset('field_available_languages');
  }

  // Enabling/disabling Lingotek comment translation will have an effect on the comment entity.
  entity_info_cache_clear();
  entity_get_info('comment');
}

/**
 * Additional translation form
 */
function lingotek_admin_additional_translation_settings_form($form, &$form_state, $show_fieldset = FALSE) {
  global $base_url;
  $api = LingotekApi::instance();
  //$setup_complete = variable_get('lingotek_setup_complete', 0);
  $setup_complete = (!lingotek_is_config_missing());
  $account = LingotekAccount::instance();
  $is_enterprise = $account->isEnterprise();

  // Configuration translation (ie. taxonomies, menus, etc.)
  $form['additional_translation'] = array(
    '#type' => $show_fieldset ? 'fieldset' : 'item',
    '#title' => ($setup_complete ? t('Configuration Translation') : t('Which additional items do you want translated?')),
    //'#description' => t('Enable/disable and set defaults for comment translation.'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#group' => 'administrative_settings',
    'actions' => array(
      '#type' => 'actions',
      'submit' => array(
        '#type' => 'submit',
        '#value' => t('Save')
      )
    ),
    '#validate' => array('lingotek_admin_additional_translation_settings_form_validate'),
    '#submit' => array('lingotek_admin_additional_translation_settings_form_submit'),
  );

  $form['additional_translation']['lingotek_translate_config_options']['lingotek_translate_config_builtins'] = array(
    '#type' => 'checkbox',
    '#title' => t('Built-in Interface'),
    '#description' => t('If configuration translation is enabled, all built-in strings will be automatically translated.'),
    '#default_value' => variable_get('lingotek_translate_config_builtins', 1),
  );
  $form['additional_translation']['lingotek_translate_config_options']['lingotek_translate_config_blocks'] = array(
    '#type' => 'checkbox',
    '#title' => t('Blocks'),
    '#description' => t('When enabled, all blocks will be updated automatically to be translatable in the Languages settings.'),
    '#default_value' => variable_get('lingotek_translate_config_blocks', 1),
  );
  $form['additional_translation']['lingotek_translate_config_options']['lingotek_translate_config_taxonomies'] = array(
    '#type' => 'checkbox',
    '#title' => t('Taxonomies'),
    '#description' => t('When enabled, all taxonomies will be updated automatically to use translation mode \'Localize\' in the Multilingual Options.'),
    '#default_value' => variable_get('lingotek_translate_config_taxonomies', 1),
  );
  $form['additional_translation']['lingotek_translate_config_options']['lingotek_translate_config_menus'] = array(
    '#type' => 'checkbox',
    '#title' => t('Menus'),
    '#description' => t('When enabled, all menus will be updated to use \'Translate and Localize\' in the Multilingual Options.'),
    '#default_value' => variable_get('lingotek_translate_config_menus', 1),
  );
  $form['additional_translation']['lingotek_translate_config_options']['lingotek_translate_config_views'] = array(
    '#type' => 'checkbox',
    '#title' => t('Views'),
    '#description' => t('When enabled, all configured views will be searched for translatable strings.  Note this will not generally include translation of all results generated by each view.'),
    '#default_value' => variable_get('lingotek_translate_config_views', 1),
  );
  if ($is_enterprise) {
    $form['additional_translation']['lingotek_translate_config_options']['lingotek_translate_config_workflow_id'] = array(
      '#type' => 'select',
      '#title' => t('Workflow'),
      '#description' => t('This workflow will be used for handling these additional translation items.'),
      '#options' => $api->listWorkflows(),
      '#default_value' => variable_get('lingotek_translate_config_workflow_id', ''),
    );
  }
  
  if($setup_complete) {
    $form['additional_translation']['lingotek_translate_config_options']['view_status'] = array(
      '#type' => 'item',
      '#description' => t('You can view the progress of the configuration translations on the ' . l('Translate Interface', 'admin/config/regional/translate') . ' page.'),
    );
  }
  return $form;
}

function lingotek_verify_modules_enabled($module_list) {
  $missing_modules = FALSE;
  foreach ($module_list as $mod) {
    if (!(module_exists($mod))) {
      form_set_error(t('missing:@mod', array('@mod' => $mod)), t('Module \'@mod\' must first be enabled.', array('@mod' => $mod)));

      $missing_modules = TRUE;
    }
  }
  if ($missing_modules) {
    return FALSE;
  }
  return TRUE;
}

function lingotek_admin_additional_translation_settings_form_validate($form, &$form_state) {
  // check for required modules if config translation is enabled
  $conf_opt_part = $form['additional_translation']['lingotek_translate_config_options'];

  if ($conf_opt_part['lingotek_translate_config_blocks'] && $conf_opt_part['lingotek_translate_config_blocks']['#value'] != 0) {
    lingotek_verify_modules_enabled(array('i18n_block'));
  }
  if ($conf_opt_part['lingotek_translate_config_taxonomies'] && $conf_opt_part['lingotek_translate_config_taxonomies']['#value'] != 0) {
    lingotek_verify_modules_enabled(array('i18n_taxonomy'));
  }
  if ($conf_opt_part['lingotek_translate_config_menus'] && $conf_opt_part['lingotek_translate_config_menus']['#value'] != 0) {
    lingotek_verify_modules_enabled(array('i18n_menu'));
  }
  if ($conf_opt_part['lingotek_translate_config_views'] && $conf_opt_part['lingotek_translate_config_views']['#value'] != 0) {
    lingotek_verify_modules_enabled(array('i18nviews'));
  }
}

function lingotek_admin_additional_translation_settings_form_submit($form, &$form_state) {
  system_settings_form_submit($form, $form_state);

  //if any configuration options are set then config translation is enabled
  if (variable_get('lingotek_translate_config', 0) || variable_get('lingotek_translate_config_taxonomies', 0) ||
      variable_get('lingotek_translate_config_menus', 0) || variable_get('lingotek_translate_config_views', 0) ||
      variable_get('lingotek_translate_config_builtins', 0)) {
    variable_set('lingotek_translate_config', TRUE);
  }

  // prepare for any configuration translation, if applicable
  if (variable_get('lingotek_translate_config_blocks', 0)) {
    lingotek_admin_prepare_blocks();
  }
  if (variable_get('lingotek_translate_config_taxonomies', 0)) {
    lingotek_admin_prepare_taxonomies();
  }
  if (variable_get('lingotek_translate_config_menus', 0)) {
    lingotek_admin_prepare_menus();
  }
  if (variable_get('lingotek_translate_config_views', 0)) {
    lingotek_admin_prepare_views();
  }
}

function lingotek_admin_prepare_blocks() {
  // update all blocks to be translatable in the Languages settings
  db_update('block')
      ->fields(array('i18n_mode' => LINGOTEK_I18N_ENABLED_VALUE))
      ->execute();
  return TRUE;
}

function lingotek_admin_prepare_taxonomies() {
  // update all taxonomies to be updated to use translation mode 'Localize'
  db_update('taxonomy_vocabulary')
      ->fields(array('i18n_mode' => LINGOTEK_TAXONOMY_LOCALIZE_VALUE))
      ->execute();
  return TRUE;
}

function lingotek_admin_prepare_menus() {
  // update all menus to use 'Translate and Localize' in the Multilingual Options
  db_update('menu_custom')
      ->fields(array('i18n_mode' => LINGOTEK_MENU_LOCALIZE_TRANSLATE_VALUE))
      ->execute();
  return TRUE;
}

function lingotek_admin_prepare_views() {
  // WTD: 
  return TRUE;
}

/**
 * Advanced Parsing - XML Configuration
 */
function lingotek_admin_advanced_parsing_form($form, &$form_state, $show_fieldset = FALSE) {
  /*
   * Advanced XML Configuration
   */
  $form['advanced-parsing'] = array(
    '#type' => $show_fieldset ? 'fieldset' : 'item',
    '#title' => t('Advanced Content Parsing'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#group' => 'administrative_settings',
    'actions' => array(
      '#type' => 'actions',
      'submit' => array(
        '#type' => 'submit',
        '#value' => t('Save')
      )
    ),
    '#submit' => array('lingotek_admin_advanced_parsing_form_submit')
  );

  $form['advanced-parsing'][] = array(
    '#type' => 'item',
    '#description' => t('Settings to support advanced parsing of translatable content.'),
  );

  $form['advanced-parsing']['lingotek_advanced_xml_config1'] = array(
    '#type' => 'textarea',
    '#title' => t('Configuration Settings (Primary)'),
    '#description' => t('Paste in the contents of an advanced configuration file (.fprm). This will be used as the primary set of advanced settings when sending content to Lingotek.'),
    '#default_value' => variable_get('lingotek_advanced_xml_config1'),
  );

  $form['advanced-parsing']['lingotek_advanced_xml_config2'] = array(
    '#type' => 'textarea',
    '#title' => t('Configuration Settings (Secondary)'),
    '#description' => t('Paste in the contents of an advanced configuration file (.fprm). This will be used as the secondary set of advanced settings when sending content to Lingotek.'),
    '#default_value' => variable_get('lingotek_advanced_xml_config2'),
  );

  if (!variable_get('lingotek_advanced_parsing', FALSE)) {
    $form['advanced-parsing']['lingotek_advanced_parsing'] = array(
      '#type' => 'checkbox',
      '#title' => t('Upgrade to advanced content parsing.'),
      '#description' => t('This site is currently using Simple content parsing.
        Check this box to upgrade your site to use advanced content parsing for existing and future content. <strong>Warning:</strong> This will update all current Lingotek-associated content on the site, possibly modifying the state of in-progress translations.'),
    );

    $form['advanced-parsing']['#submit'][] = 'lingotek_handle_advanced_xml_upgrade';
  }
  return $form;
}

function lingotek_admin_advanced_parsing_form_submit($form, &$form_state) {
  system_settings_form_submit($form, $form_state);
}

/**
 * Lingotek prefs Form
 */
function lingotek_admin_prefs_form($form, &$form_state, $show_fieldset = FALSE) {
  /*
   * Configuration
   */
  $form['prefs'] = array(
    '#type' => $show_fieldset ? 'fieldset' : 'item',
    '#title' => t('Preferences'),
    //'#description' => t('Module preferences.'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#group' => 'administrative_settings',
    'actions' => array(
      '#type' => 'actions',
      'submit' => array(
        '#type' => 'submit',
        '#value' => t('Save')
      )
    ),
    '#submit' => array('lingotek_admin_prefs_form_submit')
  );

  $form['prefs']['lingotek_hide_tlmi'] = array(
    '#type' => 'checkbox',
    '#title' => t('Hide top-level menu item'),
    '#description' => t('When hidden, the module can still be accessed under <i>Configuration &gt; Regional and language</i>'),
    '#default_value' => variable_get('lingotek_hide_tlmi', 0),
  );

  $current_theme = variable_get('theme_default', 'none'); //global $theme_key;

  $query = db_select('block', 'b');
  $query->fields('b');
  $query->condition('module', 'locale');
  $query->condition('delta', 'language');
  $query->condition('theme', $current_theme);
  $result = $query->execute();

  $block = $result->fetchAssoc();
  $block_enabled = $block['status'];
  $block_regions = system_region_list($current_theme, REGIONS_VISIBLE);

  $ls_chkbox_enabled = array(
    '#type' => 'checkbox',
    '#title' => t('Enable the default language switcher'),
    '#default_value' => $block_enabled,
  );

  $default_region = "sidebar_first";
  $default_region_value = array_key_exists($block['region'], $block_regions) ? $block['region'] : (array_key_exists($default_region, $block_regions) ? $default_region : NULL);

  $ls_select_region = array(
    '#type' => 'select',
    '#title' => t('The region where the switcher will be displayed.'),
    //'#description' => 'When enabled, the region to be displayed in.',
    '#options' => $block_regions,
    '#default_value' => $default_region_value,
    '#states' => array(
      'invisible' => array(
        ':input[name="enabled"]' => array('checked' => FALSE),
      ),
    ),
  );

  $fkey = 'prefs'; // form key
//  $form[$fkey] = array(
//    '#type' => $show_fieldset ? 'fieldset' : 'item',
//    '#title' => t('Default Language Switcher'),
//    '#collapsible' => TRUE,
//    '#collapsed' => TRUE,
//    '#group' => 'administrative_settings',
//    'actions' => array(
//      '#type' => 'actions',
//      'submit' => array(
//        '#type' => 'submit',
//        '#value' => t('Save configuration')
//      ),
//    /* 'note' => array(
//      '#markup' => t('Note: The default language switcher block is only shown if at least two languages are enabled and language negotiation is set to <i>URL</i> or <i>Session</i>.')
//      ) */
//    )
//  );
//  $form[$fkey][] = array(
//    '#type' => 'item',
//    '#description' => t('Would you like to enable the default language switcher?')
//  );
  $form[$fkey]['theme'] = array(
    '#type' => 'hidden',
    '#value' => $current_theme
  );

  $form[$fkey]['enabled'] = $ls_chkbox_enabled;
  $form[$fkey]['region'] = $ls_select_region;

  $lang_negotiation_info = locale_language_negotiation_info();
  $general_detection_config_url = substr($lang_negotiation_info['locale-url']['config'], 0, strrpos($lang_negotiation_info['locale-url']['config'], "/"));

  $form[$fkey][] = array(
    '#type' => 'item',
    '#description' => t('Note: The default language switcher block is only shown if at least two languages are enabled and language negotiation is set to <i>URL</i> or <i>Session</i>.'
        . '<br>Go to ' . l(t('Language detection and selection'), $general_detection_config_url) . ' to change this.'),
    '#states' => array(
      'invisible' => array(
        ':input[name="enabled"]' => array('checked' => FALSE),
      ),
    ),
  );
  /* $language_negotiation = variable_get('language_negotiation_language', array());
    $url_detection_enabled = array_key_exists('locale-url', $language_negotiation);
    $ls_chkbox_detection = array(
    '#type' => 'checkbox',
    '#title' => t('URL detection Enabled'),
    '#default_value' => $url_detection_enabled,
    '#disabled' => TRUE
    );
    $form[$form_key]['url_detection'] = $ls_chkbox_detection;

    $form[$form_key][] = array(
    '#type' => 'markup',
    '#markup' => json_encode($block),
    '#attributes' => array('class' => 'description')
    ); */

  return $form;
}

function lingotek_admin_prefs_form_submit($form, &$form_state) {
  lingotek_admin_language_switcher_form_submit($form, $form_state);
  system_settings_form_submit($form, $form_state);
  menu_rebuild();
}

/**
 * Lingotek Connection Settings Form
 */
function lingotek_admin_connection_form($form, &$form_state, $show_fieldset = FALSE) {

  $account = LingotekAccount::instance();

  $api = LingotekApi::instance();
  $force = isset($_GET['test_connection']) ? TRUE : FALSE;
  $connected = $api->testAuthentication($force);
  $is_enterprise = $account->isEnterprise();


  $connection_error = t('Connect this site to your Lingotek account by filling in the fields below. If you do not yet have a Lingotek account, you can <a href="@signup_url">sign up</a> to create an ID and collect OAuth credentials. If all fields are complete, there is a problem with one or more of the values.', array('@signup_url' => url(LINGOTEK_API_SERVER . '/lingopoint/portal/communitySignup.action')));
  if (!$connected) {
    drupal_set_message($connection_error, 'error');
  }
  else {
    // clear the prior error message
    $errors = drupal_get_messages('error');
    if (isset($errors['error'])) {
      foreach ($errors['error'] as $error) {
        if ($error != $connection_error) {
          drupal_set_message(check_plain($error), 'error');
        }
      }
    }
  }

  $edit_connection = isset($_GET['edit_connection']) || !$connected;

  $status_message = ($connected) ?
      '<span style="color: green;">' . t('Connected') . '</span>' :
      '<span style="color: red;">' . t('Not Connected') . '</span>';

  $form['connection']['connection_summary'] = array(
    '#type' => 'hidden',
    '#value' => $status_message,
    '#attributes' => array('id' => array('connection_summary')),
  );

  /* $form['connection'][] = array(
    '#type' => 'item',
    '#description' => filter_xss($connection_group_description),
    ); */


  if ($is_enterprise || $edit_connection) {
    $form['connection']['lingotek_login_id'] = array(
      '#type' => 'textfield',
      '#title' => t('Lingotek ID'),
      '#description' => t('Enter the Lingotek ID you use to access the Lingotek Dashboard and Workbench.'),
      '#default_value' => variable_get('lingotek_login_id', ''),
    );

    $form['connection']['lingotek_oauth_consumer_id'] = array(
      '#type' => 'textfield',
      '#title' => t('OAuth Key'),
      '#description' => t('The OAuth Key used to connect with the Lingotek server.'),
      '#default_value' => variable_get('lingotek_oauth_consumer_id', ''),
    );

    $form['connection']['lingotek_oauth_consumer_secret'] = array(
      '#type' => 'textfield',
      '#title' => t('OAuth Secret'),
      '#description' => t('The OAuth Secret used to connect with the Lingotek server.'),
      '#default_value' => variable_get('lingotek_oauth_consumer_secret', ''),
    );
    // Note: The Lingotek servers may easily be set in code or by setting a 'lingotek_use_stage_servers' drupal variable
    $form['connection']['lingotek_use_stage_servers'] = array(
      '#type' => 'select',
      '#title' => t('Environment'),
      '#options' => array(
        0 => t('Production'),
        1 => t('Staging'),
      ),
      '#description' => LINGOTEK_DEV ? t('Note: The above Environment setting above will be overriden by your local configuration (i.e., settings.php).') : '',
      '#default_value' => variable_get('lingotek_use_stage_servers', 0),
    );

    $form['connection']['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Save'),
    );
  }
  else {
    $form['connection'][] = array(
      '#type' => 'item',
      '#title' => t('Lingotek Servers'),
      '#markup' => theme('table', array('header' => array(), 'rows' => array(
          array('TMS:', LINGOTEK_API_SERVER),
          array('GMC:', LINGOTEK_GMC_SERVER),
          array('Billing:', LINGOTEK_BILLING_SERVER),
        //array('AP Key:', LINGOTEK_AP_OAUTH_KEY),
        //array('AP Secret:', LINGOTEK_AP_OAUTH_SECRET),
    )))
    );
  }

  //$form = system_settings_form($form);
  if (!($is_enterprise || $edit_connection)) {
    unset($form['connection']['actions']);
  }
  return $form;
}

function lingotek_admin_connection_form_submit($form, &$form_state) {
  system_settings_form_submit($form, $form_state);
}

/**
 * Content defaults Form
 */
function lingotek_admin_content_defaults_form($form, &$form_state, $show_fieldset = FALSE) {

  $account = LingotekAccount::instance();
  $api = LingotekApi::instance();
  $site = variable_get('site_name', 'Drupal Site');
  $is_enterprise = $account->isEnterprise();
  $workbench_moderation_enabled = FALSE;
  if (module_exists('workbench_moderation')) {
    $workbench_moderation_enabled = TRUE;
  }

  /*
   * Default Settings
   */
  $form['defaults'] = array(
    '#type' => $show_fieldset ? 'fieldset' : 'item',
    '#title' => t('Content Defaults'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#group' => 'administrative_settings',
    'actions' => array(
      '#type' => 'actions',
      'submit' => array(
        '#type' => 'submit',
        '#value' => t('Save')
      )
    ),
    '#submit' => array('lingotek_admin_content_defaults_form_submit')
  );

  $form['defaults'][] = array(
    '#type' => 'item',
    '#description' => t('Translation management defaults used when creating new nodes. At the node level, these settings can be adjusted.'),
  );

  // Upload
  $form['defaults']['lingotek_create_documents_by_default'] = array(
    '#type' => 'checkbox',
    '#title' => t('Upload Content Automatically'),
    '#default_value' => variable_get('lingotek_create_documents_by_default', 0),
    //'#disabled' => !$is_enterprise,
    '#description' => t('When enabled, your Drupal content (including saved edits) will automatically be uploaded to Lingotek for translation.<br/>When disabled, you are required to manually upload your content by clicking the "Upload" button on the Translations tab.'),
  );

  if ($workbench_moderation_enabled) {
    $form['defaults']['lingotek_create_documents_by_default_workbench_moderation'] = array(
      '#type' => 'select',
      '#title' => 'Workbench Moderation Uploads',
      '#options' => lingotek_get_workbench_moderation_states(),
      '#default_value' => variable_get('lingotek_create_documents_by_default_workbench_moderation', workbench_moderation_state_published()),
      '#states' => array(
        'invisible' => array(
          ':input[name="lingotek_create_documents_by_default"]' => array('checked' => FALSE),
        ),
      ),
      '#description' => 'The most recent revision will be automatically uploaded when changed to this state.',
    );
  }

  // Download  
  $form['defaults']['lingotek_sync'] = array(
    '#type' => 'checkbox',
    '#title' => t('Download Translations Automatically'),
    '#default_value' => (variable_get('lingotek_sync', 1) !== 0) ? 1 : 0,
    //'#disabled' => !$is_enterprise,
    '#description' => t('When enabled, completed translations will automatically be downloaded from Lingotek.<br/>When disabled, you are required to manually download translations by clicking the "Download" button on the Translations tab.'),
  );

  if ($workbench_moderation_enabled) {
    $transition_select = lingotek_workbench_moderation_get_mult_transitions();

    $form['defaults']['lingotek_sync_wb']['wb_options'] = array(
      '#type' => 'select',
      '#title' => t('Workbench Moderation Downloads'),
//      '#field_prefix' => t('After translations have downloaded'),
      '#field_suffix' => t('after translations have downloaded.'),
      '#options' => lingotek_get_workbench_moderation_options(),
      '#default_value' => variable_get('lingotek_sync_workbench_moderation', 'no_moderation'),
      '#description' => t("Transitions will not occur until <i>all</i> of a node's translations have downloaded."),
      '#states' => array(
        'invisible' => array(
          ':input[name="lingotek_sync"]' => array('checked' => FALSE),
        ),
      ),
    );

    if (!empty($transition_select)) {
      $form['defaults']['lingotek_sync_wb']['wb_select'] = array(
        '#type' => 'item',
        '#states' => array(
          'visible' => array(
            'select[name="wb_options"]' => array('value' => 'increment'),
            ':input[name="lingotek_sync"]' => array('checked' => TRUE),
          ),
        ),
      );
      $form['defaults']['lingotek_sync_wb']['wb_select']['helper'] = array(
        '#type' => 'item',
        '#description' => t('Your configuration of the Workbench Moderation module has multiple transition paths.<br>Choose the state that you would like the translations to transition to below.'),
      );
      $header = array('Current State', 'Transitions to');
      $form['defaults']['lingotek_sync_wb']['wb_select']['wb_table'] = array(
        '#type' => 'container',
        '#theme' => 'table',
        '#header' => $header,
        '#rows' => array(),
      );
      foreach ($transition_select as $from_state => $to_states) {
        $selected = variable_get('lingotek_sync_wb_select_' . $from_state); // get default
        $select_str = '<div class="form-item form-type-select form-item-lingotek-sync-wb-select-' . $from_state . '">'; // HTML for a Drupal form select element
        $select_str .= '<select id="lingotek_sync_wb_select_' . $from_state . '" name="lingotek_sync_wb_select_' . $from_state . '"';
        $select_str .= ' class="form-select">';
        foreach ($to_states as $state) {
          $select_str .= '<option ';
          $select_str .= 'value="' . $state . '"';
          if ($selected && $selected == $state) { // select default
            $select_str .= ' selected="selected"';
          }
          $select_str .= '>' . $state . '</option>';
        }
        // foreach option add to string
        $select_str .= '</select></div>';
        $form['defaults']['lingotek_sync_wb']['wb_select']['wb_table']['#rows'][] = array(
          array('data' => $from_state, 'width' => '20%'),
          array('data' => $select_str, 'width' => '80%'),
        ); // Get out of _POST
      }
    }
  }

  if ($is_enterprise) {

    // Community Translation
    $form['defaults']['lingotek_allow_community_translation'] = array(
      '#type' => 'checkbox',
      '#title' => t('Allow Community Translation'),
      '#description' => t('When enabled, anonymous site visitors will be presented with a link allowing them to contribute translations for this node.'),
      '#default_value' => variable_get('lingotek_allow_community_translation', 0),
    );

    /*
     * Misc. Options
     */

    // URL Alias Translation.
    $form['defaults']['lingotek_url_alias_translation'] = array(
      '#type' => 'select',
      '#title' => t('URL Alias Translation'),
      '#description' => t("Choose how you would like to translate the URL alias. The last option requires that you install both the Title and Pathauto modules, and define a path pattern, and check \"Enable Lingotek Translation\" for the Title field."),
      '#options' => lingotek_get_url_alias_translations(),
      '#default_value' => variable_get('lingotek_url_alias_translation', 1),
    );


    // Projects
    $projects = (class_exists('LingotekApi')) ? $api->listProjects() : array();
    $id = variable_get('lingotek_project', '');

    if ($id == '' || !array_key_exists($id, $projects)) { //No project id set, project deleted, or community changed to one without that project.  Try to find the Drupal project
      $id = array_search($site, $projects);
      if ($id === False) { //Setup a default Drupal project
        $id = lingotek_add_project($site);
        $projects = (class_exists('LingotekApi')) ? $api->listProjects() : array();
      }
      else { //Assign to an existing Drupal project
        variable_set('lingotek_project', $id);
      }
    }

    $sorted = asort($projects);
    $form['defaults']['lingotek_project'] = array(
      '#type' => 'select',
      '#title' => t('Default Project'),
      '#options' => $projects,
      '#description' => t('The default Lingotek Project with which translations will be associated.'),
      '#default_value' => $id,
    );


    // Workflows
    if ($workflows = $api->listWorkflows()) {
      $form['defaults']['lingotek_workflow'] = array(
        '#type' => 'select',
        '#title' => t('Default Workflow'),
        '#description' => t('The default Workflow to use when translating content.'),
        '#default_value' => variable_get('lingotek_workflow', ''),
        '#options' => $workflows,
      );
    }


    $vaults = $api->listVaults();
    $current_vault_id = variable_get('lingotek_vault', '');
    $personal_vault_count = ( isset($vaults['Personal Vaults']) ) ? count($vaults['Personal Vaults']) : 0;
    $community_vault_count = ( isset($vaults['Community Vaults']) ) ? count($vaults['Community Vaults']) : 0;

    // If no vault id is set, and we don't have any personal vaults, then create one and add it to our project.
    if (( $current_vault_id == '' ) && ( $personal_vault_count == 0 ) && ( $community_vault_count == 0 )) {
      $current_project_id = variable_get('lingotek_project', '');
      // But only if we have a ProjectID.
      if ($current_project_id != '') {
        $current_vault_id = lingotek_add_vault($site);
        lingotek_add_vault_to_project();
      }
    }

    $form['defaults']['lingotek_vault'] = array(
      '#type' => 'select',
      '#title' => t('Default Vault'),
      '#options' => $vaults,
      '#description' => t('The default Translation Memory Vault where translations are saved.'),
      '#default_value' => $current_vault_id,
    );
  }

  return $form;
}

function lingotek_admin_content_defaults_form_submit($form, &$form_state) {
  if (module_exists('workbench_moderation')) { // set or unset multiple transition globals based on form selection
    variable_set('lingotek_sync_workbench_moderation', $form_state['input']['wb_options']);
    $states = lingotek_get_workbench_moderation_states();
    if (isset($form_state['values']['lingotek_sync']) && $form_state['values']['lingotek_sync'] == 1) {
      if (isset($form_state['input']['wb_options']) && $form_state['input']['wb_options'] == 'increment') {
        foreach ($states as $state) {
          if (isset($form_state['input']['lingotek_sync_wb_select_' . $state])) {
            variable_set('lingotek_sync_wb_select_' . $state, $form_state['input']['lingotek_sync_wb_select_' . $state]);
          }
        }
      }
    }
    else {
      foreach ($states as $state) {
        if (variable_get('lingotek_sync_wb_select_' . $state)) {
          variable_del('lingotek_sync_wb_select_' . $state);
        }
      }
    }
  }
  system_settings_form_submit($form, $form_state);
}

/**
 * Utilities Form
 */
function lingotek_admin_utilities_form($form, &$form_state, $show_fieldset = FALSE) {

  $form['utilities'] = array(
    '#type' => $show_fieldset ? 'fieldset' : 'item',
    '#title' => t('Utilities'),
    '#description' => '',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#group' => 'additional_settings',
      //'#group' => 'administrative_settings',
  );

  $btn_reset_translations = array(
    '#type' => 'submit',
    '#value' => t('Reset translations'),
    '#submit' => array('lingotek_batch_reset_content'),
    '#attributes' => array(
      'onclick' => 'return confirm("' . t('Are you sure?\n\nAll of your content will have to be retranslated.') . '");'
    ),
  );

  $btn_cleanup = array(
    '#type' => 'submit',
    '#value' => t('Run cleanup'),
    '#submit' => array('lingotek_cleanup_utility')
  );

  $btn_callback_url_update = array(
    '#type' => 'submit',
    '#value' => t('Update URL'),
    '#submit' => array('lingotek_notify_url_update')
  );

  // Start: Not sure why, but without these below, the table button do not work properly
  $form['utilities'][] = array_merge($btn_reset_translations, array('#attributes' => array('style' => 'display: none;')));
  $form['utilities'][] = array_merge($btn_cleanup, array('#attributes' => array('style' => 'display: none;')));
  $form['utilities'][] = array_merge($btn_callback_url_update, array('#attributes' => array('style' => 'display: none;')));
  // End: Not sure why

  $rows = array(
    array(
      array('data' => array(
          '#type' => 'markup',
          '#prefix' => t('Reset Translations') . '<div class="description">',
          '#markup' => t("Disassociates all locally-published translations from their counterparts on Lingotek's servers. This allows for your Drupal content to be re-uploaded to Lingotek, so that it can be retranslated entirely using the currently-defined workflow. Also resets the translation management settings for all nodes to those defined in the Content Defaults Settings."),
          '#suffix' => '</div>'
        )),
      array('data' => $btn_reset_translations)
    ),
    array(
      array('data' => array(
          '#type' => 'markup',
          '#prefix' => t('Cleanup') . '<div class="description">',
          '#markup' => t("The cleanup utility identifies translatable content and processes all existing field data for translation-enabled fields on nodes, ensuring that if data was entered before enabling field translation on a field that the existing field data is copied over to the parent node's current language."),
          '#suffix' => '</div>'
        )),
      array('data' => $btn_cleanup),
    ),
    array(
      array('data' => array(
          '#type' => 'markup',
          '#prefix' => t('Notification Callback URL') . '<div class="description">',
          '#markup' => t("Update the notification callback URL.  This can be run whenever your site is moved (e.g., domain name change or sub-directory re-location) or whenever you would like your security token re-generated.")
          . '<br/>' . t("Current notification callback URL:") . ' <i>' . variable_get('lingotek_notify_url', 'None') . '</i>',
          '#suffix' => '</div>'
        )),
      array('data' => $btn_callback_url_update),
    )
  );
  $header = array();
  $form['utilities']['utility_table'] = array(
    '#markup' => theme('table', array('header' => $header, 'rows' => $rows))
  );

  return $form;
}

/**
 * Troubleshooting and Logging Form
 */
function lingotek_admin_logging_form($form, &$form_state, $show_fieldset = FALSE) {

  $form['logging'] = array(
    '#type' => $show_fieldset ? 'fieldset' : 'item',
    '#title' => t('Logging'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#group' => 'administrative_settings',
    'actions' => array(
      '#type' => 'actions',
      'submit' => array(
        '#type' => 'submit',
        '#value' => t('Save')
      )
    ),
    '#submit' => array('lingotek_admin_logging_form_submit')
  );

  $form['logging'][] = array(
    '#type' => 'item',
    '#description' => t('Help troubleshoot any issues with the module. The logging enabled below will be available in the') . ' ' . l('Drupal watchdog', 'admin/reports/dblog') . '.'
  );

  $form['logging']['lingotek_error_log'] = array(
    '#type' => 'checkbox',
    '#title' => t('Error Logging'),
    '#description' => t('This prints errors and warnings to the web server\'s error logs in addition to adding them to watchdog.'),
    '#default_value' => variable_get('lingotek_error_log', 1),
  );

  $form['logging']['lingotek_warning_log'] = array(
    '#type' => 'checkbox',
    '#title' => t('Warning Logging'),
    '#description' => t('This logs any warnings in watchdog and the web server\'s error logs.'),
    '#default_value' => variable_get('lingotek_warning_log', 1),
  );

  $form['logging']['lingotek_api_debug'] = array(
    '#type' => 'checkbox',
    '#title' => t('API & Interaction Logging'),
    '#description' => t('Logs the timing and request/response details of all Lingotek API calls. Additionally, interaction calls (e.g., endpoint, notifications) made back to Drupal will be logged with this enabled.'),
    '#default_value' => variable_get('lingotek_api_debug', LINGOTEK_DEV ? 1 : 0),
  );

  $form['logging']['lingotek_trace_log'] = array(
    '#type' => 'checkbox',
    '#title' => t('Trace Logging'),
    '#description' => t('This logs trace debug messages to watchdog and the web server\'s error logs. (This logging is extremely verbose.)'),
    '#default_value' => variable_get('lingotek_trace_log', 0),
  );

  $form['logging']['lingotek_flush_cache'] = array(
    '#type' => 'checkbox',
    '#title' => t('Never cache'),
    '#description' => t('Skips caching so you can test easier.  This avoids frequent polling of fresh data from Lingotek.  Only those with Developer permissions will have caching disabled.'),
    '#default_value' => variable_get('lingotek_flush_cache', 0),
  );
  return $form;
}

function lingotek_admin_logging_form_submit($form, &$form_state) {
  system_settings_form_submit($form, $form_state);
}

function lingotek_admin_configuration_view($form_short_id = NULL, $show_fieldset = TRUE) {

  lingotek_is_module_setup();
  $account = LingotekAccount::instance();

  $api = LingotekApi::instance();
  $site = variable_get('site_name', 'Drupal Site');
  $is_enterprise = $account->isEnterprise();


  //$form_short_id values:  config, logging, utilities, language_switcher
  $form_id = "lingotek_admin_{$form_short_id}_form";
  if (!is_null($form_short_id) && function_exists($form_id)) {
    return drupal_get_form($form_id);
  }

  $show_fieldset = FALSE;

  $output = array();

  $output['lingotek'] = array(
    '#type' => 'vertical_tabs',
    '#attached' => array(
      'js' => array(drupal_get_path('module', 'lingotek') . '/js/lingotek.admin.js'),
    ),
  );
  
  $account_summary = array(
    drupal_get_form('lingotek_admin_account_status_form', $show_fieldset),
    drupal_get_form('lingotek_admin_connection_form', $show_fieldset),
  );

  $output['lingotek'][] = lingotek_wrap_in_fieldset($account_summary, 'Account');

  $connected = $api->testAuthentication();

  if ($connected) {
    $arr = array(
      drupal_get_form('lingotek_admin_node_translation_settings_form', $show_fieldset),
      drupal_get_form('lingotek_admin_content_defaults_form', $show_fieldset),
    );
    if ($is_enterprise) {
      $arr[] = drupal_get_form('lingotek_admin_advanced_parsing_form', $show_fieldset);
    }

    $output['lingotek'][] = lingotek_wrap_in_fieldset($arr, 'Translate Content');
    $output['lingotek'][] = lingotek_wrap_in_fieldset(drupal_get_form('lingotek_admin_comment_translation_settings_form', $show_fieldset), 'Translate Comments');
    $output['lingotek'][] = lingotek_wrap_in_fieldset(drupal_get_form('lingotek_admin_additional_translation_settings_form', $show_fieldset), 'Translate Configuration');
    $output['lingotek'][] = lingotek_wrap_in_fieldset(drupal_get_form('lingotek_admin_prefs_form', $show_fieldset), 'Preferences');
    //$output[] = drupal_get_form('lingotek_admin_language_switcher_form', $show_fieldset);
    //$output['lingotek'][] = lingotek_wrap_in_fieldset(drupal_get_form('lingotek_admin_utilities_form', $show_fieldset), 'Utilities');
    $logging_utilities = array(
      drupal_get_form('lingotek_admin_logging_form', $show_fieldset),
      drupal_get_form('lingotek_admin_utilities_form', $show_fieldset),
    );
    $output['lingotek'][] = lingotek_wrap_in_fieldset($logging_utilities, 'Logging & Utilities');
  }



  return $output;
}

function lingotek_wrap_in_fieldset($form, $title) {
  return array(
    '#type' => 'fieldset',
    '#group' => 'lingotek',
    '#title' => t($title),
    '#attributes' => array('class' => array('lingotek-' . strtolower(str_replace(' ', '-', $title)))),
    'children' => $form,
  );
}

function lingotek_admin_language_switcher_form_submit($form, $form_state) {

  $block = array(
    'status' => $form_state['values']['enabled'],
    'weight' => 0,
    'region' => $form_state['values']['region'],
    'module' => 'locale',
    'delta' => 'language',
    'theme' => $form_state['values']['theme']
  );

  db_update('block')
      ->fields(array(
        'status' => $block['status'],
        'weight' => $block['weight'],
        'region' => $block['region'],
      ))
      ->condition('module', $block['module'])
      ->condition('delta', $block['delta'])
      ->condition('theme', $block['theme'])
      ->execute();

  //If the site does not yet have more than 1 language, drupal doesn't bootstrap
  //some language functions. So here we force this file to be loaded.
  include_once DRUPAL_ROOT . '/includes/language.inc';

  //The array variable 'language_negotiation_language' has a entry for each detection method.
  //Thus if the max is 1 the only possible enabled method is default.
  //Note that the variable is empty on a new drupal install.
  if ($form_state['values']['enabled'] && count(variable_get('language_negotiation_language')) <= 1) {
    $types = array(
      LOCALE_LANGUAGE_NEGOTIATION_URL => 2,
      LOCALE_LANGUAGE_NEGOTIATION_BROWSER => 4,
    );
    language_negotiation_set(LANGUAGE_TYPE_INTERFACE, $types);
  }

  // Flush cache
  cache_clear_all();

  $action = $form_state['values']['enabled'] ? t('now enabled') : t('not enabled');
  $message_type = $form_state['values']['enabled'] ? 'status' : 'warning';
  drupal_set_message(t('The default language switcher is @action.', array('@action' => $action)), $message_type);
}

/**
 * Custom form handler for upgrading a site from using Lingotek's simple to advanced XML parsing of content.
 */
function lingotek_handle_advanced_xml_upgrade($form, $form_state) {
  if ($form_state['values']['lingotek_advanced_parsing']) {
    $results = db_select('lingotek', 'l')->fields('l', array('nid'))->distinct()->execute();
    $operations = array();
    foreach ($results as $result) {
      $node = lingotek_node_load_default($result->nid, NULL, TRUE);
      if (!empty($node->nid)) {
        $operations[] = array('lingotek_advanced_parsing_update_node', array($node->nid));
      }
    }

    $batch = array(
      'title' => t('Lingotek Advanced Parsing Updater'),
      'operations' => $operations,
      'file' => 'lingotek.admin.inc',
      'finished' => 'lingotek_advanced_parsing_update_finished',
    );

    // The admin form might not have finished processing yet, but if we're here, we're moving to advanced processing.
    // Ensure the appropriate variable is already set.
    variable_set('lingotek_advanced_parsing', TRUE);

    batch_set($batch);
  }
}

/*
 * AJAX callback to update transition options
 */

function lingotek_workbench_moderation_callback_transitions_default($form, &$form_state) {
  return $form['defaults']['lingotek_sync_wb_select'];
}
