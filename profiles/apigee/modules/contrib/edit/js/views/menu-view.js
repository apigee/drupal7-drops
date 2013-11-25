/**
 * @file
 * A Backbone View that provides the app-level interactive menu.
 */
(function($, _, Backbone, Drupal) {

"use strict";

Drupal.edit = Drupal.edit || {};
Drupal.edit.views = Drupal.edit.views || {};
Drupal.edit.views.MenuView = Backbone.View.extend({

  events: {
    'click #edit-trigger-link': 'editClickHandler'
  },

  /**
   * Implements Backbone Views' initialize() function.
   */
  initialize: function() {
    _.bindAll(this, 'stateChange');
    this.model.on('change:isViewing', this.stateChange);
    // We have to call stateChange() here because URL fragments are not passed
    // to the server, thus the wrong anchor may be marked as active.
    this.stateChange();
  },

  /**
   * Listens to app state changes.
   */
  stateChange: function() {
    var isViewing = this.model.get('isViewing');
    // Toggle the state of the Toolbar Edit tab based on the isViewing state.
    this.$el.find('#edit-trigger-link')
      .toggleClass('active', !isViewing)
      .attr('aria-pressed', !isViewing);
  },
  /**
   * Handles clicks on the edit tab of the toolbar.
   *
   * @param {Object} event
   */
  editClickHandler: function (event) {
    var isViewing = this.model.get('isViewing');
    // Toggle the href of the Toolbar Edit tab based on the isViewing state. The
    // href value should represent to state to be entered.
    this.$el.find('#edit-trigger-link').attr('href', (isViewing) ? '#edit' : '#view');
    this.model.set('isViewing', !isViewing);
  }
});

})(jQuery, _, Backbone, Drupal);
