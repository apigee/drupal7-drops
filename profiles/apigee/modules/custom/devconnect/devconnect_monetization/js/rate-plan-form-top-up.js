(function ($) {
    Drupal.behaviors.devconnect_monetization_rate_plan_form_top_up = {
        attach: function (context) {
            $("button.btn.form-submit:visible").attr("disabled", "disabled");
            $("input.btn.form-submit:visible").attr("disabled", "disabled");
            var settings = Drupal.settings.devconnect_monetization_rate_plan_form;

            // Retrieve form for purchase plan without insufficient balance
            var url = settings.top_up_form_url + "/" + encodeURIComponent(settings.top_up.plan_id) + "/"
                + encodeURI(
                  + settings.top_up.required_balance + "|"
                  + settings.top_up.start_date.replace(/[/]/g, '-') + "|"
                  + settings.top_up.overlap
                );
            $.ajax({
                url: url,
                async: false,
                dataType: "html",
                success: function (data) {

                    $("div#top-up-wrapper").append(data);

                    var context = $("#topUpPurchase");
                    var elementPresent = $("#edit-amount", context).length > 0;
                    while (elementPresent) {
                        if (!$("#edit-amount", context).parent().is("span.topup-modal-value")) {
                            $("#edit-amount", context).unwrap();
                        }
                        else {
                            $("#edit-amount", context).unwrap();
                            break;
                        }
                    }

                    $("#edit-amount", context).on("focus", function(){
                        $(this).addClass("non-editing");
                    });

                    $("#edit-amount", context).on("blur", function(){
                        $(this).removeClass("non-editing");
                    });

                    $("#edit-amount", context).focus();

                    var currency = Drupal.settings.devconnect_monetization.currencies[$("#edit-amount", context).attr("currency")];
                    $("#edit-amount", context).maskMoney(getMaskMoneyOptions(currency));
                    $("#edit-amount", context).maskMoney("mask");
                    $("#edit-amount", context).focus();

                    if ($("input#edit-amount[minimum]", context).length) {
                        $("span#span-minimum-amount").html(formatCurrencyAmount($("input#edit-amount", context).attr("minimum"), currency));
                    }

                    if ($("input#edit-amount[maximum]", context).length) {
                        $("span#span-maximum-amount", context).html(formatCurrencyAmount($("input#edit-amount", context).attr("maximum"), currency));
                    }

                    $("input#edit-amount", context).on("keyup", function(){
                        //var value = unmaskCurrencyAmount($(this).val(), currency) * 1.0;
                        var value = $(this).maskMoney("unmasked")[0];

                        var currentBalance = unmaskCurrencyAmount($("span#topUpCurrentBalance", context).html(), currency) * 1.0;
                        var newBalanceUnformatted = value + currentBalance;
                        var newBalance = formatCurrencyAmount(newBalanceUnformatted, currency);

                        $("span#newBalance", context).html(newBalance);

                        if ($(this).attr("minimum") != undefined) {
                            if (newBalanceUnformatted < $(this).attr("minimum") * 1.0) {
                                $("#topup_alert_minimum_required", context).show();
                                $("#topup_alert_minimum_required", context).removeClass("hide");
                            }
                            else {
                                $("#topup_alert_minimum_required", context).hide();
                                $("#topup_alert_minimum_required", context).addClass("hide");
                            }
                        }

                        if ($(this).attr("maximum") != undefined) {
                            if (newBalanceUnformatted > $(this).attr("maximum") * 1.0) {
                                $("#topup_alert_maximum_required", context).show();
                                $("#topup_alert_maximum_required", context).removeClass("hide");
                            }
                            else {
                                $("#topup_alert_maximum_required", context).hide();
                                $("#topup_alert_maximum_required", context).addClass("hide");
                            }
                        }

                        // Disable submit button in any alert is visible
                        if ($("#topup_alert_minimum_required", context).is(":visible")
                            || $("#topup_alert_maximum_required", context).is(":visible")
                        ) {
                            $("#edit-submit", context).attr("disabled", "disabled");
                            $("div#newBalanceWrapper", context).addClass("alert alert-block alert-error error");
                        }
                        else {
                            $("#edit-submit", context).removeAttr("disabled");
                            $("div#newBalanceWrapper", context).removeClass("alert alert-block alert-error error");
                        }
                    });

                    context.on("hide.bs.modal", function(){
                        var value = $("#edit-amount").maskMoney("unmasked");
                        $("#edit-amount").maskMoney("destroy");
                        $("#edit-amount").val(value);
                        $("button.btn.form-submit:visible").removeAttr("disabled");
                        $("input.btn.form-submit:visible").removeAttr("disabled");
                    });

                    context.modal({
                        "keyboard" : true,
                        "show" : true
                    });

                    context.removeClass('hide');

                    $("#edit-submit", "#devconnect-monetization-insufficient-top-up-form").on("click", function(e){
                        $("input#edit-amount", context).val(unmaskCurrencyAmount($("input#edit-amount", context).val(), currency));
                        $("#devconnect-monetization-insufficient-top-up-form").submit();
                    });
                }
            });
        }
    };
})(jQuery);
