<?php
/**
 * @file
 */

function jquerymobile_form_menu_edit_item_alter(&$form, $form_state) {
  global $theme_key;

  if (_jquerymobile_is_mobile_theme($theme_key)) {
    $item = $form['original_item']['#value'];
    jquerymobile_menu_item_alter($form, $item);
  }
}

function jquerymobile_form_node_form_alter(&$form, $form_state) {
  global $theme_key;

  if (_jquerymobile_is_mobile_theme($theme_key)) {
    if (isset($form['menu']['enabled'])) {
      $item = $form['#node']->menu;
      jquerymobile_menu_item_alter($form['menu']['link'], $item);
    }
  }
}


function jquerymobile_menu_item_alter(array &$form, array $item = array()) {
  if (!module_exists('menu_attributes')) {
    $form['options'] = array(
      '#tree' => TRUE,
      '#weight' => 55,
    );
    $form['options']['attributes'] = array(
      '#type' => 'fieldset', 
      '#title' => t('Anchor Tag attributes'),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
      '#tree' => TRUE,
      '#weight' => 60,
    );
  }
  $form['options']['attributes']['data-rel'] = array(
    '#type' => 'select', 
    '#title' => t('data-rel attribute'),
    '#default_value' => isset($item['options']['attributes']['data-rel']) ? $item['options']['attributes']['data-rel'] : '', 
    '#required' => FALSE,
    '#options' => array(
      '' => t('default'),
      'dialog' => t('dialog'),
    ),
  );
  $form['options']['attributes']['data-ajax'] = array(
    '#type' => 'select', 
    '#title' => t('data-ajax attribute'),
    '#default_value' => isset($item['options']['attributes']['data-ajax']) ? $item['options']['attributes']['data-ajax'] : '', 
    '#required' => FALSE,
    '#options' => array(
      '' => t('default'),
      'true' => t('true'),
      'false' => t('false'),
    ),
  );       
  $form['options']['attributes']['data-transition'] = array(
    '#type' => 'select', 
    '#title' => t('data-transition attribute'),
    '#default_value' => isset($item['options']['attributes']['data-transition']) ? $item['options']['attributes']['data-transition'] : '', 
    '#required' => FALSE,
    '#options' => array(
      '' => t('default'),
      'pop' => t('pop'),
      'flip' => t('flip'),
      'slidedown' => t('slidedown'),
    ),
  );
  
  $form['options']['jquerymobile_attributes'] = array(
    '#type' => 'fieldset', 
    '#title' => t('List Item attributes'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#tree' => TRUE,
    '#weight' => 70,
    '#access' => TRUE,
  );    
  $form['options']['jquerymobile_attributes']['data-role'] = array(
    '#type' => 'select', 
    '#title' => t('data-role attribute'),
    '#default_value' => isset($item['options']['jquerymobile_attributes']['data-role']) ? $item['options']['jquerymobile_attributes']['data-role'] : '', 
    '#required' => FALSE,
    '#options' => array(
      '' => t('default'), 
      'list-divider' => t('List Divider')
    ),
  );
  $form['options']['jquerymobile_attributes']['data-theme'] = array(
    '#type' => 'select', 
    '#title' => t('data-theme attribute'),
    '#default_value' => isset($item['options']['jquerymobile_attributes']['data-theme']) ? $item['options']['jquerymobile_attributes']['data-theme'] : '', 
    '#required' => FALSE,
    '#options' => array(
      '' => t('default'),
      'a' => t('Black'),
      'b' => t('Blue'),
      'c' => t('White'),
      'd' => t('Grey'),
      'e' => t('Yellow')
    ),
  );
  $form['options']['jquerymobile_attributes']['data-icon'] = array(
    '#type'          => 'select', 
    '#title'         => t('data-icon attribute'),
    '#default_value' => isset($item['options']['jquerymobile_attributes']['data-icon']) ? $item['options']['jquerymobile_attributes']['data-icon'] : '', 
    '#options'       => array(
      '' => t('default'),
      'arrow-l' => t('Left arrow'),
      'arrow-r' => t('Right arrow'),
      'arrow-u' => t('Up arrow'),
      'arrow-d' => t('Down arrow'),
      'delete' => t('Delete'),
      'plus' => t('Plus'),
      'minus' => t('Minus'),
      'check' => t('Check'),
      'gear' => t('Gear'),
      'refresh' => t('Refresh'),
      'forward' => t('Forward'),
      'back' => t('Back'),
      'grid' => t('Grid'),
      'star' => t('Star'),
      'alert' => t('Alert'),
      'info' => t('Info'),
      'home' => t('Home'),
      'search' => t('Search'),
    ),
  );
}

/**
 * Implements hook_menu_link_alter().
 */
function jquerymobile_menu_link_alter(&$item, $menu) {
  global $theme_key;

  if (_jquerymobile_is_mobile_theme($theme_key)) {
    if (isset($item['options']['attributes']) && is_array($item['options']['attributes'])) {
      // Filter out blank attributes.
      foreach ($item['options']['attributes'] as $key => $value) {
        if (empty($value)) {
          unset($item['options']['attributes'][$key]);
        }
      }
    }
  
    if (isset($item['options']['jquerymobile_attributes']) && is_array($item['options']['jquerymobile_attributes'])) {
      // Filter out blank attributes.
      foreach ($item['options']['jquerymobile_attributes'] as $key => $value) {
        if (empty($value)) {
          unset($item['options']['jquerymobile_attributes'][$key]);
        }
      }
    }
  }
}

function jquerymobile_get_link_attributes($item) {
  global $theme_key;

  if (_jquerymobile_is_mobile_theme($theme_key)) {
    if (isset($item['options']['jquerymobile_attributes'])) {
      return $item['options']['jquerymobile_attributes'];
    }
    else{
      return array();
    }
  }
  else {
    return array();
  }
}








