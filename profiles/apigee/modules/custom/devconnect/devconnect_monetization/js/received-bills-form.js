/**
 * Created by isaias on 12/27/13.
 */
/**
 * Created by isaias on 12/27/13.
 */
(function ($) {
    Drupal.behaviors.devconnect_monetization_received_bills = {
        attach: function (context) {
            //$("#edit-search-billing-doc").unwrap().unwrap().parent().show();

            $("#edit-billing-month").change(function(e) {
                $("#edit-search-billing-doc").val("");
                $("#devconnect-monetization-billing-document-form").submit();
            });
        }
    };
})(jQuery);
