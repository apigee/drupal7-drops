<?php

/**
 * @file
 * Override default theme implementation to display a single Drupal page.
 *
 *
 * The doctype, html, head and body tags are not in this template. Instead they
 * can be found in the html.tpl.php template in this directory.
 * Replace some divs with HTML elements like <hgroup>, <nav>, <aside>, etc
 *
 * Available variables:
 *
 * General utility variables:
 * - $base_path: The base URL path of the Drupal installation. At the very
 *   least, this will always default to /.
 * - $directory: The directory the template is located in, e.g. modules/system
 *   or themes/bartik.
 * - $is_front: TRUE if the current page is the front page.
 * - $logged_in: TRUE if the user is registered and signed in.
 * - $is_admin: TRUE if the user has permission to access administration pages.
 *
 * Site identity:
 * - $front_page: The URL of the front page. Use this instead of $base_path,
 *   when linking to the front page. This includes the language domain or
 *   prefix.
 * - $logo: The path to the logo image, as defined in theme configuration.
 * - $site_name: The name of the site, empty when display has been disabled
 *   in theme settings.
 * - $site_slogan: The slogan of the site, empty when display has been disabled
 *   in theme settings.
 *
 * Navigation:
 * - $main_menu (array): An array containing the Main menu links for the
 *   site, if they have been configured.
 * - $secondary_menu (array): An array containing the Secondary menu links for
 *   the site, if they have been configured.
 * - $breadcrumb: The breadcrumb trail for the current page.
 *
 * Page content (in order of occurrence in the default page.tpl.php):
 * - $title_prefix (array): An array containing additional output populated by
 *   modules, intended to be displayed in front of the main title tag that
 *   appears in the template.
 * - $title: The page title, for use in the actual HTML content.
 * - $title_suffix (array): An array containing additional output populated by
 *   modules, intended to be displayed after the main title tag that appears in
 *   the template.
 * - $messages: HTML for status and error messages. Should be displayed
 *   prominently.
 * - $tabs (array): Tabs linking to any sub-pages beneath the current page
 *   (e.g., the view and edit tabs when displaying a node).
 * - $action_links (array): Actions local to the page, such as 'Add menu' on the
 *   menu administration interface.
 * - $feed_icons: A string of all feed icons for the current page.
 * - $node: The node object, if there is an automatically-loaded node
 *   associated with the page, and the node ID is the second argument
 *   in the page's path (e.g. node/12345 and node/12345/revisions, but not
 *   comment/reply/12345).
 *
 * Regions:
 * - $page['help']: Dynamic help text, mostly for admin pages.
 * - $page['highlighted']: Items for the highlighted content region.
 * - $page['content']: The main content of the current page.
 * - $page['sidebar_first']: Items for the first sidebar.
 * - $page['sidebar_second']: Items for the second sidebar.
 * - $page['header']: Items for the header region.
 * - $page['footer']: Items for the footer region.
 *
 * @see template_preprocess()
 * @see template_preprocess_page()
 * @see template_process()
 * @see html.tpl.php
 */
?>
<?php global $user; $current_path = implode("/", arg()); ?>
<header id="navbar" role="banner" class="navbar navbar-fixed-top">
  <div class="navbar-inner">
    <div class="container">
        <!-- .btn-navbar is used as the toggle for collapsed navbar content, in mobile/narrow layouts -->
        <?php if ($user->uid == 0): ?>
          <a class="btn btn-navbar" href="/user/register">Register</a>
          <a class="btn btn-navbar" href="/user/login">Login</a>
        <?php else: ?>
          <a class="btn btn-navbar" data-toggle="collapse" data-target="#second">Control Panel</a>
        <?php endif ?>
        <a class="btn btn-navbar" data-toggle="collapse" data-target="#first">Menu</a>
        <?php if ($logo): ?>
          <a class="brand" href="<?php print $front_page; ?>" title="<?php print t('Home'); ?>">
            <img src="<?php print $logo; ?>" alt="<?php print t('Home'); ?>" />
          </a>
        <?php endif; ?>
        <div id="second" class="nav-collapse hidden-desktop">
          <ul class="nav">
            <?php $user_url =  "user/".$user->uid; ?>
            <?php if (module_exists('devconnect_developer_apps')): ?>
            <li><?php echo l('My Apps', $user_url . '/apps'); ?></li>
            <?php endif; ?>
            <li><?php echo l('Edit Profile', $user_url . '/edit'); ?></li>
            <li><?php echo l(t("Logout"), "user/logout"); ?></li>
          </ul>
        </div>
        <div id="first" class="nav-collapse">
          <nav role="navigation">
            <?php if ($primary_nav): ?>
              <?php print $primary_nav; ?>
            <?php endif; ?>
          </nav>
        </div>
        <div id='login-buttons' class="pull-right visible-desktop">
          <ul class="nav">
          <?php if ($user->uid == 0) { ?>
          <!-- show/hide login and register links depending on site registration settings -->
          <?php if (($user_reg_setting != 0) || ($user->uid == 1)): ?>
            <li class="<?php echo (($current_path == "user/register")?"active":""); ?>"><?php echo l(t("register"), "user/register"); ?></li>
            <li class="<?php echo (($current_path == "user/login")?"active":""); ?>"><?php echo l(t("login"), "user/login"); ?></li>
          <?php endif; ?>

          <?php } else {
            $user_url =  "user/".$user->uid; ?>
            <li class="dropdown">
              <a href="#" class="dropdown-toggle" data-toggle="dropdown" title="<?php print $user -> mail; ?>"><?php print $truncated_user_email; ?><b class="caret"></b></a>
              <ul class="dropdown-menu">
                <?php if (module_exists('devconnect_developer_apps')): ?>
                <li><i class="icon-pencil"></i><?php echo l('My Apps', $user_url . '/apps'); ?></li>
                <?php endif; ?>
                <li><i class="icon-user"></i><?php echo l('Edit Profile', $user_url . '/edit'); ?></li>
                <li><i class="icon-off"></i><?php echo l(t("Logout"), "user/logout"); ?></li>
              </ul>
            </li>
            <li><?php echo l(t("logout"), "user/logout"); ?></li>
          <?php } ?>
          </ul>
        </div>
    </div>
  </div>
</header>
  <!-- Breadcrumbs -->
  <div id="breadcrumb-navbar hidden-phone">
    <div class="container">
      <div class="row">
        <?php if($breadcrumb): ?>
          <div class="span18">
            <?php print $breadcrumb;?>
          </div>
          <div class="span6 pull-right visible-desktop">
            <?php if ($search): ?>
              <?php if ($search): print render($search); endif; ?>
            <?php endif; ?>
          </div>
        <?php else: ?>
          <div class="span24 pull-right visible-desktop">
            <?php if ($search): ?>
              <?php if ($search): print render($search); endif; ?>
            <?php endif; ?>
          </div>
        <?php endif; ?>
      </div>
    </div>
  </div>
<?php if (drupal_is_front_page()): ?>
<section class="page-header hidden-phone">
  <div class="container">
    <div class="row">
      <div class="title">
        <?php if (theme_get_setting('welcome_message')): ?>
          <h1><?php print theme_get_setting('welcome_message'); ?></h1>
        <?php else: ?>
          <h1><span class="welcome">Welcome</span><br />to the&nbsp;<span><?php print $site_name ?></h1></span>
        <?php endif; ?>
       </div>
    </div>
    <div class="page-header-content">
      <?php print render($page['homepage_header']); ?>
    </div>
  </div>
</section>
<?php else: ?>
  <section class="page-header">
    <div class="container">
      <div class="row">
        <span class="<?php print _apigee_base_responsive_content_span($columns); ?>">
          <!-- Title Prefix -->
          <?php print render($title_prefix); ?>
          <!-- Title -->
          <h1><?php print render($title); ?></h1>
          <!-- SubTitle -->
          <h2 class="subtitle"><?php print render($subtitle); ?></h2>
          <!-- Title Suffix -->
          <?php print render($title_suffix); ?>
        </span>
      </div>
    </div>
  </section>
<?php endif; ?>
<div class="container">
  <div class="row">
    <?php if ($page['sidebar_first']): ?>
      <aside class="span6" role="complementary">
        <?php print render($page['sidebar_first']); ?>
      </aside>  <!-- /#sidebar-first -->
    <?php endif; ?>

    <section class="span24 main-content">
      <?php if ($is_front): ?>
      <div class="responsive-title visible-phone">
        <?php if (theme_get_setting('welcome_message')): ?>
          <h1><?php print theme_get_setting('welcome_message'); ?></h1>
        <?php else: ?>
          <h1><span class="welcome">Welcome</span><br />to the&nbsp;<span><?php print $site_name ?></h1></span>
        <?php endif; ?>
      </div>
      <?php endif ?>
      <?php if ($page['highlighted']): ?>
        <div class="highlighted hero-unit"><?php print render($page['highlighted']); ?></div>
      <?php endif; ?>
      <?php if (($tabs) && (!$is_front)): ?>
        <?php print render($tabs); ?>
      <?php endif; ?>
      <a id="main-content"></a>
      <div class="row">
      <?php print render($page['content']); ?>
      </div>
    </section>

    <?php if ($page['sidebar_second']): ?>
      <aside class="span6" role="complementary">
        <?php print render($page['sidebar_second']); ?>
      </aside>  <!-- /#sidebar-second -->
    <?php endif; ?>

  </div>
<footer class="footer visible-desktop">
  <div class="footer-inner">
    <div class="container">
      <?php print render($page['footer']); ?>
    </div>
  </div>
</footer>
</div>
