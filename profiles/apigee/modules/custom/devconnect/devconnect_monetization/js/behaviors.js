(function ($) {
    Drupal.behaviors.devconnect_monetization2 = {
        attach: function (context, settings) {

            $("a[href$='/monetization'][href^='/users']").attr("href", "/users/me/monetization/billing");

            $("input.numeric.currency.form-text").on("focus", function(){
                $(this).addClass("non-editing");
            });

            $("input.numeric.currency.form-text").on("blur", function(){
                $(this).removeClass("non-editing");
            });

        }
    };
})(jQuery);