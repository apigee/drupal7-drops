<?php
/**
 * Variables
 *   $tncs_forms List of accepted and new Apigee\Mint\TermsAndCondition objects for the user
 *   to accept them
 */
?>
<div class="table-responsive">
  <table class="table table-bordered">
    <thead>
      <tr>
        <th>Effective Date</th>
        <th>Terms &amp; Conditions</th>
        <th>T&amp;C Acceptance Date</th>
      </tr>
    </thead>
    <tbody>
      <?php foreach ($tncs_forms as $tnc) : ?>
      <tr>
        <td><?php print $tnc['tnc']->getFormattedStartDate('M d Y'); ?></td>
        <td><a href="<?php print $tnc['tnc']->getUrl(); ?>"><?php print $tnc['tnc']->getUrl(); ?></a></td>
        <?php if (!isset($tnc['accepted'])) : ?>
        <td><?php print $tnc['form']; ?></td>
        <?php else : ?>
        <td><?php print $tnc['accepted']; ?></td>
        <?php endif; ?>
      </tr>
      <?php endforeach; ?>
    </tbody>
  </table>
</div>