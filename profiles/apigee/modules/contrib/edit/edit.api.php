<?php

/**
 * @file
 * Edit module API
 *
 * @todo
 */

/**
 * Declare editors that can be used with CreateJS.
 *
 * @return array
 */
function hook_edit_editor_info() {
  $editors['form'] = array(
    'widget' => 'drupalFormWidget',
    'compatibility check callback' => '_edit_editor_form_is_compatible',
    'metadata callback' => '_edit_editor_form_metadata',
    'attachments callback' => '_edit_editor_form_attachments',
    'file' => 'includes/editor.form.inc',
    'file path' => drupal_get_path('module', 'edit'),
  );

  return $editors;
}

/**
 * Alter editor definitions.
 *
 * @param $editors
 *   List of all available editors
 */
function hook_edit_editor_info_alter(&$editors) {

}

/**
 * Alter a field metadata that is used by the front-end.
 *
 * @param $metadata
 *   Information used by the front-end to make the field in-place editable.
 * @param array $context
 *   An array with the following key-value pairs:
 *     - 'entity_type': the entity type
 *     - 'entity': the entity object
 *     - 'field': the field instance as returned by field_info_instance()
 *     - 'items': the items of this field on this entity
 */
function hook_edit_editor_metadata_alter(&$metadata, $context) {

}

/**
 * Alter the list of attached files for the editor depending on the fields conf.
 *
 * @param $attachments
 *   #attached array returned by the editor attachments callback.
 * @param $editor_id
 *   ID of the currently used editor.
 * @param $metadata
 *   Informations about all the fields currently used on the page.
 */
function hook_edit_editor_attachments_alter(&$attachments, $editor_id, $metadata) {
  if ($editor === 'ckeditor') {
    $attachments['library'][] = array('mymodule', 'myjslibrary');
  }
}

/**
 * JS API
 */

/**
 * $(document).on('quickedit', function (event, status) {  });
 *
 * 'status' is a bool true when overlay is displayed and quick-edit is ready.
 */
