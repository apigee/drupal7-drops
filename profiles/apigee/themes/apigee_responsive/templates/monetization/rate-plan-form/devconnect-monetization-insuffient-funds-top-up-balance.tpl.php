<?php
  $form = $variables['form'];
?>
<div id="topUpPurchase" class="modal hide fade" tabindex="-1" role="dialog" aria-labelledby="topUpLabel" aria-hidden="true">
  <div class="modal-dialog">
    <div class="modal-content">
      <div class="modal-header">
        <button type="button" class="close" data-dismiss="modal" aria-hidden="true">&times;</button>
        <h3 id="topUpLabel"><?php print t('Insufficient !currency_title Prepaid Balance', array('!currency_title' => drupal_render($form['currency_title']))); ?></h3>
        <div id="topup_alert_minimum_required" class="alert alert-error hide error">
          <?php print t('You have to top up your balance for at least !minimum_amount.', array('!minimum_amount' => '<span id="span-minimum-amount">0</span>')); ?>
        </div>
        <div id="topup_alert_maximum_required" class="alert alert-error hide error">
          <?php print t('You cannot top up your balance with more than !max_amount.', array('!max_amount' => '<span id="span-maximum-amount">0</span>')); ?>
        </div>
      </div>
      <div class="modal-body">
        <p><?php print t('You have insufficient funds to purchase plan !plan_name.', array('!plan_name' => drupal_render($form['plan_name']))); ?></p>
        <p><?php print t('To purchase this plan you are required to top up your prepaid balance with at least !req_display.', array('!req_display' => drupal_render($form['required_display']))); ?><br />
          <?php print t('Please enter the desired balance amount below.'); ?></p>
        <div style="margin-bottom: 10px;">
          <span class="topup-modal-label"><?php print t('Current Balance:'); ?></span>
          <span id="topUpCurrentBalance"><?php print drupal_render($form['current_balance_display']); ?></span>
        </div>
        <div style="margin-bottom: 10px;">
          <span class="topup-modal-label"><?php print t('Amount to Top Up:'); ?></span>
          <span class="topup-modal-value">
            <?php print drupal_render($form['amount']); ?>
          </span>
        </div>
        <div id="newBalanceWrapper">
          <span class="topup-modal-label"><?php print t('New Balance:'); ?></span>
          <span id="newBalance"><?php print drupal_render($form['new_balance_display']); ?></span>
        </div>
      </div>
      <div class="modal-footer">
        <?php print drupal_render($form['submit']); ?>
        <a class="btn" data-dismiss="modal" aria-hidden="true"><?php print t('Cancel'); ?></a>
      </div>
    </div>
  </div>
  <?php print drupal_render_children($form); ?>
</div>
