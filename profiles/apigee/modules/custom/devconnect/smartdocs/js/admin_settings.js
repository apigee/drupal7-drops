(function ($) {

  /**
   *
   */
  Drupal.behaviors.smartdocsSettingsTemplate = {
    attach: function (context, settings) {
      $('#edit-smartdocs-custom-template-file-ajax-wrapper input.form-submit', context).bind('mousedown', Drupal.smartdocsAdmin.showTemplateSaveWarning);

    }

  //insertBefore(self.table).hide().fadeIn('slow')

  };

  /**
   * File upload utility functions.
   */
  Drupal.smartdocsAdmin = Drupal.smartdocsAdmin || {

    /**
     * Prevent file uploads when using buttons not intended to upload.
     */
    showTemplateSaveWarning: function (event) {
      var clickedButton = this;
      var templateDiv = $('.form-item-smartdocs-custom-template-file');

      // Only disable upload fields for Ajax buttons.
      if ($('#smartdocs-template-save-warning').length) {
        return;
      }

      var warnMessage = '<div id="smartdocs-template-save-warning" class="messages warning">' + Drupal.t('The changes to the template will not be saved until the <em>Save configuration</em> button is clicked.') + '</div>';
      $(warnMessage).insertBefore('#edit-smartdocs-custom-template-file-ajax-wrapper').hide().fadeIn('slow');

    }

  };

})(jQuery);
