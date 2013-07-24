<article class="<?php print $classes . ' ' . $zebra; ?>"<?php print $attributes; ?>>

  <header>
    <p class="comment-submitted">
      <?php print $submitted; ?>
    </p>
  </header>

  <?php
    // We hide the comments and links now so that we can render them later.
    hide($content['links']);
    print render($content);
  ?>

  <?php print render($content['links']) ?>

  <?php if ($signature): ?>
    <footer class="user-signature clearfix">
      <?php print $signature; ?>
    </footer>
  <?php endif; ?>

</article> <!-- /.comment -->
