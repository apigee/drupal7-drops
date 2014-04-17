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
      <th>Document Type</th>
      <th>Reference</th>
      <th>Products</th>
      <th>Received Date</th>
      <th>Download</th>
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