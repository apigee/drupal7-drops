<?php
/**
 * @file
 * Various page callback related functions.
 *
 * Currently functions related to processing the WorldPay responses are stored
 * here as well since they are not needed elsewhere and would way down the
 * main module.
 */

/**
 * Page callback that listens for transaction information from WorldPay.
 *
 * DRAFT
 * Here we create new transactions and load existing ones.
 *
 * - First validate the signature fields with md5
 * - Decide action
 * -- Create response pages for Worldpay to use
 * --- Success page or cancel page (resultY.html || resultC.html)
 */
function commerce_worldpay_bg_response_page($payment_method = NULL, $debug_wppr = array()) {
  $order = NULL;
  $skip_process = FALSE;

  if (!empty($debug_wppr)) {
    $wppr = $debug_wppr;
    $debug_mode = TRUE;
  }

  // Attempt to load payment method from alternative means.
  if (!isset($payment_method)) {
    if (!empty($wppr['transId']) && $prior_wppr = commerce_worldpay_bg_txn_load($wppr['transId'], 'wp_txn_id')) {
      if (!empty($prior_wppr['transaction_id'])) {
        $transaction = commerce_payment_transaction_load($prior_wppr['transaction_id']);
        if (empty($wppr['MC_orderId'])) {
          $wppr['MC_orderId'] = $transaction->order_id;
        }
        elseif ($wppr['MC_orderId'] != $transaction->order_id) {
          // Why do we have two different order IDs? Lets bail.
          exit('');
        }
        $payment_method = commerce_payment_method_instance_load($transaction->instance_id);
      }
    }

    // @todo Is this safe? Can we end up with the wrong payment method from the
    // Order object?
    if (!$payment_method && !empty($wppr['MC_orderId'])) {
      if ($order = commerce_order_load($wppr['MC_orderId'])) {
        if (!empty($order->data['payment_method'])) {
          $payment_method = commerce_payment_method_instance_load($order->data['payment_method']);
        }
      }
      else {
        print '';
        return;
      }
    }
  }

  // Make sure we have a payment method we handle.
  if ($payment_method) {
    if ($payment_method['method_id'] != 'commerce_worldpay_bg') {
      print '';
      return;
    }
  }
  else {
    // Bail since we have no useful data to work with.
    print '';
    return;
  }

  // If the payment method specifies full IPN logging, do it now.
  if (!empty($payment_method['settings']['payment_response_logging'])
    && $payment_method['settings']['payment_response_logging'] == 'full_wppr') {
    $full_log = TRUE;
  }
  else {
    $full_log = FALSE;
  }

  if (empty($debug_wppr)) {
    $debug_mode = FALSE;
    // WorldPay Payment Response.
    $wppr = $_POST;

    if (empty($wppr['transId']) && $wppr['transStatus'] != 'C') {
      if ($full_log) {
        watchdog(
          'commerce_worldpay_bg',
          'Request with no transId was sent from <em>@ip</em> with request method <b>@method</b>.',
          array(
            '@ip' => ip_address(),
            '@method' => !empty($_SERVER['REQUEST_METHOD']) ? $_SERVER['REQUEST_METHOD'] : '',
          ),
          WATCHDOG_NOTICE);
      }
      return;
    }

    if (empty($wppr['MC_orderId'])) {
      drupal_add_http_header('Status', '404 Not Found');
      if ($full_log) {
        watchdog(
          'commerce_worldpay_bg',
          'Request with no MC_orderId was sent from <em>@ip</em> with request method <b>@method</b>.',
          array(
            '@ip' => ip_address(),
            '@method' => !empty($_SERVER['REQUEST_METHOD']) ? $_SERVER['REQUEST_METHOD'] : '',
          ),
          WATCHDOG_NOTICE);
      }
      return;
    }
  }

  if ($full_log) {
    watchdog(
      'commerce_worldpay_bg', 'Attempting to process Payment Response @wp_txn_id. !wpr_log',
      array(
        '@wp_txn_id' => $wppr['transId'],
        '!wpr_log' => '<pre>' . check_plain(print_r($wppr, TRUE)) . '</pre>',
      ),
      WATCHDOG_NOTICE);
  }

  // If this has been processed before and nothing appears to have changed.
  if (!empty($wppr['transId']) && $prior_wppr = commerce_worldpay_bg_txn_load($wppr['transId'], 'wp_txn_id')) {
    if ($prior_wppr['auth_amount'] == $wppr['authAmount'] && $prior_wppr['transStatus'] == $wppr['transaction_status']) {
      if ($full_log) {
        watchdog('commerce_worldpay_bg',
          'Attempted to process an WorldPay response that has already been processed with transaction ID @txn_id.',
          array('@txn_id' => $wppr['txn_id']),
          WATCHDOG_NOTICE);
      }
      // Don't process this.
      $skip_process = TRUE;
    }
  }
  else {
    $prior_wppr = NULL;
  }

  if (!$order && !$order = commerce_order_load($wppr['MC_orderId'])) {
    drupal_add_http_header('Status', '404 Not Found');
    return;
  }

  $order_wrapper = entity_metadata_wrapper('commerce_order', $order);

  // Authentication.
  // @todo Check that the the transId and cartId are part of the same
  //   transaction.
  if ($debug_mode && $debug_wppr['skip_auth']) {
    $order->data['payment_redirect_key'] = '123';
  }
  else {
    $authenticated = _commerce_worldpay_bg_payment_response_authenticate(
      $order_wrapper,
      $wppr,
      $payment_method,
      $prior_wppr);

    if (!$authenticated) {
      return;
    }
  }

  $theme_data = array(
    'order' => $order,
    'order_id' => $wppr['MC_orderId'],
    'order_no' => empty($wppr['cartId']) ? $order->order_number : $wppr['cartId'],
    'wp_txn_id' => $wppr['transId'],
    'settings' => $payment_method['settings'],
  );
  $theme_data['installation_id'] = $payment_method ? $payment_method['settings']['installation_id'] : $wppr['instId'];
  // Remove parts of the settings a theme need not know about.
  unset($theme_data['payment_security']);
  $theme_data['settings']['theme_debug'] = $debug_mode;
  // Allow themes to adjust based on current context.
  $theme_data['settings']['context'] = 'general';

  switch ($wppr['transStatus']) {
    case 'Y':
      $theme_data['settings']['context'] = 'result_success';
      if ($debug_mode && $debug_wppr['skip_save']) {
        $order->data['payment_redirect_key'] = '123';
        // Write a message.
      }
      elseif (!$skip_process) {
        // Create or re-establish a Commerce Transaction.
        // This will add the 'transaction_id' to $wppr.
        commerce_worldpay_bg_transaction_process($order, $payment_method, $wppr, $prior_wppr);
        $tx_data = commerce_worldpay_bg_convert_wppr_to_record($wppr);
        $tx_data['transaction_id'] = $wppr['transaction_id'];
        $tx_data['order_id'] = $order->order_id;
        $tx_data['signature_fields'] = implode(':', _commerce_worldpay_bg_md5_signature_fields());
        if ($prior_wppr) {
          $tx_data = array_replace($prior_wppr, $tx_data);
        }
        commerce_worldpay_bg_txn_save($tx_data);
      }

      if (!empty($payment_method['settings']['debug'])) {
        watchdog(
          'commerce_worldpay_bg', 'Creating success HTML for transaction @wp_txn_id.',
          array(
            '@wp_txn_id' => $wppr['transId'],
          ),
          WATCHDOG_NOTICE);
      }

      // Now generate the HTML that WorldPay will pull from to generate its
      // custom page.
      // This is printed directly as we want to skip the rest of Drupal's HTML
      // build. See: drupal_deliver_html_page().
      drupal_set_title(t('@site_name payment',
        array('@site_name' => variable_get('site_name', 'Drupal Commerce') . ' WorldPay payment')));
      print _commerce_worldpay_bg_build_page($theme_data, 'success');

      break;

    case 'C':
      $theme_data['settings']['context'] = 'result_cancel';
      if ($prior_wppr) {
        // Don't know if WorldPay ever changes transStatus after it's been set
        // the first time, but if it does we update our transaction info.
        $tx_data = commerce_worldpay_bg_convert_wppr_to_record($wppr);
        $tx_data = array_replace($prior_wppr, $tx_data);
        commerce_worldpay_bg_txn_save($tx_data);
      }

      // Not totally sure if this is the right thing to do here?
      commerce_payment_redirect_pane_previous_page($order);

      if (!empty($payment_method) and !empty($payment_method['settings']['debug'])) {
        watchdog(
          'commerce_worldpay_bg', 'Creating cancel HTML for transaction @wp_txn_id.',
          array(
            '@wp_txn_id' => $wppr['transId'],
          ),
          WATCHDOG_NOTICE);
      }

      drupal_set_title(t('@site_name payment', array(
        '@site_name' => variable_get('site_name', 'Drupal Commerce') . ' WorldPay payment canceled')));
      print _commerce_worldpay_bg_build_page($theme_data, 'cancel');
      break;
  }

}

/**
 * Generates the theme output.
 */
function _commerce_worldpay_bg_build_page($theme_data, $type = 'success') {
  drupal_add_http_header('Content-Type', 'text/html; charset=utf-8');
  $content = theme('commerce_worldpay_bg_' . $type, $theme_data);
  $page = theme('commerce_worldpay_bg_page', array('content' => $content) + $theme_data);
  return theme('commerce_worldpay_bg_html', array('page' => $page) + $theme_data);
}

/**
 * Verifies the response has come from WorldPay.
 */
function _commerce_worldpay_bg_payment_response_authenticate($order_wrapper, &$wppr, &$payment_method = NULL, &$wp_transaction = NULL) {
  $failed_authenticaion = FALSE;
  $message = '';
  $settings = $payment_method['settings'];

  // If the merchant set a password compare them callbackPW.
  if ($settings['payment_security']['use_password'] && !empty($settings['payment_security']['password'])) {
    if ($settings['payment_security']['password'] != $wppr['callbackPW']) {
      $failed_authenticaion = TRUE;
      $message = 'WorldPay passwords do not match. Make sure you have the same password set in the Commerce WorldPay settings page as set in your WorldPay installtion.';
    }
  }

  // @todo - Is it worth checking the User agent is the one WorldPay uses
  //   which is: User-Agent: WJHRO/1.0 (WorldPay Java HTTP Request Object).
  // @todo Reverse DNS lookup on IP address.
  if ($failed_authenticaion) {
    drupal_add_http_header('Status', '403 Forbidden');
    $ip = ip_address();
    watchdog('commerce_worldpay_bg', 'Access denied! ' . $message . ' Clients details: <em>@ip</em> with request method <b>@method</b>. Refered by: <b>@referer</b>.',
      array(
        '@ip' => !empty($ip) ? $ip : '0.0.0.0',
        '@method' => !empty($_SERVER['REQUEST_METHOD']) ? $_SERVER['REQUEST_METHOD'] : '',
        '@referer' => !empty($_SERVER['HTTP_REFERER']) ? $_SERVER['HTTP_REFERER'] : 'UNKNOWN',
      ), WATCHDOG_WARNING);
    return FALSE;
  }

  return TRUE;
}

/**
 * Process the WPPR once it's been validated.
 */
function commerce_worldpay_bg_transaction_process($order, $payment_method, &$wppr, &$prior_wppr = NULL) {
  // Exit when we don't get a payment status we recognize.
  if (!in_array($wppr['transStatus'], array('Y', 'C'))) {
    commerce_payment_redirect_pane_previous_page($order);
    return FALSE;
  }

  // If this is a prior authorization capture WorldPay Payment Response for
  // which we've already created a transaction...
  if ($prior_wppr) {
    // Load the prior IPN's transaction and update that with the capture values.
    $transaction = commerce_payment_transaction_load($prior_wppr['transaction_id']);
  }
  else {
    // Create a new payment transaction for the order.
    $transaction = commerce_payment_transaction_new('commerce_worldpay_bg', $order->order_id);
    $transaction->instance_id = $payment_method['instance_id'];
  }

  $transaction->remote_id = $wppr['transId'];
  $transaction->amount = commerce_currency_decimal_to_amount($wppr['authAmount'], $wppr['authCurrency']);
  $transaction->currency_code = $wppr['authCurrency'];
  $transaction->payload[REQUEST_TIME] = $wppr;
  // Remove sensitive data.
  unset($transaction->payload[REQUEST_TIME]['callbackPW']);

  // Set the transaction's statuses based on the IPN's payment_status.
  $transaction->remote_status = $wppr['transStatus'];

  // @todo - Figure out how best to  set status based on SecureCode
  //   authentication.
  if (isset($wppr['authentication'])) {
    switch ($wppr['authentication']) {
      case 'ARespH.card.authentication.0':
        $transaction->status = COMMERCE_PAYMENT_STATUS_SUCCESS;
        $transaction->message = t("Cardholder authenticated by SecureCode.");
        break;

      case 'ARespH.card.authentication.1':
        $transaction->message = t('Cardholder/Issuing Bank not enrolled for authentication.');
        break;

      case 'ARespH.card.authentication.6':
        $transaction->message = t('Cardholder authentication not available');
        break;

      case 'ARespH.card.authentication.7':
        $transaction->message = t('Cardholder did not complete authentication.');
        break;
    }
  }

  // They don't give us very detailed transaction information do they?
  switch ($wppr['transStatus']) {
    case 'Y':
      $transaction->status = COMMERCE_PAYMENT_STATUS_SUCCESS;
      $transaction->message = t("WorldPay excepted the user's transaction.");
      break;

    // I don't think we should ever see this status at this point but incase
    // we do...
    case 'C':
      // @todo - Is this a suitable status?
      $transaction->status = COMMERCE_PAYMENT_STATUS_FAILURE;
      $transaction->message = t('The user canceled the payment.');
      break;
  }

  // Save the transaction information.
  commerce_payment_transaction_save($transaction);
  $wppr['transaction_id'] = $transaction->transaction_id;

  commerce_payment_redirect_pane_next_page($order);
  watchdog('commerce_worldpay_bg', 'Payment Response processed for Order @order_number with ID @txn_id.', array('@txn_id' => $wppr['transId'], '@order_number' => $order->order_number), WATCHDOG_INFO);
}
