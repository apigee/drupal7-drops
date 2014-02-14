<?php
/**
 * @file
 * Provides the user login form HTML.
 */
?>
<?php if ($version == '2') { ?>
  <div id="<?php print $identifier?>" class="modal hide fade" tabindex="-1" role="dialog" aria-labelledby="<?php print $identifier ?>Label" aria-hidden="true">
    <div class="modal-header">
      <button type="button" class="close" data-dismiss="modal" aria-hidden="true">×</button>
      <h3 id="<?php print $identifier?>Label"><?php print $header; ?></h3>
    </div>
    <div class="modal-body">
      <?php print render($modal_form); ?>
    </div>
    <div class="modal-footer">
      <button class="btn" data-dismiss="modal" aria-hidden="true">Close</button>
    </div>
  </div>
<?php } else if ($version == '3') { ?>
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
          <button type="button" class="btn btn-default" data-dismiss="modal">Close</button>
        </div>
      </div>
    </div>
  </div>

<?php } ?>