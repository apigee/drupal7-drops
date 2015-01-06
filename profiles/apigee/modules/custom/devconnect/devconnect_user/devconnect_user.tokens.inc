<?php


/**
 * Implements hook_token_info().
 */
function devconnect_user_token_info() {
  $type = array(
    'name' => t('Developer'),
    'description' => t('Tokens related to developers'),
    'needs-data' => 'developer'
  );
  $dev['apps'] = array(
    'name' => t('Apps'),
    'description' => t('List of all Apps owned by this developer.'),
  );
  $dev['uid'] = array(
    'name' => t('Developer User ID'),
    'description' => t('Drupal User ID (uid) of this developer.'),
  );
  $dev['email'] = array(
    'name' => t('Developer Email'),
    'description' => t('Email address associated with this developer.'),
  );
  $dev['developerId'] = array(
    'name' => t('Developer ID'),
    'description' => t('Developer ID (internal to KMS) for this developer.'),
  );
  $dev['firstName'] = array(
    'name' => t('First Name'),
    'description' => t('Developer’s first name.'),
  );
  $dev['lastName'] = array(
    'name' => t('Last Name'),
    'description' => t('Developer’s last name.'),
  );
  $dev['userName'] = array(
    'name' => t('User Name'),
    'description' => t('Developer’s Drupal username.'),
  );
  $dev['status'] = array(
    'name' => t('Status'),
    'description' => t('Status of this developer.'),
  );
  /*
  $dev['attributes'] = array(
    'name' => t('Attributes'),
    'description' => t('Custom attributes for this developer.'),
  );
  */
  $dev['createdAt'] = array(
    'name' => t('Created At'),
    'description' => t('Timestamp at which the developer profile was created in Edge.'),
  );
  $dev['modifiedAt'] = array(
    'name' => t('Modified At'),
    'description' => t('Timestamp at which the developer profile was modified in Edge.'),
  );
  $dev['devUrl'] = array(
    'name' => t('Developer Profile URL'),
    'description' => t('URL on this site where the developer’s profile may be viewed.'),
  );

  return array(
    'types' => array('developer' => $type),
    'tokens' => array('developer' => $dev),
  );
}

/**
 * Implements hook_tokens().
 */
function devconnect_user_tokens($type, array $tokens, array $data = array(), array $options = array()) {
  if ($type != 'developer' || empty($data['developer'])) {
    return array();
  }
  $url_options = array('absolute' => TRUE);
  if (isset($options['language'])) {
    $url_options['language'] = $options['language'];
    $language_code = $options['language']->language;
  }
  else {
    $language_code = NULL;
  }
  $sanitize = !empty($options['sanitize']);
  $dev = $data['developer'];
  $replacements = array();
  foreach ($tokens as $name => $original) {
    switch ($name) {
      case 'devUrl':
        $replacements[$original] = url('user/' . $dev['uid'], $url_options);
        break;

      // Timestamps.
      case 'createdAt':
      case 'modifiedAt':
        $replacements[$original] = format_date(floor($dev[$name] / 1000), 'medium', '', NULL, $language_code);;
        break;

      // Arrays of strings.
      case 'apps':
        $replacements[$original] = $sanitize ? check_plain(implode(', ', $dev[$name])) : implode(', ', $dev[$name]);
        break;

      default:
        $replacements[$original] = $sanitize ? check_plain($dev[$name]) : $dev[$name];
        break;
    }
  }

  return $replacements;
}

