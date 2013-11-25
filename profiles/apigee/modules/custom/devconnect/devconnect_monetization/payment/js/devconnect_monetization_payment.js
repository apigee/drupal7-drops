(function ($) {
    Drupal.behaviors.devconnect_monetization_payment = {
        attach: function (context, settings) {
            $("form div#edit-review-pane-1").hide();
            $('#commerce-checkout-form-checkout').ajaxStart(function(){
                $("#edit-continue").attr("disabled", true);
            });
            $('#commerce-checkout-form-checkout').ajaxSuccess(function(){
                $("#edit-continue").attr("disabled", false);
            });
        }
    }
})(jQuery);