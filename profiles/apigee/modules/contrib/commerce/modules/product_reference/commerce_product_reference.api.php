<?php

/**
 * @file
 * Hooks provided by the Product Reference module.
 */

/**
 * Allows modules to alter the delta value used to determine the default product
 * entity in an array of referenced products.
 *
 * The basic behavior for determining a default product from an array of
 * referenced products is to use the first referenced product. This hook allows
 * modules to change that to a different delta value.
 *
 * Note that in some cases $products will be keyed by product ID while in other
 * cases it will be 0 indexed.
 *
 * @param $delta
 *   The key in the $products array of the product that should be the default
 *   product for display purposes in a product reference field value array.
 * @param $products
 *   An array of product entities referenced by a product reference field.
 *
 * @see commerce_product_reference_get_default_product()
 *
 * @deprecated by hook_commerce_product_reference_get_default_product_id_alter()
 */
function hook_commerce_product_reference_default_delta_alter(&$delta, $products) {
  // If a product with the SKU PROD-01 exists in the array, set that as the
  // default regardless of its position.
  foreach ($products as $key => $product) {
    if ($product->sku == 'PROD-01') {
      $delta = $key;
    }
  }
}

/**
 * Allows modules to alter the default product ID for a given entity.
 *
 * @param $product_id int
 *    The current default product ID.
 * @param $context array
 *    An associative array containing info about the referencing entity.
 *    Allowed array keys are:
 *    - 'entity': The entity referencing the products.
 *    - 'entity_type': The entity type.
 *    - 'field': The commerce_product_reference field info array.
 *    - 'langcode': The langcode to use for product loading.
 *
 * @see commerce_product_reference_get_default_product()
 */
function hook_commerce_product_reference_get_default_product_id_alter(&$product_id, $context) {
  // We just want to set the product for the 'my_nice_display_node' display node.
  if ($context['entity_type'] != 'node' || $context['entity']->type != 'my_nice_display_node') {
    return;
  }

  $values = field_get_items($context['entity_type'], $context['entity'], $context['field']['field_name'], $context['langcode']);
  if (empty($values) || !is_array($values)) {
    return;
  }

  // Get the product IDs.
  foreach ($values as &$item) {
    $item = $item['product_id'];
  }

  // Load the second available product for the given product_reference_field
  // which has a specific value for the my_product_color field.
  $query = new EntityFieldQuery();
  $query->entityCondition('entity_type', 'commerce_product')
    ->propertyCondition('status', 1)
    ->propertyCondition('product_id', $values)
    // Filter by my_product_color field value.
    ->fieldCondition('my_product_color', 'red')
    // Get one item from the second position.
    ->range(1, 1);
  $result = $query->execute();

  // If no products could be loaded matching the current selection, skip.
  if (empty($result['commerce_product'])) {
    return;
  }

  $product_id = key($result['commerce_product']);
}
