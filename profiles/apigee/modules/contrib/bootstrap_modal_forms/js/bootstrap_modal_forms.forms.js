(function ($) {

    Drupal.behaviors.bootstrap_modal_forms = {
        attach: function (context, settings) {
            var selectorModal = $('body');
            if(selectorModal.hasClass("bootstrap_modal_forms-processed")) {
                return;
            }
            selectorModal.addClass("bootstrap_modal_forms-processed");
            var currentPath = Drupal.settings.bootstrap_modal_forms_current_path;
            $.each(Drupal.settings.bootstrap_modal_forms, function(key, obj) {
                if (currentPath == obj['url']) {
                    return;
                }
                if (obj['open_onload']) {
                    $("#" + obj['identifier']).modal();
                }
                url = obj['url'];
                url_with_base_path = Drupal.settings.basePath + url;
                $links = $("a[href*='" + url_with_base_path + "'], a[href*='?q=" + url + "']", context);
                $links.click(function() {
                    $modal = Drupal.bootstrap_modal_forms.current_modal = $("#" + obj['identifier']);
                    $modal.not('.hidden-bs-modal-processed').addClass('hidden-bs-modal-processed').on('show.bs.modal',function(){
                        $iframe = $('iframe', $(this));
                        $iframe.attr('src', $iframe.data('src'));
                        $iframe.load(function(){
                            $(this).parents('.modal').find('#' + obj['identifier'] + 'Label').html($(this).contents().find('.bootstrap_page_title').text());
                        });
                    });
                    $modal.modal();
                    return false;
                });
            });
            $("iframe").attr("scrolling","no")
                .load(function() {
                    $(this).css("height", $(this).contents().height() + "px");
                });
        }
    };
    Drupal.bootstrap_modal_forms = Drupal.bootstrap_modal_forms || {};
    Drupal.bootstrap_modal_forms.current_modal = null;
    Drupal.bootstrap_modal_forms.close = function (){
        this.current_modal.modal('hide');
    };
})(jQuery);
