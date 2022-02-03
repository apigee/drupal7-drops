(function($) {

/**
 * Disable the continue buttons in the checkout process once they are clicked
 * and provide a notification to the user.
 */
Drupal.behaviors.commerceCheckout = {
  attach: function (context, settings) {
    $('input.checkout-continue', context).bind('click', Drupal.commerceCheckout.disableContinue);
  },
    detach: function (context, settings, trigger) {
    $('input.checkout-continue', context).unbind('click', Drupal.commerceCheckout.disableContinue);
  }
}

  /**
   * Commerce checkout utility functions.
   */
  Drupal.commerceCheckout = Drupal.commerceCheckout || {
    /**
     * Disable the continue button when clicked to avoid multiple submissions.
     */
    disableContinue: function (event) {
      var $this = $(this);
      $this.addClass('checkout-processed');
      $this.clone().insertAfter(this).attr('disabled', true).next().removeClass('element-invisible');
      $this.hide();
      $('form#' + $this[0].form.id).one('submit', Drupal.commerceCheckout.enableContinue);
    },
    /**
     * Re-enable the continue button if the submit is prevented/cancelled.
     */
    enableContinue: function (event) {
      if (event.isDefaultPrevented()) {
        var $continue = $('input.checkout-continue.checkout-processed');
        $($continue[0]).removeClass('checkout-processed').show().next().remove();
        $($continue[0]).siblings('span.checkout-processing').addClass('element-invisible');
      }
    },
  };

})(jQuery);
