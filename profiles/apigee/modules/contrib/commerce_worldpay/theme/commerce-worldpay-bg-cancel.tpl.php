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
 * - $order_id: The current Commerce order's ID.
 * - $order_no: The current Commerce order's number.
 * - $content: The rendered content of the page.
 * - $title: The title of the page.
 * - $return_url: The URL to the final page of the Commerce checkout process.
 * - $site_name: The name set in Drupal configuration.
 * - $site_id: The value set for Site ID in the payment module settings
 *   page.
 *
 * @see template_preprocess_commerce_worldpay_bg_cancel()
 */
?>
<header>
  <h1><?php print $title; ?></h1>
  <p><?php print t('Payment was cancelled. No funds have been transferred.'); ?></p>
  <table>
    <thead>
      <tr>
        <th><?php print t('Order No'); ?></th>
      </tr>
    </thead>
    <tbody>
      <tr>
        <td><?php print $order_no; ?></td>
      </tr>
    </tbody>
  </table>
  <p>
    <?php print l(t('Return to') . ' ' . $site_name, $return_url); ?>
  </p>
</header>
