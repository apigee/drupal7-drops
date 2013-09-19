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
 * WorldPay in the installtion control panel and then reference here as:
 *    path="/i/$installtion_id/file.ext"
 * You can optionaly have WorldPay set the installtion_id instead of the one
 * store in this site by using the WorldPay tag:
 * path="/i/<wpdisplay item=instId>/file.ext">
 * For this reason we do not make use of Drupal's asset attachment features.
 * For more information on what WorldPay tags are available
 * @see: http://www.worldpay.com/support/kb/bg/paymentresponse/pr5402.html
 *
 * NOTE this template does not go through the usual theme route so don't
 * expect the same variables availble to html.tpl.php
 *
 * Variables:
 * - $installtion_id: The WorldPay installtion ID stored in the sites 
 *   Commerece payment settings page.
 * - $order_id: The current Commerec order's ID
 * - $order_no: The current Commerec order's number
 * - $content: The rendered content of the page
 * - $title: The title of the page
 * - $return_url: The URL to the final page of the Commerce checkout process.
 * - $site_name: The name set in Drupal configuration
 * - $site_id: The value set for Site ID in the payment module settings
 *   page.
 *
 * @see template_preprocess_commerce_worldpay_bg_success()
 */
?>
<header>
  <h1><?php print $title; ?></h1>
  <p>Your transaction was successfuly recieved by WorldPay. Thank you.</p>
  <table class="bartik">
    <thead>
      <tr>
        <th>Order No</th>
        <th>WorldPay transaction code</th>
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
          <tr valign="top">
            <td class="bannererror">
              <span style=" font-family: ; font-size: ;">This was NOT a live transaction - no money has changed hands</span></td>
            </tr>
            <tr valign="top">
              <td class="banner">
                <span style=" font-family: ; font-size: ; color: ;">Thank you, your payment was successful</span><br>
                <span style=" font-family: ; font-size: ; color: ;">Merchant's Reference:&nbsp;</span>
                <span style=" font-family: ;"><b>5</b></span><br>
                <span style=" font-family: ; font-size: ; color: ;">WorldPay Transaction ID:&nbsp;</span>
                <span style=" font-family: ;"><b>128246379</b></span><br>
              </td>
            </tr>
        </tbody>
      </table>
    </td>
    </tr>
  </tbody>
  </table>
  <?php endif; ?>
  
  <p id="return-url"><a href="<?php print $return_url; ?>">Finish your order</a></p>
</header>
