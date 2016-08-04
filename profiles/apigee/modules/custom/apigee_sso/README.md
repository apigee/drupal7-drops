“Authenticate with Apigee” will only work under the following conditions:

- The `apigee_account` module is enabled.
- The hostname is either “localhost” or ends in “apigee.com”.
- The page was served via https.

In all other cases, the “Authenticate with Apigee” button will not be shown.

To show the button for Twitter federated login, download the `twitter_signin`
contrib module to `sites/all/modules`, enable it, and configure the appropriate
consumer key and secret.

To show the button for GitHub federated login, enable the `github_connect`
contrib module and configure the appropriate GitHub client_id.
