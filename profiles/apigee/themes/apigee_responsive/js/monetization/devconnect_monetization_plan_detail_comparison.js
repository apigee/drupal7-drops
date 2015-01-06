/**
 * Make the proper plan display on the Plan Details & Comparison page
 *
 * Code concept from:
 * Aidan Lister <aidan@php.net>
 * https://github.com/aidanlister/code/blob/master/jquery.stickytabs.js
 */
(function ($) {
  Drupal.behaviors.devportalMonetizationPlanComparison = {

    attach: function(context) {

      // Show the tab corresponding with the hash in the URL, or the first tab.
      var showTabFromHash = function() {
        var hash = window.location.hash;
        var selector = hash ? 'a[href="' + hash + '"]' : 'li.active > a';
        $(selector, '.nav-tabs').tab('show');
      }

      // Set the correct tab when the page loads
      showTabFromHash('.nav-tabs')

      // Set the correct tab when a user uses their back/forward button
      window.addEventListener('hashchange', showTabFromHash, false);

      // Change the URL when tabs are clicked
      $('a', '.nav-tabs').on('click', function(e) {
        history.pushState(null, null, this.href);
      });
    }
  };
})(jQuery);


