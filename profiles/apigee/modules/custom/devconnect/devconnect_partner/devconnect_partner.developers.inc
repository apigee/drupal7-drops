<?php

function devconnect_partner_developers_list_form($form, &$form_state, $user) {
  $partner = _devconnect_partner_get_partner_entity($user->uid);
  if (!$partner) {
    drupal_access_denied();
  }

  $query = db_select('dc_dev', 'd');
  $query->innerJoin('dc_dev_attributes', 'ap', 'd.developer_id = ap.developer_id AND ap.name=:name', array(':name' => 'partner_id'));
  $query->innerJoin('dc_dev', 'dp', 'ap.value = dp.developer_id');

  $query->condition('dp.uid', $partner->uid);
  $query->fields('d', array('uid', 'developer_id'));
  $result = $query->execute();

  $developer_uids = array();
  foreach ($result as $row) {
    $developer_uids[] = $row->uid;
  }

  $developer_perms = array();
  if (!empty($developer_uids)) {
    $query = db_select('dc_dev_attributes', 'da');
    $query->innerJoin('dc_dev', 'd', 'da.developer_id = d.developer_id');
    $result = $query->condition('d.uid', $developer_uids)
      ->condition('name', 'partner_perms')
      ->fields('da', array('value'))
      ->fields('d', array('uid'))
      ->execute();
    foreach ($result as $row) {
      $developer_perms[$row->uid] = explode(',', $row->value);
    }
  }

  $users = entity_load('user', $developer_uids);
  usort($users, '_devconnect_partner_sort_developers');
  $form['developers'] = array('#tree' => TRUE);
  foreach ($users as $user) {
    $perms = (array_key_exists($user->uid, $developer_perms) ? $developer_perms[$user->uid] : array());
    $developer = array(
      'delete' => array(
        '#type' => 'checkbox',
        '#return_value' => 1,
        '#title_display' => 'invisible',
        '#default_value' => 0,
        '#disabled' => ($user->uid == $partner->uid || $user->uid == $GLOBALS['user']->uid)
      )
    );
    foreach (array('admin', 'view', 'create', 'edit', 'remove') as $perm) {
      $developer[$perm] = array(
        '#type' => 'checkbox',
        '#return_value' => 1,
        '#title_display' => 'invisible',
        '#default_value' => ($user->uid == $partner->uid || in_array($perm, $perms)) ? 1 : 0,
        '#disabled' => ($user->uid == $partner->uid)
      );
    }
    $developer['uid'] = array('#type' => 'value', '#value' => $user->uid);
    $developer['real_name'] = array('#type' => 'value', '#value' => $user->field_last_name[LANGUAGE_NONE][0]['value'] . ', ' . $user->field_first_name[LANGUAGE_NONE][0]['value']);
    $developer['prev_perms'] = array('#type' => 'value', '#value' => $perms);
    $developer['mail'] = array('#type' => 'value', '#value' => $user->mail);

    $form['developers'][$user->name] = $developer;
  }

  $form['actions'] = array(
    '#type' => 'actions',
    'submit' => array(
      '#type' => 'submit',
      '#value' => t('Save Changes')
    )
  );

  return $form;
}

function devconnect_partner_developers_list_form_submit($form, &$form_state) {
  $affected_users = 0;

  foreach ($form_state['values']['developers'] as $name => $details) {
    $uid = $details['uid'];
    if ($details['delete']) {
      user_delete($uid);
      $affected_users++;
    }
    else {
      // determine if we need to perform any update
      $prev_perms = $details['prev_perms'];
      $new_perms = array();
      $perform_update = FALSE;
      foreach (array('admin', 'view', 'create', 'edit', 'remove') as $perm) {
        if ($details[$perm]) {
          $new_perms[] = $perm;
        }
        if (!$perform_update && (($details[$perm] && !in_array($perm, $prev_perms)) || (!$details[$perm] && in_array($perm, $prev_perms)))) {
          $perform_update = TRUE;
        }
      }
      if ($perform_update) {
        $dev = entity_load_single('developer', $details['mail']);
        if ($dev) {
          $dev->attributes['partner_perms'] = join(',', $new_perms);
          entity_save('developer', $dev);
          $affected_users++;
        }
      }
    }
  }
  if (!$affected_users) {
    drupal_set_message(t('No developer changes were saved.'));
  }
  else {
    drupal_set_message(t(':count developer record(s) were updated.', array(':count' => $affected_users)));
  }
}

function _devconnect_partner_sort_developers($a, $b) {
  $aln = strtolower($a->field_last_name[LANGUAGE_NONE][0]['value']);
  $bln = strtolower($b->field_last_name[LANGUAGE_NONE][0]['value']);
  if ($aln == $bln) {
    $afn = strtolower($a->field_first_name[LANGUAGE_NONE][0]['value']);
    $bfn = strtolower($b->field_first_name[LANGUAGE_NONE][0]['value']);
    if ($afn == $bfn) {
      if ($a->name == $b->name) {
        return 0;
      }
      return ($a->name < $b->name) ? -1 : 1;
    }
    return ($afn < $bfn) ? -1 : 1;
  }
  return ($aln < $bln) ? -1 : 1;
}


function theme_devconnect_partner_developers_list_form($variables) {
  $form = $variables['form'];

  $header = array(
    t('Delete'),
    t('Username'),
    t('Real Name'),
    t('Administer Developers'),
    t('View Apps'),
    t('Create Apps'),
    t('Edit Apps'),
    t('Delete Apps')
  );
  $rows = array();
  foreach ($form['developers'] as $username => &$developer) {
    if (substr($username, 0, 1) == '#') {
      continue;
    }
    $rows[] = array(
      drupal_render($developer['delete']),
      l($username, 'user/' . $developer['uid']['#value']),
      $developer['real_name']['#value'],
      drupal_render($developer['admin']),
      drupal_render($developer['view']),
      drupal_render($developer['create']),
      drupal_render($developer['edit']),
      drupal_render($developer['remove'])
    );
  }

  if (!empty($rows)) {
    $output = theme('table', array('header' => $header, 'rows' => $rows, 'sticky' => TRUE));

    if (preg_match('!<thead>(.*)</thead>!Ums', $output, $matches)) {
      if (preg_match_all('!<th>(.*)</th>!Ums', $matches[1], $ths, PREG_SET_ORDER)) {
        $thead = "<thead>\n<tr>\n";
        foreach ($ths as $i => $th) {
          if ($i < 3) {
            $thead .= '<th rowspan="2">' . $th[1] . '</th>';
          }
          elseif ($i == 3) {
            $thead .= '<th colspan="' . (count($ths) - 3) . '">Permissions</th></tr><tr>';
          }
          if ($i >= 3) {
            $thead .= '<th>' . $th[1] . '</th>';
          }
          $thead .= "\n";
        }
        $thead .= "</thead>";

        $output = str_replace($matches[0], $thead, $output);
      }
    }
  }
  else {
    hide($form['actions']);
    $output = '<p>' . t('There are currently no developers.') . '</p>';
  }
  $output .= drupal_render_children($form);

  $output .= l(t('Add a Developer'), 'user/me/partner/developers/add', array('attributes' => array('class' => 'btn')));
  return $output;
}

function devconnect_partner_add_developer_form($form, &$form_state, $user) {
  $partner = _devconnect_partner_get_partner_entity($user->uid);
  if (!$partner) {
    drupal_access_denied();
  }
  $form['partner_id'] = array(
    '#type' => 'value',
    '#value' => $partner->developerId
  );

  $form['mail'] = array(
    '#type' => 'textfield',
    '#required' => TRUE,
    '#maxlength' => 254,
    '#title' => t('Email address')
  );
  $form['name'] = array(
    '#type' => 'textfield',
    '#required' => TRUE,
    '#maxlength' => 60,
    '#title' => t('User Name')
  );
  $form['first_name'] = array(
    '#type' => 'textfield',
    '#required' => TRUE,
    '#maxlength' => 255,
    '#title' => t('First Name')
  );
  $form['last_name'] = array(
    '#type' => 'textfield',
    '#required' => TRUE,
    '#maxlength' => 255,
    '#title' => t('Last Name')
  );

  $options = array(
    'admin' => t('Administer Developers'),
    'view' => t('View Partner App Details'),
    'create' => t('Create Partner Apps'),
    'edit' => t('Edit Partner Apps'),
    'remove' => t('Delete Partner Apps')
  );

  $form['permissions'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Permissions assigned to this developer'),
    '#options' => $options,
    '#default_value' => array('view')
  );

  $form['actions'] = array(
    '#type' => 'actions',
    'submit' => array(
      '#type' => 'submit',
      '#value' => t('Create')
    )
  );
  return $form;
}

function devconnect_partner_add_developer_form_validate($form, &$form_state) {
  $or = db_or();
  $or->condition('mail', $form_state['values']['mail']);
  $or->condition('name', $form_state['values']['name']);
  $row = db_select('users', 'u')
    ->fields('u', array('mail', 'name'))
    ->condition($or)
    ->execute()
    ->fetchAssoc();
  if ($row) {
    if (strtolower($row['mail']) == strtolower($form_state['values']['mail'])) {
      form_error($form['mail'], t('A user with the email address @mail already exists.', array('@mail' => $row['mail'])));
    }
    else {
      form_error($form['name'], t('A user with the user name @name already exists.', array('@name' => $row['name'])));
    }
  }
}

function devconnect_partner_add_developer_form_submit($form, &$form_state) {
  $permissions = array();
  foreach ($form_state['values']['permissions'] as $permission) {
    if (!empty($permission)) {
      $permissions[] = $permission;
    }
  }

  // TODO: handle any other required fields
  $values = array(
    'name' => $form_state['values']['name'],
    'mail' => $form_state['values']['mail'],
    'field_first_name' => array(
      LANGUAGE_NONE => array(
        0 => array(
          'value' => $form_state['values']['first_name']
        )
      )
    ),
    'field_last_name' => array(
      LANGUAGE_NONE => array(
        0 => array(
          'value' => $form_state['values']['last_name']
        )
      )
    ),
    'pass' => user_password(),
    'attribute_partner_id' => $form_state['values']['partner_id'],
    'attribute_partner_perms' => join(',', $permissions),
    'status' => 1
  );

  $user = new stdClass;
  $user = user_save($user, $values);
  if ($user) {
    drupal_set_message(t('User @name has been created.', array('@name' => $user->name)));
  }
  else {
    drupal_set_message(t('User @name failed to save.', array('@name' => $form_state['values']['name'])));
  }
  $form_state['redirect'] = 'user/me/partner/developers';
}