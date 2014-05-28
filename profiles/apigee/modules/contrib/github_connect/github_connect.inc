<?php

/**
 * @file
 * Github connect utility functions.
 */

/**
 * Get user from GitHub access token.
 *
 * @param $token Access token from GitHub
 * @return $user Drupal user
 */
function github_connect_get_token_user($token) {
  if ($token) {
    $result = db_select('github_connect_users', 'g_u')
      ->fields('g_u', array('uid', 'access_token'))
      ->condition('access_token', $token, '=')
      ->execute()
      ->fetchAssoc();

    $uid = $result['uid'];
    if (empty($uid)) {
      return FALSE;
    }

    return user_load($uid);
  }
}

/**
 * Get GitHub access token for User
 *
 * @param $uid User user id
 * @return $token Access token from GitHub
 */
function github_connect_get_user_token($uid) {
  if ($uid) {
    $result = db_select('github_connect_users', 'g_u')
      ->fields('g_u', array('uid', 'access_token'))
      ->condition('uid', $uid, '=')
      ->execute()
      ->fetchAssoc();

    $token = $result['access_token'];
    return $token;
  }
}

/**
 * Register new user.
 */
function _github_connect_register($github_user, $token) {
  module_load_include('inc', 'github_connect');

  $username = $github_user['login'];

  $userinfo = array(
    'name' => $username,
    'mail' => $github_user['email'],
    'pass' => user_password(),
    'status' => 1,
    'access' => REQUEST_TIME,
    'init' => $github_user['email'],
  );

  $account = user_save('', $userinfo);

  if ($account) {
    _github_connect_save_github_user($account, $token);

    // Log in the stored user.
    _github_connect_user_login($account);

    drupal_goto();
  }
  else {
    drupal_set_message(t('Error saving new user.'), 'error');
    return;
  }
}

/**
 * Save the new GitHub user in github_connect_users
 */
function _github_connect_save_github_user($account, $token) {
  $github_user = _github_connect_get_github_user_info($token);

  // Set the authmap
  user_set_authmaps($account, array('authname_github_connect' => $github_user['html_url']));

  // Store GitHub user with token.
  if ($account) {
    db_insert('github_connect_users')
      ->fields(array(
      'uid' => $account->uid,
      'access_token' => $token,
      'timestamp' => REQUEST_TIME,
    ))
      ->execute();
  }
}

/**
 * Log the user with the given account in
 */
function _github_connect_user_login($account) {
  $form_state['uid'] = $account->uid;
  user_login_submit(array(), $form_state);
}

/**
 * Get the user info provided by github
 *
 * @param $token The token for the github user
 */
function _github_connect_get_github_user_info($token) {
  $cache = &drupal_static(__FUNCTION__);

  if (!is_null($cache)) {
    $github_user = $cache;
  } else {
    // Collects the User information from GitHub.
    $options = array(
      'method' => 'GET',
      'timeout' => 7200,
    );
    $ghuser = drupal_http_request('https://api.github.com/user?' . $token, $options);
    $github_user = drupal_json_decode($ghuser->data);

    $github_user_emails = _github_connect_get_github_user_emails($token);
    $github_user['email'] = $github_user_emails[0]['email'];
  }

  return $github_user;
}

/**
 * Get the private email addresses from the user
 *
 * @param $token The token for the github user
 */
function _github_connect_get_github_user_emails($token) {
  $cache = &drupal_static(__FUNCTION__);

  if (!is_null($cache)) {
    $github_user_emails = $cache;
  } else {
    // Collects the User information from GitHub.
    $options = array(
      'method' => 'GET',
      'timeout' => 7200,
    );
    $ghuser = drupal_http_request('https://api.github.com/user/emails?' . $token, $options);
    $github_user_emails = drupal_json_decode($ghuser->data);
  }

  return $github_user_emails;
}
