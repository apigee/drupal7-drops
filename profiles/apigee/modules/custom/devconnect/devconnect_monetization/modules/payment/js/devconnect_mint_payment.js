(function ($) {
    Drupal.behaviors.devconnect_mint_payment = {
        attach: function (context, settings) {

            // Enable/Disable "Continue to next step button while making ajax calls
            $("#commerce-checkout-form-review").ajaxStart(function(){
                $("#edit-continue").attr("disabled", "disabled");
            });
            $('#commerce-checkout-form-review').ajaxSuccess(function(){
                $("#edit-continue").removeAttr("disabled");
                $("div.addressfield-container-inline.locality-block").removeAttr("addressfield-container-inline");
            });

            // Enable/Disable recurring_amount and replenish_amount fields in checkout/{order_id}/review page
            $("[name='commerce_payment[payment_details][recurring_payment][is_recurring]']").change(function(){
                var selectedOption = $(this).val();
                $("[name^='commerce_payment[payment_details][recurring_payment]']").each(function(){
                   if ($(this).attr('name') != "commerce_payment[payment_details][recurring_payment][is_recurring]") {
                       if (selectedOption == "isRecurring") {
                           $(this).removeAttr("disabled");
                       }
                       else {
                           $(this).attr("disabled", "disabled");
                       }
                   }
                });
            });

            // Ensure recurring amount is not less than neither minimum amount not replenish amount
            $("input#edit-commerce-payment-payment-details-recurring-payment-recurring-amount").change(function(){
                var recurringAmount = $(this).val() * 1.0;
                var replenishAmount = $("input#edit-commerce-payment-payment-details-recurring-payment-replenish-amount").val() * 1.0;
                var minimum = $(this).attr("minimum") * 1.0;
                var decimals = Drupal.settings.devconnect_monetization.currencies[$(this).attr("currency")].minorUnit;
                if (recurringAmount < minimum) {
                    $(this).val(minimum.toFixed(decimals));
                }
                if (!(recurringAmount > replenishAmount)) {
                    var minimunUnit = Math.pow(10, decimals);
                    $("input#edit-commerce-payment-payment-details-recurring-payment-replenish-amount").val(recurringAmount - 1 / minimunUnit);
                }
            });

            // Ensure replenish amount is not greater than recurring amount
            $("input#edit-commerce-payment-payment-details-recurring-payment-replenish-amount").change(function(){
                var replenishAmount = $(this).val() * 1.0;
                var recurringAmount = $("input#edit-commerce-payment-payment-details-recurring-payment-recurring-amount").val() * 1.0;
                if (!(recurringAmount > replenishAmount)) {
                    var minimum = $(this).attr("minimum") * 1.0;
                    var decimals = Drupal.settings.devconnect_monetization.currencies[$(this).attr("currency")].minorUnit;
                    var minimumUnit = Math.pow(10, decimals);
                    $(this).val(recurringAmount - 1 / minimumUnit);
                }
                if (replenishAmount < 0) {
                    $(this).val(Drupal.settings.devconnect_monetization.currencies[$(this).attr("currency")].smallestUnit);
                }
            });

            // Checkout page
            $("form div#edit-review-pane-1").hide();

            if (Drupal.settings.devconnect_mint_payment !== undefined && Drupal.settings.devconnect_mint_payment.country_refreshed_first_time === undefined) {
                $("select[name='customer_profile_billing[commerce_customer_address][und][0][country]']").once(
                    "select[name='customer_profile_billing[commerce_customer_address][und][0][country]']", function(){
                    if (Drupal.settings.devconnect_mint_payment.initial_address_country != $(this).val()) {
                        $("#edit-continue").attr("disabled", true);

                        $(this).val(Drupal.settings.devconnect_mint_payment.initial_address_country);
                        $(this).trigger("change");
                    }
                });
                $("div.addressfield-container-inline.locality-block").removeAttr("addressfield-container-inline");
                Drupal.settings.devconnect_mint_payment.country_refreshed_first_time = true;
            }

            $('#commerce-checkout-form-checkout').ajaxStart(function(){
                $("#edit-continue").attr("disabled", "disabled");
            });
            $('#commerce-checkout-form-checkout').ajaxSuccess(function(){
                $("div.addressfield-container-inline.locality-block").removeAttr("addressfield-container-inline");
                $("#edit-continue").removeAttr("disabled");
            });
        }
    };
})(jQuery);