<?php
$can_use_apigee = FALSE;
if (module_exists('apigee_account')) {
  $server_name = strtolower($_SERVER['SERVER_NAME']);
  if (empty($server_name) && array_key_exists('HTTP_HOST', $_SERVER)) {
    $server_name = strtolower($_SERVER['HTTP_HOST']);
  }
  // Server host whitelist includes 'localhost' and *.apigee.com. TODO: make this dynamic
  if ($server_name == 'localhost' || preg_match('!apigee.com$!', $server_name) || preg_match('!opdk.info$!', $server_name)) {
    $can_use_apigee = TRUE;
  }
}
?>
<div id="apigee-sso-signin-block" class="clearfix">
  <ul class="federated-buttons">
    <?php if (module_exists('apigee_sso')): ?>
      <li><?php echo l('Authenticate with Google', 'apigee_sso', array('attributes' => array('class' => array('btn', 'google'))));?></li>
    <?php endif; ?>
    <?php if ($can_use_apigee):?>
      <li><?php echo l("Authenticate with Apigee", 'aac-login', array('attributes' => array('class' => array('btn', 'aac'))));?></li>
    <?php endif; ?>
    <?php if (module_exists('twitter_signin') && (strlen(variable_get('twitter_consumer_secret', '')) > 0)
      && (strlen(variable_get('twitter_consumer_key',''))) > 0) { ?>
      <li><?php echo l('Authenticate with Twitter', 'twitter/redirect', array('attributes' => array('class' => array('btn', 'twitter'))));?></li>
    <?php } ?>
    <?php if (module_exists('github_connect') && isset($github)): ?>
      <li><?php echo l('Authenticate with GitHub', $github, array('attributes' => array('class' => array('btn', 'github'))));?></li>
    <?php endif; ?>
  </ul>
</div>
