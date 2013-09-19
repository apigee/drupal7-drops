<?php
/**
 * @file helper to import extra structural data found in import_html and
 * save it as nodereferences.
 *
 * UNTESTED. Code here as a place to keep it when it was removed from wrapper.
 * module.
 */


/**
 * A callback function invoked by import_html when it's analyzing page
 * semantics.
 *
 * This runs as part of import_htmls node-building phase as it tried to import
 * data for any supported modules.
 *
 * We can auto-detect and populate a Nodereference CCK field from links found
 * within the content.
 *
 * If the referenced node item does not already exist, a placeholder is made on-
 * the-fly.
 *
 * For this to work, the translation template should have tagged all appropriate
 * links as rel='nodereference' because that's what we like for now.
 *
 * IF the scanned content includes such tags, AND the import profile defines
 * which nodereference-type field to use, then these references will be made.
 */
function nodereference_import_html($profile, &$node, $datadoc = NULL) {
  $references = xml_query($datadoc, './/*[@rel = "nodereference"]');
  if (empty($references) || empty($profile['nodereference_field'])) {
    return;
  }
  $references_field = $profile['nodereference_field'];

  // Prepare storage for anything we found.
  if (! $node->$references_field) {
    $node->$references_field = array();
  }
  import_html_debug("
    Scanning import document for referred items.
    Node references will be stored in $references_field.
    New nodes will be created if needed.
  ", array(), WATCHDOG_INFO);

  foreach ($references as $link) {
    if (empty($link)) {
      continue;
    }
    // rels can be multiple - like classes. Split on space
    $reltypes = explode(' ', xml_getAttribute($link, 'rel'));
    $rel_link = xml_getAttribute($link, 'href');
    $value = xml_toString($link);
    import_html_debug("Found a nodereference to " . $rel_link . " content is " . $value, array(), WATCHDOG_INFO);

    // Look for an existing weblink node that represents this URL
    // links_weblink doesn't provide a lookup so we use one of our own.
    if ($rel_node = links_weblink_get_node_by_url($rel_link)) {
      $node->{$references_field}[$rel_node->nid] = array('nid' => $rel_node->nid);
    }
    else {
      import_html_debug("Need to make a new weblinks resource for referred-to linked URL " . $rel_link, array(), WATCHDOG_INFO);
      $rel_node = (object) array(
        'type' => 'weblink',
        'links_weblink_url' => $rel_link,
        'body' => $value,
        'title' => $rel_link,
        'wrapper' => array('use_wrapper' => TRUE),
        #'links_thumbnail' => array('placeholder' => 'placeholder'), // placeholder to stop it getting fetched too quick
        'files' => array(array('filename' => 'links_thumbnail')), // placeholder to stop it getting fetched too quick
      );
      // prevent

      // Create it on-the-fly
      node_save($rel_node);
      // link the parent node to the new child;
      $node->{$references_field}[$rel_node->nid] = array('nid' => $rel_node->nid);
    }

    // While we are here, ensure that the (maybe new) referenced item has a thumbnail.
    // Maybe we (the referring page) know what it is better than it does itself!


    if (! $rel_node->links_thumbnail  ) {
      // If this link node contains an image, that image might be used as our thumbnail!
      // Mostly assuming an image gallery or similar
      $images = xml_query($link, './/*[local-name()="img"]');
      foreach ($images as $image) {
        $src = xml_getAttribute($image, 'src');
      }
      if ($src) {
        import_html_debug('Adding a reference to %src to use as a thumbnail for %url', array('%src' => $src, '%url' => $rel_link), WATCHDOG_INFO);

        // Get file, save it locally
        stream_context_get_default( array('http' => array('referer' => $src)) );
        $source = file_get_contents($src);
        wrapper_attach_file($rel_node, $src, $source, $label = 'links_thumbnail', $profile['allow_bad_urls']);

        node_save($rel_node);
      }
      else {
        #dpm('no image node found in '. xml_toString($link));
      }
    }
  }
}



