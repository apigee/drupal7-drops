(function ($) {
    Drupal.behaviors.devconnect_monetization_prepaid_balance = {
        attach: function (context) {

            var settings = Drupal.settings.devconnect_monetization_prepaid_balance;

            var container = $("div#topUpBalanceContainer");

            $("label.control-label", container).addClass("topup-modal-label");
            $("div.control-group.form-type-item div.controls", container).addClass("topup-modal-value");

            if (!$("div#edit-current-balance", container).contents().last().is("div.controls")) {
                $("div#edit-current-balance", container).contents().last().wrap("<div class=\"controls topup-modal-value\"></div>");
            }

            if (!$("div#edit-new-balance", container).contents().last().is("div.controls")) {
                $("div#edit-new-balance", container).contents().last().wrap("<div class=\"controls topup-modal-value\"></div>");
            }

            $("a.top-up.trigger").on("click", function(){

                $("input#edit-top-up-amount", container).val("0");

                // Initial state of the form
                if ($(this).attr("balance-id") == undefined) {
                    $("select#edit-currency-id option:first", container).attr("selected", "selected");
                    $("select#edit-currency-id", container).removeAttr("disabled");
                    $("input#edit-top-up-amount", container).attr("disabled", "disabled");
                    $("#edit-submit.btn.btn-primary.form-submit", container).attr("disabled", "disabled");
                    $("div#edit-new-balance div.controls", container).html("0");
                    $("select#edit-currency-id").removeAttr("disabled");
                    $("div#edit-current-balance div.controls", container).html("0");
                }
                else {
                    var currency = Drupal.settings.devconnect_monetization.currencies[$(this).attr("currency")];
                    $("div#edit-current-balance div.controls", container).html(formatCurrencyAmount($(this).attr("current-balance"), currency));
                    $("select#edit-currency-id").attr("disabled", "disabled");
                    $("select#edit-currency-id").val($(this).attr("currency"));
                    $("select#edit-currency-id").trigger("change");
                }
                container.on("hide.bs.modal", function(){
                    if ($("select#edit-currency-id", container).val() != -1) {
                        $("#edit-amount").maskMoney("destroy");
                    }
                    $("div#edit-new-balance", container).removeClass("alert alert-block alert-error error");
                    $("#topup_alert_minimum_required", container).hide();
                    $("#topup_alert_maximum_required", container).hide();
                });
                container.modal({
                    'keyboard' : true,
                    'show' : true
                });
            });

            $("div#edit-new-balance", container).removeClass("alert alert-block alert-error error");

            $("select#edit-currency-id", container).change(function(){
                var selectedCurrency = $(this).val();
                if (selectedCurrency == -1) {
                    $("input#edit-top-up-amount", container).attr("disabled", "disabled");
                    $("#edit-submit.btn.btn-primary.form-submit", container).attr("disabled", "disabled");
                    $("input#edit-top-up-amount", container).maskMoney("destroy");
                }
                else {
                    $("input#edit-top-up-amount", container).removeAttr("disabled");
                    var currency = Drupal.settings.devconnect_monetization.currencies[selectedCurrency];
                    var currentBalance = unmaskCurrencyAmount($("div#edit-current-balance div.controls", container).html(), currency) * 1.0;
                    $("div#edit-current-balance div.controls", container).html(formatCurrencyAmount(currentBalance, currency));
                    var topUpAmount =  unmaskCurrencyAmount($("input#edit-top-up-amount", container).val(), currency) * 1.0;

                    $("input#edit-top-up-amount", container).maskMoney(getMaskMoneyOptions(currency));

                    $("input#edit-top-up-amount", context).maskMoney("mask");

                    var newBalance = currentBalance + topUpAmount;

                    $("div#edit-new-balance div.controls", container).html(formatCurrencyAmount(newBalance, currency));

                    if (newBalance > currentBalance) {
                        $("#edit-submit.btn.btn-primary.form-submit", container).removeAttr("disabled");
                        $("div#edit-new-balance", container).removeClass("alert alert-block alert-error error");
                    }
                    else {
                        $("#edit-submit.btn.btn-primary.form-submit", container).attr("disabled", "disabled");
                        $("div#edit-new-balance", container).addClass("alert alert-block alert-error error");
                    }
                }
            });

            $("input#edit-top-up-amount", container).on("keyup", function(){

                var currency = Drupal.settings.devconnect_monetization.currencies[$("select#edit-currency-id", container).val()];
                var value = $(this).maskMoney("unmasked")[0];

                var currentBalance = unmaskCurrencyAmount($("div#edit-current-balance div.controls", container).html(), currency) * 1.0;
                var newBalance = value + currentBalance;
                $("div#edit-new-balance div.controls", container).html(formatCurrencyAmount(newBalance, currency));

                if ($(this).attr("minimum") != undefined) {
                    if (value < $(this).attr("minimum") * 1.0) {
                        $("#topup_alert_minimum_required", container).show();
                        $("#topup_alert_minimum_required", container).removeClass("hide");
                    }
                    else {
                        $("#topup_alert_minimum_required", container).hide();
                        $("#topup_alert_minimum_required", container).addClass("hide");
                    }
                }

                if ($(this).attr("maximum") != undefined) {
                    if (value > $(this).attr("maximum") * 1.0) {
                        $("#topup_alert_maximum_required", container).show();
                        $("#topup_alert_maximum_required", container).removeClass("hide");
                    }
                    else {
                        $("#topup_alert_maximum_required", container).hide();
                        $("#topup_alert_maximum_required", container).addClass("hide");
                    }
                }

                // Disable submit button in any alert is visible
                if ($("div.alert.hide:visible", container).length) {
                    $("#edit-submit.btn.btn-primary.form-submit", container).attr("disabled", "disabled");
                    $("div#edit-new-balance", container).addClass("alert alert-block alert-error error");
                }
                else {
                    $("#edit-submit.btn.btn-primary.form-submit", container).removeAttr("disabled");
                    $("div#edit-new-balance", container).removeClass("alert alert-block alert-error error");
                }
                if (newBalance > currentBalance) {
                    $("#edit-submit.btn.btn-primary.form-submit", container).removeAttr("disabled");
                    $("div#edit-new-balance", container).removeClass("alert alert-block alert-error error");
                }
                else {
                    $("#edit-submit.btn.btn-primary.form-submit", container).attr("disabled", "disabled");
                    $("div#edit-new-balance", container).addClass("alert alert-block alert-error error");
                }
            });

            $("#edit-submit.btn.btn-primary.form-submit", container).on("click", function(){
                var currency = Drupal.settings.devconnect_monetization.currencies[$("select#edit-currency-id", container).val()];
                $("select#edit-currency-id").removeAttr("disabled");
                $("input#edit-top-up-amount", container).val(unmaskCurrencyAmount($("input#edit-top-up-amount", container).val(), currency));
            });
        }
    };
})(jQuery);

