<?php
/**
 * @file
 * Admin forms for line items.
 */

/**
 * Form callback: builds the form to delete orphaned line items.
 */
function commerce_line_item_ui_orphaned_line_items_form($form, &$form_state) {
  $count = commerce_line_item_ui_orphans_count();

  if ($count > 0) {
    $description = format_plural($count, 'Your database currently contains 1 line item that is not attached to any order.', 'Your database currently contains @count line items that are not attached to any order.');
    return confirm_form($form, t('Are you sure you want to delete your orphaned line items?'), 'admin/commerce/config', $description, t('Delete'));
  }
  else {
    return array(
      'text' => array(
        '#markup' => t('You do not have any orphaned line items in your database.') . '<p>' . t('Line items are considered orphaned when they are not referenced by any order. This typically happens when line items are added on the create order form but the form is not submitted.'),
      ),
    );
  }
}

/**
 * Form callback: submits the form to delete orphaned line items.
  */
function commerce_line_item_ui_orphaned_line_items_form_submit($form, &$form_state) {
  $batch = array(
    'title' => t('Deleting orphaned line items'),
    'operations' => array(
      array(
        'commerce_line_item_ui_delete_orphaned_line_items',
        array(),
      ),
    ),
    'finished' => 'commerce_line_item_ui_delete_orphaned_line_items_finished',
    'file' => drupal_get_path('module', 'commerce_line_item') . '/includes/commerce_line_item_ui.admin.inc',
  );
  batch_set($batch);
}

/**
 * Returns the number of orphaned line items in the database.
 */
function commerce_line_item_ui_orphans_count() {
  $query = db_select('commerce_line_item', 'cli')
    ->fields('cli', array('line_item_id'));
  $order_alias = $query->leftJoin('commerce_order', 'co', '%alias.order_id = cli.order_id');
  return $query->isNull("$order_alias.order_id")
    ->countQuery()
    ->execute()
    ->fetchField();
}

/**
 * Batch callback: deletes 50 orphaned line items at a time.
 */
function commerce_line_item_ui_delete_orphaned_line_items(&$context) {
  if (empty($context['sandbox'])) {
    $context['sandbox'] = array();
    $context['sandbox']['progress'] = 0;
    $context['sandbox']['current_line_item_id'] = 0;
    $context['results'] = array();
    $context['sandbox']['max'] = commerce_line_item_ui_orphans_count();
  }
  $limit = 50;
  $query = db_select('commerce_line_item', 'cli')
    ->fields('cli', array('line_item_id'));
  $order_alias = $query->leftJoin('commerce_order', 'co', '%alias.order_id = cli.order_id');
  $result = $query->isNull("$order_alias.order_id")
    ->condition('cli.line_item_id', $context['sandbox']['current_line_item_id'], '>')
    ->orderBy('cli.line_item_id', 'ASC')
    ->range(0, $limit)
    ->execute()
    ->fetchCol();

  $context['sandbox']['current_line_item_id'] = end($result);
  commerce_line_item_delete_multiple($result);
  $context['results'] += $result;
  $context['sandbox']['progress'] += count($result);
  $context['finished'] = $context['sandbox']['progress'] / $context['sandbox']['max'];
}

/**
 * Batch callback: finishes the batch deletion of orphaned line items.
 */
function commerce_line_item_ui_delete_orphaned_line_items_finished($success, $results, $operations) {
  if ($success) {
    $message = format_plural(count($results), 'One line item has been deleted.', '@count line items have been deleted.');
  }
  else {
    $message = t('Finished with an error.');
  }
  drupal_set_message($message);
}
