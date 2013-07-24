This module is for administrative purposes.

It is useful if you want to lock your site for a certain amount of time,
but don't need the maintenance mode.

It is great when using DTAP environments. For instance, when making a
copy of your Production website to your Accept environment. In the accept
environment you perform all updates. When all updates have been performed,
you can now copy your accept environment back to the production environment,
without losing data!

How? The 'Read Only Mode' module puts your site in a Read-Only modus,
where people can still access all nodes and comments.

Usage of this module is very simple:
* After installing you can find the option to enable read-only mode for your
  site under "Administration" -> "Configuration" -> "Development" ->
  "Maintenance mode" (path: admin/config/development/maintenance).
* Here you can put your site in Read-Only mode, and leave a message for
  your visitors or enter a URL to redirect them to.
* During this Read-Only mode, all users are permitted from any forms.
