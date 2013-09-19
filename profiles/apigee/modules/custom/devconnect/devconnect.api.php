<?php

/**
 * Allows error message text and treatment to be customized.
 *
 * Depending on the contents of the $display parameter, the error message
 * will be formatted either with devconnect_error_inline.tpl.php or
 * devconnect_error_message.tpl.php.
 *
 * @param int $error_code
 *        This could be one of the Apigee\Util\ErrorHandling::CODE_* constants
 *        or an actual HTTP code or exception error code.
 * @param string $summary
 *        Error message that is shown to unprivileged users.
 * @param string $details
 *        Additional error message that is shown to users with the "view
 *        devconnect errors" permission.  This is set to NULL when no such
 *        additional information should be displayed.
 * @param int $severity
 *        One of the following: ErrorHandling::SEVERITY_NOTICE,
 *        ErrorHandling::SEVERITY_WARNING, ErrorHandling::SEVERITY_ERROR.
 * @param int $display
 *        Either ErrorHandling::DISPLAY_INLINE or ErrorHandling::DISPLAY_MESSAGE.
 *        If $display is DISPLAY_MESSAGE, a Drupal message is generated (whose
 *        class is determined by $severity). Otherwise an error message
 *        designed for inline display will be generated.
 */
function hook_devconnect_error_alter($error_code, &$summary, &$details, &$severity, &$display) {
  if ($error_code == 404) {
    $summary = 'Sorry, bro, I cannot find your resource.';
    $display = Apigee\Util\ErrorHandling::DISPLAY_INLINE;
    $severity = Apigee\Util\ErrorHandling::SEVERITY_WARNING;
  }
}