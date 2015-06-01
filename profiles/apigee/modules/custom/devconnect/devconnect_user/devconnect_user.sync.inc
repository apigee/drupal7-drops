<?php
/**
 * @file
 * Handles developer-sync functionality
 */

/**
 * Confirm form for DevConnect Developer Sync.
 *
 * @param array $form
 *   The form being created.
 * @param array $form_state
 *   State of the form being created.
 *
 * @return array
 *   The newly-created form.
 */
function devconnect_user_sync_confirm_form($form, $form_state) {
  $form['#submit'][] = 'devconnect_user_sync';
  return confirm_form($form, t('Are you sure you want to sync users with Edge?'), 'admin/people');
}

/**
 * Submit callback for DevConnect Developer Sync.
 *
 * Makes sure that all local users have a corresponding account in Edge;
 * likewise makes sure that all Edge users (other than those created by
 * unit tests) have Drupal accounts.
 *
 * It creates batch processes to avoid php timeouts.
 *
 * Batch process has two callbacks _devconnect_user_sync_to_edge and
 * _devconnect_user_sync_from_edge, one pushes missing developer in Edge
 * from Dev Portal and the other from Edge to Dev Portal respectively.
 *
 * Each batch process works with 10 developers per iteration.
 */
function devconnect_user_sync() {

  // Fetch list of developers' email addresses on Edge.
  /** @var DeveloperController $controller */
  $controller = entity_get_controller('developer');
  $edge_developers_orig = $controller->listEmails();
  $edge_developers = array();
  // If there are relatively few developers, fetch them all at once.
  if (count($edge_developers_orig) < 200) {
    $edge_developers_orig = entity_load('developer');
    foreach ($edge_developers_orig as $email => $data) {
      $edge_developers[strtolower($email)] = $data;
    }
    if (function_exists('drush_print')) {
      drush_print('Syncing all developers from Edge at once.');
    }
  }
  else {
    foreach ($edge_developers_orig as $email) {
      $edge_developers[strtolower($email)] = $email;
    }
    if (function_exists('drush_print')) {
      drush_print('Syncing developers from Edge one at a time.');
    }
  }
  // Fetch list of users in Drupal.
  $stmt = db_select('users', 'u')
    ->fields('u', array('mail', 'uid'))
    ->condition('u.uid', 1, '>')
    ->execute();
  $drupal_users = array();
  while ($user_data = $stmt->fetchAssoc()) {
    $drupal_users[strtolower($user_data['mail'])] = (object) $user_data;
  }

  // Get a case-insensitive diff of developer emails.
  $not_in_edge = array();
  foreach ($drupal_users as $key => $value) {
    if (!array_key_exists($key, $edge_developers)) {
      $not_in_edge[$key] = $value;
    }
  }
  $not_in_drupal = array_diff_key($edge_developers, $drupal_users);
  $offset = 10;
  $operations = array();
  for ($i = 0, $len = count($not_in_drupal); $i < $len; $i += $offset) {
    $operations[] = array(
      '_devconnect_user_sync_from_edge',
      array(
        array_slice($not_in_drupal, $i, $offset, TRUE),
        array('total' => $len, 'offset' => $offset),
      ),
    );
  }

  for ($i = 0, $len = count($not_in_edge); $i < $len; $i += $offset) {
    $operations[] = array(
      '_devconnect_user_sync_to_edge',
      array(
        array_slice($not_in_edge, $i, $offset, TRUE),
        array('total' => $len, 'offset' => $offset),
      ),
    );
  }

  $devconnect_user_sync_batch = array(
    'operations' => $operations,
    'finished' => '_devconnect_user_sync_finished',
    'title' => t('Synchronizing Dev Portal Developers'),
    'init_message' => t('Initiating Synchronization'),
    'progress_message' => t('Processed @current out of @total.'),
    'error_message' => t('Synchronization of Dev Portal Developers has encountered an error.'),
  );
  batch_set($devconnect_user_sync_batch);

  // If not run by drush, redirect to People admin page.
  if (!drupal_is_cli()) {
    batch_process('admin/people');
  }
  else {
    $batch =& batch_get();
    $batch['progressive'] = FALSE;
    drush_backend_batch_process();
  }
}

/**
 * Batch callback to save developer from Edge to Dev Portal.
 *
 * @param array $developers
 *   List of developers to be synced from Edge.
 * @param array $counters
 *   State variable containing total and offset within the series of batches.
 * @param array $context
 *   Batch context.
 */
function _devconnect_user_sync_from_edge($developers, $counters, &$context) {
  if (!isset($context['sandbox']['progress'])) {
    $context['sandbox']['progress'] = 0;
    $context['sandbox']['current_developer'] = 0;
    $context['sandbox']['max'] = $counters['total'];
    $context['results'][] = array(
      'message' => t('from Edge to Dev Portal.'),
      'processed_dc' => 0,
      'unit_test_dc' => 0,
    );
  }

  // Were we passed an array of developer objects or developer emails?
  $first_item = reset($developers);
  if (is_string($first_item)) {
    // Avoid holding onto developer entity cache so that we don't exhaust the
    // maximum number of available open file handles.
    $developers = entity_load('developer', $developers, array('cache' => FALSE));
  }

  // For each developer in Edge that is not in Drupal, create user in Drupal.
  foreach ($developers as $email => $developer) {
    // Skip any cruft users left over from failed unit tests.
    if (preg_match('!^[a-z0-9]{8}@example\.com$!i', $developer->email) || substr($developer->email, 0, 7) == 'phpunit') {
      $context['results'][0]['unit_test_dc']++;
      $context['sandbox']['progress']++;
      continue;
    }
    $edge_user_name = $developer->userName;
    // Now find an untaken Drupal username. If there is already a user with
    // the name in question, start appending numbers until we find an untaken
    // name.
    $extra = 0;
    $drupal_name = $edge_user_name;
    do {
      $uid = db_select('users', 'u')
        ->fields('u', array('uid'))
        ->condition('name', $drupal_name)
        ->condition('uid', 0, '>')
        ->execute()
        ->fetchField();
      if (!$uid) {
        break;
      }
      $drupal_name = $edge_user_name . $extra++;
    } while (TRUE);

    $account = (object) array('is_new' => TRUE);

    $edit = array(
      'name' => $drupal_name,
      'mail' => $developer->email,
      'field_first_name' => array(
        LANGUAGE_NONE => array(
          0 => array('value' => $developer->firstName),
        ),
      ),
      'field_last_name' => array(
        LANGUAGE_NONE => array(
          0 => array('value' => $developer->lastName),
        ),
      ),
      'status' => ($developer->status == 'active' ? 1 : 0),
      'pass' => user_password(),
      'already_in_kms' => !$developer->forceSync,
    );
    $drupal_user = user_save($account, $edit);
    if ($drupal_user) {
      $is_admin = devconnect_user_is_org_admin($drupal_user->mail);
      $context['results'][] = t('@type Drupal user @name (@mail) created with uid @uid', array(
          '@type' => ($is_admin ? t('Administrative') : t('Ordinary')),
          '@name' => $drupal_user->name,
          '@mail' => $drupal_user->mail,
          '@uid' => $drupal_user->uid,
        ));
      $context['results'][0]['processed_dc']++;
      if ($is_admin) {
        // Assign all administrator roles to this user.
        $admin_rids = devconnect_user_get_admin_rids();
        if (!empty($admin_rids)) {
          $query = db_insert('users_roles')->fields(array('uid', 'rid'));
          foreach ($admin_rids as $rid) {
            $query->values(array('uid' => $drupal_user->uid, 'rid' => $rid));
          }
          $query->execute();
        }
      }
    }
    else {
      $context['results'][] = t('Drupal user @name not created.', array('@name' => $drupal_name));
    }
    $context['sandbox']['progress']++;
    $context['sandbox']['current_developer'] = $email;
  }

  $context['message'] = t('%completed developers out of %total synced to Dev Portal', array(
    '%completed' => $context['results'][0]['processed_dc'],
    '%total' => $context['sandbox']['max'],
  ));
  $context['finished'] = 1;
}

/**
 * Batch callback to save developer from Dev Portal to Edge.
 *
 * @param array $developers
 *   List of developers to be saved from Dev Portal to Edge.
 * @param array $counters
 *   State variable containing total and offset within the series of batches.
 * @param array $context
 *   Batch context.
 */
function _devconnect_user_sync_to_edge($developers, $counters, &$context) {
  if (!isset($context['sandbox']['progress'])) {
    $context['sandbox']['progress'] = 0;
    $context['sandbox']['current_developer'] = 0;
    $context['sandbox']['max'] = $counters['total'];
    $context['results'][] = array(
      'message' => t('from Dev Portal to Edge.'),
      'processed_edge' => 0,
      'unit_test_edge' => 0,
    );
  }
  foreach ($developers as $email => $developer) {
    if (preg_match('!^[a-z0-9]{8}@example\.com$!i', $email)) {
      $context['results'][0]['unit_test_edge']++;
      $context['sandbox']['progress']++;
      continue;
    }
    $result = _devconnect_user_save_entity($developer);
    $context['results'][] = $result;
    $context['results'][0]['processed_edge'] += strpos($result, 'not') ? 0 : 1;
    $context['sandbox']['progress']++;
    $context['sandbox']['current_developer'] = $email;
  }
  $context['message'] = t('%completed developers processed out of %total synced to Edge', array(
    '%completed' => $context['results'][0]['processed_edge'],
    '%total' => $context['sandbox']['max'],
  ));
  $context['finished'] = 1;
}

/**
 * Finish batch callback to sync developers.
 *
 * @param bool $success
 *   TRUE if batch was successful.
 * @param array $results
 *   Contains result statistics for batch run.
 * @param array $operations
 *   Contains all operations that were queued.
 */
function _devconnect_user_sync_finished($success, $results, $operations) {
  if ($success) {
    $statistics = $results[0];
    // Guarantee some default values.
    $statistics += array(
      'processed_edge' => 0,
      'processed_dc' => 0,
      'unit_test_edge' => 0,
      'unit_test_dc' => 0,
    );
    $unit_test = $statistics['unit_test_edge'] + $statistics['unit_test_dc'];

    if (function_exists('drush_print')) {
      drush_print('Sync completed.');
      drush_print($statistics['processed_edge'] . ' account(s) created in Edge');
      drush_print($statistics['processed_dc'] . ' account(s) created in Dev Portal');
      drush_print($unit_test . ' unit testing account(s) skipped');
      return;
    }

    $message = '';
    if ($statistics['processed_edge']) {
      $message .= t('@count created in Edge', array('@count' => $statistics['processed_edge'])) . '<br>';
    }
    if ($statistics['processed_dc']) {
      $message .= t('@count created in Dev Portal', array('@count' => $statistics['processed_dc'])) . '<br>';
    }
    if ($unit_test) {
      $message .= t('@count unit testing accounts skipped', array('@count' => $unit_test)) . '<br>';
    }

    if (strlen($message) > 0) {
      array_shift($results);
      $results = array_filter($results, function ($item) {
        return !is_array($item) && trim($item) != '';
      });
      foreach (array_keys($results) as $key) {
        $results[$key] = array(
          'id' => 'Item' . $key,
          'data' => $results[$key],
        );
      }
      $message .= theme_item_list(array(
          'items' => $results,
          'title' => t('Synced Developers'),
          'type' => 'ul',
          'attributes' => array(),
        ));
    }
    else {
      $message = t('All developer accounts are in sync.');
    }
    drupal_set_message($message);
  }
  else {
    $error_operation = reset($operations);
    $message = t('An error occurred while processing %error_operation with arguments: @arguments', array(
        '%error_operation' => $error_operation[0],
        '@arguments' => print_r($error_operation[1], TRUE),
      ));
    drupal_set_message($message, 'error');
  }
}
