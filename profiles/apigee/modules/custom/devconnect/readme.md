# Summary of modules:

## Required core functionality
* `devconnect`: Exposes and configures the Edge API.
* `devconnect_user`: Maps Drupal users to Edge developers.
* `devconnect_developer_apps`: Enables functionality related to apps and keys.

## Optional core functionality
* `devconnect_app_attributes`: Allows configuration of custom attributes for apps. Enabled by default for apigee profile.
* `devconnect_apiproduct_access`: Allows RBAC for API products.
* `devconnect_key_value_map`: Thin wrapper around Edge SDK to allow key-value map use. Currently entirely unused.
* `devconnect_monetization`: Exposes Mint functionality.
* `smartdocs`: Exposes Modeling functionality. Enabled by default for apigee profile.

## Profile-related functionality
* `devconnect_admin_notify`: Notifies admin of new user registrations. Enabled by default for apigee profile.
* `devconnect_blog`: Exposes blog eye candy. Enabled by default for apigee profile.
* `devconenct_blog_content_types`: Creates image, audio, video content-types at install time. No other functionality. Required by `devconnect_blog` and therefore enabled by default for apigee profile.
* `devconnect_content_creation_menu`: Hand-holding for newbie content creators. Enabled by default for apigee profile.
* `devconnect_context`: Sets up context rules for block placement. Required by `devconnect_default_structure` and therefore enabled by default for apigee profile.
* `devconnect_default_structure`: Configures blocks, views and menus. Enabled by default for apigee profile.
* `devconnect_homepage`: Provides the home page header block. Enabled by default for apigee profile.
* `devconnect_monitor`: Sends emails when watchdog logs an error.
* `devconnect_status`: Provides a status report on Edge configuration.
* `devconnect_views`: Provides default views used by profile. Required by `devconnect_default_structure` and therefore enabled by default for apigee profile.

## Deprecated/Hidden
* `devconnect_debug`: Displays all Edge calls at bottom of page. Hidden, can be useful for troubleshooting.
