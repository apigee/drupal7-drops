<?php if (!empty($content)): ?>
<section class="page-header">
  <div class="container">
    <div class="row">
      <?php print render($title_prefix); ?>
      <h1 class="span18 title"><?php print $content; ?></h1>
      <?php print render($title_suffix); ?>
    </div>
  </div>
</section>
<? endif; ?>