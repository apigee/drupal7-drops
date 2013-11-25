(function ($) {

  Drupal.behaviors.environment_indicatorToolbar = {
    attach: function (context, settings) {
      if (typeof(Drupal.settings.environment_indicator) != 'undefined') {
        var $name = $('<div>').addClass('environment-indicator-name-wrapper').html(Drupal.settings.environment_indicator['environment-indicator-name']);
        $('#toolbar div.toolbar-menu', context).once('environment_indicator').prepend($name);
        $('#toolbar div.toolbar-menu', context).css('background-color', Drupal.settings.environment_indicator['toolbar-color']);
        $('#toolbar div.toolbar-menu .item-list', context).css('background-color', changeColor(Drupal.settings.environment_indicator['toolbar-color'], 0.15, true));
        $('#toolbar div.toolbar-menu .item-list ul li:not(.environment-indicator-switcher) a', context).css('background-color', Drupal.settings.environment_indicator['toolbar-color']);
        $('#toolbar div.toolbar-drawer', context).css('background-color', changeColor(Drupal.settings.environment_indicator['toolbar-color'], 0.25));
        $('#toolbar div.toolbar-menu ul li a', context).hover(function () {
          $(this).css('background-color', changeColor(Drupal.settings.environment_indicator['toolbar-color'], 0.1));
        }, function () {
          $(this).css('background-color', Drupal.settings.environment_indicator['toolbar-color']);
          $('#toolbar div.toolbar-menu ul li.active-trail a', context).css('background-color', changeColor(Drupal.settings.environment_indicator['toolbar-color'], 0.1));
        });
        $('#toolbar div.toolbar-menu ul li.active-trail a', context).css('background-image', 'none').css('background-color', changeColor(Drupal.settings.environment_indicator['toolbar-color'], 0.1));
        $('#toolbar div.toolbar-drawer ul li a', context).hover(function () {
          $(this).css('background-color', changeColor(Drupal.settings.environment_indicator['toolbar-color'], 0.1, true));
        }, function () {
          $(this).css('background-color', changeColor(Drupal.settings.environment_indicator['toolbar-color'], 0.25));
          $('#toolbar div.toolbar-drawer ul li.active-trail a', context).css('background-color', changeColor(Drupal.settings.environment_indicator['toolbar-color'], 0.1, true));
        });
        $('#toolbar div.toolbar-drawer ul li.active-trail a', context).css('background-image', 'none').css('background-color', changeColor(Drupal.settings.environment_indicator['toolbar-color'], 0.1, true));
        // Move switcher bar to the top
        var $switcher = $('#toolbar .environment-switcher-container').parent().clone();
        $('#toolbar .environment-switcher-container').parent().remove();
        $('#toolbar').prepend($switcher);
      };
    }
  };
  
  Drupal.behaviors.environment_indicatorAdminMenu = {
    attach: function (context, settings) {
      if (typeof(Drupal.admin) != 'undefined') {
        // Add the restyling behavior to the admin menu behaviors.
        Drupal.admin.behaviors['environment_indicator'] = function (context, settings) {
          $('#admin-menu, #admin-menu-wrapper', context).css('background-color', Drupal.settings.environment_indicator['toolbar-color']);
          $('#admin-menu .item-list', context).css('background-color', changeColor(Drupal.settings.environment_indicator['toolbar-color'], 0.15, true));
          $('#admin-menu .item-list ul li:not(.environment-switcher) a', context).css('background-color', Drupal.settings.environment_indicator['toolbar-color']);
        };
      };
    }
  };

  Drupal.behaviors.environment_indicatorSwitcher = {
    attach: function (context, settings) {
      $('#environment-indicator .environment-indicator-name, #toolbar .environment-indicator-name-wrapper', context).live('click', function () {
        $('#environment-indicator .item-list, #toolbar .item-list', context).slideToggle('fast');
      });
    }
  }

  Drupal.behaviors.environment_indicator_admin = {
    attach: function() {
      // Add the farbtastic tie-in
      if ($.isFunction($.farbtastic)) {
        Drupal.settings.environment_indicator_color_picker = $('#environment-indicator-color-picker').farbtastic('#ctools-export-ui-edit-item-form #edit-color');
      };
    }
  }


})(jQuery);