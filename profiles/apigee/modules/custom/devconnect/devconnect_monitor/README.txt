
Devconnect Monitor Module
===========================

Allows error messages to be registered in the Devconnect Monitor to send out
alerts.  This module watches the watchdog log system, and will sent out alerts
if the log message matches the log type and severity of the alerts the admin
selected in the configuration of the module.

INSTALLATION
------------
1. Enable the module using admin interface or Drush.


CONFIGURATION
-------------
Once the module is activated, go to Configuration > Dev Portal Settings >
Dev Portal monitor.  This shows any alerts that have been configured via the
hook_devconnect_monitor_event_info() hook.  Select one or more alerts, then
add one or more emails to receive alerts when these alerts are logged.

ADDING YOUR OWN ALERTS
----------------------
If you want to add alerts to this module, implement the
hook_devconnect_monitor_event_info().  Here is an example:


/**
 * Implements hook_devconnect_monitor_event_info().
 *
 * Implementations return an associative array whose keys define
 * the unique alert name and whose values are an associative array of
 * properties for each path.  The alert name can be anything you want,
 * but should be prefixed with your module name to make sure it is
 * unique, such as "mymodule_overheating" and "mymodule_virusdetected".
 *
 * The array contains the following properties:
 *
 * description: A description of the alert, this can be anything to help
 * the end user.
 *
 * log_type: The watchdog type to match against when deciding to
 * sent out an alert.
 *
 * log_severity: The watchdog severity to match against when deciding to
 * sent out an alert.
 *
 */
function devconnect_monitor_devconnect_monitor_event_info() {
  return array(
    'mint_critical' => array(
      'description' => t('Monetization payment errors.'),
      'log_type' => 'devconnect_mint_payment',
      'log_severity' => WATCHDOG_EMERGENCY,
    ),
    'edge_api_exceptions' => array(
      'description' => t('Edge API exceptions and timeouts.'),
      'log_type' => 'APIObject',
      'log_severity' => WATCHDOG_EMERGENCY,
    ),
    'page_not_found' => array(
          'description' => t('Page not found.'),
          'log_type' => 'page not found',
          'log_severity' => WATCHDOG_WARNING,
    ),
  );
}


KNOWN ISSUES
-------------
None
