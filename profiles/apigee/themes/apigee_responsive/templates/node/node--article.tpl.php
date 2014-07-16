<article id="node-<?php print $node->nid; ?>" class="<?php print $classes; ?> clearfix"<?php print $attributes; ?>>
  <?php if ($teaser): ?>
    <div class="row node-body">
      <br/>
      <div class="col-sm-1 posted">
        <?php if (($display_submitted) && ($teaser)): ?>
          <div class="node-date">
            <div class="month"><?php print $submitted_month; ?></div>
            <div class="day"><?php print $submitted_day; ?></div>
          </div>
        <?php endif; ?>
      </div>
      <div class="col-sm-11">
        <h2<?php print $title_attributes; ?>><a href="<?php print $node_url; ?>"><?php print $title; ?></a></h2>
        <?php print render($title_suffix); ?>
        <hr/>
        <?php
        // Hide comments, tags, and links now so that we can render them later.
        hide($content['comments']);
        hide($content['links']);
        hide($content['field_tags']);
        print render($content);
        ?>
      </div>
    </div>
    <div class="row">
      <br/>
      <br/>
      <div class="col-sm-1"></div>
      <div class="col-sm-11">
        <?php if (!empty($content['field_tags']) || !empty($content['links'])): ?>
          <footer>
            <?php print render($content['field_tags']); ?>
            <?php print render($content['links']); ?>
          </footer>
        <?php endif; ?>
      </div>
    </div>
    <hr/>
    <?php print render($content['comments']); ?>
  <?php else: ?>
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
    <?php if (isset($posted)) { ?>
      <div class="posted"><?php print $posted; ?></div>
    <?php } ?>
    <?php
    // Hide comments, tags, and links now so that we can render them later.
    hide($content['comments']);
    hide($content['links']);
    hide($content['field_tags']);
    print render($content);
    ?>
    <?php if (!empty($content['field_tags']) || !empty($content['links'])): ?>
      <footer>
        <?php print render($content['field_tags']); ?>
        <?php print render($content['links']); ?>
        <?php if (!empty($content['comments'])) { ?>
          <hr>
        <?php } ?>
      </footer>
    <?php endif; ?>

    <?php print render($content['comments']); ?>
  <?php endif; ?>

</article> <!-- /.node -->
