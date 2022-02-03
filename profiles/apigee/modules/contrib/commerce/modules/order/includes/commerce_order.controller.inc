<?php

/**
 * @file
 * The controller for the order entity containing the CRUD operations.
 */

/**
 * The controller class for orders contains methods for the order CRUD
 * operations. The load method is inherited from the default controller.
 */
class CommerceOrderEntityController extends DrupalCommerceEntityController implements EntityAPIControllerRevisionableInterface {

  /**
   * Create a default order.
   *
   * @param array $values
   *   An array of values to set, keyed by property name.
   *
   * @return
   *   An order object with all default fields initialized.
   */
  public function create(array $values = array()) {
    $values += array(
      'order_id' => NULL,
      'order_number' => NULL,
      'revision_id' => NULL,
      'uid' => '',
      'mail' => ( !empty($values['uid']) && ($account = user_load($values['uid'])) ) ? $account->mail : '',
      'data' => array(),
      'created' => '',
      'changed' => '',
      'placed' => '',
      'hostname' => '',
    );

    return parent::create($values);
  }

  /**
   * Saves an order.
   *
   * When saving an order without an order ID, this function will create a new
   * order at that time. For new orders, it will also determine and save the
   * order number and then save the initial revision of the order. Subsequent
   * orders that should be saved as new revisions should set $order->revision to
   * TRUE and include a log string in $order->log.
   *
   * @param $order
   *   The full order object to save.
   * @param $transaction
   *   An optional transaction object.
   *
   * @return
   *   SAVED_NEW or SAVED_UPDATED depending on the operation performed.
   */
  public function save($order, DatabaseTransaction $transaction = NULL) {
    if (!isset($transaction)) {
      $transaction = db_transaction();
      $started_transaction = TRUE;
    }

    try {
      global $user;

      // Determine if we will be inserting a new order.
      $order->is_new = empty($order->order_id);

      // Set the timestamp fields.
      if ($order->is_new) {
        if (empty($order->created)) {
          $order->created = REQUEST_TIME;
        }
        if (empty($order->changed)) {
          $order->changed = REQUEST_TIME;
        }
        if (empty($order->revision_timestamp)) {
          $order->revision_timestamp = REQUEST_TIME;
        }
        if (empty($order->hostname)) {
          $order->hostname = ip_address();
        }
      }
      else {
        // Otherwise if the order is not new but comes from an entity_create()
        // or similar function call that initializes the created timestamp, uid,
        // and hostname values to empty strings, unset them to prevent
        // destroying existing data in those properties on update.
        if ($order->created === '') {
          unset($order->created);
        }
        if ($order->uid === '') {
          unset($order->uid);
        }
        if ($order->hostname === '') {
          unset($order->hostname);
        }

        $time = time();
        $order->changed = $time;
        $order->revision_timestamp = $time;
      }

      $order->revision_hostname = ip_address();
      $order->revision_uid = $user->uid;

      // Recalculate the order total using the current line item data.
      commerce_order_calculate_total($order);

      if ($order->is_new || !empty($order->revision)) {
        // When inserting either a new order or revision, $order->log must be set
        // because {commerce_order_revision}.log is a text column and therefore
        // cannot have a default value. However, it might not be set at this
        // point, so we ensure that it is at least an empty string in that case.
        if (!isset($order->log)) {
          $order->log = '';
        }
      }
      elseif (empty($order->log)) {
        // If we are updating an existing order without adding a new revision,
        // we need to make sure $order->log is unset whenever it is empty. As
        // long as $order->log is unset, drupal_write_record() will not attempt
        // to update the existing database column when re-saving the revision.
        unset($order->log);
      }

      return parent::save($order, $transaction);
    }
    catch (Exception $e) {
      if (!empty($started_transaction)) {
        $transaction->rollback();
        watchdog_exception($this->entityType, $e);
      }
      throw $e;
    }
  }

  /**
   * Unserializes the data property of loaded orders.
   */
  public function attachLoad(&$queried_orders, $revision_id = FALSE) {
    foreach ($queried_orders as $order_id => &$order) {
      $order->data = unserialize($order->data);
    }

    // Call the default attachLoad() method. This will add fields and call
    // hook_commerce_order_load().
    parent::attachLoad($queried_orders, $revision_id);
  }

  /**
   * Deletes multiple orders by ID.
   *
   * @param $order_ids
   *   An array of order IDs to delete.
   * @param $transaction
   *   An optional transaction object.
   *
   * @return boolean
   *   TRUE on success, FALSE otherwise.
   */
  public function delete($order_ids, DatabaseTransaction $transaction = NULL) {
    if (!empty($order_ids)) {
      $orders = $this->load($order_ids, array());

      // Ensure the orders can actually be deleted.
      foreach ((array) $orders as $order_id => $order) {
        if (in_array(FALSE, module_invoke_all('commerce_order_can_delete', $order))) {
          unset($orders[$order_id]);
        }
      }

      // If none of the specified orders can be deleted, return FALSE.
      if (empty($orders)) {
        return FALSE;
      }

      parent::delete($order_ids, $transaction);
      return TRUE;
    }
    else {
      return FALSE;
    }
  }

  /**
   * {@inheritdoc}
   */
  public function deleteRevision($revision_id) {
    if ($revisions = commerce_order_load_multiple(array(), array('revision_id' => $revision_id))) {
      $revision = $revisions ? reset($revisions) : FALSE;
      // Prevent deleting the current revision.
      if (!$revision || commerce_order_is_latest_revision($revision)) {
        return FALSE;
      }

      // Delete order revision.
      db_delete('commerce_order_revision')
        ->condition('order_id', $revision->order_id)
        ->condition('revision_id', $revision->revision_id)
        ->execute();

      $this->invoke('commerce_order_revision_delete', $revision);
      field_attach_delete_revision('commerce_order', $revision);
      return TRUE;
    }
    return FALSE;
  }
}
