<?php

/**
 * @file
 * Callbacks for the "direct" Create.js PropertyEditor widget.
 */

function _edit_editor_direct_is_compatible(array $instance, array $items) {
  $field = field_info_field($instance['field_name']);

  // This editor is incompatible with multivalued fields.
  $cardinality_allows = $field['cardinality'] == 1;
  // This editor is incompatible with processed ("rich") text fields.
  $no_text_processing = empty($instance['settings']['text_processing']);

  return $cardinality_allows && $no_text_processing;
}

function _edit_editor_direct_metadata(array $instance, array $items) {
  return array();
}

function _edit_editor_direct_attachments() {
  return array(
    'library' => array(
      array('edit', 'edit.editorWidget.direct'),
    ),
  );
}
