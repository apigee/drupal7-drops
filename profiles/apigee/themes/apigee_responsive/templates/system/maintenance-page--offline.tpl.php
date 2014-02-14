<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML+RDFa 1.0//EN"
  "http://www.w3.org/MarkUp/DTD/xhtml-rdfa-1.dtd">
<html lang="<?php print $language->language; ?>" dir="<?php print $language->dir; ?>"<?php print $rdf_namespaces;?>>
<head profile="<?php print $grddl_profile; ?>">
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <?php print $head; ?>
  <title><?php print $head_title; ?></title>
  <?php print $styles; ?>
  <!-- HTML5 element support for IE6-8 -->
  <!--[if lt IE 9]>
  <script src="//html5shiv.googlecode.com/svn/trunk/html5.js"></script>
  <![endif]-->
  <?php print $scripts; ?>
</head>
<body class="<?php print $classes; ?>" <?php print $attributes;?>>
<div id="skip-link">
  <a href="#main-content" class="element-invisible element-focusable"><?php print t('Skip to main content'); ?></a>
</div>
<br>
<br>
<br>
<div class="master-container">
  <section class="page-header">
    <div class="container">
      <div class="row">
        <div class="col-md-4">
          <img src="/profiles/apigee/themes/apigee_responsive/images/homepage-image.png">
        </div>
        <div class="col-md-8">
          <br>
          <br>
          <h1>Looks like we're having an issue!</h1>
          <hr>
          <h3><span class="text-muted">Don't worry, we're going to fix it...</span></h3>
          <hr>
          <p class="text-muted">We'll be back online shortly!</p>
        </div>
      </div>
    </div>
  </section>
</div>

</body>
</html>