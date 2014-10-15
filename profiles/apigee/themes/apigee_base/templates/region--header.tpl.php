<header id="navbar" role="banner" class="navbar navbar-fixed-top">
  <div class="main-nav navbar-inner">
    <div class="container">
      <div class="row">
        <div class="nav-collapse">
          <div class="span3 pull-left">
            <?php if ($logo): ?>
              <a class="brand" href="<?php print $logo_link; ?>" title="<?php print t('Home'); ?>">
                <img src="<?php print $logo; ?>" alt="<?php print t('Home'); ?>" />
              </a>
            <?php endif; ?>
          </div>

          <?php if ($main_menu || $secondary_menu): ?>
              <?php print theme('links__system_main_menu', array('links' => $main_menu, 'attributes' => array('id' => 'main-menu', 'class' => array('nav')))); ?>
          <?php endif; ?>

          <?php if ($search): ?>
            <div class="span4 pull-right searchy">
              <?php print render($search); ?>
            </div>
          <?php endif; ?>

          <?php if ($show_sign_up): ?>
            <div id="header-sign-up-now">
              <a class="btn btn-large" href="<?php print $sign_up_url ?>">Sign Up Now</a>
              <span>Have an account?&nbsp;</span><a class="link" href="<?php print $sign_in_url ?>">Sign In</a>
            </div>
          <?php endif; ?>
        </div>
      </div>
    </div>
  </div>

  <?php if ($secondary_menu || $content): ?>
  <div class="sub-nav navbar-inner">
    <div class="container">
      <div class="row">
        <?php if ($secondary_menu): ?>
        <div role="banner" id="page-header" class="span18">
            <div class="secondary-menu-wrapper">
              <?php print theme('links__system_secondary_menu', array('links' => $secondary_menu, 'attributes' => array('id' => 'secondary-menu', 'class' => array('links', 'inline', 'clearfix')))); ?>
            </div>
        </div>
        <?php endif; ?>

        <?php if ($content): ?>
        <div id='login-buttons' class="span6 pull-right">
            <div class="<?php print $classes; ?>">
              <?php print $content; ?>
            </div>
        </div>
        <?php endif; ?>
      </div>
    </div>
  </div>
  <?php endif; ?>
</header>
