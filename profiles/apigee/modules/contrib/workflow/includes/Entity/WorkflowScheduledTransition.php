<?php

/**
 * @file
 * Contains workflow\includes\Entity\WorkflowScheduledTransition.
 */

/**
 * Implements a scheduled transition, as shown on Workflow form.
 */
class WorkflowScheduledTransition extends WorkflowTransition {
  public $scheduled;
  protected $is_scheduled = TRUE;
  protected $is_executed = FALSE;

  /**
   * Constructor
   */
  public function __construct($entity_type = '', $entity = NULL, $field_name = '', $old_sid = 0, $new_sid = 0, $uid = 0, $scheduled = 0, $comment = '') {
    parent::__construct($entity_type, $entity, $field_name, $old_sid, $new_sid, $uid, $stamp = 0, $comment);
    $this->scheduled = (!$scheduled) ? $this->scheduled : $scheduled;
  }

  /**
   * Given a node, get all scheduled transitions for it.
   *
   * @param $entity_type
   * @param $entity_id
   * @param $field_name
   *  optional
   *
   * @return array
   *  an array of WorkflowScheduledTransitions
   *
   * @deprecated: workflow_get_workflow_scheduled_transition_by_nid() --> WorkflowScheduledTransition::load()
   */
  public static function load($entity_type, $entity_id, $field_name = '', $limit = NULL) {
    if (!$entity_id) {
      return FALSE;
    }

    $query = db_select('workflow_scheduled_transition', 'wst');
    $query->fields('wst');
    $query->condition('entity_type', $entity_type, '=');
    $query->condition('nid', $entity_id, '=');
    $query->orderBy('scheduled', 'ASC');
    $query->addTag('workflow_scheduled_transition');
    if ($limit) {
      $query->range(0, $limit);
    }
    $result = $query->execute()->fetchAll(PDO::FETCH_CLASS, 'WorkflowScheduledTransition');

    return $result;
  }

  /**
   * Given a timeframe, get all scheduled transitions.
   * @deprecated: workflow_get_workflow_scheduled_transition_by_between() --> WorkflowScheduledTransition::loadBetween()
   */
  public static function loadBetween($start = 0, $end = 0) {
    $query = db_select('workflow_scheduled_transition', 'wst');
    $query->fields('wst');
    $query->orderBy('scheduled', 'ASC');
    $query->addTag('workflow_scheduled_transition');

    if ($start) {
      $query->condition('scheduled', $start, '>');
    }
    if ($end) {
      $query->condition('scheduled', $end, '<');
    }

    $result = $query->execute()->fetchAll(PDO::FETCH_CLASS, 'WorkflowScheduledTransition');
    return $result;
  }

  /**
   * Save a scheduled transition. If the transition is executed, save as logged transition.
   */
  public function save() {
    // If executed, save as logged transition.
    if ($this->is_executed) {
      $result = parent::save();
      return $result;
    }

    // Avoid duplicate entries.
    $clone = clone $this;
    $clone->delete();
    // Save (insert or update) a record to the database based upon the schema.
    drupal_write_record('workflow_scheduled_transition', $this);

    // Get name of state.
    if ($state = WorkflowState::load($this->new_sid)) {
      $entity = $this->getEntity();
      $message = '@entity_title scheduled for state change to %state_name on %scheduled_date';
      $args = array(
        '@entity_type' => $this->entity_type,
        '@entity_title' => $entity->title,
        '%state_name' => $state->label(),
        '%scheduled_date' => format_date($this->scheduled),
      );
      $uri = entity_uri($this->entity_type, $entity);
      watchdog('workflow', $message, $args, WATCHDOG_NOTICE, l('view', $uri['path'] . '/workflow'));
      drupal_set_message(t($message, $args));
    }
  }

  /**
   * Given a node, delete transitions for it.
   * @deprecated: workflow_delete_workflow_scheduled_transition_by_nid() --> WorkflowScheduledTransition::delete()
   */
  public function delete() {
    $result = $this->deleteById($this->entity_type, $this->entity_id);
    return $result;
  }

  /**
   * Given an Entity, delete transitions for it.
   * @todo: add support for Field.
   */
  public static function deleteById($entity_type, $entity_id) {
    $conditions = array(
      'entity_type' => $entity_type,
      'nid' => $entity_id,
    );
    $result = self::deleteMultiple($conditions);
    return $result;
  }

  public static function deleteMultiple(array $conditions, $table = 'dummy') {
    // The $table argument is to adhere to the parent::deleteMultiple interface. It must not be changeable.
    $result = parent::deleteMultiple($conditions, $table = 'workflow_scheduled_transition');
    return $result;
  }

  /**
   * Property functions.
   */

  /**
   * If a scheduled transition has no comment, a default comment is added before executing it.
   */
  public function addDefaultComment() {
    $this->comment = t('Scheduled by user @uid.', array('@uid' => $this->uid));
  }

  /**
   * Get the Transition's $field_info.
   *
   * This is called in hook_cron, to get the $field_info.
   * @todo: read $field_name directly from table.
   */
  public function getWorkflowItem() {
    $workflow_item = NULL;

    if (!empty($this->field_name)) {
      // @todo: read $field_name directly from table.
    }
	
    $entity_type = $this->entity_type;
    $entity = $this->getEntity();
    $entity_bundle = $this->getEntity()->type;

    foreach (field_info_instances($entity_type, $entity_bundle) as $field_name => $field_instance) {
      $field_info = field_info_field($field_instance['field_name']);
      $field_type = $field_info['type'];
      if ($field_type == 'workflow') {
        // Set cache.
        $this->field_name = $field_name;
        // Prepare return value.
        $workflow_item = new WorkflowItem($field_info, $field_instance, $entity_type, $this->getEntity());
      }
    }
    return $workflow_item;
  }

}
