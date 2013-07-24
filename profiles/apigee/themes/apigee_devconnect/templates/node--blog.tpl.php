<article id="node-<?php print $node->nid; ?>" class="<?php print $classes; ?> clearfix"<?php print $attributes; ?>>

  <header>
    <?php print render($title_prefix); ?>
    <?php if (!$page && $title): ?>
      <h2<?php print $title_attributes; ?>><a href="<?php print $node_url; ?>"><?php print $title; ?></a></h2>
    <?php endif; ?>
    <?php print render($title_suffix); ?>

    <?php if (($display_submitted) && ($teaser)): ?>
      <div class="node-date">
        <div class="month"><?php print $submitted_month; ?></div>
        <div class="day"><?php print $submitted_day; ?></div>
      </div>
    <?php endif; ?>

  </header>

  <?php if ($teaser): ?>
    <div class="node-body">
      <div class="posted"><?php print $posted; ?></div>
      <?php
        // Hide comments, tags, and links now so that we can render them later.
        hide($content['comments']);
        hide($content['links']);
        hide($content['field_tags']);
        print render($content);
      ?>
    </div>
  <?php else: ?>
    <div class="posted"><?php print $posted; ?></div>
    <?php
      // Hide comments, tags, and links now so that we can render them later.
      hide($content['comments']);
      hide($content['links']);
      hide($content['field_tags']);
      print render($content);
    ?>
  <?php endif; ?>

  <?php if (!empty($content['field_tags']) || !empty($content['links'])): ?>
    <footer>
      <?php print render($content['field_tags']); ?>
      <?php print render($content['links']); ?>
    </footer>
  <?php endif; ?>

  <?php print render($content['comments']); ?>

</article> <!-- /.node -->
