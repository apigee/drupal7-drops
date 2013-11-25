Drupal.behaviors.devconnect_monetization_validator = {
    attach: function (context, settings) {
        jQuery("input.numeric.currency").change(function(){
            var value = jQuery(this).val().trim();
            value = (value.match(/^-?\d*(\.\d+)?$/) ? value : 0.0) * 1.0;

            // Get the number of decimal a currency can hold
            var currency = jQuery(this).attr("currency");
            var decimals = Drupal.settings.devconnect_monetization.currencies[currency].minorUnit

            // Grab minimun value
            var minimun = jQuery(this).attr("minimum") * 1.0;
            minimun = isNaN(minimun * 1.0) ? 0 : minimun;

            // Set minimun value if current value is below
            if (value < minimun) {
                jQuery(this).val(minimun.toFixed(decimals));
            }
            else {
                jQuery(this).val(value.toFixed(decimals));
            }
        });
    }
}
