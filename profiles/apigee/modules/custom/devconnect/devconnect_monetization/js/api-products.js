(function ($) {
    Drupal.behaviors.devconnect_monetization_developer_apps_form = {
        attach: function (context) {

            // Validates that a user cannot add a monetized product if user has not purchased a plan that contains that product

            // If API PRODUCT WIDGET is Dropdown select box
            if ($("select#api_product").length > 0 ) {

                // API PRODUCT HANDLING is:
                // - Do not associate apps with any API Product.
                // - Associate all apps with one or more Default API Products (configured below).
                // - Allow selection of a single API Product, but do not require it.
                // - Require selection of a single API Product.
                if ($("select#api_product").is(":visible")) {
                    $('select#api_product').on("change", function(e) {
                        var optionSelected = $("option:selected", this);
                        var product_id = this.value;
                        $("#edit-submit").attr("disabled", "disabled");
                        $.ajax({
                            url: Drupal.settings.devconnect_monetization_developer_apps_form.verify_accepted_url + encodeURI(product_id),
                            accept: 'application/json; utf-8',
                            async: false,
                            cache: false,
                            success: function (data) {
                                if (data.found == false) {
                                    $("select#api_product option[value='" + product_id + "']").removeAttr("selected");
                                    $("select#api_product option[value='" + product_id + "']").attr("disabled", "disabled");
                                    $('#dialog-modal p').html(data.message);
                                    $("#dialog-modal").modal({});
                                    $("button#restrict_modal_button").on("click", function(e){
                                        $("#dialog-modal").modal("hide");
                                    });
                                }
                            },
                            complete: function () {
                                $("#edit-submit").removeAttr("disabled");
                            }
                        });
                    });
                }
                // API PRODUCT HANDLING is:
                // - Allow selection of multiple API Products, but do not require any.
                // - Allow selection of multiple API Products, and require at least one.
                else {
                    $('select#api_product').selectList({
                        instance: true,
                        clickRemove: false,
                        onAdd: function (select, product_id, text) {
                            $("#edit-submit").attr("disabled", "disabled");
                            $.ajax({
                                url: Drupal.settings.devconnect_monetization_developer_apps_form.verify_accepted_url + encodeURI(product_id),
                                accept: 'application/json; utf-8',
                                async: false,
                                cache: false,
                                success: function (data) {
                                    if (data.found == false) {
                                        $("select#api_product option[value='" + product_id + "']").removeAttr("selected");
                                        $("select#api_product option[value='" + product_id + "']").attr("disabled", "disabled");
                                        $(".selectlist-item").filter(":last").remove();
                                        $('#dialog-modal p').html(data.message);
                                        $("#dialog-modal").modal({});
                                        $("button#restrict_modal_button").on("click", function(e){
                                            $("#dialog-modal").modal("hide");
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
            }
            // API PRODUCT WIDGET is Checkboxes/radio buttons
            else {
                if (true || $("div#api_product").hasClass("form-checkboxes")) {
                    $("div#api_product input").on("change", function(e){
                        if ($(this).is(":checked")) {
                            var product_id = $(this).val();
                            $("#edit-submit").attr("disabled", "disabled");
                            var $this = this;
                            $.ajax({
                                url: Drupal.settings.devconnect_monetization_developer_apps_form.verify_accepted_url + encodeURI(product_id),
                                accept: 'application/json; utf-8',
                                async: false,
                                cache: false,
                                success: function (data) {
                                    if (data.found == false) {
                                        $($this).removeAttr("checked");
                                        $($this).attr("disabled", "disabled");
                                        $($this).parent().css("text-decoration", "line-through");
                                        $("#dialog-modal p").html(data.message);
                                        $("#dialog-modal").modal({});
                                        $("button#restrict_modal_button").on("click", function(e){
                                            $("#dialog-modal").modal("hide");
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
            }
        }
    };
})(jQuery);