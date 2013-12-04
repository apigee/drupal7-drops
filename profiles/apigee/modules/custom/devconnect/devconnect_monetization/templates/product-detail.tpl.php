<?php

use Apigee\Mint\Types\RatePlanRateType;
use Apigee\Mint\Types\MeteringType;
use Apigee\Mint\Types\Country;

?>
<?php $free_quantity_message = _devconnect_monetization_get_free_quantity_text($rate_plan_detail, $rate_plan); ?>
<?php $product = $rate_plan_detail->product; ?>
<?php if ($rate_plan_detail->type == RatePlanRateType::REVSHARE): ?>
  <?php if (isset($free_quantity_message)): ?>
      <span><strong>Free Quantity:&nbsp;</strong><?php echo $free_quantity_message; ?></span>
  <?php endif; ?>
  <?php if(isset($product)) : ?>
  <strong>Product:&nbsp;</strong><?php echo $product->getDisplayName(); ?>
  <?php endif; ?>
  <?php $rate_plan_rates = $rate_plan_detail->ratePlanRates; ?>
  <?php $is_band = count($rate_plan_rates) > 1; ?>
  <?php $row_span = 'rowspan="2"'; $col_span = 'colspan="'. count($rate_plan_rates) .'"'; ?>
  <table>
    <thead>
      <tr>
      <?php $rate_values = _devconnect_monetization_sort_rate_plan_rates($rate_plan_detail->ratePlanRates); ?>
      <?php if (count($rate_values) == 1): ?>
        <th>Operator</th>
        <th>Country</th>
        <th>Currency</th>
        <?php if (strlen($rate_plan_detail->revenueType)): ?>
        <th>Pricing Type</th>
        <?php endif; ?>
        <th>Rev Share %</th>
      <?php else: ?>
        <th rowspan="2">Operator</th>
        <th rowspan="2">Country</th>
        <th rowspan="2">Currency</th>
        <?php if (strlen($rate_plan_detail->revenueType)): ?>
        <th rowspan="2">Pricing Type</th>
        <?php endif; ?>
        <th colspan="<?php echo count($rate_values); ?>">Revenue Bands/Rev Share %</th>
      </tr>
      <tr>
        <?php foreach ($rate_values as $rate_value): ?>
          <th>
            <?php echo'Greater than ' . $rate_value->startUnit . (isset($rate_value->endUnit) ? '<br>Up to ' . $rate_value->endUnit : ''); ?>
          </th>
        <?php endforeach; ?>
      <?php  endif; ?>
      </tr>
    </thead>
    <tbody>
      <?php if ($rate_plan->isGroupPlan()): ?>
        <?php foreach ($rate_plan->getRatePlanDetailsByProduct($rate_plan_detail->product) as $rate_plan_detail): ?>
          <?php $rate_values = _devconnect_monetization_sort_rate_plan_rates($rate_plan_detail->ratePlanRates); ?>
          <tr>
            <td><?php echo $rate_plan_detail->getOrganization()->getDescription(); ?></td>
            <td><?php echo Country::getCountryName($rate_plan_detail->getOrganization()->getCountry()); ?></td>
            <td><?php echo $rate_plan_detail->currency->name; ?></td>
            <?php if (strlen($rate_plan_detail->revenueType)): ?>
            <td><?php echo ucwords(strtolower($rate_plan_detail->revenueType)); ?></td>
            <?php endif; ?>
            <?php foreach ($rate_values as $rate_value): ?>
            <td><?php echo $rate_value->revshare; ?>&nbsp;%</td>
            <?php endforeach; ?>
          </tr>
        <?php endforeach; ?>
      <?php else: ?>
      <tr>
        <td><?php echo $rate_plan_detail->getOrganization()->getDescription(); ?></td>
        <td><?php echo Country::getCountryName($rate_plan_detail->getOrganization()->getCountry()); ?></td>
        <td><?php echo $rate_plan_detail->currency->name; ?></td>
        <?php if (strlen($rate_plan_detail->revenueType)): ?>
        <td><?php echo ucwords(strtolower($rate_plan_detail->revenueType)); ?></td>
        <?php endif; ?>
        <?php foreach ($rate_values as $rate_value): ?>
        <td><?php echo $rate_value->revshare; ?>&nbsp;%</td>
        <?php endforeach; ?>
      </tr>
      <?php endif; ?>
    </tbody>
  </table>
<?php else: // $rate_plan_detail->type === RatePlanRateType::RATECARD ?>
  <?php if ($rate_plan_detail->meteringType == MeteringType::UNIT): ?>
    <br>
    <?php if (isset($product)): ?>
    <strong>Product:&nbsp;</strong><?php echo $product->getDisplayName(); ?>
    <?php else: ?>
    <strong>Products:&nbsp;</strong><?php echo str_replace(',', ' &amp;', $product_list);?>
    <?php endif; ?>
    <br>
    <strong>Rate Card is based on: </strong><?php echo _devconnect_monetization_get_rate_card($rate_plan_detail); ?>
    <?php if (isset($free_quantity_message)): ?>
    <br>
    <strong>Free Quantity:&nbsp;</strong><?php echo $free_quantity_message; ?>
    <?php else: ?>
    <br>
    <?php endif; ?>
    <br>
    <br>
    <table>
      <thead>
        <tr>
          <th>Operator</th>
          <th>Country</th>
          <th>Currency</th>
          <?php if (strlen($rate_plan_detail->revenueType)): ?>
          <th>Pricing Type</th>
          <?php endif; ?>
          <?php foreach($rate_plan_detail->ratePlanRates as $rate_plan_rate): ?>
            <?php if ($rate_plan_rate->type == RatePlanRateType::REVSHARE && $rate_plan_rate->revshare > 0): ?>
              <th>Rev Share %</th>
            <?php endif; ?>
          <?php endforeach; ?>
          <?php foreach($rate_plan_detail->ratePlanRates as $rate_plan_rate): ?>
            <?php if ($rate_plan_rate->type == RatePlanRateType::RATECARD): ?>
              <th>Rate</th>
            <?php endif; ?>
          <?php endforeach; ?>
        </tr>
      </thead>
      <tbody>
        <?php if ($rate_plan->isGroupPlan()): ?>
          <?php foreach ($rate_plan->getRatePlanDetailsByProduct($rate_plan_detail->product) as $rate_plan_detail): ?>
            <?php $rate_values = _devconnect_monetization_sort_rate_plan_rates($rate_plan_detail->ratePlanRates); ?>
             <tr>
                <td><?php echo $rate_plan_detail->getOrganization()->getDescription(); ?></td>
                <td><?php echo Country::getCountryName($rate_plan_detail->getOrganization()->getCountry()); ?></td>
                <td><?php echo $rate_plan_detail->currency->name; ?></td>
                <?php if (strlen($rate_plan_detail->revenueType)): ?>
                <td><?php echo ucwords(strtolower($rate_plan_detail->revenueType)); ?></td>
                <?php endif; ?>
                <?php foreach($rate_plan_detail->ratePlanRates as $rate_plan_rate): ?>
                  <?php if ($rate_plan_rate->type == RatePlanRateType::REVSHARE && $rate_plan_rate->revshare > 0): ?>
                    <td><?php echo $rate_plan_rate->revshare; ?>&nbsp;%</td>
                  <?php endif; ?>
                <?php endforeach; ?>
                <?php foreach($rate_plan_detail->ratePlanRates as $rate_plan_rate): ?>
                  <?php if ($rate_plan_rate->type == RatePlanRateType::RATECARD): ?>
                    <td><?php echo $rate_plan_rate->rate; ?></td>
                  <?php endif; ?>
                <?php endforeach; ?>
              </tr>
          <?php endforeach; ?>
        <?php else: ?>
            <tr>
              <td><?php echo $rate_plan_detail->getOrganization()->getDescription(); ?></td>
              <td><?php echo Country::getCountryName($rate_plan_detail->getOrganization()->getCountry()); ?></td>
              <td><?php echo $rate_plan_detail->currency->name; ?></td>
              <?php if (strlen($rate_plan_detail->revenueType)): ?>
              <td><?php echo ucwords(strtolower($rate_plan_detail->revenueType)); ?></td>
              <?php endif; ?>
              <?php foreach($rate_plan_detail->ratePlanRates as $rate_plan_rate): ?>
                <?php if ($rate_plan_rate->type == RatePlanRateType::REVSHARE && $rate_plan_rate->revshare > 0): ?>
                  <td><?php echo $rate_plan_rate->revshare; ?>&nbsp;%</td>
                <?php endif; ?>
              <?php endforeach; ?>
              <?php foreach($rate_plan_detail->ratePlanRates as $rate_plan_rate): ?>
                <?php if ($rate_plan_rate->type == RatePlanRateType::RATECARD): ?>
                  <td><?php echo $rate_plan_rate->rate; ?></td>
                <?php endif; ?>
              <?php endforeach; ?>
            </tr>
        <?php endif; ?>
      </tbody>
    </table>
  <?php elseif ($rate_plan_detail->meteringType == MeteringType::VOLUME): ?>
    <br><br>
    <?php if (isset($product)): ?>
    <strong>Product:&nbsp;</strong><?php echo $product->getDisplayName(); ?>
    <?php else: ?>
    <strong>Products:</strong>&nbsp;<?php echo str_replace(',', ' &amp;', $product_list);?>
    <?php endif; ?>
    <br>
    <strong>Rate Card is based on: </strong><?php echo _devconnect_monetization_get_rate_card($rate_plan_detail); ?>
    <br>
    <strong>Volume Aggregation Basis: </strong><?php echo $rate_plan_detail->duration . ' ' . strtolower($rate_plan_detail->durationType) . ($rate_plan_detail->duration > 1 ? 's' : ''); ?>
    <?php if (isset($free_quantity_message)): ?>
    <br>
    <strong>Free Quantity:&nbsp;</strong><?php echo $free_quantity_message; ?>
    <?php endif; ?>
    <br>
    <br>
    <table>
      <thead>
        <tr>
         <?php $rate_values = _devconnect_monetization_sort_rate_plan_rates($rate_plan_detail->ratePlanRates, TRUE); ?>
          <th rowspan="2">Operator</th>
          <th rowspan="2">Country</th>
          <th rowspan="2">Currency</th>
          <?php if (strlen($rate_plan_detail->revenueType)): ?>
          <th rowspan="2">Pricing Type</th>
          <?php foreach ($rate_values['REVSHARE'] as $rate_value): ?>
          <th rowspan="2">
            <?php echo $rate_value->type == RatePlanRateType::REVSHARE ? 'Rev Share %<br>' : '' ?>
            <?php echo'Greater than ' . $rate_value->startUnit . (isset($rate_value->endUnit) ? '<br>Up to ' . $rate_value->endUnit : ''); ?>
          </th>
          <?php endforeach; ?>
          <?php endif; ?>
          <th colspan="<?php echo count($rate_values['RATECARD']); ?>">Volume band</th>
        </tr>
        <tr>
        <?php foreach ($rate_values['RATECARD'] as $rate_value): ?>
          <th>
            <?php echo'Greater than ' . $rate_value->startUnit . (isset($rate_value->endUnit) ? '<br>Up to ' . $rate_value->endUnit : ''); ?>
          </th>
          <?php endforeach; ?>
        </tr>
      </thead>
      <tbody>
        <?php if ($rate_plan->isGroupPlan()): ?>
        <?php foreach ($rate_plan->getRatePlanDetailsByProduct($rate_plan_detail->product) as $rate_plan_detail): ?>
            <?php $rate_values = _devconnect_monetization_sort_rate_plan_rates($rate_plan_detail->ratePlanRates, TRUE); ?>
            <tr>
              <td><?php echo $rate_plan_detail->getOrganization()->getDescription(); ?></td>
              <td><?php echo Country::getCountryName($rate_plan_detail->getOrganization()->getCountry()); ?></td>
              <td><?php echo $rate_plan_detail->currency->name; ?></td>
              <?php if (strlen($rate_plan_detail->revenueType)): ?>
              <td><?php echo ucwords(strtolower($rate_plan_detail->revenueType)); ?></td>
              <?php endif; ?>
              <?php foreach ($rate_values['REVSHARE'] as $rate_value): ?>
              <td><?php echo $rate_value->revshare; ?>&nbsp;%</td>
              <?php endforeach; ?>
              <?php foreach ($rate_values['RATECARD'] as $rate_value): ?>
              <td><?php echo $rate_value->rate; ?></td>
              <?php endforeach; ?>
            </tr>
          <?php endforeach; ?>
        <?php else: ?>
          <tr>
            <td><?php echo $rate_plan_detail->getOrganization()->getDescription(); ?></td>
            <td><?php echo Country::getCountryName($rate_plan_detail->getOrganization()->getCountry()); ?></td>
            <td><?php echo $rate_plan_detail->currency->name; ?></td>
            <?php if (strlen($rate_plan_detail->revenueType)): ?>
            <td><?php echo ucwords(strtolower($rate_plan_detail->revenueType)); ?></td>
            <?php endif; ?>
            <?php foreach ($rate_values['REVSHARE'] as $rate_value): ?>
            <td><?php echo $rate_value->revshare; ?>&nbsp;%</td>
            <?php endforeach; ?>
            <?php foreach ($rate_values['RATECARD'] as $rate_value): ?>
            <td><?php echo $rate_value->rate; ?></td>
            <?php endforeach; ?>
          </tr>
        <?php endif;?>
      </tbody>
    </table>
  <?php elseif ($rate_plan_detail->meteringType == MeteringType::STAIR_STEP): ?>
    <?php if (isset($rate_plan_detail->product)): ?>
      <strong>Product: </strong><?php echo $rate_plan_detail->product->getDisplayName(); ?>
      <br>
      <strong>Rate Card is based on: </strong><?php echo _devconnect_monetization_get_rate_card($rate_plan_detail); ?>
      <br>
    <?php endif; ?>
    <?php if (isset($free_quantity_message)): ?>
      <span><strong>Free Quantity:&nbsp;</strong><?php echo $free_quantity_message; ?></span><br>
    <?php endif; ?>
    <span>Bundles expire in <?php echo $rate_plan_detail->duration . '&nbsp;' .  strtolower($rate_plan_detail->durationType) . ($rate_plan_detail->duration > 1 ? 's' : ''); ?></span>
    <br>
    <br>
    <table>
      <thead>
        <tr>
          <?php $rate_values = _devconnect_monetization_sort_rate_plan_rates($rate_plan_detail->ratePlanRates, TRUE); ?>
          <th rowspan="2">Operator</th>
          <th rowspan="2">Country</th>
          <th rowspan="2">Currency</th>
          <?php if (strlen($rate_plan_detail->revenueType)): ?>
          <th rowspan="2">Pricing Type</th>
          <?php endif; ?>
          <?php if (isset($rate_values['REVSHARE']) && $rate_values['REVSHARE']->revshare > 0): ?>
          <th rowspan="2">Rate</th>
          <?php endif; ?>
          <th colspan="<?php echo count($rate_values['RATECARD']); ?>">Bundles</th>
        </tr>
        <tr>
          <?php foreach ($rate_values['RATECARD'] as $rate_value): ?>
          <th>
            <?php echo'Greater than ' . $rate_value->startUnit . (isset($rate_value->endUnit) ? '<br>Up to ' . $rate_value->endUnit : ''); ?>
          </th>
          <?php endforeach; ?>
        </tr>
      </thead>
      <tbody>
          <?php if ($rate_plan->isGroupPlan()): ?>
          <?php foreach ($rate_plan->getRatePlanDetailsByProduct($rate_plan_detail->product) as $rate_plan_detail): ?>
            <?php $rate_values = _devconnect_monetization_sort_rate_plan_rates($rate_plan_detail->ratePlanRates, TRUE); ?>
            <tr>
              <td><?php echo $rate_plan_detail->getOrganization()->getDescription(); ?></td>
              <td><?php echo Country::getCountryName($rate_plan_detail->getOrganization()->getCountry()); ?></td>
              <td><?php echo $rate_plan_detail->currency->name; ?></td>
              <?php if (strlen($rate_plan_detail->revenueType)): ?>
              <td><?php echo ucwords(strtolower($rate_plan_detail->revenueType)); ?></td>
              <?php endif; ?>
              <?php if (isset($rate_values['REVSHARE']) && $rate_values['REVSHARE']->revshare > 0): ?>
                <?php echo $rate_value->revshare; ?>
              <?php endif; ?>
              <?php foreach ($rate_values['RATECARD'] as $rate_value): ?>
              <td><?php echo $rate_value->rate; ?></td>
              <?php endforeach; ?>
            </tr>
          <?php endforeach; ?>
        <?php else: ?>
        <tr>
          <td><?php echo $rate_plan_detail->getOrganization()->getDescription(); ?></td>
          <td><?php echo Country::getCountryName($rate_plan_detail->getOrganization()->getCountry()); ?></td>
          <td><?php echo $rate_plan_detail->currency->name; ?></td>
          <?php if (strlen($rate_plan_detail->revenueType)): ?>
          <td><?php echo ucwords(strtolower($rate_plan_detail->revenueType)); ?></td>
          <?php endif; ?>
          <?php if (isset($rate_values['REVSHARE']) && $rate_values['REVSHARE']->revshare > 0): ?>
            <?php echo $rate_value->revshare; ?>
          <?php endif; ?>
          <?php foreach ($rate_values['RATECARD'] as $rate_value): ?>
          <td><?php echo $rate_value->rate; ?></td>
          <?php endforeach; ?>
        </tr>
        <?php endif; ?>
      </tbody>
    </table>
  <?php endif; ?>
<?php endif; ?>