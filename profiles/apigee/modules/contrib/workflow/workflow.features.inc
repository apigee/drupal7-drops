<?php

/**
 * @file
 * Provides Features integration for Workflow using the CRUD API.
 *
 * As you will notice this file will only handle the <export> of Worflows, 
 * including states and transitions. The <import> is handeled magically, 
 * and all modifications are done in function Workflow::save().
 */

define('WORKFLOW_FEATURES_AUTHOR_NAME', 'workflow_features_author_name');

// Even if workflow Node is not enabled, Features may use Node API's type_maps.
module_load_include('inc', 'workflow', 'workflow.node.type_map');

/**
 * Default controller handling features integration.
 */
class WorkflowFeaturesController extends EntityDefaultFeaturesController {

  /**
   * Generates the result for hook_features_export().
   */
  public function export($data, &$export, $module_name = '') {
    $pipe = array();
    $pipe = parent::export($data, $export, $module_name);

    foreach ($data as $workflow_name) {
      if ($workflow = workflow_load_by_name($workflow_name)) {
        // Add dependency on workflow_node.
        if (count($workflow->getTypeMap())) {
          $export['dependencies']['workflownode'] = 'workflownode';
        }
        // Add dependency on workflow_field.
        $export['features']['Workflow'][$workflow_name] = $workflow_name;
      }
    }
    return $pipe;
  }

  /**
   * Generates the result for hook_features_export_render().
   *
   * This is a copy of the parent, adding 'system_roles'.
   */
  function export_render($module, $data, $export = NULL) {
    $output = array();
    $output[] = '  $items = array();';
    foreach (entity_load_multiple_by_name($this->type, $data) as $name => $entity) {

      // >> A modification for the Workflow object:
      // Add roles to translate role IDs on target system.
      $entity->system_roles = workflow_get_roles($permission = NULL);
      // >> Now resume with normal flow.

      $export  = "  \$items['$name'] = entity_import('{$this->type}', '";
      // Make sure to escape the characters \ and '.
      $export .= addcslashes(entity_export($this->type, $entity, '  '), '\\\'');
      $export .= "');";
      $output[] = $export;
    }
    $output[] = '  return $items;';
    $output = implode("\n", $output);

    $hook = isset($this->info['export']['default hook']) ? $this->info['export']['default hook'] : 'default_' . $this->type;

    return array($hook => $output);
  }

  /**
   * Overridden to not delete upon revert.
   */
  function revert($module = NULL) {
    // Loads defaults from feature code.
    $defaults = features_get_default($entity_type, $module);
    if ($defaults = features_get_default($this->type, $module)) {
      foreach ($defaults as $name => $entity) {
        // entity_delete($this->type, $name);
      }
    }
  }
}


/**
 * Implements hook_features_rebuild().
 */
function workflow_features_rebuild($module) {
  // workflow_features_revert($module);
  $entity_type = 'Workflow';

  // Do not delete the previous workflow. It will break the installation due to
  // the new $wid that is created.
  // return entity_features_get_controller($entity_type)->revert($module);

//  $info = entity_get_info($entity_type);
//  if (in_array('EntityAPIControllerInterface', class_implements($info['controller class']))) {
//    return entity_get_controller($entity_type)->import($export);
//  }
}

/** *******************************************************************
 * CRUD style helper functions below.
 */

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
