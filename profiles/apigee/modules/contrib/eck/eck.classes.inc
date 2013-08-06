<?php

class DBObject implements Iterator{
  //Wheteher this object was loaded or just created
  public $is_new;
  //Iterator variable
  private $position;
  
  //The database table where the objects exist
  private $table;
  private $vars;
  private $data;
  
  private $primary_keys;
  private $serialize;
  
  protected function __construct($table){
    $this->serialize = array();
    $this->is_new = TRUE;
    //Iterator variable
    $this->position = 0;
    
    $data = array();
    //is this a real table, check it
 
    
    if($schema = drupal_get_schema($table)){
      $this->table = $table;
      $this->primary_keys = $schema["primary key"];
      $this->vars = array_keys($schema['fields']);
      
      //do we want to handle searialized variables by default? let's do it
      //and wait for some critizism
      foreach($schema['fields'] as $name => $field){
        if(array_key_exists('serialize', $field) && $field['serialize']){
          $this->serialize[] = $name;
        }
      }
      foreach($this->vars as $var){
        if($schema['fields'][$var]['type'] != "serial"){
          $this->data[$var] = NULL;
        }
      };
    }else{
      //@todo throw an exception
    }
  }
  
  function __set($var, $value){
    if(in_array($var, $this->vars)){
      $this->data[$var] = $value;
    }
  }
  
  function __get($var){
    if(property_exists($this, $var)){
      return $this->{$var};
    }
    
    return $this->data[$var];
  }
  
  public function __isset($name) {
    return isset($this->data[$name]);
  }

    /**  As of PHP 5.1.0  */
  public function __unset($name) {
    unset($this->data[$name]);
  }
  
  //DB Interaction Functions
  public function save(){
    
    //before we save, lets serialize the properties that require it
    foreach($this->serialize as $property){
   
      $this->{$property} = drupal_json_encode($this->{$property});
    }
    
   
    
    if($this->is_new){
      $this->id =
      db_insert($this->table)
      ->fields($this->data)
      ->execute();
    }else{
      //well I need to know what the primary id is to set up the condition;
      $primary_key = $this->primary_keys[0];
      db_update($this->table)
      ->condition($primary_key, $this->{$primary_key}, '=')
      ->fields($this->data)
      ->execute();
    }
    
    //now that we are done saving lets deserialize in case that for some
    //reason we will continue manipulating the properties
    foreach($this->serialize as $property){
      $this->{$property} = drupal_json_decode($this->{$property});
    }
    
    $this->is_new = FALSE;
  }
  
  protected function load($property, $value){
    $result = 
    db_select($this->table, 't')
    ->fields('t')
    ->condition($property, $value,'=')
    ->execute()
    ->fetchAssoc();
    
    if($result){
      foreach($result as $property => $value){
        if(in_array($property, $this->serialize)){
          $value = drupal_json_decode($value);
        }
        $this->{$property} = $value;
      }
      //we should only set the is_new flag as false if we loaded something
      $this->is_new = FALSE;
    }
    
  }
  
  public function delete(){
    //we can only deleted if its a loaded object, or if it has been saved
    if(!$this->is_new){
      $query = db_delete($this->table);
      $primary_key = $this->primary_keys[0];
      $query->condition($primary_key, $this->{$primary_key}, '=');
      $query->execute();
      //Should we delete the data from the object.. not sure.
      //for right now lets just set it back to new
      $this->is_new = TRUE;
    }
    
  }
  
  //Iterator Interface Functions
  function rewind() {
      $this->position = 0;
  }

  function current() {
      return $this->data[$this->key()];
  }

  function key() {
      return $this->vars[$this->position];
  }

  function next() {
      ++$this->position;
  }

  function valid() {
    if(in_array($this->position, array_keys($this->vars))){
      return TRUE;
    }else{
      return FALSE;
    }
  }
}

class EntityType extends DBObject{
  
  //If an entity type is new, we can create its table from the current data of
  //the object, but if this is a loaded object, we need to actually keep
  //track of the changes happening so we can modify the already existing table
  //appropiately.
  private $changes;
  
  public function __construct(){
    parent::__construct('eck_entity_type');
    $this->properties = array();
    $this->changes = array();
  }
  
  public function addProperty($name, $label, $type, $behavior = NULL){
    if(!$this->is_new){
      $this->recordFieldChange('add', $name);
    }
    
    $p = $this->properties;
    //@todo check that type is an actual type
    $p[$name] = array('label' => $label, 'type' => $type, 'behavior' => $behavior);
    
    $this->properties = $p;
  }
  
  public function removeProperty($name){
    $p = $this->properties;
    if(array_key_exists($name, $p)){
      unset($p[$name]);
      $this->properties = $p;
      if(!$this->is_new){
        $this->recordFieldChange('remove', $name);
      }
    }
  }
  
  public function changeBehavior($name, $behavior){
    $p = $this->properties;
    //@todo check that type is an actual type
    if(array_key_exists($name, $p)){
      $p[$name]['behavior'] = $behavior;
      //@todo look at this more closelly, does the behavior change really affect the property
      //cache?
      entity_property_info_cache_clear();
    }else{
      //@Todo add exception.. the property does not exist
    }
    
    $this->properties = $p;
  }
  
  public function removeBehavior($name){
    $this->changeBehavior($name, NULL);
  }
  
  private function recordFieldChange($op, $name){
    //If it is not new we need to keep track of stuff
    if(!$this->is_new){
      $p = $this->properties;
      $c = $this->changes;
      switch ($op) {
        case 'add':
          //if the property does not exist already add keep track
          if(!array_key_exists($name, $p)){
            $c[$op][] = $name;
          }
        break;
        
        case 'remove':
          //if there is an add in the changes take it out, otherwise add a
          //remove
          if(array_key_exists('add', $c)){
            
            $key = array_search($name, $c['add']);
            if($key != FALSE){
              unset($c['add'][$key]);
            }     
          }else{
            $c[$op][] = $name;
          }
        break;
      }
      $this->changes = $c;
    }
  }
  
  public function save(){
    if($this->is_new){
      module_load_include('inc', 'eck', 'eck.entity_type');
      $schema = eck__entity_type__schema($this);
      db_create_table("eck_{$this->name}", $schema);

      // Allow other modules to respond to the creation of entity types.
      module_invoke_all('eck_entity_type_insert', $this);

    }else{
      //modify the already existing table in accordance with the recorded changes
      if(array_key_exists('add', $this->changes)){
        foreach($this->changes['add'] as $name){
          //first lets get the record 
          $properties = $this->properties;
          $property = $properties[$name];
          //now we check to see whether it is a default or a custom property
          //it is not custom so lets get the schema and add the field
          $schema = eck_property_type_schema($property['type']);
          db_add_field("eck_{$this->name}", $name, $schema);
        }
      }
      if(array_key_exists('remove', $this->changes)){
        foreach($this->changes['remove'] as $name){
          db_drop_field("eck_{$this->name}", $name);
        }
      }

      // Allow other modules to respond to the change of entity types.
      module_invoke_all('eck_entity_type_update', $this);
    }
    
    parent::save();
    drupal_get_schema(NULL, TRUE);
  }
  
  public function delete(){
    parent::delete();
    db_drop_table('eck_'.$this->name);

    // Allow other modules to respond to the deletion of entity types.
    module_invoke_all('eck_entity_type_delete', $this);

    drupal_flush_all_caches();
  }
  
  public static function loadByName($name){
    $self = new EntityType();
    $self->load('name', $name);
    return $self;
  }
  
  public static function loadAll(){
    $results = db_select('eck_entity_type', 't')
    ->fields('t', array('name'))
    ->execute();
    
    $entity_types = array();
    
    foreach($results as $result){
      $name = $result->name;
      $entity_types[] = EntityType::loadByName($name);
    }
    return $entity_types;
  }
}

class Bundle extends DBObject{
  
  public function __construct(){
    parent::__construct('eck_bundle');
    $this->config = array();
  }
  
  private function createMachineName(){
    $this->machine_name = "{$this->entity_type}_{$this->name}";
  }
  
  private function createLabel(){
    $name = $this->name;
    $pieces = explode("_", $name);
    $final = array();
    foreach($pieces as $piece){
      $final[] = ucfirst($piece);
    }
    
    $this->label = implode(" ", $final);
  }
  
  public function save(){
    //Lets do some checks before the bundle is saved
    if(isset($this->entity_type) && isset($this->name)){
     
      $save = TRUE;
      //we are good
      //@todo we should check that the entity type is a proper
      //entity type object
      
      //Lets set the machine name
      $this->createMachineName();
      
      //if this bundle is_new we need to check that it does not exist
      //@todo we just need to change the field in the db to be unique
      if($this->is_new){
        $bundle = Bundle::loadByMachineName($this->machine_name);
        if(!$bundle->is_new){
          $save = FALSE;
        }
      }
      
      if(!isset($this->label)){
        $this->createLabel();
      }
      
      if($save){
        parent::save();
      }else{
        //@todo throw some error
      }
      
    }else{
      //if the name an entity type are not set, we can not save
      //the bundle
      //@todo throw soem error or exception
    }
  }
  
  /**
   * This method returns a bundle object
   * @param $machine_name
   *  (String) A string composed of the entity type name and the bundle name
   *  "{$entity_type_name}_{$bundle_name}"
   */
  public static function loadByMachineName($machine_name){
    $self = new Bundle();
    $self->load('machine_name', $machine_name);
    return $self;
  }
  
  public static function loadAll(){
    //@todo move this to a general function
    $results = db_select('eck_bundle', 't')
    ->fields('t', array('machine_name'))
    ->execute();
    
    $bundles = array();
    
    foreach($results as $result){
      $name = $result->machine_name;
      $bundles[] = Bundle::loadByMachineName($name);
    }
    return $bundles;
  }
  
  public static function loadByEntityType($entity_type){
    //@todo move this to a general function
    $results = db_select('eck_bundle', 't')
    ->fields('t', array('name'))
    ->condition('entity_type', $entity_type->name, '=')
    ->execute();
    
    $bundles = array();
    
    foreach($results as $result){
      $name = $result->name;
   
      $bundles[] = Bundle::loadByMachineName("{$entity_type->name}_{$name}");
    }
    return $bundles;
  }

 /**
  * Adds a field to this bundle.
  *
  *
  * @param $field_type
  *   The type of field to add. One of the keys as defined by any field module using hook_field_info.
  * 
  * @param $options
  *   This is an optional array. Its properties can include:
  *   - use existing: If TRUE and if a 'field_name' property is specified in the 'field'
  *     property below and the field already exists, then a new instance will be created
  *     using the existing field. All specified 'field' options provided other then the field
  *     name will be ignored. If FALSE, and an existing field is found then a new field_name
  *     will be generated. TRUE by default.
  *   - field: all options accepted by field_create_field(). Defaults will be used for each
  *     property that is omitted. Most defaults come from field_create_field().
  *     Default 'field_name' generation:
  *     - field_name: 'field_' + field type + #, where # is one more then the number of fields
  *       of that type that already exist active or inactive.
  *   - instance: all options accepted by field_create_instance(). Defaults will be used for
  *     each property that is omitted. 'bundle' and 'entity_type' properties are ignored because
  *     they come from the bundle info. The field_name property is either generated or taken from
  *     the field properties. 
  *
  * @return
  *   The $instance array with the id property filled in as returned by field_create_instance().
  *
  * @throws FieldException
  *
  * See: @link field Field API data structures @endlink.
  */
  public function addField($field_type, $options = array()) {
    // Check that the field type is known.
    $field_info = field_info_field_types($field_type);
    if (!$field_info) {
      throw new FieldException(t('Attempt to add a field of unknown type %type.', array('%type' => $field_type)));
    }

    // By default use an existing field if one is found.
    $options += array('use existing' => TRUE);
    // Set field options and merge in any provided field settings.
    $field = array('type' => $field_type);
    if (!empty($options['field'])) {
      $field += $options['field'];
    }

    // Retrieve existing fields of this type.
    $field_type_fields = field_read_fields(array('type' => $field_type), array('include_inactive' => TRUE));

    // Formulate a default field name.
    if (empty($field['field_name']) || (isset($field_type_fields[$field['field_name']]) && !$options['use existing'])) {
      $iter = count($field_type_fields) + 1;
      $field += array('field_name' => substr('field_' . $field_type, 0, 28) . '_' . $iter);
    }

    // Create a new field if the field name is unique over active and disabled fields.
    if (!isset($field_type_fields[$field['field_name']])) {
      field_create_field($field);
    }

    // Add an instance of the field to this bundle.
    $instance = array(
      'field_name' => $field['field_name'],
      'entity_type' => $this->entity_type,
      'bundle' => $this->name,
    );
    // Merge any provided properties and settings.
    if(array_key_exists('instance', $options)){
      $instance += $options['instance'];
    }
    return field_create_instance($instance);
  }
}
