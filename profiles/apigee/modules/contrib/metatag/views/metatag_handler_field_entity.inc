<?php

/**
 * @file
 * Main handler for Metatag data.
 */

/**
 * Work out the entity type based upon its primary table.
 *
 * @param string $table
 *   The table name to check.
 *
 * @return string
 *   The entity type.
 */
function _metatag_entity_type_from_table($table) {
  $base_tables = &drupal_static(__FUNCTION__);
  if (empty($base_tables)) {
    foreach (entity_get_info() as $entity_type => $info) {
      $base_tables[$info['base table']] = $entity_type;
    }
  }
  return isset($base_tables[$table]) ? $base_tables[$table] : NULL;
}

/**
 * Class metatag_handler_entity.
 *
 * Uses modified code from views_handler_field_field to reliably provide the
 * current field's base table as well as entity_type and entity_id.
 */
class metatag_handler_field_entity extends views_handler_field {

  /**
   * @var string
   */
  public $entity_type_alias;

  /**
   * @var string
   */
  public $entity_id_alias;

  /**
   * Set the base_table and base_table_alias.
   *
   * Copy of views_handler_field_field::get_base_table().
   *
   * @return string
   *   The base table which is used in the current view "context".
   */
  public function get_base_table() {
    $base_table = $this->view->base_table;
    // If the current field is under a relationship you can't be sure that the
    // base table of the view is the base table of the current field.
    // For example a field from a node author on a node view does have users as
    // base table.
    if (!empty($this->options['relationship']) && $this->options['relationship'] != 'none') {
      $relationships = $this->view->display_handler->get_option('relationships');
      if (!empty($relationships[$this->options['relationship']])) {
        $options = $relationships[$this->options['relationship']];
        $data = views_fetch_data($options['table']);
        $base_table = $data[$options['field']]['relationship']['base'];
      }
    }
    return $base_table;
  }

  /**
   * Heavily modified code from views_hander_field_field::query().
   *
   * By default, the only columns added to the query are entity_id and
   * entity_type. This is because other needed data is fetched by entity_load().
   */
  public function query() {
    parent::query();

    $base_table = $this->get_base_table();
    $base_table_alias = isset($this->relationship) ? $this->relationship : $base_table;

    $entity_type = _metatag_entity_type_from_table($base_table);
    $entity_info = entity_get_info($entity_type);

    $this->entity_id_alias = $this->query->add_field($base_table_alias, $entity_info['entity keys']['id']);

    // The alias needs to be unique, so we use both the field table and the
    // entity type.
    // @todo Probably not necessary for this use case?
    $entity_type_alias = $this->table_alias . '_' . $entity_type . '_entity_type';
    $this->entity_type_alias = $this->query->add_field(NULL, "'$entity_type'", $entity_type_alias);
  }

  /**
   * {@inheritdoc}
   */
  public function get_entity_type($values) {
    return isset($values->{$this->entity_type_alias}) ? $values->{$this->entity_type_alias} : NULL;
  }

  /**
   * {@inheritdoc}
   */
  public function get_entity($values) {
    $entity_type = $this->get_entity_type($values);
    $entity_id = isset($values->{$this->entity_id_alias}) ? $values->{$this->entity_id_alias} : NULL;
    if (!is_null($entity_id)) {
      $entities = entity_load($entity_type, array($entity_id));
      return reset($entities);
    }
    return NULL;
  }

}
