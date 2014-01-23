<?php
  $overlapping = $variables['overlapping'];
?>
<ul>
  <?php foreach ($overlapping as $plan => $products): ?>
    <?php list($plan_id, $plan_name) = explode('|', $plan); ?>
    <li>
      <h3><?php print $plan_name ?></h3>

      <?php if (count($products['will-include']) > 0): ?>
        <h4>Additional products:</h4>
        <ul>
          <?php foreach ($products['will-include'] as $product): ?>
            <li><?php print (is_object($product) ? $product->getDisplayName() : $product); ?></li>
          <?php endforeach; ?>
        </ul>
        <br>
      <?php endif; ?>

      <?php if (count($products['will-exclude']) > 0): ?>
        <h4>Excluded products:</h4>
        <ul>
          <?php foreach ($products['will-exclude'] as $product): ?>
            <li><?php print (is_object($product) ? $product->getDisplayName() : $product); ?></li>
          <?php endforeach; ?>
        </ul>
        <br>
      <?php endif; ?>

      <?php if (count($products['conflicting']) > 0): ?>
        <h4>Conflicting products:</h4>
        <ul>
          <?php foreach ($products['conflicting'] as $product): ?>
            <li><?php print (is_object($product) ? $product->getDisplayName() : $product); ?></li>
          <?php endforeach; ?>
        </ul>
        <br>
      <?php endif; ?>
    </li>
  <?php endforeach; ?>
</ul>