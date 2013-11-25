<?php global $user; $current_path = implode("/", arg()); ?>
<header id="navbar" role="banner" class="navbar navbar-fixed-top">
  <div class="navbar-inner">
    <div class="container">
      <?php if ($logo): ?>
        <a class="brand" href="<?php print $front_page; ?>" title="<?php print t('Home'); ?>">
          <img src="<?php print $logo; ?>" alt="<?php print t('Home'); ?>" />
        </a>
      <?php endif; ?>
      <div class="nav-collapse">
        <nav role="navigation">
          <?php
            $menu_tree = menu_tree_output(menu_tree_all_data('main-menu', NULL, 2));
            print drupal_render($menu_tree);
          ?>
          <div id="login-buttons" class="span7 pull-right">
            <ul class="nav pull-right">
            <?php if ($user->uid == 0) { ?>
            <!-- show/hide login and register links depending on site registration settings -->
            <?php if (($user_reg_setting != 0) || ($user->uid == 1)): ?>
              <li class="<?php echo (($current_path == "user/register")?"active":""); ?>"><?php echo l(t("register"), "user/register"); ?></li>
              <li class="<?php echo (($current_path == "user/login")?"active":""); ?>"><?php echo l(t("login"), "user/login"); ?></li>
            <?php endif; ?>
            <?php } else {
              $user_url =  'user/' . $user->uid; ?>
              <li class="dropdown">
                <a href="#" class="dropdown-toggle" data-toggle="dropdown" title="<?php print $user->mail; ?>"><?php print $truncated_user_email; ?><b class="caret"></b></a>
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
        </nav>
      </div>

    </div>
  </div>
</header>
<div class="master-container">
  <!-- Header -->
  <header role="banner" id="page-header">
    <?php print render($page['header']); ?>
  </header>
  <!-- Breadcrumbs -->
  <div id="breadcrumb-navbar">
    <div class="container">
      <div class="row">
        <div class="span19">
        <?php if ($breadcrumb): print $breadcrumb; endif;?>
        </div>
        <div class="span5 pull-right">
        <?php if ($search): ?>
          <?php if ($search): print render($search); endif; ?>
        <?php endif; ?>
        </div>
      </div>
    </div>
  </div>
  <!-- Title -->
  <?php if (isset($dna) && $dna) { ?>
  <?php } else { ?>
  <?php if (drupal_is_front_page()): ?>
    <section class="page-header">
      <div class="container">
        <div class="row">
          <div class="span9">
            <div class="title">
              <?php if (theme_get_setting('welcome_message')): ?>
              <h1><?php print theme_get_setting('welcome_message'); ?></h1>
              <?php else: ?>
              <h1><span class="welcome">Welcome</span><br />to the&nbsp;<span><?php print $site_name ?></h1></span>
              <?php endif; ?>
            </div>
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
            <span class="<?php print _apigee_base_content_span($columns); ?>">
              <!-- Title Prefix -->
              <?php print render($title_prefix); ?>
              <!-- Title -->
              <h1><?php print render($title); ?></h1>
              <!-- SubTitle -->
              <h2 class="subtitle"><?php print render($subtitle); ?></h2>              <!-- Title Suffix -->
              <?php print render($title_suffix); ?>
            </span>
        </div>
      </div>
    </section>
    <?php endif; ?>
  <?php } ?>
  <div class="page-content">
    <div class="container">
      <?php print $messages; ?>
      <?php if ($page['help']): ?>
        <div class="well"><?php print render($page['help']); ?></div>
      <?php endif; ?>
      <?php if ($action_links): ?>
        <ul class="action-links"><?php print render($action_links); ?></ul>
      <?php endif; ?>
      <div class="row">
        <!-- Sidebar First (Left Sidebar)  -->
        <?php if ($page['sidebar_first']): ?>
          <aside class="span6 pull-left" role="complementary">
            <?php print render($page['sidebar_first']); ?>
          </aside>
        <?php endif; ?>
        <!-- Main Body  -->
        <section class="<?php print _apigee_base_content_span($columns); ?>">
          <?php if ($page['highlighted']): ?>
            <div class="highlighted hero-unit"><?php print render($page['highlighted']); ?></div>
          <?php endif; ?>
          <?php if (($tabs) && (!$is_front)): ?>
            <?php print render($tabs); ?>
          <?php endif; ?>
          <a id="main-content"></a>
          <?php print render($page['content']); ?>
        </section>
        <!-- Sidebar Second (Right Sidebar)  -->
        <?php if ($page['sidebar_second']): ?>
          <aside class="span6 pull-right" role="complementary">
            <?php print render($page['sidebar_second']); ?>
          </aside>  <!-- /#sidebar-second -->
        <?php endif; ?>
      </div>
    </div>
  </div>
  <!-- Footer  -->
  <footer class="footer">
    <div class="footer-inner">
      <div class="container">
        <?php print render($page['footer']); ?>
      </div>
    </div>
  </footer>
</div>
