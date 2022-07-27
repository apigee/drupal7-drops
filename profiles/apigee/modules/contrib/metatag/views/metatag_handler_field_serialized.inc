<?php

/**
 * @file
 *
 */

/**
 *
 */
class metatag_handler_field_serialized extends metatag_handler_field_entity {

  /**
   * @var bool
   */
  public $uses_default;

  /**
   * The raw value for this field.
   *
   * @var string
   */
  public $raw_value;

  /**
   * {@inheritdoc}
   */
  public function option_definition() {
    $options = parent::option_definition();
    $options['metatag'] = array(
      'contains' => array(
        'use_default' => array('default' => FALSE, 'bool' => TRUE),
        'replace_tokens' => array('default' => FALSE, 'bool' => TRUE),
      ),
    );
    return $options;
  }

  /**
   * {@inheritdoc}
   */
  public function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $form['metatag'] = array(
      '#type' => 'fieldset',
      '#title' => 'Metatag',
      '#collapsible' => TRUE,
    );
    $form['metatag']['use_default'] = array(
      '#type' => 'checkbox',
      '#title' => 'Use default metatag value',
      '#description' => 'Display default value if no value has been defined on entity.',
      '#default_value' => $this->options['metatag']['use_default'],
    );

    $form['metatag']['replace_tokens'] = array(
      '#type' => 'checkbox',
      '#title' => 'Replace tokens',
      '#description' => 'Attempt to replace all tokens with their (entity) values. Note: Tokens in metatags will be replaced before Views tokens.',
      '#default_value' => $this->options['metatag']['replace_tokens'],
    );
  }

  /**
   * {@inheritdoc}
   */
  public function get_value($values, $field = NULL) {
    $value = parent::get_value($values);
    if (!is_null($value)) {
      $value = @unserialize($value);
      $value = $this->get_nested_value($value, $this->definition['path']);
    }
    $this->uses_default = FALSE;
    if (is_null($value) && !empty($this->options['metatag']['use_default'])) {
      $value = $this->get_default_value($values);
      $this->uses_default = TRUE;
    }
    $this->raw_value = $value;
    return $value;
  }

  /**
   * {@inheritdoc}
   */
  public function get_default_value($values) {
    $entity_type = $this->get_entity_type($values);
    $instance = $entity_type;
    $entity = $this->get_entity($values);
    list(, , $bundle) = entity_extract_ids($entity_type, $entity);
    if (!is_null($bundle)) {
      $instance .= ":$bundle";
    }
    $defaults = metatag_config_load_with_defaults($instance);
    return $this->get_nested_value($defaults, $this->definition['path']);
  }

  /**
   * {@inheritdoc}
   */
  public function replace_tokens($value, $values) {
    $entity_type = $this->get_entity_type($values);
    $entity = $this->get_entity($values);
    // Reload the entity object from cache as it may have been altered.
    $token_type = token_get_entity_mapping('entity', $entity_type);
    $options['token data'][$token_type] = $entity;
    $options['entity'] = $entity;

    $value = metatag_get_value($this->definition['metatag'], array('value' => $value), $options);
    return $value;
  }

  /**
   * Retrieves a nested value from an array.
   *
   * @param array $value
   *   An array of meta tag values.
   * @param string $path
   *   The path to search for.
   *
   * @return string|null
   *   The matching string, otherwise NULL.
   */
  public function get_nested_value(array $value, $path) {
    foreach ($path as $index) {
      if (!isset($value[$index])) {
        return NULL;
      }
      $value = $value[$index];
    }
    return $value;
  }

  /**
   * {@inheritdoc}
   */
  public function render($values) {
    $value = (string) $this->get_value($values);
    if (strlen($value) && !empty($this->options['metatag']['replace_tokens'])) {
      $value = $this->replace_tokens($value, $values);
    }
    return $this->sanitize_value($value);
  }

}
