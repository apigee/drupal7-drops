<?php

/**
 * @file
 * Administrative interface for taxonomy access control.
 */

/**
 * Page callback: Renders the TAC permissions administration overview page.
 *
 * @return
 *   Form to render.
 *
 * @see taxonomy_access_menu()
 */
function taxonomy_access_admin() {
  $roles = _taxonomy_access_user_roles();
  $active_rids = db_query(
    'SELECT rid FROM {taxonomy_access_default} WHERE vid = :vid',
    array(':vid' => TAXONOMY_ACCESS_GLOBAL_DEFAULT)
  )->fetchCol();

  $header = array(t('Role'), t('Status'), t('Operations'));
  $rows = array();

  foreach ($roles as $rid => $name) {
    $row = array();
    $row[] = $name;

    if (in_array($rid, $active_rids)) {
      // Add edit operation link for active roles.
      $row[] = array('data' => t('Enabled'));

    }
    else {
      // Add enable link for unconfigured roles.
      $row[] = array('data' => t('Disabled'));
    }
    $row[] = array('data' => l(
      t("Configure"),
      TAXONOMY_ACCESS_CONFIG . "/role/$rid/edit",
      array('attributes' => array('class' => array('module-link', 'module-link-configure')))));
    $rows[] = $row;
  }

  $build['role_table'] = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
  );

  return $build;
}

/**
 * Form constructor for a form to to delete access rules for a particular role.
 *
 * @param int $rid
 *   The role ID to disable.
 *
 * @see taxonomy_access_role_delete_confirm_submit()
 * @see taxonomy_access_menu()
 * @ingroup forms
 */
function taxonomy_access_role_delete_confirm($form, &$form_state, $rid) {
  $roles = _taxonomy_access_user_roles();
  if (taxonomy_access_role_enabled($rid)) {
    $form['rid'] = array(
      '#type' => 'value',
      '#value' => $rid,
    );
    return confirm_form($form,
      t("Are you sure you want to delete all taxonomy access rules for the role %role?",
        array('%role' => $roles[$rid])),
      TAXONOMY_ACCESS_CONFIG . '/role/%/edit',
      t('This action cannot be undone.'),
      t('Delete all'),
      t('Cancel'));
  }
}

/**
 * Form submission handler for taxonomy_role_delete_confirm().
 */
function taxonomy_access_role_delete_confirm_submit($form, &$form_state) {
  $roles = _taxonomy_access_user_roles();
  $rid = $form_state['values']['rid'];
  if (is_numeric($rid)
      && $rid != DRUPAL_ANONYMOUS_RID
      && $rid != DRUPAL_AUTHENTICATED_RID) {
    if ($form_state['values']['confirm']) {
      $form_state['redirect'] = TAXONOMY_ACCESS_CONFIG;
      taxonomy_access_delete_role_grants($rid);
      drupal_set_message(t('All taxonomy access rules deleted for role %role.',
          array('%role' => $roles[$rid])));
    }
  }
}

/**
 * Generates a URL to enable a role with a token for CSRF protection.
 *
 * @param int $rid
 *   The role ID.
 *
 * @return string
 *   The full URL for the request path.
 */
function taxonomy_access_enable_role_url($rid) {
  // Create a query array with a token to validate the sumbission.
  $query = drupal_get_destination();
  $query['token'] = drupal_get_token($rid);

  // Build and return the URL with the token and destination.
  return url(
    TAXONOMY_ACCESS_CONFIG . "/role/$rid/enable",
    array('query' => $query)
  );
}

/**
 * Page callback: Enables a role if the proper token is provided.
 *
 * @param int $rid
 *   The role ID.
 */
function taxonomy_access_enable_role_validate($rid) {
  $rid = intval($rid);
  // If a valid token is not provided, return a 403.
  $query = drupal_get_query_parameters();
  if (empty($query['token']) || !drupal_valid_token($query['token'], $rid)) {
    return MENU_ACCESS_DENIED;
  }
  // Return a 404 for the anonymous or authenticated roles.
  if (in_array($rid, array(DRUPAL_ANONYMOUS_RID, DRUPAL_AUTHENTICATED_RID))) {
    return MENU_NOT_FOUND;
  }
  // Return a 404 for invalid role IDs.
  $roles = _taxonomy_access_user_roles();
  if (empty($roles[$rid])) {
    return MENU_NOT_FOUND;
  }

  // If the parameters pass validation, enable the role and complete redirect.
  if (taxonomy_access_enable_role($rid)) {
    drupal_set_message(t('Role %name enabled successfully.',
      array('%name' => $roles[$rid])));
  }
  drupal_goto();
}

/**
 * Form constructor for a form to manage grants by role.
 *
 * @param int $rid
 *   The role ID.
 *
 * @see taxonomy_access_admin_form_submit()
 * @see taxonomy_access_menu()
 * @ingroup forms
 */
function taxonomy_access_admin_role($form, $form_state, $rid) {
  // Always include the role ID in the form.
  $rid = intval($rid);
  $form['rid'] = array('#type' => 'value', '#value' => $rid);

  // For custom roles, allow the user to enable or disable grants for the role.
  if (!in_array($rid, array(DRUPAL_ANONYMOUS_RID, DRUPAL_AUTHENTICATED_RID))) {
    $roles = _taxonomy_access_user_roles();

    // If the role is not enabled, return only a link to enable it.
    if (!taxonomy_access_role_enabled($rid)) {
      $form['status'] = array(
        '#markup' => '<p>' . t(
          'Access control for the %name role is disabled. <a href="@url">Enable @name</a>.',
          array(
            '%name' => $roles[$rid],
            '@name' => $roles[$rid],
            '@url' => taxonomy_access_enable_role_url($rid))) . '</p>'
      );
      return $form;
    }
    // Otherwise, add a link to disable and build the rest of the form.
    else {
      $disable_url = url(
        TAXONOMY_ACCESS_CONFIG . "/role/$rid/delete",
        array('query' => drupal_get_destination())
      );
      $form['status'] = array(
        '#markup' => '<p>' . t(
          'Access control for the %name role is enabled. <a href="@url">Disable @name</a>.',
          array('@name' => $roles[$rid], '%name' => $roles[$rid], '@url' => $disable_url)) . '</p>'
      );
    }
  }

  // Retrieve role grants and display an administration form.
  // Disable list filtering while preparing this form.
  taxonomy_access_disable_list();

  // Fetch all grants for the role.
  $defaults =
    db_query(
      'SELECT vid, grant_view, grant_update, grant_delete, grant_create,
              grant_list
       FROM {taxonomy_access_default}
       WHERE rid = :rid',
      array(':rid' => $rid))
    ->fetchAllAssoc('vid', PDO::FETCH_ASSOC);

  $records =
    db_query(
      'SELECT ta.tid, td.vid, ta.grant_view, ta.grant_update, ta.grant_delete,
              ta.grant_create, ta.grant_list
       FROM {taxonomy_access_term} ta
       INNER JOIN {taxonomy_term_data} td ON ta.tid = td.tid
       WHERE rid = :rid',
      array(':rid' => $rid))
    ->fetchAllAssoc('tid', PDO::FETCH_ASSOC);
  $term_grants = array();
  foreach ($records as $record) {
    $term_grants[$record['vid']][$record['tid']] = $record;
  }

  // Add a fieldset for the global default.
  $form['global_default'] = array(
    '#type' => 'fieldset',
    '#title' => t('Global default'),
    '#description' => t('The global default controls access to untagged nodes. It is also used as the default for disabled vocabularies.'),
    '#collapsible' => TRUE,
    // Collapse if there are vocabularies configured.
    '#collapsed' => (sizeof($defaults) > 1),
  );
  // Print term grant table.
  $form['global_default']['grants'] = taxonomy_access_grant_add_table($defaults[TAXONOMY_ACCESS_GLOBAL_DEFAULT], TAXONOMY_ACCESS_VOCABULARY_DEFAULT);

  // Fetch all vocabularies and determine which are enabled for the role.
  $vocabs = array();
  $disabled = array();
  foreach (taxonomy_get_vocabularies() as $vocab) {
    $vocabs[$vocab->vid] = $vocab;
    if (!isset($defaults[$vocab->vid])) {
      $disabled[$vocab->vid] = $vocab->name;
    }
  }

  // Add a fieldset to enable vocabularies.
  if (!empty($disabled)) {
    $form['enable_vocabs'] = array(
      '#type' => 'fieldset',
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#title' => t('Add vocabulary'),
      '#attributes' => array('class' => array('container-inline', 'taxonomy-access-add')),
    );
    $form['enable_vocabs']['enable_vocab'] = array(
      '#type' => 'select',
      '#title' => t('Vocabulary'),
      '#options' => $disabled,
    );
    $form['enable_vocabs']['add'] = array(
      '#type' => 'submit',
      '#submit' => array('taxonomy_access_enable_vocab_submit'),
      '#value' => t('Add'),
    );
  }

  // Add a fieldset for each enabled vocabulary.
  foreach ($defaults as $vid => $vocab_default) {
    if (!empty($vocabs[$vid])) {
      $vocab = $vocabs[$vid];
      $name = $vocab->machine_name;

      // Fetch unconfigured terms and reorder term records by hierarchy.
      $sort = array();
      $add_options = array();
      if ($tree = taxonomy_get_tree($vid)) {
        foreach ($tree as $term) {
          if (empty($term_grants[$vid][$term->tid])) {
            $add_options["term $term->tid"] = str_repeat('-', $term->depth) . ' ' .check_plain($term->name);
          }
          else {
            $sort[$term->tid] = $term_grants[$vid][$term->tid];
            $sort[$term->tid]['name'] =  str_repeat('-', $term->depth) . ' ' . check_plain($term->name);
          }
        }
        $term_grants[$vid] = $sort;
      }

      $grants = array(TAXONOMY_ACCESS_VOCABULARY_DEFAULT => $vocab_default);
      $grants[TAXONOMY_ACCESS_VOCABULARY_DEFAULT]['name'] = t('Default');
      if (!empty($term_grants[$vid])) {
        $grants += $term_grants[$vid];
      }
      $form[$name] = array(
        '#type' => 'fieldset',
        '#title' => $vocab->name,
        '#attributes' => array('class' => array('taxonomy-access-vocab')),
        '#description' => t('The default settings apply to all terms in %vocab that do not have their own below.', array('%vocab' => $vocab->name)),
        '#collapsible' => TRUE,
        '#collapsed' => FALSE,
      );
      // Term grant table.
      $form[$name]['grants'] =
        taxonomy_access_grant_table($grants, $vocab->vid, t('Term'), !empty($term_grants[$vid]));
      // Fieldset to add a new term if there are any.
      if (!empty($add_options)) {
        $form[$name]['new'] = array(
          '#type' => 'fieldset',
          '#collapsible' => TRUE,
          '#collapsed' => TRUE,
          '#title' => t('Add term'),
          '#tree' => TRUE,
          '#attributes' => array('class' => array('container-inline', 'taxonomy-access-add')),
        );
        $form[$name]['new'][$vid]['item'] = array(
          '#type' => 'select',
          '#title' => t('Term'),
          '#options' => $add_options,
        );
        $form[$name]['new'][$vid]['recursive'] = array(
          '#type' => 'checkbox',
          '#title' => t('with descendants'),
        );
        $form[$name]['new'][$vid]['grants'] =
          taxonomy_access_grant_add_table($vocab_default, $vid);
        $form[$name]['new'][$vid]['add'] = array(
          '#type' => 'submit',
          '#name' => $vid,
          '#submit' => array('taxonomy_access_add_term_submit'),
          '#value' => t('Add'),
        );
      }
      $disable_url = url(
        TAXONOMY_ACCESS_CONFIG . "/role/$rid/disable/$vid",
        array('query' => drupal_get_destination())
      );
      $form[$name]['disable'] = array(
          '#markup' => '<p>' . t(
            'To disable the %vocab vocabulary, <a href="@url">delete all @vocab access rules</a>.',
            array('%vocab' => $vocab->name, '@vocab' => $vocab->name, '@url' => $disable_url)) . '</p>'
      );
    }
  }
  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save all'),
    '#submit' => array('taxonomy_access_save_all_submit'),
  );
  if (!empty($term_grants)) {
    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete selected'),
      '#submit' => array('taxonomy_access_delete_selected_submit'),
    );
  }

  return $form;
}

/**
 * Generates a grant table for multiple access rules.
 *
 * @param array $rows
 *   An array of grant row data, keyed by an ID (term, vocab, role, etc.). Each
 *   row should include the following keys:
 *   - name: (optional) The label for the row (e.g., a term, vocabulary, or
 *     role name).
 *   - view: The View grant value select box for the element.
 *   - update: The Update grant value select box for the element.
 *   - delete: The Delete grant value select box for the element.
 *   - create: The Add tag grant value select box for the element.
 *   - list: The View tag grant value select box for the element.
 * @param int $parent_vid
 *   The parent ID for the table in the form tree structure (typically a
 *   vocabulary id).
 * @param string $first_col
 *   The header for the first column (in the 'name' key for each row).
 * @param bool $delete
 *   (optional) Whether to add a deletion checkbox to each row along with a
 *   "Check all" box in the table header. The checbox is automatically disabled
 *   for TAXONOMY_ACCESS_VOCABULARY_DEFAULT. Defaults to TRUE.
 *
 * @return
 *   Renderable array containing the table.
 *
 * @see taxonomy_access_grant_table()
 */
function taxonomy_access_grant_table(array $rows, $parent_vid, $first_col, $delete = TRUE) {
  $header = taxonomy_access_grant_table_header();
  if ($first_col) {
    array_unshift(
      $header,
      array('data' => $first_col, 'class' => array('taxonomy-access-label'))
    );
  }
  if ($delete) {
    drupal_add_js('misc/tableselect.js');
    array_unshift($header, array('class' => array('select-all')));
  }
  $table = array(
    '#type' => 'taxonomy_access_grant_table',
    '#tree' => TRUE,
    '#header' => $header,
  );
  foreach ($rows as $id => $row) {
    $table[$parent_vid][$id] = taxonomy_access_admin_build_row($row, 'name', $delete);
  }
  // Disable the delete checkbox for the default.
  if ($delete && isset($table[$parent_vid][TAXONOMY_ACCESS_VOCABULARY_DEFAULT])) {
    $table[$parent_vid][TAXONOMY_ACCESS_VOCABULARY_DEFAULT]['remove']['#disabled'] = TRUE;
  }

  return $table;
}

/**
 * Generates a grant table for adding access rules with one set of values.
 *
 * @param array $rows
 *   An associative array of access rule data, with the following keys:
 *   - view: The View grant value select box for the element.
 *   - update: The Update grant value select box for the element.
 *   - delete: The Delete grant value select box for the element.
 *   - create: The Add tag grant value select box for the element.
 *   - list: The View tag grant value select box for the element.
 * @param int $id
 *   The ID for this set (e.g., a vocabulary ID).
 *
 * @return
 *   Renderable array containing the table.
 *
 * @see taxonomy_access_grant_table()
 */
function taxonomy_access_grant_add_table($row, $id) {
  $header = taxonomy_access_grant_table_header();
  $table = array(
    '#type' => 'taxonomy_access_grant_table',
    '#tree' => TRUE,
    '#header' => $header,
  );
  $table[$id][TAXONOMY_ACCESS_VOCABULARY_DEFAULT] = taxonomy_access_admin_build_row($row);

  return $table;
}

/**
 * Returns a header array for grant form tables.
 *
 * @return array
 *   An array of header cell data for a grant table.
 */
function taxonomy_access_grant_table_header() {
  $header = array(
    array('data' => t('View')),
    array('data' => t('Update')),
    array('data' => t('Delete')),
    array('data' => t('Add Tag')),
    array('data' => t('View Tag')),
  );
  foreach ($header as &$cell) {
    $cell['class'] = array('taxonomy-access-grant');
  }
  return $header;
}

/**
 * Theme our grant table.
 *
 * @todo
 *   Use a separate theme function for taxonomy_access_grant_add_table() to get
 *   out of nesting hell?
 * @todo
 *   I clearly have no idea what I'm doing here.
 */
function theme_taxonomy_access_grant_table($element_data) {
  $table = array();
  $table['header'] = $element_data['elements']['#header'];
  $table['attributes']['class'] = array('taxonomy-access-grant-table');
  $rows = array();
  foreach (element_children($element_data['elements']) as $element_key) {
    $child = $element_data['elements'][$element_key];
    foreach (element_children($child) as $child_key) {
      $record = $child[$child_key];
      $row = array();
      foreach (element_children($record) as $record_key) {
        $data = array('data' => $record[$record_key]);
        // If it's the default, add styling.
        if ($record_key == 'name') {
          $data['class'] = array('taxonomy-access-label');
          if ($child_key == TAXONOMY_ACCESS_VOCABULARY_DEFAULT) {
            $data['class'][] = 'taxonomy-access-default';
          }
        }
        // Add grant classes to grant cells.
        elseif (in_array($record_key, array('view', 'update', 'delete', 'create', 'list'))) {
          $grant_class = $record_key . '-' . $data['data']['#default_value'];
          $data['class'] = array('taxonomy-access-grant', $grant_class);
        }
        $row[] = $data;
      }
      $rows[] = $row;
    }
  }
  $table['rows'] = $rows;
  return theme('table', $table);
}

/**
 * Assembles a row of grant options for a term or default on the admin form.
 *
 * @param array $grants
 *   An array of grants to use as form defaults.
 * @param $label_key
 *   (optional) Key of the column to use as a label in each grant row. Defaults
 *   to NULL.
 */
function taxonomy_access_admin_build_row(array $grants, $label_key = NULL, $delete = FALSE) {
  $form['#tree'] = TRUE;
  if ($delete) {
    $form['remove'] = array(
      '#type' => 'checkbox',
      '#title' => t('Delete access rule for @name', array('@name' => $grants[$label_key])),
      '#title_display' => 'invisible',
    );
  }
  if ($label_key) {
    $form[$label_key] = array(
      '#type' => 'markup',
      '#markup' => check_plain($grants[$label_key]),
    );
  }
  foreach (array('view', 'update', 'delete', 'create', 'list') as $grant) {
    $for = $label_key ? $grants[$label_key] : NULL;
    $form[$grant] = array(
      '#type' => 'select',
      '#title' => _taxonomy_access_grant_field_label($grant, $for),
      '#title_display' => 'invisible',
      '#default_value' => is_string($grants['grant_' . $grant]) ? $grants['grant_' . $grant] : TAXONOMY_ACCESS_NODE_IGNORE,
      '#required' => TRUE,
    );
  }
  foreach (array('view', 'update', 'delete') as $grant) {
    $form[$grant]['#options'] = array(
      TAXONOMY_ACCESS_NODE_ALLOW => t('Allow'),
      TAXONOMY_ACCESS_NODE_IGNORE => t('Ignore'),
      TAXONOMY_ACCESS_NODE_DENY => t('Deny'),
    );
  }
  foreach (array('create', 'list') as $grant) {
    $form[$grant]['#options'] = array(
      TAXONOMY_ACCESS_TERM_ALLOW => t('Allow'),
      TAXONOMY_ACCESS_TERM_DENY => t('Deny'),
    );
  }
  return $form;
}

/**
 * Returns the proper invisible field label for each grant table element.
 */
function _taxonomy_access_grant_field_label($grant, $for = NULL) {
  if ($for) {
    $label = array('@label', $for);
    $titles = array(
      'view' => t('View grant for @label', $label),
      'update' => t('Update grant for @label', $label),
      'delete' => t('Delete grant for @label', $label),
      'create' => t('Add tag grant for @label', $label),
      'list' => t('View tag grant for @label', $label),
    );
  }
  else {
    $titles = array(
      'view' => t('View grant'),
      'update' => t('Update grant'),
      'delete' => t('Delete grant'),
      'create' => t('Add tag grant'),
      'list' => t('View tag grant'),
    );
  }

 return $titles[$grant];
}

/**
 * Form submission handler for taxonomy_access_admin_role().
 *
 * Processes submissions for the vocabulary 'Add' button.
 */
function taxonomy_access_enable_vocab_submit($form, &$form_state) {
  $rid = $form_state['values']['rid'];
  $vid = $form_state['values']['enable_vocab'];
  $roles = _taxonomy_access_user_roles();
  $vocab = taxonomy_vocabulary_load($vid);
  if (taxonomy_access_enable_vocab($vid, $rid)) {
    drupal_set_message(t(
      'Vocabulary %vocab enabled successfully for the %role role.',
      array(
        '%vocab' => $vocab->name,
        '%role' => $roles[$rid])));
  }
  else {
    drupal_set_message(t('The vocabulary could not be enabled.'), 'error');
  }
}

/**
 * Form submission handler for taxonomy_access_admin_role().
 *
 * Processes submissions for the term 'Add' button.
 */
function taxonomy_access_add_term_submit($form, &$form_state) {
  $vid = $form_state['clicked_button']['#name'];
  $new = $form_state['values']['new'][$vid];
  $rid = $form_state['values']['rid'];
  list($type, $id) = explode(' ', $new['item']);
  $rows = array();

  $rows[$id] =
    _taxonomy_access_format_grant_record($id, $rid, $new['grants'][$vid][TAXONOMY_ACCESS_VOCABULARY_DEFAULT]);

  // If we are adding children recursively, add those as well.
  if ($new['recursive'] == 1) {
    $children = _taxonomy_access_get_descendants($id);
    foreach ($children as $tid) {
      $rows[$tid] =
        _taxonomy_access_format_grant_record($tid, $rid, $new['grants'][$vid][TAXONOMY_ACCESS_VOCABULARY_DEFAULT]);
    }
  }

  // Set the grants for the row or rows.
  taxonomy_access_set_term_grants($rows);
}

/**
 * Page callback: Returns a confirmation form to disable a vocabulary.
 *
 * @param int $rid
 *   The role ID.
 * @param object $vocab
 *   The vocabulary object.
 *
 * @todo
 *   Check if vocab is enabled and return a 404 otherwise?
 *
 * @see taxonomy_access_menu()
 * @see taxonomy_access_admin_role().
 * @see taxonomy_access_disable_vocab_confirm_page()
 */
function taxonomy_access_disable_vocab_confirm_page($rid, $vocab) {
  $rid = intval($rid);

  // Return a 404 on invalid vid or rid.
  if (!$vocab->vid || !$rid) {
    return MENU_NOT_FOUND;
  }

  return drupal_get_form('taxonomy_access_disable_vocab_confirm', $rid, $vocab);
}

/**
 * Returns a confirmation form for disabling a vocabulary for a role.
 *
 * @param int $rid
 *   The role ID.
 * @param object $vocab
 *   The vocabulary object.
 *
 * @see taxonomy_access_disable_vocab_confirm_page()
 * @see taxonomy_access_disable_vocab_confirm_submit()
 * @ingroup forms
 */
function taxonomy_access_disable_vocab_confirm($form, &$form_state, $rid, $vocab) {
  $roles = _taxonomy_access_user_roles();
  if (taxonomy_access_role_enabled($rid)) {
    $form['rid'] = array(
      '#type' => 'value',
      '#value' => $rid,
    );
    $form['vid'] = array(
      '#type' => 'value',
      '#value' => $vocab->vid,
    );
    $form['vocab_name'] = array(
      '#type' => 'value',
      '#value' => $vocab->name,
    );
    return confirm_form($form,
      t("Are you sure you want to delete all Taxonomy access rules for %vocab in the %role role?",
        array('%role' => $roles[$rid], '%vocab' => $vocab->name)),
      TAXONOMY_ACCESS_CONFIG . '/role/%/edit',
      t('This action cannot be undone.'),
      t('Delete all'),
      t('Cancel'));
  }
}

/**
 * Form submission handler for taxonomy_access_disable_vocab_confirm().
 *
 * @param int $rid
 *   The role ID to disable.
 *
 * @todo
 *   Set a message on invalid $rid or $vid?
 */
function taxonomy_access_disable_vocab_confirm_submit($form, &$form_state) {
  $roles = _taxonomy_access_user_roles();
  $rid = intval($form_state['values']['rid']);
  $vid = intval($form_state['values']['vid']);
  // Do not proceed for invalid role IDs, and do not allow the global default
  // to be deleted.
  if (!$vid || !$rid || empty($roles[$rid])) {
    return FALSE;
  }

  if ($form_state['values']['confirm']) {
    $form_state['redirect'] = TAXONOMY_ACCESS_CONFIG;
    if (taxonomy_access_disable_vocab($vid, $rid)) {
      drupal_set_message(
        t('All Taxonomy access rules deleted for %vocab in role %role.',
          array(
            '%vocab' => $form_state['values']['vocab_name'],
            '%role' => $roles[$rid])
         ));
      return TRUE;
    }
  }
}

/**
 * Form submission handler for taxonomy_access_admin_role().
 *
 * Processes submissions for the "Delete selected" button.
 *
 * @todo
 *   The parent form could probably be refactored to make this more efficient
 *   (by putting these elements in a flat list) but that would require changing
 *   taxonomy_access_grant_table() and taxonomy_access_build_row().
 */
function taxonomy_access_delete_selected_submit($form, &$form_state) {
  $rid = intval($form_state['values']['rid']);
  $delete_tids = array();
  foreach ($form_state['values']['grants'] as $vid => $tids) {
    foreach ($tids as $tid => $record) {
      if (!empty($record['remove'])) {
        $delete_tids[] = $tid;
      }
    }
  }
  if ($rid) {
    if (taxonomy_access_delete_term_grants($delete_tids, $rid)) {
      drupal_set_message(format_plural(
          sizeof($delete_tids),
          '1 term access rule was deleted.',
          '@count term access rules were deleted.'));
    }
    else {
      drupal_set_message(t('The records could not be deleted.'), 'warning');
    }
  }
}
/**
 * Form submission handler for taxonomy_access_admin_form().
 *
 * Processes submissions for the 'Save all' button.
 */
function taxonomy_access_save_all_submit($form, &$form_state) {
  $values = $form_state['values'];
  $rid = $values['rid'];
  $vocabs = taxonomy_get_vocabularies();

  // Create four lists of records to update.
  $update_terms = array();
  $skip_terms = array();
  $update_defaults = array();
  $skip_defaults = array();

  foreach ($values['grants'] as $vid => $rows) {
    if ($vid == TAXONOMY_ACCESS_GLOBAL_DEFAULT) {
      $element = $form['global_default'];
    }
    else {
      $vocab = $vocabs[$vid];
      $element = $form[$vocab->machine_name];
    }
    foreach ($rows as $tid => $row) {
      // Check the default values for this row.
      $defaults = array();
      $grants = array();
      foreach (array('view', 'update', 'delete', 'create', 'list') as $grant_name) {
        $grants[$grant_name] = $row[$grant_name];
        $defaults[$grant_name] =
          $element['grants'][$vid][$tid][$grant_name]['#default_value'];
      }

      // Proceed if the user changed the row (values differ from defaults).
      if ($defaults != $grants) {
        // If the grants for node access match the defaults, then we
        // can skip updating node access records for this row.
        $update_nodes = FALSE;
        foreach (array('view', 'update', 'delete') as $op) {
          if ($defaults[$op] != $grants[$op]) {
            $update_nodes = TRUE;
          }
        }

        // Add the row to one of the four arrays.
        switch (TRUE) {
          // Term record with node grant changes.
          case ($tid && $update_nodes):
            $update_terms[$tid] =
              _taxonomy_access_format_grant_record($tid, $rid, $grants);
            break;

          // Term record and no node grant changes.
          case ($tid && !$update_nodes):
            $skip_terms[$tid] =
              _taxonomy_access_format_grant_record($tid, $rid, $grants);
            break;

          // Vocab record with node grant changes.
          case (!$tid && $update_nodes):
            $update_defaults[$vid] =
              _taxonomy_access_format_grant_record($vid, $rid, $grants, TRUE);
            break;

          // Vocab record and no node grant changes.
          case (!$tid && !$update_nodes):
            $skip_defaults[$vid] =
              _taxonomy_access_format_grant_record($vid, $rid, $grants, TRUE);
            break;
        }
      }
    }
  }

  // Process each set.
  if (!empty($update_terms)) {
    taxonomy_access_set_term_grants($update_terms);
  }
  if (!empty($skip_terms)) {
    taxonomy_access_set_term_grants($skip_terms, FALSE);
  }
  if (!empty($update_defaults)) {
    taxonomy_access_set_default_grants($update_defaults);
  }
  if (!empty($skip_defaults)) {
    taxonomy_access_set_default_grants($skip_defaults, FALSE);
  }
}


/**
 * Generates HTML markup with form instructions for the admin form.
 *
 * @return
 *   Instructions HTML string.
 */
function _taxonomy_access_admin_instructions_html() {
  $instructions = '';
  $instructions .= ''
    . "<br /><br />"
    . "<div class=\"instructions\">"
    . "<h2>" . t("Explanation of fields") . "</h2>"
    . _taxonomy_access_grant_help_table()
    . "<p>"
    . t('Options for View, Update, and Delete are <em>Allow</em> (<acronym title="Allow">A</acronym>), <em>Ignore</em> (<acronym title="Ignore">I</acronym>), and <em>Deny</em> (<acronym title="Deny">D</acronym>).')
    . "</p>\n\n"
    . "<ul>\n"
    . "<li>"
    . t('<em>Deny</em> (<acronym title="Deny">D</acronym>) overrides <em>Allow</em> (<acronym title="Allow">A</acronym>) within this role.')
    . "</li>"
    . "<li>"
    . t('Both <em>Allow</em> (<acronym title="Allow">A</acronym>) and <em>Deny</em> (<acronym title="Deny">D</acronym>) override <em>Ignore</em> (<acronym title="Ignore">I</acronym>) within this role.')
    . "</li>"
    . "<li>"
    . t('If a user has <strong>multiple roles</strong>, an <em>Allow</em> (<acronym title="Allow">A</acronym>) from another role <strong>will</strong> override a <em>Deny</em> (<acronym title="Deny">D</acronym>) here.')
    . "</li>"
    . "</ul>\n\n"
    ;
  if (arg(4) != DRUPAL_ANONYMOUS_RID && arg(4) != DRUPAL_AUTHENTICATED_RID) {
    // Role other than Anonymous or Authenticated
    $instructions .= ''
      . "<p>"
      . t('<strong>Remember:</strong> This role <strong>will</strong> inherit permissions from the <em>authenticated user</em> role.  Be sure to <a href="@url">configure the authenticated user</a> properly.',
        array("@url" => url(
            TAXONOMY_ACCESS_CONFIG
            . "/role/"
            .  DRUPAL_AUTHENTICATED_RID
            . '/edit')))
      . "</p>\n\n"
      ;
  }
  $instructions .= ''
    . "<p>"
    . t('For more information and for troubleshooting guidelines, see the <a href="@help">help page</a> and the !readme.',
      array(
        '@help' => url('admin/help/taxonomy_access'),
        '!readme' => "<code>README.txt</code>"
      ))
    . "</p>\n\n"
    . "</div>\n\n"
    ;

  return $instructions;

}
