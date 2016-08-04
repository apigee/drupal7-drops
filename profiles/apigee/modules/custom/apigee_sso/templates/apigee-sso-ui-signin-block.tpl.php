<div id="apigee-sso-signin-block" class="clearfix">
  <ul class="federated-buttons">
    <?php if ($apigee):?>
      <li><?php echo l(t('Authenticate with Apigee'), 'aac-login', array('attributes' => array('class' => array('btn', 'aac'))));?></li>
    <?php endif; ?>
    <?php if ($twitter): ?>
      <li><?php echo l(t('Authenticate with Twitter'), 'twitter/redirect', array('attributes' => array('class' => array('btn', 'twitter'))));?></li>
    <?php endif; ?>
    <?php if ($github): ?>
      <li><?php echo l(t('Authenticate with GitHub'), $github, array('attributes' => array('class' => array('btn', 'github'))));?></li>
    <?php endif; ?>
  </ul>
</div>
