<?php

/**
 * @file
 * Default implementation of the order mail template.
 *
 * Variables:
 * - $mail: Order mail address.
 *
 * @see template_preprocess()
 * @see template_process()
 */
?>
<div class="field field-name-commerce-order-mail field-type-commerce-order-mail field-label-above">
  <div class="field-label"><?php print t('E-mail address:') ?></div>
  <div class="field-items">
    <?php print $mail; ?>
  </div>
</div>
