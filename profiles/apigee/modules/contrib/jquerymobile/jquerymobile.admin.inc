<?php

/*
 * @file
 *
 */

/**
 * Form builder; Configure user settings for this site.
 *
 * @ingroup forms
 * @see system_settings_form()
 */ 
function jquerymobile_admin_settings() {

  /* GLOBAL SETTINGS */
  $form['jquerymobile'] = array(
    '#type' => 'fieldset', 
    '#title' => t('Global Settings'),
    '#collapsible' => FALSE,
    '#access' => user_access('manage jquerymobile theme settings'),
  );
  $form['jquerymobile']['jquerymobile_mobile_themes'] = array(
    '#type' => 'checkboxes', 
    '#title' => t('Mobile Theme'),
    '#options' => _jquerymobile_get_theme_list(),
    '#default_value' => variable_get('jquerymobile_mobile_themes', array()),
    '#description' => t('Select which theme will use these settings.'),
    '#multiple' => TRUE, 
    '#size' => min(5, count(variable_get('jquerymobile_mobile_themes', array()))),
  );
  
  /*    FILE SETTINGS    */
  $form['jquerymobile']['file_settings'] = array(
    '#type' => 'fieldset', 
    '#title' => t('File Settings'),
    '#collapsible' => FALSE,
    '#access' => user_access('manage jquerymobile files settings'),
  );  
  $form['jquerymobile']['file_settings']['jquerymobile_minify'] = array(
    '#type' => 'checkbox', 
    '#title' => t('Use minified versions (recommended for production).'),
    '#options' => array(
      FALSE,
      TRUE,
    ),
    '#default_value' => variable_get('jquerymobile_minify', FALSE),
  ); 
  $form['jquerymobile']['file_settings']['jquerymobile_server_type'] = array(
    '#type' => 'radios', 
    '#title' => t('Where are the framework files located?'),
    '#options' => array(
      'local' => t('Local (sites/all/libraries)'),
      'hosted' => t('Hosted (http://code.jquery.com)'),
      'other' => t('Other'),
    ),
    '#default_value' => variable_get('jquerymobile_server_type', 'local'),
  );   
  $form['jquerymobile']['file_settings']['jquerymobile_jquerymobile_version'] = array(
    '#type' => 'textfield', 
    '#default_value' => variable_get('jquerymobile_jquerymobile_version', '1.0.1'),
    '#title' => t('jQuery Mobile Version'),
    '#description' => t('Enter the version of the jQuery library to use. <i>ex: jquerymobile-1.0.1 = 1.0.1</i><br><b>Default: </b>1.0.1'),
  );
  $form['jquerymobile']['file_settings']['jquerymobile_jquery_version'] = array(
    '#type' => 'textfield', 
    '#default_value' => variable_get('jquerymobile_jquery_version', '1.6.4'),
    '#title' => t('jQuery Version'),
    '#description' => t('Enter the version of the jQuery library to use. <i>ex: jquery-1.6.4 = 1.6.4</i><br><b>Default: </b>1.6.4'),
  );
  $form['jquerymobile']['file_settings']['jquerymobile_jquery_files_path'] = array(
    '#type' => 'textfield', 
    '#default_value' => variable_get('jquerymobile_jquery_files_path', NULL),
    '#title' => t('Other jQuery Mobile File Location'),
    '#description' => t('Enter the location where the jquery files are hosted (no trailing slashes).'),
    '#states' => array(
      'visible' => array(
        ':input[name="jquerymobile_server_type"]' => array('value' => 'other'),
      ),
    ),
  );
  $form['jquerymobile']['file_settings']['jquerymobile_jqm_files_path'] = array(
    '#type' => 'textfield', 
    '#default_value' => variable_get('jquerymobile_jqm_files_path', NULL),
    '#title' => t('Other jQuery File Location'),
    '#description' => t('Enter the location where the jquerymobile files are hosted (no trailing slashes).'),
    '#states' => array(
      'visible' => array(
        ':input[name="jquerymobile_server_type"]' => array('value' => 'other'),
      ),
    ),
  );
  $form['jquerymobile']['file_settings']['jquerymobile_library_path'] = array(
    '#type' => 'textfield', 
    '#default_value' => variable_get('jquerymobile_library_path', 'sites/all/libraries'),
    '#title' => t('Local File Location'),
    '#description' => t('Enter the location where the jquery and jquerymobile files are hosted (no trailing slashes).'),
    '#states' => array(
      'visible' => array(
        ':input[name="jquerymobile_server_type"]' => array('value' => 'local'),
      ),
    ),
  );
  
  $form['#submit'][] = 'jquerymobile_admin_settings_submit';

  return system_settings_form($form);
}
  
function jquerymobile_admin_local_settings($form, &$form_state, $theme = NULL) {
  $settings = _jquerymobile_get_settings($theme);

  /* OPTIONS VALUES */
  $transitions = array(
    '' => t('default'),
    'slide' => t('slide'),
    'slideup' => t('slideup'),
    'slidedown' => t('slidedown'),
    'pop' => t('pop'),
    'fade' => t('fade')
  );
  $booleans = array(
    1 => t('True'), 
    0 => t('False')
  );

  $form['jquerymobile']['theme'] = array(
    '#type' => 'hidden', 
    '#default_value' => $settings['theme'] ? $settings['theme'] : $theme,
  );

  /* THEME SETTINGS */
  $form['jquerymobile']['theme_settings'] = array(
    '#type' => 'fieldset', 
    '#title' => t('Display Settings'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#access' => user_access('manage jquerymobile advanced settings'),
  );
  $form['jquerymobile']['theme_settings']['front'] = array(
    '#type'          => 'checkbox', 
    '#title'         => t('Display "River of News" as a list'),
    '#description'   => t('If the front page is the default river of news, this option will create a link list form the teasers.'),
    '#default_value' => $settings['front'],
  );
  
  $form['jquerymobile']['theme_settings']['custom_themes'] = array(
    '#type' => 'textarea', 
    '#default_value' => $settings['custom_themes'],
    '#title' => t('Custom CSS Swatches'),
    '#description' => t('Add new swatches or rename existing swatches. One per line, in <em>css_key|value</em> format, like <em>a|My Swatch</em> or <em>g|Red</em>.'),
  );
  
  $form['jquerymobile']['advanced_settings'] = array(
    '#type' => 'fieldset', 
    '#title' => t('Advanced Settings'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#access' => user_access('manage jquerymobile advanced settings'),
  );
  /* ns */
  $form['jquerymobile']['advanced_settings']['ns'] = array(
    '#type' => 'textfield', 
    '#title' => t('Namespace') . ' (<em>ns</em>)', 
    '#default_value' => $settings['ns'], 
    '#description' => t('The namespace used in data- attributes, for example, data-role. Can be set to anything, including a blank string which is the default. When using, it\'s clearest if you include a trailing dash, such as "mynamespace-" which maps to data-mynamespace-foo="...".<br>NOTE: if you\'re using data- namespacing, you\'ll need to manually update/override one selector in the theme CSS. The following data selectors should incorporate the namespace you\'re using:<br>.ui-mobile [data-mynamespace-role=page], .ui-mobile [data-mynamespace-role=dialog], .ui-page { ...'),
  );
  /* autoInitializePage */
  $form['jquerymobile']['advanced_settings']['autoInitializePage'] = array(
    '#type' => 'radios', 
    '#title' => t('Auto Initialize Page') . ' (<em>autoInitializePage</em>)', 
    '#default_value' => $settings['autoinitializepage'],
    '#description' => t('When the DOM is ready, the framework should automatically call $.mobile.initializePage. If FALSE, page will not initialize, and will be visually hidden until until $.mobile.initializePage is manually called.'),
    '#options' => $booleans,
  );
  /* subPageUrlKey */
  $form['jquerymobile']['advanced_settings']['subPageUrlKey'] = array(
    '#type' => 'textfield', 
    '#title' => t('Sub Page URL Key') . ' (<em>subPageUrlKey</em>)', 
    '#default_value' => $settings['subpageurlkey'], 
    '#description' => t('The url parameter used for referencing widget-generated sub-pages (such as those generated by nested listviews). Translates to to example.html&ui-page=subpageIdentifier. The hash segment before &ui-page= is used by the framework for making an Ajax request to the URL where the sub-page exists.'),
  );
  /* activePageClass */
  $form['jquerymobile']['advanced_settings']['activePageClass'] = array(
    '#type' => 'textfield', 
    '#title' => t('Active Page Class') . ' (<em>activePageClass</em>)', 
    '#default_value' => $settings['activepageclass'], 
    '#description' => t('The class assigned to page currently in view, and during transitions'),
  );
  /* activeBtnClass */
  $form['jquerymobile']['advanced_settings']['activeBtnClass'] = array(
    '#type' => 'textfield', 
    '#title' => t('Active Button Class') . ' (<em>activeBtnClass</em>)', 
    '#default_value' => $settings['activebtnclass'], 
    '#description' => t('The class used for "active" button state, from CSS framework.'),
  );
  /* ajaxEnabled */
  $form['jquerymobile']['advanced_settings']['ajaxEnabled'] = array(
    '#type' => 'radios', 
    '#title' => t('AJAX Enabled') . ' (<em>ajaxEnabled</em>)', 
    '#default_value' => $settings['ajaxenabled'],
    '#description' => t('jQuery Mobile will automatically handle link clicks and form submissions through Ajax, when possible. If FALSE, url hash listening will be disabled as well, and urls will load as regular http requests.'),
    '#options' => $booleans,
  );
  /* linkBindingEnabled */
  $form['jquerymobile']['advanced_settings']['linkBindingEnabled'] = array(
    '#type' => 'radios', 
    '#title' => t('Link Binding Enabled') . ' (<em>linkBindingEnabled</em>)', 
    '#default_value' => $settings['linkbindingenabled'],
    '#description' => t('jQuery Mobile will automatically bind the clicks on anchor tags in your document. Setting this options to FALSE will prevent all anchor click handling including the addition of active button state and alternate link bluring. This should only be used when attempting to delegate the click management to another library or custom code.'),
    '#options' => $booleans,
  );
  /* hashListeningEnabled */
  $form['jquerymobile']['advanced_settings']['hashListeningEnabled'] = array(
    '#type' => 'radios', 
    '#title' => t('Hash Listening Enabled') . ' (<em>hashListeningEnabled</em>)', 
    '#default_value' => $settings['hashlisteningenabled'],
    '#description' => t('jQuery Mobile will automatically listen and handle changes to the location.hash. Disabling this will prevent jQuery Mobile from handling hash changes, which allows you to handle them yourself, or simply to use simple deep-links within a document that scroll to a particular ID.'),
    '#options' => $booleans,
  );
  /* pushStateEnabled */
  $form['jquerymobile']['advanced_settings']['pushStateEnabled'] = array(
    '#type' => 'radios', 
    '#title' => t('Push State Enabled') . ' (<em>pushStateEnabled</em>)', 
    '#default_value' => $settings['pushstateenabled'],
    '#description' => t('Enhancement to use history.replaceState in supported browsers, to convert the hash-based Ajax URL into the full document path. Note that we <a href="http://jquerymobile.com/demos/1.0/docs/pages/page-navmodel.html">recommend</a> disabling this feature if Ajax is disabled or if extensive use of external links are used.'),
    '#options' => $booleans,
  );
  /* defaultPageTransition */
  $form['jquerymobile']['advanced_settings']['defaultPageTransition'] = array(
    '#type' => 'select', 
    '#title' => t('Default Page Transition') . ' (<em>defaultPageTransition</em>)', 
    '#default_value' => $settings['defaultpagetransition'], 
    '#description' => t('Set the default transition for page changes that use Ajax. Set to \'none\' for no transitions by default.'),
    '#options' => $transitions,
  );
  /* touchOverflowEnabled */
  $form['jquerymobile']['advanced_settings']['touchOverflowEnabled'] = array(
    '#type' => 'radios', 
    '#title' => t('Touch Overflow Enabled') . ' (<em>touchOverflowEnabled</em>)', 
    '#default_value' => $settings['touchoverflowenabled'],
    '#description' => t('Enable smoother page transitions and TRUE fixed toolbars in devices that support both the overflow: and overflow-scrolling: touch; CSS properties.'),
    '#options' => $booleans,
  );
  /* defaultDialogTransition */
  $form['jquerymobile']['advanced_settings']['defaultDialogTransition'] = array(
    '#type' => 'select', 
    '#title' => t('Default Dialog Transition') . ' (<em>defaultDialogTransition</em>)', 
    '#default_value' => $settings['defaultdialogtransition'], 
    '#description' => t('Set the default transition for dialog changes that use Ajax. Set to \'none\' for no transitions by default.'),
    '#options' => $transitions,
  );
  /* minScrollBack */
  $form['jquerymobile']['advanced_settings']['minScrollBack'] = array(
    '#type' => 'textfield', 
    '#title' => t('Minimum Scroll Back') . ' (<em>minScrollBack</em>)', 
    '#default_value' => $settings['minscrollback'],
    '#description' => t('Minimum scroll distance that will be remembered when returning to a page.'),
  );
  /* loadingMessage */  
  $form['jquerymobile']['advanced_settings']['loadingMessage'] = array(
    '#type' => 'textfield', 
    '#title' => t('Loading Message') . ' (<em>loadingMessage</em>)', 
    '#default_value' => $settings['loadingmessage'], 
    '#description' => t('Set the text that appears when a page is loading. If set to FALSE, the message will not appear at all.'),
  );
  /* pageLoadErrorMessage */
  $form['jquerymobile']['advanced_settings']['pageLoadErrorMessage'] = array(
    '#type' => 'textfield', 
    '#title' => t('Page Load Error Message') . ' (<em>pageLoadErrorMessage</em>)', 
    '#default_value' => $settings['pageloaderrormessage'], 
    '#description' => t('Set the text that appears when a page fails to load through Ajax.'),
  );
  
  $form['#validate'][] = 'jquerymobile_admin_local_settings_validate';
  $form['#submit'][] = 'jquerymobile_admin_local_settings_submit';
  
  return system_settings_form($form);
}

function jquerymobile_admin_local_settings_validate($form, &$form_state, $theme = NULL) {

  if(!is_numeric($form_state['values']['minScrollBack'])) {
    form_set_error('minScrollBack', t('The value') . ' <i>minScrollBack</i> ' . t('must be a number.'));
  }

  if(is_null($form_state['values']['subPageUrlKey']) || $form_state['values']['subPageUrlKey'] == '') {
    form_set_error('subPageUrlKey', t('The value of') . ' <i>subPageUrlKey</i> ' . t('must not be null.'));
  }

  if(is_null($form_state['values']['activePageClass']) || $form_state['values']['activePageClass'] == '') {
    form_set_error('activePageClass', t('The value of') . ' <i>activePageClass</i> ' . t('must not be null.'));
  }

  if(is_null($form_state['values']['activeBtnClass']) || $form_state['values']['activeBtnClass'] == '') {
    form_set_error('activeBtnClass', t('The value of') . ' <i>activeBtnClass</i> ' . t('must not be null.'));
  }

  if(is_null($form_state['values']['loadingMessage']) || $form_state['values']['loadingMessage'] == '') {
    form_set_error('loadingMessage', t('The value of') . ' <i>loadingMessage</i> ' . t('must not be null.'));
  }

  if(is_null($form_state['values']['pageLoadErrorMessage']) || $form_state['values']['pageLoadErrorMessage'] == '') {
    form_set_error('pageLoadErrorMessage', t('The value of') . ' <i>pageLoadErrorMessage</i> ' . t('must not be null.'));
  }

}

function jquerymobile_admin_settings_validate($form, &$form_state, $theme = NULL) {
  if (!is_dir($form_state['values']['jquerymobile_library_path'])) {
    form_set_error('jquerymobile_library_path', t('The directory') . ' <i>' . $form_state['values']['jquerymobile_library_path'] . '</i> ' . t('does not exist.'));
  }
}

function jquerymobile_admin_settings_submit($form, &$form_state, $theme = NULL) {
  $themes = $form_state['values']['jquerymobile_mobile_themes'];
  foreach ($themes as $theme => $name) {
    if ($theme !== 0) {
      $stored = _jquerymobile_get_settings($theme);
      if (is_array($stored)) {
        $settings = array_merge($stored, array('status' => $name ? 1 : 0));
      }
      else {
        $settings = array('theme' => $theme, 'status' => $name ? 1 : 0);
      }
      _jquerymobile_set_settings($settings);
    }
  } 
  menu_rebuild();
}


function jquerymobile_admin_local_settings_submit($form, &$form_state) {
  form_state_values_clean($form_state);
  
  $values = $form_state['values'];
  $settings = array();
  
  foreach ($values as $value => $data) {
    $settings[$value] = $data;
  }

  _jquerymobile_set_settings($settings);
}
