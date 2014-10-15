<?php
/**
 * Variables:
 *   $manage_users_from Rendered form to remove user from a company or to grant them Mint Roles
 *   $associate_new_user_form Rendered form to add developers to a company
 */
?>
<div class="tab-pane user-roles" id="tab4">
  <div class="col-md-11 offset2 row">
    <h3><?php print t('Associate New User'); ?></h3>
    <hr>
    <?php print $associate_new_user_form; ?>
  </div>
  <div class="col-md-11 row spacer">
    <h3><?php print t('Manage Users'); ?></h3>
    <hr>
    <?php print $manage_users_form; ?>
  </div>
</div>
