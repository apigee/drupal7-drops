/*
Ckeditor Settings for the apigee_responsive theme.  Remember, these settings will only work from the active theme.  Since this
isn't meant to be an administrative theme, it will not work if you are using a different theme for administrative items.
 */
CKEDITOR.editorConfig = function(config) {
    config.indentClasses = [ 'rteindent1', 'rteindent2', 'rteindent3', 'rteindent4' ];
    config.justifyClasses = [ 'rteleft', 'rtecenter', 'rteright', 'rtejustify' ];
    config.resize_minWidth = 450;
    config.protectedSource.push(/<\?[\s\S]*?\?>/g);
    config.protectedSource.push(/<code>[\s\S]*?<\/code>/gi);
    config.extraPlugins = '';
    config.extraCss = '';
    if (Drupal.settings.ckeditor.theme == "marinelli") {
        config.extraCss += "body{background:#FFF;text-align:left;font-size:0.8em;}";
        config.extraCss += "#primary ol, #primary ul{margin:10px 0 10px 25px;}";
    }
    if (Drupal.settings.ckeditor.theme == "newsflash") {
        config.extraCss = "body{min-width:400px}";
    }
    config.bodyClass = '';
    config.bodyId = '';
    if (Drupal.settings.ckeditor.theme == "marinelli") {
        config.bodyClass = 'singlepage';
        config.bodyId = 'primary';
    }
    config.allowedContent = true;
}
