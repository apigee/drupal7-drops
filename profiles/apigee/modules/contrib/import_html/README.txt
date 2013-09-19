
Import HTML

@author - Dan Morrison <http://coders.co.nz>

About
-----------------------------
For full details, see the import_html_help.htm

Installing tidy on Mac OSX.
-----------------------------
The distribution of tidy that comes with Tiger & Lion is OLD.
  "HTML Tidy for Mac OS X released on 31 October 2006 – Apple Inc. build 15.3"
Life is better if you use a more recent built. Some GREAT instructions at
http://www.skrinakcreative.com/wp/2010/03/installing-htmltidy-for-mac-os-xsnow-leopard/
will update it to 
  "HTML Tidy for Mac OS X released on 25 March 2009"


Drush
-----------------------------
Upgrade: Jan 2011
A drush command is now available. See drush help import-html.
It does not support setting extended profile options, 
just allows already-configured settings to be triggered to import 
specific file paths or folders.

Features
-----------------------------
Upgrade: Jan 2011
import_html now provides 'features' import/export of the import profiles.
The profiles should be available when creating features through the UI.

Upgrade: Oct 2006
-----------------------------
Significant changes were made to the file exclusion regular expression
process. The original defaults should work as well as they ever did,
but the new default config is:

 ^_
 /_
 CVS
 ^\.
 /\.
 
- this prevents files found INSIDE skipped directories from turning up.
