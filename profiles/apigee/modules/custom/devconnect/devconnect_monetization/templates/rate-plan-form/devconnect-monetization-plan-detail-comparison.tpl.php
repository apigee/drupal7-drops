<?php
  $submitted_plan_id = isset($_REQUEST['plan_options']) ? $_REQUEST['plan_options'] : NULL;
?>
<div class="plan-details-comparison">
  <h3><?php print t('Plan Details & Comparison'); ?></h3>
  <div class="tabbable">
    <ul class="nav nav-tabs">
      <?php foreach ($rate_plans as $rate_plan): ?>
        <li<?php print $submitted_plan_id ==  $rate_plan->getId() ? ' class="active"' : ''?>>
          <a class="plan-tab" href="#tab_<?php echo preg_replace('/[^a-z0-9_-]/i', '_', $rate_plan->getId()); ?>" data-toggle="tab" plan-id="<?php print $rate_plan->getId(); ?>">
            <?php echo $rate_plan->getDisplayName(); ?>
            <?php if ($rate_plan->getChildRatePlan() != NULL): ?>
              <span class="future-plan">F</span>
            <?php endif; ?>
          </a>
        </li>
      <?php endforeach; ?>
    </ul>
    <div class="tab-content plans-comparison">
      <?php foreach ($rate_plans as $rate_plan): ?>
        <?php 
        $rate_plan_details = $rate_plan->getRatePlanDetails();
        // The getRatePlanDetails() comes back as an empty array sometimes,
        // so you need to make sure it is not an empty array before using list()
        if(!empty($rate_plan_details)){list($rate_plan_detail) = $rate_plan_details;} 
        ?>
        <div id="tab_<?php echo preg_replace('/[^a-z0-9_-]/i', '_', $rate_plan->getId()); ?>" class="tab-pane<?php print $submitted_plan_id ==  $rate_plan->getId() ? ' active' : ''?>">
          <?php // Start Of Future Plan 1 ?>
          <?php if ($rate_plan->getChildRatePlan() != NULL): ?>

          <p style="color: #666;">
            <?php print t('This plan has a new version effective @start_date. Toggle below to see the future rate plan.', array('@start_date' => $rate_plan->getChildRatePlan()->getStartDateTime()->format('m-d-Y'))); ?>
          </p>
          <div class="tabbable">
            <ul class="nav nav-tabs">
              <li class="active"><a plan-version="current" href="#current_<?php echo preg_replace('/[^a-z0-9_-]/i', '_', $rate_plan->getId()); ?>" data-toggle="tab">Current</a></li>
              <li><a plan-version="future" href="#future_<?php echo preg_replace('/[^a-z0-9_-]/i', '_', $rate_plan->getId()); ?>" data-toggle="tab">Future</a></li>
            </ul>
            <div class="tab-content">
              <div id="current_<?php echo preg_replace('/[^a-z0-9_-]/i', '_', $rate_plan->getId()); ?>" class="tab-pane active">
                <?php // Start Of Future Plan 1 ?>
                <?php endif; ?>
                <?php if ($rate_plan->getContractDuration() > 0 || $rate_plan->getSetUpFee() > 0 || $rate_plan->getRecurringFee() > 0 || $rate_plan->getEarlyTerminationFee() > 0): ?>
                  <table>
                    <thead>
                    <tr>
                        <?php if ($rate_plan->getContractDuration() > 0): ?><th><?php print t('Renewal Period'); ?> </th><?php endif; ?>
                        <?php if ($rate_plan->getSetUpFee() > 0): ?><th><?php print t('Set Up Fee'); ?></th><?php endif; ?>
                        <?php if ($rate_plan->getRecurringFee() > 0): ?><th><?php print t('Recurring Fees'); ?></th><?php endif; ?>
                        <?php if ($rate_plan->getEarlyTerminationFee() > 0): ?><th><?php print t('Early Termination Fee'); ?></th><?php endif; ?>
                    </tr>
                    </thead>
                    <tbody>
                    <tr>
                      <?php if ($rate_plan->getContractDuration() > 0): ?><td><?php echo $rate_plan->getContractDuration() .'&nbsp;' . strtolower($rate_plan->getContractDurationType()) . ($rate_plan->getContractDuration() > 1 ? 's' : ''); ?></td><?php endif; ?>
                      <?php if ($rate_plan->getSetUpFee() > 0): ?><td><?php echo $rate_plan->getCurrency()->getName() . '&nbsp;' . sprintf('%.2f', $rate_plan->getSetUpFee()); ?></td><?php endif; ?>
                      <?php if ($rate_plan->getRecurringFee() > 0): ?><td><?php echo _devconnect_monetization_get_frequency_fee_text($rate_plan); ?></td><?php endif; ?>
                      <?php if ($rate_plan->getEarlyTerminationFee() > 0): ?><td><?php echo $rate_plan->getCurrency()->getName() . '&nbsp;' . $rate_plan->getEarlyTerminationFee(); ?></td><?php endif; ?>
                    </tr>
                    </tbody>
                  </table>
                <?php endif; ?>
                <?php $rate_plant_free_quantity_message = _devconnect_monetization_get_free_quantity_text_for_rate_plan_level($rate_plan); ?>
                <?php if ($rate_plant_free_quantity_message != NULL): ?>
                  <br>
                  <strong>Free Quantity:&nbsp;</strong><?php print $rate_plant_free_quantity_message; ?>
                  <br />
                  <br />
                <?php endif; ?>
                <?php print devconnect_monetization_build_rate_plan_details_output($rate_plan, isset($product_list) ? $product_list : NULL); ?>
                <?php // End Of Future Plan 1 ?>
                <?php if ($rate_plan->getChildRatePlan() != NULL): ?>
              </div>
              <div id="future_<?php echo preg_replace('/[^a-z0-9_-]/i', '_', $rate_plan->getId()); ?>" class="tab-pane">
                <?php $rate_plan = $rate_plan->getChildRatePlan(); ?>
                <?php if ($rate_plan->getContractDuration() > 0 || $rate_plan->getSetUpFee() > 0 || $rate_plan->getRecurringFee() > 0 || $rate_plan->getEarlyTerminationFee() > 0): ?>
                  <table>
                    <thead>
                    <tr>
                        <?php if ($rate_plan->getContractDuration() > 0): ?><th><?php print t('Renewal Period'); ?></th><?php endif; ?>
                        <?php if ($rate_plan->getSetUpFee() > 0): ?><th><?php print t('Set Up Fee'); ?></th><?php endif; ?>
                        <?php if ($rate_plan->getRecurringFee() > 0): ?><th><?php print t('Recurring Fees'); ?></th><?php endif; ?>
                        <?php if ($rate_plan->getEarlyTerminationFee() > 0): ?><th><?php print t('Early Termination Fee'); ?></th><?php endif; ?>
                    </tr>
                    </thead>
                    <tbody>
                    <tr>
                      <?php if ($rate_plan->getContractDuration() > 0): ?><td><?php echo $rate_plan->getContractDuration() .'&nbsp;' . strtolower($rate_plan->getContractDurationType()) . ($rate_plan->getContractDuration() > 1 ? 's' : ''); ?></td><?php endif; ?>
                      <?php if ($rate_plan->getSetUpFee() > 0): ?><td><?php echo $rate_plan->getCurrency()->getName() . '&nbsp;' . sprintf('%.2f', $rate_plan->getSetUpFee()); ?></td><?php endif; ?>
                      <?php if ($rate_plan->getRecurringFee() > 0): ?><td><?php echo _devconnect_monetization_get_frequency_fee_text($rate_plan); ?></td><?php endif; ?>
                      <?php if ($rate_plan->getEarlyTerminationFee() > 0): ?><td><?php echo $rate_plan->getCurrency()->getName() . '&nbsp;' . $rate_plan->getEarlyTerminationFee(); ?></td><?php endif; ?>
                    </tr>
                    </tbody>
                  </table>
                <?php endif; ?>
                <?php $rate_plant_free_quantity_message = _devconnect_monetization_get_free_quantity_text_for_rate_plan_level($rate_plan); ?>
                <?php if ($rate_plant_free_quantity_message != NULL): ?>
                  <br>
                  <strong>Free Quantity:&nbsp;</strong><?php print $rate_plant_free_quantity_message; ?>
                  <br />
                  <br />
                <?php endif; ?>
                <?php print devconnect_monetization_build_rate_plan_details_output($rate_plan, isset($product_list) ? $product_list : NULL); ?>
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
