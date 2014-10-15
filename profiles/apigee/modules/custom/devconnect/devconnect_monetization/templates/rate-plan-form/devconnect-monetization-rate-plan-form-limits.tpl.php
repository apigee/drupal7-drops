<?php

?>
<?php if ($has_limits): ?>
  <h3><?php print t('Limits:'); ?></h3>
  <?php if (!empty($package_limits)): ?>
    <h4><?php print t('Package Limits:'); ?></h4>
    <?php foreach ($package_limits as $package_limit): ?>
      <?php print $package_limit; ?><br>
    <?php endforeach; ?>
  <?php endif; ?>
  <?php foreach ($products_limits as $product_name => $product_limits): ?>
    <h4><?php print  $product_name; ?>:</h4>
    <?php foreach ($product_limits as $product_limit): ?>
      <?php print $product_limit; ?><br>
    <?php endforeach; ?>
  <?php endforeach; ?>
  <br>
<?php endif; ?>

