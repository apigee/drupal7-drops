<?php
/**
 * @file
 * WorldPay custom page theme
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
 * - $order_id: The current Commerec order's ID.
 * - $order_no: The current Commerec order's number.
 * - $content: The rendered content of the page.
 * - $title: The title of the page.
 * - $site_url: URL of the web site.
 * - $site_id: The value set for Site ID in the payment module settings
 *   page.
 *
 * @see template_preprocess_commerce_worldpay_bg_page()
 */
?>
<div id="page-wrapper">
  <div id="page">
    <div id="header"><div class="section">
      <div id="site-name">
         <strong><span><?php print $title; ?></span></strong>
      </div>
    </div></div>
    <div id="main">
      <?php print $content; ?>
    </div>
  </div>
</div>
