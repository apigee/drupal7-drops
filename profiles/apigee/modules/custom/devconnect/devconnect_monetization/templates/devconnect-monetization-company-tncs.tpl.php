<?php
/**
 * Variables
 *   $tncs_forms List of accepted and new Apigee\Mint\TermsAndCondition objects
 *   for the user to accept them.
 */
?>
<table>
  <thead>
    <tr>
        <th><?php print t('Effective Date'); ?></th>
        <th><?php print t('Terms & Conditions'); ?></th>
        <th><?php print t('T&C Acceptance Date'); ?></th>
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
