<?php
/**
 * @file helper to import fields provided by metatag module
 */

/**
 * Adds metatag support.
 * cast keywords into the format that metatag expects
 *
 * Implementation  of HOOK_import_html()
 */
function metatag_import_html($profile, &$node, $datadoc = NULL) {
  import_html_debug("Processing page to add metatag metadata", array(), WATCHDOG_INFO);
  if (!empty($node->import_html['keywords'])) {
    $node->metatags['keywords']['value'] = $node->import_html['keywords'][0];
  }
  if (!empty($node->import_html['description'])) {
    $node->metatags['description']['value'] = $node->import_html['description'][0];
  }
  #import_html_debug_code('metatag :: $node->metatag', $node->metatags);
}
