<?php
  $form = $variables['form'];
?>
<div id="topUpPurchase" class="modal hide fade" tabindex="-1" role="dialog" aria-labelledby="topUpLabel" aria-hidden="true">
  <div class="modal-header">
    <button type="button" class="close" data-dismiss="modal" aria-hidden="true">×</button>
    <h3 id="topUpLabel">Insufficient <?php print drupal_render($form['currency_title']); ?> Prepaid Balance.</h3>
    <div id="topup_alert_minimum_required" class="alert alert-error hide">
      You have to top up your balance for at least <span id="span-minimum-amount">0</span>.
    </div>
    <div id="topup_alert_maximum_required" class="alert alert-error hide">
      You cannot top up your balance with more than <span id="span-maximum-amount">0</span>.
    </div>
  </div>
  <div class="modal-body">
    <p>You have insufficient funds to purchase plan <?php print drupal_render($form['plan_name']); ?>.</p>
    <p>To purchase this plan you are required to top up your prepaid balance with at least <?php print drupal_render($form['required_display']); ?>.<br>
      Please enter the desired balance amount below.</p>
    <div style="margin-bottom: 10px;">
      <span class="topup-modal-label">Current Balance:</span>
      <span id="topUpCurrentBalance"><?php print drupal_render($form['current_balance_display']); ?></span>
    </div>
    <div style="margin-bottom: 10px;">
      <span class="topup-modal-label">Amount to Top Up:</span>
      <span class="topup-modal-value">
        <?php print drupal_render($form['amount']); ?>
      </span>
    </div>
    <div id="newBalanceWrapper">
      <span class="topup-modal-label">New Balance:</span>
      <span id="newBalance"><?php print drupal_render($form['new_balance_display']); ?></span>
    </div>
  </div>
  <div class="modal-footer">
    <?php print drupal_render($form['submit']); ?>
    <a class="btn" data-dismiss="modal" aria-hidden="true">Cancel</a>
  </div>
  <?php print drupal_render_children($form); ?>
</div>