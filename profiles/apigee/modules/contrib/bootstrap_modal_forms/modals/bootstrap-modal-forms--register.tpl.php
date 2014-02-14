<?php
/**
 * @file
 * Provides the user register form HTML.
 */

?>
<?php if (!user_is_logged_in() && $version == '2') { ?>
  <div id="user_register_form_modal" class="modal hide fade" tabindex="-1" role="dialog" aria-labelledby="user_register_form_modalLabel" aria-hidden="true">
    <div class="modal-header">
      <button type="button" class="close" data-dismiss="modal" aria-hidden="true">×</button>
      <h3 id="user_register_form_modalLabel"><?php print $header; ?></h3>
    </div>
    <div class="modal-body">
      <?php print render($modal_form); ?>
    </div>
    <div class="modal-footer">
      <button class="btn" data-dismiss="modal" aria-hidden="true">Close</button>
    </div>
  </div>
<?php } else if (!user_is_logged_in() && $version == '3') { ?>
  <div class="modal fade" id="user_register_form_modal" tabindex="-1" role="dialog" aria-labelledby="user_register_form_modalLabel" aria-hidden="true">
    <div class="modal-dialog">
      <div class="modal-content">
        <div class="modal-header">
          <button type="button" class="close" data-dismiss="modal" aria-hidden="true">&times;</button>
          <h4 class="modal-title" id="user_register_form_modalLabel"><?php print $header; ?></h4>
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

<?php } else {} ?>