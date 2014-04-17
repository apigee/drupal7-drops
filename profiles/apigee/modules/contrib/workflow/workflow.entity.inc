<?php

/**
 * @file
 * Integrates workflow with entity API.
 */

/**
 * Implements hook_entity_info().
 *
 *  @todo: implement hook_property_info, metadata.
 */
function workflow_entity_info() {

  $entities['Workflow'] = array(
    'label' => t('Workflow'),
    'entity class' => 'Workflow',
    'controller class' => 'WorkflowController',
    'base table' => 'workflows',
    'entity keys' => array(
      'id' => 'wid',
      'name' => 'name',
      // 'label' => 'label',
    ),
    'label callback' => 'entity_class_label',
    //'uri callback' => 'entity_class_uri',
    'module' => 'workflow',
  );

  $entities['WorkflowConfigTransition'] = array(
    'label' => t('Workflow config transition'),
    'entity class' => 'WorkflowConfigTransition',
    // Add controller Class. 'Workflow' class is the de-facto controller.
    'controller class' => 'WorkflowConfigTransitionController',
    'base table' => 'workflow_transitions',
//    'fieldable' => TRUE,
//    'exportable' => TRUE,
    'entity keys' => array(
      'id' => 'tid',
//      'name' => 'tid',
      'status' => 'status',
//      'bundle' => 'type',
    ),
    'label callback' => 'entity_class_label',
    // 'uri callback' => 'entity_class_uri',
    'module' => 'workflow',
//    'access callback' => 'workflow_tab_access', // @todo: use to-be workflow_access here. Access to Tab <> access to workflow. 
  );

  // The Controller class of Transitions and ScheduledTransition is shared.
  $entities['WorkflowTransition'] = array(
    'label' => t('Workflow executed transition'),
    'entity class' => 'WorkflowTransition',
    'controller class' => 'WorkflowTransitionController',
    'base table' => 'workflow_node_history',
    'entity keys' => array(
      'id' => 'hid',
    ),
    'label callback' => 'entity_class_label',
    'module' => 'workflow',
    'views controller class' => 'EntityDefaultViewsController',
    'rules controller class' => 'EntityDefaultRulesController',
  );

  $entities['WorkflowScheduledTransition'] = array(
    'label' => t('Workflow scheduled transition'),
    'entity class' => 'WorkflowScheduledTransition',
    'controller class' => 'WorkflowTransitionController',
    'base table' => 'workflow_scheduled_transition',
    'entity keys' => array(
      'id' => '',
    ),
    'label callback' => 'entity_class_label',
    'module' => 'workflow',
    'views controller class' => 'EntityDefaultViewsController',
    'rules controller class' => 'EntityDefaultRulesController',
  );

  return $entities;
}

/**
 * Entity loader for Workflow.
 *
 * Also used as Menu wild card loader {wildcard_name}_load for '%workflow'.
 * @see http://www.phpgainers.com/content/creating-menu-wildcard-loader-function-drupal-7
 * @todo D8: deprecated in favour of workflow_load_single(), not needed for menu. 
 */
function workflow_load($wid) {
  return entity_load_single('Workflow', $wid); 
}

function workflow_load_by_name($name) {
  foreach (entity_load('Workflow') as $workflow) {
    if ($name == $workflow->getName()) {
      return $workflow;
    }
  }
  return NULL;
}

function workflow_load_single($wid) {
  return entity_load_single('Workflow', $wid); 
}

function workflow_load_multiple($wids = FALSE, $reset = FALSE) {
  return entity_load('Workflow', $wids, array(), $reset);
}

function workflow_create($name) {
  // @todo: avoid double names in db-table, to get rid of this line of code.
  $workflow = workflow_load_by_name($name);
  if (!$workflow) {
    $workflow = entity_create('Workflow', array('name' => $name));
  }
  return $workflow;
}

/**
 * Reset the Workflow when States, Transitions have been changed.
 */
function workflow_reset_cache($wid) {
  $ids = array($wid);
  entity_get_controller('Workflow')->resetCache($ids);
}

/**
 * Loads a WorkflowState.
 */
function workflow_state_load($sid) {
  return WorkflowState::load($sid);
}

function workflow_state_load_single($sid) {
  return WorkflowState::load($sid);
}

function workflow_state_load_multiple($wid = 0, $reset = FALSE) {
  return WorkflowState::getStates($wid, $reset);
}

/**
 * Load WorkflowTransitions, most recent first.
 *
 * @param $field_name
 *   Optional. Can be NULL, if you want to load any field.
 *
 * @deprecated: workflow_get_workflow_node_history_by_nid() --> workflow_transition_load_single()
 * @deprecated: workflow_get_recent_node_history() --> workflow_transition_load_single()
 */

function workflow_state_load_by_name($name, $wid) {
  return WorkflowState::loadByName($name, $wid);
}

function workflow_transition_load_multiple($entity_type, array $entity_ids, $field_name = '', $limit = NULL) {
  return WorkflowTransition::loadMultiple($entity_type, $entity_ids, $field_name, $limit);
}

function workflow_transition_load_single($entity_type, $entity_id, $field_name = '') {
  $limit = 1;
  if ($transitions = workflow_transition_load_multiple($entity_type, array($entity_id), $field_name, $limit)) {
    return reset($transitions);
  }
  return NULL;
}

/**
 * Load function belonging to the menu option 'workflow-comment/%'.
 * Maps to this function just like 'node/%node' maps to node_load().
 *
 * @param $hid
 *   The ID of the workflow state transition record to load.
 * @return
 *   object representing one row from the {workflow_node_history} table
 */
function workflow_transition_load($hid) {
  return entity_load_single('WorkflowTransition', $hid);
}

/*
 * Test functions.
 *
 * To test caches, load same object twice in a page (see d.o. issue #1572466).
 */
function workflow_test_entity_workflow_crud() {
  $ws = workflow_load_multiple();
  $count1 = count($ws);
  $w = workflow_create('test' . REQUEST_TIME);

  $w->save();

  // Test cache: $w3 must be OK, too.
  $w2 = workflow_load($w->wid);
  $w3 = workflow_load($w->wid);
  if ($w2 != $w3) {
    // error.
  }


  // Test Cache: number of workflows must be OK.
  $ws = workflow_load_multiple();
  $count2 = count($ws);

  $w->delete();
  $ws = workflow_load_multiple();
  $count3 = count($ws);
  
  if($count1 === $count3 && ($count2 - $count1) == 1) {
    drupal_set_message('workflow->create/save/delete OK');
  }
  else  {
    drupal_set_message('workflow->create/save/delete: error');
  }


  $w = workflow_create('test' . REQUEST_TIME);
  $s1 = $w->createState('first');
  $s2 = $w->createState('second');
  $s3 = $w->createState('third');
  $s1->save();
  $s2->save();
  $s2->save();
  $s3->save();
}

function workflow_test_entity1() {
  $w = workflow_create('test' . REQUEST_TIME);
  $w->save();
  $s1 = $w->createState('first');
  $s2 = $w->createState('second');
  $s3 = $w->createState('third');
  $s1->save();
  $s2->save();
  $s2->save();
  $s3->save();
  dpm($s1);
  dpm($s2);
  dpm($w->getStates());
  $t1 = $w->createTransition($s1->sid, $s2->sid);
  $t1->save();
  dpm($t1);
  $t2 = $w->createTransition('third', 'second');
  $t2->save();
  $t3 = $w->createTransition('third', 'second');
  dpm($t2);
  dpm($t3);
//  dpm($t->uri());
//  dpm($t->label());
// $w->delete();
}

function workflow_test_entity2() {
  $ts = entity_load('WorkflowConfigTransition', array(6));
  $t = reset($ts);
  $t->roles += array(3 => 3);
  entity_save('WorkflowConfigTransition', $t);
}

function workflow_test_entity3() {
  $w = workflow_load_single(1);
  dpm($w->getStates());
dpm($w->loadTransitions());
//  $t1 = $w->createTransition($s1->sid, $s2->sid);
//  $t1->save();
//  dpm($t1);
//  $t2 = $w->createTransition('third', 'second');
//  $t2->save();
//  dpm($t2);
//  dpm($t->uri());
//  dpm($t->label());
// $w->delete();

//  $ts = entity_load('WorkflowConfigTransition', array(6));
//  $t = reset($ts);
//  $t->roles += array(3 => 3);
//  entity_save('WorkflowConfigTransition', $t);
}

function workflow_test_entity_allowable() {
  $w = workflow_create('workflow_test_'.REQUEST_TIME);
  $w->save();
  $s1 = $w->createState('to be opened');
  $s2 = $w->createState('open');
  $s3 = $w->createState('closed');
  $s1->save();
  $s2->save();
  $s3->save();
  $roles = array(1, 2); // anon, auth user.
dpm($roles);
  $t1 = $t = $w->createTransition(WORKFLOW_CREATION_STATE_NAME, 'to be opened');
  $t->roles = $roles;
  $t->save();
  $t2 = $t = $w->createTransition('to be opened', 'open');
  $t->roles = $roles;
  $t->save();
  $t3 = $t = $w->createTransition('open', 'closed');
  $t->roles = $roles;
DPM($t3);
  $t->save();
DPM($t3);
  $t4 = $t = $w->createTransition('closed', 'open');
  $t->roles = $roles;
  $t->save();

  dpm($s2->getOptions('', NULL));
  // returns  TRUE if the role is allowed to do the transition.
  $rc = $t3->isAllowed($role = 2);
  dpm($t3);
  dpm('this result must be TRUE: ' . (int) $rc);
  $rc = $t3->isAllowed($role = 3);
  dpm('this result must be FALSE: ' . (int) $rc);

  $rc = $w->GetTransitionsBySid($s3->sid);
  dpm($rc, 'allowed transitions from ' . $s3->label());

  $ts = $w->getTransitions();
  dpm($ts);
  $ts = $w->getTransitionsBySid($s3->sid);
  dpm($ts);
}
