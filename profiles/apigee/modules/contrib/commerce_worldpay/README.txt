CONTENTS OF THIS FILE
---------------------

 * Introduction
 * Installation
 * Theme development
 * Known issues
 * Support
 * Sponsorship
 * Acknowledgements/Credits


INTRODUCTION
------------

This module implements support for WorldPay's Business Gateway payment gateway
service with Drupal Commerce as a payment module.

Please note this is for WorldPays HTTP service. Not its higher tier XML Direct
service. You will need to use the commerce_worldpay_xml module for that service.


INSTALLATION
------------

 1. Copy the 'commerce_worldpay_bg' folder into the modules directory
    usually: '/sites/all/modules/'.

 2. In your Drupal site, enable the module under Administration -> Modules
    The module will be in the group Commerce - Payment.

 3. Go to Store > Configuration > Payment Methods.

 4. Verify that "Payment via WorldPay" is an enabled payment. If not then click
    "enable" in its row.

 5. Click the "edit" link for "Payment via WorldPay".

 6. Under "Actions", "Enable payment method: Payment via WorldPay", click
    "edit".

 7. Follow the instruction on the page to complete the configuration of the
    module with your WorldPay installation.


THEME DEVELOPMENT
-----------------
WorldPay allows the customisation of the presentation to its payment pages. Part
of this is handled entirely on WorldPay's end by uploading assets to the correct
area of the merchant's WorldPay administration page. Another part is generated
by Drupal and pulled in by WorldPay for presentation on their end. The later
part is editable using Drupal's standard theming system using templates. The
two parts are found within:
module_dir/
          theme/ - Contains the Drupal template definitions.
          worldpay page example/ - Contains example's of files to be used in 
                                   WorldPay's payment pages.
Please see the README.txt file in the "worldpay page example/" folder for more
information.

NOTE: The odd tags like <WPDISPLAY> are special WorldPay tag's that WorldPay
will parse when loading the pages. Please review WorldPay's own documentation
for further information on these.

The module implements a special debugging page at:
commerce_worldpay/bg/response/debug_me
This will present a simulation of the response page generated for WorldPay.

KNOWN ISSUES
------------

 * This module will not work reliably at the moment if the order
   field commerce_customer_billing is missing or if that field (profile) is
   missing the field commerce_customer_address.
   See http://drupal.org/node/2001424.
 * The order object must have the order::mail property set before the payment
   transaction is triggered. If you are using Drupal Commerce's account
   information pane then this is already done for you. But if you are
   gathering account details using a custom method, then you must ensure you
   correctly populate the order object used in checkout.
   See http://drupal.org/node/2001424
 * The "Add payment" tool on the order UI Payment page will not work if it uses
   this module to make a transaction. I'm not sure if this can be made to work
   with this service. See http://drupal.org/node/2001442.

TODO
----
* Set appropriate Commerce Transaction status for SecureCode ('authentication').
  See http://drupal.org/node/2001452.
* Set appropriate Commerce Transaction status for AVS ('AVS').
  See http://drupal.org/node/2001454.
* Finish Bartik WorldPay theme (images etc.): http://drupal.org/node/2001458
* Make the module function fine without an addressfield or billing profile.
  See http://drupal.org/node/2001424

SUPPORT
-------

If you encounter any issues, please file a support request
at http://drupal.org/project/issues/commerce_worldpay


SPONSORSHIP
-----------

This module was originally developed for Zixiao (http://www.zixiao.co.uk).

The module is maintained by MD Systems (http://www.md-systems.ch/)

ACKNOWLEDGEMENTS/CREDITS
------------------------

Much of the code here got a running start thanks to the Commerce PayPal and Sage
payment modules so thank you to the authors ikos and rszrama. Also thanks to the
Ubercart uc_worldpay author Hans Idink and psynaptic as that module also gave me
a running start on working with WorldPay's API.

AUTHORS
-------
Adam Lyall aka MagicMyth <magicmyth@magicmyth.com>
