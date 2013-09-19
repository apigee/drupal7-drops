<?php
/**
 * @file API examples for the import_html hooks
 * 
 * Examples of all these hooks are in the extras directory. The hooks were
 * developed as needed as the tasks required by the 'extras' were identified.
 */
 
/**
 * HOOK_import_html is the main callback function.
 * 
 * It runs after all the basic parsing has been done, when we have most of a
 * fully-formed node object ready to save.
 * 
 * The $node object will contain a large amount of extra attributes - values
 * that were added during the semantic data extraction. Most of the supported
 * modules that add their own data to the $node run at this point. It's like a
 * nodeapi presave function. They will run simultaneously with your modules
 * implementation, according to module weight.
 * 
 * $node->import_html_exclude
 * $node->import_html_no_menus
 *
 * At this point in the process, we never know a $node->nid value.
 * 
 * @param $profile An array of settings, containing the rules for this import
 * process.
 * 
 * @param $node The Node object. Many non-standard attributes are attached to
 * this proto-object. Some extra flags (see below) can be set on it to influence
 * later save behavior.
 * 
 * @param @datadoc An XML DOM object, containing the transformed (not raw)
 * version of the source doc.
 */
function hook_import_html($profile, &$node, $datadoc = NULL) {

  // FLAG
  // $node->import_html_exclude
  // will prevent this node from being saved altogether.
  // It can be set to a useful text string that will go into the log, 
  // but it just needs to be set to anything non-null


  // EXAMPLE RULE
  // Prevent certain paths from being processed
  $excluded_paths = array(
    'docs',
    'apps',
    'archive',
  );
  foreach ($excluded_paths as $excluded_path) {
    if (preg_match('|' . $excluded_path . '|', $node->path_alias) ) {
      $node->import_html_exclude = "Matched exclusion pattern '$excluded_path' ";
      import_html_debug("Skipping the import of this page as it matches the exclusion pattern %pattern", array('%pattern' => $excluded_path), WATCHDOG_NOTICE);
    }
  }
  
  // FLAG
  // $node->import_html_no_menus
  // can be set to stop menu.module from adding a menu item. 
  // This overrides the profile default.
    
  // Scan the document for any class that was set on the body element.
  $body_elements = xml_query($datadoc, '//xhtml:body[@class]');
  foreach ($body_elements as $body_element) {
    $classes = explode(' ', xml_getattribute($body_element, 'class'));
  }
  
  // EXAMPLE RULE
  // FAQ pages don't get a menu item
  if (in_array('faq', $classes)) {
    $node->import_html_no_menus = "FAQ pages get no menu item";
  }

  // EXAMPLE RULE
  // Pages of class 'product' need to become a 'product' node type
  if (in_array('product', $classes)) {
    $node->type = "product";
  }

}

/**
 * HOOK_import_html_merge runs when an existing node is being overwritten.
 * 
 * It can be used to selectively copy content across or to avoid conflicts. By
 * default, conflicting arrays of data will be merged, and conflicting strings
 * will be overwritten
 * 
 * This func is only called when a valid old_node was found, and is not called
 * if none exists.
 */
function hook_import_html_merge($profile, &$node, $old_node) {
  // EXAMPLE RULE
  // If the old node already has a title, 
  // do not overwrite it with the imported version.
  // The simplest way to ensure that is to set the new node title to the old node title.
  $node->title = $old_node->title;
}


/**
 * HOOK_import_html_after_save runs after the node has been created.
 * 
 * Some updates can only happen after the node has been saved and we know the
 * new nid.
 * 
 * @see menu_import_html_after_save
 */
function hook_import_html_after_save($profile, &$node, $datadoc = NULL) {

}