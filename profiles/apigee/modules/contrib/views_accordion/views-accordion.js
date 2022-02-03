/**
 * @file
 * Javascript for views-accordion.
 */
Drupal.behaviors.views_accordion = {
  attach: function(context) {
    if(Drupal.settings.views_accordion){
      (function ($) {
        $.each(Drupal.settings.views_accordion, function(id) {
          /* Our view settings */
          var usegroupheader = this.usegroupheader;
          var viewname = this.viewname;
          var display = this.display;

          /* Our panel heightStyle setting */
          var heightStyle = (this.autoheight == 1) ? 'auto' : (this.fillspace == 1 ? 'fill' : 'content');

          /* the selectors we have to play with */
          var displaySelector = '.view-id-' + viewname + '.view-display-id-' + display + ' > .view-content';
          var headerSelector = this.header;

          /* The row count to be used if Row to display opened on start is set to random */
          var row_count = 0;

          /* Prepare our markup for jquery ui accordion */
          $(displaySelector + ' ' + headerSelector + ':not(.ui-accordion-header)').each(function(i){
            // Hash to use for accordion navigation option.
            var hash = "#" + viewname + "-" + display + "-" + i;
            var $this = $(this);
            var $link = $this.find('a');
            // if the header is not already using an anchor tag, add one
            if($link.length == 0){
              // setup anchor tag for navigation
              $this.wrapInner('<a href="' + hash + '"></a>');
            }
            // if there are already, they wont be clickable with js enabled, we'll use them for accordion navigation
            else{
              // @FIXME ?
              // We are currently destroying the original link, though search crawlers will stil see it.
              // Links in accordions are NOT clickable and leaving them would kill deep linking.
              $link.get(0).href = hash;
            }

            // Wrap the accordion content within a div if necessary
            if (!usegroupheader) {
              $this.siblings().wrapAll('<div></div>');
            }
            row_count++;
          });

          if (this.rowstartopen == 'random') {
            this.rowstartopen = Math.floor(Math.random() * row_count);
          }

          var options = {};

          if (this.newoptions) {
            // Slide was removed from jQuery UI easings, provide sensible fallbacks.
            if (this.animated === 'slide' || this.animated === 'bounceslide') {
              this.animated = 'swing';
            }

            /* jQuery UI accordion options format changed for jquery >= 1.9 */
            options = {
              header: headerSelector,
              active: this.rowstartopen,
              collapsible: this.collapsible,
              event: this.event,
              heightStyle: this.autoheight ? 'auto' : this.fillspace ? 'fill' : 'content',
            };
            if (this.animated === false) {
              options.animate = false;
            }
            else {
              options.animate = {
                easing: this.animated,
                duration: this.duration,
              }
            }
          }
          else {
            options = {
              header: headerSelector,
              animated: this.animated,
              active: this.rowstartopen,
              collapsible: this.collapsible,
              autoHeight: this.autoheight,
              heightStyle: heightStyle,
              event: this.event,
              fillSpace: this.fillspace,
              navigation: this.navigation,
              clearstyle: this.clearstyle
            };
          }
          /* jQuery UI accordion call */
          $(displaySelector + ':not(.ui-accordion)').accordion(options);
        });
      })(jQuery);
    }
  }
};
