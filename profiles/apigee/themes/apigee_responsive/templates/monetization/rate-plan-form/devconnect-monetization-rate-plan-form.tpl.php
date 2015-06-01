<?php
/**
 * Author: isaias@apigee.com
 * User: Isaias
 * Date: 11/18/13
 * Time: 9:00 PM
 * To change this template use File | Settings | File Templates.
 */
$form = $variables['form'];
?>
<a href="/users/me/monetization/packages" class="back-to-catalog pull-right" ><?php print t('Back to Catalog'); ?></a>
<h3><?php print t('Package Name: @package_name', array('@package_name' => $form['#package_name'])); ?></h3>
<hr>
<?php if (isset($form['#active_plan'])): ?>
<span class="active-plan well"><strong>Active Plan:</strong>&nbsp;<?php print $form['#active_plan']; ?></span>
<?php endif; ?>
<div class="row">
<div class="col-sm-2"><h4>Included Products:</h4></div>
    <div class="col-sm-7">
<ol>
    <?php foreach ($form['product_names']['#children']['products'] as $product) print '<li>' . $product . '</li>'; ?>
</ol>
    </div>
</div>
<?php print drupal_render($form['limits']); ?>

<?php print drupal_render($form['price_points']); ?>

<?php print drupal_render($form['comparisons']); ?>

<?php print drupal_render($form['visible_form']); ?>
