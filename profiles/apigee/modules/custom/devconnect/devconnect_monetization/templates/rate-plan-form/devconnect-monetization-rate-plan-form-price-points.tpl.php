<?php foreach ($products as $product): ?>
  <?php $price_points = $product->getPricePoints(); ?>
  <?php if (!empty($price_points)): ?>
    <strong><?php print t('Price Points for @product', array('@product' => $product->getDisplayName())); ?></strong>
    <br />
    <br />
    <table>
      <thead>
      <tr>
          <th><?php print t('Operator'); ?></th>
          <th><?php print t('Country'); ?></th>
          <th><?php print t('Currency'); ?></th>
          <th><?php print t('Min Gross'); ?></th>
          <th><?php print t('Max Gross'); ?></th>
          <th><?php print t('Min Net'); ?></th>
          <th><?php print t('Max Net'); ?></th>
      </tr>
      </thead>
      <tbody>
      <?php foreach ($price_points as $price_point): ?>
        <tr>
          <td><?php echo $price_point->getOrganization()->getDescription(); ?></td>
          <td><?php echo Apigee\Mint\Types\Country::getCountryName($price_point->getOrganization()->getCountry()); ?></td>
          <td><?php echo $price_point->getOrganization()->getCurrency(); ?></td>
          <td><?php echo is_numeric($price_point->getGrossStartPrice()) ? sprintf('%.2f', $price_point->getGrossStartPrice()) : '--'; ?></td>
          <td><?php echo is_numeric($price_point->getGrossEndPrice()) ? sprintf('%.2f', $price_point->getGrossEndPrice()) : '--'; ?></td>
          <td><?php echo is_numeric($price_point->getNetStartPrice()) ? sprintf('%.2f', $price_point->getNetStartPrice()) : '--'; ?></td>
          <td><?php echo is_numeric($price_point->getNetEndPrice()) ? sprintf('%.2f', $price_point->getNetEndPrice()) : '--'; ?></td>
        </tr>
      <?php endforeach; ?>
      </tbody>
    </table>
  <?php endif; ?>
<?php endforeach; ?>
