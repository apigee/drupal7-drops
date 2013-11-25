<?php
/**
 * @file
 * Conditional Rules framework implementation.
 */

/**
 * Base conditional statement plugin implementation.
 */
abstract class RulesConditionalContainer extends RulesContainerPlugin implements RulesActionInterface {
  /**
   * Magic methods to intercept.
   * @var array
   */
  protected $interceptMethods = array();

  /**
   * @var RulesActionContainer
   */
  protected $fluentElement;

  protected $providesVariables;

  public function label() {
    $info = $this->pluginInfo();
    $label = isset($info['label']) ? $info['label'] : t('unlabeled');
    return $label;
  }

  /**
   * Intercepts calls to magic methods, possibly using reserved keywords.
   */
  public function __call($name, $arguments = array()) {
    if (in_array($name, $this->interceptMethods) && method_exists($this, $mapMethod = 'call_' . $name)) {
      return call_user_func_array(array($this, $mapMethod), $arguments);
    }
    else {
      return parent::__call($name, $arguments);
    }
  }

  /**
   * Adds an action to the active fluent statement.
   *
   * Pass either an instance of the RulesActionInterface or the arguments as
   * needed by rules_action().
   *
   * @param $name
   * @param array $settings
   * @return $this
   *   Returns $this for fluent interface.
   */
  public function action($name, array $settings = array()) {
    if (isset($this->fluentElement)) {
      $this->fluentElement->action($name, $settings);
    }
    return $this;
  }

  /**
   * Evaluates the conditional statement.
   */
  public function evaluate(RulesState $state) {
    // Evaluate selected branches.
    $branches = $this->selectBranches($state);
    foreach ($branches as $branch) {
      $branch->evaluate($state);
    }
  }

  /**
   * Asserts no variables (since a conditional is *conditionally* evaluated).
   */
  protected function variableInfoAssertions() {
    return array();
  }

  /**
   * Declares only parent state variables for individual branches.
   *
   * By definition, divergent branches should not have each other's variables.
   */
  protected function stateVariables($element = NULL) {
    return $this->availableVariables();
  }

  /**
   * Provides intersections of variables in all branches, at least one default.
   */
  public function providesVariables() {
    if (!isset($this->providesVariables)) {
      $this->providesVariables = parent::providesVariables();

      if (!$this->isRoot()) {
        // Collect variables.
        $hasDefault = FALSE;
        $childVariables = array();
        /** @var $child RulesConditionalElement */
        $isEmpty = FALSE;
        foreach ($this->children as $child) {
          $hasDefault = $hasDefault || $child->isDefault();
          if ($childProvides = $child->providesVariables()) {
            $childVariables[] = $childProvides;
          }
          else {
            // Mark as empty if any branch does not provide variables. This is
            // to avoid having to perform intersections over empty sets.
            $isEmpty = TRUE;
            break;
          }
        }

        if ($hasDefault && !$isEmpty) {
          // Collect intersection of variable names.
          $names = NULL;
          foreach ($childVariables as $variables) {
            $newNames = array_keys($variables);
            $names = isset($names) ? array_intersect($names, $newNames) : $newNames;
          }
          // Add variables.
          if (isset($names)) {
            foreach ($names as $name) {
              // Determine if variable types are consistent.
              $type = NULL;
              foreach ($childVariables as $variables) {
                if (isset($type) && $type != $variables[$name]['type']) {
                  continue 2;
                }
                else {
                  $type = $variables[$name]['type'];
                }
              }
              // Add compatible variable.
              if (isset($type)) {
                $lastVariables = end($childVariables);
                $this->providesVariables[$name] = $lastVariables[$name];
              }
            }
          }
        }
      }
    }

    return $this->providesVariables;
  }

  /**
   * Selects the branches to evaluate for this conditional.
   *
   * @param RulesState $state
   *   Rules state to use.
   * @return RulesConditionalElement[]
   *   An array of branches to evaluate.
   */
  abstract protected function selectBranches(RulesState $state);

  /**
   * Deletes the container and its children.
   */
  public function delete($keep_children = FALSE) {
    parent::delete($keep_children);
  }

  public function dependencies() {
    $modules = array('rules_conditional' => 1);
    $modules += array_flip(parent::dependencies());
    return array_keys($modules);
  }

  protected function exportSettings() {
    $export = parent::exportSettings();
    // Remove provided variables as plugin is only a container.
    if (isset($export['PROVIDE'])) {
      unset($export['PROVIDE']);
    }
    return $export;
  }

  public function resetInternalCache() {
    parent::resetInternalCache();
    $this->providesVariables = NULL;
  }
}

/**
 * Base conditional element plugin implementation.
 */
abstract class RulesConditionalElement extends RulesActionContainer implements RulesActionInterface {
  /**
   * The parent conditional.
   * @var RulesConditionalContainer
   */
  protected $parent;

  public function label() {
    $info = $this->pluginInfo();
    $label = isset($info['label']) ? $info['label'] : t('unlabeled');
    return $label;
  }

  /**
   * @todo Remove once http://drupal.org/node/1671344 is resolved.
   */
  public function setParent(RulesContainerPlugin $parent) {
    if ($this->parent === $parent) {
      return;
    }
    // Check parent class against the compatible class.
    $pluginInfo = $this->pluginInfo();
    if (empty($pluginInfo['embeddable'])) {
      throw new RulesEvaluationException('This element cannot be embedded.', array(), $this, RulesLog::ERROR);
    }
    elseif (!$parent instanceof $pluginInfo['embeddable']) {
      throw new RulesEvaluationException('The given container is incompatible with this element.', array(), $this, RulesLog::ERROR);
    }

    if (isset($this->parent) && ($key = array_search($this, $this->parent->children, TRUE)) !== FALSE) {
      // Remove element from any previous parent.
      unset($this->parent->children[$key]);
      $this->parent->resetInternalCache();
    }

    // Update parent.
    $this->parent = $parent;
    $parent->children[] = $this;
    $this->parent->resetInternalCache();
  }

  public function providesVariables() {
    $provides = parent::providesVariables();
    if (!$this->isRoot()) {
      foreach ($this->children as $action) {
        $provides += $action->providesVariables();
      }
    }
    return $provides;
  }

  /**
   * Determines whether this branch is default, i.e. covers the remainder of
   * conditions outside of all non-default branches inside the conditional.
   */
  public function isDefault() {
    return FALSE;
  }

  /**
   * Determines whether this branch can be evaluated.
   *
   * Non-default plugins should override this method.
   */
  public function canEvaluate(RulesState $state) {
    return $this->isDefault();
  }

  /**
   * Gets the previous sibling element.
   *
   * @return RulesPlugin
   */
  public function getPreviousSibling() {
    if (isset($this->parent)) {
      $previous = NULL;
      foreach ($this->parent->getIterator() as $element) {
        if ($element === $this) {
          return $previous;
        }
        $previous = $element;
      }
    }
    // Otherwise, return nothing if no previous sibling is applicable.
    return NULL;
  }

  /**
   * Gets the next sibling element.
   *
   * @return RulesPlugin
   */
  public function getNextSibling() {
    if (isset($this->parent)) {
      $previous = NULL;
      foreach ($this->parent->getIterator() as $element) {
        if ($previous === $this) {
          return $element;
        }
        $previous = $element;
      }
    }
    // Otherwise, return nothing if no next sibling is applicable.
    return NULL;
  }

  public function integrityCheck() {
    parent::integrityCheck();
    $this->checkSiblings();
    return $this;
  }

  /**
   * Checks basic conditional element integrity.
   */
  protected function checkSiblings() {
    // Check a default element is the last.
    if ($this->isDefault() && $this->getNextSibling()) {
      throw new RulesIntegrityException(t('The "%plugin" cannot precede another element.', array('%plugin' => $this->plugin())), $this);
    }

    $pluginInfo = $this->pluginInfo();
    // Check dependent element.
    if (!empty($pluginInfo['conditional depends'])) {
      if (!($previous = $this->getPreviousSibling()) || !in_array($previous->plugin(), $pluginInfo['conditional depends'])) {
        $depends = $pluginInfo['conditional depends'];
        $list = t('"%plugin"', array('%plugin' => array_shift($depends)));
        foreach ($depends as $depend) {
          $list = t('!preceding, "%plugin"', array(
            '!preceding' => $list,
            '%plugin' => $depend,
          ));
        }
        throw new RulesIntegrityException(t('The "%plugin" must be preceded by one of: !list.', array('%plugin' => $this->plugin(), '!list' => $list)), $this);
      }
    }
    // Check single element in a conditional container.
    if (!$this->isRoot() && $this->parentElement() instanceof RulesConditionalContainer && !empty($pluginInfo['conditional single'])) {
      $plugin = $this->plugin();
      $previous = $this->getPreviousSibling();
      $next = $this->getNextSibling();
      do {
        if (($previous && $previous->plugin() == $plugin) || ($next && $next->plugin() == $plugin)) {
          throw new RulesIntegrityException(t('The "%plugin" cannot occur more than once within the enclosing container.', array('%plugin' => $this->plugin())), $this);
        }
      }
      while (($previous && $previous = $previous->getPreviousSibling()) || ($next && $next = $next->getNextSibling()));
    }
  }

  /**
   * Deletes the element and its children.
   */
  public function delete($keep_children = FALSE) {
    parent::delete($keep_children);
  }

  public function dependencies() {
    $modules = array('rules_conditional' => 1);
    $modules += array_flip(parent::dependencies());
    return array_keys($modules);
  }

  protected function importChildren($export, $key = NULL) {
    parent::importChildren($export, 'DO');
  }

  protected function exportChildren($key = NULL) {
    return parent::exportChildren('DO');
  }

  protected function exportSettings() {
    $export = parent::exportSettings();
    // Remove provided variables as plugin is only a container.
    if (isset($export['PROVIDE'])) {
      unset($export['PROVIDE']);
    }
    return $export;
  }
}

/**
 * Base conditional element that uses a predicate.
 */
abstract class RulesConditionalPredicateElement extends RulesConditionalElement {
  /**
   * @var RulesCondition
   */
  protected $predicate;

  public function __construct($predicate = NULL, $settings = array()) {
    parent::__construct();
    if (isset($predicate)) {
      $predicate = is_object($predicate) && $predicate instanceof RulesConditionInterface ? $predicate : rules_condition($predicate, $settings);
      $this->setPredicate($predicate);
    }
  }

  /**
   * Sets a condition as predicate.
   */
  protected function setPredicate($predicate) {
    $this->predicate = $predicate;
    $this->predicate->parent = $this;
    // Set up variables with the new parent.
    $this->resetInternalCache();
  }

  public function resetInternalCache() {
    parent::resetInternalCache();
    if (isset($this->predicate)) {
      $this->predicate->resetInternalCache();
    }
  }

  public function __sleep() {
    $array = parent::__sleep();
    $array['predicate'] = 'predicate';
    return $array;
  }

  public function label() {
    $text = '@plugin';
    $variables = array('@plugin' => $this->pluginLabel());
    if (isset($this->predicate)) {
      $text = '@plugin: @label';
      $variables['@label'] = $this->predicate->label();
    }
    return t($text, $variables);
  }

  public function pluginLabel() {
    return parent::label();
  }

  public function integrityCheck() {
    if (!isset($this->predicate)) {
      throw new RulesIntegrityException(t('The conditional "%plugin" does not have a valid predicate.', array('%plugin' => $this->plugin())), $this);
    }
    parent::integrityCheck();
    return $this;
  }

  /**
   * Adds predicate assertions to state.
   */
  protected function stateVariables($element = NULL) {
    if (!isset($element) || $element === $this->predicate) {
      return parent::stateVariables();
    }
    else {
      // Add assertions from the predicate.
      $variables = parent::stateVariables($element);
      if (isset($this->predicate) && $assertions = $this->predicate->call('variableInfoAssertions')) {
        $variables = RulesData::addMetadataAssertions($variables, $assertions);
      }
      return $variables;
    }
  }

  public function dependencies() {
    $modules = array_flip(parent::dependencies());
    if (isset($this->predicate)) {
      $modules += array_flip($this->predicate->dependencies());
    }
    return array_keys($modules);
  }

  /**
   * Negates the predicate.
   */
  public function negate($negate = TRUE) {
    $this->predicate->negate($negate);
    return $this;
  }

  /**
   * Returns whether the predicate is negated.
   */
  public function isNegated() {
    return $this->predicate->isNegated();
  }

  /**
   * Evaluates the predicate.
   */
  public function canEvaluate(RulesState $state) {
    return $this->predicate->evaluate($state);
  }

  /**
   * Imports predicate.
   */
  protected function importChildren($export, $key = NULL) {
    $this->importPredicate($export);
    parent::importChildren($export, 'DO');
  }

  /**
   * Imports predicate.
   */
  protected function importPredicate($export, $key = NULL) {
    if (!isset($key)) {
      $key = strtoupper($this->plugin());
    }
    $predicate = rules_plugin_factory('condition');
    $this->setPredicate($predicate);
    $predicate->import($export[$key]);
  }

  /**
   * Exports predicate with actions.
   */
  protected function exportChildren($key = NULL) {
    $export = $this->exportPredicate();
    return $export + parent::exportChildren('DO');
  }

  /**
   * Exports predicate.
   */
  protected function exportPredicate($key = NULL) {
    $export = array();
    if (!isset($key)) {
      $key = strtoupper($this->plugin());
    }
    if (isset($this->predicate)) {
      $export[$key] = $this->predicate->export();
    }
    return $export;
  }

  protected function exportFlat() {
    return TRUE;
  }
}
