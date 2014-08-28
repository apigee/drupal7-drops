(function ($) {
    Drupal.behaviors.smartdocs = {
        attach: function (context, settings) {
            for (var key in Drupal.settings.smartdocs) {
                if (Drupal.settings.smartdocs[key] == 'body-doc') {
                    CodeMirror.fromTextArea(document.getElementById("edit-" + Drupal.settings.smartdocs[key]), {
                        lineNumbers: true,
                        mode: "text/html"
                    });
                } else {
                    CodeMirror.fromTextArea(document.getElementById("edit-" + Drupal.settings.smartdocs[key]), {
                        lineNumbers: true,
                        mode: "javascript"
                    });
                }
            }
        }
    };
})(jQuery);