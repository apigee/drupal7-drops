//Billing Top Up Balance helper functions
function topUpBalance(id, currentBalance, cur) {
  var currency = cur;
  (function($, id, currentBalance) {
    $("#top-up-balance-input").val("");
    if (id != undefined) {
      $("#topUpCurrentBalance").html(currentBalance);
      $("#topUpCurrentBalanceCurrency").html(currency);
      $("#newBalance").html(currentBalance);
      $("#newBalanceCurrency").html(currency);
      $("#currency_title").html(currency);
      $("input[name='currency_id']").val(currency);
      $("#currency_selector").hide();
    } else {
      $("#currency_selector").change(function(e) {
        currency = $("#currency_selector option:selected").val();
        if (currency == "-1") {
          $("#topUpCurrentBalanceCurrency").html("");
          $("#newBalanceCurrency").html("");
          $("#currency_alert").show();
          $("#currency_title").html("");
        } else {
          $("#topUpCurrentBalanceCurrency").html(currency);
          $("#newBalanceCurrency").html(currency);
          $("#currency_alert").hide();
          $("#currency_title").html(currency);
        }
      });
      $("#currency_selector").show();
      $("#topUpCurrentBalance").html("0.00");
      $("#newBalance").html("0.00");
      $("#currency_title").html("");
      $("#topUpCurrentBalanceCurrency").html("");
      $("#newBalanceCurrency").html("");
      $("#currency_alert").show();
      $("#currency_selector option[value='-1']").attr("selected", "selected");
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

      // Verify currency has been selected
      if (id == undefined) {
        currency = $("#currency_selector option:selected").val();
        if (currency == "-1") {
          $("#currency_alert").show();
        } else {
          $("#currency_alert").hide();
        }
      }
    });

    $("#topUp").modal({
      'keyboard' : true,
      'show' : true,
    });
    $("#topup_alert").show();
  })(jQuery, id, currentBalance);
}

function validateBalanceToTopUp() {
  if (jQuery('#currency_alert').is(':visible')) {
    return;
    return false;
  }
  if (jQuery("#topup_alert").is(':visible')) {
    return;
    return false;
  }
  var form = jQuery("#devconnect-monetization-top-up-balance-form");
  jQuery("input[name='top_up_amount']", form).val(
      jQuery("#top-up-balance-input").val());
  if (jQuery("input[name='currency_id']", form).val().length == 0) {
    jQuery("input[name='currency_id']", form).val(
        jQuery("#currency_selector option:selected").val());
  }
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
  $("#top-up-balance-input").keypress(validateDecimalCharonKeyDownEvent);
  $("select option:first-child",
  "#devconnect-monetization-dowload-prepaid-report-form").attr("selected",
      true);
  $("select option:first-child",
  "#devconnect-monetization-dowload-prepaid-report-form").attr("disabled",
      true);

  $("#donwload_previous_prepaid_statements_anchor").click(
      function(e) {
        jQuery("#donwload_previous_prepaid_statements_error_div").hide();
        var currency = jQuery("select[name='account']").val();
        var year = jQuery("select[name='year']").val();
        var month = jQuery("select[name='month']").val();
        var message = "";
        if (currency == "-1") {
          message += "<li>Selected an account</li>";
        }
        if (year == "-1") {
          message += "<li>Selected a year</li>";
        }
        if (month == "-1") {
          message += "<li>Selected a month</li>";
        }
        if (message == "") {
          jQuery("#donwload_previous_prepaid_statements_error_div p").html("");
          var href = 'billing/' + currency + '/' + month + "-" + year;
          $("#donwload_previous_prepaid_statements_anchor").attr("href", href);
        } else {
          e.preventDefault();
          message = "<ul>" + message + "</ul>";
          jQuery("#donwload_previous_prepaid_statements_error_div p").html(
              message);
          jQuery("#donwload_previous_prepaid_statements_error_div").show();
        }
        // do other stuff when a click happens
      });

  $("#edit-billing-month").change(function(e) {
    $("#devconnect-monetization-billing-document-form").submit();
  });

  $.fn.fixBillingMonthSelect = function() {
    $("#devconnect-monetization-dowload-prepaid-report-form select:last")
    .html(
        $(
            "#devconnect-monetization-dowload-prepaid-report-form select:last div")
            .html());
  };

  $("#devconnect-monetization-developer-report-form .date[name='start_date']")
  .datepicker(
      {
        // defaultDate: "+1w",
        changeMonth : true,
        changeYear : true,
        showWeek : true,
        numberOfMonths : 1,
        showAnim : "fold",
        onClose : function(selectedDate) {
          $(
              "#devconnect-monetization-developer-report-form .date[name='end_date']")
              .datepicker("option", "minDate", selectedDate);
        }
      });

  $("#devconnect-monetization-developer-report-form .date[name='end_date']")
  .datepicker(
      {
        // defaultDate: "+1w",
        changeMonth : true,
        changeYear : true,
        numberOfMonths : 3,
        showAnim : "fold",
        onClose : function(selectedDate) {
          $(
              "#devconnect-monetization-developer-report-form .date[name='start_date']")
              .datepicker("option", "maxDate", selectedDate);
        }
      });
  // jQuery("#top-up-balance-input").keyup([{input:
  // jQuery("#top-up-balance-input"), regex :
  // /^[1-9][0-9]*((\.[0-9]{1,2})|\.)?$/, hiddenSelector: '#valid_top_up'}],
  // function(e){debugger;});
});
