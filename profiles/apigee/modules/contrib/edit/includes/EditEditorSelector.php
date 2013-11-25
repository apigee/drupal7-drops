<?php

/**
 * @file
 * Contains' Edit's EditorSelector.
 *
 * @see Drupal 8's \Drupal\edit\EditorSelector.
 */

module_load_include('php', 'edit', 'includes/EditEditorSelectorInterface');

/**
 * Selects an in-place editor (an Editor plugin) for a field.
 */
class EditEditorSelector implements EditEditorSelectorInterface {

  /**
   * A list of alternative editor plugin IDs, keyed by editor plugin ID.
   *
   * @var array
   */
  protected $alternatives;

  /**
   * Constructs a new EditEditorSelector.
   */
  public function __construct() {
  }

  /**
   * Implements EditEditorSelectorInterface::getEditor().
   */
  public function getEditor($formatter_type, array $instance, array $items) {
    $editors = edit_editor_list();

    // Build a static cache of the editors that have registered themselves as
    // alternatives to a certain editor.
    if (!isset($this->alternatives)) {
      foreach ($editors as $alternative_editor_id => $editor) {
        if (isset($editor['alternativeTo'])) {
          foreach ($editor['alternativeTo'] as $original_editor_id) {
            $this->alternatives[$original_editor_id][] = $alternative_editor_id;
          }
        }
      }
    }

    // Check if the formatter defines an appropriate in-place editor. For
    // example, text formatters displaying untrimmed text can choose to use the
    // 'direct' editor. If the formatter doesn't specify, fall back to the
    // 'form' editor, since that can work for any field. Formatter definitions
    // can use 'disabled' to explicitly opt out of in-place editing.
    $formatter_settings = $formatter_type['settings'];
    $editor_id = (isset($formatter_settings['edit']) && is_array($formatter_settings['edit']) && isset($formatter_settings['edit']['editor'])) ? $formatter_settings['edit']['editor'] : 'form';
    if ($editor_id === 'disabled') {
      return;
    }
    elseif ($editor_id === 'form') {
      return 'form';
    }

    // No early return, so create a list of all choices.
    $editor_choices = array($editor_id);
    if (isset($this->alternatives[$editor_id])) {
      $editor_choices = array_merge($editor_choices, $this->alternatives[$editor_id]);
    }

    // Make a choice.
    foreach ($editor_choices as $editor_id) {
      $editor = $editors[$editor_id];
      if ($editor['file']) {
        require_once $editor['file path'] . '/' . $editor['file'];
      }
      if ($editor['compatibility check callback']($instance, $items)) {
        return $editor_id;
      }
    }

    // We still don't have a choice, so fall back to the default 'form' editor.
    return 'form';
  }

  /**
   * Implements EditEditorSelectorInterface::getEditorAttachments().
   */
  public function getEditorAttachments(array $editor_ids, array $metadata) {
    $attachments = array();

    // Editor plugins' attachments.
    foreach (array_unique($editor_ids) as $editor_id) {
      $editor = edit_editor_get($editor_id);
      if (!empty($editor['attachments callback'])) {
        if ($editor['file']) {
          require_once  $editor['file path'] . '/' . $editor['file'];
        }
        if (function_exists($editor['attachments callback'])) {
          $attachments[$editor_id] = $editor['attachments callback']($metadata);
          // Allows contrib to declare additional dependencies for the editor.
          drupal_alter('edit_editor_attachments', $attachments[$editor_id], $editor_id, $metadata);
        }
      }
    }

    // JavaScript settings for Edit.
    foreach (array_unique($editor_ids) as $editor_id) {
      $editor = edit_editor_get($editor_id);
      $attachments[] = array(
        // This will be used in Create.js' propertyEditorWidgetsConfiguration.
        'js' => array(
          array(
            'type' => 'setting',
            'data' => array('edit' => array('editors' => array(
              $editor_id => array('widget' => $editor['widget'])
            )))
          )
        )
      );
    }

    return drupal_array_merge_deep_array($attachments);
  }

}
