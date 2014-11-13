<?php

/**
 * @file
 * Code to manage monetization plans.
 *
 */

use Apigee\Mint\Developer;
use Apigee\Mint\DeveloperRatePlan;
use Apigee\Mint\Exceptions\MintApiException;
use Apigee\Mint\Limit;
use Apigee\Mint\MonetizationPackage;
use Apigee\Mint\Organization;
use Apigee\Mint\RatePlan;
use Apigee\Mint\SuborgProduct;
use Apigee\Mint\Types\QuotaType;
use Apigee\Util\OrgConfig;
use Apigee\Util\CacheFactory;

/**
 * Drupal form for monetization plan.
 */
function devconnect_monetization_plan_form(array $form, array &$form_state, Developer $developer, MonetizationPackage $package, $selected_plan_id = NULL) {

  // @TODO Define what tab is to be shown given request conditions.
  if (!isset($form_state['stage'])) {
    $form_state['stage'] = 'purchase';
  }

  $form = array();
  $form_state['#developer'] = isset($form_state['#developer']) ? $form_state['#developer'] : $developer;
  $form_state['#package'] = isset($form_state['#package']) ? $form_state['#package'] : $package;

  switch ($form_state['stage']) {
    case 'purchase':
      $form = devconnect_monetization_purchase_plan_form($form, $form_state, $developer, $package);
      break;

    case 'overlap':
      $form = devconnect_monetization_overlap_plan_confirm_form($form, $form_state);
      break;
  }
  // If not enough points, then include script and variables.
  if (isset($form_state['top_up']['not_enough_balance'])) {
    $module_path = drupal_get_path('module', 'devconnect_monetization');
    drupal_add_js(array(
          'devconnect_monetization_rate_plan_form' => array(
          'top_up' => $form_state['top_up'],
          'top_up_form_url' => '/users/me/monetization/packages/' . rawurlencode($package->getId()) . '/required-top-up-form',
    )), 'setting');
    $form['#attached']['js'][] = array(
      'data' => $module_path . '/js/rate-plan-form-top-up.js',
      'type' => 'file',
    );
  }

  $form['#suffix'] = '<div id="top-up-wrapper"></div>';
  return $form;
}

/**
 * Drupal Form.
 */
function devconnect_monetization_purchase_plan_form($form, &$form_state) {
  $config = devconnect_monetization_default_org_config();
  $developer = $form_state['#developer'];
  $package = $form_state['#package'];

  $org_timezone = new DateTimeZone($package->getOrganization()->getTimezone());

  $js_settings = array();

  $products_names = array();
  foreach ($package->getProducts() as $product) {
    $products_names['products'][] = $product->getDisplayName();
  }
  $form['product_list'] = array(
    '#type'     => 'container',
    '#children' => theme(
      'devconnect_monetization_rate_plan_form_product',
      $products_names
    ),
  );

  $form['limits'] = array(
    '#type'     => 'container',
    '#children' => theme(
      'devconnect_monetization_rate_plan_form_limits',
      devconnect_monetization_get_developer_limits($developer, $package, $config)
    ),
  );

  $form['price_points'] = array(
    '#type'     => 'container',
    '#children' => theme('devconnect_monetization_rate_plan_form_price_points',
      array('products' => $package->getProducts())
    ),
  );

  $plans = (array) devconnect_monetization_get_plans($developer, $package, $config);

  // See if there is something that keeps user from purchasing a plan from this
  // package.
  $cannot_purchase_messages = module_invoke_all('purchase_plan_requirements', $developer, $package);
  foreach (array_keys($cannot_purchase_messages) as $message_key) {
    $cannot_purchase_messages[$message_key] += array('#weight' => 0);
  }
  uasort($cannot_purchase_messages, "element_sort");
  drupal_alter('purchase_plan_requirements', $cannot_purchase_messages, $developer, $package);
  $cannot_purchase_message = end($cannot_purchase_messages);

  // Get products that this monetization package includes.
  foreach ($package->getProducts() as $product) {
    $form['package']['#product_list'][] = $product->getDisplayName();
  }

  foreach ($package->getProducts() as $product) {
    $product_id = $product->getId();
    $sub_org_object = new SuborgProduct($product_id, $config);
    $sub_org_list = $sub_org_object->getList();
    $sub_org_strings = array();
    foreach ($sub_org_list as $sub_org) {
      if ($sub_org->getOrganization()->getParent() != NULL
        && !in_array($sub_org->getOrganization()
                     ->getDescription(), $sub_org_strings)
      ) {
        $sub_org_strings[] = $sub_org->getOrganization()->getDescription();
      }
    }
    $form['package']['#product_list_title'][] = $product->getDisplayName() . (count($sub_org_strings) ? ' (' . implode(', ', $sub_org_strings) . ')' : '');
  }

  $form['action'] = array(
    '#type' => 'radios',
    '#required' => TRUE,
    '#options' => array(
      'purchase' => t('Purchase'),
      'cancel' => t('Cancel'),
    ),
    '#attributes' => array(
      'class' => array('element-hidden'),
      'style' => array('display:none'),
    ),
  );

  switch (count($plans['rate_plans'])) {
    case 0:
      drupal_set_message(t('No plans were found for package <strong>@display_name</strong>', array('@display_name' => $package->getDisplayName())));
      return array();

    case 1:
      list($rate_plan) = array_values($plans['rate_plans']);
      $form['comparisons'] = array(
        '#type'     => 'container',
        '#children' => theme(
          'devconnect_monetization_rate_plan_form_product_specific_package',
          array(
            'rate_plan'  => $rate_plan,
            'rate_plans' => $plans,
          )
        ),
      );
      $js_settings['product_specific_plan_name'] = $rate_plan->getDisplayName();
      break;

    default:
      $form['comparisons'] = array(
        '#type' => 'container',
        '#attributes' => array(
          'class' => array(
            'plan-details-comparison',
          ),
        ),
        '#children' => theme('devconnect_monetization_plan_detail_comparison', $plans),
      );
      break;
  }

  if ($cannot_purchase_message === FALSE) {
    foreach ($plans['plan_dates']['can_purchase'] as $id => $plan_date) {
      $js_settings['date_limits'][$id] = devconnect_monetization_build_js_date_widget_construct_param($plan_date, $org_timezone, $id);
    }
    foreach ($plans['plan_dates']['can_end'] as $id => $plan_date) {
      $js_settings['date_limits'][$id] = devconnect_monetization_build_js_date_widget_construct_param($plan_date, $org_timezone, $id);
    }
  }

  $form['#action_dates'] = $plans['plan_dates'];

  $form['plan_options'] = array(
    '#type' => 'radios',
    '#required' => TRUE,
    '#options' => array(),
    '#attributes' => array(
      'class' => array('element-hidden'),
      'style' => array('display:none'),
    ),
  );

  foreach ($plans['rate_plans'] as $plan_id => $plan) {
    if (isset($form['plan_options']['#default_value'])) {
      $form['plan_options']['#default_value'] = $plan_id;
    }
    $form['plan_options']['#options'][$plan_id] = $plan->getDisplayName();
  }

  // This logic is taken from rate-plan-from.js, used to tell if this is a purchase
  // or cancel action.
  if($plans['active_plan_id'] == null || $plans['active_plan_ends_today'] == TRUE) {
    $plan_action = DEVCONNECT_MONETIZATION_PLAN_ACTION_PURCHASE;
  }
  else {
    $plan_action = DEVCONNECT_MONETIZATION_PLAN_ACTION_CANCEL;
  }

  $form['visible_form'] = array(
    'start_date' => array(
      '#title_display' => 'invisible',
      '#type' => 'textfield',
      '#date_format' => 'j F Y',
      '#attributes' => array(
        'size' => 40,
        'class' => array('date'),
      ),
    ),
    'actions' => array(
      'purchase' => array(
        '#type' => 'submit',
        '#value' => t('Purchase This Plan'),
      ),
      'cancel' => array(
        '#type' => 'submit',
        '#value' => t('End This Plan'),
      ),
    ),
    'plan_action' => array(
      '#type' => 'hidden',
      '#value' => $plan_action,
    ),
    '#prefix' => '<div class="purchase-plan well">'
    . '<h3><span id="action_title_wrapper"></span>&nbsp;<span id="plan_name_wrapper"></span></h3>'
    . '<p class="purchase">'. t('Any API products contained within this package will be available after the start date of your plan.') . '</p>'
    . '<p class="cancel">'. t('Ending a plan will result in loss of access to the API Products contained within this plan. All keys will be revoked to these API Products upon the end selected below.') . '</p>'
    . '<br/>'
    . '<strong class="purchase">' . t('Select a start date for this plan') . '</strong>'
    . '<strong class="cancel">' . t('Select an end date') . '</strong>'
    . '<br/>',
    '#suffix' => '</div>',
  );

  if ($cannot_purchase_message !== FALSE) {
    $cannot_purchase_message += array(
      '#prefix' => '<div class="purchase-plan well">',
      '#suffix' => '</div>',
    );

    $form['visible_form'] = $cannot_purchase_message;
  }

  $form['#attached']['js'][] = array(
    'data' => drupal_get_path('module', 'devconnect_monetization') . '/js/rate-plan-form.js',
    'type' => 'file',
  );

  $js_settings += array(
    'active_plan_id'         => $plans['active_plan_id'],
    'active_plan_name'       => $plans['active_plan_name'],
    'active_plan_end_date'   => isset($plans['active_plan_end_date']) ? $plans['active_plan_end_date']->format('Y-m-d') : NULL,
    'active_plan_ends_today' => $plans['active_plan_ends_today'],
    'action_title_wrapper' => array(
      'cancel' => t('End'),
      'purchase' => t('Purchase'),
    ),
    'select_date_tip' => array(
      'cancel' => t('Select an end date...'),
      'purchase' => t('Select a start date...'),
    ),
  );

  drupal_add_library('system', 'ui.datepicker');
  drupal_add_js(array('devconnect_monetization_rate_plan_form' => $js_settings), 'setting');

  $form['#validate'][] = 'devconnect_monetization_purchase_plan_form_validate';
  $form['#submit'][] = 'devconnect_monetization_purchase_plan_form_submit';
  if ($plans['active_plan_name'] !== NULL) {
    $form['#active_plan'] = $plans['active_plan_name'];
  }
  $form['#package'] = $package;
  $form['#package_name'] = $package->getDisplayName();
  $form['#theme'] = 'devconnect_monetization_rate_plan_form';

  return $form;
}

/**
 * Validate the purchase plan before submit.
 */
function devconnect_monetization_purchase_plan_form_validate(&$form, &$form_state) {
  // Make sure start date when purchasing a plan or end date when ending a plan
  // is not empty and is a valid date.
  if ($form_state['values']['plan_action'] == DEVCONNECT_MONETIZATION_PLAN_ACTION_PURCHASE) {
    $start_date = strtotime($form_state['values']['start_date']);
    if ($start_date === FALSE) {
      form_set_error('start_date', t('Invalid or blank start date. Please select a start date for this plan.'));
    }
  }
  else {
    if ($form_state['values']['plan_action']  == DEVCONNECT_MONETIZATION_PLAN_ACTION_CANCEL) {
      $end_date = strtotime($form_state['values']['start_date']);
      if ($end_date === FALSE) {
        form_set_error('start_date', t('Invalid or blank end date. Please select an end date to end this plan.'));
      }
    }
  }

}

/**
 * Drupal submit form.
 */
function devconnect_monetization_purchase_plan_form_submit(&$form, &$form_state) {
  $values = $form_state['values'];
  $plan_id = $values['plan_options'];

  if ($values['action'] == 'purchase') {
    $developer_id = _devconnect_monetization_get_developer_id(TRUE);
    $config = devconnect_monetization_default_org_config();
    $developer_rate_plan = new DeveloperRatePlan($developer_id, $config);
    $rate_plan = new RatePlan($form_state['#package']->getId(), $config);
    $rate_plan->load($plan_id);
    $developer_rate_plan->setRatePlan($rate_plan);

    $dev_plan_id = NULL;
    $dev_plans = $developer_rate_plan->getList();
    foreach ($dev_plans as $dev_plan) {
      if ($dev_plan->getRatePlan()->getId() == $rate_plan->getId()) {
        $dev_plan_id = $dev_plan->getId();
        break;
      }
    }

    $org_timezone = new DateTimeZone($rate_plan->getOrganization()->getTimezone());
    $utc_timezone = new DateTimeZone('UTC');
    $separator = strpos($values['start_date'], '/') > 0 ? '/' : '-';
    $submitted_start_date = $values['start_date'] . ' 00:00:00';
    $start_date = DateTime::createFromFormat("m{$separator}d{$separator}Y H:i:s", $submitted_start_date, $org_timezone);
    $start_date->setTimezone($utc_timezone);

    $developer_rate_plan->setId($dev_plan_id);
    $developer_rate_plan->setStartDate($start_date->format('Y-m-d H:i:s'));
    try {
      $developer_rate_plan->save('create');
      $vars = array(
        '@package' => $rate_plan->getMonetizationPackage()->getDisplayName(),
        '@plan' => $rate_plan->getDisplayName(),
      );
      drupal_set_message(t('@package, @plan has been purchased.', $vars), 'status');
      CacheFactory::getCacheManager(NULL)->clear('developer_accepted_rateplan:' . $developer_id);
      module_invoke_all('devconnect_monetization_plan_purchased', $developer_rate_plan);

      $modules = module_implements('redirect_after_purchase_plan');
      if (!empty($modules)) {
        $module = reset($modules);
        $url = module_invoke($module, 'redirect_after_purchase_plan');
        $form_state['redirect'] = $url;
      }
      if (!isset($form_state['redirect'])) {
        $form_state['rebuild'] = FALSE;
        $form_state['redirect'] = array(
          'users/' . $GLOBALS['user']->uid . '/monetization/packages/purchased',
        );
      }
    }
    catch (\Apigee\Exceptions\ResponseException $re) {
      drupal_set_message(t('The website encountered an unexpected error. Please try again later.'), 'error');
      $config->logger->critical($re);
    }
    catch (\Apigee\Mint\Exceptions\MintApiException $ce) {
      // If overlaps any rate plan, redirect to confirm override.
      if ($ce->getMintCode() == MintApiException::DEVELOPER_HAS_FOLLOWING_OVERLAP_RATE_PLANS) {
        $form_state['rate_plan'] = $rate_plan;
        $form_state['stage'] = 'overlap';
        $form_state['rebuild'] = TRUE;
        $form_state['mint_message'] = $ce->getMintMessage();
        $form_state['start_date'] = $values['start_date'];
        return;
      }
      else {
        if ($ce->getMintCode() == MintApiException::INSUFFICIENT_FUNDS
          || $ce->getMintCode() == MintApiException::PREPAID_DEVELOPER_HAS_NO_BALANCE
        ) {
          if ($rate_plan->getCurrency()->getName() != 'POINTS') {
            $message = $ce->getMintMessage(TRUE);
            $amount_starts_at = strpos($message, '[') + 1;
            $required = str_replace(',', '', substr($message, $amount_starts_at, strlen($message) - $amount_starts_at - 1));
            $currencies = commerce_currencies();
            $required = devconnect_monetization_round_up_amount($required, $currencies[$rate_plan->getCurrency()->getName()]['decimals']);
            $form_state['top_up'] = array(
              'not_enough_balance' => TRUE,
              'required_balance' => $required,
              'required_currency' => $rate_plan->getCurrency()->getName(),
              'package_id' => $form_state['#package']->getId(),
              'plan_id' => $plan_id,
              'plan_name' => $rate_plan->getDisplayName(),
              'start_date' => str_replace('-', '/', $values['start_date']),
              'overlap' => 'false',
            );
            $form_state['rebuild'] = TRUE;
          }
          else {
            drupal_set_message(t('You do not have enough points to purchase this plan.'), 'status');
          }
          return;
        }
      }
      $message = $ce->getMintMessage();
      if ($message !== NULL) {
        drupal_set_message(t($message), 'error');
      }
      else {
        drupal_set_message(t('The website encountered an unexpected error. Please try again later.'), 'error');
      }
      if ($GLOBALS['devconnect_monetization_debug_endpoint_response']) {
        drupal_set_message('DEBUG: ' . $ce->getMintMessage(TRUE), 'warning');
      }
      $config->logger->critical($ce);
    }
    catch (\Exception $e) {
      drupal_set_message(t('The website encountered an unexpected error. Please try again later.'), 'error');
      $config->logger->critical($e);
    }
  }
  else {
    $developer_id = _devconnect_monetization_get_developer_id(TRUE);
    $config = devconnect_monetization_default_org_config();

    // Get DeveloperRatePlans.
    $developer_rate_plan = new DeveloperRatePlan($developer_id, $config);
    $developer_rate_plans = $developer_rate_plan->getList();

    // Reload RatePlan.
    $rate_plan = new RatePlan($form_state['#package']->getId(), $config);
    $rate_plan->load($plan_id);
    $developer_rate_plan->setRatePlan($rate_plan);

    $developer_rate_plan = NULL;
    foreach ($developer_rate_plans as $dev_rate_plan) {
      if ($dev_rate_plan->getRatePlan()->getId() == $rate_plan->getId()
        && $dev_rate_plan->getRatePlan()->getMonetizationPackage()->getId() == $rate_plan->getMonetizationPackage()
                                                                               ->getId()
      ) {
        $developer_rate_plan = $dev_rate_plan;
        break;
      }
    }
    if (!isset($developer_rate_plan)) {
      return;
    }

    $org = new Organization($config);
    $org->load();
    $org_timezone = new DateTimeZone($org->getTimezone());
    $utc_timezone = new DateTimeZone('UTC');

    $separator = strpos($form_state['values']['start_date'], '/') > 0 ? '/' : '-';
    $submitted_end_date = $form_state['values']['start_date'] . ' 00:00:00';
    $end_date = DateTime::createFromFormat("m{$separator}d{$separator}Y H:i:s", $submitted_end_date, $org_timezone);
    $end_date->setTimezone($utc_timezone);

    $developer_rate_plan->setEndDate($end_date->format('Y-m-d H:i:s'));
    try {
      $developer_rate_plan->save('update');
      drupal_set_message(t('Plan is scheduled to end at the end of @date, and any refunds will be triggered at that point', array('@date' => $form_state['values']['start_date'])), 'status');
      CacheFactory::getCacheManager(NULL)->clear('developer_accepted_rateplan:' . $developer_id, FALSE);
      module_invoke_all('devconnect_monetization_plan_ended', $developer_rate_plan);
    }
    catch (\Apigee\Exceptions\ResponseException $re) {
      drupal_set_message(t('The website encountered an unexpected error. Please try again later.'), 'error');
      $config->logger->critical($re);
    }
    catch (MintApiException $ce) {
      $message = $ce->getMintMessage();
      if ($message !== NULL) {
        drupal_set_message($message, 'error');
      }
      else {
        drupal_set_message(t('The website encountered an unexpected error. Please try again later.'), 'error');
      }

      if ($GLOBALS['devconnect_monetization_debug_endpoint_response']) {
        drupal_set_message('DEBUG: ' . $ce->getMintMessage(TRUE), 'warning');
      }
      $config->logger->critical($ce);
    }
    catch (\Exception $e) {
      drupal_set_message(t('The website encountered an unexpected error. Please try again later.'), 'error');
      $config->logger->critical($e);
    }
  }
}

/**
 * Drupal form.
 */
function devconnect_monetization_overlap_plan_confirm_form($form, &$form_state) {

  $mint_message = $form_state['mint_message'];
  $overlaps = json_decode(substr($mint_message, strpos($mint_message, '=') + 1), TRUE);

  // Remove prefix xxx@ from product ids.
  $overlaps = array_map(function ($products) {
    $values = array();
    foreach ($products as $product_id => $product_name) {
      $values[substr($product_id, strrpos($product_id, '@') + 1)] = $product_name;
    }
    return $values;
  }, $overlaps);

  $package = $form_state['#package'];
  $rate_plan = $form_state['rate_plan'];

  // Process products in attempted purchased plan.
  $products = array();
  foreach ($package->getProducts() as $product) {
    $products[$product->getId()] = $product;
  }

  $plan_items = array();
  $overlapping = array();
  foreach ($overlaps as $plan_id => $overlapping_products) {
    list($id, $name) = explode('|', $plan_id);
    $plan_item = array(
      'data' => $name,
      'children' => array(),
    );
    $overlapping = array(
      t('Additional products:') => array_diff_key($products, $overlapping_products),
      t('Excluded products:') => array_diff_key($overlapping_products, $products),
      t('Conflicting products:') => array_intersect_key($products, $overlapping_products),
    );
    foreach ($overlapping as $situation => $situation_products) {
      if (!empty($situation_products)) {
        $product_items = array(
          'data' => $situation,
          'children' => array(),
        );
        foreach ($situation_products as $situation_product) {
          $product_name = is_object($situation_product) ? $situation_product->getDisplayName() : $situation_product;
          if (!in_array($product_name, $product_items['children'])) {
            $product_items['children'][] = $product_name;
          }
        }
        $plan_item['children'][] = $product_items;
      }
    }
    $plan_items[] = $plan_item;
  }

  $form['message'] = array(
    '#markup' => t('The plans below share some common products with the plan you are trying to purchase. However if you continue to purchase then these plans will be replaced by the plan you are about to purchase'),
  );

  $form['conflicting'] = array(
    '#type' => 'item',
    '#markup' => theme('devconnect_monetization_conflicting_products', array(
      'items' => $plan_items,
      'title' => t('Conflicts:'),
      'type' => 'ol',
      'attributes' => array(),
    )),
  );

  $form['package_id'] = array(
    '#type' => 'hidden',
    '#value' => $package->getId(),
  );
  $form['rate_plan_id'] = array(
    '#type' => 'hidden',
    '#value' => $rate_plan->getId(),
  );
  $form['start_date'] = array(
    '#type' => 'hidden',
    '#value' => $form_state['start_date'],
  );
  $form['#submit'][] = 'devconnect_monetization_overlap_plan_confirm_form_submit';

  return confirm_form($form,
    t('Plan "@plan_name" conflicts with others', array('@plan_name' => $rate_plan->getDisplayName())),
    'users/' . $GLOBALS['user']->uid . '/monetization/packages/' . rawurlencode($package->getId()) . '/view',
    NULL,
    t('Purchase'),
    t('Cancel'));
}

/**
 * Theme callback for devconnect_monetization_conflicting_products.
 *
 * Decorator of theme_item_list, we just wrap theme_item_list within
 * out own theme function thus allowing other modules to alter
 * this theme instead of theme_item_list.
 */
function theme_devconnect_monetization_conflicting_products($products) {
  return theme('item_list', $products);
}

/**
 * Drupal submit form.
 */
function devconnect_monetization_overlap_plan_confirm_form_submit($form, &$form_state) {

  $developer_id = _devconnect_monetization_get_developer_id(TRUE);

  $config = devconnect_monetization_default_org_config();
  $developer_rate_plan = new DeveloperRatePlan($developer_id, $config);

  $rate_plan = $form_state['rate_plan'];

  $developer_rate_plan->setRatePlan($rate_plan);

  $org_timezone = new DateTimeZone($rate_plan->getOrganization()->getTimezone());
  $utc_timezone = new DateTimeZone('UTC');
  $separator = strpos($form_state['values']['start_date'], '/') > 0 ? '/' : '-';
  $submitted_start_date = $form_state['values']['start_date'] . ' 00:00:00';
  $start_date = DateTime::createFromFormat("m{$separator}d{$separator}Y H:i:s", $submitted_start_date, $org_timezone);
  $start_date->setTimezone($utc_timezone);
  $developer_rate_plan->setStartDate($start_date->format('Y-m-d H:i:s'));
  try {
    $developer_rate_plan->force_save();
    $vars = array(
      '@package' => $rate_plan->getMonetizationPackage()->getDisplayName(),
      '@plan' => $rate_plan->getDisplayName(),
    );
    drupal_set_message(t('@package, @plan has been purchased.', $vars), 'status');
    CacheFactory::getCacheManager(NULL)->clear('developer_accepted_rateplan:' . $developer_id, FALSE);
    module_invoke_all('devconnect_monetization_plan_purchased', $developer_rate_plan);

    $modules = module_implements('redirect_after_override_plan');
    if (!empty($modules)) {
      $module = reset($modules);
      $url = module_invoke($module, 'redirect_after_override_plan');
      $form_state['redirect'] = $url;
    }
    elseif (!isset($form_state['redirect'])) {
      $form_state['rebuild'] = FALSE;
      $form_state['redirect'] = array(
        'users/' . $GLOBALS['user']->uid . '/monetization/packages/purchased',
      );
    }
  }
  catch (\Apigee\Exceptions\ResponseException $re) {
    drupal_set_message(t('The website encountered an unexpected error. Please try again later.'), 'error');
    $config->logger->critical($re);
    if ($GLOBALS['devconnect_monetization_debug_endpoint_response']) {
      drupal_set_message('DEBUG: ' . $re->getMessage(), 'warning');
    }
  }
  catch (MintApiException $ce) {
    if ($ce->getMintCode() == MintApiException::INSUFFICIENT_FUNDS
      || $ce->getMintCode() == MintApiException::PREPAID_DEVELOPER_HAS_NO_BALANCE
    ) {
      if ($ce->getMintCode() == MintApiException::INSUFFICIENT_FUNDS
        || $ce->getMintCode() == MintApiException::PREPAID_DEVELOPER_HAS_NO_BALANCE
      ) {
        if ($rate_plan->getCurrency()->getName() != 'POINTS') {
          $message = $ce->getMintMessage(TRUE);
          $amount_starts_at = strpos($message, '[') + 1;
          $required = str_replace(',', '', substr($message, $amount_starts_at, strlen($message) - $amount_starts_at - 1));
          $currencies = commerce_currencies();
          $required = devconnect_monetization_round_up_amount($required, $currencies[$rate_plan->getCurrency()->getName()]['decimals']);
          $form_state['top_up'] = array(
            'not_enough_balance' => TRUE,
            'required_balance' => $required,
            'required_currency' => $rate_plan->getCurrency()->getName(),
            'package_id' => $form_state['#package']->getId(),
            'plan_id' => $rate_plan->getId(),
            'plan_name' => $rate_plan->getDisplayName(),
            'start_date' => $form_state['values']['start_date'],
            'overlap' => 'true',
          );
          $form_state['rebuild'] = TRUE;
        }
        else {
          drupal_set_message(t('You do not have enough points to purchase this plan.'), 'status');
        }
        return;
      }
    }
    $message = $ce->getMintMessage();
    if ($message !== NULL) {
      drupal_set_message(t($message), 'error');
    }
    else {
      drupal_set_message(t('The website encountered an unexpected error. Please try again later.'), 'error');
    }
    if ($GLOBALS['devconnect_monetization_debug_endpoint_response']) {
      drupal_set_message('DEBUG: ' . $ce->getMintMessage(TRUE), 'warning');
    }
    $config->logger->critical($ce);
  }
  catch (\Exception $e) {
    drupal_set_message(t('The website encountered an unexpected error. Please try again later.'), 'error');
    $config->logger->critical($e);
  }
}

/**
 * Get developer limits per package and per product.
 *
 * Excludes limits with developer specified as ALL
 * Excludes product limits that do not have a corresponding product in the
 * package.
 * Excludes limits with quotaType = QuotaType::BALANCE or
 * quotaType = QuotaType::CREDIT
 */
function devconnect_monetization_get_developer_limits(Apigee\Mint\Developer $developer, Apigee\Mint\MonetizationPackage $package, Apigee\Util\OrgConfig $config) {

  $limit = new Limit($config);
  $limits = $limit->getDeveloperLimits($developer->getEmail(), $package->getId(), TRUE);
  $dev_limits = array();
  // Loop through developer limits and exclude those where developer is ALL and
  // haltExecution is true.
  foreach ($limits as $limit_item) {
    if ($limit_item->getDeveloper() != 'ALL' && in_array($limit_item->getUserId(), array('ANY', 'ALL'))
      && $limit_item->getHaltExecution() == TRUE
    ) {
      $dev_limits[] = $limit_item;
    }
  }

  // Get the products that monetization package includes.
  $included_products = array();
  foreach ($package->getProducts() as $product) {
    $included_products[$product->getId()] = $product->getDisplayName();
  }

  $product_limits = array();
  $package_limits = array();
  foreach ($dev_limits as $limit) {
    // Exclude limits with QuotaType equals to either Balance or CreditLimit.
    if ($limit->getQuotaType() != QuotaType::BALANCE && $limit->getQuotaType() != QuotaType::CREDIT_LIMIT) {
      /* Is limit a product level limit? */
      if ($limit->isPublished() && array_key_exists($limit->getProduct(), $included_products)) {
        $product_limits[$included_products[$limit->getProduct()]][] = _devconnect_monetization_build_limit_text($limit, NULL);
      }
      /************************************************************************
       * Else it may be a package level limit. If limit is implicit (quotaTyte
       * is either Balance or CreditLimt) then it is excluded. Values of this
       * limits are concatenated in $package_limits for later display.
       ***********************************************************************/
      else {
        if ($limit->isPublished() && $limit->getProduct() == 'ALL') {
          $limit_text = _devconnect_monetization_build_limit_text($limit);
          if (isset($limit_text)) {
            $package_limits[] = $limit_text;
            unset($limit_text);
          }
        }
      }
    }
  }

  // Sort the limits by product name ASC.
  ksort($product_limits);
  $developer_limits = array(
    'has_limits'      => !empty($package_limits) || !empty($product_limits),
    'package_limits'  => $package_limits,
    'products_limits' => $product_limits,
  );
  return $developer_limits;
}

/**
 * Pull the Rate Plans that package has available for the given developer.
 *
 * It also does some pre-processing.
 *
 * returns:
 *   active_plan_name: The name of the active plan in the package, if any.
 *   active_plan_id: The id of the active plan in the package, if any.
 *   active_plan_end_date: The date the plan is to end, if any.
 *   accepted_rate_plans: Rate Plans accepted/purchased, these are not
 *   filtered out by package.
 *   rate_plans: key|value array of Rate Plans in this package.
 *   Key is the Rate Plan id.
 *   plan_dates: Array with rate plans and dates for developer to
 *   purchase/end.
 *   can_purchase: Plans that can be purchased.
 *   available_since: Start date for a developer to purchase the plan.
 *   available_until: End date for a developer to purchase the plan.
 *   can_end: Plans that can be ended (It must be only one).
 *   can_end_since: Start date for a developer to cancel a plan.
 *   can_end_until: End date for a developer to cancel a plan.
 */
function devconnect_monetization_get_plans(Developer $developer, MonetizationPackage $package, OrgConfig $config) {

  // Retrieve developer accepted plans.
  $developer_rate_plan = new DeveloperRatePlan($developer->getEmail(), $config);
  $developer_rate_plans = $developer_rate_plan->getList();

  // Initialize the organization timezone.
  $org_timezone = new DateTimeZone($package->getOrganization()->getTimezone());

  // UTC Time zone.
  $utc_timezone = new DateTimeZone('UTC');

  // Initialize the organization current date and next day.
  $today = new DateTime('today', $org_timezone);
  $tomorrow = new DateTime('tomorrow', $org_timezone);

  // Hold the active rate plan id if any.
  $active_plan_id = NULL;

  // Hold the active rate plan name if any.
  $active_plan_name = NULL;

  // Hold the active rate plan end date if any.
  $active_plan_end_date = NULL;

  // Hold the dates for the purchase/end plan end forms.
  $plans_dates = array(
    'can_purchase' => array(),
    'can_end'      => array(),
  );

  /*************************************************************************
   * Loop accepted rate plans for building the purchase/end plan forms     *
   * also to detect the active plan id/name if any                         *
   *************************************************************************/
  foreach ($developer_rate_plans as $developer_rate_plan) {

    /*************************************************************************
     * Skip plans that do not belong to to monetization package in question. *
     *************************************************************************/
    if ($developer_rate_plan->getRatePlan()->getMonetizationPackage()
        ->getId() != $package->getId()
    ) {
      continue;
    }

    /***************************************************************************
     * Convert time strings with org timezone to DateTime objects for friendly
     * usage. *
     **************************************************************************/
    $dev_rate_plan_start_date = DateTime::createFromFormat('Y-m-d H:i:s', $developer_rate_plan->getStartDate(), $utc_timezone);
    $dev_rate_plan_end_date = DateTime::createFromFormat('Y-m-d H:i:s', $developer_rate_plan->getEndDate(), $utc_timezone);
    $rate_plan_start_date = DateTime::createFromFormat('Y-m-d H:i:s', $developer_rate_plan
                                                                      ->getRatePlan()
                                                                      ->getStartDate(), $utc_timezone);
    $rate_plan_end_date = DateTime::createFromFormat('Y-m-d H:i:s', $developer_rate_plan
                                                                    ->getRatePlan()
                                                                    ->getEndDate(), $utc_timezone);

    // Convert UTC datetimes to ORG's timezone.
    $dev_rate_plan_start_date->setTimezone($org_timezone);
    $dev_rate_plan_end_date = $dev_rate_plan_end_date !== FALSE ? $dev_rate_plan_end_date->setTimezone($org_timezone) : FALSE;
    $rate_plan_start_date = $rate_plan_start_date !== FALSE ? $rate_plan_start_date->setTimezone($org_timezone) : FALSE;
    $rate_plan_end_date = $rate_plan_end_date !== FALSE ? $rate_plan_end_date->setTimezone($org_timezone) : FALSE;

    /******************************************************************
     * Test either this plan has ended before today or may end today. *
     ******************************************************************/
    if ($dev_rate_plan_end_date !== FALSE && $dev_rate_plan_end_date < $tomorrow) {

      /********************************************************************
       * Does this plan end today? If so then this plan will be available *
       * for purchasing tomorrow or whenever the plan starts again.       *
       *********************************************************************/
      if ($dev_rate_plan_end_date >= $today) {

        $plans_dates['can_purchase'][$developer_rate_plan->getRatePlan()->getId()] = array(
          'available_since' => $tomorrow < $rate_plan_start_date ? $rate_plan_start_date : $tomorrow,
          'available_until' => $rate_plan_end_date,
        );

        /*******************************************************************
         * Since plan is active for today and no other plan has claimed    *
         * to be active for today, then this is the active plan. It may be *
         * replaced as active plan if other plan starts today.             *
         *******************************************************************/
        if (is_null($active_plan_id)) {
          $active_plan_id = $developer_rate_plan->getRatePlan()->getId();
          $active_plan_name = $developer_rate_plan->getRatePlan()->getDisplayName();
          if (!array_key_exists($developer_rate_plan->getRatePlan()->getId(), $plans_dates['can_end'])) {
            $active_plan_end_date = $dev_rate_plan_end_date;
          }
        }
      }
      /**********************************************************************
       * Since this plan ended before today then it is available for
       * purchasing
       * at this moment or whenever the plan starts again.
       **********************************************************************/
      else {
        $plans_dates['can_purchase'][$developer_rate_plan->getRatePlan()->getId()] = array(
          'available_since' => $today < $rate_plan_start_date ? $rate_plan_start_date : $today,
          'available_until' => $rate_plan_end_date,
        );
      }
    }
    /***********************************************************************
     * Plan is still active now and in the future, therfore this plan      *
     * can be ended at any time as long the end date is not ahead of date  *
     * when the plan ends.                                                 *
     ***********************************************************************/
    else {
      $plans_dates['can_end'][$developer_rate_plan->getRatePlan()->getId()] = array(
        'can_end_since' => $today < $dev_rate_plan_start_date ? $dev_rate_plan_start_date : $today,
        'can_end_until' => $rate_plan_end_date,
      );
      /***********************************************************************
       * If plan has already started or starts today, then this is the active
       * plan.
       * It also replaces any active plan that ends today.
       ***********************************************************************/
      if ($dev_rate_plan_start_date <= $today) {
        $active_plan_id = $developer_rate_plan->getRatePlan()->getId();
        $active_plan_name = $developer_rate_plan->getRatePlan()
                            ->getDisplayName();
      }
    }
  }

  /**********************************************************************
   * Remove plans that are to be ended but still remain as purchasable  *
   **********************************************************************/
  foreach (array_keys($plans_dates['can_end']) as $can_end_plan_id) {
    if (array_key_exists($can_end_plan_id, $plans_dates['can_purchase'])) {
      unset($plans_dates['can_purchase'][$can_end_plan_id]);
    }
  }

  $rate_plan = new RatePlan($package->getId(), $config);
  $rate_plan->setDeveloper($developer);

  // Get current rate plans.
  $rate_plans = $rate_plan->getList(0, 0);

  /************************************************************** **************
   * Get current rate plans plus future rate plans. Notice that parent plan is
   * actually the current plan therefore the plan we get from the list is the
   * future one.
   * To make things easier and understandable in the list the parent rate plan
   * replaces  the plan in the list and future plan is accessed as the child
   * plan.
   * A little bit  tricky even to explain.
   ****************************************************************************/
  $all_rate_plans = $rate_plan->getList(0, 0, FALSE, TRUE);

  // Loop rate plans to find out which ones have future plans.
  for ($i = 0, $len = count($rate_plans); $i < $len; $i++) {
    $rate_plan_item = $rate_plans[$i];

    // Assigned the rate plans for future plans.
    foreach ($all_rate_plans as $all_rate_plan_item) {
      if ($all_rate_plan_item->getParentRatePlan() != NULL
        && $rate_plan_item->getId() == $all_rate_plan_item->getParentRatePlan()
                                       ->getId()
      ) {
        $rate_plans[$i] = $all_rate_plan_item->getParentRatePlan();
        break;
      }
    }

    /**************************************************************************
     * Replace plan in the list for parent rate plan. The replaced plan will
     * be later accessed by parent rate plan ->getChildRatePlan()
     **************************************************************************/
    $rate_plan_item = $rate_plans[$i];

    /*************************************************************************
     * Assigned the minimum and maximum purchasing dates for the remaining
     * plans that are not accepted by the developer
     *************************************************************************/
    if (!array_key_exists($rate_plan_item->getId(), $plans_dates['can_end'])
      && !array_key_exists($rate_plan_item->getId(), $plans_dates['can_purchase'])
    ) {
      $start_date = DateTime::createFromFormat('Y-m-d H:i:s', $rate_plan_item->getStartDate(), $utc_timezone)
                    ->setTimezone($org_timezone);
      $end_date = DateTime::createFromFormat('Y-m-d H:i:s', $rate_plan_item->getEndDate(), $utc_timezone);
      $plans_dates['can_purchase'][$rate_plan_item->getId()] = array(
        'available_since' => $start_date > $today ? $start_date : $today,
        'available_until' => $end_date !== FALSE ? $end_date->setTimezone($org_timezone) : FALSE,
      );
    }
  }

  $keyed_developer_rate_plans = array();
  foreach ($developer_rate_plans as $developer_rate_plan) {
    $keyed_developer_rate_plans[$developer_rate_plan->getId()] = $developer_rate_plan;
  }

  $keyed_rate_plans = array();
  foreach ($rate_plans as $rate_plan) {
    $keyed_rate_plans[$rate_plan->getId()] = $rate_plan;
  }

  $plans = array(
    'active_plan_name'       => $active_plan_name,
    'active_plan_id'         => $active_plan_id,
    'active_plan_end_date'   => $active_plan_end_date,
    'active_plan_ends_today' => $active_plan_end_date == $today,
    'plan_dates'             => $plans_dates,
    'accepted_rate_plans'    => $keyed_developer_rate_plans,
    'rate_plans'             => $keyed_rate_plans,
  );
  return $plans;
}

/**
 * TODO: explain this function.
 */
function devconnect_monetization_build_js_date_widget_construct_param(array $plan_date, DateTimeZone $org_timezone) {
  $min_key = array_key_exists('available_since', $plan_date) ? 'available_since' : 'can_end_since';
  $max_key = array_key_exists('available_until', $plan_date) ? 'available_until' : 'can_end_until';
  $plan_dates = array();
  if (FALSE && $plan_date[$max_key] !== FALSE) {
    $plan_dates['max_date'] = array_combine(array('year', 'month', 'day'), explode('-', $plan_date[$max_key]->setTimezone($org_timezone)->format('Y-m-d')));
  }
  $plan_dates['min_date'] = array_combine(array('year', 'month', 'day'), explode('-', $plan_date[$min_key]->setTimezone($org_timezone)->format('Y-m-d')));
  return $plan_dates;
}

/**
 * Form builder for a developer to top up.
 *
 * Form builder for a developer to top up when he has already attempted to
 * purchase a plan with insufficient founds. Note that $form_state should
 * contain the top up details.
 *
 * Page callback
 *   users/%devconnect_monetization_developer/monetization/packages/
 * %devconnect_monetization_package/required-top-up-form
 */
function devconnect_monetization_insufficient_top_up_form($form, &$form_state, \Apigee\Mint\Developer $developer) {

  $current_balance = 0;
  $balances = $developer->getPrepaidBalance();
  foreach ($balances as $balance) {
    if ($balance->getSupportedCurrency()->getName() == $form_state['values']['currency']) {
      $current_balance = $balance->getCurrentBalance();
      break;
    }
  }

  $formatted_current_balance = commerce_currency_format($current_balance, $form_state['values']['currency'], NULL, FALSE);

  $form['currency_title'] = array(
    '#type' => 'markup',
    '#markup' => $form_state['values']['currency'],
    '#prefix' => '<span id="currency_title">',
    '#suffix' => '</span>',
  );
  $form['plan_name'] = array(
    '#type' => 'markup',
    '#markup' => $form_state['values']['plan_name'],
    '#prefix' => '<span id="plan_name"><strong>',
    '#suffix' => '</strong></span>',
  );

  // The default topup amount is the required balance minus the current balance.
  $default_topup_amount = $form_state['values']['required_balance'] - $current_balance;

  // The new balance is the default amount the user will add plus the current balance.
  $new_balance = $default_topup_amount + $current_balance;
  $formatted_new_balance = commerce_currency_format($new_balance, $form_state['values']['currency'], NULL, FALSE);

  $formatted_required = commerce_currency_format($form_state['values']['required_balance'], $form_state['values']['currency'], NULL, FALSE);
  $form['required_display'] = array(
    '#type' => 'markup',
    '#markup' => $formatted_required,
    '#prefix' => '<strong>',
    '#suffix' => '</strong>',
  );
  $form['current_balance_display'] = array(
    '#type' => 'markup',
    '#markup' => $formatted_current_balance,
  );
  $form['new_balance_display'] = array(
    '#type' => 'markup',
    '#markup' => $formatted_new_balance,
  );
  $form['start_date'] = array(
    '#type' => 'hidden',
    '#value' => $form_state['values']['start_date'],
  );
  $form['required_balance'] = array(
    '#type' => 'hidden',
    '#value' => $form_state['values']['required_balance'],
  );
  $form['currency'] = array(
    '#type' => 'hidden',
    '#value' => $form_state['values']['currency'],
  );
  $form['currency_id'] = array(
    '#type' => 'value',
    '#value' => $form_state['values']['currency'],
  );
  $form['package_id'] = array(
    '#type' => 'value',
    '#value' => $form_state['values']['package_id'],
  );
  $form['plan_id'] = array(
    '#type' => 'hidden',
    '#value' => $form_state['values']['plan_id'],
  );
  $form['current_balance'] = array(
    '#type' => 'hidden',
    '#value' => $current_balance,
  );
  $form['overlap'] = array(
    '#type' => 'hidden',
    '#value' => $form_state['values']['overlap'],
  );

  $currencies = commerce_currencies();
  $form['amount'] = array(
    '#type' => 'textfield',
    '#required' => TRUE,
    '#default_value' => number_format($default_topup_amount, $currencies[$form_state['values']['currency']]['decimals'], '.', ''),
    '#maxlength' => 12,
    '#attributes' => array(
      'class' => array(
        'numeric',
        'currency',
      ),
      'currency' => $form_state['values']['currency'],
      'minimum' => $form_state['values']['required_balance'],
    ),
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Proceed to next step'),
  );

  $form['#theme'] = 'devconnect_monetization_insuffient_funds_top_up_balance';
  $form['#submit'][] = 'devconnect_monetization_insufficient_top_up_form_submit';
  return $form;
}

/**
 * Drupal form submit function.
 */
function devconnect_monetization_insufficient_top_up_form_submit($form, &$form_state) {
  $top_up_form_state = array(
    'values' => array(
      'currency_id' => $form_state['values']['currency_id'],
      'top_up_amount' => $form_state['values']['amount'],
      'notes' => NULL,
      'order_data' => array(
        'packageid' => $form_state['values']['package_id'],
        'planid' => $form_state['values']['plan_id'],
        'date' => $form_state['values']['start_date'],
        'overlap' => $form_state['values']['overlap'],
      ),
    ),
  );
  module_load_include('inc', 'devconnect_monetization', 'devconnect_monetization.billing');
  drupal_form_submit('devconnect_monetization_top_up_balance_form_submit', $top_up_form_state);
  $form_state['rebuild'] = $top_up_form_state['rebuild'];
  $form_state['redirect'] = $top_up_form_state['redirect'];
}

/**
 * Drupal form.
 */
function devconnect_monetization_ajax_insufficient_top_up_form(\Apigee\Mint\Developer $developer, \Apigee\Mint\MonetizationPackage $package, \Apigee\Mint\RatePlan $plan, $params
) {

  list($required_balance, $start_date, $overlap) = explode('|', rawurldecode($params));
  $form_state = array(
    'values' => array(
      'package_id' => $package->getId(),
      'plan_id' => $plan->getId(),
      'plan_name' => $plan->getDisplayName(),
      'currency' => $plan->getCurrency()->getName(),
      'required_balance' => rawurldecode($required_balance),
      'start_date' => str_replace('-', '/', rawurldecode($start_date)),
      'overlap' => rawurldecode($overlap),
    ),
    'build_info' => array(
      'args' => array($developer, $package),
    ),
  );
  return drupal_build_form('devconnect_monetization_insufficient_top_up_form', $form_state);
}

/**
 * Delivery function to output only a rendered form.
 */
function devconnect_monetization_insufficient_top_up_form_output($form) {
  print render($form);
}

/**
 * Remove a plan from a developer.
 */
function devconnect_monetization_remove_plan_from_user($package, $dev_rate_plan_id) {
  $config = devconnect_monetization_default_org_config();
  $developer_id = _devconnect_monetization_get_developer_id(TRUE);

  $dev_rate_plan_id = rawurldecode($dev_rate_plan_id);
  try {

    $dev_rate_plan = new DeveloperRatePlan($developer_id, $config);
    $dev_rate_plans = $dev_rate_plan->getList();

    foreach ($dev_rate_plans as $dev_rate_plan) {
      if ($dev_rate_plan->getId() == $dev_rate_plan_id) {
        $org_timezone = new DateTimeZone($dev_rate_plan->getRatePlan()->getOrganization()->getTimezone());
        $utc_timezone = new DateTimeZone('UTC');
        $today = new DateTime('today', $org_timezone);
        $start_date = DateTime::createFromFormat('Y-m-d H:i:s', $dev_rate_plan->getStartDate(), $utc_timezone)
                      ->setTimezone($org_timezone);
        if ($start_date > $today) {
          $dev_rate_plan->delete($dev_rate_plan_id);
          drupal_set_message(t('Your plan has been removed.'), 'status');
          CacheFactory::getCacheManager(NULL)->clear('developer_accepted_rateplan:' . $developer_id, FALSE);
          module_invoke_all('devconnect_monetization_plan_removed', $dev_rate_plan);
          drupal_goto('users/' . $GLOBALS['user']->uid . '/monetization/packages/purchased');
        }
      }
    }
    throw new Exception(t('Could not delete developer rate plan. Either does not exists or start and end dates not in the future'));
  }
  catch (MintApiException $cae) {
    if ($cae->getMintCode() == MintApiException::ONLY_FUTURE_DEVELOPER_RATE_PLAN_CAN_BE_DELETED) {
      drupal_set_message($cae->getMintMessage(), 'error');
      $config->logger->critical($cae->getMintMessage());
    }
  }
  catch (Exception $e) {
    drupal_set_message(t('Could not complete the operation. Please try again later.'), 'error');
    $config->logger->critical($e);
    drupal_goto('users/' . $GLOBALS['user']->uid . '/monetization/packages/purchased');
  }
}

/**
 * Round up amount.
 */
function devconnect_monetization_round_up_amount($amount, $decimals) {
  $factor = pow(10, $decimals);
  return ceil($amount * $factor) / $factor;
}
