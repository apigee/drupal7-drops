<?php
/**
 * @file
 * Theme and pre/process functions.
 *
 * This includes the base template pre/process functions used by the WorldPay
 * payment response templates.
 * 
 * NOTE: You can test these themes by using the debugging URL:
 * http://[drupal_base]/commerce_worldpay/bg/response/debug_me?cartId=[valid_cart_id]
 */

/**
 * @todo add description
 *
 * @param array $variables
 */
function _template_common_variables_worldpay_bg(&$variables) {
  if (empty($variables['settings']['site_id'])) {
    $variables['site_id'] = '';
  }
  else {
    $variables['site_id'] = $variables['settings']['site_id'];
  }
  $variables['site_name'] = variable_get('site_name', 'Drupal');
}

/**
 * Template preprocess for the overal page (decleration) template.
 */
function template_preprocess_commerce_worldpay_bg_html(&$variables) {
  // Pinched from template_preprocess_html()
  // RDFa allows annotation of XHTML pages with RDF data, while GRDDL provides
  // mechanisms for extraction of this RDF content via XSLT transformation
  // using an associated GRDDL profile.
  $variables['rdf_namespaces']    = drupal_get_rdf_namespaces();
  $variables['grddl_profile']     = 'http://www.w3.org/1999/xhtml/vocab';
  $variables['language']          = $GLOBALS['language'];
  $variables['language']->dir     = $GLOBALS['language']->direction ? 'rtl' : 'ltr';

  _template_common_variables_worldpay_bg($variables);

  if (drupal_get_title()) {
    $variables['head_title'] = strip_tags(drupal_get_title());
  }
  else {
    $variables['head_title'] = check_plain(variable_get('site_name', 'Drupal Commerce')) . ' WorldPay payment';
  }

  $variables['title'] = variable_get('site_name', 'Drupal Commerce') . ' WorldPay payment';
  $variables['body_attributes_array'] = array();
  if (!empty($variables['settings']['site_id'])) {
    $variables['body_attributes_array']['class'] = 'css-' . drupal_clean_css_identifier($variables['settings']['site_id']);
  }
}

/**
 * Template process for the overal page template.
 */
function template_process_commerce_worldpay_bg_html(&$variables) {
  $variables['body_attributes'] = drupal_attributes($variables['body_attributes_array']);
}

/**
 * Template preprocess for the main body of the page.
 */
function template_preprocess_commerce_worldpay_bg_page(&$variables) {
  $variables['title'] = variable_get('site_name', 'Drupal Commerce') . ' WorldPay payment';
  _template_common_variables_worldpay_bg($variables);
}

/**
 * Template preprocess for the success page.
 */
function template_preprocess_commerce_worldpay_bg_success(&$variables) {
  $order = $variables['order'];
  $variables['title'] = t('Payment has been completed');
  _template_common_variables_worldpay_bg($variables);

  $url_options = array();
  if ($variables['settings']['payment_urls']['force_non_ssl_links']) {
    $url_options['https'] = FALSE;
  }

  $variables['return_url'] = url(
    'checkout/' . $order->order_id . '/payment/return/' . $order->data['payment_redirect_key'],
    array('absolute' => TRUE) + $url_options);
}

/**
 * Template preprocess for the cancel page.
 */
function template_preprocess_commerce_worldpay_bg_cancel(&$variables) {
  $order = $variables['order'];
  $variables['title'] = t('The transaction has been canceled');
  _template_common_variables_worldpay_bg($variables);

  $url_options = array();
  if ($variables['settings']['payment_urls']['force_non_ssl_links']) {
    $url_options['https'] = FALSE;
  }
  // Return to the previous page when payment is canceled.
  $variables['return_url'] = url(
    'checkout/' . $order->order_id . '/payment/back/' . $order->data['payment_redirect_key'],
    array('absolute' => TRUE) + $url_options);
}
