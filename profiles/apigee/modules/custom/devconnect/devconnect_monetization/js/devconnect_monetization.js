(function($) {
  Drupal.behaviors.devconnect_monetization = {
    attach : function(context) {
      $('select#api_product').selectList(
          {
            instance : true,
            clickRemove : false,
            onAdd : function(select, value, text) {
              $("#edit-submit").attr("disabled", "disabled");
              $.ajax({
                url : "/users/me/monetization/accepted-product/"
                    + encodeURI(value),
                accept : 'application/json; utf-8',
                async : true,
                cache : false,
                success : function(data) {
                  if (jQuery('.selectlist-item').filter(':last').find('span.delete').length == 0) {
                    $('.selectlist-item').filter(":last").append('<span class="delete"></span>');
                  }
                  if (data.found == false) {
                    $('.selectlist-item').filter(':last').find('span.delete').trigger('click');
                    $("p", "#dialog-modal").html(data.message);
                    $("#dialog-modal").dialog({
                      height : 180,
                      width : 550,
                      modal : true,
                      buttons : {
                        "Continue" : function() {
                          $("select.form-select.selectlist-select option[value='prod-" + text + "']").attr("disabled", "disabled");
                          $("#dialog-modal").dialog("close");
                        }
                      }
                    });
                  }
                },
                complete : function() {
                  $("#edit-submit").removeAttr("disabled");
                }
              });
            }
          });
      $("#previous_prepaid_stmt_download").click(function(e) {
        if ($("#previous_prepaid_stmt_download").attr("href").length = 1) {
          e.stopPropagation();
          e.preventDefault();
          alert("Select an account and a month");
        }
      });
    },
    detach : function(context) {
    }
  };
})(jQuery);