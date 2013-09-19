<?php
/**
 * @file
 * Displays Packagaes and Purchased plans.
 *
 * Available vars:
 *   $was_plan_purchased - Indicates if the user is landing here right after purchasing a Plan
 *   $packages - An array of available packages for the user to purchase Plan(s)
 *   $purchased_plans - Array of associative key/value of Plans info in which the user has purchased a Plan(s)
 *     package - string package display name
 *     package_id - string package id
 *     products - string product names the plan owns
 *     rate_plan - string plan display name
 *     rate_plan_id - string plan id
 *     plan_start_date - string format YYYY-MM-DD
 *     renewal_date - string format
 */
?>
<div class="tabbable">
  <ul class="nav nav-tabs">
    <li class="active"><a data-toggle="tab" href="#tab1">Catalog</a></li>
    <li><a data-toggle="tab" href="#tab2">Purchased Plans</a></li>
  </ul>
  <div class="tab-content">
    <div class="tab-pane active" id="tab1">
      <table>
        <thead>
          <tr>
            <th>Package Name</th>
            <th>Products</th>
          </tr>
        </thead>
        <tbody>
        <?php foreach ($packages as $package_id => $package) : ?>
          <tr>
            <td><?php echo l($package['displayName'], 'users/me/monetization/packages/' . rawurlencode($package_id), array('attributes' => array('title'=>$package['description']))); ?>
            <td><?php echo implode(', ', $package['products']); ?></td>
          </tr>
        <?php endforeach; ?>
        </tbody>
      </table>
    </div>
    <div class="tab-pane" id="tab2">
      <table>
        <thead>
          <tr>
            <th>Package</th>
            <th>Products</th>
            <th>Plan</th>
            <th>Start Date</th>
            <th>End Date</th>
            <th>Renewal Date</th>
            <th>Actions</th>
          </tr>
        </thead>
        <tbody>
          <?php foreach ($purchased_plans as $rate_plan): ?>
            <tr>
              <td><?php echo $rate_plan['package']; ?></td>
              <td><?php echo $rate_plan['products']; ?></td>
              <td><?php echo l($rate_plan['rate_plan'], 'users/me/monetization/packages/' . rawurlencode($rate_plan['package_id']) . '/rate-plans/' . rawurlencode($rate_plan['rate_plan_id'])); ?></td>
              <td><?php echo $rate_plan['start_date']; ?></td>
              <td><?php echo $rate_plan['end_date']; ?></td>
              <td><?php echo $rate_plan['renewal_date']; ?></td>
              <td><?php echo $rate_plan['action']; ?></td>
            </tr>
          <?php endforeach; ?>
        </tbody>
      </table>
    </div>
  </div>
</div>
