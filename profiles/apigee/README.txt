CONTENTS OF THIS FILE
---------------------

 * About Apigee Developer Portal install profile
 * About Drupal Installation Profiles
 * Custom Development

ABOUT APIGEE DEVELOPER PORTAL INSTALL PROFILE
-----------------------------------------------
As an API provider, you need a way to expose your APIs, educate developers about
your APIs, sign up developers, and let developers register apps. Exposing your
APIs to developers is only part of creating a truly dynamic community. You also
need a way for your developer community to provide feedback, make support and
feature requests, and submit their own content that can be accessed by other
developers.

Learn more about Apigee Developer Portal at:
http://apigee.com/docs/developer-services/content/what-developer-portal

ABOUT DRUPAL INSTALLATION PROFILES
----------------------------------

The Apigee Developer Portal is a Drupal install profile. Drupal install profiles
allow us to define steps and configuration in order to enable and configure a
standard Drupal installation with extra modules and theme to create an out of
the box Apigee Developer Portal.

Learn more about Drupal installation profiles at:
https://www.drupal.org/developing/distributions

CUSTOM DEVELOPMENT
------------------

IMPORTANT: Do not modify any existing files or modules, or you will not be able
to upgrade your site!

If you want to modify the look and feel of the site, do not modify
the themes in this profile. Instead, create a sub-theme of the theme you want
to modify and put it in /sites/all/themes.  To learn how to subtheme the Apigee
Responsive theme, read the readme in the
/profile/apigee/themes/apigee_responsive directory.

Do not modify any of the Apigee modules in this profile. Instead, you need to
create your own custom module to override behavior using Drupal hooks, which
is the standard Drupal customization process.  The module should be placed in
/sites/all/modules/custom, not in this directory.

If you want to use a different contrib module than what we have in this profile,
download the different module version to /sites/all/modules/contrib.  Any
modules in the /sites/all/modules/contrib will have a higher precedence than
any modules in this profile.
