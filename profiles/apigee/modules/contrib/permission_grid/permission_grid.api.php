<?php

/**
 * @file
 * This file contains no working PHP code; it exists to provide additional
 * documentation for doxygen as well as to document hooks in the standard
 * Drupal manner.
 */

/**
 * @addtogroup hooks
 * @{
 */

/**
 * Declare this module's structured permissions.
 *
 * A set of structured permissions is defined as a list of objects and a list
 * of verbs. The permissions are created by combining these. For example, the
 * list of node types and the verbs (create, edit, delete) form a grid of
 * permissions for operations on all the node types.
 *
 * This hook may be placed in the file MODULE.permissions.inc.
 *
 * @return
 *  An array of data for object types that have multiple permissions associated
 *  with them. The array keys are object machine names, eg, 'node' (though note
 *  these need not be entities).
 *  Values are arrays with the following properties:
 *    - 'label': The human-readable label of the object type.
 *    - 'objects': An array of the different objects of this type that have
 *      their own permissions, for example, node types, which each provide
 *      permissions such as 'create foo node types'. These will form the rows 
 *      of the permissions grid. The keys are the substrings of the permission
 *      strings, and the values are human-readable labels.
 *    - 'verb_groups': An array of one or more verb groups. Each verb group
 *      defines a list of verbs and a pattern for the permissions using those
 *      verbs. The verb groups keys are arbitrary (but using the name of the
 *      providing module is a good idea for ease of alterability), and each
 *      array has the following properties:
 *      - 'verbs': An array of verbs. The keys are the 'machine names', that is,
 *        the strings that are replaced into the pattern. The values may be
 *        human-readable labels. Note that verb machine names must be unique
 *        across all the verb groups for the object type.
 *      - 'pattern': The pattern of the permissions machine name, with the
 *        following replacements:
 *        - '%verb': The verb of the permission. This takes all the keys in the
 *          verbs array in this verb group.
 *        - '%object': The object of the permission. This takes all the keys in
 *          the objects array.
 *      - 'object_process_callback' (optional) A function to process the object
 *        name before replacing it into the pattern. (This is just a hack for
 *        taxonomy to work.) 
 */
function hook_permission_grid_info() {
  $return = array(
    'node' => array(
      'label' => t('Content type'),
      'objects' => array(),
      'verb_groups' => array(
        'node' => array(
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
 * Alter the structured permission info defined by other modules.
 */
function hook_permission_grid_info_alter(&$info) {
  // Add a 'publish own node' verb.
  $info['node']['verb_groups']['node']['verbs']['publish own'] = t('Publish own');
}
