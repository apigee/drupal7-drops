<?php
/**
 * @file
 * Testing implementations.
 */

/**
 * Test base conditional.
 *
 * @method RulesConditional test() test().
 */
class RulesConditionalTestStubContainer extends RulesConditionalContainer {
  protected $itemName = 'stub conditional';

  protected $interceptMethods = array('test');

  /**
   * Selects the branches to evaluate for this conditional.
   *
   * @param RulesState $state
   *   Rules state to use.
   * @return RulesConditionalElement[]
   *   An array of branches to evaluate.
   */
  protected function selectBranches(RulesState $state) {
    // Pick the first element to pass.
    foreach ($this->children as $element) {
      if ($element->canEvaluate($state)) {
        return array($element);
      }
    }
    return array();
  }

  /**
   * Intercepts the 'test' method.
   */
  protected function call_test() {
    throw new Exception('intercept');
  }

  /**
   * Sets a fluent element.
   */
  public function fluent(RulesActionContainer $element) {
    $this->fluentElement = $element;
    return $this;
  }
}

/**
 * Test element.
 */
class RulesConditionalTestStubElement extends RulesConditionalElement {
  protected $itemName = 'stub conditional element';

  protected $pass;

  public function __construct($pass = TRUE) {
    parent::__construct();
    $this->setPass($pass);
  }

  /**
   * Sets whether the element should pass.
   */
  public function setPass($pass) {
    $this->pass = $pass;
  }

  /**
   * Determines whether this branch can be evaluated.
   */
  public function canEvaluate(RulesState $state) {
    return $this->pass;
  }
}

/**
 * Test default element.
 */
class RulesConditionalTestStubDefaultElement extends RulesConditionalElement {
  protected $itemName = 'stub conditional default element';

  public function isDefault() {
    return TRUE;
  }
}

/**
 * Test predicate element.
 */
class RulesConditionalTestStubPredicateElement extends RulesConditionalPredicateElement {
  protected $itemName = 'stub conditional predicate element';
}

/**
 * Test single element.
 */
class RulesConditionalTestStubSingleElement extends RulesConditionalElement {
  protected $itemName = 'stub conditional single element';
}

/**
 * Test dependent element.
 */
class RulesConditionalTestStubDependentElement extends RulesConditionalElement {
  protected $itemName = 'stub conditional dependent element';
}
