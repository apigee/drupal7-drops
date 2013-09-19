<?php
/**
 * @file
 * Displays detail for a single app.
 *
 * Available vars:
 *   $account - stdClass: user owning this app.
 *   $access_type - string: read, write, read/write, none
 *   $callback_url - string
 *   $name - string
 *   $status - string (App status)
 *   $app_attributes - associative array: display-name => value.
 *   $credentials - array of associative arrays with the following keys:
 *     apiproducts - array of strings
 *     consumer_key
 *     consumer_secret
 *     status (Credential status)
 *   $analytics_chart - boolean|string
 *
 * Each $credentials[$x]['apiproducts'] is an associative array with the following
 * keys:
 *   display_name
 *   description
 *   status (API Product status)
 */

?>
<h1>This template is obsolete.</h1>
<p>
  If you override this template in your custom theme, that template will be
  used. If you need customization, however, it is recommended that you
  implement hook_devconnect_developer_app_details_alter() in a custom module
  instead, and then remove any devconnect_developer_apps.tpl.php file from
  your theme.
</p>
