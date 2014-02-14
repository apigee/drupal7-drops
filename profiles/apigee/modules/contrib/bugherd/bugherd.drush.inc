<?php
/**
 * @file
 * BugHerd drush integration.
 */

/**
 * Implements hook_drush_sql_sync_sanitize().
 */
function bugherd_drush_sql_sync_sanitize($site) {
  $sanitize_query  = "DELETE FROM {variable} WHERE name = 'bugherd_project_key';";
  drush_sql_register_post_sync_op('bugherd-sanitize', dt('Delete BugHerd project key'), $sanitize_query);
}
