This is a starter kit for all the theme developers who wish to customise the look and feel of the Apigee developer portal and yet maintain the responsive features provided by the Apigee Responsive theme.

This starter kit theme will not have any features on its own. You might need to build your features and customisation using the skeleton provided by this starter kit theme.

In order to get your theme up and working you need to make the following changes

* Copy the apigee_responsive_starterkit folder to your sites/all/themes folder
* Rename the apigee_responsive_starterkit folder to YOUR_THEME_NAME
* Rename the apigee_responsive_starterkit.info file to YOUR_THEME_NAME.info
* Rename the css/apigee_responsive_starterkit.css to css/YOUR_THEME_NAME.css
* Rename the js/apigee_responsive_starterkit.js to YOUR_THEME_NAME.js
* Edit YOUR_THEME_NAME.info file and replace the following lines 
	stylesheets[all][] = css/apigee_responsive_starterkit.css
	scripts[] = js/apigee_responsive_starterkit.js
    with
	stylesheets[all][] = css/YOUR_THEME_NAME.css
	scripts[] = js/YOUR_THEME_NAME.js
* Change the name of the theme in YOUR_THEME_NAME.info file from Apigee Responsive Starter Kit to YOUR THEME NAME
* If you wish to add a new screenshot for your theme then replace the existing screenshot.png with you theme's screenshot.png
*If you wish to add a new logo to your theme then replace the exiting logo.png with your themes logo.png
*If you wish to add a new favicon to your theme replace the existing favicon.ico with your themes favicon.ico

