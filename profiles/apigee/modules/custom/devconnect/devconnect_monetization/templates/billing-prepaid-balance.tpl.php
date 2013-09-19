<?php
/**
 * Variables:
 *   $top_up_balance_perm Indicates either the user is granted to top up balance
 *   $has_balances Indicates if the developer has available report balances to download
 *   $balances collection of objects of type Apigee\Mint\DataStructure\DeveloperBalance, these
 *     objects are the reports available for downloading.
 *   $download_prepaid_report_perm Indicates if the user is granted to dowload reports
 *   $can_top_up_another_currency Indicates if the user has not topped up balance in all available currencies
 *   $previous_prepaid_statements_form Form to search previous prepraid statements
 */
?>
<h3>Current Prepaid Balance</h3>
<table>
  <thead>
    <tr>
      <th>Account Currency</th>
      <th>Balance Brought Forward</th>
      <th>Top Ups</th>
      <th>Usage</th>
      <th>Tax</th>
      <th>Current Balance</th>
      <?php if ($top_up_balance_perm) : ?>
      <th>Actions</th>
      <?php endif; ?>
    </tr>
  </thead>
  <tbody>
    <?php if ($has_balances) : ?>
      <?php foreach ($balances as $balance) : ?>
        <tr>
          <td><?php print $balance->supportedCurrency->name; ?></td>
          <td><?php print sprintf('%.2f', $balance->previousBalance); ?></td>
          <td><?php print sprintf('%.2f', $balance->topups); ?></td>
          <td><?php print sprintf('%.2f', $balance->usage); ?></td>
          <td><?php print sprintf('%.2f', $balance->tax); ?></td>
          <td>
            <?php print sprintf('%.2f', $balance->currentBalance); ?>
            <?php if ($download_prepaid_report_perm) : ?>&nbsp;&nbsp;
              <?php print l('Balance Detail (CSV)', 'users/me/monetization/billing/billing/' . rawurlencode($balance->supportedCurrency->name) . '/' . rawurlencode(date('F-Y', time())), array('attributes' => array('style' => 'float:right'))); ?>
            <?php endif; ?>
          </td>
        <?php if ($top_up_balance_perm) : ?>
          <td>
            <a href="javascript: topUpBalance('<?php print $balance->id; ?>', <?php print sprintf('%.2f', $balance->currentBalance); ?>, '<?php print $balance->supportedCurrency->name; ?>');" role="button" class="btn" >Top Up Balance</a>
          </td>
        <?php endif; ?>
        </tr>
      <?php endforeach; ?>
    <?php endif; ?>
    <?php if ($can_top_up_another_currency) : ?>
        <tr>
          <td>--</td>
          <td>--</td>
          <td>--</td>
          <td>--</td>
          <td>--</td>
          <td>--</td>
          <?php if ($top_up_balance_perm) : ?>
          <td><a href="javascript: topUpBalance();" class="btn">Top Up Balance</a></td>
          <?php endif; ?>
        </tr>
    <?php endif; ?>
  </tbody>
</table>
<?php if ($download_prepaid_report_perm) : ?>
<div>
  <h3>Previous Prepaid Statements</h3>
  <?php print $previous_prepaid_statements_form; ?>
</div>
<?php endif; ?>

<?php if ($top_up_balance_perm): ?>
<!-- Top Up Modal -->
<div id="topUp" class="modal hide fade" tabindex="-1" role="dialog" aria-labelledby="topUpLabel" aria-hidden="true">
  <div class="modal-header">
    <button type="button" class="close" data-dismiss="modal" aria-hidden="true">×</button>
    <h3 id="topUpLabel">Top Up Prepaid Balance <span id="currency_title"></span></h3>
    <div id="topup_alert" class="alert hide">
      <strong>Warning!</strong>&nbsp;The Amount to Top Up must be a valid number and bigger than zero.
    </div>
    <div id="currency_alert" class="alert hide">
      <strong>Warning!</strong>&nbsp;You must select currency.
    </div>
  </div>
  <div class="modal-body">
    <?php print $top_up_form; ?>
    <p>To top up your prepaid balance you will be taken to World Pay to process your payment.<br>
      Please enter the desired balance amount below.</p>
      <div id="currency_selector" style="margin-bottom: 10px; display:none;">
        <select>
          <option value="-1" selected="selected">select currency</option>
          <?php foreach ($currencies as $currency): ?>
          <option value="<?php print $currency->name; ?>"><?php print $currency->name . ' ('. $currency->displayName . ')'; ?></option>
          <?php endforeach; ?>
        </select>
      </div>
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
<?php endif; ?>