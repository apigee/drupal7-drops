<?php

use Apigee\Mint\Types\RatePlanRateType;
use Apigee\Mint\Types\MeteringType;
use Apigee\Mint\Types\Country;

?>
<?php $free_quantity_message = _devconnect_monetization_get_free_quantity_text($rate_plan_detail, $rate_plan); ?>
<?php $product = $rate_plan_detail->getProduct(); ?>
<?php if ($rate_plan_detail->getType() == RatePlanRateType::REVSHARE): ?>
  <?php if (isset($free_quantity_message)): ?>
    <span><strong><?php print t('Free Quantity:'); ?></strong> <?php echo $free_quantity_message; ?></span>
  <?php endif; ?>
  <?php if(isset($product)) : ?>
    <strong><?php print t('Product:'); ?></strong> <?php echo $product->getDisplayName(); ?>
  <?php endif; ?>
  <?php $rate_plan_rates = $rate_plan_detail->getRatePlanRates(); ?>
  <?php $is_band = count($rate_plan_rates) > 1; ?>
  <?php $row_span = 'rowspan="2"'; $col_span = 'colspan="'. count($rate_plan_rates) .'"'; ?>
  <div class="table-responsive">
    <table class="table table-bordered">
      <thead>
      <tr>
        <?php $rate_values = _devconnect_monetization_sort_rate_plan_rates($rate_plan_detail->getRatePlanRates()); ?>
        <?php if (count($rate_values) == 1): ?>
          <th><?php print t('Operator'); ?></th>
          <th><?php print t('Country'); ?></th>
          <th><?php print t('Currency'); ?></th>
          <?php if (strlen($rate_plan_detail->getRevenueType())): ?>
            <th><?php print t('Pricing Type'); ?></th>
          <?php endif; ?>
          <th><?php print t('Rev Share %'); ?></th>
        <?php else: ?>
        <th rowspan="2"><?php print t('Operator'); ?></th>
        <th rowspan="2"><?php print t('Country'); ?></th>
        <th rowspan="2"><?php print t('Currency'); ?></th>
        <?php if (strlen($rate_plan_detail->getRevenueType())): ?>
          <th rowspan="2"><?php print t('Pricing Type'); ?></th>
        <?php endif; ?>
        <th colspan="<?php echo count($rate_values); ?>"><?php print t('Revenue Bands/Rev Share %'); ?></th>
      </tr>
      <tr>
        <?php foreach ($rate_values as $rate_value): ?>
          <th>
            <?php echo t('Greater than') . ' ' . $rate_value->getStartUnit() . (is_numeric($rate_value->getEndUnit()) ? '<br />'. t('Up to'). ' ' . $rate_value->getEndUnit() : ''); ?>
          </th>
        <?php endforeach; ?>
        <?php  endif; ?>
      </tr>
      </thead>
      <tbody>
      <?php if ($rate_plan->isGroupPlan()): ?>
        <?php foreach ($rate_plan->getRatePlanDetailsByProduct($rate_plan_detail->getProduct()) as $rate_plan_detail): ?>
          <?php $rate_values = _devconnect_monetization_sort_rate_plan_rates($rate_plan_detail->getRatePlanRates()); ?>
          <tr>
            <td><?php echo $rate_plan_detail->getOrganization()->getDescription(); ?></td>
            <td><?php echo Country::getCountryName($rate_plan_detail->getOrganization()->getCountry()); ?></td>
            <td><?php echo $rate_plan_detail->getCurrency()->getName(); ?></td>
            <?php if (strlen($rate_plan_detail->getRevenueType())): ?>
              <td><?php echo ucwords(strtolower($rate_plan_detail->getRevenueType())); ?></td>
            <?php endif; ?>
            <?php foreach ($rate_values as $rate_value): ?>
              <td><?php echo $rate_value->getRevshare(); ?>&nbsp;%</td>
            <?php endforeach; ?>
          </tr>
        <?php endforeach; ?>
      <?php else: ?>
        <tr>
          <td><?php echo $rate_plan_detail->getOrganization()->getDescription(); ?></td>
          <td><?php echo Country::getCountryName($rate_plan_detail->getOrganization()->getCountry()); ?></td>
          <td><?php echo $rate_plan_detail->getCurrency()->getName(); ?></td>
          <?php if (strlen($rate_plan_detail->getRevenueType())): ?>
            <td><?php echo ucwords(strtolower($rate_plan_detail->getRevenueType())); ?></td>
          <?php endif; ?>
          <?php foreach ($rate_values as $rate_value): ?>
            <td><?php echo $rate_value->getRevshare(); ?>&nbsp;%</td>
          <?php endforeach; ?>
        </tr>
      <?php endif; ?>
      </tbody>
    </table>
  </div>
<?php else: // $rate_plan_detail->getType() === RatePlanRateType::RATECARD ?>
  <?php if ($rate_plan_detail->getMeteringType() == MeteringType::UNIT): ?>
    <br />
    <?php if (isset($product)): ?>
      <strong><?php print t('Product:'); ?></strong> <?php echo $product->getDisplayName(); ?>
    <?php else: ?>
      <?php if (isset($product_list)): ?>
      <strong><?php print t('Products:'); ?></strong> <?php echo str_replace(',', ' &amp;', $product_list);?>
      <?php endif; ?>
    <?php endif; ?>
    <br />
    <strong><?php print t('Rate Card is based on:');?></strong><?php echo _devconnect_monetization_get_rate_card($rate_plan_detail); ?>
    <?php if (isset($free_quantity_message)): ?>
      <br />
      <strong><?php print t('Free Quantity:'); ?></strong> <?php echo $free_quantity_message; ?>
    <?php else: ?>
      <br />
    <?php endif; ?>
    <br />
    <br />
    <div class="table-responsive">
      <table class="table table-bordered">
        <thead>
        <tr>
          <th><?php print t('Operator'); ?></th>
          <th><?php print t('Country'); ?></th>
          <th><?php print t('Currency'); ?></th>
          <?php if (strlen($rate_plan_detail->getRevenueType())): ?>
            <th><?php print t('Pricing Type'); ?></th>
          <?php endif; ?>
          <?php foreach($rate_plan_detail->getRatePlanRates() as $rate_plan_rate): ?>
            <?php if ($rate_plan_rate->getType() == RatePlanRateType::REVSHARE && $rate_plan_rate->getRevshare() > 0): ?>
              <th>Rev Share %</th>
            <?php endif; ?>
          <?php endforeach; ?>
          <?php foreach($rate_plan_detail->getRatePlanRates() as $rate_plan_rate): ?>
            <?php if ($rate_plan_rate->getType() == RatePlanRateType::RATECARD): ?>
              <th>Rate</th>
            <?php endif; ?>
          <?php endforeach; ?>
        </tr>
        </thead>
        <tbody>
        <?php if ($rate_plan->isGroupPlan()): ?>
          <?php foreach ($rate_plan->getRatePlanDetailsByProduct($rate_plan_detail->getProduct()) as $rate_plan_detail): ?>
            <?php $rate_values = _devconnect_monetization_sort_rate_plan_rates($rate_plan_detail->getRatePlanRates()); ?>
            <tr>
              <td><?php echo $rate_plan_detail->getOrganization()->getDescription(); ?></td>
              <td><?php echo Country::getCountryName($rate_plan_detail->getOrganization()->getCountry()); ?></td>
              <td><?php echo $rate_plan_detail->getCurrency()->getName(); ?></td>
              <?php if (strlen($rate_plan_detail->getRevenueType())): ?>
                <td><?php echo ucwords(strtolower($rate_plan_detail->getRevenueType())); ?></td>
              <?php endif; ?>
              <?php foreach($rate_plan_detail->getRatePlanRates() as $rate_plan_rate): ?>
                <?php if ($rate_plan_rate->getType() == RatePlanRateType::REVSHARE && $rate_plan_rate->getRevshare() > 0): ?>
                  <td><?php echo $rate_plan_rate->getRevshare(); ?>&nbsp;%</td>
                <?php endif; ?>
              <?php endforeach; ?>
              <?php foreach($rate_plan_detail->getRatePlanRates() as $rate_plan_rate): ?>
                <?php if ($rate_plan_rate->getType() == RatePlanRateType::RATECARD): ?>
                  <td><?php echo $rate_plan_rate->getRate(); ?></td>
                <?php endif; ?>
              <?php endforeach; ?>
            </tr>
          <?php endforeach; ?>
        <?php else: ?>
          <tr>
            <td><?php echo $rate_plan_detail->getOrganization()->getDescription(); ?></td>
            <td><?php echo Country::getCountryName($rate_plan_detail->getOrganization()->getCountry()); ?></td>
            <td><?php echo $rate_plan_detail->getCurrency()->getName(); ?></td>
            <?php if (strlen($rate_plan_detail->getRevenueType())): ?>
              <td><?php echo ucwords(strtolower($rate_plan_detail->getRevenueType())); ?></td>
            <?php endif; ?>
            <?php foreach($rate_plan_detail->getRatePlanRates() as $rate_plan_rate): ?>
              <?php if ($rate_plan_rate->getType() == RatePlanRateType::REVSHARE && $rate_plan_rate->getRevshare() > 0): ?>
                <td><?php echo $rate_plan_rate->getRevshare(); ?>&nbsp;%</td>
              <?php endif; ?>
            <?php endforeach; ?>
            <?php foreach($rate_plan_detail->getRatePlanRates() as $rate_plan_rate): ?>
              <?php if ($rate_plan_rate->getType() == RatePlanRateType::RATECARD): ?>
                <td><?php echo $rate_plan_rate->getRate(); ?></td>
              <?php endif; ?>
            <?php endforeach; ?>
          </tr>
        <?php endif; ?>
        </tbody>
      </table>
    </div>
  <?php elseif ($rate_plan_detail->getMeteringType() == MeteringType::VOLUME): ?>
    <br /><br />
    <?php if (isset($product)): ?>
      <strong><?php print t('Product:'); ?></strong> <?php echo $product->getDisplayName(); ?>
    <?php else: ?>
      <?php if (isset($product_list)): ?>
      <strong><?php print t('Products:'); ?></strong> <?php echo str_replace(',', ' &amp;', $product_list);?>
      <?php endif; ?>
    <?php endif; ?>
    <br />
    <strong><?php print t('Rate Card is based on:'); ?></strong> <?php echo _devconnect_monetization_get_rate_card($rate_plan_detail); ?>
    <br />
    <strong><?php print t('Volume Aggregation Basis:'); ?></strong> <?php echo $rate_plan_detail->getDuration() . ' ' . strtolower($rate_plan_detail->getDurationType()) . ($rate_plan_detail->getDuration() > 1 ? 's' : ''); ?>
    <?php if (isset($free_quantity_message)): ?>
      <br />
      <strong><?php print t('Free Quantity:'); ?></strong> <?php echo $free_quantity_message; ?>
    <?php endif; ?>
    <br />
    <br />
    <div class="table-responsive">
      <table class="table table-bordered">
        <thead>
        <tr>
          <?php $rate_values = _devconnect_monetization_sort_rate_plan_rates($rate_plan_detail->getRatePlanRates(), TRUE); ?>
          <th rowspan="2"><?php print t('Operator'); ?></th>
          <th rowspan="2"><?php print t('Country'); ?></th>
          <th rowspan="2"><?php print t('Currency'); ?></th>
          <?php if (strlen($rate_plan_detail->getRevenueType())): ?>
            <th rowspan="2"><?php print t('Pricing Type'); ?></th>
            <?php foreach ($rate_values['REVSHARE'] as $rate_value): ?>
              <th rowspan="2">
                <?php echo $rate_value->getType() == RatePlanRateType::REVSHARE ? 'Rev Share %<br />' : '' ?>
                <?php echo t('Greater than') . ' ' . $rate_value->getStartUnit() . (is_numeric($rate_value->getEndUnit()) ? '<br />' . t('Up to') . ' ' . $rate_value->getEndUnit() : ''); ?>
              </th>
            <?php endforeach; ?>
          <?php endif; ?>
          <th colspan="<?php echo count($rate_values['RATECARD']); ?>">Volume band</th>
        </tr>
        <tr>
          <?php foreach ($rate_values['RATECARD'] as $rate_value): ?>
            <th>
              <?php echo t('Greater than') . ' ' . $rate_value->getStartUnit() . (is_numeric($rate_value->getEndUnit()) ? '<br />' . t('Up to') . ' ' . $rate_value->getEndUnit() : ''); ?>
            </th>
          <?php endforeach; ?>
        </tr>
        </thead>
        <tbody>
        <?php if ($rate_plan->isGroupPlan()): ?>
          <?php foreach ($rate_plan->getRatePlanDetailsByProduct($rate_plan_detail->getProduct()) as $rate_plan_detail): ?>
            <?php $rate_values = _devconnect_monetization_sort_rate_plan_rates($rate_plan_detail->getRatePlanRates(), TRUE); ?>
            <tr>
              <td><?php echo $rate_plan_detail->getOrganization()->getDescription(); ?></td>
              <td><?php echo Country::getCountryName($rate_plan_detail->getOrganization()->getCountry()); ?></td>
              <td><?php echo $rate_plan_detail->getCurrency()->getName(); ?></td>
              <?php if (strlen($rate_plan_detail->getRevenueType())): ?>
                <td><?php echo ucwords(strtolower($rate_plan_detail->getRevenueType())); ?></td>
              <?php endif; ?>
              <?php foreach ($rate_values['REVSHARE'] as $rate_value): ?>
                <td><?php echo $rate_value->getRevshare(); ?> %</td>
              <?php endforeach; ?>
              <?php foreach ($rate_values['RATECARD'] as $rate_value): ?>
                <td><?php echo $rate_value->getRate(); ?></td>
              <?php endforeach; ?>
            </tr>
          <?php endforeach; ?>
        <?php else: ?>
          <tr>
            <td><?php echo $rate_plan_detail->getOrganization()->getDescription(); ?></td>
            <td><?php echo Country::getCountryName($rate_plan_detail->getOrganization()->getCountry()); ?></td>
            <td><?php echo $rate_plan_detail->getCurrency()->getName(); ?></td>
            <?php if (strlen($rate_plan_detail->getRevenueType())): ?>
              <td><?php echo ucwords(strtolower($rate_plan_detail->getRevenueType())); ?></td>
            <?php endif; ?>
            <?php (isset($rate_values['REVSHARE']))? $revshare_rate_values = $rate_values['REVSHARE'] : $revshare_rate_values = array();  ?>
            <?php foreach ($revshare_rate_values as $rate_value): ?>
              <td><?php echo $rate_value->getRevshare(); ?>&nbsp;%</td>
            <?php endforeach; ?>
            <?php (isset($rate_values['RATECARD']))? $ratecard_rate_values = $rate_values['RATECARD'] : $ratecard_rate_values = array();  ?>
            <?php foreach ($ratecard_rate_values as $rate_value): ?>
              <td><?php echo $rate_value->getRate(); ?></td>
            <?php endforeach; ?>
          </tr>
        <?php endif;?>
        </tbody>
      </table>
    </div>
  <?php elseif ($rate_plan_detail->getMeteringType() == MeteringType::STAIR_STEP): ?>
    <?php // Next statement and if condition commented out as per https://apigeesc.atlassian.net/browse/COMMERCE-508 ?>
    <?php //$rate_plant_free_quantity_message = _devconnect_monetization_get_free_quantity_text_for_rate_plan_level($rate_plan); ?>
    <?php //if ($rate_plant_free_quantity_message != NULL): ?>
    <?php if (!is_null($rate_plan_detail->getProduct())): ?>
      <strong><?php print t('Product:'); ?></strong> <?php echo $rate_plan_detail->getProduct()->getDisplayName(); ?>
      <br />
      <strong><?php print t('Rate Card is based on:'); ?></strong> <?php echo _devconnect_monetization_get_rate_card($rate_plan_detail); ?>
      <br />
    <?php endif; ?>
    <?php //endif; ?>
    <?php if (isset($free_quantity_message)): ?>
      <span><strong><?php print t('Free Quantity:'); ?></strong> <?php echo $free_quantity_message; ?></span><br/>
    <?php endif; ?>
    <span>
      <?php print t('Bundles expire in') . $rate_plan_detail->getDuration() . '&nbsp;' .  strtolower($rate_plan_detail->getDurationType()) . ($rate_plan_detail->getDuration() > 1 ? 's' : ''); ?>
    </span>
    <br />
    <br />
    <div class="table-responsive">
      <table class="table table-bordered">
        <thead>
        <tr>
          <?php $rate_values = _devconnect_monetization_sort_rate_plan_rates($rate_plan_detail->getRatePlanRates(), TRUE); ?>
          <th rowspan="2"><?php print t('Operator'); ?></th>
          <th rowspan="2"><?php print t('Country'); ?></th>
          <th rowspan="2"><?php print t('Currency'); ?></th>
          <?php if (strlen($rate_plan_detail->getRevenueType())): ?>
            <th rowspan="2"><?php print t('Pricing Type'); ?></th>
          <?php endif; ?>
          <?php if (isset($rate_values['REVSHARE']) && is_object($rate_values['REVSHARE']) && $rate_values['REVSHARE']->getRevshare() > 0): ?>
            <th rowspan="2"><?php print t('Rate'); ?></th>
          <?php endif; ?>
          <th colspan="<?php echo count($rate_values['RATECARD']); ?>"><?php print t('Bundles'); ?></th>
        </tr>
        <tr>
          <?php foreach ($rate_values['RATECARD'] as $rate_value): ?>
            <th>
              <?php echo t('Greater than') . ' ' . $rate_value->getStartUnit() . (is_numeric($rate_value->getEndUnit()) ? '<br />' . t('Up to') . ' ' . $rate_value->getEndUnit() : ''); ?>
            </th>
          <?php endforeach; ?>
        </tr>
        </thead>
        <tbody>
        <?php if ($rate_plan->isGroupPlan()): ?>
          <?php foreach ($rate_plan->getRatePlanDetailsByProduct($rate_plan_detail->getProduct()) as $rate_plan_detail): ?>
            <?php $rate_values = _devconnect_monetization_sort_rate_plan_rates($rate_plan_detail->getRatePlanRates(), TRUE); ?>
            <tr>
              <td><?php echo $rate_plan_detail->getOrganization()->getDescription(); ?></td>
              <td><?php echo Country::getCountryName($rate_plan_detail->getOrganization()->getCountry()); ?></td>
              <td><?php echo $rate_plan_detail->getCurrency()->getName(); ?></td>
              <?php if (strlen($rate_plan_detail->getRevenueType())): ?>
                <td><?php echo ucwords(strtolower($rate_plan_detail->getRevenueType())); ?></td>
              <?php endif; ?>
              <?php if (isset($rate_values['REVSHARE']) && is_object($rate_values['REVSHARE']) && $rate_values['REVSHARE']->getRevshare() > 0): ?>
                <?php echo $rate_value->getRevshare(); ?>
              <?php endif; ?>
              <?php foreach ($rate_values['RATECARD'] as $rate_value): ?>
                <td><?php echo $rate_value->getRate(); ?></td>
              <?php endforeach; ?>
            </tr>
          <?php endforeach; ?>
        <?php else: ?>
          <tr>
            <td><?php echo $rate_plan_detail->getOrganization()->getDescription(); ?></td>
            <td><?php echo Country::getCountryName($rate_plan_detail->getOrganization()->getCountry()); ?></td>
            <td><?php echo $rate_plan_detail->getCurrency()->getName(); ?></td>
            <?php if (strlen($rate_plan_detail->getRevenueType())): ?>
              <td><?php echo ucwords(strtolower($rate_plan_detail->getRevenueType())); ?></td>
            <?php endif; ?>
            <?php if (isset($rate_values['REVSHARE']) && is_object($rate_values['REVSHARE']) && $rate_values['REVSHARE']->getRevshare() > 0): ?>
              <?php echo $rate_value->getRevshare(); ?>
            <?php endif; ?>
            <?php foreach ($rate_values['RATECARD'] as $rate_value): ?>
              <td><?php echo $rate_value->getRate(); ?></td>
            <?php endforeach; ?>
          </tr>
        <?php endif; ?>
        </tbody>
      </table>
    </div>
  <?php endif; ?>
<?php endif; ?>
