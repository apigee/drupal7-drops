<?php
/**
 * @file
 * Tokens hooks for Workflow module.
 *
 * To every entity type, a default Workflow token tree is added. To support
 * multiple tokens per entity bundle, an extra token tree 'Workflows' is
 * created.
 *
 * How to test?
 * - Enable module 'Token'; use page admin/help/token;
 * - Enable module 'Token example'; use page examples/token;
 * - Enable module Automatic Entity Label, set a label, and save entity.
 */

/**
 * Implements drupal_alter('token_field_info', $info) from token.tokens.inc. 
 *
 * It would be nice if this would work! But it doesn't...
 */
function workflow_token_field_info_alter(&$info) {
//  $workflow_field_info = _workflow_info_fields();
//  foreach($workflow_field_info as $field_name => $field_info) {
//    if (array_key_exists($field_name, $info)) {
//      $info[$field_name]['type'] = 'WorkflowLastTransition';
//      $info[$field_name]['module'] = 'workflow';
//    }
//  }
}

/**
 * Adds a subtree to each WorkflowField.
 *
 * ATM we only generate tokens for the last transition of a field.
 */
function workflow_token_info_alter(&$data) {
  foreach ($data['tokens'] as $object => &$tokens) {

    // Add a token for scheduling, in 'seconds ago' format.
    if ($object == 'date' && !isset($tokens['seconds'])) {
      $tokens['seconds'] = array(
        'name'  => 'Seconds-since', 
        'description' => "A date in 'seconds ago' format (<i>604800</i>). Use it for easy scheduling workflow transitions.",
        'module' => 'workflow',
      );
    }
    // High-jack the fields (they do not have sub-tokens, yet).
    foreach ($tokens as &$token) {
      // Caveat: the following agorithm is just a guess.
      if (isset($token['module']) && $token['module'] == 'token') {
        if (isset($token['description']) && 0 == substr_compare($token['description'], 'Workflow', 0, 8)) {
          $token['type'] = 'WorkflowLastTransition';
          $token['module'] = 'workflow';
        }
      }
    }
  }
}

/**
 * Implements hook_token_info().
 *
 * Adds tokens and token types, from field to transition to workflow and state.
 * Using the names of the entities from Workflow module.
 */
function workflow_token_info() {
  /*
   * Token types.
   */
  $types['WorkflowField'] = array(
    'name' => t('Workflows'),
    'description' => t('Tokens related to workflows.'),
    'needs-data' => 'entity',
  );
  $types['WorkflowTransition'] = array(
    'name' => t('Workflow Transition'),
    'description' => t('Tokens related to workflow transitions.'),
    // 'needs-data' => 'entity',
    'needs-data' => 'WorkflowField',
  );
  $types['WorkflowState'] = array(
    'name' => t('Workflow State'),
    'description' => t('Tokens related to workflow state.'),
    'needs-data' => 'WorkflowTransition',
  );
  $types['Workflow'] = array(
    'name' => t('Workflow'),
    'description' => t('Tokens related to workflows.'),
    'needs-data' => 'WorkflowTransition',
  );

  /*
   * Chained tokens for nodes.
   */
  $last_transition = array(
    'name' => t('Workflow last transition'),
    'description' => t('Last workflow state transition of content.'),
    'type' => 'WorkflowTransition',
    'module'=> 'workflow',
  );

  // Add a token tree to each core entity type. This allows easy reference
  // in the majority of cases.
  $workflow_field['last-transition'] = $last_transition;
  $entity['last-transition'] = $last_transition;
  $user['last-transition'] = $last_transition;
  $node['last-transition'] = $last_transition;
  $term['last-transition'] = $last_transition;

  // Tokens for 'Workflow' class.
  $workflow['id'] = array(
    'name' => t('Workflow ID'),
    'description' => t('The unique ID of the workflow applied to this node.'),
    'module'=> 'workflow',
  );
  $workflow['label'] = array(
    'name' => t('Workflow label'),
    'description' => t('The workflow applied to this node.'),
    'module'=> 'workflow',
  );

  // Tokens for 'Workflow State' class.
  $workflow_state['id'] = array(
    'name' => t('State ID'),
    'description' => t('The State ID.'),
    'module'=> 'workflow',
  );
  $workflow_state['label'] = array(
    'name' => t('State label'),
    'description' => t('The state label.'),
    'module'=> 'workflow',
  );

  // Tokens for 'Workflow Transition' class.
  $workflow_transition['Workflow'] = array(
    'name' => t('Workflow'),
    'description' => t('Workflow the state belongs to.'),
    'type' => 'Workflow',
    'module'=> 'workflow',
  );
  $workflow_transition['new_state'] = array(
    'name' => t('New state'),
    'description' => t("The new state."),
    'type' => 'WorkflowState',
    'module'=> 'workflow',
  );
  $workflow_transition['old_state'] = array(
    'name' => t('Old state'),
    'description' => t('The old state.'),
    'type' => 'WorkflowState',
    'module'=> 'workflow',
  );
  $workflow_transition['body'] = array(
    'name' => t('Comment'),
    'description' => t('The comment on the transititon.'),
    'module'=> 'workflow',
  );
  $workflow_transition['user'] = array(
    'name' => t('User'),
    'description' => t('The user that executed the transition.'),
    'type' => 'user',
  );
  $workflow_transition['created'] = array(
    'name' => t('Created'),
    'description' => t('The date the transition was created.'),
    'type' => 'date',
  );

  $return = array(
    'types' => $types,
    'tokens' => array(
      'entity' => $entity,
      'user' => $user,
      'node' => $node,
      'term' => $term,
      'WorkflowField' => $workflow_field,
      'WorkflowTransition' => $workflow_transition,
      'WorkflowState' => $workflow_state,
      'Workflow' => $workflow,
    ),
  );

  return $return;
}

/**
 * Implements hook_tokens().
 */
function workflow_tokens($type, $tokens, array $data = array(), array $options = array()) {
  $replacements = array();
	
  $sanitize = !empty($options['sanitize']);
  $langcode = !empty($options['language']->language) ? $options['language']->language : LANGUAGE_NONE;

  if ($type == 'date' && !empty($data['date']) ) {
    foreach ($tokens as $name => $original) {
      switch ($name) {
        case 'seconds':
          // This is our custom date token in 'seconds ago' format.
          $seconds = REQUEST_TIME - $data['date'];
          $replacements[$original] = $seconds;
          break;
      }
    }
  }
  elseif ($type == 'entity' && !empty($data['entity']) ) {
    // new, chained tokens, as of version 7.x-2.3.
    $entity = $data['entity'];
    $entity_type = $data['entity_type'];
    $token_type = $data['token_type'];

    foreach ($tokens as $name => $original) {
      switch ($name) {
        case 'id':
          // Valid for WorkflowTransition, WorkflowState, Workflow.
          list($id, $vid, $bundle) = entity_extract_ids($entity_type, $entity);
          $replacements[$original] = $id;
          break;

        case 'label':
          // Valid for WorkflowTransition, WorkflowState, Workflow.
          $replacements[$original] = entity_label($entity_type, $entity);
          break;

        case 'body':
          // Valid for WorkflowTransition.
          $replacements[$original] = $sanitize ? filter_xss($entity->comment) : $entity->comment;
          break;

        case 'seconds':
          // Valid for WorkflowTransition.
          $replacements[$original] = $sanitize ? filter_xss($entity->comment) : $entity->comment;
          break;

        // Default values for the chained tokens handled below.
        case 'old_state':
          // Valid for WorkflowTransition.
          $state = workflow_state_load($entity->old_sid);
          $replacement = $state ? $state->label() : '<unknown>';
          $replacements[$original] = $sanitize ? filter_xss($replacement) : $replacement;
          break;

        case 'new_state':
          // Valid for WorkflowTransition.
          $state = workflow_state_load($entity->new_sid);
          $replacement = $state ? $state->label() : '<unknown>';
          $replacements[$original] = $sanitize ? filter_xss($replacement) : $replacement;
          break;

        case 'user':
          // Valid for WorkflowTransition.
          $replacements[$original] = $sanitize ? $entity->uid : $entity->uid;
          break;

        case 'created':
          // Valid for WorkflowTransition.
          $replacements[$original] = format_date($entity->stamp, 'medium', '', NULL, $langcode);
          break;
      }
    }

    // If this is a WorkflowField, get subtokens for it.
    $name_parts = explode(':', $name, 2);
    $field_name = reset($name_parts);
    if (isset($entity->{$field_name})) {
      if ($sub_tokens = token_find_with_prefix($tokens, $field_name)) {
        $sub_token_type = 'WorkflowTransition';
        $data = array(
          'entity_type' => $data['entity_type'],
          'entity' => $data['entity'],
          'token_type' => $sub_token_type,
          'field_name' => $field_name,
        );
        $replacements += token_generate('entity', $sub_tokens, $data, $options);
      }
    }

    // If this is a Last Transition, get subtokens for it.
    if ($sub_tokens = token_find_with_prefix($tokens, 'last-transition')) {
      // Get the workflow tokens from the transition of this entity.
      $field_name = isset($data['field_name']) ? $data['field_name'] : NULL;
      // Get NUMERIC id.
      list($entity_id, , ) = entity_extract_ids($entity_type, $entity);

      // If the node is being created or updated, do not read from db, since
      // the field widget has not been processed yet.
      // @todo: solve this with module weight?
      if (empty($entity_id) || isset($entity->original)) {
        $transitions = _workflow_tokens_get_transitions($entity_type, $entity, $entity_id);
        $last_transition = $field_name ? $transitions[$field_name] : reset($transitions);
      }
      else {
        $last_transition = workflow_transition_load_single($entity_type, $entity_id, $field_name);
      }
      $sub_token_type = 'WorkflowTransition';
      $data = array(
        'entity_type' => $sub_token_type,
        'entity' => $last_transition,
        'token_type' => $sub_token_type,
      );
      $replacements += token_generate('entity', $sub_tokens, $data, $options);
    }

    if ($token_type == 'WorkflowTransition') {
      if ($sub_tokens = token_find_with_prefix($tokens, 'Workflow')) {
        // $entity is a Transition, get its workflow.
        $sub_token_type = 'Workflow';
        $data = array(
          'entity_type' => $sub_token_type,
          'entity' => $entity->getWorkflow(),
          'token_type' => $sub_token_type,
        );
        $replacements += token_generate('entity', $sub_tokens, $data, $options);
      }
      if ($sub_tokens = token_find_with_prefix($tokens, 'old_state')) {
        if ($data['entity_type'] == 'WorkflowTransition') {
          $transition = $entity;
        }
        else {
          $field_name = $data['field_name'];
          // Get NUMERIC id.
          list($entity_id, , ) = entity_extract_ids($entity_type, $entity);
          $transition = workflow_transition_load_single($entity_type, $entity_id, $field_name);
        }
        $state = workflow_state_load($transition->old_sid);

        // $entity is a Transition, get its state.
        $sub_token_type = 'WorkflowState';
        $data = array(
          'entity_type' => $sub_token_type,
          'entity' => $state,
          'token_type' => $sub_token_type,
        );
        $replacements += token_generate('entity', $sub_tokens, $data, $options);
      }
      if ($sub_tokens = token_find_with_prefix($tokens, 'new_state')) {
        if ($data['entity_type'] == 'WorkflowTransition') {
          $transition = $entity;
        }
        else {
          $field_name = $data['field_name'];
          // Get NUMERIC id.
          list($entity_id, , ) = entity_extract_ids($entity_type, $entity);
          $transition = workflow_transition_load_single($entity_type, $entity_id, $field_name);
        }
        $state = workflow_state_load($transition->new_sid);

        // $entity is a Transition, get its state.
        $sub_token_type = 'WorkflowState';
        $data = array(
          'entity_type' => $sub_token_type,
          'entity' => $state,
          'token_type' => $sub_token_type,
        );
        $replacements += token_generate('entity', $sub_tokens, $data, $options);
      }
      if ($sub_tokens = token_find_with_prefix($tokens, 'user')) {
        // $entity is a Transition, get its user.
        $sub_token_type = 'user';
        $data = array(
          'entity_type' => $sub_token_type,
          'user' => user_load($entity->uid),
          'token_type' => $sub_token_type,
        );
        $replacements += token_generate('user', $sub_tokens, $data, $options);
      }
      if ($sub_tokens = token_find_with_prefix($tokens, 'created')) {
        // $entity is a Transition, get its state.
        $sub_token_type = 'date';
        $data = array(
          'entity_type' => $sub_token_type,
          'date' => $entity->stamp,
          'token_type' => $sub_token_type,
        );
        $replacements += token_generate('date', $sub_tokens, $data, $options);
      }
    }
  }

  return $replacements;
}

function _workflow_tokens_get_transitions($entity_type, $entity, $entity_id) {
  global $user;
  $transitions = array();

  foreach ($fields = _workflow_info_fields($entity, $entity_type) as $field_name => $field) {
    // Create a dummy transition, just to set $node->workflow_transitions.
    $new_sid = _workflow_get_sid_by_items($entity->{$field_name}[LANGUAGE_NONE]);
    if (!isset($entity_id)) {
      // Creating an entity.
      $new_state = workflow_state_load($new_sid);
      $workflow = $new_state->getWorkflow();
      $old_sid = $workflow->getCreationSid();
    }
    elseif (isset($entity->original)) {
      // Updating an entity.
      $old_sid = _workflow_get_sid_by_items($entity->original->{$field_name}[LANGUAGE_NONE]);
    }
    $transition = new WorkflowTransition();
    $transition->setValues($entity_type, $entity, $field_name, $old_sid, $new_sid, $user->uid, REQUEST_TIME, '');

    // Store the transition, so it can be easily fetched later on.
    // Store in an array, to prepare for multiple workflow_fields per entity.
    // This is a.o. used in hook_entity_update to trigger 'transition post'.
    $transitions[$field_name] = $transition;
  }
  return $transitions;
}
