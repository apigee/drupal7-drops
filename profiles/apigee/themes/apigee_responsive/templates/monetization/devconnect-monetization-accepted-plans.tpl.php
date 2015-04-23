<?php
/**
 * @file
 * Display the developer's purchased plans.
 *
 * Available variables:
 * - $rate_plan
 */
?>
<div class="table-responsive">
  <table class="table table-bordered">
    <thead>
    <tr>
      <th><?php print t('Package'); ?></th>
      <th><?php print t('Products'); ?></th>
      <th><?php print t('Plan'); ?></th>
      <th><?php print t('Start Date'); ?></th>
      <th><?php print t('End Date'); ?></th>
      <th><?php print t('Plan End Date'); ?></th>
      <th><?php print t('Renewal Date'); ?></th>
      <th><?php print t('Actions'); ?></th>
    </tr>
    </thead>
    <tbody>
    <?php foreach ($purchased_plans as $rate_plan): ?>
      <tr>
        <td><?php echo $rate_plan['package']; ?></td>
        <td><?php echo $rate_plan['products']; ?></td>
        <?php if ($rate_plan['has_ended']): ?>
          <td><?php echo $rate_plan['rate_plan']; ?></td>
        <?php else: ?>
          <td><?php echo l($rate_plan['rate_plan'], 'users/me/monetization/packages/' . rawurlencode($rate_plan['package_id']) . '/view', array('fragment' => 'tab_' . preg_replace('/[^a-z0-9_-]/i', '_', $rate_plan['rate_plan_id']))); ?></td>
        <?php endif; ?>
        <td><?php echo $rate_plan['start_date']; ?></td>
        <td><?php echo $rate_plan['end_date']; ?></td>
        <td><?php echo $rate_plan['plan_end_date']; ?></td>
        <td><?php echo $rate_plan['renewal_date']; ?></td>
        <td><?php echo $rate_plan['action']; ?></td>
      </tr>
    <?php endforeach; ?>
    </tbody>
  </table>
</div>
