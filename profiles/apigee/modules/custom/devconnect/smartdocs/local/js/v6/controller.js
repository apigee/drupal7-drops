/*
 * Copyright (c) 2013, Apigee Corporation. All rights reserved.
 * Apigee(TM) and the Apigee logo are trademarks or
 * registered trademarks of Apigee Corp. or its subsidiaries. All other
 * trademarks are the property of their respective owners.
 * ---------------------------------------------------------------------------------------------------
 * This file handles API Modeling docs page related event handling methods, method to receive oAuth 2 access token and code mirror editor invocation methods.
 * Depends:
 *  jquery_1.7.2.min.js, bootstrap-tooltip.js, codemirror.js, codemirror_javascript.js, codemirror_xml.js, prism.js, base64_min.js.
 */
var apiModelCommon; // An Instance of a 'Apigee.APIModel.Common' class.
var apiModelMethods; // An Instance of a 'Apigee.APIModel.Methods' class.
var apiModelInlineEdit; // An Instance of a 'Apigee.APIModel.InlineEdit' class.
var apiModelEditor;
Apigee.APIModel.authUrl = "";
Apigee.APIModel.proxyURL; // Stores proxy URL.

jQuery(function() {
    apiModelEditor = new Apigee.APIModel.Editor();
    apiModelCommon = new Apigee.APIModel.Common();
    if (localStorage.getItem("unsupportedBrowserFlag") == null) {
        apiModelCommon.showUnsupportedBrowserAlertMessage();
    }
    if (Apigee.APIModel.methodType == "index") { // Create an Instance of a 'Apigee.APIModel.Common' class and call the init method.
        Apigee.APIModel.initIndexEvents();
    } else {
        // Create an instance of 'Apigee.APIModel.Methods' and 'Apigee.APIModel.apiModelCommon' classes.
        apiModelMethods = new Apigee.APIModel.Methods();
        apiModelMethods.init();
        var editModeQueryParam = apiModelCommon.getQueryParam(location.href,"editMode");
        // Create an instance of 'Apigee.APIModel.InlineEdit' class, if there is a query param called 'editMode' with value =1 or 2 in the URL.
        if (editModeQueryParam == "1" || editModeQueryParam == "2") { // Invoke index class object if there is a query param called 'editMode' available in the URL.
          apiModelInlineEdit = new Apigee.APIModel.InlineEdit();
          apiModelInlineEdit.init(parseInt(editModeQueryParam))
        }
    }
});
/*
 * Define Apigee.APIModel.Index class events.
 * Apigee.APIModel.Index does not have any methods other than method name truncating and resource path truncating.
 */
Apigee.APIModel.initIndexEvents = function() {
    // If method name length is more than 20, show the first 17 charecters followed by ellipsis (...).
    jQuery("a[data-role='method_name']").each(function() {
        apiModelCommon.shortenText(jQuery(this),20)
    });
    // If resource name length is more than 25, show the first 22 charecters followed by ellipsis (...).
    jQuery("p[data-role='resource_path']").each(function() {
        apiModelCommon.shortenText(jQuery(this),25)
    });
    jQuery('#index_content').show();
}
/*
 * Event handlers which are called more than once.
 */
Apigee.APIModel.initMethodsAuthDialogsEvents = function() {
    jQuery("a.link_open_basicauth").unbind("click").click(function() {
        apiModelMethods.updateAuthModalFooter("basic_auth_modal");
    });
    jQuery("a.link_open_oauth2").unbind("click").click(function() {
        apiModelMethods.updateAuthModalFooter("oauth2_modal");
    });
    jQuery("a.link_open_customtoken").unbind("click").click(function() {
        apiModelMethods.getCustomTokenCredentials();
        jQuery("[data-role='custom_token_modal']").modal('show');
    });
    jQuery("[data-role='basic_auth_modal']").find(".button_close_modal").unbind("click").click(apiModelCommon.closeAuthModal);
    jQuery("[data-role='basic_auth_modal']").find(".button_save_modal").unbind("click").click(apiModelMethods.saveAuthModal);
    jQuery("[data-role='oauth2_modal']").find(".button_close_modal").unbind("click").click(apiModelCommon.closeAuthModal);
    jQuery("[data-role='oauth2_modal']").find(".button_save_modal").unbind("click").click(apiModelMethods.saveAuthModal);
    jQuery("[data-role='custom_token_modal']").find(".button_close_modal").unbind("click").click(apiModelCommon.closeAuthModal);
    jQuery("[data-role='custom_token_modal']").find(".button_save_modal").unbind("click").click(apiModelMethods.saveAuthModal);
    jQuery("[data-role='basic_auth_container'] .icon-remove").unbind("click").click("basicauth",apiModelMethods.clearSessionStorage);
    jQuery("[data-role='oauth2_container'] .icon-remove").unbind("click").click("oauth2",apiModelMethods.clearSessionStorage);
    jQuery("[data-role='custom_token_container'] .icon-remove").unbind("click").click("customtoken",apiModelMethods.clearSessionStorage);
    jQuery(".authentication .well").unbind("click").click(apiModelMethods.toggleAuthScheme);
    jQuery("#modal_container.modal input").keyup(function(e){
        jQuery(this).removeClass("error");
        jQuery(".modal .error_container").hide().html("");
    });
}
/*
 * Define Apigee.APIModel.Details class events.
 */
Apigee.APIModel.initMethodsPageEvents = function() {
    // Set the tooltip text position.
    jQuery("span").tooltip({
        'selector': '',
        'placement': 'top'
    });
    // Template params related event handlers.
    jQuery("[data-role='method_url_container'] span.template_param")
        .keyup(function(e){
            var rightArrow = (e.which == 39) ? true : false;
            jQuery(this).removeClass("error");
            apiModelMethods.clearErrorContainer();
            apiModelMethods.updateTemplateParamText(jQuery(this));
            apiModelMethods.updateTemplateParamWidth(jQuery(this),rightArrow);
        })
        .keypress(function(e){
            var code = e.keyCode || e.which;
            var rightArrow = (code == 39) ? true : false;
            apiModelMethods.updateTemplateParamWidth(jQuery(this),rightArrow);
        })
        .blur(function(e){
            jQuery(this).text(jQuery.trim(jQuery(this).text()));
        });
    jQuery("[data-role='query-param-list'] input, [data-role='header-param-list'] input, [data-role='body-param-list'] input, [data-role='param-group-list'] input, [data-role='response_errors_list'] input, [data-role='attachments-list'] input").keyup(function(e){
        jQuery(this).removeClass("error");
        apiModelMethods.clearErrorContainer();
    });
    // Send request related
    jQuery("a.link_reset_default").unbind("click").click(apiModelMethods.resetFields);
    jQuery("#send_request").unbind("click").click(apiModelMethods.sendRequest);
    jQuery(".request_response_tabs a").unbind("click").click(apiModelMethods.swapSampleRequestResponseContainer);
    jQuery("ul[data-role='request-payload-link'] a").unbind("click").click(apiModelMethods.toggleRequestPayload);
}
/*
 * Define Apigee.APIModel.DetailsEdit class events.
 */
Apigee.APIModel.initInlineEditAdminAuthEvents = function() {
    // Authentication related event handlers.
    jQuery("[data-role='edit_auth_modal']").find(".button_save_modal").unbind("click").click(apiModelInlineEdit.saveAuthModal);
    jQuery("[data-role='edit_auth_modal']").find(".button_close_modal").unbind("click").click(apiModelCommon.closeAuthModal);
    jQuery("[data-role='confirm_modal']").find(".button_close_modal").unbind("click").click( function() {
      apiModelCommon.closeAuthModal();
      apiModelInlineEdit.resetEditableElement();
      return false;
    });
    jQuery("[data-role='confirm_modal']").find(".button_save_modal").unbind("click").click(apiModelInlineEdit.handleConfirmDialogSave);
}
Apigee.APIModel.inlineEditPageEvents = function() {
    jQuery(".icon-remove").click( function() {
      apiModelInlineEdit.clearAdminAuthDetails();
      location.reload()
    });
    jQuery("a.auth_admin_email").click(apiModelInlineEdit.reOpenAdminAuthDetails);
    // Editable fields event handlers.
    jQuery("[contenteditable='true'], [data-allow-edit='true']").unbind("hover").hover(apiModelInlineEdit.handleEditPropertiesMouseOver, apiModelInlineEdit.handleEditPropertiesMouseOut);
    // Show, Save, Cancel event handlers.
    jQuery("[contenteditable='true'], [data-allow-edit='true'], .resource_description").unbind("click").click(apiModelInlineEdit.handleEditableElementsClick);
    jQuery("a.allow_edit.ok").unbind("click").click(apiModelInlineEdit.makeAPICall);
    jQuery("a.allow_edit.cancel").unbind("click").click(apiModelInlineEdit.resetEditableElement);
    // Document click handler,
    jQuery(document).click(apiModelInlineEdit.documentClickHandler);

}
/**
 * Called after successful oAuth 2 dance.
 * Constructs JSON object and calls the 'Apigee.APIModel.Details' class setOAuth2Credentials method.
 */
setAccessTokenAndLocation = function(errorCode, errorMessage, accessToken, accessTokenType , accessToeknParamName, proxyURL) {
    var oauth2Credentials = {};
    oauth2Credentials.errorCode = errorCode;
    oauth2Credentials.errorMessage = errorMessage;
    oauth2Credentials.accessToken  = accessToken;
    oauth2Credentials.accessTokenType = accessTokenType.toLowerCase();
    oauth2Credentials.accessToeknParamName = accessToeknParamName;
    oauth2Credentials.proxyURL = proxyURL;
    apiModelMethods.setOAuth2Credentials(oauth2Credentials);
}

/**
 * Event handler to handle the Oauth token message
 */
function oAuthAccessTokenAndLocationListener(e) {
  var obj = e.data;
  setAccessTokenAndLocation(obj.ERRORCODE, obj.ERRORMESSAGE, obj.ACCESSTOKEN, obj.ACCESSTOKENTYPE, obj.ACCESSTOKENPARAMNAME, obj.PROXYURL);
}
//Add a listener to listen for the oauth token message
window.addEventListener('message', oAuthAccessTokenAndLocationListener, false);
