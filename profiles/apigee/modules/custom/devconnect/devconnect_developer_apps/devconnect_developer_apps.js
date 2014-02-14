(function($) {
    Drupal.behaviors.devconnect_developer_apps = {
        attach: function(context) {
            $("form.form-stacked button.btn.primary.action:not(button-processed)").click(function(evt) {
                evt.stopPropagation();
                document.location.href=Drupal.settings.basePath+jQuery(this).attr('data-url');
                return false;
            }).addClass("button-processed");

            if (Drupal.settings.devconnect_developer_apps.selectlist == 'true'){
                // jQuery Select
                $('select#api_product').attr('title', 'Select an API Product');

                var sl = $('select#api_product').selectList({
                    instance: true,
                    clickRemove: false,
                    onAdd: function (select, value, text) {
                        $('.selectlist-item').last().append('<span style="margin-top:5px;" class="btn btn-primary pull-right remove-product">Remove</span>');
                    }
                });

                $('.selectlist-list').on('click', '.remove-product', function(event) {
                    sl.remove($(this).parent().data('value'));
                });

                $('.selectlist-item').append('<span style="margin-top:5px;" class="btn btn-primary pull-right remove-product">Remove</span>');
            }

        },
        detach: function(context) {
        }
    }
})(jQuery);
