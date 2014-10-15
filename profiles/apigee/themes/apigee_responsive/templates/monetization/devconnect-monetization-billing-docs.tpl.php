<?php
/**
 * Variables:
 *   $billing_documents_form Search and Filter form
 *   $billing_documents collection of object of type Apigee\Mint\BillingDocuements
 */
?>
<?php print $billing_documents_form; ?>
<div class="table-responsive">
  <table class="table table-bordered">
    <thead>
    <tr>
      <th><?php print t('Document Type'); ?></th>
      <th><?php print t('Reference'); ?></th>
      <th><?php print t('Products'); ?></th>
      <th><?php print t('Received Date'); ?></th>
      <th><?php print t('Download'); ?></th>
    </tr>
    </thead>
    <tbody>
    <?php foreach ($billing_documents as $doc): ?>
      <tr>
        <td><?php print $doc['type'];  ?></td>
        <td><?php print $doc['ref']; ?></td>
        <td><?php print $doc['prods']; ?></td>
        <td><?php print $doc['rec_date'] ?></td>
        <td><?php print l(t('Download'), 'users/me/monetization/billing-document/' . rawurlencode($doc['ref']), array('attributes' => array('class' => array('btn')))); ?></td>
      </tr>
    <?php endforeach; ?>
    </tbody>
  </table>
</div>
