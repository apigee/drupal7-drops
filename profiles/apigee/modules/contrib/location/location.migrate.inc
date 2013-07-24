<?php
/**
 * @file
 * Migrate integration.
 *
 * Primary value passed to this field is the is_primary value (boolean)
 * but can be overwritten with subfields.
 *
 * Subfields are used to specify all the other values:
 *  'street'
 *  'additional',
 *  'city',
 *  'province',
 *  'postal_code',
 *  'country',
 *  'is_primary',
 *  'latitude',
 *  'longitude'
 *
 * @code
 *   $this->addFieldMapping('field_location_dest')->defaultValue('TRUE');
 *   $this->addFieldMapping('field_location_dest:street', 'field_source_address_1');
 *   $this->addFieldMapping('field_location_dest:city', 'field_source_city');
 * @endcode
 */
class MigrateLocationFieldHandler extends MigrateFieldHandler {

  public function __construct() {
    $this->registerTypes(array('location'));
  }

  /**
   * Implementation of MigrateFieldHandler::fields().
   *
   * @param $type
   *  The file field type - 'file' or 'image'
   * @param $parent_field
   *  Name of the parent field.
   * @param Migration $migration
   *  The migration context for the parent field. We can look at the mappings
   *  and determine which subfields are relevant.
   * @return array
   */
  public function fields($type, $parent_field, $migration = NULL) {

    $fields = array(
      'name'        => t('Subfield: String to be used as the name value'),
      'street'      => t('Subfield: String to be used as the street value'),
      'additional'  => t('Subfield: String to be used as the additional value'),
      'city'        => t('Subfield: String to be used as the city value'),
      'province'    => t('Subfield: String to be used as the province value'),
      'postal_code' => t('Subfield: String to be used as the postal code value'),
      'latitude'    => t('Subfield: Float to be used as the latitude value'),
      'longitude'   => t('Subfield: Float to be used as the longitude value'),
      'country'     => t('Subfield: String (ISO code) to be used as the country value'),
      'source'      => t('Subfield: String to be used as the source value'),
      'is_primary'  => t('Subfield: Boolean to be used as the is primary value'),
    );
    return $fields;
  }

  public function prepare($entity, array $field_info, array $instance, array $values) {
    $migration   = Migration::currentMigration();
    $destination = $migration->getDestination();

    $arguments = array();
    if (isset($values['arguments'])) {
      $arguments = $values['arguments'];
      unset($values['arguments']);
    }

    $language = $this->getFieldLanguage($entity, $field_info, $arguments);
    $return = array($language => array());

    foreach ($values as $delta => $value) {
      // Handle potentially multiple arguments.
      $instance_arguments = array();

      foreach ($arguments as $key => $argument) {
        // For a scalar argument, pass it directly.
        if (!is_array($argument)) {
          if ($delta == 0) {
            $instance_arguments[$key] = $argument;
          }
        }
        elseif (isset($argument[$delta])) {
          $instance_arguments[$key] = $argument[$delta];
        }
      }

      if (isset($instance_arguments['country'])) {
        $instance_arguments['country'] = strtolower($instance_arguments['country']);
      }

      // Ensure we have at least one location field besides 'is_primary' before saving
      if (!empty($instance_arguments)) {
        $instance_arguments['is_primary'] = $value;

        // Only save the location if saved successfully.
        if ($lid = location_save($instance_arguments)) {
          $return[$language][$delta]['lid'] = $lid;
        }
      }
    }

    return $return;
  }

}
