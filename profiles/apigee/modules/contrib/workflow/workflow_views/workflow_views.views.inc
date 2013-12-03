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

  // Workflow node.
  // Only use when 'Workflow Node' is active. Use separate title.
  if (module_exists('workflownode')) {
    $data['workflow_node']['table']['group'] = t('Workflow node');
    $data['workflow_node']['table']['join'] = array(
      'node' => array(
        'field' => 'nid',
        'left_table' => 'node',
        'left_field' => 'nid',
      ),
    );
    $data['workflow_node']['sid'] = array(
      'title' => t('Current state'),
      'help' => t('The current workflow state that the node is in.'),
      'field' => array(
        'handler' => 'workflow_views_handler_field_sid',
        'click sortable' => TRUE,
      ),
      'argument' => array(
        'handler' => 'views_handler_argument_numeric',
        'click sortable' => TRUE,
        'numeric' => TRUE,
        'name table' => 'workflow_states',
        'name field' => 'state',
      ),
      'filter' => array(
        'handler' => 'workflow_views_handler_filter_sid',
        'numeric' => TRUE,
      ),
    );
    $data['workflow_node']['stamp'] = array(
      'title' => t('Current time'),
      'help' => t('The time at which the node moved to the current state.'),
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
    $data['workflow_node']['uid'] = array(
      'title' => t('Last changed user'),
      'help' => t('The user who performed the last state change.'),
      'relationship' => array(
        'base' => 'users',
        'base field' => 'uid',
        'handler' => 'views_handler_relationship',
        'label' => t('User'),
      ),
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
  $data['workflow_scheduled_transition']['table']['group'] = t('Workflow schedule');
  $data['workflow_scheduled_transition']['table']['join'] = array(
    'node' => array(
      'field' => 'nid',
      'left_table' => 'node',
      'left_field' => 'nid',
    ),
  );
  $data['workflow_scheduled_transition']['sid'] = array(
    'title' => t('Scheduled state'),
    'help' => t('The current workflow state that the node is in.'),
    'field' => array(
      'handler' => 'workflow_views_handler_field_sid',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'workflow_views_handler_filter_sid',
      'numeric' => TRUE,
    ),
  );
  $data['workflow_scheduled_transition']['scheduled'] = array(
    'title' => t('Scheduled time'),
    'help' => t('The time at which the node will change workflow states.'),
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
  $data['workflow_scheduled_transition']['comment'] = array(
    'title' => t('Scheduled comment'),
    'help' => t('A comment describing why the node was scheduled for state transition.'),
    'field' => array(
      'handler' => 'views_handler_field_xss',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
      'numeric' => TRUE,
    ),
  );

  // Workflow node history.
  $data['workflow_node_history']['table']['group'] = t('Workflow history');
  $data['workflow_node_history']['table']['join'] = array(
    'node' => array(
      'field' => 'nid',
      'left_table' => 'node',
      'left_field' => 'nid',
    ),
  );
  $data['workflow_node_history']['sid'] = array(
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
  );
  $data['workflow_node_history']['old_sid'] = array(
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
  $data['workflow_node_history']['stamp'] = array(
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
  $data['workflow_node_history']['comment'] = array(
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
  $data['workflow_node_history']['uid'] = array(
    'title' => t('Previous user'),
    'help' => t('The user that executed a state change.'),
    'field' => array(
      'handler' => 'workflow_views_handler_field_username',
      'click sortable' => TRUE,
    ),
    'relationship' => array(
      'title' => t('Author'),
      'help' => t("The User ID of the state change."),
      'base' => 'users',
      'base field' => 'uid',
      'handler' => 'views_handler_relationship',
      'label' => t('author'),
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_user_name',
    ),
  );

  // Information about the last state transition.
  // This mostly the same as 'workflow_node_history', so just copy it
  // and change the titles.
  // DO not use the realtionship to {workflow_node}, but directly to {node}
  $data['workflow_node_current'] = $data['workflow_node_history'];
  $data['workflow_node_current']['table']['group'] = t('Workflow current');
  // Explain how this table joins to others.
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
  $data['workflow_node_current']['stamp']['title'] = t('Time of last state change.');
  $data['workflow_node_current']['stamp']['help'] = t('The time of the last state change.');
  $data['workflow_node_current']['comment']['title'] = t('Current comment');
  $data['workflow_node_current']['uid']['title'] = t('Current user');

  return $data;
}

/**
 * @}
 */
