<?php
/**
 * @file
 * WorldPay custom theme for the payment success page
 *
 * WorldPay generates custom pages by calling the Payment Response/Notification
 * server, which generates a HTML(ish) output with some WorldPay propriority
 * tags.
 *
 * We cannot point to any files on this server. They must be uploaded to
 * WorldPay in the installation control panel and then reference here as:
 *    path="/i/$installation_id/file.ext"
 * You can optionally have WorldPay set the installation_id instead of the one
 * store in this site by using the WorldPay tag:
 * path="/i/<wpdisplay item=instId>/file.ext">
 * For this reason we do not make use of Drupal's asset attachment features.
 * For more information on what WorldPay tags are available
 * @see: http://www.worldpay.com/support/kb/bg/paymentresponse/pr5402.html
 *
 * NOTE this template does not go through the usual theme route so don't
 * expect the same variables available to html.tpl.php
 *
 * Variables:
 * - $installation_id: The WorldPay installation ID stored in the sites
 *   Commerce payment settings page.
 * - $order_id: The current Commerce order's ID
 * - $order_no: The current Commerce order's number
 * - $content: The rendered content of the page
 * - $title: The title of the page
 * - $return_url: The URL to the final page of the Commerce checkout process.
 * - $site_name: The name set in Drupal configuration
 * - $site_id: The value set for Site ID in the payment module settings
 *   page.
 *
 * @see template_preprocess_commerce_worldpay_bg_success()
 * @todo find a way to print the merchant id under the transaction id:
 * <span><?php print t('Merchant\'s Reference:'); ?>&nbsp;</span>
 * <span><b><?php print $merchant_id ?></b></span><br>
 */
?>
<header>
  <h1><?php print $title; ?></h1>
  <p><?php t('Your transaction was successfuly recieved by WorldPay. Thank you.'); ?></p>
  <table class="bartik">
    <thead>
      <tr>
        <th><?php print t('Order No'); ?></th>
        <th><?php print t('WorldPay transaction code'); ?></th>
      </tr>
    </thead>
    <tbody>
      <tr>
        <td><?php print $order_no; ?></td>
        <td><?php print $wp_txn_id; ?></td>
      </tr>
    </tbody>
  </table>
  <?php if (!$settings['theme_debug']): ?>
    <WPDISPLAY ITEM=banner>
  <?php else: ?>
  <table border="0" cellpadding="0" cellspacing="0" class="bannercontainer">
  <tbody>
  <tr>
    <td>
      <table cellpadding="2" cellspacing="0" class="banner">
        <tbody>
            <?php if (!$settings['theme_debug']): ?>
              <tr valign="top">
                <td class="bannererror">
                  <span><?php print t('This was NOT a live transaction - no money has changed hands'); ?></span>
                </td>
              </tr>
            <?php endif; ?>
            <tr valign="top">
              <td class="banner">
                <span><?php print t('Thank you, your payment was successful'); ?></span><br />
                <span><?php print t('WorldPay Transaction ID:'); ?>&nbsp;</span>
                <span><b><?php print $wp_txn_id; ?></b></span><br />
              </td>
            </tr>
        </tbody>
      </table>
    </td>
    </tr>
  </tbody>
  </table>
  <?php endif; ?>
  <p id="return-url">
    <?php print l(t('Finish your order'), $return_url, array('absolute' => TRUE)); ?>
  </p>
</header>
