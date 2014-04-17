<?php foreach ($products as $product): ?>
  <?php $price_points = $product->getPricePoints(); ?>
  <?php if (!empty($price_points)): ?>
    <strong>Price Points for <?php echo $product->getDisplayName(); ?></strong>
    <br>
    <br>
    <div class="table-responsive">
      <table class="table table-bordered">
        <thead>
        <tr>
          <th>Operator</th>
          <th>Country</th>
          <th>Currency</th>
          <th>Min Gross</th>
          <th>Max Gross</th>
          <th>Min Net</th>
          <th>Max Net</th>
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
    </div>
  <?php endif; ?>
<?php endforeach; ?>