<?php

use Apigee\Util\Debugger;
use Apigee\Mint\DataStructures\RatePlanDetail;
use Apigee\Mint\Types\RatePlanRateType;
use Apigee\Mint\Types\MeteringType;
use Apigee\Mint\Types\Country;

?>

<div>
  <a href="/users/me/monetization/packages" class="back-to-catalog">Back to Catalog</a>
  <?php if ($active_plan_name !== NULL): ?>
  <span class="active-plan well"><strong>Active Plan:</strong>&nbsp;<?php echo $active_plan_name; ?></span>
  <?php endif; ?>
  <h3>Package Name:&nbsp;<?php echo $package->getDisplayName(); ?></h3>
  <h3>Products:<br><?php echo $product_list_title; ?></h3>
  <?php if ($has_limits): ?>
    <h3>Limits: </h3>
    <?php if (isset($package_limits)): ?>
      Package Limits:<br>
      <?php echo $package_limits; ?><br>
    <?php endif; ?>
    <?php print $limits_text; ?>
  <?php endif; ?>
</div>
<br>

<?php // Render price points?>
<?php foreach ($package_products as $product): ?>
  <?php $price_points = $product->getPricePoints(); ?>
  <?php if (!empty($price_points)): ?>
    <strong>Price Points for <?php echo $product->getDisplayName(); ?></strong>
    <br>
    <br>
    <table>
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
            <td><?php echo Country::getCountryName($price_point->getOrganization()->getCountry()); ?></td>
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

<?php if (!$has_many_rate_plans): ?>
<div>
  <?php if (!isset($rate_plan)) $rate_plan = $rate_plans[0]; ?>
  <h3>Plan Details</h3>
  <strong><?php echo $rate_plan->getDisplayName(); ?></strong>
  <?php // Start Future Product Specific 1 ?>
  <?php if ($rate_plan->getChildRatePlan() != NULL): ?>
  <p style="color: #666;">This plan has a new version effective <?php print substr($rate_plan->getChildRatePlan()->getStartDate(), 0, 10); ?>. Toggle below to see the future rate plan.</p>
  <div class="tabbable">
    <ul class="nav nav-pills">
      <li class="active"><a href="#current" data-toggle="tab">Current</a></li>
      <li class=""><a href="#future" data-toggle="tab">Future</a></li>
    </ul>
    <div class="tab-content">
      <div id="current" class="tab-pane active">
  <?php // Start Future Product Specific 1 ?>
  <?php endif; ?>
        <br>
        <?php if ($rate_plan->getContractDuration() > 0 || $rate_plan->getSetUpFee() > 0 || $rate_plan->getRecurringFee() > 0 || $rate_plan->getEarlyTerminationFee() > 0): ?>
        <table>
          <thead>
            <tr>
              <?php if ($rate_plan->getContractDuration() > 0): ?><th>Renewal Period</th><?php endif; ?>
              <?php if ($rate_plan->getSetUpFee() > 0): ?><th>Set Up Fee</th><?php endif; ?>
              <?php if ($rate_plan->getRecurringFee() > 0): ?><th>Recurring Fees</th><?php endif; ?>
              <?php if ($rate_plan->getEarlyTerminationFee() > 0): ?><th>Early Termination Fee</th><?php endif; ?>
            </tr>
          </thead>
          <tbody>
            <tr>
              <?php if ($rate_plan->getContractDuration() > 0): ?><td><?php echo $rate_plan->getContractDuration() . '&nbsp;' . strtolower($rate_plan->getContractDurationType()) . ($rate_plan->getContractDuration() > 1 ? 's' : ''); ?></td><?php  endif; ?>
              <?php if ($rate_plan->getSetUpFee() > 0): ?><td><?php echo $rate_plan->getCurrency()->name . '&nbsp;' . sprintf('%.2f', $rate_plan->getSetUpFee()); ?></td><?php  endif; ?>
              <?php if ($rate_plan->getRecurringFee() > 0): ?><td><?php echo _devconnect_monetization_get_frequency_fee_text($rate_plan); ?></td><?php endif; ?>
              <?php if ($rate_plan->getEarlyTerminationFee() > 0): ?><td><?php echo $rate_plan->getCurrency()->name . '&nbsp;' . $rate_plan->getEarlyTerminationFee(); ?></td><?php endif; ?>
            </tr>
          </tbody>
        </table>
        <?php endif; ?>
        <?php foreach ($rate_plan->getRatePlanDetails() as $rate_plan_detail): ?>
          <?php print theme('devconnect_monetization_product_detail', array('rate_plan' => $rate_plan, 'rate_plan_detail' => $rate_plan_detail, 'product_list' => $product_list)); ?>
          <?php if ($rate_plan->isGroupPlan()) { break; } ?>
        <?php endforeach; ?>
        <div class="purchase-plan well">
          <?php if ($rate_plan->getId() != $active_rate_plan_id || array_key_exists($rate_plan->getId(), $plans_dates['can_purchase'])): ?>
            <?php if (!isset($prevent_from_purchase_message)): ?>
              <?php if (in_array($rate_plan->getId(), $plans_accepted_and_ended_in_future)): ?>
                You already have a future plan for this, please delete the future plan if you want to start this plan on a different date.
              <?php else: ?>
                <?php $form = drupal_get_form('_devconnect_monetization_purchase_plan_form', $rate_plan, FALSE); ?>
                <?php echo drupal_render($form); ?>
              <?php endif; ?>
            <?php else: ?>
              <?php print $prevent_from_purchase_message; ?>
            <?php endif; ?>
          <?php else: ?>
            <?php $form = drupal_get_form('_devconnect_monetization_end_plan_form', $rate_plan, FALSE); ?>
            <?php echo drupal_render($form); ?>
          <?php endif; ?>
        </div>
<?php // End Future Plan Product Specific Plan 1 ?>
<?php if ($rate_plan->getChildRatePlan() != NULL): ?>
      </div>
      <div id="future" class="tab-pane">
        <br>
        <?php $rate_plan = $rate_plan->getChildRatePlan(); ?>
        <?php list($rate_plan_rates) = $rate_plan_detail->ratePlanRates; ?>
        <?php if ($rate_plan->getContractDuration() > 0 || $rate_plan->getSetUpFee() > 0 || $rate_plan->getRecurringFee() > 0 || $rate_plan->getEarlyTerminationFee() > 0): ?>
        <table>
          <thead>
            <tr>
              <?php if ($rate_plan->getContractDuration() > 0): ?><th>Renewal Period</th><?php endif; ?>
              <?php if ($rate_plan->getSetUpFee() > 0): ?><th>Set Up Fee</th><?php endif; ?>
              <?php if ($rate_plan->getRecurringFee() > 0): ?><th>Recurring Fees</th><?php endif; ?>
              <?php if ($rate_plan->getEarlyTerminationFee() > 0): ?><th>Early Termination Fee</th><?php endif; ?>
            </tr>
          </thead>
          <tbody>
            <tr>
              <?php if ($rate_plan->getContractDuration() > 0): ?><td><?php echo $rate_plan->getContractDuration() . '&nbsp;' . strtolower($rate_plan->getContractDurationType()) . ($rate_plan->getContractDuration() > 1 ? 's' : ''); ?></td><?php  endif; ?>
              <?php if ($rate_plan->getSetUpFee() > 0): ?><td><?php echo $rate_plan->getCurrency()->name . '&nbsp;' . sprintf('%.2f', $rate_plan->getSetUpFee()); ?></td><?php  endif; ?>
              <?php if ($rate_plan->getRecurringFee() > 0): ?><td><?php echo _devconnect_monetization_get_frequency_fee_text($rate_plan); ?></td><?php endif; ?>
              <?php if ($rate_plan->getEarlyTerminationFee() > 0): ?><td><?php echo $rate_plan->getCurrency()->name . '&nbsp;' . $rate_plan->getEarlyTerminationFee(); ?></td><?php endif; ?>
            </tr>
          </tbody>
        </table>
        <?php endif; ?>
        <?php foreach ($rate_plan->getRatePlanDetails() as $rate_plan_detail): ?>
          <?php echo theme('devconnect_monetization_product_detail', array('rate_plan' => $rate_plan, 'rate_plan_detail' => $rate_plan_detail, 'product_list' => $product_list)); ?>
          <?php if ($rate_plan->isGroupPlan()) { break; } ?>
        <?php endforeach; ?>
      </div>
    </div>
  </div>
<?php //End Future Plan Product Specific Plan 1 ?>
<?php endif; ?>

</div>
<?php else: ?>
<div class="plan-details-comparison">
  <h3>Plan Details &amp; Comparision</h3>
  <div class="tabbable">
     <ul class="nav nav-tabs">
       <?php foreach ($rate_plans as $rate_plan): ?>
         <?php $reate_plan_name = $rate_plan->getDisplayName() . ($rate_plan->getChildRatePlan() == NULL ? '' : ' <span class="future-plan">F</span>'); ?>
         <?php if ($active_tab === TRUE || $active_tab == $rate_plan->getId()): ?>
         <li class="active">
           <a href="#tab_<?php echo preg_replace('/[^a-z0-9_-]/i', '_', $rate_plan->getId()); ?>" data-toggle="tab"><?php echo $reate_plan_name; ?></a>
           <input type="hidden" name="rate_plan_id" value="<?php echo $rate_plan->getId(); ?>">
         </li>
           <?php $active_content = $active_tab; ?>
           <?php $active_tab = FALSE; ?>
         <?php else: ?>
         <li>
           <a href="#tab_<?php echo preg_replace('/[^a-z0-9_-]/i', '_', $rate_plan->getId()); ?>" data-toggle="tab"><?php echo $reate_plan_name; ?></a>
           <input type="hidden" name="rate_plan_id" value="<?php echo $rate_plan->getId(); ?>">
         </li>
         <?php endif; ?>
       <?php endforeach; ?>
     </ul>
     <div class="tab-content">
       <?php foreach ($rate_plans as $rate_plan): ?>
         <?php list($rate_plan_detail) = $rate_plan->getRatePlanDetails(); ?>
         <div id="tab_<?php echo preg_replace('/[^a-z0-9_-]/i', '_', $rate_plan->getId()); ?>" class="tab-pane<?php if ($active_content === TRUE || $active_content == $rate_plan->getId()) { echo ' active'; $active_content = FALSE; } ?>">
         <?php // Start Of Future Plan 1 ?>
         <?php if ($rate_plan->getChildRatePlan() != NULL): ?>

            <p style="color: #666;">This plan has a new version effective <?php print substr($rate_plan->getChildRatePlan()->getStartDate(), 0, 10); ?>. Toggle below to see the future rate plan.</p>
            <div class="tabbable">
              <ul class="nav nav-pills">
                <li class="active"><a href="#current_<?php echo preg_replace('/[^a-z0-9_-]/i', '_', $rate_plan->getId()); ?>" data-toggle="tab">Current</a></li>
                <li class=""><a href="#future_<?php echo preg_replace('/[^a-z0-9_-]/i', '_', $rate_plan->getId()); ?>" data-toggle="tab">Future</a></li>
              </ul>
              <div class="tab-content">
                <div id="current_<?php echo preg_replace('/[^a-z0-9_-]/i', '_', $rate_plan->getId()); ?>" class="tab-pane active">
         <?php // Start Of Future Plan 1 ?>
         <?php endif; ?>
                <?php if ($rate_plan->getContractDuration() > 0 || $rate_plan->getSetUpFee() > 0 || $rate_plan->getRecurringFee() > 0 || $rate_plan->getEarlyTerminationFee() > 0): ?>
                <table>
                  <thead>
                    <tr>
                      <?php if ($rate_plan->getContractDuration() > 0): ?><th>Renewal Period</th><?php endif; ?>
                      <?php if ($rate_plan->getSetUpFee() > 0): ?><th>Set Up Fee</th><?php endif; ?>
                      <?php if ($rate_plan->getRecurringFee() > 0): ?><th>Recurring Fees</th><?php endif; ?>
                      <?php if ($rate_plan->getEarlyTerminationFee() > 0): ?><th>Early Termination Fee</th><?php endif; ?>
                    </tr>
                  </thead>
                  <tbody>
                    <tr>
                      <?php if ($rate_plan->getContractDuration() > 0): ?><td><?php echo $rate_plan->getContractDuration() .'&nbsp;' . strtolower($rate_plan->getContractDurationType()) . ($rate_plan->getContractDuration() > 1 ? 's' : ''); ?></td><?php endif; ?>
                      <?php if ($rate_plan->getSetUpFee() > 0): ?><td><?php echo $rate_plan->getCurrency()->name . '&nbsp;' . sprintf('%.2f', $rate_plan->getSetUpFee()); ?></td><?php endif; ?>
                      <?php if ($rate_plan->getRecurringFee() > 0): ?><td><?php echo _devconnect_monetization_get_frequency_fee_text($rate_plan); ?></td><?php endif; ?>
                      <?php if ($rate_plan->getEarlyTerminationFee() > 0): ?><td><?php echo $rate_plan->getCurrency()->name . '&nbsp;' . $rate_plan->getEarlyTerminationFee(); ?></td><?php endif; ?>
                    </tr>
                  </tbody>
                </table>
                <?php endif; ?>
                <?php $rate_plant_free_quantity_message = _devconnect_monetization_get_free_quantity_text_for_rate_plan_level($rate_plan); ?>
                <?php if ($rate_plant_free_quantity_message != NULL): ?>
                  <br>
                  <strong>Free Quantity:&nbsp;</strong><?php print $rate_plant_free_quantity_message; ?>
                  <br>
                  <br>
                <?php endif; ?>
                <?php $displayed_products = array(); ?>
                <?php foreach ($rate_plan->getRatePlanDetails() as $rate_plan_detail): ?>
                  <?php if (!isset($rate_plan_detail->product)): ?>
                    <?php echo theme('devconnect_monetization_product_detail', array('rate_plan' => $rate_plan, 'rate_plan_detail' => $rate_plan_detail, 'product_list' => $product_list)); ?>
                    <?php if ($rate_plan->isGroupPlan()) { break; } ?>
                  <?php elseif (!in_array($rate_plan_detail->product->getId(), $displayed_products)): ?>
                    <?php echo theme('devconnect_monetization_product_detail', array('rate_plan' => $rate_plan, 'rate_plan_detail' => $rate_plan_detail, 'product_list' => $product_list)); ?>
                    <?php $displayed_products[] = $rate_plan_detail->product->getId(); ?>
                  <?php  endif; ?>
                <?php endforeach; ?>
                <br><br>
                <div class="purchase-plan well">
                  <?php if (array_key_exists($rate_plan->getId(), $plans_dates['can_purchase'])): ?>
                    <?php if (!isset($prevent_from_purchase_message)): ?>
                      <?php if (in_array($rate_plan->getId(), $plans_accepted_and_ended_in_future)): ?>
                        You already have a future plan for this, please delete the future plan if you want to start this plan on a different date.
                      <?php else: ?>
                        <?php $form = drupal_get_form('_devconnect_monetization_purchase_plan_form', $rate_plan, TRUE); ?>
                        <?php echo drupal_render($form); ?>
                      <?php endif; ?>
                    <?php else: ?>
                      <?php print $prevent_from_purchase_message; ?>
                    <?php endif; ?>
                  <?php else: ?>
                    <?php $form = drupal_get_form('_devconnect_monetization_end_plan_form', $rate_plan, TRUE); ?>
                    <?php echo drupal_render($form); ?>
                  <?php endif; ?>
                </div>
         <?php // End Of Future Plan 1 ?>
         <?php if ($rate_plan->getChildRatePlan() != NULL): ?>
                  </div>
                  <div id="future_<?php echo preg_replace('/[^a-z0-9_-]/i', '_', $rate_plan->getId()); ?>" class="tab-pane">
                    <?php $rate_plan = $rate_plan->getChildRatePlan(); ?>
                    <?php list($rate_plan_rates) = $rate_plan_detail->ratePlanRates; ?>
                    <?php if ($rate_plan->getContractDuration() > 0 || $rate_plan->getSetUpFee() > 0 || $rate_plan->getRecurringFee() > 0 || $rate_plan->getEarlyTerminationFee() > 0): ?>
                    <table>
                      <thead>
                        <tr>
                          <?php if ($rate_plan->getContractDuration() > 0): ?><th>Renewal Period</th><?php endif; ?>
                          <?php if ($rate_plan->getSetUpFee() > 0): ?><th>Set Up Fee</th><?php endif; ?>
                          <?php if ($rate_plan->getRecurringFee() > 0): ?><th>Recurring Fees</th><?php endif; ?>
                          <?php if ($rate_plan->getEarlyTerminationFee() > 0): ?><th>Early Termination Fee</th><?php endif; ?>
                        </tr>
                      </thead>
                      <tbody>
                        <tr>
                          <?php if ($rate_plan->getContractDuration() > 0): ?><td><?php echo $rate_plan->getContractDuration() .'&nbsp;' . strtolower($rate_plan->getContractDurationType()) . ($rate_plan->getContractDuration() > 1 ? 's' : ''); ?></td><?php endif; ?>
                          <?php if ($rate_plan->getSetUpFee() > 0): ?><td><?php echo $rate_plan->getCurrency()->name . '&nbsp;' . sprintf('%.2f', $rate_plan->getSetUpFee()); ?></td><?php endif; ?>
                          <?php if ($rate_plan->getRecurringFee() > 0): ?><td><?php echo _devconnect_monetization_get_frequency_fee_text($rate_plan); ?></td><?php endif; ?>
                          <?php if ($rate_plan->getEarlyTerminationFee() > 0): ?><td><?php echo $rate_plan->getCurrency()->name . '&nbsp;' . $rate_plan->getEarlyTerminationFee(); ?></td><?php endif; ?>
                        </tr>
                      </tbody>
                    </table>
                    <?php endif; ?>
                    <?php $rate_plant_free_quantity_message = _devconnect_monetization_get_free_quantity_text_for_rate_plan_level($rate_plan); ?>
                    <?php if ($rate_plant_free_quantity_message != NULL): ?>
                      <br>
                      <strong>Free Quantity:&nbsp;</strong><?php print $rate_plant_free_quantity_message; ?>
                      <br>
                      <br>
                    <?php endif; ?>

                    <?php $displayed_products = array(); ?>
                    <?php foreach ($rate_plan->getRatePlanDetails() as $rate_plan_detail): ?>
                      <?php if (!isset($rate_plan_detail->product)): ?>
                        <?php echo theme('devconnect_monetization_product_detail', array('rate_plan' => $rate_plan, 'rate_plan_detail' => $rate_plan_detail, 'product_list' => $product_list)); ?>
                        <?php if ($rate_plan->isGroupPlan()) { break; } ?>
                      <?php elseif (!in_array($rate_plan_detail->product->getId(), $displayed_products)): ?>
                        <?php echo theme('devconnect_monetization_product_detail', array('rate_plan' => $rate_plan, 'rate_plan_detail' => $rate_plan_detail, 'product_list' => $product_list)); ?>
                        <?php $displayed_products[] = $rate_plan_detail->product->getId(); ?>
                      <?php  endif; ?>
                    <?php endforeach; ?>
                  </div>
                </div>
            </div>
         <?php // End Of Future Plan 1 ?>
         <?php endif; ?>
         </div>
       <?php endforeach; ?>
    </div>
  </div>
  <br />
</div>
<?php endif; ?>

<!-- Purchase Top Up Modal -->
<div id="topUpPurchase" class="modal hide fade" tabindex="-1" role="dialog" aria-labelledby="topUpLabel" aria-hidden="true">
  <div class="modal-header">
    <button type="button" class="close" data-dismiss="modal" aria-hidden="true">×</button>
    <h3 id="topUpLabel">Insuffient Prepaid Balance <span id="currency_title"></span></h3>
    <div id="topup_alert" class="alert hide">
      <strong>Warning!</strong>&nbsp;The Amount to Top Up must be a valid number and bigger than zero.
    </div>
    <div id="currency_alert" class="alert hide">
      <strong>Warning!</strong>&nbsp;You must select currency.
    </div>
  </div>
  <div class="modal-body">
    <?php print isset($forms['top_up_form']) ? $forms['top_up_form'] : ''; ?>
    <p>You have insuffient funds to purchase this plan.</p>
    <p>To top up your prepaid balance you will be taken to World Pay to process your payment.<br>
      Please enter the desired balance amount below.</p>
      <div style="margin-bottom: 10px;">
        <span class="topup-modal-label">Current Balance:</span>
        <span id="topUpCurrentBalance" class="topup-modal-value"></span>&nbsp;
        <span id="topUpCurrentBalanceCurrency" class="topup-modal-value"></span>
      </div>
      <div style="margin-bottom: 10px;">
        <span class="topup-modal-label">Amount to Top Up:</span>
        <span class="topup-modal-value">
          <input id="top-up-balance-input" type="text" placeholder="enter an amount" onkeyup="javascript: restrictRegexOnChangeEvent(this, /^[1-9][0-9]*((\.[0-9]{1,2})|\.)?$/, '#valid_top_up');">
          <input id="valid_top_up" type="hidden" />
        </span>
      </div>
      <div>
        <span class="topup-modal-label">New Balance:</span>
        <span id="newBalance" class="topup-modal-value"></span>&nbsp;
        <span id="newBalanceCurrency" class="topup-modal-value"></span>
      </div>
  </div>
  <div class="modal-footer">
    <a href="javascript: validateBalanceToTopUp();" class="btn btn-primary">Proceed to World Pay</a>
    <a class="btn" data-dismiss="modal" aria-hidden="true">Cancel</a>
  </div>
</div>

