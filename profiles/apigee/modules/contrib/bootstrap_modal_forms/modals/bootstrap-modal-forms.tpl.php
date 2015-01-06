<?php
/**
 * @file
 * Provides the user login form HTML.
 *
 * Expected variables:
 * - $version (should be either 2 or 3)
 * - $identifier (unique string identifier of the form)
 * - $modal_style (optional CSS style for the form)
 * - $header (pre-sanitized header HTML)
 * - $modal_form (array representing Drupal form)
 */

$label_element = ($version == 2 ? 'h3' : 'h4');
?>
<div id="<?php print $identifier?>" class="modal fade<?php if ($version == 2) { print ' hide'; }?>" tabindex="-1" role="dialog" aria-labelledby="<?php print $identifier ?>Label" aria-hidden="true">
<?php if ($version == 3): // modal-dialog requires wrappers ?>
  <div class="modal-dialog"<?php if ($modal_style) { print ' style="' . $modal_style . '"';} ?>><div class="modal-content">
<?php endif; ?>
  <div class="modal-header">
    <button type="button" class="close" data-dismiss="modal" aria-hidden="true">×</button>
    <<?php print $label_element; ?> id="<?php print $identifier; ?>Label"<?php if ($version == 3) { print ' class="modal-title"'; }?>><?php print $header; ?></<?php print $label_element?>>
  </div>
  <div class="modal-body">
    <?php print render($modal_form); ?>
  </div>
  <div class="modal-footer">
    <button type="button" class="btn<?php if ($version == 2) { print ' btn-default'; } ?>" data-dismiss="modal"><?php print t('Close'); ?></button>
  </div>
<?php if ($version == 3): // Close the modal-dialog wrappers ?>
  </div></div>
<?php endif; ?>
</div>