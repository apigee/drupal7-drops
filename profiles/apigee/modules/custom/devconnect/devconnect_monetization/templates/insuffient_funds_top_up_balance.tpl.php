<?php
/**
 * Variables
 *   $top_up_form Drupal form to submit
 *
 */

?>
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
    <?php print isset($top_up_form) ? $top_up_form : ''; ?>
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