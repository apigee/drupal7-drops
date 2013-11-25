<?php
/**
 * @file
 * Rules integration file.
 */

/**
 * Implements hook_rules_plugin_info().
 */
function rules_conditional_rules_plugin_info() {
  return array(
    'conditional' => array(
      'label' => t('Conditional', array(), array('context' => 'conditional rules')),
      'class' => 'RulesConditional',
      'embeddable' => 'RulesActionContainer',
      'extenders' => array(
        'RulesPluginUIInterface' => array(
          'class' => 'RulesConditionalEmptyUI',
        ),
      ),
    ),
    'if' => array(
      'label' => t('If', array(), array('context' => 'conditional rules')),
      'class' => 'RulesConditionalIf',
      'embeddable' => 'RulesConditional',
      'extenders' => array(
        'RulesPluginUIInterface' => array(
          'class' => 'RulesConditionalPredicateUI',
        ),
      ),
      'import keys' => array('IF', 'ELSE IF'),
    ),
    'else' => array(
      'label' => t('Else', array(), array('context' => 'conditional rules')),
      'class' => 'RulesConditionalElse',
      'embeddable' => 'RulesConditional',
      'conditional single' => TRUE,
      'conditional depends' => array('if'),
      'extenders' => array(
        'RulesPluginUIInterface' => array(
          'class' => 'RulesConditionalEmptyUI',
        ),
      ),
    ),
    'switch' => array(
      'label' => t('Switch', array(), array('context' => 'conditional rules')),
      'class' => 'RulesConditionalSwitch',
      'embeddable' => 'RulesActionContainer',
      'extenders' => array(
        'RulesPluginUIInterface' => array(
          'class' => 'RulesConditionalPluginUI',
        ),
      ),
    ),
    'case' => array(
      'label' => t('Case', array(), array('context' => 'conditional rules')),
      'class' => 'RulesConditionalCase',
      'embeddable' => 'RulesConditionalSwitch',
      'extenders' => array(
        'RulesPluginUIInterface' => array(
          'class' => 'RulesConditionalCaseUI',
        ),
      ),
    ),
    'default case' => array(
      'label' => t('Default case', array(), array('context' => 'conditional rules')),
      'class' => 'RulesConditionalDefaultCase',
      'embeddable' => 'RulesConditionalSwitch',
      'conditional single' => TRUE,
      'conditional depends' => array('case'),
      'extenders' => array(
        'RulesPluginUIInterface' => array(
          'class' => 'RulesConditionalEmptyUI',
        ),
      ),
    ),
    'while' => array(
      'label' => t('While', array(), array('context' => 'conditional rules')),
      'class' => 'RulesConditionalWhile',
      'embeddable' => 'RulesActionContainer',
      'extenders' => array(
        'RulesPluginUIInterface' => array(
          'class' => 'RulesConditionalWhileUI',
        ),
      ),
    ),
    'rule condition set' => array(
      'label' => t('Condition set (rule)', array(), array('context' => 'conditional rules')),
      'class' => 'RuleConditionSet',
      'embeddable' => FALSE,
      'component' => TRUE,
      'extenders' => array(
        'RulesPluginUIInterface' => array(
          'class' => 'RuleConditionSetUI',
        ),
      ),
    ),
  );
}
