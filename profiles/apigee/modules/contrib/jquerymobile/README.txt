Module: jQuery Mobile
Author: Jason Savino <http://drupal.org/user/411241>

Requirements
============
Libraries module http://drupal.org/project/libraries

Installation
============
Module
--------------------------------
  Copy the 'jquerymobile' module directory into your Drupal
  sites/all/modules directory as usual.

  Install the framework file using one of the two methods below

 * Manual Method
 1. create a folder in the 'sites/all' directory name 'libraries'
 
 2. create a folder in the newly create 'sites/all/libraries' directory
     named jquery-VERSION ex. jquery-1.6.4

 3. create a folder in the newly create 'sites/all/libraries' directory
     named jquery-mobile-VERSION ex. jquery-mobile-1.0.1
      
 4. download and extract minified and uncompressed version of
     the jquery framework version into the newly created 
     'sites/all/libraries/jquery-VERSION' directory
     -Minified: http://code.jquery.com/jquery-1.6.4.min.js
     -Uncompressed: http://code.jquery.com/jquery-1.6.4.js
     
 5. download and extract the jquery mobile framework in the
     newly created 'sites/all/libraries/jquery-mobile-VERSION' directory
     -Stable: http://code.jquery.com/mobile/1.0.1/jquery.mobile-1.0.1.zip

 * Drush Method
Using Drush, you can now download and extract the jquerymobile framework. 

 1. To enable jquerymobile and install the framework files
     - drush en jquerymobile
 2. To install the framework files only (useful for updating the jquerymobile framework files)
     -  to download required libraries
        drush jquerymobile
     -- OR --
     -  to download required libraries while specifying the options
        drush jquerymobile --jqm=1.0.1 --jquery=1.6.4 --path=sites/all/libraries

Usage
=====

You can put the settings in your site's settings.php file if you choose. 
**NOTE: Doing so will prevent changes from being made through the configuration form.

Just copy, paste and modify the following:

$conf['jquerymobile_library_path'] = "sites/all/libraries";
$conf['jquerymobile_jquerymobile_version'] = "1.0.1";
$conf['jquerymobile_jquery_version'] = "1.6.4";
$conf['jquerymobile_minify'] = 1;
$conf['jquerymobile_mobile_themes'] = array("mobile_jquery" => "mobile_jquery");


