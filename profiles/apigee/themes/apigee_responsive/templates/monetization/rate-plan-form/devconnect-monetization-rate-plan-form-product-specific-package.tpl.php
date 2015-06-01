<?php

?>

<div>
  <br/><h3><?php print t('Plan Details'); ?></h3>
  <hr>
  <h4><?php print t('Plan Name: @plan_name', array('@plan_name' => $rate_plan->getDisplayName())); ?></h4>
  <?php if ($rate_plan->getChildRatePlan() != NULL): // Start Future Product Specific 1 ?>
  <p style="color: #666;">
    <?php print t('This plan has a new version effective @start_date. Toggle below to see the future rate plan.', array('@start_date' => $rate_plan->getChildRatePlan()->getStartDateTime()->format('m-d-Y'))); ?>
  </p>
  <div class="tabbable">
    <ul class="nav nav-pills">
      <li class="active"><a href="#current" data-toggle="tab"><?php print t('Current'); ?></a></li>
      <li class=""><a href="#future" data-toggle="tab"><?php print t('Future'); ?></a></li>
    </ul>
    <div class="tab-content">
      <div id="current" class="tab-pane active">
        <?php // Start Future Product Specific 1 ?>
        <?php endif; ?>
        <br />
        <?php if ($rate_plan->getContractDuration() > 0 || $rate_plan->getSetUpFee() > 0 || $rate_plan->getRecurringFee() > 0 || $rate_plan->getEarlyTerminationFee() > 0): ?>
          <div class="table-responsive">
            <table class="table table-bordered">
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
                <?php if ($rate_plan->getContractDuration() > 0): ?><td><?php echo $rate_plan->getContractDuration() . '&nbsp;' . strtolower($rate_plan->getContractDurationType()) . ($rate_plan->getContractDuration() > 1 ? 's' : ''); ?></td><?php  endif; ?>
                <?php if ($rate_plan->getSetUpFee() > 0): ?><td><?php echo $rate_plan->getCurrency()->getName() . '&nbsp;' . sprintf('%.2f', $rate_plan->getSetUpFee()); ?></td><?php  endif; ?>
                <?php if ($rate_plan->getRecurringFee() > 0): ?><td><?php echo _devconnect_monetization_get_frequency_fee_text($rate_plan); ?></td><?php endif; ?>
                <?php if ($rate_plan->getEarlyTerminationFee() > 0): ?><td><?php echo $rate_plan->getCurrency()->getName() . '&nbsp;' . $rate_plan->getEarlyTerminationFee(); ?></td><?php endif; ?>
              </tr>
              </tbody>
            </table>
          </div>
        <?php endif; ?>
        <?php print devconnect_monetization_build_rate_plan_details_output($rate_plan, isset($product_list) ? $product_list : NULL); ?>
        <?php // End Future Plan Product Specific Plan 1 ?>
        <?php if ($rate_plan->getChildRatePlan() != NULL): ?>
      </div>
      <div id="future" class="tab-pane">
        <br />
        <?php $rate_plan = $rate_plan->getChildRatePlan(); ?>
        <?php if ($rate_plan->getContractDuration() > 0 || $rate_plan->getSetUpFee() > 0 || $rate_plan->getRecurringFee() > 0 || $rate_plan->getEarlyTerminationFee() > 0): ?>
          <div class="table-responsive">
            <table class="table table-bordered">
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
                <?php if ($rate_plan->getContractDuration() > 0): ?><td><?php echo $rate_plan->getContractDuration() . '&nbsp;' . strtolower($rate_plan->getContractDurationType()) . ($rate_plan->getContractDuration() > 1 ? 's' : ''); ?></td><?php  endif; ?>
                <?php if ($rate_plan->getSetUpFee() > 0): ?><td><?php echo $rate_plan->getCurrency()->getName() . '&nbsp;' . sprintf('%.2f', $rate_plan->getSetUpFee()); ?></td><?php  endif; ?>
                <?php if ($rate_plan->getRecurringFee() > 0): ?><td><?php echo _devconnect_monetization_get_frequency_fee_text($rate_plan); ?></td><?php endif; ?>
                <?php if ($rate_plan->getEarlyTerminationFee() > 0): ?><td><?php echo $rate_plan->getCurrency()->getName() . '&nbsp;' . $rate_plan->getEarlyTerminationFee(); ?></td><?php endif; ?>
              </tr>
              </tbody>
            </table>
          </div>
        <?php endif; ?>
        <?php print devconnect_monetization_build_rate_plan_details_output($rate_plan, isset($product_list) ? $product_list : NULL); ?>
      </div>
    </div>
  </div>
<?php endif; //End Future Plan Product Specific Plan 1 ?>
</div>
