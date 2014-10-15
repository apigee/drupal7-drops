<?php
/**
 * Variables:
 *   $top_up_balance_perm Indicates either the user is granted to top up balance
 *   $has_balances Indicates if the developer has available report balances to download
 *   $balances collection of objects of type Apigee\Mint\DataStructure\DeveloperBalance, these
 *     objects are the reports available for downloading.
 *   $download_prepaid_report_perm Indicates if the user is granted to download reports
 *   $can_top_up_another_currency Indicates if the user has not topped up balance in all available currencies
 *   $previous_prepaid_statements_form Form to search previous prepaid statements
 */
?>
  <h3><?php print t('Current Prepaid Balance'); ?></h3>
  <hr>
  <div class="table-responsive">
    <table class="table table-bordered">
      <thead>
      <tr>
        <th><?php print t('Account Currency'); ?></th>
        <th><?php print t('Balance Brought Forward'); ?></th>
        <th><?php print t('Top Ups'); ?></th>
        <th><?php print t('Usage'); ?></th>
        <th><?php print t('Tax'); ?></th>
        <th><?php print t('Current Balance'); ?></th>
        <?php if ($top_up_balance_perm) : ?>
          <th><?php print t('Actions'); ?></th>
        <?php endif; ?>
      </tr>
      </thead>
      <tbody>
      <?php if ($has_balances) : ?>
        <?php foreach ($balances as $balance) : ?>
          <tr>
            <td><?php print $balance->getSupportedCurrency()->getName(); ?></td>
              <?php if ($balance_report_type == BILLING_AND_REPORTS_USE_PREPAID_API_CALL): ?>
                <td><?php print commerce_currency_format($balance->getPreviousBalance(), $balance->getSupportedCurrency()->getName(), NULL, FALSE); ?></td>
                <td><?php print commerce_currency_format($balance->getTopups(), $balance->getSupportedCurrency()->getName(), NULL, FALSE); ?></td>
                <td><?php print commerce_currency_format($balance->getUsage(), $balance->getSupportedCurrency()->getName(), NULL, FALSE); ?></td>
                <td><?php print commerce_currency_format($balance->getTax(), $balance->getSupportedCurrency()->getName(), NULL, FALSE); ?>
                <td><?php print commerce_currency_format($balance->getCurrentBalance(), $balance->getSupportedCurrency()->getName(), NULL, FALSE); ?>
              <?php else: ?>
                <td><?php print commerce_currency_format($balance->getPreviousBalance(), $balance->getSupportedCurrency()->getName(), NULL, FALSE); ?></td>
                <td><?php print commerce_currency_format($balance->getTransaction()->getRate(), $balance->getSupportedCurrency()->getName(), NULL, FALSE); ?></td>
                <td><?php print commerce_currency_format($balance->getUsage(), $balance->getSupportedCurrency()->getName(), NULL, FALSE); ?></td>
                <td><?php print commerce_currency_format($balance->getTax(), $balance->getSupportedCurrency()->getName(), NULL, FALSE); ?></td>
                <td><?php print commerce_currency_format($balance->getAmount(), $balance->getSupportedCurrency()->getName(), NULL, FALSE); ?>
              <?php endif; ?>
              <?php if ($download_prepaid_report_perm) : ?>&nbsp;&nbsp;
                <?php print l(t('Balance Detail (CSV)'), 'users/me/monetization/billing/report/download-prepaid-report/' . rawurlencode($balance->getSupportedCurrency()->getName()) . '/' . rawurlencode(date('F-Y', time())), array('attributes' => array('style' => 'float:right'))); ?>
              <?php endif; ?>
            </td>
            <?php if ($top_up_balance_perm) : ?>
              <?php if ($balance->getSupportedCurrency()->getName() != 'POINTS'): ?>
                <td>
                  <a class="top-up trigger btn" balance-id="<?php print $balance->getId(); ?>"
                     current-balance="<?php print $balance->getCurrentBalance(); ?>"
                     currency="<?php print $balance->getSupportedCurrency()->getName(); ?>" role="button"><?php print t('Top Up Balance'); ?></a>
                </td>
              <?php else: ?>
                <td>&nbsp;</td>
              <?php endif; ?>
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
            <td><a class="top-up trigger btn" role="button"><?php print t('Top Up Balance'); ?></a></td>
          <?php endif; ?>
        </tr>
      <?php endif; ?>
      </tbody>
    </table>
  </div>
<?php if ($download_prepaid_report_perm) : ?>
  <div class="spacer">
    <h3><?php print t('Previous Prepaid Statements'); ?></h3>
    <hr>
    <?php print $previous_prepaid_statements_form; ?>
  </div>
<?php endif; ?>

<?php if ($top_up_balance_perm): ?>
  <?php print $top_up_form; ?>
<?php endif; ?>
