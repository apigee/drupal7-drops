<?php
/**
 * @file
 * Defines developer-based views.
 */

/**
 * Implements hook_views_data().
 */
function devconnect_user_views_data() {

  $base_table = 'dc_dev';
  $attrib_table = 'dc_dev_attributes';

  $data[$base_table] = array(
    'table' => array(
      'group' => t('Developers'),
      'base' => array(
        'field' => 'developer_id', // primary key
        'title' => t('Developers'),
        'help' => t('Contains cached data related to developers.'),
      )
    ),
    'developer_id' => array(
      'title' => t('Developer ID'),
      'help' => t('Universally unique identifier for a developer'),
      'field' => array('handler' => 'views_handler_field'),
      'sort' => array('handler' => 'views_handler_sort'),
      'filter' => array('handler' => 'views_handler_filter_string'),
      'argument' => array('handler' => 'views_handler_argument_string'),
    ),
    'uid' => array(
      'title' => t('User ID'),
      'help' => t('User ID of the Drupal user corresponding to the Developer'),
      'field' => array('handler' => 'views_handler_field_numeric'),
      'sort' => array('handler' => 'views_handler_sort'),
      'filter' => array('handler' => 'views_handler_filter_numeric'),
      'argument' => array('handler' => 'views_handler_argument_numeric'),
      'relationship' => array(
        'base' => 'users', // The name of the table to join with.
        'base field' => 'uid', // The name of the field on the joined table.
        'handler' => 'views_handler_relationship',
        'label' => t('Users'),
        'title' => t('Users'),
        'help' => t('Relates Edge developers to users'),
      ),
    ),
    // This dummy field is only here for the purpose of relating
    // dc_dev table to dc_dev_app.
    'app_uid' => array(
      'real field' => 'uid',
      'relationship' => array(
        'base' => 'dc_dev_app',
        'base field' => 'uid',
        'handler' => 'views_handler_relationship',
        'label' => t('Developer Apps'),
        'title' => t('Developer Apps'),
        'help' => t('Relates developers to their apps'),
      ),
    ),
    'status' => array(
      'title' => t('Status'),
      'help' => t('Indicates whether a Developer is in active or inactive status'),
      'field' => array('handler' => 'views_handler_field_boolean'),
      'sort' => array('handler' => 'views_handler_sort'),
      'filter' => array('handler' => 'views_handler_filter_boolean_operator'),
      'argument' => array('handler' => 'views_handler_argument_numeric'),
    ),
  );

  $data[$attrib_table] = array(
    'table' => array('group' => t('Developers')),
    'attribute_name' => array(
      'real field' => 'name',
      'title' => t('Attribute Name'),
      'help' => t('Name of the developer attribute'),
      'field' => array('handler' => 'views_handler_field'),
      'sort' => array('handler' => 'views_handler_sort'),
      'filter' => array('handler' => 'views_handler_filter_string'),
      'argument' => array('handler' => 'views_handler_argument_string'),
    ),
    'attribute_value' => array(
      'real field' => 'value',
      'title' => t('Attribute Value'),
      'help' => t('Value of the developer attribute'),
      'field' => array('handler' => 'views_handler_field'),
      'sort' => array('handler' => 'views_handler_sort'),
      'filter' => array('handler' => 'views_handler_filter_string'),
      'argument' => array('handler' => 'views_handler_argument_string'),
    ),
    'attribute_filter' => array(
      'real field' => 'value',
      'title' => t('Attribute'),
      'help' => t('Value for a named attribute'),
      'filter' => array('handler' => 'du_handler_filter_attribute'),
    ),
  );

  $data[$attrib_table]['table']['join'][$base_table] = array(
    'left_field' => 'developer_id',
    'field' => 'developer_id',
  );

  return $data;
}

/**
 * Implements hook_views_pre_render().
 *
 * This heinous abomination makes the developer_save_queue view present much
 * more meaningful data.
 */
function devconnect_user_views_post_render(&$view, &$output, &$cache) {
  if ($view->name == 'developer_save_queue') {
    $dom = new DOMDocument();
    $dom->loadHTML($output);
    $list = $dom->getElementsByTagName('td');
    foreach ($list as $elem) {
      if ($elem->hasAttribute('class')) {
        $classes = explode(' ', $elem->getAttribute('class'));
        $class = end($classes);
        $text = trim($elem->nodeValue);
        switch ($class) {
          case 'views-field-username': // username

            $uid = db_select('users', 'u')->fields('u', array('uid'))->condition('name', $text)->execute()->fetchField();
            if ($uid) {
              $a = $dom->createElement('a');
              $a->setAttribute('href', url('user/' . $uid));
              $t = $dom->createTextNode($text);
              $a->appendChild($t);
              $elem->nodeValue = '';
              $elem->appendChild($a);
            }

            break;

          case 'views-field-retries': // retry count
            if (is_numeric($text)) {
              $elem->nodeValue = (string) variable_get('developer_queue_retries_' . $text, 0);
            }
            break;
        }
      }
    }

    $output = $dom->saveHTML();
  }
}
