<?php

/**
 * @file
 * Theme implementation to show forum legend.
 *
 */
?>

<div class="col-md-12">
  <div class="panel panel-default">
    <div class="panel-heading">
      <h3><?php print t('Legend'); ?></h3>
    </div>
    <div class="panel-body">
      <div class="forum-topic-legend clearfix">
        <div class="topic-icon-new"><?php print t('New posts'); ?></div>
        <div class="topic-icon-default"><?php print t('No new posts'); ?></div>
        <div class="topic-icon-hot-new"><?php print t('Hot topic with new posts'); ?></div>
        <div class="topic-icon-hot"><?php print t('Hot topic without new posts'); ?></div>
        <div class="topic-icon-sticky"><?php print t('Sticky topic'); ?></div>
        <div class="topic-icon-closed"><?php print t('Locked topic'); ?></div>
      </div>
    </div>
    <div class="panel-footer"></div>
  </div>
</div>

 
 