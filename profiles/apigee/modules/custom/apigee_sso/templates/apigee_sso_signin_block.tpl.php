<div id="apigee-sso-signin-block" class="clearfix">
	<ul class="federated-buttons">
		<li><?php echo l("Authenticate with Google", "apigee_sso", array("attributes" => array("class" => array("btn", "google"))));?></li>
		<?php if (module_exists("fbconnect")):?>
		<li><?php echo l("Authenticate with Facebook", "facebook", array("attributes" => array("class" => array("btn", "facebook"))));?></li>
		<?php endif; ?>
		<?php if (module_exists("twitter_signin")):?>
		<li><?php echo l("Authenticate with Twitter", "twitter/redirect", array("attributes" => array("class" => array("btn", "twitter"))));?></li>
		<?php endif; ?>
	</ul>
</div>
