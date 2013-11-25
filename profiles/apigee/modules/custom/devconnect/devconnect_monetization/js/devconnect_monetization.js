(function ($) {
    Drupal.behaviors.devconnect_monetization = {
        attach: function (context) {

            $("#previous_prepaid_stmt_download").click(function (e) {
                if ($("#previous_prepaid_stmt_download").attr("href").length = 1) {
                    e.stopPropagation();
                    e.preventDefault();
                    alert("Select an account and a month");
                }
            });
        },
    };
})(jQuery);