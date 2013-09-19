function topUpBalancePurchase(id, currentBalance, cur) {
  var currency = cur;
  (function($, id, currentBalance) {
    $("#top-up-balance-input").val("");
    if (id != null) {
      $("#topUpCurrentBalance").html(currentBalance);
      $("#topUpCurrentBalanceCurrency").html(currency);
      $("#newBalance").html("0.00");
      $("#newBalanceCurrency").html(currency);
      $("#currency_title").html(currency);
      $("input[name='currency_id']").val(currency);
    }
    $("#top-up-balance-input").keyup(function() {
      currentBalance = $("#topUpCurrentBalance").html() - 0;
      var topUpBalance = $(this).val() - 0;
      topUpBalance = isNaN(topUpBalance) ? 0 : topUpBalance;
      var newBalance = currentBalance + topUpBalance;
      $("#newBalance").html(newBalance.toFixed(2));
      $topUp = $(this).val() - 0;

      // Verify top up amount is valid
      if (isNaN($topUp) || $topUp <= 0) {
        $("#topup_alert").show();
      } else {
        $("#topup_alert").hide();
      }

    });

    $("#topUpPurchase").modal({
      'keyboard' : true,
      'show' : true,
    });
    $("#topup_alert").show();
  })(jQuery, id, currentBalance);
}

function validateBalanceToTopUp() {
  if (jQuery("#topup_alert").is(':visible')) {
    return;
  }
  var form = jQuery("#devconnect-monetization-top-up-balance-form");
  jQuery("input[name='top_up_amount']", form).val(
      jQuery("#top-up-balance-input").val());
  form.submit();
}

function restrictRegexOnChangeEvent(input, regex, hiddenSelector) {
  var valueToTest = jQuery(input).val();
  if (jQuery("#top-up-balance-input").val().length == 0
      || regex.test(valueToTest)) {
    jQuery(hiddenSelector).val(parseFloat(valueToTest));
  } else {
    jQuery(input).val(jQuery(hiddenSelector).val());
  }
}

function validateDecimalCharonKeyDownEvent(e) {
  if (e.charCode > 0 && !/\d|\./.test(String.fromCharCode(e.charCode))) {
    e.stopPropagation();
    e.preventDefault();
    return false;
  }
}

jQuery(function($) {
  jQuery("#top-up-balance-input").keypress(validateDecimalCharonKeyDownEvent);
  // jQuery("#top-up-balance-input").keyup([{input:
  // jQuery("#top-up-balance-input"), regex :
  // /^[1-9][0-9]*((\.[0-9]{1,2})|\.)?$/, hiddenSelector: '#valid_top_up'}],
  // function(e){debugger;});
});
