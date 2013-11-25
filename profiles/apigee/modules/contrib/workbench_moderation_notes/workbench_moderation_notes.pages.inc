<?php

/**
 * Page callback for workbench moderation view with notes
 */
function workbench_moderation_notes_node_history_view($node) {
  global $user;

  // Alert if there is no live node
  if (!isset($node->workbench_moderation['published'])) {
    drupal_set_message(t('Currently there is no published revision of this site.'), 'warning');
  }

  // Get all of the node revisions, each with its most recent moderation.
  $query = db_select('node', 'n');
  $query->leftJoin('node_revision', 'r', 'n.nid = r.nid');
  $query->leftJoin('users', 'u', 'r.uid = u.uid');
  $query->addField('n', 'vid', 'live_revision');
  $query->condition('n.nid', $node->nid)
    ->orderBy('r.vid', 'DESC')
    ->fields('r', array('nid', 'vid', 'title', 'log', 'uid', 'timestamp'))
    ->fields('u', array('name'));

  $revisions = $query->execute()
    ->fetchAllAssoc('vid');

  // Build the table rows.
  $rows = array();
  foreach ($revisions as $revision) {
    $row = array(
      'data' => array(
        'date' => '',
        'revision' => '',
        'moderation' => '',
      ),
      'class' => array('revision'),
    );

    // Add row classes.
    if ($revision->vid == $node->workbench_moderation['current']->vid) {
      $row['class'][] = 'current-revision';
    }
    if (isset($node->workbench_moderation['published']) && $revision->vid == $node->workbench_moderation['published']->vid) {
      $row['class'][] = 'published-revision';
    }
    else {
      $row['class'][] = 'unpublished-revision';
    }

    // Add row data.
    $row['data']['date'] = format_date($revision->timestamp, 'short');

    // Revision operations.
    $revision_operations = array();
    if (isset($node->workbench_moderation['published']) && $revision->vid == $node->workbench_moderation['published']->vid) {
      $revision_operations['view'] = workbench_moderation_access_link(t('view'), "node/{$revision->nid}");
    }
    elseif ($revision->vid == $node->workbench_moderation['current']->vid) {
      $revision_operations['view'] = workbench_moderation_access_link(t('view'), "node/{$revision->nid}/current-revision");
    }
    else {
      $revision_operations['view'] = workbench_moderation_access_link(t('view'), "node/{$revision->nid}/revisions/{$revision->vid}/view");
    }

    $revision_operations['revert'] = workbench_moderation_access_link(t('revert'), "node/{$revision->nid}/revisions/{$revision->vid}/revert");
    $revision_operations['delete'] = workbench_moderation_access_link(t('delete'), "node/{$revision->nid}/revisions/{$revision->vid}/delete");

    $row['data']['revision'] = implode(' | ', array_filter($revision_operations));

    // Get the moderation history of this revision.
    $query = db_select('workbench_moderation_node_history', 'm');
    $query->leftJoin('users', 'u', 'm.uid = u.uid');
    $query->condition('m.vid', $revision->vid)
      ->orderBy('m.stamp', 'DESC')
      ->fields('m')
      ->fields('u', array('name'));

    $moderations = $query->execute()
      ->fetchAllAssoc('hid');

    // Make sure it's an array.
    if (empty($moderations)) {
      $moderations = array();
      if ($revision->vid == $node->workbench_moderation['current']->vid) {
        $moderations = array($node->workbench_moderation['current']);
      }
    }

    // If this is the current revision, provide moderation links.
    if (!empty($node->workbench_moderation['published']) && $revision->vid == $node->workbench_moderation['published']->vid) {
      $row['data']['moderation'] = '<div class="moderation-state"><strong>' . t('This is the published revision.') . '</strong></div>';
      // Provide an unpublish link.
      $next_states = workbench_moderation_states_next(workbench_moderation_state_published(), $user, $node->type);
      if (!empty($next_states)) {
        $row['data']['moderation'] .= '<div class="moderation-actions">' . l(t('Unpublish'), "node/{$revision->nid}/moderation/{$revision->live_revision}/unpublish") . '</div>';
      }
    }
    elseif ($revision->vid == $node->workbench_moderation['current']->vid) {
      $node_current = workbench_moderation_node_current_load($node);
      $row['data']['moderation'] = '<div class="moderation-state"><strong>' . t('This is the current revision. The current state is %state.', array('%state' => workbench_moderation_state_label($node->workbench_moderation['current']->state))) . '</strong></div>';
      $moderate_form = drupal_get_form('workbench_moderation_moderate_form', $node_current);
    }

    // Build a table of the moderation history.
    $moderation_rows = array();
    $items = array();
    foreach ($moderations as $moderation) {
      if ($moderation->from_state) {
        $note = ($moderation->note)
                  ?t('<br/><strong>Note:</strong><p>@note</p>', array('@note' => $moderation->note))
                  : '';
        $items[] = t('From @from_state --> @to_state on %date by !user !note',
          array(
            '@from_state' => workbench_moderation_state_label($moderation->from_state),
            '@to_state' => workbench_moderation_state_label($moderation->state),
            '%date' => format_date($moderation->stamp, 'short'),
            '!user' => theme('username', array('account' => $moderation)),
            '!note' => $note,
          )
        );
      }
      else {
        $items[] = t('Created as @to_state on %date by !user',
          array(
            '@from_state' => workbench_moderation_state_label($moderation->from_state),
            '@to_state' => workbench_moderation_state_label($moderation->state),
            '%date' => format_date($moderation->stamp, 'short'),
            '!user' => theme('username', array('account' => $moderation)),
          )
        );
      }
    }
    $row['data']['moderation'] .= theme('item_list', array('items' => $items));

    $rows[] = $row;
  }

  // Set the table header.
  $header = array(t('Date'), t('Actions'), t('Moderation notes'));

  // Return properly styled output.
  return array(
    '#attached' => array(
      'css' => array(
        drupal_get_path('module', 'workbench_moderation') . '/css/workbench_moderation.css',
      ),
    ),
    '#prefix' => drupal_render($moderate_form),
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
  );
}