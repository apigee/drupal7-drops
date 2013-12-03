<?php

/**
 * @file
 * Integrates workflow with features.
 */

define('WORKFLOW_FEATURES_AUTHOR_NAME', 'workflow_features_author_name');

// @todo: This include has not been fully tested with Workflow Field and its classes.
// Even if workflow Node is not enabled, Features may use Node API's type_maps.
module_load_include('inc', 'workflow', 'workflow.node.type_map');

/**
 * Workflows are a **faux-exportable** component.
 */

/**
 * Implements hook_features_export_options().
 *
 * Generates the options to choose from.
 * Using $wid as key. Result is used by hook_featues_export().
 */
function workflow_features_export_options() {
  return workflow_get_workflow_names();
}

/**
 * Implements hook_features_export().
 */
function workflow_features_export($data, &$export, $module_name = '') {
  // fontyourface_default_fonts integration is provided by Features.
  $export['dependencies']['features'] = 'features';
  $export['dependencies']['workflow'] = 'workflow';
  foreach ($data as $wid) {
    if ($workflow = Workflow::load($wid)) {
      $name = $workflow->getName(); 
      $export['features']['workflow'][$name] = $name;
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
 * Implements hook_features_revert().
 */
function workflow_features_revert($module) {
  // Including the features inc to make sure this function is available during install of a Features module.
  module_load_include('inc', 'features', 'features.export');

  // The return value might not be an array, E.g., whilst workflow itself is being reverted.
  $default_workflows = features_get_default('workflow', $module);
  if (!is_array($default_workflows)) {
    $default_workflows = array();
  }

  foreach ($default_workflows as $key => $workflow) {
    workflow_update_workflows_full_object($workflow);
  }

  // "NOTE: This return value is no longer used in the latest Features,
  // "so modules should no longer count on this value.
  return TRUE;
}

/**
 * Implements hook_features_rebuild().
 */
function workflow_features_rebuild($module) {
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
function workflow_update_workflows_full_object($workflow_data) {
  $workflow_data = (object) $workflow_data;

  // First we grab the states, transitions and node_maps out.
  $new_states = isset($workflow_data->states) ? $workflow_data->states : array();
  $new_transitions = isset($workflow_data->transitions) ? $workflow_data->transitions : array();
  $new_node_types = isset($workflow->node_types) ? $workflow->node_types : array();
  $name = $workflow_data->name;

  // Then make a workflow so we can track by wid.
  if (!$workflow = Workflow::loadByName($name)) {
    $workflow = Workflow::create($name);
  }
  unset($workflow->tab_roles);
  // Roles are exported by rolename, so need to translate to RID.
  $workflow->tab_roles = !empty($workflow_data->tab_roles) ? _workflow_roles_to_rids($workflow_data->tab_roles) : '';
  unset($workflow->options);
  $workflow->options = $workflow_data->options;

  $workflow->save(FALSE);
  // Wid may by known only after save().

  // Cancel out if workflow failed to save.
  if (!isset($workflow->wid) || empty($workflow->wid)) {
    return FALSE;
  }

  // Workflow is now a fully vetted workflow object. We have NOT created a creation state with this.
  // Then make states, marking state name to state sid.
  $orig_states = $workflow->getStates(); // Initialize the cache for this workflow.
  $active_states = array();
  foreach ($new_states as $state_data) {
    $name = $state_data['name'];
    if (!$state = WorkflowState::loadByName($name, $workflow->wid)) {
      $state = WorkflowState::create($workflow->wid, $name);
    }
    $state->weight = $state_data['weight'];
    // $state->sysid = $state_data['*sysid'] // Creation state can't be changed.

    $state->save();
    $active_states[$name] = $state->sid;
  }

  // Delete any states *not* in our original construction.
  foreach ($workflow->getStates() as $state) {
    if (!in_array($state->sid, $active_states)) {
      $state->delete();
    }
  }

  // Then make transitions with the state mapping.
  $active_transitions = array();
  foreach ($new_transitions as $transition) {
    $transition = (object) $transition;
    $transition->sid = $active_states[$transition->state_name];
    $transition->target_sid = $active_states[$transition->target_state_name];
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
  foreach ($new_node_types as $node_type) {
    // Insert, nodes only have one workflow. Insert will delete any prior workflow assoc.
    workflow_insert_workflow_type_map($node_type, $workflow->wid);
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
  $workflow = NULL;

  if ($workflow = Workflow::loadByName($name)) {
    // Translate to role names so works cross install.
    $workflow->tab_roles = !empty($workflow->tab_roles) ? _workflow_rids_to_roles($workflow->tab_roles) : '';

    // Add the active states for workflow, an array of sub-objects.
    // Add only the active ones and 'creation'.
    foreach ($workflow->getStates(TRUE) as $sid => $state) {
      // Remove what we don't need to export.
      if ($state->isActive()) {
        // Clone so we can modify it independently of cached original.
        $state = clone $state;
        unset($state->sid);
        unset($state->wid);
        $state->name = $state->getName(); // Add explicitly.
        // unset($state->workflow); // Not possible: protected property.
        $workflow->states[$sid] = $state;
      }
    }

    // Then, add the active transitions for workflow, an array of sub-objects.
    $options = array('status' => 1); // We only want active transitions for this export.
    foreach ($workflow->states as $sid => $state) {
      // We're going to look everything up by the start state, not state involved, to avoid dupes.
      foreach (workflow_get_workflow_transitions_by_sid($sid, $options) as $transition) {
        // Clone so we can modify it independently of cached original.
        $transition = clone $transition;

        // And to get the target state (by name) we need to look it up, too.
        $target_state = WorkflowState::load($transition->target_sid);
        if ($target_state && $target_state->isActive() && $state->isActive()) {
          $transition->state_name = $state->getName();
          $transition->target_state_name = $target_state->getName();
          // Translate to role names so works cross install.
          $transition->roles = !empty($transition->roles) ? _workflow_rids_to_roles($transition->roles) : '';
          // Remove what we don't need to export.
          unset($transition->tid, $transition->sid, $transition->target_sid);
          $workflow->transitions[] = $transition;
        }
      }
    }

    // Finally, add the type_maps for workflow, an array of sub-objects.
    // Now we need to add data to the export for each type map, an array of sub-objects.
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
 *   A string of RIDs separated by commas.
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
 *   A string of role names separated by commas.
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
