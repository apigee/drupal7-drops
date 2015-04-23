CONTENTS OF THIS FILE
---------------------

 * Introduction
 * Installation
 * Payment Success and Cancellation pages
 * NOTES

INTRODUCTION
------------

Included in this folder are a set of sample files for customizing the look
of WorldPay's payment pages. They will implement roughly the same visual
design of the default Drupal Bartik theme. See INSTALLATION on how to use.

INSTALLATION
------------

 * Log into your WorldPay installation page.
 
 * Click "Installations" and under "Payment Page Editor" click
   "Edit Payment Pages"
   
 * First we should remove the default inline theming set by WorldPay:
   - Click "Text Styles" and change all the selection widgets to blank and
     click save.
   - In the left navigation click "Colours" and blank all the fields.
   - In the left navigation click "Page Format" and blank all the fields.
   
 * Now in the left navigation click "File Management". Here you will one by
   one upload all the files in this folder excluding this README file. Do not
   enter anything in the "WorldPay Name for File".
   
 * Now you can preview your customized WorldPay payment pages by clicking on
   "Preview" (towards the top right).

PAYMENT SUCCESS AND CANCELLATION PAGES
--------------------------------------

The Payment Success and Cancellation pages (typically resultY.html and
result.C.html files in WorldPay) are built by Drupal and fetched by WorldPay
as needed. They are customized just like any other Drupal template files. You
can find the default implementations in the theme/ folder. Note that these
templates can make use of <WPDISPLAY> tags just like the ones directly hosted
on WorldPay's payment servers.

NOTES
-----

Any CSS file named stylesheet.css is embedded, rather than linked to in the
payment pages. If you do not want this, rename the file. Next create a head.html
file and inside add:
<link rel="stylesheet" href="/i/<wpdisplay item=instId>/[stylesheetname].css" type="text/css" />
Where [stylesheetname] is what you called your CSS file.

RESOURCES ON CUSTOMIZING
------------------------

These are some helpful resources on the structure of WorldPay's payment page
customization features.

http://www.worldpay.com/support/kb/bg/customisingadvanced/custa7104.html
http://www.worldpay.com/support/kb/bg/customisingadvanced/custa7100.html
http://www.worldpay.com/support/kb/bg/htmlredirect/rhtml6000.html