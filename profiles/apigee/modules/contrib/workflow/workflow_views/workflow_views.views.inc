<?php

/**
 * @file
 * Provide views data and handler information for workflow.module.
 */

/**
 * @defgroup views_workflow_module workflow.module handlers
 *
 * Includes the 'workflow_node' and 'workflow_state' tables, but generates a
 * pseudo-table for each separate workflow that's been set up.
 * @{
 */

/**
 * Implements hook_views_data().
 */
function workflow_views_views_data() {

  // This is a basic set of subfield data. It is then replicated for each table.
  $field_data['sid'] = array(
    'title' => t('New state'),
    'help' => t('The new state in a transition. (For the latest transition, this is the current state.)'),
    'field' => array(
      'handler' => 'workflow_views_handler_field_sid',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'workflow_views_handler_filter_sid',
      'numeric' => TRUE,
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
      'click sortable' => TRUE,
      'numeric' => TRUE,
      'name table' => 'workflow_states',
      'name field' => 'state',
    ),
  );

  $field_data['old_sid'] = array(
    'title' => t('Old state'),
    'help' => t('The old (previous) state in a transition.'),
    'field' => array(
      'handler' => 'workflow_views_handler_field_sid',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'workflow_views_handler_filter_sid',
      'numeric' => TRUE,
    ),
  );
  $field_data['stamp'] = array(
    'title' => t('Time'),
    'help' => t('The time of a state change.'),
    'field' => array(
      'handler' => 'views_handler_field_date',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_date',
      'numeric' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort_date',
    ),
  );
  $field_data['comment'] = array(
    'title' => t('Comment'),
    'help' => t('The comment describing why the the state change was done.'),
    'field' => array(
      'handler' => 'views_handler_field_xss',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
      'numeric' => TRUE,
    ),
  );
  $field_data['uid'] = array(
    'title' => t('Previous user'),
    'help' => t('The user that executed a state change.'),
    'field' => array(
      'handler' => 'workflow_views_handler_field_username',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_user_name',
    ),
    'relationship' => array(
      'title' => t('User'),
      'help' => t("The User ID of the state change."),
      'base' => 'users',
      'base field' => 'uid',
      'handler' => 'views_handler_relationship',
      'label' => t('user'),
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
    ),
  );

  // Workflow node.
  // Only use when 'Workflow Node' is active. Use separate title.
  if (module_exists('workflownode')) {
    $data['workflow_node'] = $field_data;
    $data['workflow_node']['table']['group'] = t('Workflow node');
    $data['workflow_node']['table']['join'] = array(
      'node' => array(
        'field' => 'nid',
        'left_table' => 'node',
        'left_field' => 'nid',
      ),
    );
    $data['workflow_node']['sid']['title'] = t('Current state');
    $data['workflow_node']['sid']['help'] = t('The current workflow state that the node is in.');
    // $data['workflow_node']['old_sid']['title'] = t('Previous state');
    // $data['workflow_node']['old_sid']['help'] = t('The previous state that the node was in.');
    unset($data['workflow_node']['old_sid']); // Workflow Node does not have this.

    $data['workflow_node']['comment']['title'] = t('Scheduled comment');
    $data['workflow_node']['comment']['help'] = t('A comment describing why the node was scheduled for state transition.');
    $data['workflow_node']['uid']['title'] = t('Last changing user');
    $data['workflow_node']['uid']['help'] = t('The user who performed the last state change.');
    $data['workflow_node']['stamp']['title'] = t('Current time');
    $data['workflow_node']['stamp']['help'] = t('The time at which the node moved to the current state.');

    // The following 'uid' handlers are different between 'workflow node' and
    // the other ones. At the moment of rewriting in 2013: I'm not what's best,
    // and I don't want to break anything, so I'll just leave this untouched.
    unset($data['workflow_node']['uid']['argument']);
    unset($data['workflow_node']['uid']['filter']);
    $data['workflow_node']['uid'] += array(
      'argument' => array(
        'handler' => 'views_handler_argument_user_uid',
        'click sortable' => TRUE,
        'name table' => 'workflow_node',
        'name field' => 'uid',
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_user_name',
        'numeric' => TRUE,
        'name table' => 'workflow_node',
        'name field' => 'uid',
      ),
    );
  }

  // Workflow scheduled transition.
  $data['workflow_scheduled_transition'] = $field_data;
  $data['workflow_scheduled_transition']['table']['group'] = t('Workflow schedule');
  $data['workflow_scheduled_transition']['table']['join'] = array(
    'node' => array(
      'field' => 'nid',
      'left_table' => 'node',
      'left_field' => 'nid',
    ),
  );
  $data['workflow_scheduled_transition']['sid']['title'] = t('Scheduled state');
  $data['workflow_scheduled_transition']['sid']['help'] = t('The state, the node is scheduled for.');
  $data['workflow_scheduled_transition']['old_sid']['title'] = t('Current state');
  $data['workflow_scheduled_transition']['old_sid']['help'] = t('The current state that the node is in.');
  $data['workflow_scheduled_transition']['comment']['title'] = t('Scheduled comment');
  $data['workflow_scheduled_transition']['comment']['help'] = t('A comment describing why the node was scheduled for state transition.');
  $data['workflow_scheduled_transition']['uid']['title'] = t('User');
  $data['workflow_scheduled_transition']['uid']['help'] = t('The user that scheduled the state change.');

  // Scheduled transitions do not have a stamp, but are scheduled.
  $data['workflow_scheduled_transition']['scheduled'] = $data['workflow_scheduled_transition']['stamp'];
  $data['workflow_scheduled_transition']['scheduled'] = t('Scheduled time');
  $data['workflow_scheduled_transition']['scheduled'] = t('The time at which the node will change workflow states.');
  unset($data['workflow_scheduled_transition']['stamp']);

  // Workflow node history.
  $data['workflow_node_history'] = $field_data;
  $data['workflow_node_history']['table']['group'] = t('Workflow history');
  $data['workflow_node_history']['table']['join'] = array(
    'node' => array(
      'field' => 'nid',
      'left_table' => 'node',
      'left_field' => 'nid',
    ),
  );
  $data['workflow_node_history']['sid']['title'] = t('New state');
  $data['workflow_node_history']['sid']['help'] = t('The new state in a transition. (For the latest transition, this is the current state.)');
  $data['workflow_node_history']['old_sid']['title'] = t('The old (previous) state in a transition.');
  $data['workflow_node_history']['old_sid']['help'] = t('The previous state, from the last state change.');
  $data['workflow_node_history']['comment']['title'] = t('Comment');
  $data['workflow_node_history']['comment']['help'] = t('The comment describing why the the state change was done.');
  $data['workflow_node_history']['uid']['title'] = t('Previous user');
  $data['workflow_node_history']['uid']['help'] = t('The user that executed a state change.');
  $data['workflow_node_history']['stamp']['title'] = t('Time');
  $data['workflow_node_history']['stamp']['help'] = t('The time of the last state change.');

  // Information about the last state transition.
  // This mostly the same as 'workflow_node_history', so just copy it
  // and change the titles.
  // DO not use the realtionship to {workflow_node}, but directly to {node}
  $data['workflow_node_current'] = $field_data;
  $data['workflow_node_current']['table']['group'] = t('Workflow current');
  $data['workflow_node_current']['table']['join'] = array(
    // For reference, the relationship used in Workflow versions below 7.x-2.x
    // 'node' => array(
    //   'table' => 'workflow_node_history',
    //   'field' => 'nid',
    //   'left_table' => 'workflow_node',
    //   'left_field' => 'nid',
    //   'extra' => 'workflow_node.stamp = workflow_node_current.stamp AND workflow_node.nid = workflow_node_current.nid',
    // ),
    'node' => array(
      'table' => 'workflow_node_history',
      'field' => 'hid',
      'left_table' => 'node',
      'left_field' => 'nid',
      // Do not use the default 'views_join', but add a subquery. 
      'handler' => 'views_join_subquery',
      'left_query' => "  SELECT max(hid) FROM {workflow_node_history} where nid = node.nid and sid != old_sid ",
    ),
  );
  $data['workflow_node_current']['sid']['title'] = t('Current state');
  $data['workflow_node_current']['sid']['help'] = t('The current state, from the last state change.');
  $data['workflow_node_current']['old_sid']['title'] = t('Previous state');
  $data['workflow_node_current']['old_sid']['help'] = t('The previous state, from the last state change.');
  $data['workflow_node_current']['comment']['title'] = t('Current comment');
  $data['workflow_node_current']['comment']['help'] = t('The comment describing why the the state change was done.');
  $data['workflow_node_current']['uid']['title'] = t('Current user');
  $data['workflow_node_current']['uid']['help'] = t('The user that executed a state change.');
  $data['workflow_node_current']['stamp']['title'] = t('Time of last state change.');
  $data['workflow_node_current']['stamp']['help'] = t('The time of the last state change.');

  // Workflow states.
  // @todo: implement this as a proper relationship (like 'user').
  //        to support Workflow Field, do not use workflow_node
  if (module_exists('workflownode')) {
    $data['workflow_states']['table']['group'] = t('Workflow states (deprecated)');
    $data['workflow_states']['table']['join'] = array(
      'node' => array(
        'field' => 'sid',
        'left_table' => 'workflow_node',
        'left_field' => 'sid',
      ),
    );
    $data['workflow_states']['weight'] = array(
      'title' => t('State weight'),
      'help' => t('The weight of the current workflow state that the node is in.'),
      'sort' => array(
        'handler' => 'views_handler_sort',
      ),
    );
    $data['workflow_states']['state'] = array(
      'title' => t('Current state name'),
      'help' => t('The readable name of the workflow state that the node is in. (Less efficient, use only when click-sorting by state name.)'),
      'field' => array(
        'handler' => 'workflow_views_handler_field_state',
        'click sortable' => TRUE,
      ),
    );
  }

  return $data;
}

/**
 * @}
 */
