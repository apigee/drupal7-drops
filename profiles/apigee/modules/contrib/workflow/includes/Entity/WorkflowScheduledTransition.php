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
   *
   * @todo: use parent::__construct ?
   */
  public function __construct($entity_type = '', $entity = NULL, $field_name = '', $old_sid = 0, $new_sid = 0, $uid = 0, $stamp = 0, $comment = '') {
    parent::__construct($entity_type, $entity, $field_name, $old_sid, $new_sid, $uid, $stamp, $comment);
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
    $results = db_query('SELECT * ' .
                        'FROM {workflow_scheduled_transition} ' .
                        'WHERE entity_type = :entity_type ' .
                        'AND   nid = :nid ' .
                        'ORDER BY scheduled ASC ',
                        array(':nid' => $entity_id, ':entity_type' => $entity_type));
    $result = $results->fetchAll(PDO::FETCH_CLASS, 'WorkflowScheduledTransition');
    return $result;
  }

  /**
   * Given a timeframe, get all scheduled transitions.
   * @deprecated: workflow_get_workflow_scheduled_transition_by_between() --> WorkflowScheduledTransition::loadBetween()
   */
  public static function loadBetween($start = 0, $end = REQUEST_TIME) {
    $results = db_query('SELECT * ' .
                        'FROM {workflow_scheduled_transition} ' .
                        'WHERE scheduled > :start AND scheduled < :end ' .
                        'ORDER BY scheduled ASC',
                        array(':start' => $start, ':end' => $end));
    $result = $results->fetchAll(PDO::FETCH_CLASS, 'WorkflowScheduledTransition');
    return $result;
  }

  /**
   * Save a scheduled transition. If the transition is executed, save as logged transition.
   */
  public function save() {
    // If executed, save as logged transition.
    if ($this->is_executed) {
      return parent::save();
    }

    // Avoid duplicate entries.
    $this->delete();
    // Save (insert or update) a record to the database based upon the schema.
    drupal_write_record('workflow_scheduled_transition', $this);

    // Get name of state.
    if ($state = WorkflowState::load($this->new_sid)) {
      $message = '@entity_title scheduled for state change to %state_name on %scheduled_date';
      $args = array(
        '@entity_type' => $this->entity_type,
        '@entity_title' => $this->entity->title,
        '%state_name' => $state->label(),
        '%scheduled_date' => format_date($this->scheduled),
      );
      $uri = entity_uri($this->entity_type, $this->entity);
      watchdog('workflow', $message, $args, WATCHDOG_NOTICE, l('view', $uri['path'] . '/workflow'));
      drupal_set_message(t($message, $args));
    }
  }

  /**
   * Given a node, delete transitions for it.
   * @deprecated: workflow_delete_workflow_scheduled_transition_by_nid() --> WorkflowScheduledTransition::delete()
   */
  public function delete() {
    return $this->deleteById($this->entity_type, $this->entity_id);
  }

  public static function deleteMultiple(array $conditions, $table = 'dummy') {
    // The $table argument is to adhere to the parent::deleteMultiple interface. It must not be changeable.
    return parent::deleteMultiple($conditions, $table = 'workflow_scheduled_transition');
  }

  /**
   * Given a Entity, delete transitions for it.
   * @todo: add support for Field.
   */
  public static function deleteById($entity_type, $entity_id) {
    $conditions = array(
//      'entity_type' => $entity_type,
      'nid' => $entity_id,
    );
    return self::deleteMultiple($conditions);
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
