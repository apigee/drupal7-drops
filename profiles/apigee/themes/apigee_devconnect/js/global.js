(function ($) {
  Drupal.behaviors.apigee = {
    attach: function (context, settings) {
      if ($('body').hasClass('page-user-me-edit')) {
        $('body').addClass('page-user-edit');
      };
      // Add Classes
      $('.view-home-featured-forum-posts .views-row').addClass('row');
      $('table').addClass('table table-condensed');
      $('.comment-delete a, .comment-edit a, .comment-reply a').addClass('btn');
      $('.faq-question-answer').addClass('accordion-group');
      $('#devconnect-developer-apps-edit-form').addClass('well');
      $('.page-user-register .page-content .container, .page-user-edit .page-content .container').addClass('well');
      $('#user-register-form input.form-submit').wrap('<div class="form-actions" />');

      // CSS
      $('.node-blog.node-teaser:first').css('padding-top','0px');
      $(".collapse").collapse();
      $('.page-comment-reply article.comment ul.links.inline').hide();

      var activeTab = $('[href=' + location.hash + ']');
      activeTab && activeTab.tab('show');

      // Add Hover effect to menus
      $('ul.nav li.dropdown').hover(function() {
        $(this).find('.dropdown-menu').stop(true, true).delay(50).fadeIn();
      }, function() {
        $(this).find('.dropdown-menu').stop(true, true).delay(50).fadeOut();
      });

      // Monetization Date Picker
      if ($.datepicker != undefined && $(".purchase-plan #datepicker").length) {
        $(".purchase-plan #datepicker").datepicker({
          minDate: -0,
          changeMonth: true,
          changeYear: true
        });
      }
    }
  };
})(jQuery);
