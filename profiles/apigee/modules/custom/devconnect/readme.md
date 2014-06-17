Main readme moved to Apigee-Drupal repo: [https://github.com/apigee/apigee-drupal/blob/7.x-1.x/README.md](https://github.com/apigee/apigee-drupal/blob/7.x-1.x/README.md)

Summary of submodules:

* devconnect_admin_notify: enabled by default
* devconnect_app_attributes: enabled by default
* devconnect_blog: enabled by default
* devconnect_blog_content_types: enabled during apigee.profile install process
* devconnect_content_admin: disabled by default. Feature. Should we deprecate this?
* devconnect_content_authoring: disabled by default. Feature. Should we deprecate this?
* devconnect_content_creation_menu: enabled by default
* devconnect_context: enabled by default (dependency of devconnect_default_structure).
* devconnect_debug: disabled by default. HIDDEN.
* devconnect_default_structure: enabled in new installs via apigee.profile.
* devconnect_developer_apps: enabled by default.
* devconnect_docgen: disabled by default.
* devconnect_downloads: disabled by default. Used by some customers.
* devconnect_homepage: enabled by default. Supplies header block for homepage.
* devconnect_key_value_map: disabled by default. For use in customer customizations.
* devconnect_monetization: disabled by default. Used by some customers.
* devconnect_partner: disabled by default. HIDDEN.
* devconnect_ui: disabled by default. Deprecated; will be removed in 14.08
* devconnect_user: enabled by default.
* devconnect_views: enabled by default (dependency of devconnect_default_structure).
* devportal_updates: disabled by default. For use with OPDK sites only.

Available Translations:

* English (default)
* Japanese (ja) (pending)
** Instructions on installing languages
> 1) Install Content Translation, Internationalization, Locale, Localization Update, and other contributed
> translation modules.
>
> 2) Take the desired language from the module (arranged by language symbol)
> and upload it to the language update page.
>
> 3) Ensure the translation strings are in the database, and in the strings translation page.

