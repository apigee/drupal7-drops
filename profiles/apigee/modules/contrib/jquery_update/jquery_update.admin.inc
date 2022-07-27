<?php

/**
 * @file
 * Provides the administration settings for jQuery Update.
 */

/**
 * Admin settings menu callback.
 *
 * @see jquery_update_menu()
 */
function jquery_update_settings_form() {
  // Vertical Tabs.
  $form['jquery_update'] = array(
    '#type' => 'vertical_tabs',
    '#weight' => 99,
  );

  // Provide the form item to choose which jQuery version to use.
  $default_version = variable_get('jquery_update_jquery_version', '1.10');
  $custom_jquery = variable_get('jquery_update_custom_version_jquery', FALSE);
  if (!empty($custom_jquery)) {
    $default_version = $custom_jquery;
  }
  $version_options = jquery_update_get_version_options(FALSE);
  $form['jquery_update_jquery_version'] = array(
    '#type' => 'select',
    '#title' => t('Default jQuery version'),
    '#options' => $version_options,
    '#default_value' => $default_version,
    '#description' => t('Select which version of jQuery to use on the site.'),
  );

  // Theme-specific override version
  $themes = list_themes();
  $theme_default = variable_get('theme_default', FALSE);
  $admin_theme = variable_get('admin_theme', FALSE);
  $header = array(t('Theme'), t('jQuery version'), t('Operations'));
  $rows = array();
  // Go through all themes.
  foreach ($themes as $theme_key => $theme) {
    // Skip disabled themes, but only if they are not configured as admin
    // theme. This is an inconsistency in drupal core, that you can select a
    // disabled theme as admin theme.
    if (!$theme->status && $theme_key !== $admin_theme) {
      continue;
    }

    // Retrieve the version jQuery for this theme.
    $theme_version = theme_get_setting('jquery_update_jquery_version', $theme_key);

    // Replace or modify the version name to be displayed.
    if (empty($theme_version)) {
      $theme_version = t('Site Default');
    }
    elseif (in_array($theme_version, array_keys($version_options))) {
      $theme_version = $version_options[$theme_version];
    }
    else {
      $theme_version .= ' (' . t('unknown version') . ')';
    }

    // Provide additional information for default and admin themes.
    $theme_name = $theme->info['name'];
    if ($theme_key === $theme_default && ($theme_key === $admin_theme || empty($admin_theme))) {
      $theme_name .= ' (' . t('default/admin theme') . ')';
    }
    elseif ($theme_key === $theme_default) {
      $theme_name .= ' (' . t('default theme') . ')';
    }
    elseif ($theme_key === $admin_theme) {
      $theme_name .= ' (' . t('admin theme') . ')';
    }

    // Construct the table row.
    $rows[] = array(
      $theme_name,
      $theme_version,
      l(t('Configure'), 'admin/appearance/settings/' . $theme_key, array(
        'attributes' => array(
          'class' => array(
            'module-link',
            'module-link-configure',
          ),
        ),
        'query' => drupal_get_destination(),
        'fragment' => 'edit-jquery-update',
      )),
    );
  }

  $form['themes'] = array(
    '#type' => 'fieldset',
    '#title' => t('Theme Overrides'),
    '#description' => t('You can override the default jQuery version above on each themes settings page. This is useful for administrative based themes.'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#tree' => FALSE,
    '#weight' => -2,
    '#group' => 'jquery_update',
  );
  $form['themes']['overrides'] = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
  );


  $form['performance'] = array(
    '#type' => 'fieldset',
    '#title' => t('Performance'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#tree' => FALSE,
    '#weight' => -1,
    '#group' => 'jquery_update',
    '#description' => t('Modify how jQuery is loaded to increase download and render performance.')
  );

  $form['performance']['jquery_update_compression_type'] = array(
    '#type' => 'radios',
    '#title' => t('jQuery compression level'),
    '#options' => array(
      'min' => t('Production (minified)'),
      'none' => t('Development (uncompressed)'),
    ),
    // Do not show this field if jQuery version is default
    '#states' => array(
      'invisible' => array(
        ':input[name=jquery_update_jquery_version]' => array('value' => "default"),
      ),
    ),
    '#default_value' => variable_get('jquery_update_compression_type', 'min'),
  );
  $form['performance']['jquery_update_jquery_cdn'] = array(
    '#type' => 'select',
    '#title' => t('jQuery and jQuery UI CDN'),
    '#options' => array(
      'none' => t('None'),
      'google' => t('Google'),
      'microsoft' => t('Microsoft'),
      'jquery' => t('jQuery'),
    ),
    // Do not show this field if jQuery version is default
    '#states' => array(
      'invisible' => array(
        ':input[name=jquery_update_jquery_version]' => array('value' => "default"),
      ),
    ),
    '#default_value' => variable_get('jquery_update_jquery_cdn', 'none'),
    '#description' => t('Use jQuery and jQuery UI from a CDN. If the CDN is not available the local version of jQuery and jQuery UI will be used.'),
  );

  $form['jquery_migrate'] = array(
    '#type' => 'fieldset',
    '#title' => t('jQuery Migrate'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#tree' => FALSE,
    '#group' => 'jquery_update',
    '#description' => t('<a href="!url">jQuery Migrate</a> can be used to detect and restore APIs or features that have been deprecated in jQuery and removed as of version 1.9 or higher.', array(
      '!url' => 'http://github.com/jquery/jquery-migrate/#readme',
    )),
  );

  $form['jquery_migrate']['jquery_update_jquery_migrate_enable'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable jQuery Migrate Plugin'),
    '#default_value' => variable_get('jquery_update_jquery_migrate_enable', FALSE),
    '#description' => t('Even if jQuery Migrate is enabled, it will not be loaded if the current page\'s jQuery version is lower than 1.9.'),
  );

  $jquery_migrate_states = array(
    'visible' => array(
      ':input[name="jquery_migrate[jquery_update_jquery_migrate_enable]"]' => array('checked' => TRUE),
    ),
  );
  $form['jquery_migrate']['jquery_update_jquery_migrate_cdn'] = array(
    '#type' => 'select',
    '#title' => t('jQuery Migrate CDN'),
    '#options' => array(
      'none' => t('None'),
      'jquery' => t('jQuery'),
    ),
    '#default_value' => variable_get('jquery_update_jquery_migrate_cdn', 'none'),
    '#description' => t('Load the jQuery Migrate plugin using a CDN. If the CDN is not available the local module version of the plugin will be used instead.'),
    '#states' => $jquery_migrate_states,
  );

  $jquery_migrate_api_url = 'https://github.com/jquery/jquery-migrate/#migrate-plugin-api';
  $form['jquery_migrate']['jquery_update_jquery_migrate_warnings'] = array(
    '#type' => 'checkbox',
    '#title' => t('Console warnings'),
    '#default_value' => variable_get('jquery_update_jquery_migrate_warnings', FALSE),
    '#description' => t('Toggle the <a href="!url">generation of console warnings</a> when using the debug version of jQuery Migrate.', array(
      '!url' => $jquery_migrate_api_url,
    )),
    '#states' => $jquery_migrate_states,
  );

  $form['jquery_migrate']['jquery_update_jquery_migrate_trace'] = array(
    '#type' => 'checkbox',
    '#title' => t('Console trace'),
    '#default_value' => variable_get('jquery_update_jquery_migrate_trace', FALSE),
    '#description' => t('Toggle the <a href="!url">generation of console trace messages</a> when using the debug version of jQuery Migrate.', array(
      '!url' => $jquery_migrate_api_url,
    )),
    '#states' => $jquery_migrate_states,
  );

  $form['jquery_custom'] = array(
    '#type' => 'fieldset',
    '#title' => t('Custom paths'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#tree' => FALSE,
    '#group' => 'jquery_update',
    '#description' => t('Custom paths for jQuery libraries (e.g. CDN URLs or paths to local files).'),
  );
  $form['jquery_custom']['jquery_update_custom_path_jquery'] = array(
    '#type' => 'textfield',
    '#title' => t('Custom jQuery path'),
    '#default_value' => variable_get('jquery_update_custom_path_jquery', ''),
    '#description' => t('Example: %url or %path', array(
        '%url' => 'https://code.jquery.com/jquery-3.6.0.js',
        '%path' => '/sites/default/files/jquery_update/jquery.js'
      )
    ),
  );
  $form['jquery_custom']['jquery_update_custom_version_jquery'] = array(
    '#type' => 'textfield',
    '#title' => t('Custom jQuery version'),
    '#default_value' => variable_get('jquery_update_custom_version_jquery', ''),
    '#description' => t('Example: %version', array('%version' => '3.6.0')),
  );
  $form['jquery_custom']['jquery_update_custom_path_jqueryui'] = array(
    '#type' => 'textfield',
    '#title' => t('Custom jQuery UI path'),
    '#default_value' => variable_get('jquery_update_custom_path_jqueryui', ''),
    '#description' => t('Example: %url or %path', array(
        '%url' => 'https://cdnjs.cloudflare.com/ajax/libs/jqueryui/1.12.1/jquery-ui.min.js',
        '%path' => '/sites/default/files/jquery_update/jquery-ui.min.js'
      )
    ),
  );
  $form['jquery_custom']['jquery_update_custom_version_jqueryui'] = array(
    '#type' => 'textfield',
    '#title' => t('Custom jQuery UI version'),
    '#default_value' => variable_get('jquery_update_custom_version_jqueryui', ''),
    '#description' => t('Example: %version', array('%version' => '1.13.0')),
  );
  $form['jquery_custom']['jquery_update_custom_path_cookie'] = array(
    '#type' => 'textfield',
    '#title' => t('Custom jQuery Cookie path'),
    '#default_value' => variable_get('jquery_update_custom_path_cookie', ''),
    '#description' => t('Example: %url or %path', array(
        '%url' => 'https://cdn.jsdelivr.net/gh/carhartl/jquery-cookie@1.4.1/jquery.cookie.js',
        '%path' => '/sites/default/files/jquery_update/jquery.cookie.js'
      )
    ),
  );
  $form['jquery_custom']['jquery_update_custom_version_jquery_cookie'] = array(
    '#type' => 'textfield',
    '#title' => t('Custom jQuery Cookie version'),
    '#default_value' => variable_get('jquery_update_custom_version_jquery_cookie', ''),
    '#description' => t('Example: %version', array('%version' => '1.4.0')),
  );
  $form['jquery_custom']['jquery_update_custom_path_form'] = array(
    '#type' => 'textfield',
    '#title' => t('Custom jQuery Form path'),
    '#default_value' => variable_get('jquery_update_custom_path_form', ''),
    '#description' => t('Example: %url or %path', array(
        '%url' => 'https://cdn.jsdelivr.net/gh/jquery-form/form@4.2.1/dist/jquery.form.min.js',
        '%path' => '/sites/default/files/jquery_update/jquery.form.js'
      )
    ),
  );
  $form['jquery_custom']['jquery_update_custom_version_jquery_form'] = array(
    '#type' => 'textfield',
    '#title' => t('Custom jQuery Form version'),
    '#default_value' => variable_get('jquery_update_custom_version_jquery_form', ''),
    '#description' => t('Example: %version', array('%version' => '4.2.1')),
  );
  $form['jquery_custom']['jquery_update_custom_path_migrate'] = array(
    '#type' => 'textfield',
    '#title' => t('Custom jQuery Migrate path'),
    '#default_value' => variable_get('jquery_update_custom_path_migrate', ''),
    '#description' => t('Example: %url or %path', array(
        '%url' => 'https://cdn.jsdelivr.net/npm/jquery-migrate@3.3.2/dist/jquery-migrate.min.js',
        '%path' => '/sites/default/files/jquery_update/jquery-migrate.js'
      )
    ),
  );
  $form['jquery_custom']['jquery_update_custom_version_jquery_migrate'] = array(
    '#type' => 'textfield',
    '#title' => t('Custom jQuery Migrate version'),
    '#default_value' => variable_get('jquery_update_custom_version_jquery_migrate', ''),
    '#description' => t('Example: %version', array('%version' => '3.3.2')),
  );

  return system_settings_form($form);
}
