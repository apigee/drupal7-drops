<?php

/**
 * @file
 * Contains test functions.
 */

/*
 * Test functions.
 *
 * To test caches, load same object twice in a page (see d.o. issue #1572466).
 */
function workflow_test_entity_workflow_crud() {
  $ws = workflow_load_multiple();
  $count1 = count($ws);
  $workflow = workflow_create('test' . REQUEST_TIME);

  $workflow->save();

  // Test cache: $w3 must be OK, too.
  $w2 = workflow_load($workflow->wid);
  $w3 = workflow_load($workflow->wid);
  if ($w2 != $w3) {
    // error.
  }


  // Test Cache: number of workflows must be OK.
  $ws = workflow_load_multiple();
  $count2 = count($ws);

  $workflow->delete();
  $ws = workflow_load_multiple();
  $count3 = count($ws);
  
  if($count1 === $count3 && ($count2 - $count1) == 1) {
    drupal_set_message('workflow->create/save/delete OK');
  }
  else  {
    drupal_set_message('workflow->create/save/delete: error');
  }


  $workflow = workflow_create('test' . REQUEST_TIME);
  $s1 = $workflow->createState('first');
  $s2 = $workflow->createState('second');
  $s3 = $workflow->createState('third');
  $s1->save();
  $s2->save();
  $s2->save();
  $s3->save();
}

function workflow_test_entity1() {
  $workflow = workflow_create('test' . REQUEST_TIME);
  $workflow->save();
  $s1 = $workflow->createState('first');
  $s2 = $workflow->createState('second');
  $s3 = $workflow->createState('third');
  $s1->save();
  $s2->save();
  $s2->save();
  $s3->save();
  dpm($s1);
  dpm($s2);
  dpm($workflow->getStates());
  $t1 = $workflow->createTransition($s1->sid, $s2->sid);
  $t1->save();
  dpm($t1);
  $t2 = $workflow->createTransition('third', 'second');
  $t2->save();
  $t3 = $workflow->createTransition('third', 'second');
  dpm($t2);
  dpm($t3);
//  dpm($t->uri());
//  dpm($t->label());
// $workflow->delete();
}

function workflow_test_entity2() {
  $ts = entity_load('WorkflowConfigTransition', array(6));
  $t = reset($ts);
  $t->roles += array(3 => 3);
  entity_save('WorkflowConfigTransition', $t);
}

function workflow_test_entity3() {
  $workflow = workflow_load_single(1);
  dpm($workflow->getStates());
dpm($workflow->loadTransitions());
//  $t1 = $workflow->createTransition($s1->sid, $s2->sid);
//  $t1->save();
//  dpm($t1);
//  $t2 = $workflow->createTransition('third', 'second');
//  $t2->save();
//  dpm($t2);
//  dpm($t->uri());
//  dpm($t->label());
// $workflow->delete();

//  $ts = entity_load('WorkflowConfigTransition', array(6));
//  $t = reset($ts);
//  $t->roles += array(3 => 3);
//  entity_save('WorkflowConfigTransition', $t);
}

function workflow_test_entity_allowable() {
  $workflow = workflow_create('workflow_test_'.REQUEST_TIME);
  $workflow->save();
  $s1 = $workflow->createState('to be opened');
  $s2 = $workflow->createState('open');
  $s3 = $workflow->createState('closed');
  $s1->save();
  $s2->save();
  $s3->save();
  $roles = array(1, 2); // anon, auth user.
dpm($roles);
  $t1 = $t = $workflow->createTransition(WORKFLOW_CREATION_STATE_NAME, 'to be opened');
  $t->roles = $roles;
  $t->save();
  $t2 = $t = $workflow->createTransition('to be opened', 'open');
  $t->roles = $roles;
  $t->save();
  $t3 = $t = $workflow->createTransition('open', 'closed');
  $t->roles = $roles;
DPM($t3);
  $t->save();
DPM($t3);
  $t4 = $t = $workflow->createTransition('closed', 'open');
  $t->roles = $roles;
  $t->save();

  dpm($s2->getOptions('', NULL));
  // returns  TRUE if the role is allowed to do the transition.
  $rc = $t3->isAllowed($role = 2);
  dpm($t3);
  dpm('this result must be TRUE: ' . (int) $rc);
  $rc = $t3->isAllowed($role = 3);
  dpm('this result must be FALSE: ' . (int) $rc);

  $rc = $workflow->GetTransitionsBySid($s3->sid);
  dpm($rc, 'allowed transitions from ' . $s3->label());

  $ts = $workflow->getTransitions();
  dpm($ts);
  $ts = $workflow->getTransitionsBySid($s3->sid);
  dpm($ts);
}
