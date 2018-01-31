<?php
$conf['allow_authorize_operations'] = FALSE;
// Do not directly edit this file; instead, put your custom code in
// sites/default/settings.local.php.
if (defined('DRUPAL_ROOT') && file_exists(DRUPAL_ROOT . '/sites/default/settings.local.php')) {
  include DRUPAL_ROOT . '/sites/default/settings.local.php';
}

if (isset($_ENV['PANTHEON_ENVIRONMENT']) && php_sapi_name() != 'cli') {
  // Redirect to apigee.io from apigee.com for all Pantheon environments.
  // Only for the apigee.com domain, not customer custom domains.
  $domain_old = '.devportal.apigee.com';
  $domain_new = '.devportal.apigee.io';
  // If the domain contains apigee.com.
  if (strpos($_SERVER['HTTP_HOST'], $domain_old) !== FALSE ) {
    // Redirect with same URI, but change old domain to new domain.
    header('Location: https://' . substr($_SERVER['HTTP_HOST'], 0, strpos($_SERVER['HTTP_HOST'], $domain_old)) . $domain_new . $_SERVER['REQUEST_URI']);
    exit;
  }
}

