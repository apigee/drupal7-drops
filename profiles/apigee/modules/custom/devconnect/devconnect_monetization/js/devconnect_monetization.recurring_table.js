(function ($) {
    Drupal.behaviors.devconnect_monetization_recurring_table = {
        attach: function (context) {

            // Switch from Charge per Usage to Recurring
            $("input.recurring[type='checkbox']").change(function () {
                if ($(this).is(":checked")) {
                    $(this).parents('tr').find("input[type='text']").each(function () {
                        $(this).removeAttr("disabled");
                    });
                    $(this).parents('tr').find("input.charge-per-usage[type='checkbox']").prop("checked", false);
                }
            });

            // Switch from Recurring to Charge per Usage
            $("input.charge-per-usage[type='checkbox']").change(function () {
                if ($(this).is(":checked")) {
                    $(this).parents('tr').find("input[type='text']").each(function () {
                        $(this).attr("disabled", "disabled");
                    });
                    $(this).parents('tr').find("input.recurring[type='checkbox']").prop("checked", false);
                }
            });

            // Avoid user entering a lower recurring amount than replenish amount
            $("input.recurring[type='text']").change(function(){
                var recurring = $(this).val() * 1.0;
                var replenish = $(this).parents("tr").find("input.replenish[type='text']").val() * 1.0;
                if (replenish > recurring) {
                    var decimals = Drupal.settings.devconnect_monetization.currencies[$(this).attr("currency")].minorUnit;
                    var minimunUnit = Math.pow(10, decimals);
                    $(this).parents("tr").find("input.replenish[type='text']").val(recurring - 1 / minimunUnit);
                }
            });

            // Avoid user entering a greater replenish amount than a recurring amount
            jQuery("input.replenish[type='text']").change(function(){
                var replenish = jQuery(this).val() * 1.0;
                var recurring = jQuery(this).parents("tr").find("input.recurring[type='text']").val() * 1.0;
                if (replenish > recurring) {
                    var decimals = Drupal.settings.devconnect_monetization.currencies[jQuery(this).attr("currency")].minorUnit;
                    var minimunUnit = Math.pow(10, decimals);
                    jQuery(this).val(recurring - 1 / minimunUnit);
                }
            });
        }
    };
})(jQuery);