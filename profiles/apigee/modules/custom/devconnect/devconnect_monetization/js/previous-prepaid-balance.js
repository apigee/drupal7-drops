(function ($) {
    Drupal.behaviors.devconnect_monetization_previous_prepaid_balance = {
        attach: function (context) {

            var settings = Drupal.settings.devconnect_monetization_previous_prepaid_statement;

            /*
            $("select option:first-child",
                "#devconnect-monetization-download-prepaid-report-form").attr("selected",
                    true);
            $("select option:first-child",
                "#devconnect-monetization-download-prepaid-report-form").attr("disabled",
                    true);
            */

            $.fn.fixBillingMonthSelect = function() {
                var divHtml = $("#devconnect-monetization-download-prepaid-report-form select:last div").html();
                $("#devconnect-monetization-download-prepaid-report-form select:last").html(divHtml);
            };

            $("#download_previous_prepaid_statements_anchor").click(function(e) {
                jQuery("#download_previous_prepaid_statements_error_div").hide();
                var currency = jQuery("select[name='account']").val();
                var year = jQuery("select[name='year']").val();
                var month = jQuery("select[name='month']").val();
                var message = "";
                if (currency == "-1") {
                    message += "<li>" + settings.select_account_message + "</li>";
                }
                if (year == "-1") {
                    message += "<li>" + settings.select_year_message + "</li>";
                }
                if (month == "-1") {
                    message += "<li>" + settings.select_month_message + "</li>";
                }
                if (message == "") {
                    jQuery("#download_previous_prepaid_statements_error_div p").html("");
                    var href = settings.download_url + '/' + currency + '/' + month + "-" + year;
                    $("#download_previous_prepaid_statements_anchor").attr("href", href);
                } else {
                    e.preventDefault();
                    message = "<ul>" + message + "</ul>";
                    jQuery("#download_previous_prepaid_statements_error_div p").html(
                        message);
                    jQuery("#download_previous_prepaid_statements_error_div").show();
                }
            });
        }
    };
})(jQuery);

