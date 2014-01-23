(function ($) {
    Drupal.behaviors.devconnect_monetization2 = {
        attach: function (context, settings) {

            $("input.numeric.currency.form-text").on("focus", function(){
                $(this).addClass("non-editing");
            });

            $("input.numeric.currency.form-text").on("blur", function(){
                $(this).removeClass("non-editing");
            });

        }
    };
})(jQuery);