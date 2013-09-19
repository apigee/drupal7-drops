<div class="devconnect-error <?php print $severity; ?>">
  <div class="error-summary"><?php print check_plain($summary); ?></div>
  <?php if ($detail): ?><pre class="error-detail"><?php print check_plain($detail); ?></pre><?php endif; ?>
</div>