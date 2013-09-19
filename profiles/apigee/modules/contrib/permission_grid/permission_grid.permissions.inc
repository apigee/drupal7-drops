<?php
/**
 * @file permission_grid.permissions.inc
 * Contains hook implementations on behalf of other modules.
 */

// =========================================================== Core modules

/**
 * Implements hook_permission_grid_info() on behalf of node module.
 */
function node_permission_grid_info() {
  $return = array(
    'node' => array(
      'label' => t('Content type'),
      'objects' => array(),
      'verb_groups' => array(
        'core' => array(
          'pattern' => '%verb %object content',
          'verbs' => array(
            'create'      => t('Create'),
            'edit own'    => t('Edit own'),
            'edit any'    => t('Edit any'),
            'delete own'  => t('Delete own'),
            'delete any'  => t('Delete any'),
          ),
        ),
      ),
    ),
  );

  $node_types = node_type_get_types();
  $configured_types = node_permissions_get_configured_types();
  foreach ($configured_types as $type) {
    $return['node']['objects'][$type] = $node_types[$type]->name;
  }

  return $return;
}

/**
 * Implements hook_permission_grid_info() on behalf of taxonomy module.
 */
function taxonomy_permission_grid_info() {
  $vocabularies = taxonomy_get_vocabularies();
  $return = array(
    'vocabulary' => array(
      'label' => t('Taxonomy'),
      'objects' => array(),
      'verb_groups' => array(
        'core' => array(
          'pattern' => '%verb terms in %object',
          'verbs' => array(
            'edit'    => t('Edit terms'),
            'delete'  => t('Delete terms'),
          ),
          // Oh yeah, taxonomy module, you're so cool you get a special thing
          // just for you. And by 'cool' I mean 'still in D6 land'.
          'object_process_callback' => 'permission_grid_taxonomy_object_process',
        )
      ),
    ),
  );

  foreach ($vocabularies as $vocabulary) {
    $return['vocabulary']['objects'][$vocabulary->machine_name] = $vocabulary->name;
  }

  return $return;
}

/**
 * Object process callback for taxonomy vocabularies.
 *
 * This handles the weirdness with taxonomy permissions using vocabulary ids
 * rather than machine names.
 */
function permission_grid_taxonomy_object_process($object_name) {
  $vocabulary_names = taxonomy_vocabulary_get_names();
  return $vocabulary_names[$object_name]->vid;
}

// =========================================================== Contrib modules

/**
 * Implements hook_permission_grid_info_alter() on behalf of vppr module.
 */
function vppr_permission_grid_info_alter(&$info) {
  $info['vocabulary']['verb_groups']['vppr'] = array(
    'pattern' => '%verb %object vocabulary terms',
    'verbs' => array(
      'administer' => t('Administer terms'),
    ),
  );
}

/**
 * Implements hook_permission_grid_info() on behalf of profile2 module.
 */
function profile2_permission_grid_info() {
  $return = array(
    'profile2' => array(
      'label' => t('Profiles'),
      'objects' => array(),
      'verb_groups' => array(
        'core' => array(
          'pattern' => '%verb %object profile',
          'verbs' => array(
            'edit own'  => t('Edit own'),
            'edit any'  => t('Edit any'),
            'view own'  => t('View own'),
            'view any'  => t('View any'),
          ),
        ),
      ),
    ),
  );

  foreach (profile2_get_types() as $type => $type_info) {
    $return['profile2']['objects'][$type] = $type_info->label;
  }

  return $return;
}

/**
 * Implements hook_permission_grid_info() on behalf of commerce module.
 */
function commerce_permission_grid_info() {
  $info = array();
  
  // @todo: other commerce entity types.
  foreach (array('commerce_product') as $entity_type) {
    $entity_info = entity_get_info($entity_type);
    $types = array();
    foreach ($entity_info['bundles'] as $bundle => $bundle_info) {
      $types[$bundle] = $bundle_info['label'];
    }

    $info[$entity_type] = array(
      'label' => $entity_info['label'],
      'objects' => $types,
      'verb_groups' => array(
        // Put plural first so 'create' comes first.
        'commerce_plural' => array(
          'pattern' => "%verb $entity_type entities of bundle %object",
          'verbs' => array(
            'create' => t('Create'),
            'edit own' => t('Edit own'),
            'view own' => t('View own'),
          ),
        ),
        'commerce_singular' => array(
          'pattern' => "%verb $entity_type entity of bundle %object",
          'verbs' => array(
            'edit any' => t('Edit any'),
            'view any' => t('View any'),
          ),
        ),
      ),
    );
  }
  return $info;
}

/**
 * Implements hook_permission_grid_info() on behalf of flag module.
 */
function flag_permission_grid_info() {
  $return = array(
    'flag' => array(
      'label' => t('Flag'),
      'objects' => array(),
      'verb_groups' => array(
        'flag' => array(
          'pattern' => '%verb %object',
          'verbs' => array(
            'flag'      => t('Flag'),
            'unflag'    => t('Unflag'),
          ),
        ),
      ),
    ),
  );

  foreach (flag_get_flags() as $flag) {
    $return['flag']['objects'][$flag->name] = $flag->title;
  }

  return $return;
}
