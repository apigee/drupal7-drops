<?php

/**
 * @file
 * 
 * Plugin to provide an argument handler for user uid *and* "me"
 */

/**
 * Plugins are described by creating a $plugin array which will be used
 * by the system that includes this file.
 */
$plugin = array(
  'title' => t("User: ID (me supported)"),
  // keyword to use for %substitution
  'keyword' => 'user',
  'description' => t('Creates a user context from a user ID argument or the %me alias.', array('%me' => _me_get_me_alias(TRUE))),
  'context' => 'me_me_uid_context',
  'placeholder form' => array(
    '#type' => 'textfield',
    '#description' =>  t('Enter the user ID of a user for this argument or the %me alias.', array('%me' => _me_get_me_alias(TRUE))),
  ),
  // 'default' => array('to_arg' => TRUE),
  // 'path placeholder' => '%user', // This is in pagemanager.
  // 'path placeholder to_arg' => TRUE,
  // 'no ui' => TRUE,
);


/**
 * Discover if this argument gives us the user we crave.
 */
function me_me_uid_context($arg = NULL, $conf = NULL, $empty = FALSE) {
  // If unset it wants a generic, unfilled context.
  if ($empty) {
    return ctools_context_create_empty('user');
  }

  // We can accept either a node object or a pure nid.
  if (is_object($arg)) {
    return ctools_context_create('user', $arg);
  }

  // Convert the "me" argument if needed.
  $arg = _me_check_arg($arg);

  if (!is_numeric($arg)) {
    return NULL;
  }

  $account = user_load($arg);
  if (!$account) {
    return NULL;
  }

  return ctools_context_create('user', $account);
}
