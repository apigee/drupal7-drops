<script type="text/javascript">
  var proxyUrl = '<?php print $proxy; ?>';
  var apiModelEndPoint = '<?php print $endpoint; ?>';
  jQuery(function() {
    var windowLocation = window.location.href.toString();
    var code = getQueryParam(windowLocation, "code");
    if (code != "") {
      code = (code.indexOf("#") != -1) ? code.split("#")[0] : code;
      var requestUrl = proxyUrl+"?targeturl="+encodeURIComponent(apiModelEndPoint+"/o/"+getQueryParam(windowLocation, "org")+"/apimodels/"+getQueryParam(windowLocation, "api")+"/revisions/"+getQueryParam(windowLocation, "revision")+"/authschemes/oauth2WebServerFlow");
      jQuery.ajax({
        url:requestUrl,
        //cache: false,
        type: "get",
        success:function(data, textStatus, jqXHR) {
          var responseContent = unescape(data.responseContent);
          responseContent = jQuery.parseJSON(responseContent);
          var accessTokenUrl = proxyUrl+"?targeturl=";
          accessTokenUrl += encodeURIComponent(responseContent.accessTokenUrl+"?code="+code+"&grant_type=authorization_code&client_id="+responseContent.clientId+"&client_secret="+responseContent.clientSecret+"&redirect_uri="+windowLocation.split("code=")[0]);
          jQuery.ajax({
            url:accessTokenUrl,
            //cache: false,
            type: "get",
            success:function(data, textStatus, jqXHR) {
              var accessTokenContent = unescape(data.responseContent);
              accessTokenContent = jQuery.parseJSON(accessTokenContent);
              if(accessTokenContent.access_token) {
                window.opener.setAccessTokenAndLocation("", "", accessTokenContent.access_token, responseContent.accessTokenType, responseContent.accessTokenParamName, proxyUrl);
              } else {
                var error = 'No Access Token from provider was given.';
                window.opener.setAccessTokenAndLocation(error, error,"","", "", "");
              }
              self.close();
            },
            error: function(xhr, status, error) {
              var error = 'Access Token call has failed.';
              window.opener.setAccessTokenAndLocation(error, error,"","", "", "");
              self.close();
            }
          });
        },
        error: function(xhr, status, error) {
          var error = 'Invalid credentials.';
          window.opener.setAccessTokenAndLocation(error, error,"","", "", "");
          self.close();
        }
      });
    }

  });
  getQueryParam = function(queryURL , paramName) {
    var QueryString = queryURL.split("?"); // Get the QueryString from the URL.
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
</script>