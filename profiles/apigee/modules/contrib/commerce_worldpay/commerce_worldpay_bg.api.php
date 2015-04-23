<?php

/**
 * @file
 * Documents hooks provided by the WorldPay Business Gateway modules.
 */


/**
 * Lets modules add extra data to post to WorldPay.
 *
 * Before directing the user to WorldPay a set of hidden form details are
 * created that contain both mandatory and optional information for WorldPay
 * to use. This module will post standard commerce profile information like
 * the address field data to WorldPay but many sites add additional attributes
 * to their checkout forms such as contact numbers which can also be useful
 * for WorldPay. This hook allows other modules to amend such information to
 * the form.
 *
 * @see http://www.worldpay.com/support/kb/bg/htmlredirect/rhtml5902.html
 *
 * NOTE Not sure if I will keep this around as it may be possible to add
 * extra data using rules through the UI (using the data fields) but I do not
 * know enough about Rule's internals and I need to be able to this stuff
 * now ;)
 *
 * @param object $order
 *   The order that initiated the payment associated with the WorldPay
 *   transaction.
 * @param object $profile
 *   The fully loaded profile assigned to this order.
 * @param array $settings
 *   The payment methods settings.
 *
 * @return array
 *   An array with keys matching WorldPay purchase transaction parameters.
 */
function hook_commerce_worldpay_bg_post_data($order, $profile, $settings) {
  // Example.
  return array(
    // The key must match a WorldPay parameter.
    'tel' => $profile->field_phone_no[LANGUAGE_NONE][0],
  );
}
