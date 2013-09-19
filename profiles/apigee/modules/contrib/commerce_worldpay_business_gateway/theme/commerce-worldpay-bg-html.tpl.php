<?php
/**
 * @file
 * WorldPay custom theme
 *
 * WorldPay generates custom pages by calling the Payment Response/Notification
 * server, which generates a HTML(ish) output with some WorldPay propriority
 * tags.
 *
 * NOTE The only WorldPay tag that MUST be in this file is:
 *    <WPDISPLAY ITEM=banner>
 * Failure to include this tag will prevent WorldPay displaying the customized
 * page and potentialy lead to a suspension of the owner's WorldPay account.
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
 * Example: Add an uploaded stylsheet to use
 * <link rel="stylesheet" href="/i/<wpdisplay item=instId>/stylesheetname.css" type="text/css" />
 *
 * NOTE this template does not go through the usual theme route so don't
 * expect the same variables availble to html.tpl.php
 *
 * Variables:
 * - $installtion_id: The WorldPay installtion ID stored in the sites
 *   Commerece payment settings page.
 * - $order_id: The current Commerec order's ID
 * - $order_no: The current Commerec order's number
 * - $page - The rendered page content
 *   $language->language contains its textual representation. $language->dir
 *   contains the language direction. It will either be 'ltr' or 'rtl'.
 * - $rdf_namespaces: All the RDF namespace prefixes used in the HTML document.
 * - $grddl_profile: A GRDDL profile allowing agents to extract the RDF data.
 * - $head_title: A modified version of the page title, for use in the TITLE
 *   tag.
 * - $body_attributes: Contains any html attribute definitions intended for the
 *   body tag. These are generated in theme's preprocess' $body_attributes_array.
 * - $site_id: This is a customisable string set in the payment modules
 *   settings page. The same value is passed to WorldPay as <WPDISPLAY ITEM=C_siteId>.
 *   It is useful for creating alternate styles and selecting alternate images.
 *   For example. If you have two sites with their own brands but just one
 *   WorldPay merchant account. You can show the different logos by prefixing
 *   the image files name with the $site_id like so:
 *   <img src="< ?php print $site_id; ? >-logo.png" /> or if it is a WorldPay hosted
 *   payment page template:
 *   <img src="<WPDISPLAY ITEM=C_siteId-ppe empty='fallback_id'>-logo.png" />
 *
 * @see template_preprocess_commerce_worldpay_bg_html()
 */
?><!DOCTYPE html>
<html lang="<?php print $language->language; ?>" version="HTML+RDFa 1.0" dir="<?php print $language->dir; ?>"<?php print $rdf_namespaces; ?>>
<head profile="<?php print $grddl_profile; ?>">
  <title><?php print $head_title; ?></title>
  <!-- Embed the stylesheet stored on WorldPay's server -->
  <style type="text/css">
  <?php if ($settings['theme_debug']): //Allows simulating a bit of WorldPay's behavior. ?>
  <?php include drupal_get_path('module', 'commerce_worldpay_bg') . '/worldpay page example/stylesheet.css'; ?>
  <?php else: ?>
  <WPDISPLAY FILE=stylesheet.css>
  <?php endif; ?>
  </style>
</head>
<body <?php print $body_attributes; ?>>
  <?php print $page; ?>
</body>
</html>
