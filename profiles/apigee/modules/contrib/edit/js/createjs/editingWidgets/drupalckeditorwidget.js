/**
 * @file
 * CKEditor-based Create.js widget for processed text content in Drupal.
 */
(function ($, Drupal) {

"use strict";

// This value needs to be set before ckeditor.js is loaded (when ckeditor.js
// is loaded dynamically and when using jQuery <1.9).
// @see http://bugs.jquery.com/ticket/11795#comment:20
window.CKEDITOR_BASEPATH = Drupal.settings.edit.ckeditor.basePath;


// @todo D8: use jQuery UI Widget bridging.
// @see http://drupal.org/node/1874934#comment-7124904
jQuery.widget('DrupalEditEditor.ckeditor', jQuery.Create.editWidget, {

  textFormat: null,
  textFormatHasTransformations: null,
  textEditor: null,

  /**
   * Implements Create.editWidget.getEditUISettings.
   */
  getEditUISettings: function () {
    return { padding: true, unifiedToolbar: true, fullWidthToolbar: true };
  },

  /**
   * Implements jQuery.widget._init.
   *
   * @todo D8: Remove this.
   * @see http://drupal.org/node/1874934
   */
  _init: function () {},

  /**
   * Implements Create.editWidget._initialize.
   */
  _initialize: function () {
    var propertyID = Drupal.edit.util.calcPropertyID(this.options.entity, this.options.property);
    var metadata = Drupal.edit.metadataCache[propertyID].custom;

    this.textFormat = metadata.format;
    this.textFormatHasTransformations = metadata.formatHasTransformations;
    this.ckeditorSettings = metadata.ckeditorSettings;
    // @todo use Drupal.settings.edit.ckeditor.editorSettings[this.textFormat] ???
  },

  /**
   * Implements Create.editWidget.stateChange.
   */
  stateChange: function (from, to) {
    var that = this;
    switch (to) {
      case 'inactive':
        break;

      case 'candidate':
        // Detach the text editor when entering the 'candidate' state from one
        // of the states where it could have been attached.
        if (from !== 'inactive' && from !== 'highlighted') {
            this._ckeditor_detach(this.element.get(0), 'unload');
        }
        break;

      case 'highlighted':
        break;

      case 'activating':
        // When transformation filters have been been applied to the processed
        // text of this field, then we'll need to load a re-processed version of
        // it without the transformation filters.
        if (this.textFormatHasTransformations) {
          var propertyID = Drupal.edit.util.calcPropertyID(this.options.entity, this.options.property);
          this._getUntransformedText(propertyID, this.element, function (untransformedText) {
            that.element.html(untransformedText);
            that.options.activated();
          });
        }
        // When no transformation filters have been applied: start WYSIWYG
        // editing immediately!
        else {
          this.options.activated();
        }
        break;

      case 'active':
        this._ckeditor_attachInlineEditor(
          this.element.get(0),
          this.ckeditorSettings,
          this.toolbarView.getMainWysiwygToolgroupId(),
          this.toolbarView.getFloatedWysiwygToolgroupId()
        );
        // Set the state to 'changed' whenever the content has changed.
        this._ckeditor_onChange(this.element.get(0), function (html) {
          that.options.changed(html);
        });
        break;

      case 'changed':
        break;

      case 'saving':
        break;

      case 'saved':
        break;

      case 'invalid':
        break;
    }
  },

  /**
   * Loads untransformed text for a given property.
   *
   * More accurately: it re-processes processed text to exclude transformation
   * filters used by the text format.
   *
   * @param String propertyID
   *   A property ID that uniquely identifies the given property.
   * @param jQuery $editorElement
   *   The property's PropertyEditor DOM element.
   * @param Function callback
   *   A callback function that will receive the untransformed text.
   *
   * @see \Drupal\editor\Ajax\GetUntransformedTextCommand
   */
  _getUntransformedText: function (propertyID, $editorElement, callback) {
    // Create a Drupal.ajax instance to load the form.
    Drupal.ajax[propertyID] = new Drupal.ajax(propertyID, $editorElement, {
      url: Drupal.edit.util.buildUrl(propertyID, Drupal.settings.edit.ckeditor.getUntransformedTextURL),
      event: 'edit-internal.edit-ckeditor',
      submit: { nocssjs : true },
      progress: { type : null } // No progress indicator.
    });
    // Implement a scoped editCKEditorGetUntransformedText AJAX command: calls
    // the callback.
    Drupal.ajax[propertyID].commands.editCKEditorGetUntransformedText = function(ajax, response, status) {
      callback(response.data);
      // Delete the Drupal.ajax instance that called this very function.
      delete Drupal.ajax[propertyID];
      $editorElement.off('edit-internal.edit-ckeditor');
    };
    // This will ensure our scoped editCKEditorGetUntransformedText AJAX command
    // gets called.
    $editorElement.trigger('edit-internal.edit-ckeditor');
  },

  // @see Drupal 8's Drupal.editors.ckeditor.attachInlineEditor().
  _ckeditor_attachInlineEditor: function (element, ckeditorSettings, mainToolbarId, floatedToolbarId) {
    this._ckeditor_loadExternalPlugins(ckeditorSettings);

    var settings = $.extend(true, {}, ckeditorSettings);

    // If a toolbar is already provided for "true WYSIWYG" (in-place editing),
    // then use that toolbar instead: override the default settings to render
    // CKEditor UI's top toolbar into mainToolbar, and don't render the bottom
    // toolbar at all. (CKEditor doesn't need a floated toolbar.)
    if (mainToolbarId) {
      var settingsOverride = {
        removePlugins: 'floatingspace,elementspath',
        sharedSpaces: {
          top: mainToolbarId
        }
      };
      settings.removePlugins += ',' + settingsOverride.removePlugins;
      settings.sharedSpaces = settingsOverride.sharedSpaces;
    }

    // CKEditor requires an element to already have the contentEditable
    // attribute set to "true", otherwise it won't attach an inline editor.
    element.setAttribute('contentEditable', 'true');

    return !!CKEDITOR.inline(element, settings);
  },

  // @see Drupal 8's Drupal.editors.ckeditor.detach().
  _ckeditor_detach: function (element, trigger) {
    var editor = CKEDITOR.dom.element.get(element).getEditor();
    if (editor) {
      if (trigger === 'serialize') {
        editor.updateElement();
      }
      else {
        editor.destroy();
        element.removeAttribute('contentEditable');
      }
    }
    return !!editor;
  },

  _ckeditor_onChange: function (element, callback) {
    var editor = CKEDITOR.dom.element.get(element).getEditor();
    if (editor) {
      editor.on('change', function () {
        callback(editor.getData());
      });
    }
    return !!editor;
  },
  // @see Drupal 8's Drupal.editors.ckeditor._loadExternalPlugins().
  _ckeditor_loadExternalPlugins: function(ckeditorSettings) {
    if (ckeditorSettings.loadPlugins) {
      if (typeof ckeditorSettings.extraPlugins === 'undefined') {
        ckeditorSettings.extraPlugins = '';
      }
      for (var pluginName in ckeditorSettings.loadPlugins) {
        if (ckeditorSettings.loadPlugins.hasOwnProperty(pluginName)) {
          var name = ckeditorSettings.loadPlugins[pluginName]['name'];
          ckeditorSettings.extraPlugins += (ckeditorSettings.extraPlugins) ? ',' + name : name;
          CKEDITOR.plugins.addExternal(pluginName,  ckeditorSettings.loadPlugins[pluginName].path);
        }
      }
    }
  }

});

})(jQuery, Drupal);
