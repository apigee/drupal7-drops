<?php
/**
 * @file
 *
 * Theme implementation: Template for each forum forum statistics section.
 *
 * Available variables:
 * - $current_total: Total number of users currently online.
 * - $current_users: Number of logged in users.
 * - $current_guests: Number of anonymous users.
 * - $online_users: List of logged in users.
 * - $topics: Total number of nodes (threads / topics).
 * - $posts: Total number of nodes + comments.
 * - $users: Total number of registered active users.
 * - $latest_users: Linked user names of latest active users.
 */
?>

<div class="panel panel-default">
  <div class="panel-heading">
    <h3><?php print t("What's Going On?"); ?></h3>
  </div>
  <div class="table-responsive">
    <table class="table table-condensed">
      <tbody>
      <tr>
        <td><strong><?php print t('Currently active users:'); ?></strong></td>
        <td><?php print $current_total; ?></td>
      </tr>
      <tr>
        <td><strong><?php print t("Online Users:"); ?></strong></td>
        <td><?php print $online_users; ?></td>
      </tr>
      <tr>
        <td><strong><?php print t('Topics:'); ?></strong></td>
        <td><?php print $topics; ?></td>
      </tr>
      <tr>
        <td><strong><?php print t('Posts:'); ?></strong></td>
        <td><?php print $posts; ?></td>
      </tr>
      <tr>
        <td><strong><?php print t('Users:'); ?></strong></td>
        <td><?php print $users; ?></td>
      </tr>
      <tr>
        <td><strong><?php print t('Latest Members:'); ?></strong></td>
        <td><?php print $latest_users; ?></td>
      </tr>
      </tbody>
    </table>
  </div>
  <div class="panel-footer"></div>
</div>