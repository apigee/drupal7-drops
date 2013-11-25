/**
 * @file
 * Form-based Create.js widget for structured content in Drupal.
 */
(function ($, Drupal) {

"use strict";

  // @todo D8: change the name to "form" + use jQuery UI Widget bridging.
  // @see http://drupal.org/node/1874934#comment-7124904
  $.widget('DrupalEditEditor.formEditEditor', $.Create.editWidget, {

    id: null,
    $formContainer: null,

    /**
     * Implements getEditUISettings() method.
     */
    getEditUISettings: function() {
      return { padding: false, unifiedToolbar: false, fullWidthToolbar: false };
    },

    /**
     * Implements jQuery UI widget factory's _init() method.
     *
     * @todo: POSTPONED_ON(Create.js, https://github.com/bergie/create/issues/142)
     * Get rid of this once that issue is solved.
     */
    _init: function() {},

    /**
     * Implements Create's _initialize() method.
     */
    _initialize: function() {},

    /**
     * Makes this PropertyEditor widget react to state changes.
     */
    stateChange: function(from, to) {
      switch (to) {
        case 'inactive':
          break;
        case 'candidate':
          if (from !== 'inactive') {
            this.disable();
            if (from !== 'highlighted') {
              this.element.removeClass('edit-belowoverlay');
            }
          }
          break;
        case 'highlighted':
          break;
        case 'activating':
          this.element.addClass('edit-belowoverlay');
          this.enable();
          break;
        case 'active':
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
     * Enables the widget.
     */
    enable: function () {
      var $editorElement = $(this.options.widget.element);
      var propertyID = Drupal.edit.util.calcPropertyID(this.options.entity, this.options.property);

      // Generate a DOM-compatible ID for the form container DOM element.
      this.id = 'edit-form-for-' + propertyID.replace(/\//g, '_');

      // Render form container.
      this.$formContainer = $(Drupal.theme('editFormContainer', {
        id: this.id,
        loadingMsg: Drupal.t('Loading…')}
      ));
      this.$formContainer
        .find('.edit-form')
        .addClass('edit-editable edit-highlighted edit-editing')
        .attr('role', 'dialog');

      // Insert form container in DOM.
      if ($editorElement.css('display') === 'inline') {
        this.$formContainer.prependTo($editorElement.offsetParent());
        // Position the form container to render on top of the field's element.
        var pos = $editorElement.position();
        this.$formContainer.css('left', pos.left).css('top', pos.top);
      }
      else {
        this.$formContainer.insertBefore($editorElement);
      }

      // Load form, insert it into the form container and attach event handlers.
      var widget = this;
      var formOptions = {
        propertyID: propertyID,
        $editorElement: $editorElement,
        nocssjs: false
      };
      Drupal.edit.util.form.load(formOptions, function(form, ajax) {
        Drupal.ajax.prototype.commands.insert(ajax, {
          data: form,
          selector: '#' + widget.id + ' .placeholder'
        });

        var $submit = widget.$formContainer.find('.edit-form-submit');
        Drupal.edit.util.form.ajaxifySaving(formOptions, $submit);
        widget.$formContainer
          .on('formUpdated.edit', ':input', function () {
            // Sets the state to 'changed'.
            widget.options.changed();
          })
          .on('keypress.edit', 'input', function (event) {
            if (event.keyCode === 13) {
              return false;
            }
          });

        // Sets the state to 'activated'.
        widget.options.activated();
      });
    },

    /**
     * Disables the widget.
     */
    disable: function () {
      if (this.$formContainer === null) {
        return;
      }

      Drupal.edit.util.form.unajaxifySaving(this.$formContainer.find('.edit-form-submit'));
      // Allow form widgets to detach properly.
      Drupal.detachBehaviors(this.$formContainer, null, 'unload');
      this.$formContainer
        .off('change.edit', ':input')
        .off('keypress.edit', 'input')
        .remove();
      this.$formContainer = null;
    }
  });

})(jQuery, Drupal);
