<?php
/**
 * @file DOCUMENTATION OF Additional per-module functionality used by
 * import_html.
 *
 * Each available module MAY implement a hook_import_html() function to add or
 * manipulate its own data as the node gets saved.
 *
 * @ingroup import_html Import HTML
 * @author Dan Morrison http://coders.co.nz/
 */

/**
 * hook_import_html - prototype function hook
 *
 * Invoke this when creating a node out of nothing. This is invoked from
 * import_html_xhtml_to_node
 *
 * @param $profile array of settings for the import_html process
 * @param $node node object with lots of extra data added to it, only some of which we want to save.
 * @param $datadoc XML doc of the 'simple' node source that we may investigate for extra info if the cooked node object is not good enough
 */
function hook_import_html($profile, &$node, $datadoc = NULL) {
  // If you choose not to import this node at this late stage,
  // further processing can be aborted by setting
  // $node->import_html_exclude = TRUE;

}
/**
 * hook_import_html_after_save - prototype function hook
 *
 * Some functions (alias) can only operate when the nid is already known.
 * Invoke this when the node has been created
 *
 * @param $profile array of settings for the import_html process
 * @param $node node object with lots of extra data added to it, only some of which we want to save.
 * @param $datadoc XML doc of the 'simple' node source that we may investigate for extra info if the cooked node object is not good enough
 */
function hook_import_html_after_save($profile, &$node, $datadoc = NULL) {

}


/**
 * hook_import_html_node_merge - prototype function hook
 *
 * It's tricky to handle merges for some complex modules.
 * Invoke a callback to let (for example) taxonomy figure out
 * how to blend an old node term list with a new node term list.
 *
 * This gets called after hook_import_html() if a pre-existing matching node was
 * found and needs to be updated without over-writing stuff.
 *
 * Values found in hook_import_html will be available in $node. Here you can
 * merge these with $old_node.
 * $profile  may contain rules to use.
 *
 * Update $node by reference.
 *
 *
 * @see taxonomy_import_html_node_merge
 *
 ***/
function hook_import_html_node_merge($profile, &$node, &$old_node) { }


