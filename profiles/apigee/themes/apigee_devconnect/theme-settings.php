<?php
function apigee_devconnect_form_system_theme_settings_alter(&$form, $form_state) {
  $form['welcome_message'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Welcome Message'),
    '#default_value' => theme_get_setting('welcome_message'),
    '#description'   => t("Use this field to change the default welcome message"),
    '#weight' => -11,
  );
  $form['devconnect_branding'] = array(
    '#type' => 'fieldset',
    '#weight' => -10,
    '#title' => t('Site Branding'),
  );
  $form['devconnect_branding']['header_bg_color'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Header Background Color'),
    '#default_value' => theme_get_setting('header_bg_color'),
    '#description'   => t("Use this field to change the header background color"),
  );
  $form['devconnect_branding']['header_txt_color'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Header Text Color'),
    '#default_value' => theme_get_setting('header_txt_color'),
    '#description'   => t("Use this field to change the header text color"),
  );
  $form['devconnect_branding']['header_hover_bg_color'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Header Hover Background Color'),
    '#default_value' => theme_get_setting('header_hover_bg_color'),
    '#description'   => t("Use this field to change the header background color on hover"),
  );
  $form['devconnect_branding']['header_hover_txt_color'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Header Hover Text Color'),
    '#default_value' => theme_get_setting('header_hover_txt_color'),
    '#description'   => t("Use this field to change the header text color on hover"),
  );
  $form['devconnect_branding']['link_color'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Link Color'),
    '#default_value' => theme_get_setting('link_color'),
    '#description'   => t("Use this field to change the color of links"),
  );
  $form['devconnect_branding']['link_hover_color'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Link Hover Color'),
    '#default_value' => theme_get_setting('link_hover_color'),
    '#description'   => t("Use this field to change the color of links on hover"),
  );
  $form['devconnect_branding']['footer_bg_color'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Footer Background Color'),
    '#default_value' => theme_get_setting('footer_bg_color'),
    '#description'   => t("Use this field to change the footer background color"),
  );
  $form['devconnect_branding']['footer_link_color'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Footer Link Color'),
    '#default_value' => theme_get_setting('footer_link_color'),
    '#description'   => t("Use this field to change the color of footer links"),
  );
  $form['devconnect_branding']['footer_link_hover_color'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Footer Link Hover Color'),
    '#default_value' => theme_get_setting('footer_link_hover_color'),
    '#description'   => t("Use this field to change the color of footer links on hover"),
  );
  $form['devconnect_branding']['button_background_color'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Button Background Color'),
    '#default_value' => theme_get_setting('button_background_color'),
    '#description'   => t("Use this field to change the button background color"),
  );
  $form['devconnect_branding']['button_text_color'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Button Text Color'),
    '#default_value' => theme_get_setting('button_text_color'),
    '#description'   => t("Use this field to change the button text color"),
  );
  $form['devconnect_branding']['devconnect_branding']['button_hover_background_color'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Button Hover Background Color'),
    '#default_value' => theme_get_setting('button_hover_background_color'),
    '#description'   => t("Use this field to change the button background color on hover"),
  );
  $form['devconnect_branding']['devconnect_branding']['button_hover_text_color'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Button Hover Text Color'),
    '#default_value' => theme_get_setting('button_hover_text_color'),
    '#description'   => t("Use this field to change the button text color on hover"),
  );
  $form['responsive_design'] = array(
    '#type' => 'fieldset',
    '#title' => t('Responsive Layouts'),
    '#description'   => t("Enable/Disable responsive layouts"),
  );
  $form['responsive_design']['wide_layout'] = array(
    '#type'          => 'checkbox',
    '#title'         => t('Wide Layout'),
    '#default_value' => theme_get_setting('wide_layout'),
  );
  $form['logo_variant'] = array(
    '#type' => 'fieldset',
    '#title' => t('Logo Settings'),
    '#description'   => t("Define the logo size for the header of the site"),
  );
  $form_state['values']['logo_size'] = theme_get_setting('logo_size');
  $size = apigee_devconnect_logo_size_example($form, $form_state);
  $form['logo_variant']['logo_size'] = array(
    '#type' => 'select',
    '#default_value' => theme_get_setting('logo_size'),
    '#options' => array(
      'standard' => t('71px x 26px (standard)'),
      'big' => '272px x 100px (big)',
      'bigger' => '372px x 136px (bigger)',
    ),
    '#submit' => array('apigee_devconnect_logo_size_submit'),
    '#ajax' => array(
      'wrapper' => 'logo-size-example',
      'callback' => 'apigee_devconnect_logo_size_example',
      'effect' => 'fade',
      'progress' => 'none',
    ),
    '#suffix' => $size,
  );
  
  //settings to diable the deprecation message.
  $form['disable_deprecation_message'] = array(
    '#type' => 'checkbox',
    '#title' => t("Disable deprecation message"),
    '#default_value' => theme_get_setting('disable_deprecation_message'),
    '#description' => t("Select this option to disable the deprecation message that is shown to the administrative users."),
  );
}

/**
 * Callback function for logo size
 *
 * Depends on the placehold.it service.
 *
 */
function apigee_devconnect_logo_size_example($form, $form_state) {
  $html = '<div id="logo-size-example" style="float:left;clear:both;">';
  switch($form_state['values']['logo_size']) {
    case 'standard':
      $html .= '<img src="http://placehold.it/71x26">';
      break;
    case 'big':
      $html .= '<img src="http://placehold.it/272x100">';
      break;
    case 'bigger':
      $html .= '<img src="http://placehold.it/372x136">';
      break;
    default:
      break;
  }
  $html .= '</div>';
  return $html;
}
?>
