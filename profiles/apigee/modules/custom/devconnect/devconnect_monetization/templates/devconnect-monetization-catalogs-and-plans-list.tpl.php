<?php
/**
 * @file
 * Displays Packagaes and Purchased plans.
 *
 * Available vars:
 *   $was_plan_purchased - Indicates if the user is landing here right
 *      after purchasing a Plan.
 *   $packages - An array of available packages for the user to purchase Plan(s)
 *   $purchased_plans - Array of associative key/value of Plans info in which
 *    the user has purchased a Plan(s).
 *     package - string package display name
 *     package_id - string package id
 *     products - string product names the plan owns
 *     rate_plan - string plan display name
 *     rate_plan_id - string plan id
 *     plan_start_date - string format YYYY-MM-DD
 *     renewal_date - string format
 */
?>
<table>
  <thead>
    <tr>
        <th><?php print t('Package Name'); ?></th>
        <th><?php print t('Products'); ?></th>
    </tr>
  </thead>
  <tbody>
  <?php foreach ($packages as $package_id => $package) : ?>
    <tr>
      <td><?php echo l($package['displayName'], 'users/me/monetization/packages/' . rawurlencode($package_id) . '/view', array('attributes' => array('title'=>$package['description']))); ?>
      <td><?php echo implode(', ', $package['products']); ?></td>
    </tr>
  <?php endforeach; ?>
  </tbody>
</table>
