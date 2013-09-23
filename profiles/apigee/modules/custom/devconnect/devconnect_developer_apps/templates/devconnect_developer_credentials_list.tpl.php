<?php
/**
 * @file
 * Default theme implementation to display a list of credentials.
 *
 * Available variables:
 * $credentials - a stdClass object containing the following four attributes:
 * - apiproducts: an array of API Product identifiers
 * - statuses: an array of status strings
 * - consumer_keys: an array of consumer keys (public facing)
 * - secret_keys: an array of secret keys (should not be divulged to public)
 *
 * The above four arrays should always have the same number of elements.
 * This allows for a tabular display if properly styled.
 */
?>
<div class="api-credentials-list">

  <div class="api-credentials-list-item-container api-credentials-list-apiproduct">
    <div class="api-credentials-list-item-caption"><?php print t('API Product'); ?>:&nbsp;</div>
    <?php foreach ($credentials->apiproducts as $apiproduct): ?>
      <div class="api-credentials-list-item"><?php print check_plain($apiproduct); ?></div>
    <?php endforeach; ?>
  </div>

  <div class="api-credentials-list-item-container api-credentials-list-status">
    <div class="api-credentials-list-item-caption"><?php print t('Status'); ?>:&nbsp;</div>
    <?php foreach ($credentials->statuses as $status): ?>
    <div class="api-credentials-list-item"><?php print check_plain($status); ?></div>
    <?php endforeach; ?>
  </div>

  <div class="api-credentials-list-item-container api-credentials-list-consumer-key">
    <div class="api-credentials-list-item-caption"><?php print t('Consumer Key'); ?>:&nbsp;</div>
    <?php foreach ($credentials->consumer_keys as $key): ?>
    <div class="api-credentials-list-item"><?php print check_plain($key); ?></div>
    <?php endforeach; ?>
  </div>

  <div class="api-credentials-list-item-container api-credentials-list-secret-key">
    <div class="api-credentials-list-item-caption"><?php print t('Secret Key'); ?>:&nbsp;</div>
    <?php foreach ($credentials->secret_keys as $key): ?>
    <div class="api-credentials-list-item"><?php print check_plain($key); ?></div>
    <?php endforeach; ?>
  </div>

</div>
