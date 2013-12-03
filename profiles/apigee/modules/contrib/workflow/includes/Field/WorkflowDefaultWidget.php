<?php

/**
 * @file
 * Contains workflow\includes\Field\WorkflowDefaultWidget.
 */

/**
 * Plugin implementation of the 'workflow_default' widget.
 * @todo D8: Replace "extends WorkflowD7WidgetBase" by "extends WidgetBase"
 *           or perhaps by "extends OptionsWidgetBase" from Options module.
 *
 * @FieldWidget(
 *   id = "workflow_default",
 *   label = @Translation("Workflow"),
 *   field_types = {
 *     "workflow"
 *   },
 *   settings = {
 *     "name_as_title" = 1
 *     "comment" = 1
 *   }
 * )
 */
class WorkflowDefaultWidget extends WorkflowD7Base { // D8: extends WidgetBase {

  /**
   * Function, that gets replaced by the 'annotations' in D8. (See comments above this class)
   */
  public static function settings() {
    return array(
      'workflow_default' => array(
        'label' => t('Workflow'),
        'field types' => array('workflow'),
        'settings' => array(
          'name_as_title' => 1,
          'comment' => 1,
        ),
      ),
    );
  }

  /**
   * Implements hook_field_widget_settings_form() --> WidgetInterface::settingsForm().
   *
   * {@inheritdoc}
   *
   * The Widget Instance has no settings. To have a uniform UX, all settings are done on the Field level.
   */
  public function settingsForm(array $form, array &$form_state, $has_data) {
    $element = array();
    return $element;
  }

  /**
   * Implements hook_field_widget_form --> WidgetInterface::formElement().
   *
   * {@inheritdoc}
   *
   * Be careful: this widget may be shown in very different places. Test carefully!!
   *  - On a entity add/edit page
   *  - On a entity preview page
   *  - On a entity view page
   *  - On a entity 'workflow history' tab
   *  - On a comment display, in the comment history
   *  - On a comment form, below the comment history
   * @todo D8: change "array $items" to "FieldInterface $items"
   */
  public function formElement(array $items, $delta, array $element, $langcode, array &$form, array &$form_state) {
    $field = $this->field;
    $instance = $this->instance;
    $entity = $this->entity;
    $entity_type = $this->entity_type;
    $entity_id = entity_id($entity_type, $entity);
    $field_name = $field['field_name'];

    if (!$entity) {
      // If no entity given, do not show a form. E.g., on the field settings page.
      return $element;
    }

    // Capture settings to format the form/widget.
    $settings_title_as_name = !empty($field['settings']['widget']['name_as_title']);
    // The schedule cannot be shown on a Content add page.
    $settings_schedule = !empty($field['settings']['widget']['schedule']) && $entity_id;
    $settings_schedule_timezone = !empty($field['settings']['widget']['schedule_timezone']);
    // Show comment, when both Field and Instance allow this.
    $settings_comment = $field['settings']['widget']['comment'] ? 'textarea' : 'hidden';

    $current_sid = workflow_node_current_state($entity, $entity_type, $field_name);
    $current_state = WorkflowState::load($current_sid);
    $options = array();
    $options = $current_state->getOptions($entity_type, $entity);
    // Determine the default value. If we are in CreationState, use a fast alternative for $workflow->getFirstSid().
    $default_value = $current_state->isCreationState() ? key($options) : $current_sid;

    // Get the scheduling info. This may change the $current_sid on the Form.
    $scheduled = '0';
    $timestamp = REQUEST_TIME;
    $comment = NULL;

    if ($settings_schedule) {
      // Read scheduled information.
      // Technically you could have more than one scheduled, but this will only add the soonest one.
      foreach (WorkflowScheduledTransition::load($entity_type, $entity_id, $field_name) as $scheduled_transition) {
        $scheduled = '1';
        $current_sid = $scheduled_transition->sid;
        $timestamp = $scheduled_transition->scheduled;
        $comment = $scheduled_transition->comment;
        break;
      }
    }

    // Fetch the form ID. This is unique for each entity, to allow multiple form per page (Views, etc.).
    $form_id = $form_state['build_info']['form_id'];

    $label = workflow_get_wid_label($field['settings']['wid']);

    // Prepare a wrapper. This might be a fieldset.
    $element['workflow']['#type'] = 'container';
    $element['workflow']['#attributes'] = array('class' => array('workflow-form-container'));

    // Save the current value of the node in the form, for later Workflow-module specific references.
    // We add prefix, since #tree == FALSE.
    $element['workflow']['workflow_entity'] = array('#type' => 'value', '#value' => $this->entity);
    $element['workflow']['workflow_entity_type'] = array('#type' => 'value', '#value' => $this->entity_type);
    $element['workflow']['workflow_field'] = array('#type' => 'value', '#value' => $field);
    $element['workflow']['workflow_instance'] = array('#type' => 'value', '#value' => $instance);

    // Save the form_id, so the form values can be retrieved in submit function.
    $element['workflow']['form_id'] = array('#type' => 'value', '#value' => $form_id);

    // First of all, we add the default value in the place were normal fields
    // have it. This is to cater for 'preview' of the entity.
    $element['#default_value'] = $default_value;
    // Decide if we show a widget or a formatter.
    // There is no need to a widget when the only choice is the current sid.
    if (!$current_state->showWidget($options)) {
      $element['workflow']['workflow_sid'] = workflow_state_formatter($entity_type, $entity, $field, $instance);
      return $element;
    }
    else {
      $element['workflow']['workflow_sid'] = array(
        '#type' => $field['settings']['widget']['options'],
        '#title' => $settings_title_as_name ? t('Change !name state', array('!name' => $label)) : '',
        '#options' => $options,
        // '#name' => $label,
        // '#parents' => array('workflow'),
        '#default_value' => $default_value,
      );
    }

    // Display scheduling form, but only if entity is being edited and user has
    // permission. State change cannot be scheduled at entity creation because
    // that leaves the entity in the (creation) state.
    if ($settings_schedule == TRUE && user_access('schedule workflow transitions')) {
      global $user;

      if (variable_get('configurable_timezones', 1) && $user->uid && drupal_strlen($user->timezone)) {
        $timezone = $user->timezone;
      }
      else {
        $timezone = variable_get('date_default_timezone', 0);
      }
      $timezones = drupal_map_assoc(timezone_identifiers_list());
      $hours = format_date($timestamp, 'custom', 'H:i', $timezone);

      $element['workflow']['workflow_scheduled'] = array(
        '#type' => 'radios',
        '#title' => t('Schedule'),
        '#options' => array(
          '0' => t('Immediately'),
          '1' => t('Schedule for state change'),
        ),
        '#default_value' => $scheduled,
        '#attributes' => array(
          'id' => 'scheduled_' .  $form_id,
        ),
      );
      $element['workflow']['workflow_scheduled_date_time'] = array(
        '#type' => 'fieldset',
        '#title' => t('At'),
        '#attributes' => array('class' => array('container-inline')),
        '#prefix' => '<div style="margin-left: 1em;">',
        '#suffix' => '</div>',
        '#states' => array(
          'visible' => array(':input[id="' . 'scheduled_' .  $form_id . '"]' => array('value' => '1')),
        ),
      );
      $element['workflow']['workflow_scheduled_date_time']['workflow_scheduled_date'] = array(
        '#type' => 'date',
        '#default_value' => array(
          'day'   => date('j', $timestamp),
          'month' => date('n', $timestamp),
          'year'  => date('Y', $timestamp),
        ),
      );
      $element['workflow']['workflow_scheduled_date_time']['workflow_scheduled_hour'] = array(
        '#type' => 'textfield',
        '#title' => t('Time'),
        '#maxlength' => 5,
        '#size' => 6,
        '#default_value' => $scheduled ? $hours : '00:00',
      );
      $element['workflow']['workflow_scheduled_date_time']['workflow_scheduled_timezone'] = array(
        '#type' => $settings_schedule_timezone ? 'select' : 'hidden',
        '#title' => t('Time zone'),
        '#options' => $timezones,
        '#default_value' => array($timezone => $timezone),
      );
      $element['workflow']['workflow_scheduled_date_time']['workflow_scheduled_help'] = array(
        '#type' => 'item',
        '#prefix' => '<br />',
        '#description' => t('Please enter a time in 24 hour (eg. HH:MM) format.
          If no time is included, the default will be midnight on the specified date.
          The current time is: @time.', array('@time' => format_date(REQUEST_TIME, 'custom', 'H:i', $timezone))
        ),
      );
    }
    $element['workflow']['workflow_comment'] = array(
      '#type' => $settings_comment,
      '#title' => t('Workflow comment'),
      '#description' => t('A comment to put in the workflow log.'),
      '#default_value' => $comment,
      '#rows' => 2,
    );

    // The 'add submit' can explicitely set by workflowfield_field_formatter_view(),
    // to add the submit button on the Content view page and the Workflow history tab.
    if (!empty($instance['widget']['settings']['submit_function'])) {
      // Add a submit button, but only on Entity View and History page.
      $element['workflow']['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Update workflow'),
        '#executes_submit_callback' => TRUE,
        '#submit' => array($instance['widget']['settings']['submit_function']),
      );
    }

    return $element;
  }

  /**
   * Implements workflow_transition() -> WorkflowDefaultWidget::submit().
   *
   * Overrides submit(array $form, array &$form_state).
   * Contains 2 extra parameters for D7
   *
   * @param array $form
   * @param array $form_state
   * @param array $items
   *   The value of the field.
   * @param bool $force
   *   TRUE if all access must be overridden, e.g., for Rules.
   *
   * @return int
   *   If update succeeded, the new State Id. Else, the old Id is returned.
   *
   * This is called from function _workflowfield_form_submit($form, &$form_state)
   * It is a replacement of function workflow_transition($node, $new_sid, $force, $field)
   * It performs the following actions;
   * - save a scheduled action
   * - update history
   * - restore the normal $items for the field.
   * @todo: remove update of {node_form} table. (separate task, because it has features, too)
   */
  public function submit(array $form, array &$form_state, array &$items, $force = FALSE) {
    $entity_type = $this->entity_type;
    $entity = $this->entity;
    $field_name = isset($this->field['field_name']) ? $this->field['field_name'] : '';

    // Extract the data from $items, depending on the type of widget.
    // @todo D8: use MassageFormValues($values, $form, $form_state).
    $old_sid = workflow_node_previous_state($entity, $entity_type, $field_name);
    $transition = $this->getTransition($old_sid, $items);

    $force = $force || $transition->isForced();
    $new_sid = $transition->new_sid;

    if (!$transition) {
      drupal_set_message(t('Error when try to find a WorkflowTransition.'), 'status');
      // The current value is still the previous state.
      $new_sid = $old_sid;
    }
    elseif ($error = $transition->isAllowed($force)) {
      drupal_set_message($error, 'error');
    }
    elseif (!$transition->isScheduled()) {
      // Now the data is captured in the Transition, and before calling the Execution,
      // restore the default values for Workflow Field.
      // For instance, workflow_rules evaluates this.
      if ($field_name) {
        $items = array();
        $items[0]['value'] = $old_sid;
        $entity->{$field_name}['und'] = $items;
      }

      // It's an immediate change. Do the transition.
      // - validate option; add hook to let other modules change comment.
      // - add to history; add to watchdog
      // return the new value of the sid. (Execution may fail and return the old Sid.)
      $new_sid = $transition->execute($force);
    }
    else {
      // A scheduled transition must only be saved to the database. The entity is not changed.
      $transition->save();

      // The current value is still the previous state.
      $new_sid = $old_sid;
    }

    // The entity is still to be saved, so set to a 'normal' value.
    if ($field_name) {
      $items = array();
      $items[0]['value'] = $new_sid;
      $entity->{$field_name}['und'] = $items;
    }
    return $new_sid;
  }

  /**
   * Implements hook_field_widget_error --> WidgetInterface::errorElement().
   */
  // public function errorElement(array $element, ConstraintViolationInterface $violation, array $form, array &$form_state) {
  // }

  // public function settingsSummary() {
  // }

  // public function massageFormValues(array $values, array $form, array &$form_state) {
  // }

  /**
   * Extract WorkflowTransition or WorkflowScheduledTransition from the form.
   *
   * @todo: move validation and messages to errorElement();
   */
  public function getTransition($old_sid, array $items) {
    global $user;

    $entity_type = $this->entity_type;
    $entity = $this->entity;
    // $entity_id = entity_id($entity_type, $entity);
    $field_name = !empty($this->field) ? $this->field['field_name'] : '';

    if (isset($items[0]['transition'])) {
      // a complete transition was already passed on.
      $transition = $items[0]['transition'];
    }
    else {
      // Fetch the form ID. This is unique for each entity, to allow multiple form per page (Views, etc.).
      $form_id = isset($items[0]['workflow']['form_id']) ? $items[0]['workflow']['form_id'] : '';
      $comment = isset($items[0]['workflow']['workflow_comment']) ? $items[0]['workflow']['workflow_comment'] : '';

      if (isset($items[0]['workflow']['workflow_sid'])) {
        $new_sid = $items[0]['workflow']['workflow_sid'];
      }
      elseif (isset($items[0]['value'])) {
        $new_sid = $items[0]['value'];
      }
      else {
        // This may happen if only 1 option is left, and a formatter is shown.
        $new_sid = $old_sid;
      }

      // Caveat: for the #states to work in multi-node view, the name is suffixed by unique ID.
      // We check both variants, for Node API and Field API, for backwards compatibility.
      $scheduled = (isset($items[0]['workflow']['workflow_scheduled']) ? $items[0]['workflow']['workflow_scheduled'] : 0);

      if (!$scheduled) {
        $transition = new WorkflowTransition($entity_type, $entity, $field_name, $old_sid, $new_sid, $user->uid, REQUEST_TIME, $comment);
      }
      else {
        // Schedule the time to change the state.
        // If Field Form is used, use plain values;
        // If Node Form is used, use fieldset 'workflow_scheduled_date_time'.
        $schedule = isset($items[0]['workflow']['workflow_scheduled_date_time']) ? $items[0]['workflow']['workflow_scheduled_date_time'] : $items[0]['workflow'];
        if (!isset($schedule['workflow_scheduled_hour'])) {
          $schedule['workflow_scheduled_hour'] = '00:00';
        }

        $scheduled_date_time
          = $schedule['workflow_scheduled_date']['year']
          . substr('0' . $schedule['workflow_scheduled_date']['month'], -2, 2)
          . substr('0' . $schedule['workflow_scheduled_date']['day'], -2, 2)
          . ' '
          . $schedule['workflow_scheduled_hour']
          . ' '
          . $schedule['workflow_scheduled_timezone'];

        if ($stamp = strtotime($scheduled_date_time)) {
          $transition = new WorkflowScheduledTransition($entity_type, $entity, $field_name, $old_sid, $new_sid, $user->uid, $stamp, $comment);
        }
        else {
          $transition = NULL;
        }
      }
    }
    return $transition;
  }

}
