<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML+RDFa 1.0//EN"
  "http://www.w3.org/MarkUp/DTD/xhtml-rdfa-1.dtd" >
<html lang="en"
      xmlns="http://www.w3.org/1999/xhtml"
      xmlns:foaf="http://xmlns.com/foaf/0.1/"
      xmlns:dc="http://purl.org/dc/elements/1.1/"
      version="XHTML+RDFa 1.0" xml:lang="en">
<head>
  <meta charset="utf-8">
  <?php print $head; ?>
  <title><?php print $head_title; ?></title>
  <?php print $styles; ?>
  <!-- HTML5 element support for IE6-8 -->
  <!--[if lt IE 9]>
  <?php print $shiv; ?>
  <![endif]-->
  <script type="text/javascript" src="//use.typekit.net/ezw2jtl.js"></script>
  <script type="text/javascript">try{Typekit.load();}catch(e){}</script>
</head>
<body class="<?php print $classes; ?>" <?php print $attributes;?>>
<?php print $page_top; ?>
<?php print $page; ?>
<?php print $scripts; ?>
<?php print $page_bottom; ?>
<script type="text/javascript" src="http://include.reinvigorate.net/re_.js"></script>
<script type="text/javascript">
  try
  { reinvigorate.track("5ub50-jv73654n36"); }
  catch(err) {}
</script>
<script type="text/javascript">
  (function() {
    var didInit = false;
    function initMunchkin() {
      if(didInit === false)
      { didInit = true; Munchkin.init('351-WXY-166'); }
    }
    var s = document.createElement('script');
    s.type = 'text/javascript';
    s.async = true;
    s.src = document.location.protocol + '//munchkin.marketo.net/munchkin.js';
    s.onreadystatechange = function() {
      if (this.readyState == 'complete' || this.readyState == 'loaded')
      { initMunchkin(); }
    };
    s.onload = initMunchkin;
    document.getElementsByTagName('head')[0].appendChild(s);
  })();
</script>
</body>
</html>
