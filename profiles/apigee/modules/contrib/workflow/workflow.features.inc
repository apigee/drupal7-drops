<?php

/**
 * @file
 * Intergrates workflow with features.
 */

define('WORKFLOW_FEATURES_AUTHOR_NAME', 'workflow_features_author_name');

/**
 * Workflows are a **faux-exportable** component.
 */

/**
 * Implements hook_features_export().
 */
function workflow_features_export($data, &$export, $module_name = '') {
  // fontyourface_default_fonts integration is provided by Features.
  $export['dependencies']['features'] = 'features';
  $export['dependencies']['workflow'] = 'workflow';
  foreach (workflow_get_workflows() as $workflow) {
    if (in_array($workflow->name, $data)) {
      $export['features']['workflow'][$workflow->name] = $workflow->name;
    }
  }
  return $export;
}

/**
 * Implements hook_features_export_render().
 */
function workflow_features_export_render($module, $data) {
  $translatables = $code = array();
  $code[] = '  $workflows = array();';
  $code[] = '';

  $workflows = workflow_get_workflows();
  foreach ($data as $name) {
    if ($workflow = workflow_get_workflows_full_object($name)) {
      unset($workflow->wid);
      $workflow_export = features_var_export($workflow, '  ');
      $workflow_identifier = features_var_export($workflow->name);
      $code[] = "  // Exported workflow: $name";
      $code[] = "  \$workflows[{$workflow_identifier}] = {$workflow_export};";
      $code[] = "";
    }
  }

  $code[] = '  return $workflows;';
  $code = implode("\n", $code);
  return array('workflow_default_workflows' => $code);
}

/**
 * Implements hook_features_export_options().
 */
function workflow_features_export_options() {
  $workflows = array();
  foreach (workflow_get_workflows() as $workflow) {
    $workflows[$workflow->name] = $workflow->name;
  }
  return $workflows;
}

/**
 * Implements hook_features_revert().
 */
function workflow_features_revert($module) {
  // Including the features inc to make sure this func is available during install of a Features module.
  module_load_include('inc', 'features', 'features.export');
  foreach (features_get_default('workflow', $module) as $key => $workflow) {
    workflow_update_workflows_full_object($workflow);
  }
}

/**
 * Implements hook_features_export_rebuild().
 */
function workflow_features_export_rebuild($module) {
  workflow_features_revert($module);
}

/**
 * CRUD style functions below.
 */

/**
 * For use by CRUD only, save everything from the CRUD formed object.
 *
 * @see workflow_get_workflows_full_object
 *
 * @param $workflow
 *   A fully loaded workflow object to save the states of.
 *
 * @return
 *   Returns whether the workflow was saved fully.
 */
function workflow_update_workflows_full_object($workflow) {
  $workflow = (object) $workflow;
  // Given a workflow in the format returned from export.
  // First we grab the states, transitions and node_maps out.
  $states = isset($workflow->states) ? $workflow->states : array();
  $transitions = isset($workflow->transitions) ? $workflow->transitions : array();
  $node_types = isset($workflow->node_types) ? $workflow->node_types : array();
  unset($workflow->states, $workflow->transitions, $workflow->node_types);

  // Then make a workflow so we can track by wid.
  if ($orig_workflow = workflow_get_workflows_by_name($workflow->name)) {
    $workflow->wid = $orig_workflow->wid;
  }

  workflow_update_workflows($workflow, FALSE);

  // Cancel out if workflow failed to save.
  if (!isset($workflow->wid) || empty($workflow->wid)) {
    return FALSE;
  }

  // Workflow is now a fully vetted workflow object. We have NOT created a creation state with this.
  // Then make states, marking state name to state sid.
  $active_states = array();
  foreach ($states as $state) {
    $state = (object) $state;
    $state->wid = $workflow->wid;
    if ($orig_state = reset(workflow_get_workflow_states_by_wid_state($state->wid, $state->state))) {
      $state->sid = $orig_state->sid;
    }
    workflow_update_workflow_states($state);
    $active_states[$state->state] = $state->sid;
  }

  // Delete any states *not* in our original construction.
  foreach (workflow_get_workflow_states_by_wid($workflow->wid) as $state) {
    if (!in_array($state->sid, $active_states)) {
      workflow_delete_workflow_states_by_sid($state->sid);
    }
  }

  // Then make transitions with the state mapping.
  $active_transitions = array();
  foreach ($transitions as $transition) {
    $transition = (object) $transition;
    $transition->sid = $active_states[$transition->state];
    $transition->target_sid = $active_states[$transition->target_state];
    // Roles are exported by rolename, so need to translate to RID.
    $transition->roles = !empty($transition->roles) ? _workflow_roles_to_rids($transition->roles) : '';
    workflow_update_workflow_transitions($transition);
    $active_transitions[] = $transition->tid;
  }

  // Delete any transitions in our workflow that are *not* in our original construction.
  foreach (workflow_get_workflow_transitions_by_wid($workflow->wid) as $transition) {
    if (!in_array($transition->tid, $active_transitions)) {
      workflow_delete_workflow_transitions_by_tid($transition->tid);
    }
  }
  // Then add the node_type mapping.
  foreach ($node_types as $node_type) {
    $node_type = (object) array(
      'type' => $node_type,
      'wid' => $workflow->wid
    );
    // Insert, nodes only have one workflow. Insert will delete any prior workflow assoc.
    workflow_insert_workflow_type_map($node_type);
  }

  return TRUE;
}

/**
 * For use by CRUD only, gather everything into the CRUD formed object.
 *
 * @param $name
 *   A string corresponding to a workflow object.
 *
 * @return
 *   A fully loaded workflow object with type and statue mappings.
 */
function workflow_get_workflows_full_object($name) {
  if ($workflow = workflow_get_workflows_by_name($name)) {
    // Now we need to add data to the object for each state, an array of sub-objects.
    $options = array('status' => 1); // We only want active states for this export.
    $active_states = array();
    foreach (workflow_get_workflow_states_by_wid($workflow->wid, $options) as $index => $state) {
      $active_states[$state->sid] = $state->state;
      // Remove what we don't need to export.
      unset($state->sid);
      unset($state->wid);
      $workflow->states[] = $state;
    }

    // Now we need to add data to the export for each transition, an array of sub-objects.
    // Same goes for transitions, see above re: states.
    foreach ($active_states as $sid => $state) {
      // We're going to look everythign up by the start state, not state involved, to avoid dupes.
      foreach (workflow_get_workflow_transitions_by_sid($sid, $options) as $transition) {
        // And to get the target state (by name) we need to look it up too.
        $target_state = workflow_get_workflow_states_by_sid($transition->target_sid);
        $transition->state = $state;
        $transition->target_state = $target_state->state;
        unset($transition->sid, $transition->target_sid);
        // Translate to role names so works cross install.
        $transition->roles = !empty($transition->roles) ? _workflow_rids_to_roles($transition->roles) : '';
        // Remove what we don't need to export.
        unset($transition->tid);
        $workflow->transitions[] = $transition;
      }
    }

    // Now we need to add data to the export for each type map, an array of sub-objects.
    // Same goes for node mappings, see above re: states.
    foreach (workflow_get_workflow_type_map_by_wid($workflow->wid) as $index => $type_map) {
      $workflow->node_types[] = $type_map->type;
    }
  }
  return $workflow;
}

/**
 * Internally cache the user roles as core doesn't.
 */
function _workflow_user_roles($reset = FALSE) {
  $roles = &drupal_static(__FUNCTION__);
  if ($reset || !isset($roles)) {
    $roles = user_roles();
  }
  return $roles;
}

/**
 * Translates a role string to RIDs for importing.
 *
 * @param $role_string
 *   A string of roles or fake 'author' role.
 *
 * @return
 *   A string of RIDs seperated by commas.
 */
function _workflow_roles_to_rids($role_string) {
  $roles = _workflow_user_roles();
  $rid_array = array();
  foreach (explode(',', $role_string) as $role_name) {
    if ($role_name === WORKFLOW_FEATURES_AUTHOR_NAME) {
      $rid_array[] = 'author';
    }
    elseif ($role_name && in_array($role_name, $roles)) {
      $rid_array[] = array_search($role_name, $roles);
    }
  }
  return implode(',', $rid_array);
}

/**
 * Translates a string of rids to role names for exporting.
 *
 * @param $rid_string
 *   A string of rids or fake 'author' role.
 *
 * @return
 *   A string of role names seperated by commas.
 */
function _workflow_rids_to_roles($rid_string) {
  $roles = _workflow_user_roles();
  $rid_array = explode(',', $rid_string);
  // There may be a role named 'author', so make 'author' distinct.
  $return = in_array('author', $rid_array) ? WORKFLOW_FEATURES_AUTHOR_NAME . ',' : '';
  // Translate RIDs to rolenames.
  $return .= implode(',', array_intersect_key($roles, array_flip($rid_array)));
  return trim($return, ',');
}
