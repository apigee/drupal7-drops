<?php
/**
 * @file
 * Plugin UI implementation.
 */

/**
 * Base UI for providing customized operation links.
 */
class RulesConditionalPluginUI extends RulesContainerPluginUI {
  public function operations() {
    $ops = parent::operations();
    $elementChildrenTypes = array();
    foreach ($this->element->getIterator() as $element) {
      $plugin = $element->plugin();
      $elementChildrenTypes[$plugin] = $plugin;
    }
    foreach (rules_fetch_data('plugin_info') as $plugin => $info) {
      // Remove operations as specified in the plugin info.
      $remove = FALSE;
      if (!empty($info['conditional single']) && isset($elementChildrenTypes[$plugin])) {
        $remove = TRUE;
      }
      elseif (!empty($info['conditional depends']) && !array_intersect($elementChildrenTypes, $info['conditional depends'])) {
        $remove = TRUE;
      }
      // Remove link.
      if ($remove) {
        unset($ops['#links']['add_' . $plugin]);
      }
    }
    return $ops;
  }
}

/**
 * Empty UI for doing nothing with the plugin.
 */
class RulesConditionalEmptyUI extends RulesConditionalPluginUI {
  public function form(&$form, &$form_state, $options = array()) {
    // Save element.
    $this->element->save();
    // Redirect right away.
    $path = RulesPluginUI::defaultRedirect($this->element);
    drupal_goto($path);
  }

  public function operations() {
    $ops = parent::operations();
    // Get rid of the 'edit' link.
    if (isset($ops['#links']['edit'])) {
      unset($ops['#links']['edit']);
    }
    return $ops;
  }
}

/**
 * UI for configuring the predicate in a conditional branch.
 */
class RulesConditionalPredicateUI extends RulesContainerPluginUI {
  /**
   * @var RulesPlugin
   */
  protected $predicate;

  public function __construct(FacesExtendable $object) {
    parent::__construct($object);
    if (!$this->element instanceof RulesConditionalPredicateElement) {
      return;
    }
    $this->predicate = $this->property('predicate');
  }

  /**
   * Delegates the form to the predicate.
   */
  public function form(&$form, &$form_state, $options = array()) {
    $baseCount = count(explode('/', RulesPluginUI::$basePath));
    $op = arg($baseCount + 2);
    if ($op == 'add') {
      // Redirect to extended path.
      $pathAddPredicate = RulesPluginUI::path($this->element->root()->name, 'add-predicate', $this->element->parentElement(), $this->element->plugin());
      drupal_goto($pathAddPredicate);
    }

    if (isset($this->predicate)) {
      // Build form for predicate.
      $form_state['rules_element'] = $this->predicate;
      $this->predicate->form($form, $form_state, $options);
      $heading = '<h4 class="rules-form-heading">';
      $heading .= t('Condition');
      $heading .= '</h4>';
      $form['parameter']['#prefix'] = $heading;
    }
  }

  /**
   * Delegates the form validator to the predicate.
   */
  public function form_validate($form, &$form_state) {
    if (isset($this->predicate)) {
      // Validate form for predicate.
      $form_state['rules_element'] = $this->predicate;
      $this->predicate->form_validate($form, $form_state);
    }
  }

  /**
   * Delegates the form submit handler to the predicate.
   */
  public function form_submit($form, &$form_state) {
    if (isset($this->predicate)) {
      // Handle form submission for predicate.
      $form_state['rules_element'] = $this->predicate;
      $this->predicate->form_submit($form, $form_state);
    }
  }

  public function buildContent() {
    $content = NULL;
    if (isset($this->predicate)) {
      // Build default content.
      $content = parent::buildContent();
      // Build parameter description from predicate.
      $predicateContent = $this->predicate->buildContent();
      if (isset($predicateContent['description']['parameter'])) {
        $content['description']['parameter'] = $predicateContent['description']['parameter'];
      }
    }
    return $content;
  }
}

/**
 * UI for supporting option lists in the case value.
 */
class RulesConditionalCaseUI extends RulesConditionalPluginUI {
  public function form(&$form, &$form_state, $options = array(), $iterator = NULL) {
    // TODO Remove when http://drupal.org/node/1676998 is fixed.
    $baseCount = count(explode('/', RulesPluginUI::$basePath));
    $op = arg($baseCount + 2);
    if ($op == 'add') {
      // Redirect to extended path.
      $pathAddPredicate = RulesPluginUI::path($this->element->root()->name, 'add-case', $this->element->parentElement(), $this->element->plugin());
      drupal_goto($pathAddPredicate);
    }

    $options['init'] = FALSE;
    parent::form($form, $form_state, $options, $iterator);
  }

  protected function getParameterForm($name, $info, $settings, &$mode) {
    $form = parent::getParameterForm($name, $info, $settings, $mode);
    if ($name == 'value' && $mode == 'input') {
      $labels = $this->getValueOptionLabels();
      if (isset($labels)) {
        $form['settings']['value']['#type'] = 'select';
        $form['settings']['value']['#options'] = $labels;
        $form['settings']['value']['#empty_value'] = '';
      }
    }
    return $form;
  }

  public function buildContent() {
    $content = parent::buildContent();
    // Use option label for text.
    if (isset($this->element->settings['value'])) {
      $value = $this->element->settings['value'];
      $labels = $this->getValueOptionLabels();
      $content['label']['#markup'] = t('@plugin: @case', array(
        '@plugin' => $this->element->label(),
        '@case' => isset($labels[$value]) ? $labels[$value] : $value,
      ));
      unset($content['description']['parameter']['value']);
    }
    return $content;
  }

  public function getValueOptionLabels() {
    $parent = $this->element->parentElement();
    if (isset($parent->settings['data:select'])) {
      $dataSelector = $parent->settings['data:select'];
      if ($wrapper = $this->element->applyDataSelector($dataSelector)) {
        $dataInfo = $wrapper->info();
        if (!empty($dataInfo['options list'])) {
          return $wrapper->optionsList('view');
        }
      }
    }
  }
}

/**
 * UI for while loops to add extra description.
 */
class RulesConditionalWhileUI extends RulesConditionalPredicateUI {
  public function form(&$form, &$form_state, $options = array()) {
    parent::form($form, $form_state, $options);

    // Display help for while loop.
    $form['while_help'] = array(
      '#weight' => -5,
    );
    $form['while_help']['text'] = array(
      '#prefix' => '<p>',
      '#markup' => t('Configure the condition for this while loop to continue. This loop will not end until the condition evaluates to false, or the maximum limit on the number of iterations has been reached.'),
      '#suffix' => '</p>',
    );

    // Display help to change iteration limit.
    $maxIterations = variable_get('rules_conditional_max_iterations', RULES_CONDITIONAL_MAX_ITERATIONS);
    $form['while_help']['description'] = array(
      '#prefix' => '<div class="description"><p>',
      '#markup' => t('The current limit is @limit. Note that this cannot be changed in the user interface. Although it can be changed by setting the %variable variable to another value, raising this limit must be done with care since Rules evaluation can be resource-intensive.', array('@limit' => $maxIterations, '%variable' => 'rules_conditional_max_iterations')),
      '#suffix' => '</p></div>',
    );
  }
}

/**
 * UI for rule condition set.
 *
 * This UI is adapted from RulesRuleUI by swapping conditions for actions.
 */
class RuleConditionSetUI extends RulesConditionContainerUI {
  /** @var RulesActionContainer */
  protected $actions;
  /** @var RuleConditionSet */
  protected $set;

  public function __construct(FacesExtendable $object) {
    parent::__construct($object);
    $this->set = $object;
    /** @var RuleConditionSet $object  */
    $this->actions = $object->actionContainer();
  }

  public function form(&$form, &$form_state, $options = array()) {
    $form_state['rules_element'] = $this->set;

    $form += array('actions' => array('#weight' => -5, '#tree' => TRUE));
    $this->actions->form($form['actions'], $form_state);

    // Add condition result form.
    $iterator = new RecursiveIteratorIterator($this->set->conditions(), RecursiveIteratorIterator::SELF_FIRST);
    parent::form($form, $form_state, $options, $iterator);
    $form['elements']['#caption'] = t('Result conditions');
    $form['negate']['#weight'] = 1;
    // Hide nested elements during creation.
    $form['elements']['#access'] = empty($options['init']);
    $form['actions']['elements']['#access'] = empty($options['init']);

    $form['help']['#markup'] = t('A rule condition set evaluates the actions first. Variables provided by the actions then can be used to evaluate the result conditions (as an "AND", i.e. evaluating to TRUE if conditions are empty).');
    $form['help']['#weight'] = -5;

    $form_state['redirect'] = RulesPluginUI::path($this->element->root()->name);
    if (!empty($options['button'])) {
      $form['submit']['#value'] = t('Save changes');
    }
  }

  /**
   * Applies the values of the form to the rule configuration.
   */
  function form_extract_values($form, &$form_state) {
    // Extract action container values.
    if (isset($form['actions'])) {
      $this->actions->extender('RulesActionContainerUI')->form_extract_values($form['actions'], $form_state);
    }
    // Extract condition container values.
    parent::form_extract_values($form, $form_state);
  }
}
