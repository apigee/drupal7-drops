CONTENTS OF THIS FILE
---------------------

 * Introduction
 * Installation


INTRODUCTION
------------

This module prints a debug log of each API call made to the Apigee Management API on the bottom
of the page, including the time spent waiting for responses.

This module uses the kpr() function from the devel module to print out the API call debug information.

The authorization information will be masked out from the debug log.


INSTALLATION
------------

1. Enable the module just like any other Drupal module.

2. Grant 'access development information' permission to any roles you want to see the log displayed.

3. When the system executes a drupal_goto(), the debug log will be lost. Go into
   Development > Devel Settings and turn on "Display redirection page" to make sure you do not miss any
   debug information.
