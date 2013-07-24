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
          <?php if ($primary_nav): ?>
            <?php print $primary_nav; ?>
          <?php endif; ?>
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
  <?php if ($title): ?>
      <section class="page-header">
          <div class="container">
            <div class="row">
              <span class="<?php print _apigee_base_content_span($columns); ?>">
               <?php print render($title_prefix); ?>
               <h1><?php print $title; ?></h1>
               <?php print render($title_suffix); ?>
              </span>
            </div>
          </div>
      </section>
  <?php endif; ?>

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
        <!-- Left Sidebar  -->
        <?php if ($page['sidebar_first']): ?>
          <aside class="span6" role="complementary">
            <?php print render($page['sidebar_first']); ?>
          </aside>
        <?php endif; ?>

        <!-- Main Body  -->
        <section class="<?php print _apigee_base_content_span($columns); ?>">
          <?php if ($page['highlighted']): ?>
            <div class="highlighted hero-unit"><?php print render($page['highlighted']); ?></div>
          <?php endif; ?>
          <?php if ($tabs): ?>
            <?php print render($tabs); ?>
          <?php endif; ?>
          <a id="main-content"></a>
          <?php print render($page['content']); ?>
        </section>

        <!-- Right Sidebar  -->
        <?php if ($page['sidebar_second']): ?>
          <aside class="span6" role="complementary">
            <?php print render($page['sidebar_second']); ?>
          </aside>  <!-- /#sidebar-second -->
        <?php endif; ?>

      </div>
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




