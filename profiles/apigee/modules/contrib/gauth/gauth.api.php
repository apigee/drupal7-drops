<?php

/**
 * @file
 * Hooks provided by the GAuth module.
 */

/**
 * @addtogroup hooks
 * @{
 */

/**
 * A gauth account was created.
 *
 * The module should save its custom additions.
 *
 * Note that when this hook is invoked, the changes have not yet been written to
 * the database.
 *
 * @param $account
 *   The account array which will be written to db.
 */
function hook_gauth_account_insert($account) {
  db_insert('mytable')
    ->fields(array(
      'myfield' => $edit['myfield'],
      'uid' => $account->uid,
    ))
    ->execute();
}

/**
 * A gauth account was updated.
 *
 * Modules may use this hook to update their data in a custom storage
 * after a gauth account has been updated.
 *
 * @param $edit
 *   The array of account details submitted by the user.
 * @param $account
 *   The original array of account from the db.
 */
function hook_gauth_account_update(&$edit, $account) {
  db_insert('account_changes')
    ->fields(array(
      'id' => $account['id'],
      'changed' => time(),
    ))
    ->execute();
}

/**
 * Respond to gauth account deletion.
 *
 * This hook is invoked from gauth_account_delete()
 * is called and before account is actually removed from the database.
 *
 * @param $account
 *   The account that is being deleted.
 */
function hook_gauth_account_delete($account) {
  db_delete('mytable')
    ->condition('id', $account['id'])
    ->execute();
}

/**
 * A google response was received.
 *
 * Modules may use this hook to carry operations based on google response.
 * This is helpful when response other than authentication are received.
 * Google response have data in url so $_GET can be used in this function.
 */
function hook_gauth_google_response() {
  if (isset($_GET['state'])) {
    $state = json_decode(stripslashes($_GET['state']));
    $action = $state->action;
    // Some other code to handle things.
  }
}
/**
 * @} End of "addtogroup hooks".
 */
