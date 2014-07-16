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
var authUrl;
var editor; // A Code mirror editor for the request payload.
var proxyURL; // Stores proxy URL.

jQuery(function() {
    apiModelEditor = new Apigee.APIModel.Editor();
    apiModelCommon = new Apigee.APIModel.Common();
    if (methodType == "index") { // Create an Instance of a 'Apigee.APIModel.Common' class and call the init method.
        initIndexEvents();
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
function initIndexEvents() {
    // If method name length is more than 20, show the first 17 charecters followed by ellipsis (...).
    jQuery("a[data-role='method_name']").each(function() {
        apiModelCommon.shortenText(jQuery(this),20)  
    });
    // If resource name length is more than 25, show the first 22 charecters followed by ellipsis (...).
    jQuery("p[data-role='resource_path']").each(function() {
        apiModelCommon.shortenText(jQuery(this),25)  
    });
    jQuery('div#index_container').show();      
}
/*
 * Event handlers which are called more than once.
 */
function initMethodsAuthDialogsEvents() {
    jQuery("a.link_open_basicauth").unbind("click").click("basic",apiModelCommon.openAuthModal);
    jQuery("a.link_open_oauth2").unbind("click").click("oauth2",apiModelCommon.openAuthModal);
    jQuery("a.link_customtoken").unbind("click").click("customtoken",apiModelCommon.openAuthModal);

    jQuery("#modal_container.modal.basic_auth").find(".button_close_modal").unbind("click").click(apiModelCommon.closeAuthModal);
    jQuery("#modal_container.modal.basic_auth").find(".button_save_modal").unbind("click").click(apiModelMethods.saveAuthModal);
    jQuery("#modal_container.modal.oauth2").find(".button_close_modal").unbind("click").click(apiModelCommon.closeAuthModal);
    jQuery("#modal_container.modal.oauth2").find(".button_save_modal").unbind("click").click(apiModelMethods.saveAuthModal);

    jQuery(".authentication .icon-remove.basicauth").unbind("click").click("basicauth",apiModelMethods.clearSessionStorage); 
    jQuery(".authentication .icon-remove.oauth2").unbind("click").click("oauth2",apiModelMethods.clearSessionStorage);
    jQuery(".authentication .icon-remove.customtoken").unbind("click").click("customtoken",apiModelMethods.clearSessionStorage);

    jQuery(".authentication .well").unbind("click").click(apiModelMethods.toggleAuthScheme);
    jQuery("#modal_container.modal input").keyup(function(e){
        jQuery(this).removeClass("error");
        jQuery(".modal .error_container").hide().html("");
    });  
} 
/*
 * Define Apigee.APIModel.Details class events.
 */
function initMethodsPageEvents() {
    // Set the tooltip text position.
    jQuery("span").tooltip({
        'selector': '',
        'placement': 'top'
    });  
    // Template params related event handlers.                
    jQuery("#resource_URL input").each(function() {
        apiModelMethods.updateTemplateParamWidth(jQuery(this));
    }); 
    jQuery('#resource_URL input').keyup(function(e){
        jQuery(this).removeClass("error");
        apiModelMethods.clearErrorContainer();
        apiModelMethods.updateTemplateParamWidth(jQuery(this));
    }); 
    // Select the value of template param on click.
    jQuery("#resource_URL input").focus(function(){
        this.select();
    });
    // Query/Header params related event handlers.
    jQuery(".method_table .method_details .method_data input").keyup(function(e){
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
function initInlineEditAdminAuthEvents() {
    // Authentication related event handlers.
    jQuery(".modal.edit").find(".button_save_modal").unbind("click").click(apiModelInlineEdit.saveAuthModal);
    jQuery(".modal.edit").find(".button_close_modal").unbind("click").click(apiModelCommon.closeAuthModal);
    jQuery(".modal.confirm").find(".button_close_modal").unbind("click").click( function() {
      apiModelCommon.closeAuthModal();
      apiModelInlineEdit.resetEditableElement();
      return false;
    });
    jQuery(".modal.confirm").find(".button_save_modal").unbind("click").click(apiModelInlineEdit.handleConfirmDialogSave);
}
function inlineEditPageEvents() {
    jQuery(".icon-remove").click( function() {
      apiModelInlineEdit.clearAdminAuthDetails();
      location.reload()
    });  
    jQuery(".set_admin_credentials").click(apiModelCommon.openAuthModal);
    jQuery("a.auth_admin_email").click(apiModelInlineEdit.reOpenAdminAuthDetails);
    // Editable fields event handlers.
    jQuery("#resource_URL").find("input").each(apiModelInlineEdit.setCustomPropertiesToEdiatableInputElements);
    jQuery("[contenteditable='true'], [data-allow-edit='true']").unbind("hover").hover(apiModelInlineEdit.handleEditPropertiesMouseOver, apiModelInlineEdit.handleEditPropertiesMouseOut);
    jQuery("#resource_URL input").unbind("click").click(apiModelInlineEdit.handleTempleParamEdit);    
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
    oauth2Credentials.accessTokenType = accessTokenType;
    oauth2Credentials.accessToeknParamName = accessToeknParamName;
    oauth2Credentials.proxyURL = proxyURL;
    apiModelMethods.setOAuth2Credentials(oauth2Credentials);
}