<?php
/**
 * @file
 * Tokens hooks for Workflow module.
 */

/**
 * Implements hook_tokens().
 */
function workflow_tokens($type, $tokens, array $data = array(), array $options = array()) {
  $replacements = array();
  $sanitize = !empty($options['sanitize']);
  $langcode = !empty($options['language']->language) ? $options['language']->language : LANGUAGE_NONE;
  $date = REQUEST_TIME;

  if (($type == 'node' || $type == 'workflow') && !empty($data['node'])  && !empty($data['node']->nid)) {
    $node = $data['node'];

    // Get a list of all possible states for returning state names.
    $states = workflow_get_workflow_states_all();

    if ($workflow = workflow_get_workflow_type_map_by_type($node->type)) {
      // @TODO: If we ever move Workflow to allow M:M content types to workflows, this will need updating.
      if ($workflow = workflow_get_workflows_by_wid($workflow->wid)) {
        $wid = $workflow->wid;
        $last_history = workflow_get_recent_node_history($node->nid);

        if (isset($node->workflow) && !isset($node->workflow_stamp)) {
          // The node is being submitted but the form data has not been saved to the database yet,
          // so we set the token values from the workflow form fields.
          $sid = $node->workflow;
          $old_sid = isset($last_history->old_sid) ? $last_history->old_sid : workflow_get_creation_state_by_wid($workflow->wid);
          $user_name = $node->uid ? (isset($node->name) ? $node->name
            : variable_get('anonymous', 'Anonymous'))
            : variable_get('anonymous', 'Anonymous');
          $uid = $node->uid;
          $account = user_load($node->uid);
          $mail = ($node->uid && isset($node->user_mail)) ? $node->user_mail : '';
          $comment = isset($node->workflow_comment) ? $node->workflow_comment : '';
        }
        else {
          if (empty($last_history)) {
            // If the node has no workflow history,
            // the node is being inserted and will soon be transitioned to the first valid state.
            // We find this state using the same logic as workflow_nodeapi().
            if ($choices = workflow_field_choices($node)) {
              // @TODO: Some users, like anonymous, may not be allowed
              // to cause transitions, so there will be no choices.
              $keys = array_keys($choices);
              $sid = array_shift($keys);
              $old_sid = workflow_get_creation_state_by_wid($workflow->wid);
            }
            else {
              // What to choose?
              $sid = $node->workflow;
              $old_sid = $workflow->creation_state;
            }
            $sid = workflow_get_workflow_states_by_sid($sid)->sid;
            $old_sid = workflow_get_workflow_states_by_sid($old_sid)->sid;
            $user_name = $node->uid ? $node->name : variable_get('anonymous', 'Anonymous');
            $uid = $node->uid;
            $account = user_load($node->uid);
            $mail = ($node->uid && isset($node->user_mail)) ? $node->user_mail : '';
            $comment = isset($node->workflow_comment) ? $node->workflow_comment : '';
          }
          else {
            // Sometimes there is no workflow set (edit?).
            if (!isset($node->workflow)) {
              $node->workflow = $last_history->sid;
            }
            // Is a transition in progress?
            if ($node->workflow != $last_history->sid) {
              $sid = $node->workflow;
              $old_sid = $last_history->sid;
              $date = $node->workflow_stamp;
              $uid = $node->uid;
              $account = user_load($node->uid);
            }
            else {
              // Not a transition.
              $sid = $last_history->sid;
              $old_sid = $last_history->old_sid;
              $date = $last_history->stamp;
              $uid = $last_history->uid;
              $account = user_load($last_history->uid);
            }

            // Default to the most recent transition data in the workflow history table.
            $user_name = $account->uid ? $account->name : variable_get('anonymous', 'Anonymous');
            $mail = $account->uid ? $account->mail : '';
            $comment = $last_history->comment;
          }
        }

        foreach ($tokens as $name => $original) {
          switch ($name) {
            case 'workflow-name':
              $replacements[$original] = $sanitize ? check_plain($workflow->name) : $workflow->name;
              break;
            case 'workflow-current-state-name':
              $replacements[$original] = $sanitize ? check_plain($states[$sid]) : $states[$sid];
              break;
            case 'workflow-old-state-name':
              $replacements[$original] = $sanitize ? check_plain($states[$old_sid]) : $states[$old_sid];
              break;
            case 'workflow-current-state-updating-user-name':
              $name = format_username($account);
              $replacements[$original] = $sanitize ? check_plain($name) : $name;
              break;
            case 'workflow-current-state-updating-user-link':
              $replacements[$original] = theme('username', array('account' => $account));
              break;
            case 'workflow-current-state-updating-user-uid':
              // User IDs are integers only and do not need sanitization.
              $replacements[$original] = $uid;
              break;
            case 'workflow-current-state-updating-user-mail':
              $replacements[$original] = $sanitize ? check_plain($account->mail) : $account->mail;
              break;
            case 'workflow-current-state-updating-user-mailto':
              $replacements[$original] = '<a href="mailto:' . check_plain($account->mail) . '">' . check_plain($user_name) . '</a>';
              break;
            case 'workflow-current-state-log-entry':
              $replacements[$original] = $sanitize ? check_markup($comment, filter_default_format(), $langcode) : $comment;
              break;
            case 'workflow-current-state-date-iso':
              $replacements[$original] = format_date($date, 'custom', 'c', NULL, $langcode);
              break;
            case 'workflow-current-state-date-tstamp':
              $replacements[$original] = $sanitize ? check_plain($date) : $date;
              break;
            case 'workflow-current-state-date-formatted':
              $replacements[$original] = format_date($date, 'medium', '', NULL, $langcode);
              break;
            default:
              // Add support for custom date formats. (see token.tokens.inc)
              // @todo Remove when http://drupal.org/node/1173706 is fixed.
              $date_format_types = system_get_date_types();
              foreach ($date_format_types as $date_type => $date_info) {
                if ($name == 'workflow-current-state-date-' . $date_type) {
                  $replacements[$original] = format_date($date, $date_type, '', NULL, $langcode);
                }
              }
              break;
          }
        }
      }
    }
  }

  return $replacements;
}

/**
 * Implements hook_token_info().
 */
function workflow_token_info() {
  $type = array(
    'name' => t('Workflow'),
    'description' => t('Tokens related to workflows.'),
    'needs-data' => 'node',
    );

  $tokens = array();
  $tokens['workflow-name'] = array(
    'name' => t('Workflow name'),
    'description' => t('Name of workflow appied to this node'),
    );
  $tokens['workflow-current-state-name'] = array(
    'name' => t('Current state name'),
    'description' => t('Current state of content'),
    );
  $tokens['workflow-old-state-name'] = array(
    'name' => t('Old state name'),
    'description' => t('Old state of content'),
    );
  $tokens['workflow-current-state-updating-user-name'] = array(
    'name' => t('Username of last state changer'),
    'description' => t('Username of last state changer'),
    );
  $tokens['workflow-current-state-updating-user-link'] = array(
    'name' => t('Username link of last state changer'),
    'description' => t('Themed Username of last state changer'),
    );
  $tokens['workflow-current-state-updating-user-uid'] = array(
    'name' => t('Uid of last state changer'),
    'description' => t('uid of last state changer'),
    );
  $tokens['workflow-current-state-updating-user-mail'] = array(
    'name' => t('Email of last state changer'),
    'description' => t('email of last state changer'),
    );
  $tokens['workflow-current-state-updating-user-mail'] = array(
    'name' => t('Email link of last state changer'),
    'description' => t('email link of last state changer'),
    );
  $tokens['workflow-current-state-log-entry'] =array(
    'name' => t('Last workflow comment log'),
    'description' => t('Last workflow comment log'),
    );
  $tokens['workflow-current-state-date-iso'] = array(
    'name' => t('Current state date (ISO)'),
    'description' => t('Date of last state change (ISO)'),
    );
  $tokens['workflow-current-state-date-tstamp'] = array(
    'name' => t('Current state date (timestamp)'),
    'description' => t('Date of last state change (timestamp)'),
    );
  $tokens['workflow-current-state-date-formatted'] = array(
    'name' => t('Current state date (formatted by site default)'),
    'description' => t('Date of last state change (formatted by site default)'),
    );

  // Add support for custom date formats. (see token.tokens.inc)
  // @todo Remove when http://drupal.org/node/1173706 is fixed.
  $date_format_types = system_get_date_types();
  foreach ($date_format_types as $date_type => $date_info) {
    $tokens['workflow-current-state-date-' . $date_type] = array(
      'name' => t('Current state date (using @format format)', array('@format' => $date_info['title'])),
      'description' => t("A date in '@type' format. (%date)", array('@type' => $date_type, '%date' => format_date(REQUEST_TIME, $date_type))),
      'module' => 'workflow',
      );
  }

  return array(
    'types' => array('workflow' => $type),
    'tokens' => array('workflow' => $tokens, 'node' => $tokens),
  );
}
