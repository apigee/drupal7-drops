/**
 * Remove currency mask from amount and returns only the number value
 * e.g. USD $ 10.25 is returned as 10.25
 * @param amount amount with currency format
 * @param currency currency object with the formatting to remove
 * @returns string
 */
function unmaskCurrencyAmount(amount, currency) {
    while (amount.indexOf(currency.thousands_separator) >= 0) {
        amount = amount.replace(currency.thousands_separator, "");
    }
    amount = amount.replace(currency.decimal_separator, ".");
    return amount.replace(/[^+0-9.-]/g, "");
}

/**
 * Given a number and a currency object, it returns the formatted number
 * e.g. 10.25 formatted with USD then $ 10.25 is returned.
 * @param amount Formatted amount
 * @param currency Currency to be used in format
 * @returns string
 */
function formatCurrencyAmount(amount, currency) {
    var pricePieces = (amount * 1.0).toFixed(currency.decimals).split(".");
    var invertedAbsAmount = pricePieces[0].split("");
    var absAmount = "";
    for (var index = 1; invertedAbsAmount.length > 0; index++) {
        var digit = invertedAbsAmount.pop();
        if (digit == '-' || digit == '+') {
            continue;
        }
        absAmount = digit + absAmount;
        if (index % 3 == 0) {
            absAmount = currency.thousands_separator + absAmount;
        }
    }
    var price = absAmount.charAt(0) == currency.thousands_separator ? absAmount.substr(1) : absAmount;

    if (currency.decimals > 0) {
        price += currency.decimal_separator + pricePieces[1];
    }

    var replacements = {
        "@code_before": currency.code_placement == "before" ? currency.code : "",
        "@symbol_before": currency.symbol_placement == "before" ? currency.symbol : "",
        "@price": price,
        "@symbol_after": currency.symbol_placement == "after" ? currency.symbol : "",
        "@code_after": currency.code_placement == "after" ? currency.code : "",
        "@negative": amount < 0 ? "-" : "",
        "@symbol_spacer": currency.symbol_spacer,
        "@code_spacer": currency.code_spacer
    };

    var formattedValue = "@code_before@code_spacer@negative@symbol_before@price@symbol_spacer@symbol_after@code_spacer@code_after";
    for (var replacement in replacements) {
        while (formattedValue.indexOf(replacement) >= 0) {
            formattedValue = formattedValue.replace(replacement, replacements[replacement]);
        }
    }
    return formattedValue.trim();
}

/**
 * Given a currency object, create the options for the
 * jquery-maskmoney plugin.
 * @param amount Formatted amount
 * @param currency Currency to be used in format
 * @returns object of proper jquery-maskmoney options
 */
function getMaskMoneyOptions(currency) {

  var indicator_type = null;
  var prefix_option = "";
  var suffix_option = "";

  // If currency symbol is not hidden, use it, else use currency code.
  if(currency.symbol_placement != "hidden") {
    indicator_type = "symbol";
  }
  else {
    indicator_type = "code";
  }

  // Create indicator string in prefix or suffix position.
  if(currency[indicator_type + "_placement"] == "before") {
    prefix_option = currency[indicator_type] + currency[indicator_type + "_spacer"];
  }
  else {
    suffix_option = currency[indicator_type + "_spacer"] + currency[indicator_type];
  }

  var options = {
      prefix: prefix_option,
      suffix: suffix_option,
      affixesStay: false,
      thousands: currency.thousands_separator,
      decimal: currency.decimal_separator,
      precision: currency.decimals,
      allowZero: false,
      allowNegative: false
  };
  return options;
}