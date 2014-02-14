/**
 * Created by isaias on 2/11/14.
 */
(function ($) {
    Drupal.behaviors.devconnect_monetization_menu_link = {
        attach: function (context) {
            $(".navbar nav ul li.expanded").addClass("active");
        }
    };
})(jQuery);

