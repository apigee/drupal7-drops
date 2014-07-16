(function ($) {
  Drupal.behaviors.apigee = {
    attach: function (context, settings) {
        if (top.frames.length!=0)
            top.location=self.document.location;
    }
  };
})(jQuery);