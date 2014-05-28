/*
 * Copyright (c) 2013, Apigee Corporation. All rights reserved.
 * Apigee(TM) and the Apigee logo are trademarks or
 * registered trademarks of Apigee Corp. or its subsidiaries. All other
 * trademarks are the property of their respective owners.
 */
// This file contains API Modeling docs related class defitions.
// This file is depends on JQuery, base64 jQuery plugin.
// This file also use bootstrap tooltip plug in, Codemirror's XML and JSON editor plugin and Prism editor plugin.

var Apigee = Apigee || {}; // Look for a namespace 'Apigee', if it is not there, creates an empty one.
Apigee.APIModel = Apigee.APIModel || {}; // Look for a namespace 'APIModel' under 'Apigee', if it is not there, creates an empty one.
/**
 * This class handles all commonly used functions in APIM docs such as:
 * - make an AJAX call.
 * - Opens authentication modal.
 * - Closes authentication modal.
 * - Validates authentication fields.
 * - Parse and returns JSON.
 * - Get query parameters from a URL.
 * - Escapes special charecters from a string.
 * - Shows error message to user.
 */
Apigee.APIModel.Common = function() {
    // Private properties.
    var self = this; // Keep a reference of the current class when the context of 'this' is changing.
    var authModalPosition; // To hold Authentican modal's postion.

    var MODAL_TOP_CLOSE_BUTTON = '<button aria-hidden="true" data-dismiss="modal" class="close button_close_modal" type="button">x</button>';
    var MODAL_BASIC_AUTH_HEADING = '<h3>Set Authentication</h3>';
    var MODAL_EDIT_ADMIN_AUTH_HEADING = '<h3>Set Organization Admin Credentials</h3>';
    var MODAL_INLINE_EDIT_CONFIRM_HEADING = '<h3>Warning</h3>';
    var MODAL_CUSTOM_TOKEN_HEADING = '<h3>Custom Token</h3>';
    if (typeof apiName != "undefined") {
        var MODAL_OAUTH2_HEADING = '<h3>Request '+ apiName+' permissions</h3>';
    }
    var MODAL_SESSION_WARNING = '<p>Your credentials are saved for the session only.</p>';

    var MODAL_BUTTONS = '<a class="btn btn-primary button_save_modal" href="javascript:void(0)">Save</a><a class="button_close_modal" href="javascript:void(0)">Cancel</a>'
    var MODAL_BUTTONS_EDIT = MODAL_BUTTONS.replace("Cancel","Discard");
    // Private methods
    /**
     * This method validates if an email address is valid or not.
     * @param {String} elementValue An email ID value.
     * @return {Boolean} true if it is a valid email address, otherwise returns false.
     */
    validateEmail = function(elementValue) {
        var flag = false;
        if (jQuery.trim(elementValue).length > 1) { // Chceck if it is empty.
            if (/^[a-zA-Z0-9_\-\+\&\/\$\!\#\%\'\*\=\?\^\`\{\|\}\~]{0,1}([a-zA-Z0-9_\.\-\+\&\/\$\!\#\%\'\*\=\?\^\`\{\|\}\~])+([a-zA-Z0-9_\-\+\&\/\$\!\#\%\'\*\=\?\^\`\{\|\}\~]{0,1})+\@(([a-zA-Z0-9\-])+\.)+([a-zA-Z0-9]{2,4})+$/.test(elementValue)) {
                if(elementValue.indexOf("..")==-1) flag = true;
            }
        }
        return flag;
    };

    // Public methods.
    /**
     * This method makes an AJAX call and handles the success/failure callback.
     * @param {Object} request A request which contains all necessary information to make an AJAX like, method type, URL and so on..,
     * @return {Void} make an AJAX call and handles succces and failure callback.
     */
    this.makeAJAXCall = function(request) {
        jQuery.ajax({
            url:request.url,
            cache: false,
            type:(request.type) ? request.type : "get", // Type of a method, "get" by default.
            data:(request.data) ? request.data : null, // Request payload of a method, "null" by default.
            contentType: (request.contentType) ? request.contentType : "application/x-www-form-urlencoded;charset=utf-8",
            // Set custom headers, if any.
            beforeSend : function(req) {
                if (request.headers) {
                    for (var i=0,l=request.headers.length; i<l; i++) {
                        var header = request.headers[i];
                        req.setRequestHeader(header.name, header.value);
                    }
                }
            },
            // Success callback handler of an AJAX call.
            // Invoke the the request's callback method with the response content.
            success:function(data, textStatus, jqXHR) {
                if (request.dataType != "json") {
                    request.callback(jqXHR.responseText);
                } else {
                    request.callback(data);
                }
            },
            // Error callback handler of an AJAX call.
            // Invoke the request's error callback method, if any. Otherwise call the general callback method.
            error: function(xhr, status, error) {
                if (request.errorCallback) {
                    request.errorCallback(xhr.status);
                } else {
                    request.callback(xhr.responseText);
                }
            },
            // Gets called once an AJAX completes.
            complete: function() {
                jQuery("#working_alert").fadeOut(); // Hide working alert message.
            }
        });
    };
    /**
     * This method opens a authentication modal dialog.
     * @param {String} type Type of Modal, currently we have to authentication modal Admin authentication, send request basic authentication and send request OAuth 2 authentication.
     * Request authentication modal is the default one.
     * @return {Void} opens the authentication modal dialog.
     */
    this.openAuthModal = function(type) {
        type = (typeof type.data == "undefined") ? type : type.data;
        var modalContainer = jQuery("#modal_container.modal");
        var modalContainerHeader = jQuery("#modal_container.modal .modal-header");
        var modalContainerBody = jQuery("#modal_container.modal .modal-body");
        var modalContainerFooter = jQuery("#modal_container.modal .modal-footer");

        if (type == "basic") { // Send request basic auth dialog HTML construction.
            modalContainer.addClass('basic_auth').removeClass('oauth2').removeClass('edit').removeClass('confirm').removeClass('custom_token');
            modalContainerHeader.html(MODAL_TOP_CLOSE_BUTTON+MODAL_BASIC_AUTH_HEADING);
            modalContainerBody.html('<form class="form-horizontal"><div class="error_container"></div>    <div class="control-group"><label class="control-label" for="inputEmail">Email/Username</label><div class="controls"><input  class="form-control" type="text" id="inputEmail" placeholder="Email/Username"/></div></div><div class="control-group"><label class="control-label" for="inputPassword">Password</label><div class="controls"><input class="form-control" type="password" id="inputPassword" placeholder="Password"/></div></div></form>');
            jQuery("#modal_container.modal .modal-footer").html(MODAL_SESSION_WARNING+MODAL_BUTTONS);
            window.initMethodsAuthDialogsEvents();
        } else if (type == "edit") { // Inline edit admin credentials basic auth dialog HTML construction.
            modalContainer.addClass('edit').removeClass('basic_auth').removeClass('oauth2').removeClass('confirm').removeClass('custom_token');
            modalContainerHeader.html(MODAL_TOP_CLOSE_BUTTON+MODAL_EDIT_ADMIN_AUTH_HEADING);
            modalContainerBody.html('<form class="form-horizontal"><div class="error_container"></div>    <div class="control-group"><label class="control-label" for="inputEmail">Email/Username</label><div class="controls"><input  class="form-control" type="text" id="inputEmail" placeholder="Email/Username"/></div></div><div class="control-group"><label class="control-label" for="inputPassword">Password</label><div class="controls"><input class="form-control" type="password" id="inputPassword" placeholder="Password"/></div></div></form>');
            modalContainerFooter.html(MODAL_SESSION_WARNING+MODAL_BUTTONS);
            window.initInlineEditAdminAuthEvents();
        } else if (type == "oauth2") { // Send request oAuth 2 dialog HTML construction.
            modalContainer.addClass('oauth2').removeClass('basic_auth').removeClass('edit').removeClass('confirm').removeClass('custom_token');
            modalContainerHeader.html(MODAL_TOP_CLOSE_BUTTON+MODAL_OAUTH2_HEADING);
            modalContainerBody.html('<div class="content"><p>Making '+apiName+' API requests requires you to grant access to this app.</p><p>You will be directed to '+apiName+' to approve the use of your credentials and then returned to this page.</p><p>You can revoke these permissions at any time.</p></div>');
            modalContainerFooter.html(MODAL_SESSION_WARNING+MODAL_BUTTONS);
            window.initMethodsAuthDialogsEvents();
        } else if (type == "confirm") { // Inline edit unsaved changes confirm dialog HTML construction.
            modalContainer.addClass('confirm').removeClass('basic_auth').removeClass('oauth2').removeClass('edit').removeClass('custom_token');
            modalContainerHeader.html(MODAL_TOP_CLOSE_BUTTON+MODAL_INLINE_EDIT_CONFIRM_HEADING);
            modalContainerBody.html('<div class="content"><p>Your changes have not been saved.</p></div>');
            modalContainerFooter.html(MODAL_BUTTONS_EDIT);
            window.initInlineEditAdminAuthEvents();

        } else if (type == "customtoken") { // Send request basic auth dialog HTML construction.
            modalContainer.addClass('custom_token').removeClass('oauth2').removeClass('edit').removeClass('confirm').removeClass('basic_auth');
            modalContainerHeader.html(MODAL_TOP_CLOSE_BUTTON+MODAL_CUSTOM_TOKEN_HEADING);
            modalContainerBody.html('<form class="form-horizontal"><div class="error_container"></div>    <div class="control-group"><label class="control-label" for="inputName">Name</label><div class="controls"><input class="form-control" type="text" id="inputName" placeholder="Name"/></div></div><div class="control-group"><label class="control-label" for="inputValue">Value</label><div class="controls"><input class="form-control" type="text" id="inputValue" placeholder="Value"/></div></div><div class="control-group"><label class="control-label" for="optHeader">Header</label><div class="controls"><input class="form-control" name="rdoCustomTokenType" value="header" type="radio" id="optHeader" checked="checked"/></div></div><div class="control-group"><label class="control-label" for="optQuery">Query</label><div class="controls"><input class="form-control" name="rdoCustomTokenType" value="query" type="radio" id="optQuery" selected="selected"/></div></div></form>');
            modalContainerFooter.html(MODAL_SESSION_WARNING+MODAL_BUTTONS);
            var custemTokenSession = sessionStorage.revisionsCustomTokenCredentialsDetails;
            window.initMethodsAuthDialogsEvents();
        }
        jQuery("#modal_container.modal").modal('show');
        return false;
    };
    /**
     * This method closes the authentication modal dialog.
     */
    this.closeAuthModal = function() {
        jQuery('#modal_container.modal').modal('hide')
        jQuery("#modal_container.modal input").removeClass("error"); // Remove error class from the input boxes.
        jQuery("#modal_container.modal .error_container").hide().html(""); // Empty the error container and hide it.
        //return false;
    };
    /**
     * This method validates the authentication fileds like email and password.
     * @return {String} empty string if there are no validation errors, otherwise returns the error message which needs to be displayed.
     */
    this.validateBasicAuthFields = function() {
        var errMessage = "";
        userEmail = jQuery.trim(jQuery("#inputEmail").val());
        if (!userEmail.length) { // Check if it is empty.
            jQuery("#inputEmail").addClass("error");
            errMessage += "<span>Email/Username required.</span><br/>";
        }
        if (!jQuery.trim(jQuery("#inputPassword").val()).length) { // Check if it is empty.
            jQuery("#inputPassword").addClass("error");
            errMessage += "<span>Password required.</span><br/>"
        }
        return(errMessage);
    };
    this.validateCustomAuthFields = function() {
        var errMessage = "";
        var inputName = jQuery.trim(jQuery("#inputName").val());
        if (!inputName.length) { // Check if it is empty.
            jQuery("#inputName").addClass("error");
            errMessage += "<span>Name required.</span><br/>";
        }
        if (!jQuery.trim(jQuery("#inputValue").val()).length) { // Check if it is empty.
            jQuery("#inputValue").addClass("error");
            errMessage += "<span>Value required.</span><br/>"
        }
        return(errMessage);
    };
    this.shortenText = function(element,len) {
        var elementVal = element.text();
        if (elementVal.length > len) {
            elementVal = elementVal.substring(0,len-3) + "...";
            element.text(elementVal);
        }
    };
    /**
     * This method parses the given JSON from a string.
     * @param {String} theText A string which needs to convert as JSON,
     * @return {Object} JSON object if the given text is a proper JSON.
     */
    this.parseAndReturn = function(theText) {
        var theJson = '';
        try {
          theJson = jQuery.parseJSON(theText);
        } catch (e) {
          theJson = theText;
        }
        return theJson;
    };
    /**
     * This method fetches query parameter from the given URL.
     * @param {String} queryURL An URL.
     * @param {String} paramName A query parameter name.
     * @return {String} param value if the param name available in the URL, otherwise returns an empty string.
     */
    this.getQueryParam = function(queryURL , paramName) {
        var QueryString = queryURL.split("?") // Get the QueryString from the URL.
        if (QueryString.length > 2 ) {
            QueryString = queryURL.split(/\?(.*)/);
        }
        if ( QueryString.length > 1 ) {
            QueryString = QueryString[1];
            QueryString = QueryString.split( "&" );
            for( var i = 0; i < QueryString.length; i++ ) {
                var queryParam =  QueryString[i].split( "=" ); // Creates a name and value element for each parameter in the QueryString.
                if( queryParam[0] == paramName ) {
                    return queryParam[1]; // Return query param value if query param name match with the given name.
                }
            }
            return ""; // Return empty string, if param name does not match with the the URL.
        }
        return ""; // // Return empty string, if there are no query params in the URL.
    };
    /**
     * This method escapes the special charecters like new line charecter, quotes and .., from a string.
     * @param {String} str A String,
     * @return {String} escaped charecters string.
     */
    this.escapeSpecialChars = function(str) {
        return str.replace(/\\n/g, "\\n")
            .replace(/'/g, "\'")
            .replace(/\\"/g, '\\"')
            .replace(/\\&/g, "\\&")
            .replace(/\\r/g, "\\r")
            .replace(/\\t/g, "\\t")
            .replace(/\\b/g, "\\b")
            .replace(/\\f/g, "\\f");
    };
    /**
     * This method shows error message to the user.
     * @param {String} errorMessage A error message string.
     * @return {Void} displays error message.
     */
    this.showError = function(errorMessage) {
        jQuery("#error_container").html(errorMessage).show();
        jQuery("body").scrollTop(0); // Scroll to page's top position.
    };

};
Apigee.APIModel.Editor = function() {
    var editor; // A Code mirror editor for the request payload.
    /**
     * This method initializes the request payload sample code mirror editor.
     */
    this.initRequestPayloadEditor = function() {
        if (jQuery('[data-role="request-payload-example"]').length) { // Check if request payload example element is available.
            jQuery('[data-role="request-payload-example"]').children("textarea").show();
            bodyPayloadElementValue = jQuery.trim(jQuery('[data-role="request-payload-example"]').find("textarea").val());
            jQuery('.request_payload textarea').val(bodyPayloadElementValue);
            bodyPayloadElement = jQuery('.request_payload textarea');
            if (bodyPayloadElement) { // Set xml/json mode based on the request payload value.
                var modeName = (bodyPayloadElement.data("format") == "application/xml") ? "xml" : "javascript";
                editor = CodeMirror.fromTextArea( jQuery('.request_payload textarea').get(0), {
                    mode: modeName,
                    lineNumbers: true
                });
                if (editor.lineCount() <= 2) {
                    editor.setSize('100%',editor.lineCount()*18);
                } else {
                    editor.setSize('100%',editor.lineCount()*15);
                }

            }
        }
    };
    /*
     * Get the request payload sample editor value.
     * @return {String} Value of a request payload editor.
     */
    this.getRequestPayLoad = function() {
        return editor.getValue();
    };
    /*
     * Set request payload sample editor value.
     * @param {String} payload A request payload value.
     */
    this.setRequestPayLoad = function(payload) {
        editor.setValue(payload);
    };
};
/**
 * This class handles operation page related functions.
 */
 Apigee.APIModel.Methods = function() {
    // Private properties
    var self = this; // Keep a reference of the current class when the context of 'this' is changing.
    // Check if it needed here, bacase it is not used anywhere other then init
    var months = ["January","February","March","April","May","June","July","August","September","October","November","December"]; // Stores all the month's display name.
    var lastModifiedDate; // Last modified date in readable form.
    var methodURLElement; // Holds the resource URL element.
    var isTemplateParamEditorOpen = false; // This is not used anywhere remove it.
    var basicAuth = ""; // Holds basic auth value.
    var userEmail = ""; // Holds user email.
    var authType; // Holds auth type details.
    var rawCode = ""; // Stores response content of the testApi call.
    var bodyContent; // Stores request content of the testApi call.
    var isTemplateParamMissing = false; // To check if template param is missing.
    var templateParamMissing = []; // Stores missing template params.
    var isHeaderParamMissing = false; // To check if header param is missing.
    var headerParamMissing = []; // Stores missing header params.
    var isQueryParamMissing = false; // To check if query param is missing.
    var queryParamMissing = []; // Stores missing query params.
    var requestEditor; // A Prism editor for method's request.
    var responseEditor; // A Prism editor for method's response.
    var oauth2Credentials = {}; // Holds OAuth 2 credential details.
    var custemTokenCredentials = "";
    var selectedAuthScheme = ""; // Holds selected auth scheme name.
    var windowLocation = window.location.href; // Current window URL.
    var apiName = window.apiName; // Stores the apiName rendered from template.
    var revisionNumber = window.revisionNumber; // Stores the revision number rendered from template.
    var targetUrl = "";
    //Private methods.

    constructAuthenticationHTML = function(authType, selectedClass,emailString) {
        var EMPTY_AUTH_TITLE = "Set credentials.";
        var CLOSE_ICON = "";
        var AUTH_TITLE = "Basic Auth";
        if (selectedClass && emailString) {
            EMPTY_AUTH_TITLE = "Reset credentials.";
            CLOSE_ICON = "<i class='icon-remove "+ authType +"' title='Clear authentication details.'></i>";
            selectedClass = ' '+selectedClass;
        } else {
            authClassName = "";
            emailString = "Set..."
        }
        if (authType == "oauth2") {
            AUTH_TITLE = "OAuth 2";
        }
        else if (authType == "customtoken") {
            AUTH_TITLE = "Custom Token";
        }
        return "<div class='well "+ authType + selectedClass +"'><p class='title'>"+AUTH_TITLE+"</p><div class='details'><a class='link_open_"+ authType +"' title='"+EMPTY_AUTH_TITLE+"' href='#"+authType+"_modal' role='button' data-toggle='modal'>"+emailString+"</a>" +CLOSE_ICON + "</div></div>";

    }
    /**
     * This method constructs the basic auth credentials to make Send request API call.
     * @return {Void} displays the basic auth credentials and store the values in local variables.
     */
    constructBasicAuthCredentials = function() {
        if (sessionStorage.revisionsBasicAuthDetails) { // Check if basic auth details stored in session storage.
            // Format of the revisionsBasicAuthDetails - api name@@@revision number@@@basic auth details.
            if (apiName==sessionStorage.revisionsBasicAuthDetails.split("@@@")[0] && revisionNumber==sessionStorage.revisionsBasicAuthDetails.split("@@@")[1]) { // Check if apiName and revison number matches.
                userEmail = sessionStorage.revisionsBasicAuthDetails.split("@@@")[2];
                var emailString = userEmail;
                if (emailString.length > 12) {
                    emailString = emailString.substring(0,12) +"..."; // Trim the email string.
                }
                basicAuth = sessionStorage.revisionsBasicAuthDetails.split("@@@")[3]; // Store to local variable, for further reference.
                var selected = (apiName == sessionStorage.selectedAuthScheme.split("@@@")[0] && revisionNumber == sessionStorage.selectedAuthScheme.split("@@@")[1] && sessionStorage.selectedAuthScheme.split("@@@")[2]== "basicauth") ? "selected" : "";
                jQuery(".authentication").html(constructAuthenticationHTML('basicauth',selected,emailString)); // Display current user's basic auth info.
            } else { // Display default message.
                jQuery(".authentication").html(constructAuthenticationHTML('basicauth'));
            }
        } else { // Display default message.
            jQuery(".authentication").html(constructAuthenticationHTML('basicauth'));
        }
    };
    /**
     * This method constructs the OAuth 2 credentials to make Send request API call.
     * @return {Void} displays OAuth 2 credentials and store the values in local variables.
     */
    constructOAuth2Credentials = function() {
        if (sessionStorage.revisionsOAuth2CredentialsDetails) { // Check if OAuth 2 details stored in session storage.
            // Format of the revisionsBasicAuthDetails - api name@@@revision number@@@oauth 2 details.
            if (apiName==sessionStorage.revisionsOAuth2CredentialsDetails.split("@@@")[0] && revisionNumber==sessionStorage.revisionsOAuth2CredentialsDetails.split("@@@")[1]) { // Check if apiName and revison number matches.
                oauth2Credentials = jQuery.parseJSON(sessionStorage.revisionsOAuth2CredentialsDetails.split("@@@")[2]);
                var selected = (apiName == sessionStorage.selectedAuthScheme.split("@@@")[0] && revisionNumber == sessionStorage.selectedAuthScheme.split("@@@")[1] && sessionStorage.selectedAuthScheme.split("@@@")[2]== "oauth2") ? "selected" : "";
                jQuery(".authentication").append(constructAuthenticationHTML("oauth2",selected,"Authenticated")); // Display current user's OAuth 2 auth info.
            } else { // Display default message.
                jQuery(".authentication").append(constructAuthenticationHTML("oauth2"));
            }
        } else { // Display default message.
            jQuery(".authentication").append(constructAuthenticationHTML("oauth2"));
        }

        jQuery(".authentication").width(jQuery(".authentication .well").length * 150);
    };
    /**
     * This method constructs the Custom Token credentials to make Send request API call.
     * @return {Void} displays OAuth 2 credentials and store the values in local variables.
     */
    constructCustomTokenCrendiails = function() {
        var custemTokenSession = sessionStorage.revisionsCustomTokenCredentialsDetails;
        if (custemTokenSession) { // Check if OAuth 2 details stored in session storage.
            // Format of the revisionsBasicAuthDetails - api name@@@revision number@@@oauth 2 details.
            if (apiName==custemTokenSession.split("@@@")[0] && revisionNumber==custemTokenSession.split("@@@")[1]) { // Check if apiName and revison number matches.
                custemTokenCredentials = custemTokenSession.split("@@@")[2]+ "@@@" + custemTokenSession.split("@@@")[3]+ "@@@" + custemTokenSession.split("@@@")[4];
                var selected = (apiName == sessionStorage.selectedAuthScheme.split("@@@")[0] && revisionNumber == sessionStorage.selectedAuthScheme.split("@@@")[1] && sessionStorage.selectedAuthScheme.split("@@@")[2]== "customtoken") ? "selected" : "";
                jQuery(".authentication").append(constructAuthenticationHTML("customtoken",selected,"Custom Token"));
            } else { // Display default message.
                jQuery(".authentication").append(constructAuthenticationHTML("customtoken"));
            }
        } else { // Display default message.
            jQuery(".authentication").append(constructAuthenticationHTML("customtoken"));
        }

        jQuery(".authentication").width(jQuery(".authentication .well").length * 140);
    };

    // Public methods.
    /**
     * This method invokes the necessary details for the operation page.
     */
    this.init = function() {
        // Convert the timestamp as user friendly time format ('DD Month, YYYY').
        var timeStampElement = jQuery("[data-role='modified-time']");
        lastModifiedDate = new Date(parseInt(jQuery.trim(timeStampElement.text())));
        lastModifiedDate = lastModifiedDate.getDate()+" "+months[lastModifiedDate.getMonth()]+", "+lastModifiedDate.getFullYear();
        timeStampElement.html(lastModifiedDate); // Update the time stamp HTML element.
        // Convert the auth type value as user friendly text.
        var authTypeElement = jQuery("[data-role='auth-type']");
        authType = jQuery.trim(authTypeElement.text());
        if (authType.split(",").length > 1) {
            authType = authType.substr(0,authType.length-1); // Remove the last extra comma symbol.
        }
        authType = authType.replace("BASICAUTH","Basic Auth").replace("CUSTOM","Custom Token").replace("OAUTH1WEBSERVER", "OAuth 1").replace("OAUTH1CLIENTCREDENTIALS", "OAuth 1 Client Credentials").replace("OAUTH2WEBSERVER","OAuth 2").replace("OAUTH2CLIENTCREDENTIALS","OAuth 2 Client Credentials").replace("OAUTH2IMPLICITGRANT","OAuth 2 Implicit Grant Flow").replace("NOAUTH","No auth");

        authTypeElement.html(authType); // Update the auth type HTML element.
        self.updateAuthContainer();
        //Fix for extraneous space in the resource URL.
        var resourceURLString = "";
        jQuery("#resource_URL span").each(function() {
            resourceURLString += '<span data-role="'+ jQuery(this).attr('data-role') + '">' +jQuery(this).html() + '</span>';
        });
        jQuery("#resource_URL").html(resourceURLString);
        //console.log(jQuery("#resource_URL").text());
        // Template parameter releated changes.
        methodURLElement = jQuery("[data-role='method-url']");
        // Add tooltip to template params.
        methodURLElement.html(methodURLElement.html().replace(/\{/g,"<span data-toggle='tooltip' data-original-title=''><input value='{").replace(/\}/g,"}' /><span></span></span>"));
        methodURLElement.find("input").each(function() {
            jQuery(this).siblings("span").html(jQuery(this).val()).attr("data-role",jQuery(this).val());
        });
        // Create a sibling node to each template param and add original value to the siblings.
        // Original value will be used while validating template params.
        jQuery("div[data-role='template-params']").find("p").each(function() {
            var templateParamName = jQuery(this).find("[data-role='name']").html();
            var templateParamDescription = jQuery(this).find("[data-role='description']").html();
            jQuery("[data-toggle='tooltip']").each(function() {
                var curElement = jQuery(this).find("span").data("role");
                if (curElement) {
                    curElement = curElement.substring(1,curElement.length-1);
                    if (curElement == templateParamName) {
                        jQuery(this).attr('data-original-title',templateParamDescription+" Click to edit the value.");
                    }
                }
            });
        });
        // Replace template param values with the values stored in local storage.
        if (localStorage.hasOwnProperty('templateParams')) {
            var templateParams = JSON.parse(localStorage.getItem('templateParams'));
            for (var i=0; i<templateParams.length; i++) {
                var paramName = templateParams[i].name;
                var paramValue = templateParams[i].value;
                jQuery("#resource_URL input").each(function() {
                    var spanElement = jQuery(this).siblings("span");
                    var inputElement = jQuery(this);
                    if(spanElement.attr('data-role') == paramName) {
                        inputElement.val(paramValue);
                        spanElement.html(paramValue);
                    }
                });
            }
        }
        // Create a new custom property called 'data-original-value' in query params and header params value field.
        // Assign the default value to the custom property 'data-original-value'. This value will be used in clicking 'reset' link.
        jQuery("[data-role='query-param-list'],[data-role='header-param-list']").each(function(i, obj) {
            var valueElement = jQuery(this).find("[data-role='value']");
            valueElement.attr('data-original-value',jQuery.trim(valueElement.val()));
        });
        // Remove the last extra comma symbol from category field.
        var categoryElement = jQuery("[data-role='category']");
        var categoryElementValue = jQuery.trim(categoryElement.text());
        if (categoryElementValue.split(",").length > 1) {
            categoryElementValue = categoryElementValue.substr(0,categoryElementValue.length-1); // Remove the last extra comma symbol.
        }
        categoryElement.html(categoryElementValue); // Update the auth type HTML element.
        // Show the request payload docs by default if request payload sample is not available.
        if (jQuery("[data-role='request-payload-docs']").length && !jQuery("[data-role='request-payload-example']").length) {
            jQuery("[data-role='request-payload-docs']").show();
        }
        jQuery("#working_alert").css('left',(jQuery(window).width()/2)- 56); // Set working alert container left position to show in window's center position.
        jQuery('div#content').prepend('<div id="error_container"></div>').show(); // Insert error container HTML element in the page.
        window.apiModelEditor.initRequestPayloadEditor(); // Initialize the request payload sample editor.
        var proxyURLLocation = windowLocation.split("/apimodels/")[0];
        if (typeof Drupal != "undefined" && typeof Drupal.settings != "undefined") {
            proxyURLLocation = Drupal.settings.devconnect_docgen.apiModelBaseUrl +"/v1/o/" + organizationName;
            //proxyURLLocation = "https://jaksapi-prod.apigee.net/smartdocs/v1/o/" + organizationName;
        }
        proxyURLLocation = proxyURLLocation + "/apimodels/proxyUrl"; // Proxy URL location format: https://<domain name>/<alpha/beta/v1>/o/apihub/apimodels/proxyUrl
        self.makeAJAXCall({"url":proxyURLLocation, "callback":self.storeProxyURL}); // Make an AJAX call to retrieve proxy URL to make send request call.
        window.initMethodsPageEvents();
        window.initMethodsAuthDialogsEvents();
    };
    /**
     * Success callback method of a proxy URL AJAX call.
     * @param {Object} data - response content of a proxy URL AJAX call.
     * @return {Void} sets proxy URL value to local variable 'proxyURL'.
     */
    this.storeProxyURL = function(data) {
        data = jQuery.parseJSON(data);
        window.proxyURL = data.proxyUrl;
        window.authUrl = data.authUrl;

    }
    /**
     * Success callback method of a OAuth2 web serser auth URL AJAX call.
     * @param {Object} data - response content of OAuth2 web serser auth URL AJAX call.
     * @return {Void} opens a new window to make OAuth dance.
     */
    this.renderCallbackURL= function(data) {
        window.open(data.authUrl, "oauth2Window", "resizable=yes,scrollbars=yes,status=1,toolbar=1,height=500,width=500");
    };
    /**
     * Error callback method of a OAuth2 web serser auth URL AJAX call.
     * @return {Void} shows error message to the User.
     */
    this.handleOAuth2Failure = function() {
        self.showError("Unable to proceed because of missing OAuth configuration.");
    };
    /**
     * Update template param width based on number of charecter.
     * @param {HTML Element} element - Template parameter input element.
     * @return {Void} sets the input element's width based on number of charecters.
     */
    this.updateTemplateParamWidth= function(element) {
        var value = element.val();
        var size  = value.length;
        if (size == 0) {
            size = 8.3; // average width of a char.
        } else {
            size = Math.ceil(size*8.3); // average width of a char.
        }
        element.siblings("span").html(element.val());
        element.css('width',size); // Set the width.
    };
    /**
     * This method updates the authentication container based on the auth type value to make Send request AJAX call.
     * @return {Void} updates the authentication container.
     */
    this.updateAuthContainer = function() {
        if (authType.indexOf("No auth") != -1) {
            jQuery(".operation_container .authentication").css({'visibility':'hidden'});
        } else {
            if (authType.indexOf("Basic Auth") != -1) { // Show Basic auth info in the operation container.
                if (authType.indexOf(",") == -1) {
                    sessionStorage.selectedAuthScheme = apiName +"@@@"+ revisionNumber + "@@@" + "basicauth";
                    selectedAuthScheme ="basicauth";
                }
                constructBasicAuthCredentials();
            }
            if (authType.indexOf("OAuth 2") != -1) { // Show OAuth 2 info in the operation container.
                if (authType.indexOf(",") == -1) {
                    sessionStorage.selectedAuthScheme = apiName +"@@@"+ revisionNumber + "@@@" +"oauth2";
                    selectedAuthScheme = "oauth2";
                    jQuery(".authentication").html("");
                }
                constructOAuth2Credentials();
            }
            if (authType.indexOf("Custom Token") != -1) { // Show OAuth 2 info in the operation container.
                if (authType.indexOf(",") == -1) {
                    sessionStorage.selectedAuthScheme = apiName +"@@@"+ revisionNumber + "@@@" +"customtoken";
                    selectedAuthScheme = "customtoken";
                    jQuery(".authentication").html("");
                }
                constructCustomTokenCrendiails();
            }
            window.initMethodsAuthDialogsEvents();
        }
        // Hide send request related elements, if a method does have Basic Auth, OAuth 2 or No Auth.
        /*if (authType.indexOf("Basic Auth") == -1  && authType.indexOf("OAuth 2") == -1 && authType.indexOf("No auth") == -1) { // Check whether Basic auth, OAuth 2, No Auth is not available.
        } else if (authType.indexOf("No auth") != -1) { // Hide the visiblity of authentication container, if only No auth is available.

        } else if (authType.indexOf("Basic Auth") != -1  && authType.indexOf("OAuth 2") != -1) { // Show both OAuth 2 and Basic auth info in the operation container.
            constructBasicAuthCredentials();
            constructOAuth2Credentials("second");
            window.reInitDetailsEvents();
        } else if (authType.indexOf("Basic Auth") != -1) { // Show Basic auth info in the operation container.
            sessionStorage.selectedAuthScheme = apiName +"@@@"+ revisionNumber + "@@@" + "basicauth";
            selectedAuthScheme ="basicauth";
            constructBasicAuthCredentials();
            window.reInitDetailsEvents();
        } else if (authType.indexOf("OAuth 2") != -1) { // Show OAuth 2 info in the operation container.
            sessionStorage.selectedAuthScheme = apiName +"@@@"+ revisionNumber + "@@@" +"oauth2";
            selectedAuthScheme = "oauth2";
            jQuery(".authentication").html("");
            constructOAuth2Credentials("");
            window.reInitDetailsEvents();
        }*/
    };
    /**
     * The request/response link click event handler - Show/Hide request/response tab content, based on the link.
     * @return {Void} Show/Hide request/response tab content.
     */
    this.swapSampleRequestResponseContainer = function() {
        var $currentElement = jQuery(this);
        if ($currentElement.attr('id') ==  'link_request_tab') { // Show the request
            jQuery("#link_response_tab").removeClass('selected');
            jQuery("#request_response_container .response").hide();
            jQuery("#request_response_container .request").show();
        } else {
            jQuery("#link_request_tab").removeClass('selected');
            jQuery("#request_response_container .request").hide();
            jQuery("#request_response_container .response").show();
        }
        $currentElement.addClass('selected');
    };
    /**
     * The method handles saving basic auth details/displays error, when user clicks 'Save' button in the Basic Auth pop-up dialog.
     */

    this.saveAuthModal = function(e) {
        var parentClass = jQuery(this).parents("#modal_container");
        if (parentClass.hasClass('basic_auth')) {
            var errMessage = self.validateBasicAuthFields(); // Validate email and password.
            if (errMessage == "") { // If there are no errors.
                userEmail = jQuery("#inputEmail").val();
                basicAuth = "Basic "+jQuery.base64Encode(userEmail+':'+jQuery("#inputPassword").val());
                sessionStorage.revisionsBasicAuthDetails = apiName +"@@@"+ revisionNumber + "@@@" + userEmail + "@@@" + basicAuth; // Store basic auth info in session storage.
                self.closeAuthModal(); // Close the auth modal.
                sessionStorage.selectedAuthScheme = apiName +"@@@"+ revisionNumber + "@@@" + "basicauth"; // Store seleted auth scheme info in session storage.
                selectedAuthScheme = "basicauth";
                self.updateAuthContainer();
            } else { // Display error message.
                jQuery("#modal_container.modal .error_container").html(errMessage+"Please try again.").show();
            }
        } else if (parentClass.hasClass('oauth2')) {
            var windowLocation = window.location.href;
            windowLocation = windowLocation.split("/resources/")[0];
            self.closeAuthModal();
            // Make an AJAX call to retrieve an auth URL.
            self.makeAJAXCall({"url":windowLocation+"/authSchemes/oauth2webserverflow/authUrl",dataType:"json", "callback":self.renderCallbackURL, "errorCallback" :self.handleOAuth2Failure});
        } else if (parentClass.hasClass('custom_token')) {
            var errMessage = self.validateCustomAuthFields(); // Validate email and password.
            if (errMessage == "") { // If there are no errors.
                sessionStorage.revisionsCustomTokenCredentialsDetails = apiName +"@@@"+ revisionNumber + "@@@" + jQuery("#inputName").val() + "@@@" + jQuery("#inputValue").val() + "@@@" + jQuery('input[name=rdoCustomTokenType]:checked').val();
                self.closeAuthModal(); // Close the auth modal.
                sessionStorage.selectedAuthScheme = apiName +"@@@"+ revisionNumber + "@@@" + "customtoken"; // Store seleted auth scheme info in session storage.
                selectedAuthScheme = "customtoken";
                self.updateAuthContainer();
            } else { // Display error message.
                jQuery("#modal_container.modal .error_container").html(errMessage+"Please try again.").show();
            }
        }
    };
    /**
     * The request payload sample/request payload description link click event handler - Show/Hide payload sample content/request payload sample content, based on the link.
     */
    this.toggleRequestPayload = function(e) {
        var element = jQuery(this);
        if (!element.parent().hasClass("active")) { // Do nothing, if user clicks on the active link.
            element.parent().siblings().removeClass("active");
            element.parent().addClass("active");
            var role = element.attr("data-role");
            var requestPayloadDocsElement = jQuery("[data-role='request-payload-docs']");
            var requestPayloadExampleElement = jQuery("[data-role='request-payload-example']");
            if (requestPayloadDocsElement.siblings("textarea").length) { // show/hide text are in inline edit use case.
                requestPayloadDocsElement.siblings("textarea").hide();
                requestPayloadDocsElement.siblings("a.allow_edit").hide();
            }
            if (requestPayloadExampleElement.siblings("a.allow_edit").length) {
                requestPayloadExampleElement.siblings("a.allow_edit").hide();
            }
            if (role =="docs-link") {
                requestPayloadExampleElement.hide();
                requestPayloadDocsElement.show();
            } else {
                requestPayloadDocsElement.hide();
                requestPayloadExampleElement.show();
            }
        }
    };
    /**
     * Click event handler for the reset link avaiable next to the send request button.
     */
    this.resetFields = function() {
        jQuery(".method_table").find("input").each(function() { // Empty the header/query input elements.
            jQuery(this).val('');
        });
        jQuery("[data-role='query-param-list'],[data-role='header-param-list']").each(function(i, obj) {
            var valueElement = jQuery(this).find("[data-role='value']");
            valueElement.val(valueElement.attr('data-original-value'));
        });
        if (jQuery('[data-role="request-payload-example"]').length) {
            var bodyPayloadElementValue = jQuery('[data-role="request-payload-example"]').children('textarea').val();
            window.apiModelEditor.setRequestPayLoad(bodyPayloadElementValue);
        };
        jQuery("#request_response_container .response").html("<p>Make a request and see the response.</p>");
        jQuery("#request_response_container .request").html("<p>Make a request and see the response.</p>");
        rawCode = "";
        bodyContent = false;
    };
    /**
     * Click event handler for the send request button.
     * Constructs all necessary params and make an AJAX call to proxy or display validation error message.
     */
    this.sendRequest = function() {
        jQuery("#working_alert").fadeIn(); // Show working alert message.
        jQuery("#request_response_container .response").html("<p>Make a request and see the response.</p>");
        jQuery("#request_response_container .request").html("<p>Make a request and see the response.</p>");
        var templateInputElements = jQuery("#resource_URL input");
        if (templateInputElements.length >= 1) { // Check if template param available.
            // Stores the template param name and values in local storage, if user modified the default template param value.
            // Loop through the template params and check against local stroage variable.
            if (localStorage.hasOwnProperty('templateParams')) {
                var templateParams = JSON.parse(localStorage.getItem('templateParams'));
                jQuery("#resource_URL input").each(function() {
                    var inputElementName = jQuery(this).siblings("span").attr('data-role');
                    var inputElementValue = jQuery(this).val();
                    if (inputElementName == inputElementValue || inputElementValue == "") {
                        isTemplateParamMissing = true;
                        templateParamMissing.push(inputElementName.substring(1,inputElementName.length-1));
                        jQuery(this).addClass('error');
                    }
                    var isModified = false;
                    for (var i=0; i<templateParams.length; i++) {
                        var paramName = templateParams[i].name;
                        var paramValue = templateParams[i].value;
                        if (inputElementName == paramName) {
                            isModified=true;
                        }
                        if(inputElementName == paramName && inputElementValue != paramValue && inputElementValue.length) {
                            templateParams[i].value = inputElementValue;
                        }
                    }
                    if (!isModified) {
                        templateParams.push({"name":inputElementName,"value":inputElementValue});
                    }
                });
                localStorage.setItem("templateParams",JSON.stringify(templateParams));
            } else {
                var templateParamArray = [];
                jQuery("#resource_URL input").each(function() {
                var spanElement = jQuery(this).siblings("span");
                    templateParamArray.push({"name":spanElement.attr("data-role"),"value":spanElement.html()});
                });
                localStorage.setItem("templateParams",JSON.stringify(templateParamArray)); // Create local storage variable and assign the values.
            }
        }
        //change the variable name to Target URL.
        var urlToTest = jQuery("#resource_URL").text();
        var methodVerb = jQuery.trim(jQuery("#content").find("[data-role='verb']").text().toLowerCase()); // Retrieve the verb from the HTML element.

        var headersList = [];
        // Loop through the header params and identify if required params are empty otherwise add header params in the list.
        if (jQuery("[data-role='header-param-list']").length) {
            jQuery("[data-role='header-param-list']").each(function(i, obj) {
                var headerParamName = jQuery(this).find("[data-role='name']").text();
                var headerParamValue;
                if (jQuery(this).find("[data-role='multiple-value']").length) {
                    headerParamValue = jQuery(this).find("select option:selected").val();
                } else {
                    headerParamValue = jQuery(this).find("[data-role='value']").val();
                }
                headersList.push({"name" : headerParamName, "value" : headerParamValue});
                if (jQuery(this).find("span.required").length && jQuery(this).find("[data-role='value']").val() == "") {
                    isHeaderParamMissing = true;
                    headerParamMissing.push(headerParamName);
                    jQuery(this).find("[data-role='value']").addClass('error');
                }
            });
        }
        var queryParamString = "";
        // Loop through the query params and identify if required params are empty otherwise add query params in the list.
        if (jQuery("[data-role='query-param-list']").length >= 1) {
            var isFistParam = true;
            jQuery("[data-role='query-param-list']").each(function(i, obj) {
                var queryParamName = jQuery(this).find("[data-role='name']").text();
                var queryParamValue;
                if (jQuery(this).find("[data-role='multiple-value']").length) {
                    queryParamValue = jQuery(this).find("select option:selected").val();
                } else {
                    queryParamValue = jQuery(this).find("[data-role='value']").val();
                }
                if (jQuery.trim(queryParamValue).length >= 1) {
                    var separator = (isFistParam) ? "" : "&";
                    queryParamString += separator + queryParamName + "=" + encodeURIComponent(decodeURIComponent(queryParamValue));
                    isFistParam = false;
                }
                if (jQuery(this).find("span.required").length && queryParamValue == "") {
                    isQueryParamMissing = true;
                    queryParamMissing.push(queryParamName);
                    jQuery(this).find("[data-role='value']").addClass('error');
                }
            });
        }

        var errorMessage = "";
        urlToTest = urlToTest.replace(/\{/g,"").replace(/\}/g,"");
        urlToTest = jQuery.trim(urlToTest);
        queryParamString = jQuery.trim(queryParamString);
        if (queryParamString != "") {
            var separator = "?";
            if (urlToTest.indexOf("?") != -1) {
                separator = "&"
            }
            urlToTest =  urlToTest + separator + queryParamString; // Append query params.
        }
        if (isTemplateParamMissing) {
            errorMessage += "Missing value for template parameter(s): <span>"+templateParamMissing.join(", ")+"</span></br>";
        }
        if (isQueryParamMissing) {
            errorMessage += "Missing value for required query parameter(s):&nbsp;<span>"+queryParamMissing.join(", ")+"</span></br>";
        }
        if (isHeaderParamMissing) {
            errorMessage += "Missing value for required header parameter(s):&nbsp;<span>"+headerParamMissing.join(", ")+"</span></br>";
        }

        var paramGroups = jQuery("[data-role='param-group']");
        if (paramGroups.length >= 1) {
            paramGroups.each(function(i, obj) {
                var paramGroup = jQuery(this);
                var choice = parseInt(paramGroup.find("[data-role='choice']").attr('data-choice'));
                var counter = 0;
                var paramGroupMissing = [];
                if (paramGroup.find("[data-role='param-group-list']").length >= 1) {
                    paramGroup.find("[data-role='param-group-list']").each(function(i, obj) {
                        var paramGroupName = jQuery(this).find("[data-role='name']").text();
                        var paramGroupValue = jQuery(this).find("[data-role='value']").val();
                        var paramGroupType = jQuery(this).find("[data-role='type']").attr('data-type');
                        if (jQuery.trim(paramGroupValue).length >= 1) {
                            counter++;
                            if (paramGroupType == "query") {
                                var separator = (queryParamString.indexOf("?") != -1 ) ? "&" : "?";
                                queryParamString += separator + paramGroupName + "=" + encodeURIComponent(decodeURIComponent(paramGroupValue));
                            } else if (paramGroupType == "header") {
                                headersList.push({"name" : paramGroupName, "value" : paramGroupValue});
                            }
                        } else {
                            paramGroupMissing.push(jQuery.trim(paramGroupName));
                        }
                    });
                }
                if (choice > counter) {
                    errorMessage += "Missing "+ (choice-counter) +" value for parameter group of: <span>"+paramGroupMissing.join(", ")+"</span></br>";
                }
            });
        }

        if (errorMessage != "") { // Display error message, if any of the required param is missing.
            jQuery("#error_container").html(errorMessage);
            jQuery("body").scrollTop(0);
            jQuery("#error_container").show();
            self.clearMissingArray();
        }
        if (selectedAuthScheme  == "basicauth") { // Add basic details in send request proxy API call.
            if (basicAuth) {
                headersList.push({"name" : "Authorization", "value" : basicAuth});
            }
        } else { // Add OAuth 2 details in send request proxy API call.
            if (oauth2Credentials != null) {
                if (oauth2Credentials.accessTokenType == "query") { // Add OAuth 2 details in the query param.
                    var paramName = (oauth2Credentials.accessToeknParamName == "") ? "oauth_token" : oauth2Credentials.accessToeknParamName;
                    var separator = (queryParamString == "") ? "?"  : "&";
                    urlToTest += separator + paramName +"=" + oauth2Credentials.accessToken;
                } else if (oauth2Credentials.accessTokenType == "header") { // Add OAuth 2 details in headers.
                    headersList.push({"name" : "Authorization", "value" : oauth2Credentials.accessToken});
                }
            }
        }
        targetUrl = urlToTest;
        urlToTest = encodeURIComponent(urlToTest).replace(/\{.*?\}/g,"");

        urlToTest = window.proxyURL+"?targeturl="+urlToTest;
        // If a method has an attachment, we need to modify the standard AJAX the following way.
        if (jQuery("[data-role='attachments-list']").length || jQuery("[data-role='body-param-list']").length) {
            if (jQuery('[data-role="request-payload-example"]').length) {
                var requestPayLoad = "<textarea class='hide' name='text'>"+window.apiModelEditor.getRequestPayLoad()+"</textarea>";
                jQuery("#formAttachment").append(requestPayLoad);
            }
            var formData = new FormData(jQuery('form')[0]); // Create an arbitrary FormData instance
            jQuery.ajax(urlToTest, {
                processData: false,
                type: "POST",
                contentType: false,
                data: formData,
                success: function(data, textStatus, jqXHR) {
                    self.renderRequest(jqXHR.responseText)
                },
                error: function(xhr, status, error) {
                    self.renderRequest(xhr.responseText)
                },
                complete: function() { // Gets called once an AJAX completes.
                  jQuery("#working_alert").fadeOut();
                }
            });
        } else { // If a method does not have attach, use standard makeAJAXCall() method to send request.
            if (jQuery('[data-role="request-payload-example"]').length) {
                self.makeAJAXCall({"url":urlToTest,"type":methodVerb,"data" : window.apiModelEditor.getRequestPayLoad(), "callback":self.renderRequest,"headers":headersList});
            } else {
                self.makeAJAXCall({"url":urlToTest,"type":methodVerb,"callback":self.renderRequest,"headers":headersList});
            }
        }
    };
    /**
     * Success/Error callback method of a send request proxy API call.
     * This methods fetches the response and show the headers, contents and other details in the request and response tab.
     * The request and response content are shown in Prism editor.
     */
    this.renderRequest = function(data) {
        var responseContainerElement = jQuery("[data-role='response-container']");
        var requestContainerElement = jQuery("[data-role='request-container']");
        if (data == "" || data == null) {
            requestContainerElement.html("<strong> An internal error has occurred. Please retry your request.</strong>");
            responseContainerElement.html("<strong> An internal error has occurred. Please retry your request.</strong>");
            return;
        }
        data = jQuery.parseJSON(data); // Parse the JSON.
        rawCode = unescape(data.responseContent); // Stores response content.
        //rawCode = jQuery.parseJSON(rawCode); //:TODO:: check the proxy and fix the issue and remove it.
        //rawCode = unescape(rawCode.responseContent); //:TODO:: check the proxy and fix the issue and remove it.
        // Response line fine details contruction.
        var responseContainerString = "<strong";
        var responseStatusCode;
        var httpVersion;
        var responseReasonPhrase;
        if (data.responseStatusCode) {
            responseStatusCode = data.responseStatusCode;
            httpVersion = data.httpVersion;
            responseReasonPhrase = data.responseReasonPhrase;
        } else {
            responseStatusCode = data.responseCode;
            httpVersion = data.messageVersion;
            responseReasonPhrase = data.responsePharse;
        }

        if (parseInt(responseStatusCode) >= 100 && parseInt(responseStatusCode) < 400) {
             responseContainerString += " class='success'";
        }
        responseContainerString += "> HTTP/"+httpVersion +" "+ responseStatusCode +"  "+ responseReasonPhrase+"</strong>";
        // Response headers construction.
        responseContainerString += "<dl>";
        for (var i=0; i<data.responseHeaders.length; i++) {
            responseContainerString +=  "<dt>";
            responseContainerString += unescape(data.responseHeaders[i].name);
            responseContainerString += ": </dt><dd>";
            responseContainerString += unescape(data.responseHeaders[i].value);
            responseContainerString +=  "</dd>";
        }
        responseContainerString += "</dl>";
        responseContainerElement.html(responseContainerString);
        // Response content construction.
        if (rawCode != "") {
            /**
                *  The below tries to extract a json string by checking if string already contains a double quote at the begin
                *  and end. To make sure, we do it for json we check additionally for presence of { or [ at the begin and },]
                *  at the end. The reason for doing this is that a Fix made earlier to obfuscate some credentials caused additional
                *  quotes to be introduced by The json library we use(JSONObject).
                *  For now, making these checks to get the underlying json.
            */
            var getAsJson = (/^"\{/.test(rawCode) && /\}"$/.test(rawCode)) || (/^"\[/.test(rawCode) && /\]"$/.test(rawCode));
            if (getAsJson) {
                /**
                    * Modified the regexp below to include scanning of new line character included part of json response.
                    * '.' regexp doesnt go past \n and hence some responses were not detected as json even though they were.
                    *  Reference bug: 22246
                */
                rawCode = rawCode.replace(/^"((.|\s)*)"$/, "$1");
            }
            if (rawCode) {
                var forJSON = true;
                try {
                    var tmp = jQuery.parseJSON(rawCode);
                }
                catch (e) {
                    forJSON = false;
                }
                if (forJSON) { // Handle JSON response content.
                    var rawasjson = self.parseAndReturn(rawCode);
                    rawCode = JSON.stringify(rawasjson, null, 2);
                    rawCode = rawCode.replace("[{", "[\n  {");
                    rawCode = rawCode.replace(/,\n[\n ]*$/, "");
                    rawCode = rawCode.replace(/\n(\s*)},\n{/g, "\n$1},\n$1{");
                    responseContainerElement.append("<pre class='language-javascript'><code class='language-javascript' id='some-code'>"+rawCode+"</code></pre>");
                } else { // Handle non JSON response content (treat as markup language)
                    rawCode =rawCode.replace(/>/g,"&gt;").replace(/</g,"&lt;");
                    responseContainerElement.append("<pre class='language-markup'><code class='language-markup' id='some-code'>"+rawCode+"</code></pre>");

                }
            }
        }
        // Request line fine details contruction.
        var hostName = targetUrl.split("//")[1].split("/")[0];
        var requestContainerString = "<strong>"+data.requestVerb+" "+ targetUrl.split(hostName)[1] + " HTTP/"+httpVersion+"</strong>";
        // Request headers construction.
        requestContainerString += "<dl>";
        for (var i=0; i<data.requestHeaders.length; i++) {
            var headerName = data.requestHeaders[i].name;
            if (headerName.toLowerCase() != "origin" && headerName.toLowerCase() != "referer") {
                var headerValue = data.requestHeaders[i].value;
                requestContainerString +=  "<dt>";
                requestContainerString += unescape(headerName);
                requestContainerString += ": </dt><dd>";
                requestContainerString += unescape(headerValue);
                requestContainerString +=  "</dd>";
            }
        }
        requestContainerString += "</dl>";
        requestContainerElement.html(requestContainerString);
        // Resquest content construction.
        bodyContent = unescape(data.requestContent);
        if (bodyContent) {
            var forJSON = true;
            try {
                tmp = jQuery.parseJSON(bodyContent);
            }
            catch (e) {
                forJSON = false;
            }
            if (forJSON) { // JSON request content.
                var rawasjson = self.parseAndReturn(bodyContent);
                bodyContent = JSON.stringify(rawasjson, null, 2);
                bodyContent = bodyContent.replace("[{", "[\n  {");
                bodyContent = bodyContent.replace(/,\n[\n ]*$/, "");
                bodyContent = bodyContent.replace(/\n(\s*)},\n{/g, "\n$1},\n$1{");
                requestContainerElement.append("<pre class='language-javascript'><code class='language-javascript' id='some-code'>"+bodyContent+"</code></pre>");
            } else { // Non JSON request content.
                bodyContent =bodyContent.replace(/>/g,"&gt;").replace(/</g,"&lt;");
                requestContainerElement.append("<pre class='language-markup'><code class='language-markup' id='some-code'>"+bodyContent+"</code></pre>");
            }
        }
        Prism.highlightAll(); // Update the Prism editor.
    };
    /**
     * This method clears the error container and it's related arrays and variable.
     */
    this.clearErrorContainer = function() {
        self.clearMissingBooleanVariables();
        self.clearMissingArray();
        jQuery("#error_container").hide();
        jQuery("#error_container").html("");
    };
    /**
     * This method clears the params variable.
     */
    this.clearMissingBooleanVariables = function() {
        isTemplateParamMissing = false;
        isHeaderParamMissing = false;
        isQueryParamMissing = false;
        isRequestBodyMissing = false;
    };
    /**
     * This method clears the params array.
     */
    this.clearMissingArray = function() {
        templateParamMissing = [];
        headerParamMissing = [];
        queryParamMissing = [];
        requestBodyMissing = "";
    };
    /**
     * This method gets called after the successful OAuth 2 dance.
     * Display error message if any.
     * Stroe the OAuth 2 auth details in session storage.
     * Set OAuth 2 as seleted auth scheme.
     */
    this.setOAuth2Credentials = function(obj) {
        if (obj.errorMessage != "") { // Display error message if any.
            self.showError(obj.errorMessage);
        } else {
            oauth2Credentials = obj;
            selectedAuthScheme = "oauth2";
            sessionStorage.revisionsOAuth2CredentialsDetails = apiName +"@@@"+ revisionNumber + "@@@" + JSON.stringify(oauth2Credentials); // Stroe the OAuth 2 auth details in session storage.
            sessionStorage.selectedAuthScheme = apiName +"@@@"+ revisionNumber + "@@@" +"oauth2"; // Set OAuth 2 as seleted auth scheme.
            self.updateAuthContainer();
        }
    };
    /**
     * The auth scheme container click event handler - sets clicked auth scheme as selected auth scheme.
     */
    this.toggleAuthScheme = function(e) {
        jQuery(".authentication .well").removeClass("selected");
        jQuery(this).addClass("selected");
        if (jQuery(this).hasClass("basicauth")) {
            sessionStorage.selectedAuthScheme = apiName +"@@@"+ revisionNumber + "@@@" + "basicauth";
            selectedAuthScheme = "basicauth";
        } else {
            sessionStorage.selectedAuthScheme = apiName +"@@@"+ revisionNumber + "@@@" + "oauth2";
            selectedAuthScheme = "oauth2";
        }
    };
    /**
     * This method clears session storage variables.
     * @param {String} type A type of auth scheme (basicauth or oauth2).
     * @return {Void} clears session storage variables.
     */
    this.clearSessionStorage = function(type) {
        type = (typeof type.data == "undefined") ? type : type.data;
        if (type == "oauth2") {
            sessionStorage.removeItem('revisionsOAuth2CredentialsDetails');
            oauth2Credentials  = null;
            jQuery(".authentication .well.oauth2 .details").html("<a class='link_open_oauth2' title='Set OAuth 2 credentials.' href='javascript:void(0)'>Set...</a>");
        } else if (type == "basicauth"){
            basicAuth = "";
            userEmail = "";
            sessionStorage.removeItem('revisionsBasicAuthDetails');
            jQuery(".authentication .well.basicauth .details").html("<a class='link_open_basicauth' role='button' data-toggle='modal' title='Set basic auth credentials.' href='#myModal'>Set...</a>");
        } else if (type == "customtoken") {
            sessionStorage.removeItem('revisionsCustomTokenCredentialsDetails');
            jQuery(".authentication .well.customtoken .details").html("<a class='link_customtoken' title='Set custom token credentials.' href='javascript:void(0)'>Set...</a>");
        }

        window.initMethodsAuthDialogsEvents(); // Re initialize events after the change.
    };
};
// The class/object Apigee.APIModel.Details extents Apigee.APIModel.Common.
Apigee.APIModel.Methods.prototype = new Apigee.APIModel.Common();
/**
 * This class handles operation page inline edit related functionalities.
 */
 Apigee.APIModel.InlineEdit = function() {
    // Private properties
    var self = this; // Keep a reference of the current class when the context of 'this' is changing.
    var editMode = 0; // Holds the edit mode value
    var basicAuth; // Stores the basic auth info.
    var windowLocation = window.location.href; // Stores the window location URL.
    var currentEdiatableElement = null;
    var currentEdiatableElementValue = "";
    var editingFlag = false;
    var isAdminAuthModalReopened = false;
    //Private methods.
    /**
     * This method clears the inline edit basic auth session storage details.
     */
    function clearEditSessions() {
        sessionStorage.removeItem('basicAuth1');
        sessionStorage.removeItem('userEmail1');
    }
    function constructParams(paramType) {
        var liEmenets;
        var typeVal;
        if (paramType == "queryAndHeader") {
            liEmenets = "[data-role='query-param-list'][data-scope='method'], [data-role='header-param-list'][data-scope='method']";
        } else if (paramType ==  "body") {
            liEmenets = "[data-role='body-param-list'][data-scope='method']";
            typeVal = "BODY";
        } else if (paramType == "attachments") {
            liEmenets = "[data-role='attachments-list']";
        }

        var paramString = "";
        jQuery(liEmenets).each(function(index) {
            var currentLIElement = jQuery(this);
            typeVal = paramType;
            if (paramType == "queryAndHeader") {
                typeVal = (currentLIElement.attr('data-role') == "query-param-list") ? "QUERY" : "HEADER";
            }
            var requiredParam = (currentLIElement.find("[data-role='required']").text().indexOf("required") != -1) ? true : false;
            var paramName;
            var descriptionValue;
            paramName = jQuery.trim(currentLIElement.find("[data-role='name']").text());

            /*if (currentLIElement.find("div.title input[data-role='value']").length) {
                paramName = jQuery.trim(currentLIElement.find("div.title input[data-role='value']").val());
            } else {
                paramName = jQuery.trim(currentLIElement.find("div.title span[data-role='name']").text());
            }*/
            if (currentLIElement.find("textarea").length) {
                descriptionValue = jQuery.trim(currentLIElement.find("div.description textarea").val());
            } else {
                descriptionValue = jQuery.trim(currentLIElement.find("[data-role='description']").text());
            }

            descriptionValue = JSON.stringify(descriptionValue);
            descriptionValue = descriptionValue.substring(1,descriptionValue.length-1); //Check if this required.
            descriptionValue = self.escapeSpecialChars(descriptionValue);
            paramString += '{"name" :"' + paramName + '",';
            paramString += '"description" :"' + descriptionValue + '",';
            paramString += '"required" : ' + requiredParam + ',';
            if (paramType == "attachments") {
                paramString += '"contentDisposition" : "' + jQuery.trim(currentLIElement.find("div.value input[data-role='value']").attr('data-content-disposition')) + '"}';
            } else {
                paramString += '"type" :"'+ typeVal +'",';
                paramString += '"defaultValue" :"' + currentLIElement.find("[data-role='value']").val() + '",';
                paramString += '"dataType" :"string"}';
            }
            var noOfParam = jQuery(liEmenets).length;
            if (noOfParam > (index+1) ) {
                paramString += ',';
            }

        });
        return paramString;
    }

    //Public methods.
    /**
     * This method initilize the edit mode based on the mode.
     * @param {Int} mode - Mode type. type 1 provides basic edit functionalities 2 provides advance edit.
     * @return {Void} checks whether user already signed in or not using session storage variable.
     * If yes, stores the basic auth details in local variable and construct the inline edit mode.
     * If no, opens pop up basic authentication dialog and stores the basic auth details in local variable and construct the inline edit mode.
     */
    this.init = function(mode) {
        editMode = mode;
        if (sessionStorage.userEmail1) {
            self.showAdminAuthenticationSection(); // Store the basic auth details in local variable.
        } else {
            self.openAuthModal("edit"); // Open pop up basic authentication dialog.
        }
        window.initInlineEditAdminAuthEvents();
    };
    /**
     * The method handles saving basic auth details/displays error to user, when user clicks 'Save' button in the Inline edit Basic Auth pop-up dialog.
     */
    this.saveAuthModal = function() {
        var errMessage = self.validateBasicAuthFields();
        if (errMessage == "") {
            var windowLocation = window.location.href;
            if (window.authUrl.indexOf("{user}") != -1) {
                var dataObj = "password="+ jQuery("#inputPassword").val();
                if (window.authUrl != "null") {
                    var authUrl = window.authUrl.replace("{user}",encodeURIComponent(userEmail));
                    self.makeAJAXCall({"url": window.proxyURL+"?targeturl="+authUrl,type:"post",dataType:"json",data:dataObj,"contentType":"application/x-www-form-urlencoded","callback":self.saveAdminCredentials, "errorCallback" :self.showUnauthorizedInfo });
                } else {
                    self.saveAdminCredentials();
                }
            } else {
                var jsonBody = '{ "userName" : "'+ userEmail + '", "password" : "'+ jQuery("#inputPassword").val() + '" }';
                if (window.authUrl != "null") {
                    self.makeAJAXCall({"url": window.proxyURL+"?targeturl="+window.authUrl,type:"post",dataType:"json",data:jsonBody,"contentType":"application/json","callback":self.saveAdminCredentials, "errorCallback" :self.showUnauthorizedInfo });
                } else {
                    self.saveAdminCredentials();
                }
            }
        } else {
            jQuery("#modal_container.modal .error_container").html(errMessage+"Please try again.").show();
        }
    };
    this.saveAdminCredentials = function() {
        basicAuth = "Basic "+jQuery.base64Encode(userEmail+':'+jQuery("#inputPassword").val());
        sessionStorage.basicAuth1 = basicAuth;
        sessionStorage.userEmail1 = userEmail;
        self.closeAuthModal();
        self.showAdminAuthenticationSection();
    };
    this.showUnauthorizedInfo = function(errorCode) {
        if (errorCode == "401") {
            jQuery("#modal_container.modal .error_container").html("Invalid credentials. Please try again.").show();
        } else {
            jQuery("#modal_container.modal .error_container").html("Error saving details. Please try again.").show();
        }
    };
    /**
     * The method shows the info about logged in users and provide clear and reset functionlities.
     */
    this.showAdminAuthenticationSection = function() {
        if (sessionStorage.userEmail1) {
            basicAuth = sessionStorage.basicAuth1;
            var emailString = sessionStorage.userEmail1;
            jQuery(".admin_auth_section a.set_admin_credentials").hide();
            jQuery(".admin_auth_section a.auth_admin_email").html(emailString).show();
            jQuery(".admin_auth_section .icon-remove").show();
            jQuery(".admin_auth_section").show();
        } else {
            sessionStorage.removeItem('basicAuth1');
            sessionStorage.removeItem('userEmail1');
            jQuery(".admin_auth_section a.auth_admin_email").html("").hide();
            jQuery(".admin_auth_section a.set_admin_credentials").show();
            jQuery(".admin_auth_section .icon-remove").hide();
            jQuery(".admin_auth_section").show();
        };
        if (!isAdminAuthModalReopened) {
            self.constructEditMode();
        }

    };
    /**
     * The method clears the inline edit basic auth related session storage and reset the HTML element.
     */
    this.clearAdminAuthDetails = function() {
        clearEditSessions();
        jQuery(this).siblings("a.auth_admin_email").html("").hide();
        jQuery(this).siblings("a.set_admin_credentials").show();
        jQuery(this).hide();
    };
    /**
     * The method handles reseting the inline edit basic auth.
     */
    this.reOpenAdminAuthDetails = function() {
        isAdminAuthModalReopened = true;
        clearEditSessions();
        self.openAuthModal("edit");
        window.initInlineEditAdminAuthEvents();

    };
    /**
     * The method handles constructing the inline edit HTML elements and invokes the necessary methods.
     * Idetify the editable element based on the mode.
     * Append the allow_edit, ok and cancel elements to the editable elements.
     * Append textarea to method desction, request payload sample, response payload sample elements.
     */
    this.constructEditMode = function() {
        jQuery("#content").addClass("edit_mode");
        var editIconHTML = '<a class="allow_edit hover"></a><a class="allow_edit ok" title="save and quit."></a><a class="allow_edit cancel" title="reset and quit."></a>';
        var urlContainerHTML = '<div class="popover fade top in" style="display: block;display:none;max-width:428px;width:428px;z-index:99999"><div class="arrow"></div><div class="popover-content"><div style="float:left;"><label>NAME</label><input type="text" style="height: 25px; margin-bottom: 5px; width: 125px;float:left;margin-right:30px;" disabled="disabled" value=""/></div><div style="float:left;"><label>DESCRIPTION</label><textarea style="float:left;border-radius:4px 0 4px 4px;"></textarea></div>'+editIconHTML+'</div></div>'
        jQuery(".edit_mode .resource_details").parent().addClass("clearfix");
        jQuery(".edit_mode .resource_details").children("div").addClass("clearfix");
        jQuery("[data-role='method-title']").parent().append(editIconHTML).addClass("clearfix");
        jQuery(".description_container").addClass("clearfix");
        jQuery(".url_container").append(urlContainerHTML);
        jQuery("ul.method_table").parent().css({"clear":"both"});
        // Append edit HTML to header and query params.
        if (jQuery("[data-role='query-param-list'],[data-role='header-param-list'], [data-role='body-param-list'], [data-role='param-group-list'] ").length) {
            jQuery("[data-role='query-param-list'],[data-role='header-param-list'], [data-role='body-param-list'], [data-role='param-group-list']").each(function(i, obj) {

                jQuery(this).find("[data-role='description']").parent().append(editIconHTML);
            });
        }
        jQuery("[data-scope='resource']").find("[data-role='description']").removeAttr("data-allow-edit"); // Remove edit mode to resource level params.
        // Request payload description related changes.
        jQuery(".description_container").append('<textarea class="resource_description_edit"></textarea>'+editIconHTML);
        var requestPayLoadDocsContainer = jQuery("[data-role='request-payload-docs']");
        requestPayLoadDocsContainer.wrap("<div class='clearfix'></div>");
        requestPayLoadDocsContainer.attr("data-allow-edit","true");
        requestPayLoadDocsContainer.parent().append('<textarea class="request_payload_doc_edit hide"></textarea>'+editIconHTML);
        // Request payload sample related changes.
        var requestPayLoadExampleContainer = jQuery("[data-role='request-payload-example']");
        requestPayLoadExampleContainer.wrap("<div class='clearfix'></div>")
        requestPayLoadExampleContainer.attr("data-allow-edit","true").width(600).css({'float':'left'});
        requestPayLoadExampleContainer.parent().append(editIconHTML);
        // Response payload description related changes.
        var responsePayLoadDocsContainer = jQuery("[data-role='response-payload-docs']");
        responsePayLoadDocsContainer.wrap("<div class='clearfix'></div>");
        responsePayLoadDocsContainer.attr("data-allow-edit","true");
        responsePayLoadDocsContainer.parent().append('<textarea class="response_payload_doc_edit hide"></textarea>'+editIconHTML);
        window.inlineEditPageEvents();
    };
    /**
     * The method sets custom properties to editable template params.
     */
    this.setCustomPropertiesToEdiatableInputElements = function() {
        var currentElement = jQuery(this);
        var currentElementParamName = currentElement.siblings().attr("data-role");
        currentElementParamName = currentElementParamName.replace(/\{/g,"").replace(/\}/g,"")
        jQuery("div[data-role='template-params']").find("[data-scope='method']").each(function() {
            var templateParamName = jQuery.trim(jQuery(this).find("[data-role='name']").html());
            if (currentElementParamName == templateParamName) {
                currentElement.parent().attr('data-description', currentElement.parent().attr('data-original-title'));
                currentElement.parent().attr('data-original-title', 'click to edit the description.');
            }
        });

    };
    /**
     * The Mouse over event handler for editable element, shows the edit icon.
     */
    this.handleEditPropertiesMouseOver = function() {
        var dataRole = jQuery(this).attr("data-role");
        if (!jQuery(this).hasClass("editing")) {
            jQuery(this).addClass('edit');
            jQuery(this).siblings("a.allow_edit.hover").css({'display':'inline-block'});
        }
    };
    /**
     * The Mouse out event handler for editable element, hides the edit icon.
     */
    this.handleEditPropertiesMouseOut = function() {
        var dataRole = jQuery(this).attr("data-role");
        if (!jQuery(this).hasClass("editing")) {
            jQuery(this).removeClass('edit');
            jQuery(this).siblings("a.allow_edit.hover").hide();
        }
    };
    /**
     * The template param click event handler. // Opens a pop up window and shows the param name and description.
     */
    this.handleTempleParamEdit = function(e) {
        jQuery("#error_container").hide();
        jQuery("#error_container").html("");
        var position = jQuery(this).position();
        //self.resetEditableElement();
        var currentElement = jQuery(this);
        var currentElementParamName = currentElement.siblings().attr("data-role");
        currentElementParamName = currentElementParamName.replace(/\{/g,"").replace(/\}/g,"");
        jQuery("div[data-role='template-params']").find("[data-scope='method']").each(function() {
            var templateParamName = jQuery.trim(jQuery(this).find("[data-role='name']").html());
            if (currentElementParamName == templateParamName) {
                jQuery(".popover .popover-content input").val(currentElementParamName);
                jQuery(".popover .popover-content textarea").val(currentElement.parent().attr('data-description'));
                jQuery(".popover .popover-content").find("a.allow_edit.ok").css({'display':'inline-block'});
                jQuery(".popover .popover-content").find("a.allow_edit.cancel").css({'display':'inline-block'});
                jQuery(".popover .popover-content input").focus();
                jQuery(".popover").css({'left': position.left-(jQuery(".popover").width()/2)+40, 'top' : position.top-(jQuery(".popover").height()-2)}).show();
            }
        });
        e.stopPropagation();
        return false;
    };
    /**
     * Editable elements click event handler.
     * Makes the current element as editable element. Shows OK, Cancel icon,
     */
    this.handleEditableElementsClick = function(e) {
        jQuery("#error_container").hide();
        jQuery("#error_container").html("");
        //self.undoLastChange();
        if (currentEdiatableElementValue != "" && editingFlag && currentEdiatableElement != null && currentEdiatableElement.attr('data-role')!=jQuery(this).attr('data-role')) {
            var previsionEditableElementValue = jQuery.trim(currentEdiatableElement.html());
            if (currentEdiatableElement.siblings("textarea").length) {
                previsionEditableElementValue = jQuery.trim(currentEdiatableElement.siblings("textarea").val());
            }
            if (currentEdiatableElementValue != previsionEditableElementValue) {
                self.openAuthModal("confirm");
                window.initInlineEditAdminAuthEvents();
            } else {
                self.resetEditableElement();
            }
        } else {
            currentEdiatableElement = jQuery(this);
            currentEdiatableElementValue = jQuery.trim(jQuery(this).html());
            if (jQuery(this).hasClass("resource_description") || jQuery(this).attr('data-role') == "request-payload-docs" || jQuery(this).attr('data-role') == "response-payload-docs") {
                jQuery(this).hide();
                jQuery(this).siblings("textarea").val(jQuery.trim(jQuery(this).html())).height(jQuery(this).height()+30).show();
                jQuery(this).siblings("textarea").focus();
                jQuery(this).siblings("textarea").unbind("click").click(function() {
                    return false;
                });

            }
            if (jQuery(this).attr('data-role') == "method-title" || jQuery(this).attr('data-role') == 'description') {
                jQuery(this).attr('contenteditable','true');
            } else {
                jQuery("[data-role='method-title']").removeAttr('contenteditable');
            }
            jQuery(this).addClass("editing");
            // Hide other editable elements icons.
            jQuery(this).siblings("a.allow_edit.hover").hide();
            // Show OK, Cancel icon to current element.
            jQuery(this).siblings('a.allow_edit.ok').show();
            jQuery(this).siblings('a.allow_edit.cancel').show();
            jQuery(this).addClass("edit"); // Add a class called 'edit'.
            //jQuery(this).removeClass("editing"); // Add a class called 'edit'.
            editingFlag = true;
            jQuery(this).focus();
        }

        e.preventDefault();
        return false;
    };
    this.resetEditableElement = function() {
        editingFlag = false;
        currentEdiatableElement.siblings("a.allow_edit").hide();
        currentEdiatableElement.html(currentEdiatableElementValue);
        currentEdiatableElement.removeClass("edit").removeClass("editing");
        if (currentEdiatableElement.hasClass("resource_description") || currentEdiatableElement.attr('data-role') == "request-payload-docs" || currentEdiatableElement.attr('data-role') == "response-payload-docs") {
            //currentEdiatableElement.hide();
            currentEdiatableElement.siblings("textarea").hide();
            currentEdiatableElement.show();
        }
        currentEdiatableElementValue = "";
        return false;

    }
    this.documentClickHandler = function() {
        if (currentEdiatableElementValue != "" && jQuery("body").children("#modal_container.modal").is(":visible") == false) {
            self.openAuthModal("confirm");
            window.initInlineEditAdminAuthEvents();
        }
    }
    this.handleConfirmDialogSave = function() {
        currentEdiatableElement.siblings("a.allow_edit.ok").trigger("click");
        self.closeAuthModal();
        window.initInlineEditAdminAuthEvents();
        currentEdiatableElementValue = "";
        return false;
    };
    /**
     * Click event handler for the OK/Cancel icon.
     * If it is OK icon, Constructs all necessary params and make an AJAX call to update the modified values.
     * If it is Cancel icon, Resets the editable elements value.
     */
    this.makeAPICall = function(e) {
        var operationPath = location.href;
        editingFlag = false;
        operationPath = operationPath.replace("/doc?editMode=1","").replace("/doc?editMode=2","");
        operationPath = operationPath.replace("/doc?editMode=1","").replace("/doc?editMode=2","");
        if (typeof Drupal != "undefined" && typeof Drupal.settings != "undefined") {
            operationPath = Drupal.settings.devconnect_docgen.apiModelBaseUrl + "/v1/o/" + organizationName + "/apimodels/"+apiName+"/revisions/"+revisionNumber+"/resources/"+resourceId+"/methods/"+methodId;
            //operationPath = "https://jaksapi-prod.apigee.net/smartdocs/v1/o/" + organizationName + "/apimodels/"+apiName+"/revisions/"+revisionNumber+"/resources/"+resourceId+"/methods/"+methodId;
        }
        //operationPath = "https://api.jupiter.apigee.net/v1/o/o_harsha_1/apimodels/"+apiName+"/revisions/"+revisionNumber+"/resources/"+resourceId+"/methods/"+methodId;

        operationPath = window.proxyURL+"?targeturl="+operationPath;
        //operationPath = operationPath;
        // Description text construction.
        var descriptionText = jQuery.trim(jQuery(".resource_description").text());
        if (jQuery("textarea.resource_description_edit").is(":visible") == true) {
            descriptionText =  jQuery.trim(jQuery("textarea.resource_description_edit").val());
        }
        // Authentication value construction.
        var authenticationValue = jQuery("[data-role='auth-type']").text()
        authenticationValue = authenticationValue.replace("Basic Auth","BASICAUTH").replace("Custom Token","CUSTOM").replace( "OAuth 1","OAUTH1WEBSERVER").replace("OAuth 1 Client Credentials","OAUTH1CLIENTCREDENTIALS").replace("OAuth 2","OAUTH2WEBSERVER").replace("OAuth 2 Client Credentials","OAUTH2CLIENTCREDENTIALS").replace("OAuth 2 Implicit Grant Flow","OAUTH2IMPLICITGRANT").replace("No auth","NOAUTH");
        var authtenticationString = "";
        if (authenticationValue.split(",").length > 1) {
            authtenticationString = '[';
            for (var i=0; i<authenticationValue.split(",").length; i++) {
             authtenticationString += '"'+   authenticationValue.split(",")[i] + '"';
             if (i != (authenticationValue.split(",").length-1) ) {
                authtenticationString += ",";
             }
            }
            authtenticationString += ']';

        } else {
            authtenticationString = '[ "'+ authenticationValue + '" ]';
        }
        //authtenticationString = authtenticationString.replace("NOAUTH","PUBLIC");
        // Categories value construction.
        var categoriesValue = jQuery.trim(jQuery("[data-role='category']").text());
        var categoriesString = "";
        if (categoriesString.split(",").length > 1) {
            categoriesString = '[';
            for (var i=0; i<categoriesValue.split(",").length; i++) {
             categoriesString += '"'+   categoriesValue.split(",")[i] + '"';
             if (i != (categoriesValue.split(",").length-1) ) {
                categoriesString += ",";
             }
            }
            categoriesString += ']';
        } else {
            categoriesString = '[ "'+ categoriesValue + '" ]';
        }
        // Stringify the description, remove quotes and escape the special charectes.
        descriptionText = JSON.stringify(descriptionText);
        descriptionText = descriptionText.substring(1,descriptionText.length-1); //Check if this required.
        descriptionText = self.escapeSpecialChars(descriptionText)
        // Construct the AJAX method body.
        var jsonBody = '{ "displayName":"'+ jQuery.trim(jQuery("[data-role='method-title']").html()) +'", "description": "' + descriptionText  + '","verb": "'+jQuery.trim(jQuery(".verb_container p").text()) + '",';
        jsonBody += '"authSchemes" : ' + authtenticationString + ', "tags" : ' + categoriesString + ',';
        var contentTypeValue  = "";
        //jsonBody += ' "request": { ';
        if (jQuery.trim(jQuery("[data-role='content-type']").text()) != "N.A.") {
            //jsonBody += '"contentType" : "'+ jQuery.trim(jQuery("[data-role='content-type']").text()) + '",';
            contentTypeValue = jQuery.trim(jQuery("[data-role='content-type']").text());
        }
        //jsonBody += '}';
        jsonBody += '"parameters": [';
        // Header, Query params contruction excluding the resource level params.
        jsonBody += constructParams("queryAndHeader");

        jsonBody += ' ]';

        var paramGroups = jQuery("[data-role='param-group']");
        if (paramGroups.length >= 1) {
            jsonBody += ', "parameterGroups": [ {';
            paramGroups.each(function(i, obj) {
                var paramGroup = jQuery(this);
                var choice = parseInt(paramGroup.find("[data-role='choice']").attr('data-choice'));
                jsonBody += ' "choice" : '+choice+', "parameters" : [ ';
                if (paramGroup.find("[data-role='param-group-list']").length >= 1) {
                    paramGroup.find("[data-role='param-group-list']").each(function(index, obj) {
                        var currentLIElement = jQuery(this);
                        var paramGroupName = jQuery.trim(currentLIElement.find("[data-role='name']").text());
                        var paramGroupValue = jQuery.trim(currentLIElement.find("[data-role='value']").val());
                        var paramGroupType = currentLIElement.find("[data-role='type']").attr('data-type');
                        paramGroupType = paramGroupType.toUpperCase();
                        var descriptionValue;
                        if (currentLIElement.find("div.description textarea").length) {
                            descriptionValue = jQuery.trim(currentLIElement.find("div.description textarea").val());
                        } else {
                            descriptionValue = jQuery.trim(currentLIElement.find("div.description").text());
                        }
                        descriptionValue = JSON.stringify(descriptionValue);
                        descriptionValue = descriptionValue.substring(1,descriptionValue.length-1); //Check if this required.
                        descriptionValue = self.escapeSpecialChars(descriptionValue);
                        jsonBody += '{"name" :"' + paramGroupName + '",';
                        jsonBody += '"defaultValue" :"' + paramGroupValue + '",';
                        jsonBody += '"type" :"'+ paramGroupType +'",';
                        jsonBody += '"description" :"' + descriptionValue + '",';
                        jsonBody += '"dataType" :"string"}';

                        var noOfParam = paramGroup.find("[data-role='param-group-list']").length;
                        if (noOfParam > (index+1) ) {
                            jsonBody += ',';
                        }
                    });
                }
                jsonBody += ']';
            });
            jsonBody += '}]';
        }




        jsonBody += ', "body": {';

        jsonBody += '"parameters": [';
        // Body params contruction excluding the resource level params.
        jsonBody += constructParams('body');

        jsonBody += ' ]';



        jsonBody += ', "attachments": [';
        // Body params contruction excluding the resource level params.
        jsonBody += constructParams('attachments');

        jsonBody += ' ]';

        // Request payload sample contruction.
        if (jQuery('[data-role="request-payload-example"]').length) {
            var requestPayload = JSON.stringify(window.apiModelEditor.getRequestPayLoad());
            requestPayload = requestPayload.substring(1,requestPayload.length-1); //Check if this required.
            requestPayload = self.escapeSpecialChars(requestPayload)
            //jsonBody += ', "requestBody": "' + requestPayload +'"';
            jsonBody += ', "contentType":"' + contentTypeValue + '", "sample" :"'+requestPayload +'"';
        }

        //jsonBody += '"customAttributes" : [';
        // Request/Response payload description construction.
        var requestPayloadDocElement = jQuery("[data-role='request-payload-docs']");
        var responsePayloadDocElement = jQuery("[data-role='response-payload-docs']");
        var requestPayloadDocValue = "";
        var responsePayloadDocValue = "";

        if (requestPayloadDocElement.length) {
            var requestPayloadDocValue = jQuery.trim(requestPayloadDocElement.html());
            if (requestPayloadDocElement.siblings("textarea").is(":visible") == true) {
                requestPayloadDocValue =  jQuery.trim(requestPayloadDocElement.siblings("textarea").val());
            }
            requestPayloadDocValue = JSON.stringify(requestPayloadDocValue);
            requestPayloadDocValue = requestPayloadDocValue.substring(1,requestPayloadDocValue.length-1); //Check if this required.
            requestPayloadDocValue = self.escapeSpecialChars(requestPayloadDocValue);
            //requestPayloadDocValue = ', "request": { "doc": "' + requestPayloadDocValue + '" } ';
            requestPayloadDocValue = ', "doc": "' + requestPayloadDocValue + '" ';
            jsonBody += requestPayloadDocValue;
        }

        jsonBody += ' }';


        if (responsePayloadDocElement.length) {
            var responsePayloadDocValue = jQuery.trim(responsePayloadDocElement.html());
            if (responsePayloadDocElement.siblings("textarea").is(":visible") == true) {
                responsePayloadDocValue =  jQuery.trim(responsePayloadDocElement.siblings("textarea").val());
            }
            responsePayloadDocValue = JSON.stringify(responsePayloadDocValue);
            responsePayloadDocValue = responsePayloadDocValue.substring(1,responsePayloadDocValue.length-1); //Check if this required.
            responsePayloadDocValue = self.escapeSpecialChars(responsePayloadDocValue);

            var responsePayload = JSON.stringify(jQuery("[data-role='response-payload']").val());
            responsePayload = responsePayload.substring(1,responsePayload.length-1); //Check if this required.
            responsePayload = self.escapeSpecialChars(responsePayload)

            responsePayloadDocValue = ', "response": { "doc": "' + responsePayloadDocValue + '"  ,"sample" : "' + responsePayload + '", "contentType" : "' + jQuery("[data-role='response-content-type']").text() + '" }';
            jsonBody += responsePayloadDocValue;
        }
        jsonBody += '}';
        var headersList = [];
        // Basic auth info.
        headersList.push({"name" : "Authorization", "value" : basicAuth});
        jQuery("#working_alert").fadeIn();

        self.makeAJAXCall({"url":operationPath,type:"put",dataType:"json","headers": headersList, data:jsonBody,"contentType":"application/json","callback":self.handleAPICallSuccess, "errorCallback" :self.handleUpdateFailure });
        jQuery(this).siblings("[contenteditable='true']").removeClass("edit");
        jQuery(this).siblings("a.allow_edit.cancel").hide();
        jQuery(this).siblings("a.allow_edit.ok").hide();
        if (jQuery(this).parent().hasClass('popover-content')) {
            jQuery(this).parent().parent().hide();
        }
        if (jQuery(this).siblings("textarea").is(":visible")) {
            jQuery(this).siblings("textarea").hide();
            jQuery(this).siblings("[data-allow-edit='true']").html(jQuery(this).siblings("textarea").val()).removeClass("edit").removeClass("editing").show();
        }
        jQuery(this).hide();

        e.stopPropagation();
        return false;
    };
    /**
     * Inline edit update AJAX call success handler.
     * Updates the modified values .
     */
    this.handleAPICallSuccess = function(data) {
        currentEdiatableElementValue = jQuery.trim(currentEdiatableElement.html());
        jQuery("[data-role='method-title']").html(data.displayName);
        jQuery("[data-role='method-description']").html(data.description); // Set the description.
        // Set the query/header param values.
        jQuery("[data-role='query-param-list'][data-scope='method'], [data-role='header-param-list'][data-scope='method']").each(function(index) {
            var currentLIElement = jQuery(this);
            var paramName = jQuery.trim(currentLIElement.find("[data-role='name']").text());
            var paramStyle = (currentLIElement.attr('data-role') == 'query-param-list') ? "QUERY" : "HEADER";
            if (data.request) {
                if (data.request.parameters && data.request.parameters.length) {
                    for (var i=0;i<data.request.parameters.length; i++) {
                        var param  = data.request.parameters[i];
                        if (param.name == paramName && param.style == paramStyle) {
                            currentLIElement.find("[data-role='description']").html(param.description)
                        }
                    }
                }
            }
        });
        if (typeof Drupal != "undefined" && typeof Drupal.settings != "undefined") {
            var windowURL = window.location.href;
            var drupalURL = windowURL = windowURL.replace("?editMode=1","?flush=1").replace("?editMode=2","?flush=1");
            self.makeAJAXCall({"url":drupalURL, "callback":self.drupalUpdateSuccess});
        }
        currentEdiatableElementValue = "";
    };
    this.drupalUpdateSuccess = function() {
    }
    this.handleUpdateFailure = function() {
        //console.log(currentEdiatableElementValue);
        self.resetEditableElement();
        self.showError("Error saving changes.");
    };
};
Apigee.APIModel.InlineEdit.prototype = new Apigee.APIModel.Common();
