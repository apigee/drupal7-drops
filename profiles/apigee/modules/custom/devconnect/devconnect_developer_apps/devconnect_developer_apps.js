(function($) {
    Drupal.behaviors.devconnect_developer_apps = {
        attach: function(context) {
            $("form.form-stacked button.btn.primary.action:not(button-processed)").click(function(evt) {
                evt.stopPropagation();
                document.location.href=Drupal.settings.basePath+jQuery(this).attr('data-url');
                return false;
            }).addClass("button-processed");
        },
        detach: function(context) {}
    }
})(jQuery);
