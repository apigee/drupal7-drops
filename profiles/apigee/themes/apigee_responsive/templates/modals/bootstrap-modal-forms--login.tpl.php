<?php
/**
 * @file
 * Provides the user login form HTML.
 */
?>
<div class="modal fade" id="<?php print $identifier?>" tabindex="-1" role="dialog" aria-labelledby="<?php print $identifier?>Label" aria-hidden="true">
  <div class="modal-dialog" style="<?php print $modal_style;?>">
    <div class="modal-content">
      <div class="modal-header">
        <button type="button" class="close" data-dismiss="modal" aria-hidden="true">&times;</button>
        <h4 class="modal-title" id="<?php print $identifier?>Label"><?php print $header; ?></h4>
      </div>
      <div class="modal-body">
        <?php print render($modal_form); ?>
      </div>
      <div class="modal-footer">
        <?php print render($sso); ?>
      </div>
    </div>
  </div>
</div>