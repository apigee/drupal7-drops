<?php

/**
 * @file
 * Admin page callback file for the permissions grid pages.
 */

/**
 * Menu callback: role list.
 */
function permissions_grid_admin_roles() {
  $header = array(t('Name'), array('data' => t('Operations'), 'colspan' => 2));

  foreach (user_roles() as $rid => $name) {
    $edit_permissions = l(t('edit permissions grid'), 'admin/people/permissions/grid/' . $rid);
    switch ($rid) {
      case DRUPAL_ANONYMOUS_RID:
      case DRUPAL_AUTHENTICATED_RID:
        $rows[] = array(
          $name,
          t('locked'),
          $edit_permissions,
        );
        break;
      default:
        $rows[] = array(
          $name,
          l(t('edit role'), 'admin/people/permissions/roles/edit/' . $rid),
          $edit_permissions,
        );
    }
  }

  $output['roles_list'] = array(
    '#markup' => theme('table', array('header' => $header, 'rows' => $rows)),
  );

  return $output;
}

/**
 * Form builder: administer permissions.
 *
 * @see theme_permissions_grid_admin_node_permissions_form()
 *
 * @param $role
 *  A fully-loaded role object from user_role_load().
 *  @todo: implement monster page of all roles when this is NULL.
 */
function permissions_grid_admin_node_permissions_form($form, &$form_state, $role = NULL) {
  // We have to load this explicitly because it won't match any module.
  module_load_include('inc', 'permission_grid', 'permission_grid.permissions');

  $form['header'] = array(
    '#markup' => '<h2>Permissions grid for ' . check_plain($role->name) . '</h2>',
  );
  $form['#role'] = $role;

  // Invoke hook_permission_grid_info().
  $permission_info = module_invoke_all('permission_grid_info');
  drupal_alter('permission_grid_info', $permission_info);
  //dsm($permission_info, '$permission_info');

  // Fetch permissions for the one selected role.
  // Gets an array keyed by rid, with an array of permission => TRUE
  $role_permissions = user_role_permissions(array($role->rid => $role->rid));
  $role_permissions = $role_permissions[$role->rid];
  //dsm($role_permissions, 'perms');

  // Work through each type, eg node, filter, product, etc.
  $checkboxes_options = array();
  foreach ($permission_info as $object_type => $object_permission_info) {
    $form['permission'][$object_type]['header'] = array(
      '#markup' => "<h3>" . $object_permission_info['label'] . "</h3>",
    );

    // Work through each object, eg node type, product type, etc.
    $checkboxes_options = array();
    foreach ($object_permission_info['objects'] as $object => $object_label) {
      // First column is the label.
      $form['permission'][$object_type]['objects'][$object]['label'] = array(
        '#markup' => check_plain($object_label),
        //'type'   => $type, // store the machine name too?
      );

      // Work through each verb, combining to make a permission.
      $verbs = array();
      foreach ($object_permission_info['verb_groups'] as $verb_group) {
        // Collate verbs for the theme.
        $verbs += $verb_group['verbs'];

        foreach ($verb_group['verbs'] as $verb => $verb_label) {
          // Special handling in case object labels need translating.
          if (isset($verb_group['object_process_callback'])) {
            $function = $verb_group['object_process_callback'];
            $object_replace = $function($object);
          }
          else {
            $object_replace = $object;
          }

          $permission_string = str_replace(
            array('%verb', '%object'),
            array($verb,   $object_replace),
            $verb_group['pattern']
          );

          $form['permission'][$object_type]['objects'][$object]['perms'][$verb]['#permission'] = $permission_string;
          // TODO LABEL
          $checkboxes_options[$permission_string] = $permission_string;

          // Create a checkbox for the permission.
          $form['permission'][$object_type]['checkboxes'][$permission_string] = array(
            '#type' => 'checkbox',
            '#default_value' => !empty($role_permissions[$permission_string]),
            // Set this manually because of http://drupal.org/node/1130946.
            '#parents' => array('permission', $permission_string),
          );

        }
      }

      $form['permission'][$object_type]['#verbs'] = $verbs;
    }
  }
  //dsm($checkboxes_options, 'options');
  //dsm($form, 'form');

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array('#type' => 'submit', '#value' => t('Save permissions'));

  return $form;
}

/**
 * Theme the permissions grid form.
 */
function theme_permissions_grid_admin_node_permissions_form($variables) {
  $form = $variables['form'];
  $output = '';

  $output .= drupal_render($form['header']);

  foreach (element_children($form['permission']) as $object_type) {
    // The heading for each table.
    $output .= drupal_render($form['permission'][$object_type]['header']);

    // Add an initial empty header cell for the header column.
    $header = array_merge(
      array(''),
      $form['permission'][$object_type]['#verbs']
    );
    $rows = array();

    foreach (element_children($form['permission'][$object_type]['objects']) as $object) {
      // One row for each object.
      $row = array();
      $row[] = array(
        'data' => drupal_render($form['permission'][$object_type]['objects'][$object]['label']),
        'header' => TRUE,
      );
      // One column for each verb.
      foreach (element_children($form['permission'][$object_type]['objects'][$object]['perms']) as $verb) {
        $permission_string = $form['permission'][$object_type]['objects'][$object]['perms'][$verb]['#permission'];

        // Create labels with element-invisible for accessibility.
        //$form['permission'][$object_type]['checkboxes'][$permission_string]['#title'] = $name . ': ' . $form['permission'][$object_type]['checkboxes'][$permission_string];//. t($key);
        $form['permission'][$object_type]['checkboxes'][$permission_string]['#title_display'] = 'invisible';

        $row[] = drupal_render($form['permission'][$object_type]['checkboxes'][$permission_string]);
      }
      $rows[] = $row;
    }
    //dsm($rows);

    $output .= theme('table', array(
      'rows'    => $rows,
      'header'  => $header,
      //'caption' => check_plain($name),
      'attributes' => array('id' => 'permissions'),
      'sticky' => TRUE,
    ));
  }

  $output .= drupal_render_children($form);

  return $output;
}

/**
 * Form submission for the permissions grid form.
 */
function permissions_grid_admin_node_permissions_form_submit($form, &$form_state) {
  $role = $form['#role'];
  user_role_change_permissions($role->rid, $form_state['values']['permission']);
}
