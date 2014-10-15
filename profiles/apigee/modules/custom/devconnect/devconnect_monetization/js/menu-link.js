/**
 * Created by isaias on 2/11/14.
 */
(function($) {
    Drupal.behaviors.devconnect_monetization_menu_link = {
        attach: function(context) {
            if (Drupal.settings.devconnect_monetization.menu_active) {
                $("ul.primary-nav li span:contains('Monetization')").parent("li").addClass("active");
            }
        }
    };
})(jQuery);

