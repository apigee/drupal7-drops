(function ($) {
    Drupal.behaviors.devconnect_monetization_developer_apps_form = {
        attach: function (context) {

            // Validates that a user cannot add a monetized product
            $('select#api_product').selectList({
                instance: true,
                clickRemove: false,
                onAdd: function (select, product_id, text) {
                    $("#edit-submit").attr("disabled", "disabled");
                    $.ajax({
                        url: Drupal.settings.devconnect_monetization_developer_apps_form.verify_accepted_url + encodeURI(product_id),
                        accept: 'application/json; utf-8',
                        async: true,
                        cache: false,
                        success: function (data) {
                            if ($('.selectlist-item').filter(':last').find('span.delete').length == 0) {
                                $('.selectlist-item').filter(":last").append('<span class="delete"></span>');
                            }
                            if (data.found == false) {
                                $('.selectlist-item').filter(':last').find('span.delete').trigger('click');
                                $("p", "#dialog-modal").html(data.message);
                                $("#dialog-modal").dialog({
                                    height: 180,
                                    width: 550,
                                    modal: true,
                                    buttons: {
                                        "Continue": function () {
                                            $("select.form-select.selectlist-select option[value='prod-" + text + "']").attr("disabled", "disabled");
                                            $("#dialog-modal").dialog("close");
                                        }
                                    }
                                });
                            }
                        },
                        complete: function () {
                            $("#edit-submit").removeAttr("disabled");
                        }
                    });
                }
            });
        }
    };
})(jQuery);