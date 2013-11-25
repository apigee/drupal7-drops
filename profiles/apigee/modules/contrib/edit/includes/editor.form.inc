<?php

/**
 * @file
 * Callbacks for the "form" Create.js PropertyEditor widget.
 */

function _edit_editor_form_is_compatible(array $instance, array $items) {
  return TRUE;
}

function _edit_editor_form_metadata(array $instance, array $items) {
  return array();
}

function _edit_editor_form_attachments() {
  return array(
    'library' => array(
      array('edit', 'edit.editorWidget.form'),
    ),
  );
}
