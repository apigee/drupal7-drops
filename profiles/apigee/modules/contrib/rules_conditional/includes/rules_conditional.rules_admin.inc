<?php
/**
 * @file
 * Extensions to Rules UI.
 */

// Make Rules form functions available.
module_load_include('inc', 'rules', 'ui/ui.forms');

/**
 * Provides menu callbacks to extend Rules UI for conditionals.
 */
function _rules_conditional_rules_ui_menu_alter(&$items, $base_path, $base_count) {
  // Use existing "add" menu item as extension base.
  $add_item = $items[$base_path . '/manage/%rules_config/add/%rules_element'];

  // Extend "add".
  $items[$base_path . '/manage/%rules_config/add-predicate/%rules_element'] = array(
    'page arguments' => array('rules_conditional_add_predicate') + $add_item['page arguments'],
    'file' => 'includes/rules_conditional.rules_admin.inc',
    'file path' => drupal_get_path('module', 'rules_conditional'),
  ) + $add_item;

  // Add temporary fix for http://drupal.org/node/1666890.
  // TODO Remove when http://drupal.org/node/1676998 is fixed.
  $items[$base_path . '/manage/%rules_config/add-case/%rules_element'] = array(
    'page arguments' => array('rules_conditional_add_case') + $add_item['page arguments'],
    'file' => 'includes/rules_conditional.rules_admin.inc',
    'file path' => drupal_get_path('module', 'rules_conditional'),
  ) + $add_item;
}

/**
 * Provides a variant of rules_ui_add_element() to create a condition as the
 * predicate of a plugin.
 */
function rules_conditional_add_predicate($form, &$form_state, $rules_config, $plugin_name, RulesContainerPlugin $parent, $base_path) {
  $cache = rules_get_cache();
  if (!isset($cache['plugin_info'][$plugin_name]['class'])) {
    drupal_not_found();
    exit;
  }

  // Build condition form.
  RulesPluginUI::$basePath = $base_path;
  if (!isset($form_state['rules_element'])) {
    RulesPluginUI::formDefaults($form, $form_state);
    $form_state += array('parent_element' => $parent, 'plugin' => $plugin_name);

    $form['element_name'] = array(
      '#type' => 'select',
      '#title' => t('Select the condition to add for %element', array('%element' => $plugin_name)),
      '#options' => RulesPluginUI::getOptions('condition'),
      '#ajax' => rules_ui_form_default_ajax() + array(
        'trigger_as' => array('name' => 'continue'),
      ),
    );
    $form['continue'] = array(
      '#type' => 'submit',
      '#name' => 'continue',
      '#value' => t('Continue'),
      '#ajax' => rules_ui_form_default_ajax(),
    );
  }

  // Build element form.
  if (isset($form_state['rules_element'])) {
    $form_state['rules_element']->form($form, $form_state, array('button' => TRUE, 'init' => TRUE));
    $form['#validate'][] = 'rules_ui_edit_element_validate';
    $form['#submit'][] = 'rules_ui_edit_element_submit';
  }
  else {
    $form['#submit'][] = 'rules_ui_add_element_submit';
  }
  return $form;
}

/**
 * Provides a variant of rules_ui_add_element() to create a case.
 * @todo Remove when http://drupal.org/node/1676998 is fixed.
 */
function rules_conditional_add_case($form, &$form_state, $rules_config, $plugin_name, RulesContainerPlugin $parent, $base_path) {
  // Build condition form.
  RulesPluginUI::$basePath = $base_path;
  if (!isset($form_state['rules_element'])) {
    RulesPluginUI::formDefaults($form, $form_state);
    $form_state += array('parent_element' => $parent, 'plugin' => $plugin_name);

    $element = rules_plugin_factory($plugin_name);
    // Always add the new element at the bottom, thus set an appropriate weight.
    $iterator = $parent->getIterator();
    if ($sibling = end($iterator)) {
      $element->weight = $sibling->weight + 1;
    }
    $element->setParent($parent);
    $form_state['rules_element'] = $element;
  }

  // Build element form.
  $form_state['rules_element']->form($form, $form_state, array('button' => TRUE, 'init' => TRUE));
  $form['#validate'][] = 'rules_ui_edit_element_validate';
  $form['#submit'][] = 'rules_ui_edit_element_submit';
  return $form;
}
