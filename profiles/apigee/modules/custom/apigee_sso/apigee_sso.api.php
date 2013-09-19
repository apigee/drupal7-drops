<?php

/**
 * Allows alteration of parameters used in OpenID authentication with Google.
 *
 * @param array $params
 *   Has members with the following keys, any of which can be altered:
 *   - scheme (defaults to current scheme, http or https)
 *   - redirect (defaults to 'node' unless a 'destination' param is present in
 *               the query string)
 *   - openid_identifier (defaults to https://www.google.com/accounts/o8/id)
 */
function hook_apigee_sso_alter(&$params) {
  $params['redirect'] = '<front>';
}