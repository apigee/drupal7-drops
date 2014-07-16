(function ($) {
    Drupal.behaviors.apigee_responsive = {
        attach: function (context, settings) {
            $('div.modal ul.openid-links li.user-link').hide();
            $('div.modal a[href="#openid-login"]').click(function() {
                $('div.modal div.apigee-responsive-openidhide').show();
            });

            $('li.dropdown').mouseover(function() {
                $(this).addClass('open');
            });

            $('li.dropdown').mouseout(function() {
                $(this).removeClass('open');
            });

            $('.apigee-modal-link-delete a').click(function() {
                var hrefLocation = $(this).attr('href');
                var identifier = $(this).attr('data-target');
                $(identifier).modal();
                if (($(identifier + ' .modal-body #devconnect_developer_application_delete').length == 0)) {
                    $(identifier + ' .modal-body').html('<p class="load-indicator" style="display:none;">' +
                        '<span class="label label-success" style="padding:5px;">Loading...</span></p>');
                    apigeePulsateForever(identifier + ' .modal-body .load-indicator');
                }
                $(identifier + ' .modal-body').load(hrefLocation + ' #devconnect_developer_application_delete', function() {
                    if (!($(identifier + ' .modal-body #devconnect_developer_application_delete').length == 0)) {
                        $(this).remove('.load-indicator');
                    }
                });
                return false;
            });

            $('.apigee-modal-link-edit a').click(function() {
                var hrefLocation = $(this).attr('href');
                var identifier = $(this).attr('data-target');
                $(identifier).modal();
                if (($(identifier + ' .modal-body #devconnect-developer-apps-edit-form').length == 0)) {
                    $(identifier + ' .modal-body').html('<p class="load-indicator" style="display:none;">' +
                        '<span class="label label-success" style="padding:5px;">Loading...</span></p>');
                    apigeePulsateForever(identifier + ' .modal-body .load-indicator');
                    $(identifier + ' .modal-body').load(hrefLocation + ' #devconnect-developer-apps-edit-form', function() {
                        if (!($(identifier + ' .modal-body #devconnect_developer_application_delete').length == 0)) {
                            $(this).remove('.load-indicator');
                        }
                        if (Drupal.settings.devconnect_developer_apps.selectlist == 'true'){
                            var selectItem = identifier + ' .selectlist-item';
                            $(identifier + ' select#api_product').attr('title', 'Select an API Product');

                            var sl = $(identifier + ' select#api_product').selectList({
                                instance: true,
                                clickRemove: false,
                                onAdd: function (select, value, text) {
                                    $(selectItem + ':last').append('<span style="margin-top:5px;" ' +
                                        'class="btn btn-primary pull-right remove-product">Remove</span>');
                                }
                            });

                            $('.selectlist-list').on('click', '.remove-product', function(event) {
                                sl.remove($(this).parent().data('value'));
                            });

                            $(selectItem).append('<span style="margin-top:5px;" ' +
                                'class="btn btn-primary pull-right remove-product">Remove</span>');
                        }
                    });
                }
                return false;
            });

            function apigeePulsateForever(elem) {
                $(elem).fadeTo(500, 1.0);
                $(elem).fadeTo(500, 0.1, function() {
                    apigeePulsateForever(elem);
                });
            }
        }
    };
})(jQuery);