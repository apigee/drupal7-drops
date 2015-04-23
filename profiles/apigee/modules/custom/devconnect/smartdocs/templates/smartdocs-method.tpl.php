<div class="row method_details clearfix">
  <div class="col-sm-4">
    <div class="row method_data title">
      <div class="col-sm-2">
        <div class="verb-auth">
          <p class="<?php print $verb; ?>" data-role="<?php print $verb; ?>"><?php print $verb; ?></p>
          <span class="icon_lock" title="<?php print t('This method needs authentication.'); ?>"></span>
        </div>
      </div>
      <div class="col-sm-10">
        <div class="title-description">
          <?php print l($node->title, 'node/' . $node->nid); ?>
          <p data-role="resource_path" class="resource_path" title="<?php print $path; ?>">
            <?php print $path; ?>
          </p>
        </div>
      </div>
    </div>
  </div>
  <div class="col-sm-8">
    <div class="method_data description">
      <p><?php print $body; ?></p>
    </div>
  </div>
</div>
