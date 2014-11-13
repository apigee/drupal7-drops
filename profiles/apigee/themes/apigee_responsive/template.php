<?php

/**
 * @file
 * template.php
 */

/**
 * Implements hook_preprocess_html().
 */
function apigee_responsive_preprocess_html(&$vars) {
  $header_bg_color                = theme_get_setting('header_bg_color');
  $header_txt_color               = theme_get_setting('header_txt_color');
  $header_hover_bg_color          = theme_get_setting('header_hover_bg_color');
  $header_hover_txt_color         = theme_get_setting('header_hover_txt_color');
  $link_color                     = theme_get_setting('link_color');
  $link_hover_color               = theme_get_setting('link_hover_color');
  $footer_bg_color                = theme_get_setting('footer_bg_color');
  $footer_link_color              = theme_get_setting('footer_link_color');
  $footer_link_hover_color        = theme_get_setting('footer_link_hover_color');
  $button_background_color        = theme_get_setting('button_background_color');
  $button_text_color              = theme_get_setting('button_text_color');
  $button_hover_background_color  = theme_get_setting('button_hover_background_color');
  $button_hover_text_color        = theme_get_setting('button_hover_text_color');

  //add additional class to the body to adjust the body padding according to the logo size.
  //this is to prevent the search box from going beneath the header.
  $vars['classes_array'][] = "logo_" . theme_get_setting('logo_size');
 
  $cdn = theme_get_setting('bootstrap_cdn');

  if (!(bool)$cdn || !isset($cdn) || empty($cdn)) {
    drupal_add_css(drupal_get_path('theme', 'bootstrap') . '/css/overrides.css', array('group' => CSS_SYSTEM));
    drupal_add_css(drupal_get_path('theme', 'apigee_responsive') . '/css/bootstrap.min.css', array('group' => CSS_SYSTEM));
    drupal_add_js(drupal_get_path('theme', 'apigee_responsive') . '/js/bootstrap.min.js', array('group' => CSS_SYSTEM));
  }
  if (module_exists('devconnect_monetization')) {
    require_once drupal_get_path('theme', 'apigee_responsive') . '/templates/monetization/template.php';
    drupal_add_css(drupal_get_path('theme', 'apigee_responsive') . '/templates/monetization/css/monetization.css');
  }

  drupal_add_css(".faq .collapsed {display:block !important}", array('group' => CSS_THEME, 'type' => 'inline'));
  drupal_add_css("body header.navbar {background-color: $header_bg_color}", array('group' => CSS_THEME, 'type' => 'inline'));
  drupal_add_css("body header.navbar {border-color: $header_bg_color}", array('group' => CSS_THEME, 'type' => 'inline'));
  drupal_add_css("body .navbar-inverse .navbar-toggle {border-color: $header_bg_color; background-color: $header_bg_color}", array('group' => CSS_THEME, 'type' => 'inline'));
  drupal_add_css("body .navbar-inverse .navbar-collapse:focus {border-color: $header_bg_color; background-color: $header_bg_color}", array('group' => CSS_THEME, 'type' => 'inline'));
  drupal_add_css("body .navbar-inverse .navbar-toggle:hover {border-color: $header_bg_color; background-color: $header_bg_color}", array('group' => CSS_THEME, 'type' => 'inline'));
  drupal_add_css("body .navbar-inverse .navbar-toggle:focus {border-color: $header_bg_color; background-color: $header_bg_color}", array('group' => CSS_THEME, 'type' => 'inline'));
  drupal_add_css("body .navbar-inverse .navbar-nav > .open > a {border-color: $header_bg_color; background-color: $header_bg_color}", array('group' => CSS_THEME, 'type' => 'inline'));
  drupal_add_css("body .navbar-inverse .navbar-nav > .open > a:hover {border-color: $header_bg_color; background-color: $header_hover_bg_color; color:$header_hover_txt_color}", array('group' => CSS_THEME, 'type' => 'inline'));
  drupal_add_css("body .navbar-inverse .navbar-nav > .open > a:focus {border-color: $header_bg_color; background-color: $header_bg_color}", array('group' => CSS_THEME, 'type' => 'inline'));
  drupal_add_css("body .navbar-default .navbar-toggle:hover {border-color: $header_bg_color; background-color: $header_bg_color}", array('group' => CSS_THEME, 'type' => 'inline'));
  drupal_add_css("body .navbar-default .navbar-toggle:active {border-color: $header_bg_color; background-color: $header_bg_color}", array('group' => CSS_THEME, 'type' => 'inline'));
  drupal_add_css("body .navbar-default .navbar-toggle:focus {border-color: $header_bg_color; background-color: $header_bg_color}", array('group' => CSS_THEME, 'type' => 'inline'));
  drupal_add_css("body .navbar-default .navbar-collapse {border-color: $header_bg_color; background-color: $header_bg_color}", array('group' => CSS_THEME, 'type' => 'inline'));
  drupal_add_css("body .navbar-default .navbar-collapse:hover {border-color: $header_bg_color; background-color: $header_bg_color}", array('group' => CSS_THEME, 'type' => 'inline'));
  drupal_add_css("body .navbar-default .navbar-collapse:focus {border-color: $header_bg_color; background-color: $header_bg_color}", array('group' => CSS_THEME, 'type' => 'inline'));
  drupal_add_css("body .navbar-default .navbar-nav > .open > a {border-color: $header_bg_color; background-color: $header_bg_color}", array('group' => CSS_THEME, 'type' => 'inline'));
  drupal_add_css("body .navbar-default .navbar-nav > .open > a:hover {border-color: $header_bg_color; background-color: $header_bg_color}", array('group' => CSS_THEME, 'type' => 'inline'));
  drupal_add_css("body .navbar-default .navbar-nav > .open > a:focus {border-color: $header_bg_color; background-color: $header_bg_color}", array('group' => CSS_THEME, 'type' => 'inline'));
  drupal_add_css("body header.navbar.navbar-nav > .open > a {border-color: $header_bg_color; background-color: $header_bg_color}", array('group' => CSS_THEME, 'type' => 'inline'));
  drupal_add_css("body header.navbar.navbar-nav > .open > a:hover {border-color: $header_bg_color; background-color: $header_bg_color}", array('group' => CSS_THEME, 'type' => 'inline'));
  drupal_add_css("body header.navbar.navbar-nav > .open > a:focus {border-color: $header_bg_color; background-color: $header_bg_color}", array('group' => CSS_THEME, 'type' => 'inline'));
  drupal_add_css("body header.navbar .nav > li > a {color: $header_txt_color}", array('group' => CSS_THEME, 'type' => 'inline'));
  drupal_add_css("body header.navbar .nav > li > a:hover {background-color: $header_hover_bg_color}", array('group' => CSS_THEME, 'type' => 'inline'));
  drupal_add_css(".navbar-inverse .navbar-collapse, .navbar-inverse .navbar-form, .navbar-inverse .navbar-toggle,
  .navbar-default .navbar-collapse, .navbar-default .navbar-form, .navbar-default .navbar-toggle, header.navbar.navbar-collapse,
  header.navbar.navbar-form, header.navbar.navbar-toggle {border-color: $header_hover_bg_color}", array('group' => CSS_THEME, 'type' => 'inline'));
  drupal_add_css(".navbar-inverse .navbar-toggle, .navbar-inverse .navbar-toggle:hover, .navbar-inverse .navbar-toggle:focus,
  .navbar-inverse .navbar-nav > .open > a, .navbar-inverse .navbar-nav > .open > a:hover, .navbar-inverse .navbar-nav > .open > a:focus,
  .navbar-default .navbar-toggle:hover, .navbar-default .navbar-toggle:active, .navbar-default .navbar-toggle:focus,
  .navbar-default .navbar-nav > .open > a, .navbar-default .navbar-nav > .open > a:hover, .navbar-default .navbar-nav > .open > a:focus,
  header.navbar.navbar-nav > .open > a, header.navbar.navbar-nav > .open > a:hover, header.navbar.navbar-nav > .open > a:focus {background-color: $header_hover_bg_color}", array('group' => CSS_THEME, 'type' => 'inline'));
  drupal_add_css("body header.navbar .nav .active > a, body .navbar .nav .active > a:hover, .navbar.navbar-fixed-top #main-menu li a:hover {background-color: $header_hover_bg_color}", array('group' => CSS_THEME, 'type' => 'inline'));
  drupal_add_css("body header.navbar .nav > li > a:hover {color: $header_hover_txt_color}", array('group' => CSS_THEME, 'type' => 'inline'));
  drupal_add_css("body a {color: $link_color}", array('group' => CSS_THEME, 'type' => 'inline'));
  drupal_add_css("body a:hover {color: $link_hover_color}", array('group' => CSS_THEME, 'type' => 'inline'));
  drupal_add_css("body .footer {background-color: $footer_bg_color}", array('group' => CSS_THEME, 'type' => 'inline'));
  drupal_add_css("body .footer .navbar ul.footer-links > li > a {color: $footer_link_color}", array('group' => CSS_THEME, 'type' => 'inline'));
  drupal_add_css("body .footer .navbar ul.footer-links > li > a:hover {color: $footer_link_hover_color}", array('group' => CSS_THEME, 'type' => 'inline'));
  drupal_add_css("body .btn {background-color: $button_background_color;}", array('group' => CSS_THEME, 'type' => 'inline'));
  drupal_add_css("body .btn {color: $button_text_color;}", array('group' => CSS_THEME, 'type' => 'inline'));
  drupal_add_css("body .btn:hover {background-color: $button_hover_background_color;}", array('group' => CSS_THEME, 'type' => 'inline'));
  drupal_add_css("body .btn:hover {color: $button_hover_text_color;}", array('group' => CSS_THEME, 'type' => 'inline'));
  drupal_add_css("body .btn:focus {background-color: $button_hover_background_color;}", array('group' => CSS_THEME, 'type' => 'inline'));
  drupal_add_css("body .btn:focus {color: $button_hover_text_color;}", array('group' => CSS_THEME, 'type' => 'inline'));
  drupal_add_css("body .btn:active {background-color: $button_hover_background_color;}", array('group' => CSS_THEME, 'type' => 'inline'));
  drupal_add_css("body .btn:active {color: $button_hover_text_color;}", array('group' => CSS_THEME, 'type' => 'inline'));
  drupal_add_css("body .btn.active {background-color: $button_hover_background_color;}", array('group' => CSS_THEME, 'type' => 'inline'));
  drupal_add_css("body .btn.active {color: $button_hover_text_color;}", array('group' => CSS_THEME, 'type' => 'inline'));
  drupal_add_css("body .open .dropdown-toggle.btn {background-color: $button_hover_background_color;}", array('group' => CSS_THEME, 'type' => 'inline'));
  drupal_add_css("body .open .dropdown-toggle.btn {color: $button_hover_text_color;}", array('group' => CSS_THEME, 'type' => 'inline'));

  // Main menu expanded drop down colors.
  drupal_add_css(".navbar-nav > li > span {color: $header_txt_color}", array('group' => CSS_THEME, 'type' => 'inline'));
  drupal_add_css(".navbar-nav > li > span:hover {color: $header_hover_txt_color}", array('group' => CSS_THEME, 'type' => 'inline'));
  drupal_add_css(".navbar-nav > li.expanded span:hover {background-color: $header_hover_bg_color}", array('group' => CSS_THEME, 'type' => 'inline'));
  drupal_add_css(".navbar-nav > li.expanded.active {background-color: $header_hover_bg_color}", array('group' => CSS_THEME, 'type' => 'inline'));
  drupal_add_css(".navbar-nav > li > span > span.caret {border-bottom-color: $header_txt_color; border-top-color: $header_txt_color; color: $header_txt_color;}", array('group' => CSS_THEME, 'type' => 'inline'));


  switch (theme_get_setting('logo_size')) {
    case 'big':
      drupal_add_css("body #navbar.navbar { height:60px; }", array(
          'group' => CSS_THEME,
          'type' => 'inline')
      );
      drupal_add_css("@media(max-width:767px) { body #navbar.navbar { height:inherit; } }", array(
          'group' => CSS_THEME,
          'type' => 'inline')
      );
      drupal_add_css(".navbar-nav > li > a { line-height:30px; }", array(
          'group' => CSS_THEME,
          'type' => 'inline')
      );
      drupal_add_css("@media(min-width:992px) { .logo > img { width:100%; } }", array(
        'group' => CSS_THEME,
        'type' => 'inline'
      ));
      break;
    case 'bigger':
      drupal_add_css("body #navbar.navbar { height:70px; }", array(
          'group' => CSS_THEME,
          'type' => 'inline')
      );
      drupal_add_css("@media(max-width:767px) { body #navbar.navbar { height:inherit; } }", array(
          'group' => CSS_THEME,
          'type' => 'inline')
      );
      drupal_add_css(".navbar-nav > li > a { line-height:40px; }", array(
          'group' => CSS_THEME,
          'type' => 'inline')
      );
      drupal_add_css("@media(min-width:992px) { .logo > img { width:100%; } }", array(
        'group' => CSS_THEME,
        'type' => 'inline'
      ));
      break;

    default:
      break;
  }

  switch (theme_get_setting('footer_position')) {
    case 'fixed':
      drupal_add_css("body #push { height: 100px; }", array('group' => CSS_THEME, 'type' => 'inline'));
      if (path_is_admin(current_path())) {
        drupal_add_css("#module-filter-tabs.bottom-fixed { bottom: 100px; }", array('group' => CSS_THEME, 'type' => 'inline'));
        drupal_add_css("html.js #module-filter-submit { padding: 10px; }", array('group' => CSS_THEME, 'type' => 'inline'));
      }
      break;
    case 'static':
      drupal_add_css(".footer.footer-fixed-bottom { position:static }", array('group' => CSS_THEME, 'type' => 'inline'));
      break;
    default;

  }

}

/**
 * Implements hook_preprocess_page().
 */
function apigee_responsive_preprocess_page(&$vars) {
  global $user;
  if (module_exists('apachesolr')) {
    $search = drupal_get_form('search_form');
    $search['basic']['keys']['#size'] = 20;
    $search['basic']['keys']['#title'] = '';
    unset($search['#attributes']);
    $search_form = drupal_render($search);
    $find = array('type="submit"', 'type="text"');
    $replace = array('type="hidden"', 'type="search" placeholder="search" autocapitalize="off" autocorrect="off"');
    $vars['search_form'] = str_replace($find, $replace, $search_form);
  }

  if (isset($user->mail)) {
    $vars['user_mail_clipped'] = substr($user->mail, 0, 16) . '...';
  }
  else {
    $vars['user_mail_clipped'] = '';
  }
  $vars['current_path'] = implode("/", arg());

  $user_url = 'user/' . $user->uid;
  $vars['myappslink'] = l('<span class="glyphicon glyphicon-pencil"></span>&nbsp;&nbsp; ' . t('My ' . _devconnect_developer_apps_get_app_label(TRUE)), $user_url . '/apps', array('html' => TRUE));
  $vars['profilelink'] = l('<span class="glyphicon glyphicon-user"></span>&nbsp;&nbsp; Edit Profile', $user_url . '/edit', array('html' => TRUE));
  $vars['logoutlink'] = l('<span class="glyphicon glyphicon-off"></span>&nbsp;&nbsp; Logout','user/logout',array('html' => TRUE));

  // Custom Search.
  $vars['search'] = FALSE;
  if (theme_get_setting('toggle_search') && module_exists('search')) {
    $vars['search'] = drupal_get_form('search_form');
  }

  $second_arg = module_exists('me') ? 'me' : $GLOBALS['user']->uid;

  $dropdown_links = array(
    array(
      'classes' => array('glyphicon', 'glyphicon-user'),
      'text' => 'Edit Profile',
      'url' => 'user/' . $second_arg . '/edit'
    ),
    array(
      'classes' => array('glyphicon', 'glyphicon-off'),
      'text' => 'Logout',
      'url' => 'user/logout'
    )
  );

  drupal_alter('apigee_responsive_links', $dropdown_links);

  $ddl = '';
  foreach ($dropdown_links as $dropdown_link) {
    $classes = join(' ', $dropdown_link['classes']);
    $text = t($dropdown_link['text']);
    $ddl .= '<li>' . l('<span class="' . $classes . '"></span>&nbsp;&nbsp; ' . check_plain($text), $dropdown_link['url'], array('html' => TRUE)) . '</li>';
  }

  $vars['dropdown_links'] = $ddl;
}

/**
 * Implements hook_preprocess_region().
 */
function apigee_responsive_preprocess_region(&$vars) {
  if (module_exists('devconnect_default_content')) {
    if (drupal_is_front_page() && isset($vars['region']) && $vars['region'] == 'content') {
      $vars['classes_array'][] = 'row';
    }
  }
}

/**
 * Implements hook_preprocess_block().
 */
function apigee_responsive_preprocess_block(&$vars) {
  if (drupal_is_front_page() && $vars['block']->module == 'views') {
    $vars['classes_array'][] = 'col-md-4';
  }
  if (drupal_is_front_page() && $vars['block_html_id'] == 'block-system-main') {
    $vars['classes_array'][] = 'row';
  }
}

/**
 * Implements hook_preprocess_hook().
 */
function apigee_responsive_preprocess_devconnect_developer_apps_list(&$vars) {
  $user = (isset($vars['user']) ? $vars['user'] : $GLOBALS['user']);
  // Set Title.
  if ($user->uid == $GLOBALS['user']->uid) {
    $title = t('My ' . _devconnect_developer_apps_get_app_label(TRUE));
  }
  else {
    $title = t('@name’s ' . _devconnect_developer_apps_get_app_label(TRUE), array('@name' => $user->name));
  }
  drupal_set_title($title);

  // Build Breadcrumbs.
  $breadcrumb = array();
  $breadcrumb[] = l(t('Home'), '<front>');
  // Set Breadcrumbs.
  drupal_set_breadcrumb($breadcrumb);

  $vars['show_status'] = variable_get('devconnect_show_apiproduct_status', FALSE);
  
  if (user_access("create developer apps")) {
    $link_text = '<span class="glyphicon glyphicon-plus"></span> ' . t('Add a new ' . _devconnect_developer_apps_get_app_label(FALSE));
    $vars['add_app'] = l($link_text, 'user/' . $user->uid . '/apps/add', array('html' => TRUE, 'attributes' => array('class' => array('add-app'))));
  }

  $vars['i'] = 0;

  foreach ($vars['applications'] as $key => $detail) {
    $vars['applications'][$key]['created'] = ($detail['entity']->createdAt) ? floor($detail['entity']->createdAt) / 1000 : $detail['entity']->attributes['create_date'];
    if ((time() - 86400) < $vars['applications'][$key]['created']) {
      $vars['applications'][$key]['new_status'] = 1;
    }
    else {
      $vars['applications'][$key]['new_status'] = 0;
    }
    $vars['applications'][$key]['edit_url_id'] = preg_replace('/[^A-Za-z0-9\-]/', '', $detail['edit_url']);
    $vars['applications'][$key]['delete_url_id'] = preg_replace('/[^A-Za-z0-9\-]/', '', $detail['delete_url']);
    if (empty($vars['applications'][$key]['credential']['apiProducts'])) {
      $vars['applications'][$key]['noproducts'] = TRUE;
    } else {
      $vars['applications'][$key]['noproducts'] = FALSE;
    }
  }

  $vars['show_analytics'] = FALSE;
  if (variable_get('devconnect_show_analytics', FALSE)) {
    $vars['show_analytics'] = TRUE;
  }
}

/**
 * Implements hook_preprocess_hook().
 */
function apigee_responsive_preprocess_bootstrap_modal_forms(&$vars) {
  switch($vars['identifier']){
    case 'login':
      if (module_exists('openid')) {
        $vars['modal_form']['openid_identifier']['#prefix'] = '<div class="apigee-responsive-openidhide" style="display:none">';
        $vars['modal_form']['openid_identifier']['#suffix'] = '</div>';
      }
      $vars['sso'] = $vars['modal_form']['sso_buttons'];
      unset($vars['modal_form']['sso_buttons']);
      break;
    case 'register':
      $vars['sso'] = '';
      if (isset($vars['modal_form']['sign_in_with_google_apps'])) {
        $vars['sso'] = $vars['modal_form']['sign_in_with_google_apps']['#markup'];
        unset($vars['modal_form']['sign_in_with_google_apps']);
      }
      break;
  }
}

/**
 * Implements hook_preprocess_node().
 */
function apigee_responsive_preprocess_node(&$vars) {
  $vars['submitted_month'] = date('M' ,$vars['created']);
  $vars['submitted_day'] = date('d' , $vars['created']);
}

/**
 * Implements hook_form_alter
 */
function apigee_responsive_form_alter(&$form, &$form_state, $form_id) {
  switch ($form_id) {
    case 'user_login':
      if (module_exists('github_connect')) {
        if (array_key_exists('github_links', $form)) {
          unset($form['github_links']);
        }
      }
      if (isset($form['userpasswordlink'])) {
        $form['userpasswordlink']['#prefix'] = '<br>';
      }
      break;
    case 'devconnect_developer_app_list':
      if (module_exists('devconnect_monetization')) {
        $settings = array(
          'verify_accepted_url' => MONETIZATION_PRODUCT_VERIFY_ACCEPTED_URL,
        );
        drupal_add_js(array('devconnect_monetization_developer_apps_form' => $settings), 'setting');
        drupal_add_js(drupal_get_path('module', 'devconnect_monetization') . '/js/api-products.js', 'file');
        drupal_add_js('jQuery( document ).ajaxComplete(function() { Drupal.attachBehaviors(jQuery("body")); });', 'inline');
      }
    default:
      break;
  }
}

/**
 * Implements theme_developer_app_tabs().
 */
function apigee_responsive_developer_app_tabs(&$vars, $apps = NULL) {
  // the app details page will only be used for analytics for now.
  return '';
}

/**
 * Implements hook_preprocess_user_profile().
 *
 * @param $vars
 */
function apigee_responsive_preprocess_user_profile(&$vars) {
  $vars['user_profile']['account'] = $vars['elements']['#account'];
}

/**
 * Implements hook_preprocess().
 */
function apigee_responsive_preprocess(&$vars, $hook) {
  if (module_exists('advanced_forum')) {

  }
  switch($hook) {
    case 'comment_wrapper':
      $vars['content']['comment_form']['#attributes']['class'] = 'well';
      break;
  }
}

/**
 * Implements theme_advanced_forum_l().
 */
function apigee_responsive_advanced_forum_l(&$vars) {
  $text = $vars['text'];
  $path = empty($vars['path']) ? NULL : $vars['path'];
  $options = empty($vars['options']) ? array() : $vars['options'];
  $button_class = empty($vars['button_class']) ? NULL : $vars['button_class'];

  $l = '';
  if (!isset($options['attributes'])) {
    $options['attributes'] = array();
  }
  if (!is_null($button_class)) {
    // Buttonized link: add our button class and the span.
    if (!isset($options['attributes']['class'])) {
      $options['attributes']['class'] = array("af-button-$button_class", 'btn', 'btn-primary');
    }
    else {
      $options['attributes']['class'][] = "af-button-$button_class";
      $options['attributes']['class'][] = "btn-primary";
      $options['attributes']['class'][] = "btn";
    }
    $options['html'] = TRUE;
    $l = l('<span>' . $text . '</span>', $path, $options);
  }
  else {
    // Standard link: just send it through l().
    $l = l($text, $path, $options);
  }

  return $l;
}

/**
 * Implements theme_developer_app_panes().
 */
function apigee_responsive_developer_app_panes(&$vars) {
  $output = '<div class="tab-content app-details">';
  foreach ($vars['panes'] as $pane) {
    $id = $pane['#id'];
    switch($id) {
      case 'performance':
        $output .= '<div class="tab-pane';
        $output .= '" id="' . $id . '">';
        $output .= drupal_render($pane);
        break;
    }
    $output .= '</div>';
  }
  $output .= '</div>';
  return $output;
}

/**
 * Overrides theme_menu_link().
 */
function apigee_responsive_menu_link($vars) {
  $element = $vars['element'];
  $sub_menu = '';
  if ($element['#below']) {
    if (!(($element['#original_link']['menu_name'] == 'management') && (module_exists('navbar')))) {
      $below = '';
      if ($element['#below']) {
        $element['#title'] .= ' <span class="caret"></span>';
        $element['#attributes']['class'][] = 'dropdown';
        $element['#localized_options']['html'] = TRUE;
        $element['#localized_options']['attributes']['data-target'] = '#';
        $element['#localized_options']['attributes']['class'][] = 'dropdown-toggle';
        $element['#localized_options']['attributes']['data-toggle'] = 'dropdown';
        $below = _apigee_responsive_get_below($element['#below']);
      }
      $output = l($element['#title'], $element['#href'], $element['#localized_options']) . $below;
    } else {
      $sub_menu = drupal_render($element['#below']);
      if (($element['#href'] == $_GET['q'] || ($element['#href'] == '<front>' && drupal_is_front_page())) && (empty($element['#localized_options']['language']))) {
        $element['#attributes']['class'][] = 'active';
      }
      $output = l($element['#title'], $element['#href'], $element['#localized_options']);
    }
  } else {
    if (($element['#href'] == $_GET['q'] || ($element['#href'] == '<front>' && drupal_is_front_page())) && (empty($element['#localized_options']['language']))) {
      $element['#attributes']['class'][] = 'active';
    }
    $output = l($element['#title'], $element['#href'], $element['#localized_options']);
  }
  return '<li' . drupal_attributes($element['#attributes']) . '>' . $output . $sub_menu . "</li>\n";
}

function _apigee_responsive_get_below($element) {
  $output = '';
  foreach ($element as $mlid => $item) {
    if (is_numeric($mlid)) {
      if ($item['#below']) {
        $below = '';
        $item['#attributes']['class'][] = 'dropdown-submenu';
        $item['#localized_options']['html'] = TRUE;
        $link = l($item['#title'], $item['#href'], $item['#localized_options']);
        $output .= '<li' . drupal_attributes($item['#attributes']) . '>' . $link;
        if ($item['#below']) {
          $below = _apigee_responsive_get_below($item['#below']);
        }
        $output .= $below . "</li>\n";
      } else {
        $link = l($item['#title'], $item['#href'], $item['#localized_options']);
        $output .= '<li' . drupal_attributes($item['#attributes']) . '>' . $link . "</li>\n";
      }
    }
  }
  return '<ul class="dropdown-menu">' . $output . '</ul>';
}

/**
 * Bootstrap theme wrapper function for the primary menu links.
 */
function apigee_responsive_menu_tree__primary(&$vars) {
  $html = '<ul class="menu nav navbar-nav full-menu primary-nav hidden-xs">' . $vars['tree'] . '</ul>';
  $html .= '<ul class="menu nav navbar-nav mobile-menu primary-nav visible-xs">' . $vars['tree'] . '</ul>';
  return $html;
}

/**
 * Menu Local Tasks
 */
function apigee_responsive_menu_local_tasks(&$vars) {
  $output = '';

  if (!empty($vars['primary'])) {
    $vars['primary']['#prefix'] = '<h2 class="element-invisible">' . t('Primary tabs') . '</h2>';
    $vars['primary']['#prefix'] .= '<ul class="tabs--primary nav nav-pills">';
    $vars['primary']['#suffix'] = '</ul>';
    $output .= drupal_render($vars['primary']) . '<hr>';
  }

  if (!empty($vars['secondary'])) {
    $vars['secondary']['#prefix'] = '<h2 class="element-invisible">' . t('Secondary tabs') . '</h2>';
    $vars['secondary']['#prefix'] .= '<ul class="tabs--secondary pagination pagination-sm">';
    $vars['secondary']['#suffix'] = '</ul>';
    $output .= drupal_render($vars['secondary']);
  }

  return $output;
}

/**
 * Implements theme_advanced_forum_reply_link
 */
function apigee_responsive_advanced_forum_reply_link(&$vars) {
  $node = $vars['node'];
  $reply_link = advanced_forum_get_reply_link($node);

  if (is_array($reply_link)) {
    $output = '<div class="topic-reply-allowed">';
    $output .= theme('advanced_forum_l', array(
      'text' => $reply_link['title'],
      'path' => $reply_link['href'],
      'options' => $reply_link['options'],
      'button_class' => 'large'
    ));
    $output .= '</div>';
    return $output;
  }
  elseif ($reply_link == 'reply-locked') {
    return '<div class="topic-reply-locked"><span class="af-button-large btn"><span>' . t('Topic locked') . '</span></span></div>';
  }
  elseif ($reply_link == 'reply-forbidden') {
    return theme('comment_post_forbidden', array('node' => $node));
  }
}


/**
 * Implements hook_form_FORM_ID_alter().
 *
 * @param $form
 * @param $form_state
 */
function apigee_responsive_form_search_form_alter(&$form, &$form_state) {
  $form['#attributes']['class'][] = 'navbar-search';
  $form['#attributes']['class'][] = 'pull-right';
  $form['basic']['keys']['#title'] = '';
  $form['basic']['keys']['#attributes']['class'][] = 'search-query';
  $form['basic']['keys']['#attributes']['class'][] = 'span2';
  $form['basic']['keys']['#attributes']['placeholder'] = t('Search');

  $form['basic']['submit']['#value'] = t('Search');
  $form['basic']['submit']['#attributes']['style'] = 'display:none';

  $default_search = variable_get('search_default_module', 'site');
  if ($default_search == 'apachesolr_search') {
    $default_search = 'site';
  }

  if ($default_search == 'site') {
    unset($form['basic']['#type']);
    unset($form['basic']['#attributes']);
    $form += $form['basic'];
    unset($form['basic']);
    unset($form['action']);
    $form['#submit'] = array('_apigee_responsive_search_form_submit');
  }
}

/**
 * Submit function for Implements hook_form_FORM_ID_alter().
 *
 * @param $form
 * @param $form_state
 */
function _apigee_responsive_search_form_submit($form, &$form_state) {
  if (!empty($form_state['values']['keys'])) {
    $default_search = variable_get('search_default_module', 'site');
    if ($default_search == 'apachesolr_search') {
      $default_search = 'site';
    }
    $form_state['redirect'] = 'search/' . $default_search . '/' . $form_state['values']['keys'];
  }
}

function apigee_responsive_app_status($app) {
  $pending = $revoked = FALSE;
  foreach($app['credential']['apiProducts'] as $product) {
    switch ($product['status']) {
      case 'pending':
        $pending = TRUE;
        break;
      case 'revoked':
        $revoked = TRUE;
        break;
    }
  }
  if ($revoked) {
    return 'Revoked';
  }
  if ($pending) {
    return 'Pending';
  }
  return 'Approved';
}