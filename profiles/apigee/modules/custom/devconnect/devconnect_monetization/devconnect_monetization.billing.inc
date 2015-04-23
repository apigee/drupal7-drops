<?php

/**
 * @file
 * Billing methods.
 */

use Apigee\Mint\Types\BillingDocumentType;
use Apigee\Mint\BillingDocument;
use Apigee\Mint\Exceptions\MintApiException;
use Apigee\Mint\Organization;
use Apigee\Mint\DeveloperBalance;

/**
 * Page callback for:
 *   users/%devconnect_monetization_developer/monetization
 *   users/%devconnect_monetization_developer/monetization/billing
 *   users/%devconnect_monetization_developer/monetization/billing/prepaid-balance
 *
 * @param \Apigee\Mint\Developer $developer
 * @return string
 */
function devconnect_monetization_prepaid_balance(\Apigee\Mint\Developer $developer) {
  $config = devconnect_monetization_default_org_config();
  $organization = new Organization($config);
  $currencies = $organization->listSupportedCurrencies();
  $top_up_form = NULL;

  $access_prepaid_reports = user_access('access mint prepaid reports');
  if ($access_prepaid_reports) {
    $top_up_balance_perm = user_access('top up mint balance');
    if ($top_up_balance_perm) {
      $form = drupal_get_form('devconnect_monetization_top_up_balance_form');
      $top_up_form = drupal_render($form);
    }
    $download_prepaid_report_perm = user_access('download mint prepaid report');
    if ($download_prepaid_report_perm) {
      $previous_prepaid_statements_form = drupal_get_form('devconnect_monetization_download_prepaid_report_form');
    }

    $company = apigee_company_get_current_context_company();
    if (!empty($company)) {
      $company_id = $company->name;
    }
    else {
      $company_id = NULL;
    }

    // Decide what API call to use to get the report data for Billing and
    // Reports page.
    if (variable_get('devconnect_monetization_balance_report_type', BILLING_AND_REPORTS_USE_PREPAID_API_CALL) == BILLING_AND_REPORTS_USE_PREPAID_API_CALL) {
      try {
        $balances = $developer->getPrepaidBalance(NULL, NULL, NULL, $company_id);
      }
      catch(\Apigee\Exceptions\ResponseException $e) {
        drupal_set_message(t('There was an error retrieving your balance. If the problem persists please contact your administrator.'), 'error');
        global $user;
        $message_vars = array(
          '%user_name' => $user->name,
          '%user_email' => $user->mail,
          '%error_code' => $e->getCode(),
          '%error_message' => $e->getMessage(),
        );
        watchdog('devconnect_monetization', "Could not retrieve balance from Edge server for user %user_name (%user_email).  Error Code: %error_code Message: %error_message", $message_vars, WATCHDOG_ERROR);

        $balances = array();
      }
    }
    else {
      $developer_balance = new DeveloperBalance($developer->getEmail(), $config);
      $balances = $developer_balance->getList();
    }
    $topped_up_currencies = 0;
    /** @var Apigee\Mint\DeveloperBalance $balance */
    foreach ($balances as $balance) {
      /** @var Apigee\Mint\DataStructures\SupportedCurrency $currency */
      foreach ($currencies as $currency) {
        if ($balance->getSupportedCurrency()->getId() == $currency->getId()) {
          $topped_up_currencies++;
          break;
        }
      }
      // Calculate the Current Balance from Total - Usage.
      if (! (variable_get('devconnect_monetization_balance_report_type', BILLING_AND_REPORTS_USE_PREPAID_API_CALL) == BILLING_AND_REPORTS_USE_PREPAID_API_CALL) )
        $balance->setAmount($balance->getAmount() - $balance->getUsage());
    }
  }
  else {
    $balances = array();
    $top_up_balance_perm = FALSE;
    $topped_up_currencies = 0;
    $download_prepaid_report_perm = FALSE;
  }
  $variables = array(
    'access_prepaid_reports' => $access_prepaid_reports,
    'balances' => $balances,
    'has_balances' => count($balances) > 0,
    'top_up_form' => $top_up_form,
    'top_up_balance_perm' => $top_up_balance_perm,
    'currencies' => $currencies,
    'previous_prepaid_statements_form' => drupal_render($previous_prepaid_statements_form),
    'can_top_up_another_currency' => $topped_up_currencies < count($currencies),
    'download_prepaid_report_perm' => $download_prepaid_report_perm,
    'balance_report_type' => variable_get('devconnect_monetization_balance_report_type', BILLING_AND_REPORTS_USE_PREPAID_API_CALL),
  );
  drupal_add_js(drupal_get_path('module', 'devconnect_monetization') . '/js/global.js', 'file');
  return theme('devconnect_monetization_billing_prepaid_balance', $variables);
}

/**
 * Page callback for:
 *   users/%devconnect_monetization_developer/monetization/billing/billing-document
 *
 * @param \Apigee\Mint\Developer $developer
 * @return string
 */
function devconnect_monetization_billing_documents(\Apigee\Mint\Developer $developer) {
  $billing_documents_form = drupal_get_form('devconnect_monetization_billing_document_form', $developer);
  $billing_docs_collection = isset($billing_documents_form['#billing_docs']) ? $billing_documents_form['#billing_docs'] : array();
  unset($billing_documents_form['#billing_docs']);
  $docs = array();
  /** @var Apigee\Mint\BillingDocument $doc */
  foreach ($billing_docs_collection as &$doc) {
    switch ($doc->getType()) {
      case BillingDocumentType::REV_STMT:
        $type = 'Revenue Share Statement';
        break;

      case BillingDocumentType::GPA:
        $type = 'GPA';
        break;

      case BillingDocumentType::NPA:
        $type = 'NPA';
        break;

      case BillingDocumentType::SELF_INVOICE:
        $type = 'Self Invoice';
        break;

      case BillingDocumentType::INVOICE:
        $type = 'Invoice';
        break;

      case BillingDocumentType::NETTING_STMT:
        $type = 'Netting Statement';
        break;
      default: // should never get here!
        $type = NULL;
        break;
    }
    $products = array();
    /** @var Apigee\Mint\Product $product */
    foreach ($doc->getProducts() as $product) {
      $products[] = $product->getDisplayName();
    }
    $docs[] = array(
      'type' => $type,
      'ref' => $doc->getDocumentNumber(),
      'prods' => implode(', ', $products),
      'rec_date' => $doc->getBillingDocDate(),
      'id' => $doc->getId(),
    );
  }

  $variables = array(
    'billing_documents_form' => drupal_render($billing_documents_form),
    'billing_documents' => $docs,
  );
  return theme('devconnect_monetization_billing_docs', $variables);
}

function devconnect_monetization_top_up_balance_form($form, $form_state) {

  $config = devconnect_monetization_default_org_config();
  $organization = new Organization($config);
  $currencies = $organization->listSupportedCurrencies();

  $currency_options = array();
  /** @var Apigee\Mint\DataStructures\SupportedCurrency $currency */
  foreach ($currencies as $currency) {
    if ($currency->isVirtualCurrency()) {
      continue;
    }
    $currency_options[$currency->getName()] = $currency->getName() . ' ('. $currency->getDisplayName() . ')';
  }

  $form = array();

  $form['currency_id'] = array(
    '#type' => 'select',
    '#title' => t('Currency'),
    '#required' => TRUE,
    '#options' => $currency_options,
    '#empty_option' => t('- Select currency -'),
    '#empty_value' => -1,
  );

  $form['current_balance'] = array(
    '#type' => 'item',
    '#title' => t('Current balance:'),
    '#markup' => "0.0",
  );

  $form['top_up_amount'] = array(
    '#type' => 'textfield',
    '#title' => t('Amount to Top Up'),
    '#required' => TRUE,
    '#attributes' => array(
      'placeholder' => t('enter an amount'),
      'class' => array(
        'numeric',
        'currency',
      ),
      'currency' => '',
      'minimum' => 0,
    ),
    '#maxlength' => 12,
  );

  $form['new_balance'] = array(
    '#type' => 'item',
    '#title' => t('New balance:'),
    '#markup' => "0.0",
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Proceed to next step'),
    '#attributes' => array(
      'class' => array('btn', 'btn-primary'),
    ),
  );

  $form['#validate'][] = 'devconnect_monetization_top_up_balance_form_validate';
  $form['#submit'][] = 'devconnect_monetization_top_up_balance_form_submit';
  $form['#theme'] = 'devconnect_monetization_top_up_balance';
  $form['#attached']['js'][] = array(
    'data' => drupal_get_path('module', 'devconnect_monetization') . '/js/top-up-balance-form.js',
    'type' => 'file',
  );
  return $form;
}

function devconnect_monetization_top_up_balance_form_validate(&$form, &$form_state) {
  $top_up_amount = (double) $form_state['values']['top_up_amount'];
  $rate_plancurrency = isset($form_state['values']['currency']) ? $form_state['values']['currency'] : NULL;
  $currency = isset($form_state['values']['currency']) ? $form_state['values']['currency'] : NULL;
  // TODO: do something with these values.
}

function devconnect_monetization_top_up_balance_form_submit($form, &$form_state) {
  if (!user_access('top up mint balance')) {
    drupal_access_denied();
  }
  try {
    global $user;
    $values = array(
      'currency_id' => $form_state['values']['currency_id'],
      'amount' => $form_state['values']['top_up_amount'],
      'notes' => (isset($form_state['values']['notes'])) ? $form_state['values']['notes'] : '',
    );

    $deposit_order = devconnect_monetization_start_payment_order($user, $values);
    if (!empty($form_state['values']['order_data'])) {
      $deposit_order->data['purchase'] = array(
        'amount' => $form_state['values']['top_up_amount'],
        'oderid' => $deposit_order->order_id,
        'packageid' => $form_state['values']['order_data']['packageid'],
        'planid' => $form_state['values']['order_data']['planid'],
        'date' => $form_state['values']['order_data']['date'],
        'overlap' => $form_state['values']['order_data']['overlap'],
      );
    }

    commerce_order_save($deposit_order);
    $form_state['order_id'] = $deposit_order->order_id;
    $form_state['redirect'] = 'checkout/' . $deposit_order->order_id;
  }
  catch (Exception $e) {
    drupal_set_message(t('The website encountered an unexpected error. Please try again later.'), 'error');
    $message = (method_exists($e, '__toString') ? (string) $e : $e->getMessage());
    devconnect_default_org_config()->logger->critical($message);
  }
}

/**
 * Page callback for:
 *   users/%devconnect_monetization_developer/monetization/billing/report/download-prepaid-report/%/%
 *
 * @param $currency_id
 * @param $date Format is M-YYYY where M is full upper-case month name e.g. MAY, APRIL.
 * @param \Apigee\Mint\Developer $developer
 *
 * @throws Apigee\Mint\Exceptions\MintApiException
 * @throws Exception
 */
function devconnect_monetization_download_report($currency_id, $date, \Apigee\Mint\Developer $developer) {
  $currency_id = rawurldecode($currency_id);
  $date = rawurldecode($date);

  list($month, $year) = explode('-', $date);
  try {

    $developer_or_company_id = $developer->getEmail();
    // If there is a company in the context then download the report for the
    // company instead of the developer. So we replace the developer email with
    // the company machine name below.
    $company = apigee_company_get_current_context_company();
    if(!empty($company)){
      $developer_or_company_id = $company->name;
    }

    $org = new Organization(devconnect_monetization_default_org_config());
    $response = $org->getPrepaidBalanceReport($month, $year, $developer_or_company_id, $currency_id);

    header('Content-Disposition: attachment; filename="Prepaid_Balance-' . $month . $year . '_' . time() . '.csv"');
    header('Content-Type: application/octet-stream;charset=utf-8');
    header('Content-Length: ' . strlen($response));
    header('Connection: close');

    echo $response;
    module_invoke_all('exit');
    exit;
  }
  catch (MintApiException $cae) {
    if ($cae->getMintCode() == MintApiException::RESOURCE_DOES_NOT_EXIST) {
      drupal_set_message(t('There is no report for account @account in @month-@year', array(
        '@account' => $currency_id,
        '@month' => $month,
        '@year' => $year,
      )), 'error');
    }
    else {
      if ($GLOBALS['devconnect_monetization_debug_endpoint_response']) {
        drupal_set_message('DEBUG: ' . $cae->getMintMessage(TRUE), 'warning');
      }
      throw $cae;
    }
  }
  catch (Exception $e) {
    if ($e->getCode() == 404) {
      drupal_set_message(t('There is no report for account @account in @month-@year', array(
        '@account' => $currency_id,
        '@month' => $month,
        '@year' => $year,
          )), 'error');
    }
    else{
      drupal_set_message(t('The website encountered an unexpected error. Please try again later.'), 'error');
      $message = (method_exists($e, '__toString') ? (string) $e : $e->getMessage());
      devconnect_default_org_config()->logger->critical($message);
    }
  }
  $me = module_exists('me') ? 'me' : $GLOBALS['user']->uid;
  drupal_goto("users/$me/monetization/billing/prepaid-balance");
}

function devconnect_monetization_download_prepaid_report_form($form, $form_state) {

  $config = devconnect_monetization_default_org_config();

  $organization = new Organization($config);
  $currencies = $organization->listSupportedCurrencies();

  $billing_documents = new BillingDocument($config);
  $billing_months = $billing_documents->listBillingMonths();
  $years = array();
  /** @var Apigee\Mint\DataStructures\BillingMonth $billing_month */
  foreach ($billing_months as $billing_month) {
    if (!in_array($billing_month->getYear(), $years)) {
      $years[$billing_month->getYear()] = $billing_month->getYear();
    }
  }
  rsort($years);
  $accounts = array();
  /** @var Apigee\Mint\DataStructures\SupportedCurrency $currency */
  foreach ($currencies as $currency) {
    $accounts[$currency->getName()] = "{$currency->getName()} ({$currency->getDisplayName()})";
  }

  if (!empty($years)) {
    $years = array_combine($years, $years);
  }

  $form = array();
  $form['validation_message'] = array(
    '#markup' => t('<div id="download_previous_prepaid_statements_error_div" class="alert alert-block alert-error fade in hide"><h4 class="alert-heading">Make sure you have:</h4><p></p></div>'),
  );
  $form['account'] = array(
    '#type' => 'select',
    '#title' => t('Select an account'),
    '#options' => array(-1 => t('Select an account')) + $accounts,
  );
  $years = $years !== FALSE ? $years : array();

  $form['year'] = array(
    '#type' => 'select',
    '#title' => t('Select a year'),
    '#options' => array(-1 => t('Select a year')) + $years,
    '#ajax' => array(
      'callback' => 'devconnect_monetization_billing_get_billing_months_by_year',
    ),
  );
  $form['month'] = array(
    '#type' => 'select',
    '#title' => t('Select a month'),
    '#options' => array(-1 => t('Select a month')),
  );

  $form['#attached']['js'][] = array(
    'data' => drupal_get_path('module', 'devconnect_monetization') . '/js/previous-prepaid-balance.js',
    'type' => 'file',
  );

  $me = $GLOBALS['user']->uid;
  $js_settings = array(
    'download_url' => url('users/' . $me . '/monetization/billing/report/download-prepaid-report'),
    'select_account_message' => t('Selected an account'),
    'select_year_message' => t('Selected a year'),
    'select_month_message' => t('Selected a month'),
  );
  drupal_add_js(array('devconnect_monetization_previous_prepaid_statement' => $js_settings), 'setting');

  $form['submit'] = array(
    '#markup' => '<a id="download_previous_prepaid_statements_anchor" class="btn">' . t('Download CSV') . '</a>'
  );
  return $form;
}

function devconnect_monetization_billing_get_billing_months_by_year($form, &$form_state) {

  $year = $form_state['values']['year'];
  $billing_document = new BillingDocument(devconnect_monetization_default_org_config());
  $billing_months = $billing_document->listBillingMonths();
  $months = array();
  /** @var Apigee\Mint\DataStructures\BillingMonth $billing_month */
  foreach ($billing_months as $billing_month) {
    if ($billing_month->getYear() == $year) {
      $months[(int) $billing_month->getMonth()] = $billing_month->getMonthEnum();
    }
  }
  ksort($months);
  $options = t('<option value="-1">Select a month</option>');
  foreach ($months as $month) {
    $options .= '<option value="' . $month . '">' . t(ucwords(strtolower($month))) . '</option>';
  }

  $commands = array();
  $commands[] = ajax_command_replace('#devconnect-monetization-download-prepaid-report-form select:last option', $options);
  $commands[] = ajax_command_invoke('#devconnect-monetization-download-prepaid-report-form select:last option:first-child', 'attr', array(
    'disabled',
    TRUE,
  ));
  $commands[] = ajax_command_invoke('#devconnect-monetization-download-prepaid-report-form select:last option:first-child', 'attr', array(
    'selected',
    TRUE,
  ));
  $commands[] = ajax_command_invoke(NULL, 'fixBillingMonthSelect');
  return array('#type' => 'ajax', '#commands' => $commands);
}

/**
 * Form builder for page callback
 *   users/%devconnect_monetization_developer/monetization/billing/developer-reports
 *
 * @param array
 * @param array
 * @param \Apigee\Mint\Developer $developer
 * @return array
 */
function devconnect_monetization_developer_report_form($form, $form_state, \Apigee\Mint\Developer $developer) {

  $user = user_load_by_mail($developer->getEmail());
  $account = entity_load_single('user', $user->uid);

  $preferences = isset($account->field_mint_rev_reprt_preferences[LANGUAGE_NONE][0]['value']) ? json_decode($account->field_mint_rev_reprt_preferences[LANGUAGE_NONE][0]['value']) : NULL;
  $preferences = $preferences != NULL ? (array) $preferences : array();
  $org = new Organization(devconnect_monetization_default_org_config());
  $org->load();
  $currencies = $org->listSupportedCurrencies();
  $currency_options = array();
  $show_local_currency_option = FALSE;
  $count_default = 0;
  /** @var Apigee\Mint\DataStructures\SupportedCurrency $currency */
  foreach ($currencies as $currency) {
    if (!in_array($currency->getName(), array('GBP', 'EUR', 'USD'))) {
      $show_local_currency_option = TRUE;
    }
    else {
      $currency_options[$currency->getName()] = "({$currency->getName()}) {$currency->getDisplayName()}";
    }
    if (in_array($currency->getName(), array('GBP', 'EUR', 'USD'))) {
      $count_default++;
    }
  }

  if ($show_local_currency_option || count($currency_options) == 0 || $count_default > 1) {
    $currency_options = array('LOCAL' => t('Local Currency')) + $currency_options;
  }

  $form = array();

  $form['org_timezone'] = array(
    '#type' => 'value',
    '#value' => $org->getTimezone(),
  );

  $form['start_date'] = array(
    '#title' => t('From'),
    '#type' => 'textfield',
    '#required' => TRUE,
    '#date_format' => variable_get('devconnect_monetization_date_format', NORTH_AMERICAN_DATE_FORMAT),
    '#attributes' => array(
      'size' => 23,
      'class' => array('date'),
    ),
    '#default_value' => date(variable_get('devconnect_monetization_date_format', NORTH_AMERICAN_DATE_FORMAT), time()),
  );

  $form['end_date'] = array(
    '#title' => t('To'),
    '#type' => 'textfield',
    '#required' => TRUE,
    '#size' => 10,
    '#date_format' => variable_get('devconnect_monetization_date_format', NORTH_AMERICAN_DATE_FORMAT),
    '#attributes' => array(
      'size' => 23,
      'class' => array('date'),
    ),
    '#default_value' => date(variable_get('devconnect_monetization_date_format', NORTH_AMERICAN_DATE_FORMAT), time()),
  );

  $form['reporting_level'] = array(
    '#type' => 'radios',
    '#required' => TRUE,
    '#options' => array(
      'SUMMARY' => t('Summary Report'),
      'DETAILED' => t('Detailed Report'),
    ),
    '#default_value' => isset($preferences['reporting_level']) ? $preferences['reporting_level'] : 'SUMMARY',
  );

  if (count($currency_options) > 1) {
    list($default_currency) = array_keys($currency_options);
    $form['currency'] = array(
      '#type' => 'radios',
      '#required' => TRUE,
      '#options' => $currency_options,
      '#default_value' => isset($preferences['currency']) && array_key_exists($preferences['currency'], $currency_options) ? $preferences['currency'] : $default_currency,
    );
  }
  else {
    $form['currency'] = array(
      '#type' => 'value',
      '#value' => 'LOCAL',
    );
  }

  $form['#base_inputs'] = array(
    'date_range',
    'reporting_level',
    'currency',
  );

  $form['download_report'] = array(
    '#type' => 'submit',
    '#value' => t('Download Report (CSV)'),
  );

  $form['#attached']['js'][] = array(
    'data' => drupal_get_path('module', 'devconnect_monetization') . '/js/developer-reports-form.js',
    'type' => 'file',
  );
  $form['#validate'][] = 'devconnect_monetization_developer_report_validate';
  $form['#submit'][] = 'devconnect_monetization_developer_report_submit';
  $form['#theme'] = 'devconnect_monetization_developer_report_form';

  $js_settings = array(
    'date_format' => variable_get('devconnect_monetization_date_format', NORTH_AMERICAN_DATE_FORMAT),
  );

  drupal_add_js(array('devconnect_monetization_developer_reports' => $js_settings), 'setting');
  drupal_add_library('system', 'ui.datepicker');
  return $form;
}

function devconnect_monetization_developer_report_validate($form, &$form_state) {
  if ($form_state['values']['currency'] == '') {
    drupal_set_message(t('You must specify a currency'), 'error');
    form_set_error('currency', t('You must specify a currency'));
  }

  $org_timezone = new DateTimeZone($form_state['values']['org_timezone']);
  $start_date_string = $form_state['values']['start_date'] . ' 00:00:00';
  $start_date = DateTime::createFromFormat(variable_get('devconnect_monetization_date_format', NORTH_AMERICAN_DATE_FORMAT) . " H:i:s", $start_date_string, $org_timezone);

  $end_date_string = $form_state['values']['end_date'] . ' 00:00:00';
  $end_date = DateTime::createFromFormat(variable_get('devconnect_monetization_date_format', NORTH_AMERICAN_DATE_FORMAT) . " H:i:s", $end_date_string, $org_timezone);

  if (!is_object($start_date)) {
    form_set_error('start_date', t('Incorect date format in From field'));
  }
  else {
    $form_state['values']['start_date_parsed'] = $start_date;
  }

  if (!is_object($end_date)) {
    form_set_error('end_date', t('Incorrect date format in To field'));
  }
  else {
    $form_state['values']['end_date_parsed'] = $end_date;
  }

  if (count(form_get_errors() > 0)) {
    drupal_add_js('jQuery("div.reports.tabbable a[href=\'#tab2\']").tab("show");', 'inline');
  }
}

function devconnect_monetization_developer_report_submit(&$form, &$form_state) {
  if (!user_access('download mint revenue report')) {
    drupal_access_denied();
  }
  $dev = devconnect_monetization_developer_load();

  $report = new stdClass();
  $report->currencyOption = $form_state['values']['currency'];
  $report->transactionTypes = array(
    'PURCHASE',
    'CHARGE',
    'REFUND',
    'CREDIT',
    'SETUPFEES',
    'TERMINATIONFEES',
    'RECURRINGFEES',
  );
  if ($form_state['values']['reporting_level'] == 'SUMMARY') {
    $report->showSummary = TRUE;
    $report->groupBy = array('PACKAGE', 'PRODUCT', 'DEVELOPER', 'APPLICATION');
    $report_prefix_file_name = 'Summarized';
  }
  else {
    $report->showTxDetail = TRUE;
    $report_prefix_file_name = 'Detailed';
  }

  $utc_timezone = new DateTimeZone('UTC');
  $form_state['values']['end_date_parsed']->add(new DateInterval('P1D'));
  $report->fromDate = $form_state['values']['start_date_parsed']->setTimezone($utc_timezone)->format('Y-m-d H:i:s');
  $report->toDate = $form_state['values']['end_date_parsed']->setTimezone($utc_timezone)->format('Y-m-d H:i:s');

  $preferences = array(
    'start_date' => $form_state['values']['start_date'],
    'end_date' => $form_state['values']['end_date'],
    'reporting_level' => $form_state['values']['reporting_level'],
    'currency' => $form_state['values']['currency'],
  );

  $user = user_load_by_mail($dev->getEmail());
  if ($user === FALSE) {
    drupal_set_message(t('Your user does not exist in this DevConnect instance.'), 'error');
    return '';
  }
  $account = entity_load_single('user', $user->uid);
  $account->field_mint_rev_reprt_preferences[LANGUAGE_NONE][0]['value'] = json_encode($preferences);

  $field_info = field_info_field('field_mint_rev_reprt_preferences');
  $fields = array($field_info['id']);

  if (isset($account->field_mint_rev_reprt_preferences[LANGUAGE_NONE][0]['value'])) {
    field_sql_storage_field_storage_write('user', $account, 'update', $fields);
  }
  else {
    field_sql_storage_field_storage_write('user', $account, 'insert', $fields);
  }
  cache_clear_all("field:user:" . $user->uid, 'cache_field');

  $company = apigee_company_get_current_context_company();
  // If the user is in a company context then show him the company reports.
  if ($company) {
    // @TODO: This should be handled in the SDK later.
    $dev->setEmail($company->name);
  }

  try {
    $response = $dev->getRevenueReport($report);

    header('Content-Disposition: attachment; filename="' . $report_prefix_file_name . '_Revenue_Report-' . $report->fromDate . '_to_' . $report->toDate . '-' . time() . '.csv"');
    header('Content-Type: application/octet-stream;charset=utf-8');
    header('Content-Length: ' . strlen($response));
    header('Connection: close');

    echo $response;
    module_invoke_all('exit');
    exit();
  } catch (Exception $e) {
    if ($e->getCode() == 404) {
      drupal_set_message(t('There is no data within this date range. Please try another date range.'), 'warning');
    }
    else {
      drupal_set_message(t('There was an error trying to download your report. Please try again, if the problem persists please contact your administrator.'), 'error');
    }
  }
}

/**
 * Form builder called by menu callback devconnect_monetization_billing_documents
 *
 * @param $form
 * @param $form_state
 * @param \Apigee\Mint\Developer $developer
 * @return array
 */
function devconnect_monetization_billing_document_form($form, &$form_state, \Apigee\Mint\Developer $developer) {

  $billing_document = new BillingDocument(devconnect_monetization_default_org_config());
  $billing_months = $billing_document->listBillingMonths();

  $months = array();
  /** @var Apigee\Mint\DataStructures\BillingMonth $billing_month */
  foreach ($billing_months as $billing_month) {
    // Build the key as float made out of Y as interger part and month as decimal part pad with 0 e.g. January 2013 = 2013.01
    // This way we can sort them descending
    $months[(float) $billing_month->getYear() . '.' . str_pad($billing_month->getMonth(), 2, '0', STR_PAD_LEFT)] = $billing_month;
  }

  // Sort billing months by descending order
  krsort($months);

  $opts = array();
  foreach ($months as $billing_month) {
    $opts[$billing_month->getMonthEnum() . '-' . $billing_month->getYear()] = ucfirst(strtolower($billing_month->getMonthEnum())) . ' ' . $billing_month->getYear() . ' - ' . $billing_month->getStatus();
  }

  $form = array();

  $form['billing_month'] = array(
    '#title' => t('Billing Month'),
    '#type' => 'select',
    '#options' => $opts,
  );


  $form['search-billing-doc'] = array(
    '#type' => 'textfield',
    '#title_display' => 'invisible',
    '#attributes' => array(
      'class' => array('input-large', 'search-query'),
      'placeholder' => t('Search Billing Documents'),
    ),
  );

  $form['search'] = array(
    '#type' => 'submit',
    '#value' => t('Go'),
    '#attributes' => array(
      'class' => array('btn'),
    ),
  );

  $form['list'] = array(
    '#type' => 'submit',
    '#value' => 'list',
    '#attributes' => array('style' => 'display:none;'),
  );

  $form += array(
    '#attributes' => array(
      'class' => array('form-search', 'form-horizontal'),
    ),
    '#attached' => array(
      'js' => array(
        array(
          'data' => drupal_get_path('module', 'devconnect_monetization') . '/js/received-bills-form.js',
          'type' => 'file',
        ),
      ),
    ),
  );

  if (isset($form_state['data'])) {
    $form['#billing_docs'] = $form_state['data'];
    $form['#submited'] = TRUE;
  }
  else {
    if (count($opts) > 0) {
      list($default_billing_month) = array_keys($opts);
      list($month, $year) = explode('-', $default_billing_month);

      $developer_or_company_id = $developer->getEmail();
      // If there is a company in the context then fetch the billing
      // documents for the company instead of the developer. So we replace the
      // developer email with the company machine name.
      $company = apigee_company_get_current_context_company();
      if(!empty($company)){
        $developer_or_company_id = $company->name;
      }

      $form['#billing_docs'] = $billing_document->getBillingDocuments($developer_or_company_id, strtoupper($month), $year, TRUE);
      $form['#submited'] = FALSE;
    }
  }

  $form['#submit'][] = 'devconnect_monetization_billing_document_form_submit';
  return $form;
}

function devconnect_monetization_billing_document_form_submit(&$form, &$form_state) {

  try {
    if (!user_access('download mint billing documents')) {
      drupal_access_denied();
    }
    $developer_id = _devconnect_monetization_get_developer_id(TRUE);

    $developer_or_company_id = $developer_id;
    // If there is a company in the context then search the document for the
    // company instead of the developer. So we replace the developer email with
    // the company machine name.
    $company = apigee_company_get_current_context_company();
    if (!empty($company)) {
      $developer_or_company_id = $company->name;
    }

    list($month, $year) = explode('-', $form_state['values']['billing_month']);
    $billing_document = new BillingDocument(devconnect_monetization_default_org_config());
    if (strlen($form_state['values']['search-billing-doc'])) {
      try {
        $billing_docs = $billing_document->searchBillingDoc($form_state['values']['search-billing-doc'], $developer_or_company_id);
      }
      catch (MintApiException $re) {
        drupal_set_message(t("Error: %error", array('%error' => $re->getMintMessage(TRUE, TRUE))), 'warning');
        return;
      }
    }
    else {
      $billing_docs = $billing_document->getBillingDocuments($developer_or_company_id, strtoupper($month), $year, TRUE);
    }

    $form_state['rebuild'] = TRUE;
    $form_state['data'] = $billing_docs;
  } catch (Exception $e) {
    drupal_set_message(t('Could not retrieve billing documents. Please try again later'), 'error');
  }
}

/**
 * Page callback for:
 *   users/%devconnect_monetization_developer/monetization/billing/billing-document/%
 *
 * @param $doc_id
 * @param \Apigee\Mint\Developer $developer
 * @return string
 */
function devconnect_monetization_billing_get_billing_document($doc_id, \Apigee\Mint\Developer $developer) {
  try {
    $doc_id = rawurldecode($doc_id);
    $doc = new BillingDocument(devconnect_monetization_default_org_config());
    $doc->load($doc_id);
    $report = $doc->getFile();

    drupal_add_http_header('Content-Disposition', 'filename="Billing_Document-' . $doc->getDocumentNumber() . '_' . time() . '.html');
    drupal_add_http_header('Content-Type', 'application/octet-stream');
    drupal_add_http_header('Content-Length', $report['length']);
    drupal_add_http_header('Connection', 'close');
    echo $report['content'];
    drupal_exit();
  } catch (Exception $e) {
    drupal_set_message(t('Error while trying to serve your Billing Document. Please try again later.'), 'error');
    $message = (method_exists($e, '__toString') ? (string)$e : $e->getMessage());
    devconnect_default_org_config()->logger->error($message);
  }
  return "";
}

/**
 * Create an order to top up
 *
 * @param $user
 * @param $values
 *  keys:
 *    currency_id: Id of the currency to top up
 *    amount: the amount to top up
 *    notes: Notes to include in the the order
 * @return mixed order object
 */
function devconnect_monetization_start_payment_order($user, $values) {
  $currency = strtoupper($values['currency_id']);

  $deposit = commerce_product_new('deposit');

  $deposit->uid = $user->uid;
  $deposit->sku = 'deposit_' . $user->uid . '_' . $values['amount'] . '_' . microtime(TRUE);

  $deposit->commerce_price['und'][0]['amount'] = commerce_currency_decimal_to_amount($values['amount'], $currency);

  $deposit->commerce_price['und'][0]['currency_code'] = $currency;

  $deposit->title = t('Deposit @amount', array('@amount' => commerce_currency_decimal_to_amount($values['amount'], $currency)));

  commerce_product_save($deposit);

  $deposit_order = ($user->uid) ? commerce_order_new($user->uid, 'checkout_checkout') : commerce_cart_order_new();

  $line_item = commerce_product_line_item_new($deposit, 1, $deposit_order->order_id);

  commerce_line_item_save($line_item);

  $order_wrapper = entity_metadata_wrapper('commerce_order', $deposit_order);

  $order_wrapper->commerce_line_items[] = $line_item;


  $deposit_order->data['type'] = 'commerce_funds_deposit';
  $deposit_order->data['notes'] = $values['notes'];

  commerce_order_save($deposit_order);

  return $deposit_order;
}
