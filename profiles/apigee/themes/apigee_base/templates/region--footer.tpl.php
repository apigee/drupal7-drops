<!-- Footer  -->
<footer id="footer" role="banner" class="footer navbar navbar-fixed-bottom">
  <div class="footer-inner">
    <div class="container">
      <div class="row">
        <div class="span13">
          <?php if ($content): ?>
            <div class="<?php print $classes; ?>">
              <?php print $content; ?>
            </div>
          <?php endif; ?>
        </div>
        <div class="footer-txt span8 pull-right">
          <p>&copy; 2013 Apigee Corp. All rights reserved</p>
        </div>
      </div>
    </div>
  </div>
</footer>
