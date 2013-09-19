<?php

function xhprof_admin_settings() {
  $description = extension_loaded('xhprof') ? t('Profile requests with the xhprof php extension.') : '<span class="warning">' . t('You must enable the <a href="!url">xhprof php extension</a> to use this feature.', array('!url' => url('http://techportal.ibuildings.com/2009/12/01/profiling-with-xhprof/'))) . '</span>';
  $form['xhprof_enabled'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable profiling of all page views and <a href="!drush">drush</a> requests.', array('!drush' => url('http://drush.ws'))),
    '#default_value' => variable_get('xhprof_enabled', FALSE),
    '#description' => $description,
    '#disabled' => !extension_loaded('xhprof'),
  );
  $form['xhprof_disable_admin_paths'] = array(
    '#type' => 'checkbox',
    '#title' => t('Disable profiling of admin pages'),
    '#default_value' => variable_get('xhprof_disable_admin_paths', TRUE),
  );
  $form['settings'] = array(
    '#type' => 'container',
    '#states' => array(
      'invisible' => array(
        'input[name="xhprof_enabled"]' => array('checked' => FALSE),
      ),
    ),
  );

  $options = drupal_map_assoc(xhprof_get_classes());
  $form['xhprof_default_class'] = array(
    '#type' => 'radios',
    '#title' => t('XHProf storage'),
    '#default_value' => variable_get('xhprof_default_class', 'XHProfRunsFile'),
    '#options' => $options,
    '#description' => t('Choose an XHProf runs class.'),
  );

  return system_settings_form($form);
}
