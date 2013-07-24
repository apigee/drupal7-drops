(function ($) {
  Drupal.behaviors.apigee = {
    attach: function (context, settings) {
      // Add Classes
      $('.view-home-featured-forum-posts .views-row').addClass('row');
      $('table').addClass('table table-condensed');
      $('.comment-delete a, .comment-edit a, .comment-reply a').addClass('btn');
      $('.faq-question-answer').addClass('accordion-group');
      $('#devconnect-developer-apps-edit-form').addClass('well');
      $('.page-user-register .page-content .container, .page-user-edit .page-content .container').addClass('well');
      $('#user-register-form input.form-submit').wrap('<div class="form-actions" />');
      if ($('body').hasClass('page-user-me-edit')) {
        $('body').addClass('page-user-edit');
      };
      $('.front .block.develop-with-these').addClass('span9');
      $('.front .block.forum-discussions').addClass('span6 offset1');
      $('.front .block.recent-blog-posts').addClass('span7 offset1');

      // CSS
      $('.node-blog.node-teaser:first').css('padding-top','0px');
      $(".collapse").collapse();
      $('.page-comment-reply article.comment ul.links.inline').hide();

      var activeTab = $('[href=' + location.hash + ']');
      activeTab && activeTab.tab('show');

      // alert($(window).width());
    }
  };
})(jQuery);
