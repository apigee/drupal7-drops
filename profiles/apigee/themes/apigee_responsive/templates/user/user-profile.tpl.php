<?php
$account = $user_profile['account'];

?>
<div class="media">
  <div class="media-body">
    <?php if (!empty($account->field_first_name) && !empty($account->field_last_name)) { ?>
      <h4 class="media-heading"><?php print $account->field_first_name['und'][0]['safe_value']; ?>
        <?php print $account->field_last_name['und'][0]['safe_value']; ?></h4>
    <?php } else { ?>
      <h4 class="media-heading"><?php print $account->name; ?></h4>
    <?php } ?>

  </div>
</div>