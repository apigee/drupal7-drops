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

            // Mask recurring/replenishing amount as money fields
            $("input.numeric.currency").each(function(index, value){
                var currency = Drupal.settings.devconnect_monetization.currencies[$(this).attr("currency")];
                var value = ($(this).attr("value") * 1).toFixed(currency.decimals) + '';
                $(this).attr("value", value);
                $(this).maskMoney(getMaskMoneyOptions(currency));
                $(this).maskMoney("mask");
            });


            // Avoid user entering a lower recurring amount than replenish amount
            $("input.recurring[type='text']").blur(function(){
                var recurring = $(this).maskMoney("unmasked")[0];
                var replenish =  $(this).parents("tr").find("input.replenish[type='text']").maskMoney("unmasked")[0];
                if (replenish > recurring) {
                    var currency = Drupal.settings.devconnect_monetization.currencies[$(this).attr("currency")];
                    var minimunUnit = Math.pow(10, currency.decimals);
                    $(this).parents("tr").find("input.replenish[type='text']").maskMoney("mask", recurring - 1 / minimunUnit);
                }
            });

            // Avoid user entering a greater replenish amount than a recurring amount
            $("input.replenish[type='text']").blur(function(){
                var replenish = $(this).maskMoney("unmasked")[0];
                var recurring = $(this).parents("tr").find("input.recurring[type='text']").maskMoney("unmasked")[0];
                if (replenish > recurring) {
                    var currency = Drupal.settings.devconnect_monetization.currencies[$(this).attr("currency")];
                    var minimunUnit = Math.pow(10, currency.decimals);
                    $(this).maskMoney("mask", recurring - 1 / minimunUnit);
                }
            });

            // Remove money masking from masked fields
            $("#edit-submit.btn.btn-primary.form-submit").on("click", function(){
                $("input.numeric.currency").each(function(){
                    var value = $(this).maskMoney("unmasked")[0];
                    $(this).maskMoney("destroy");
                    $(this).val(value);
                });
            });
        }
    };
})(jQuery);
