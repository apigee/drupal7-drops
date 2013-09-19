<?php
/**
 * @file Deal with pathauto problems
 *
 * Pathauto misbehaves. If saving a node programatically, it *always* tries to set an auto-alias even if the global
 * "Do nothing. Leave the old alias intact" was selected.
 *
 * *bitchslap*
 *
 * @see pathauto_nodeapi()
 */
function pathauto_import_html($profile, &$node, $datadoc = NULL) {
  // pathauto requires us to say NO! if we don't want it to override our real wanted path.
  $node->path_aliasauto_perform_alias = FALSE;
}
