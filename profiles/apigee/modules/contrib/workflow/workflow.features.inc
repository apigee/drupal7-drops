<?php

/**
 * @file
 * Integrates workflow with features.
 */

define('WORKFLOW_FEATURES_AUTHOR_NAME', 'workflow_features_author_name');

// Even if workflow Node is not enabled, Features may use Node API's type_maps.
module_load_include('inc', 'workflow', 'workflow.node.type_map');

/**
 * Workflows are a **faux-exportable** component.
 */

/**
 * Implements hook_features_export_options().
 *
 * Generates the options to choose from.
 * Using $wid as key. Result is used by hook_features_export().
 */
function workflow_features_export_options() {
  $workflows = array();
  foreach (workflow_load_multiple() as $workflow) {
    $name = $workflow->getName();
    $workflows[$name] = $name;
  }
  return $workflows;
}

/**
 * Implements hook_features_export().
 */
function workflow_features_export($data, &$export, $module_name = '') {
  $pipe = array();

  $export['dependencies']['features'] = 'features';
  $export['dependencies']['workflow'] = 'workflow';

  foreach ($data as $workflow_name) {
    if ($workflow = workflow_load_by_name($workflow_name)) {

      if (count($workflow->getTypeMap())) {
        $export['dependencies']['workflownode'] = 'workflownode';
      }

      $export['features']['workflow'][$workflow_name] = $workflow_name;
    }
  }

  return $pipe;
}

/**
 * Implements hook_features_export_render().
 *
 * Do not use default exportable functions. For backwards compatibility we
 * want to generate PHP code, not JSON.
 */
function workflow_features_export_render($module, $data) {
  $translatables = $code = array();
  $code[] = '  $workflows = array();';
  $code[] = '';

  foreach ($data as $workflow_name) {
    // Avoid default var_export, by using array.
    if ($workflow = _workflow_load_workflows_full($workflow_name)) {
      $workflow_export = features_var_export((array) $workflow, '  ', FALSE);
      $workflow_identifier = features_var_export($workflow_name);
      $code[] = "  // Exported workflow: " . $workflow_name;
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
    _workflow_update_workflows_full_object($workflow);
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
 * CRUD style helper functions below.
 */

/**
 * For use by CRUD only, gather everything into the CRUD formed object.
 *
 * @param $name
 *   A string corresponding to a workflow object.
 *
 * @return
 *   A fully loaded workflow array.
 */
function _workflow_load_workflows_full($name) {
  $result = array();

  if ($workflow = workflow_load_by_name($name)) {
    // Clone so we can modify it independently of cached original.
    $workflow = clone $workflow;

    $result['name'] = $workflow->getName();
    // Translate to role names so it works cross install.
    $result['tab_roles'] = _workflow_rids_to_roles($workflow->tab_roles);
    $result['options'] = $workflow->options;

    // Add the active states for workflow, an array of sub-objects.
    // Add only the active ones and 'creation'.
    $result['states'] = array();
    foreach ($workflow->getStates('CREATION') as $sid => $state) {
      // Remove what we don't need to export.
      if ($state->isActive()) {
        // Clone so we can modify it independently of cached original.
        $state = clone $state;
        unset($state->sid);
        unset($state->wid);
        $state->name = $state->getName(); // Add explicitly.
        // unset($state->workflow); // Not possible: protected property.

        $result['states'][$sid] = $state;
      }
    }

    // Then, add the active transitions for workflow, an array of sub-objects.
    foreach ($workflow->states as $sid => $state) {
      // We're going to look everything up by the start state, not state involved, to avoid dupes.
      foreach ($t = $workflow->getTransitionsBySid($sid) as $transition) {
        // Clone so we can modify it independently of cached original.
        $transition = clone $transition;

        // And to get the target state (by name) we need to look it up, too.
        $target_state = workflow_state_load_single($transition->target_sid);
        if ($target_state && $target_state->isActive() && $state->isActive()) {
          // The Transition seems an exportable entity, so we need to copy it.
          $new_transition = new stdClass();
          $new_transition->state_name = $state->getName();
          $new_transition->target_state_name = $target_state->getName();
          // Translate to role names so it works cross install.
          $new_transition->roles = _workflow_rids_to_roles($transition->roles);
          // Remove what we don't need to export.
          unset($transition->tid, $transition->sid, $transition->target_sid);

          $result['transitions'][] = $new_transition;
        }
      }
    }

    // Finally, add the type_maps for workflow, an array of sub-objects.
    // Now we need to add data to the export for each type map, an array of sub-objects.
    foreach (workflow_get_workflow_type_map_by_wid($workflow->wid) as $index => $type_map) {
      $result['node_types'][] = $type_map->type;
    }
  }

  return $result;
}

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
function _workflow_update_workflows_full_object($workflow_data) {
  $workflow_data = (object) $workflow_data;

  // First we grab the states, transitions and node_maps out.
  $new_states = isset($workflow_data->states) ? $workflow_data->states : array();
  $new_transitions = isset($workflow_data->transitions) ? $workflow_data->transitions : array();
  $new_node_types = isset($workflow->node_types) ? $workflow->node_types : array();
  $name = $workflow_data->name;

  // Then make a workflow so we can track by wid.
  if (!$workflow = workflow_load_by_name($name)) {
    $workflow = workflow_create($name);
  }

  unset($workflow->tab_roles);
  // Roles are exported by rolename, so need to translate to RID.
  $workflow->tab_roles = !empty($workflow_data->tab_roles) ? _workflow_roles_to_rids($workflow_data->tab_roles) : array();

  unset($workflow->options);
  $workflow->options = $workflow_data->options;

  // For new workflows, $wid is only known after save().
  $workflow->save();

  // Cancel out if workflow failed to save.
  if (!isset($workflow->wid) || empty($workflow->wid)) {
    return FALSE;
  }

  // Workflow is now a fully vetted workflow object.
  // Then make states, marking state name to state sid.
  $orig_states = $workflow->getStates($all = TRUE); // Initialize the cache for this workflow.
  $active_states = array();
  foreach ($new_states as $state_data) {
    $name = $state_data['name'];
    if (!$state = workflow_state_load_by_name($name, $workflow->wid)) {
      $state = $workflow->createState($name);
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
  foreach ($new_transitions as $new_data) {
    $transition = $workflow->createTransition($new_data['state_name'], $new_data['target_state_name']);

    // Roles are exported by rolename, so need to translate to RID.
    $transition->roles = !empty($new_data['roles']) ? _workflow_roles_to_rids($new_data['roles']) : array();
    $transition->save();

    $active_transitions[] = $transition->tid;
  }

  // Delete any transitions in our workflow that are *not* in our original construction.
  foreach ($workflow->getTransitions() as $transition) {
    if (!in_array($transition->tid, $active_transitions)) {
      $transition->delete();
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
 * Translates a role string to RIDs for importing.
 *
 * @param $role_string
 *   A string of roles or fake 'author' role.
 *
 * @return
 *   An array of RIDs.
 */
function _workflow_roles_to_rids($role_string) {
  $roles = workflow_get_roles($permission = NULL);
  $rid_array = array();
  foreach (explode(',', $role_string) as $role_name) {
    if ($role_name === WORKFLOW_FEATURES_AUTHOR_NAME) {
      $rid_array[WORKFLOW_ROLE_AUTHOR_RID] = WORKFLOW_ROLE_AUTHOR_RID;
    }
    elseif ($role_name && in_array($role_name, $roles)) {
      $rid = array_search($role_name, $roles);
      $rid_array[$rid] = $rid;
    }
  }
  return $rid_array;
}

/**
 * Translates a string of rids to role names for exporting.
 *
 * @param $rid_array
 *   An array of rids or fake 'author' role.
 *
 * @return
 *   A string of role names separated by commas.
 */
function _workflow_rids_to_roles($rid_array) {
  // Get all roles, including 'author'.
  $roles = workflow_get_roles($permission = NULL);
  // There may be a role named 'author', so make 'author' distinct.
  $roles[WORKFLOW_ROLE_AUTHOR_RID] = WORKFLOW_FEATURES_AUTHOR_NAME;
  // Translate RIDs to rolenames.
  $return = implode(',', array_intersect_key($roles, array_flip($rid_array)));
  return trim($return, ',');
}
