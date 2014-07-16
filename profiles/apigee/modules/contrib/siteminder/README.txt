What is Siteminder?
===================

The Siteminder module allows for users to authenticate to a Drupal site when the site is being run within the Siteminder environment.  Siteminder itself is a single sign-on application made by Computer Associates.  You can read more about the Siteminder system here http://www.ca.com/us/internet-access-control.aspx


How Siteminder module works
===========================

The basics:  The Siteminder module works by looking at what headers a user's Siteminder-enabled browser is sending to the Drupal website.  The Siteminder module uses one of these headers as the key identifier which maps to a user id in Drupal.  IMPORTANT: See security note below.  If the module is able to find a mathing ID in Drupal, the users is logged in to Drupal.  Otherwise, a new user is created.

Security
========

The Siteminder module should only be used on a Drupal site entirely protected by the Siteminder system.  If the site is not entirely protected by the Siteminder system, there are obvious security implications and the module should not be used under such circumstances.  Contact the maintainer of this module for any questions.


Credits
=======

This module is sponsored by Development Seed with contributions from the World Bank.  A Siteminder module was originally developed by Phase 2 Technologies.  I've taken this module and done a near-complete rewrite.  The authentication system works close to the same, but I rewrote the majority of everything else.
