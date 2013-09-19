<?php

use Apigee\Mint\Types\BillingType;
use Apigee\Mint\Types\BillingDocumentType;
use Apigee\Mint\BillingDocument;
use Apigee\Mint\ReportDefinition;
use Apigee\Util\Debugger;
use Apigee\Mint\Exceptions\MintApiException;
use Apigee\Mint\Organization;
use Apigee\Mint\DataStructures\Address;
use Apigee\Mint\DataStructures\DeveloperBalance;
use Apigee\Mint\Developer;

function devconnet_monetization_billing_tabs($user, $active_tab) {
  drupal_set_title(t('Billing & Reports'));
  $me = module_exists('me') ? 'me' : $GLOBALS['user']->uid;
  $base_path = 'users/' . $me . '/monetization/billing';
  $tabs = array(
    'attributes' => array(
      array(
        'name' => 'id',
        'value' => 'billing_and_reports_tabs',
      ),
    ),
    'tabs' => array(
      $base_path . '/prepaid-balance' => array(
        'text' => t('Prepaid Balance'),
        'access callback' => 'devconnect_monetization_billing_prepaid_access',
        'access args' => array('access prepaid reports'),
        'active' => 'prepaid-balance' == $active_tab,
        'callback' => 'devconnect_monetization_prepaid_balance',
      ),
      $base_path . '/developer-reports' => array(
        'text' => t('Reports'),
        'access callback' => 'user_access',
        'access args' => array('download revenue report'),
        'active' => 'developer-reports' == $active_tab,
        'callback' => 'drupal_get_form',
        'callback args' => array('devconnect_monetization_developer_report_form'),
      ),
      $base_path . '/billing-document' => array(
        'text' => t('Received Bills'),
        'access callback' => 'user_access',
        'access args' => array('download billing documents'),
        'pane' => array ('class' => 'tnc'),
        'active' => 'billing-document' == $active_tab,
        'callback' => 'devconnect_monetization_billing_documents',
      ),
    )
  );
  if (!array_key_exists($base_path . '/' . $active_tab, $tabs['tabs'])) {
    drupal_not_found();
  }
  module_load_include('inc', 'devconnect_monetization', 'devconnect_monetization.tabs');
  $output = devconnect_monetization_theme_tabs($tabs);
  return $output;
}

function devconnect_monetization_billing_prepaid_access() {
  $args = func_get_args();
  $developer_id = _devconnect_monetization_get_developer_id(TRUE);
  $client = devconnect_monetization_default_api_client();
  $developer = new Developer($client);
  $developer->load($developer_id);
  return $developer->getBillingType() == BillingType::PREPAID && call_user_func_array('user_access', $args);
}

function devconnect_monetization_prepaid_balance() {
  $developer_id = _devconnect_monetization_get_developer_id(TRUE);
  $client = devconnect_monetization_default_api_client();
  $developer = new Developer($client);
  $developer->setEmail($developer_id);

  $access_prepaid_reports = user_access('access prepaid reports');
  if ($access_prepaid_reports) {
    $top_up_balance_perm = user_access('top up balance');
    if ($top_up_balance_perm) {
      $form = drupal_get_form('devconnect_monetization_top_up_balance_form');
      $top_up_form = drupal_render($form);
    }
    $download_prepaid_report_perm = user_access('download prepaid report');
    $organization = new Organization($client);
    $currencies = $organization->listSupportedCurrencies();

    $billing_documents = new BillingDocument($client);
    $billing_months = $billing_documents->listBillingMonths();
    $years = array();
    foreach ($billing_months as $billing_month) {
      if (!in_array($billing_month->getYear(), $years)) {
        $years[$billing_month->getYear()] = $billing_month->getYear();
      }
    }
    rsort($years);
    $months = array();
    foreach ($billing_months as $billing_month) {
      if ($billing_month->getYear() == $years[0]) {
        $months[] = $billing_month->getMonth();
      }
    }
    $accounts = array();
    foreach ($currencies as $currency) {
      $accounts[$currency->name] = "{$currency->name} ({$currency->displayName})";
    }
    $previous_prepaid_statements_form = drupal_get_form('devconnect_monetization_dowload_prepaid_report_form', $accounts, $years);
    $balances = $developer->getPrepaidBalance();
    $topped_up_currencies = 0;
    foreach ($balances as $balance) {
      foreach ($currencies as $currency) {
        if ($balance->supportedCurrency->id == $currency->id) {
          $topped_up_currencies++;
          break;
        }
      }
    }
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
  );
  drupal_add_js(drupal_get_path('module', 'devconnect_monetization') . '/js/global.js', 'file');
  return theme('billing_prepaid_balance', $variables);
}

function devconnect_monetization_billing_documents() {
  $billing_documents_form = drupal_get_form('devconnect_monetization_billing_document_form');
  $billing_docs_collection = isset($billing_documents_form['#billing_docs']) ? $billing_documents_form['#billing_docs'] : NULL;
  unset($billing_documents_form['#billing_docs']);
  $docs = array();
  foreach ($billing_docs_collection as &$doc) {
    switch ($doc->getType()) {
      case BillingDocumentType::REV_STMT: $type = 'Revenue Share Statement'; break;
      case BillingDocumentType::GPA: $type = 'GPA'; break;
      case BillingDocumentType::NPA: $type = 'NPA'; break;
      case BillingDocumentType::SELF_INVOICE: $type = 'Self Invoice'; break;
      case BillingDocumentType::INVOICE: $type = 'Invoice'; break;
      case BillingDocumentType::NETTING_STMT: $type = 'Netting Statement';  break;
    }
    $products = array();
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
  if (isset($form_state['values']['top_up_amount'])) {
    $new_form_state = array(
      'values' => array(
        'notes' => '',
        'amount' => $form_state['values']['top_up_amount'],
        'currency_id' => $form_state['values']['currency_id'],
      ),
    );

    devconnect_monetization_top_up_balance_form_submit($form, $new_form_state);
  }

  $form['top_up_amount'] = array(
    '#type' => 'hidden',
  );

  $form['currency_id'] = array(
    '#type' => 'hidden',
  );

  $form['#validate'][] = 'devconnect_monetization_top_up_balance_form_validate';
  $form['#submit'][] = 'devconnect_monetization_top_up_balance_form_submit';

  return $form;
}

function devconnect_monetization_top_up_balance_form_validate(&$form, &$form_state) {
  $top_up_amount = (double)$form_state['values']['top_up_amount'];
  $rate_plancurrency = isset($form_state['values']['currency']) ? $form_state['values']['currency'] : NULL;
  $currency = isset($form_state['values']['currency']) ? $form_state['values']['currency'] : NULL;
}

function devconnect_monetization_top_up_balance_form_submit(&$form, &$form_state) {
  if (!user_access('top up balance')) {
    drupal_access_denied();
  }
  try {
    global $user;
    $currency = strtoupper($form_state['values']['currency_id']);

    $deposit = commerce_product_new('deposit');

    $deposit->uid = $user->uid;
    $deposit->sku = 'deposit_' . $user->uid . '_' . $form_state['values']['amount'] . '_' . microtime();

    $deposit->commerce_price['und'][0]['amount'] = $form_state['values']['amount'] * 100;
    $deposit->commerce_price['und'][0]['currency_code'] = $currency;

    $deposit->title = t('Deposit @amount', array('@amount' => commerce_currency_format($form_state['values']['amount'] * 100, $currency)));

    commerce_product_save($deposit);

    $deposit_order = ($user->uid) ? commerce_order_new($user->uid, 'checkout_checkout') : commerce_cart_order_new();

    $line_item = commerce_product_line_item_new($deposit, 1, $deposit_order->order_id);

    commerce_line_item_save($line_item);

    $order_wrapper = entity_metadata_wrapper('commerce_order', $deposit_order);

    $order_wrapper->commerce_line_items[] = $line_item;


    $deposit_order->data['type'] = 'commerce_funds_deposit';
    $deposit_order->data['notes'] = $form_state['values']['notes'];

    $plan_id = isset($_REQUEST['planid']) && strlen($_REQUEST['planid']) ? $_REQUEST['planid'] : NULL;
    if (isset($plan_id)) {
      $deposit_order->data['purchase'] = array(
        'packageid' => rawurldecode(arg(4)),
        'planid' => $plan_id,
        'date' => $_REQUEST['date'],
        'overlap' => $_REQUEST['overlap'],
      );
    }

    commerce_order_save($deposit_order);

    drupal_goto('checkout/' . $deposit_order->order_id);

  }
  catch(Exception $e) {
    drupal_set_message(t('The website encountered an unexpected error. Please try again later.'), 'error');
    Apigee\Util\Log::write('devconnect_monetization', Apigee\Util\Log::LOGLEVEL_CRITICAL, $e);
  }
}

function devconnect_monetization_download_report($currency_id, $date) {
  $currency_id = rawurldecode($currency_id);
  $date = rawurldecode($date);

  try {
    $developer_id = _devconnect_monetization_get_developer_id(TRUE);
    $client = devconnect_monetization_default_api_client();

    list($month, $year) = explode('-', $date);

    $org = new Organization($client);
    $response = $org->getPrepaidBalanceReport($month, $year, $developer_id, $currency_id);

    header('Content-Disposition: attachment; filename="Prepaid_Balance-' . $month . $year . '_' . time() . '.csv"');
    header('Content-Type: application/octet-stream;charset=utf-8');
    header('Content-Length: ' . strlen($response));
    header('Connection: close');

    echo $response;
    return;
  }
  catch (MintApiException $cae) {
    if ( $cae->getMintCode() == MintApiException::RESOURCE_DOES_NOT_EXIST) {
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
    drupal_set_message(t('The website encountered an unexpected error. Please try again later.'), 'error');
    Apigee\Util\Log::write('devconnect_monetization', Apigee\Util\Log::LOGLEVEL_CRITICAL, $e);
  }
  $me = module_exists('me') ? 'me' : $GLOBALS['user']->uid;
  drupal_goto("users/$me/monetization/billing/prepaid-balance");
}

function devconnect_monetization_dowload_prepaid_report_form($form, $form_state) {
  $accounts = $form_state['build_info']['args'][0];
  $years = array_combine($form_state['build_info']['args'][1], $form_state['build_info']['args'][1]);
  $form = array();
  $form['validation_message'] = array(
    '#markup' => t('<div id="donwload_previous_prepaid_statements_error_div" class="alert alert-block alert-error fade in hide"><h4 class="alert-heading">Make sure you have:</h4><p></p></div>'),
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
  $form['submit'] = array(
    '#markup' => '<a id="donwload_previous_prepaid_statements_anchor" class="btn">' . t('Download CSV') . '</a>'
  );
  return $form;
}

function devconnect_monetization_billing_get_billing_months_by_year($form, &$form_state) {

  $year = $form_state['values']['year'];
  $client = devconnect_monetization_default_api_client();
  $billing_document = new BillingDocument($client);
  $billing_months = $billing_document->listBillingMonths();
  $months = array();
  foreach ($billing_months as $billing_month) {
    if ($billing_month->getYear() == $year) {
      $months[(int)$billing_month->getMonth()] = $billing_month->getMonthEnum();
    }
  }
  ksort($months);
  $options = t('<option value="-1">Select a month</option>');
  foreach ($months as $month) {
    $options .= '<option value="' . $month . '">' . t(ucwords(strtolower($month))) . '</option>';
  }

  $commands = array();
  $commands[] = ajax_command_replace('#devconnect-monetization-dowload-prepaid-report-form select:last option', $options);
  $commands[] = ajax_command_invoke('#devconnect-monetization-dowload-prepaid-report-form select:last option:first-child', 'attr', array('disabled', true));
  $commands[] = ajax_command_invoke('#devconnect-monetization-dowload-prepaid-report-form select:last option:first-child', 'attr', array('selected', true));
  $commands[] = ajax_command_invoke(NULL, 'fixBillingMonthSelect');
  return array('#type' => 'ajax', '#commands' => $commands);
}

function devconnect_monetization_developer_report_form() {
  $client = devconnect_monetization_default_api_client();
  $developer_id = _devconnect_monetization_get_developer_id(TRUE);

  $user = user_load_by_mail($developer_id);
  $account = entity_load_single('user', $user->uid);

  $preferences = isset($account->field_mint_rev_reprt_preferences[LANGUAGE_NONE][0]['value']) ? json_decode($account->field_mint_rev_reprt_preferences[LANGUAGE_NONE][0]['value']) : NULL;
  $preferences = $preferences != NULL ? (array)$preferences : array();
  $org = new Organization($client);
  $org->load();
  $currencies = $org->listSupportedCurrencies();
  $currs = array();
  $show_local_currency_option = FALSE;
  $count_default = 0;
  foreach ($currencies as $currency) {
    if (!in_array($currency->name, array('GBP', 'EUR', 'USD'))) {
      $show_local_currency_option = TRUE;
    }
    else {
      $currs[$currency->name] = "({$currency->name}) {$currency->displayName}";
    }
    if (in_array($currency->name, array('GBP', 'EUR', 'USD'))) {
      $count_default++;
    }
  }

  if ($show_local_currency_option ||  count($currs) == 0 || $count_default > 1) {
    $currs = array('LOCAL' => t('Local Currency')) + $currs;
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
      '#date_format' => 'j F Y',
      '#attributes' => array(
          'size' => 23,
          'class' => array('date')
      ),
      '#default_value' => date('m/d/Y', time()),
  );

  $form['end_date'] = array(
      '#title' => t('To'),
      '#type' => 'textfield',
      '#required' => TRUE,
      '#size' => 10,
      '#date_format' => 'j F Y',
      '#attributes' => array(
          'size' => 23,
          'class' => array('date')
      ),
      '#default_value' => date('m/d/Y', time()),
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

  if (count($currs) > 1) {
    list($default_currency) = array_keys($currs);
    $form['currency'] = array(
        '#type' => 'radios',
        '#required' => TRUE,
        '#options' => $currs,
        '#default_value' => isset($preferences['currency']) && array_key_exists($preferences['currency'], $currs) ? $preferences['currency'] : $default_currency,
    );
  }
  else {
    $currs_keys = array_keys($currs);
    $form['currency'] = array(
        '#type' => 'value',
        '#value' => 'LOCAL',
    );
  }

  $form['report_name'] = array(
      '#type' => 'textfield',
  );
  $form['report_desc'] = array(
      '#type' => 'textfield',
      '#attributes' => array('placeholder' => t('Enter a description...')),
  );
  $form['#base_inputs'] = array(
      'date_range',
      'reporting_level',
      'currency',
      'report_name',
      'report_desc',
  );

  $form['download_report'] = array(
      '#type' => 'submit',
      '#value' => t('Download Report (CSV)'),
  );

  $form['#attached']['js'][] = drupal_get_path('module', 'devconnect_monetization') . '/js/global.js';
  $form['#validate'][] = 'devconnect_monetization_developer_report_validate';
  $form['#submit'][] = 'devconnect_monetization_developer_report_submit';
  $form['#theme'] = 'devconnect_monetization_developer_report_form';

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
  $separator = strpos($start_date_string, '/') > 0 ? '/' : '-';
  $start_date = DateTime::createFromFormat("m{$separator}d{$separator}Y H:i:s", $start_date_string, $org_timezone);

  $end_date_string = $form_state['values']['end_date'] . ' 00:00:00';
  $separator = strpos($end_date_string, '/') > 0 ? '/' : '-';
  $end_date = DateTime::createFromFormat("m{$separator}d{$separator}Y H:i:s", $end_date_string, $org_timezone);

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

  if (count(form_get_errors()>0)) {
    drupal_add_js('jQuery("div.reports.tabbable a[href=\'#tab2\']").tab("show");', 'inline');
  }
}

function devconnect_monetization_developer_report_submit(&$form, &$form_state) {
  if (!user_access('download revenue report')) {
    drupal_access_denied();
  }
  $client = devconnect_monetization_default_api_client();
  $developer_id = _devconnect_monetization_get_developer_id(TRUE);
  $dev = new Developer($client);
  $dev->setEmail($developer_id);

  $report = new stdClass();
  $report->currencyOption = $form_state['values']['currency'];
  $report->transactionTypes = array('PURCHASE', 'CHARGE', 'REFUND', 'CREDIT', 'SETUPFEES', 'TERMINATIONFEES', 'RECURRINGFEES');
  if ($form_state['values']['reporting_level'] == 'SUMMARY') {
    $report->showSummary = true;
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

  $user = user_load_by_mail($developer_id);
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
  try {
    $response = $dev->getRevenueReport($report);

    header('Content-Disposition: attachment; filename="' . $report_prefix_file_name . '_Revenuew_Report-' . $report->fromDate . '_to_' . $report->toDate . '-' . time() . '.csv"');
    header('Content-Type: application/octet-stream;charset=utf-8');
    header('Content-Length: ' . strlen($response));
    header('Connection: close');

    echo $response;
    die();
  }
  catch(Exception $e) {
    if ($e->getCode() == 404) {
      drupal_set_message(t('There is no data within this date range. Please try another date range'), 'warning');
    }
    else {
      drupal_set_message(t('There was an error trying to download your report. Please try again, if the problem persists please contact your administrator.'), 'error');
    }
  }
}

function devconnect_monetization_billing_document_form($form, &$form_state) {
  $developer_id = _devconnect_monetization_get_developer_id(TRUE);
  $client = devconnect_monetization_default_api_client();

  $billing_document = new BillingDocument($client);
  $billing_months = $billing_document->listBillingMonths();

  $months = array();
  foreach ($billing_months as $billing_month) {
    // Build the key as float made of Y as interger part and month as decimal part pad with 0 e.g. January 2013 = 2013.01
    // This way we can sort them descending
    $months[(float)$billing_month->getYear() . '.' . str_pad($billing_month->getMonth(), 2, '0', STR_PAD_LEFT)] = $billing_month;
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
    '#attributes' => array('onchange' => 'javascript: jQuery("#edit-search-billing-doc").val(""); jQuery("#devconnect-monetization-billing-document-form").submit();'),
  );


  $form['search-billing-doc'] = array(
      '#type' => 'textfield',
      '#title_display' => 'invisible',
      '#attributes' => array(
          'class' => array('input-large', 'search-query'),
          'placeholder' => t('Search Billing Documents'),
      ),
      '#prefix' => '<div style="float:right;margin-top:-46px;display:none;">',
  );

  $form['search'] = array(
      '#type' => 'submit',
      '#value' => t('Go'),
      '#attributes' => array(
          'class' => array('btn'),
      ),
      '#suffix' => '</div>',
  );

  $form['list'] = array(
      '#type' => 'submit',
      '#value' => 'list',
      '#attributes' => array('style' => 'display:none;'),
  );

  $unwrap_script = 'jQuery(document).ready(function(){ jQuery("#edit-search-billing-doc").unwrap().unwrap().parent().show(); });';
  $form += array(
      '#attributes' => array(
          'class' => array('form-search', 'form-horizontal'),
      ),
      '#attached' => array(
          'js' => array(
              $unwrap_script => array('type' => 'inline'),
          ),
      ),
  );

  if (isset($form_state['data'])) {
    $form['#billing_docs'] = $form_state['data'];
    $form['#submited'] = TRUE;
  }
  else if (count($opts) > 0) {
    list($default_billing_month) = array_keys($opts);
    list($month, $year) = explode('-', $default_billing_month);

    //@TODO Remove FALSE parameter once confirmation of field values is right
    $form['#billing_docs'] = $billing_document->getBillingDocuments($developer_id, strtoupper($month), $year, TRUE);
    $form['#submited'] = FALSE;
  }

  $form['#submit'][] = 'devconnect_monetization_billing_document_form_submit';
  return $form;
}

function devconnect_monetization_billing_document_form_submit(&$form, &$form_state) {

  try {
    if (!user_access('download billing documents')) {
      drupal_access_denied();
    }
    $developer_id = _devconnect_monetization_get_developer_id(TRUE);
    $client = devconnect_monetization_default_api_client();

    list($month, $year) = explode('-', $form_state['values']['billing_month']);
    $billing_document = new BillingDocument($client);
    //@TODO Remove FALSE parameter once confirmation of field values is right
    if (strlen($form_state['values']['search-billing-doc'])) {
      try {
        $billing_docs = $billing_document->searchBillingDoc($form_state['values']['search-billing-doc'], $developer_id);
      }
      catch (MintApiException $re) {
        drupal_set_message($re->getMintMessage(TRUE, TRUE), 'warning');
        return;
      }
    }
    else {
      $billing_docs = $billing_document->getBillingDocuments($developer_id, strtoupper($month), $year, TRUE);
    }

    $form_state['rebuild'] = TRUE;
    $form_state['data'] = $billing_docs;
  }
  catch(Exception $e) {
    drupal_set_message(t('Could not retrieve billing documents. Please try again later'), 'error');
  }
}

function devconnect_monetization_billing_get_billing_document($doc_id) {
  try {
    $doc_id = rawurldecode($doc_id);
    $client = devconnect_monetization_default_api_client();
    $doc = new BillingDocument($client);
    $doc->load($doc_id);
    $file = $doc->getFile();

    header('Content-Disposition: attachment; filename="Billing_Document-' . $doc->getDocumentNumber() . '_' . time() . '.htm"');
    header('Content-Type: application/octet-stream');
    header('Content-Length: ' . strlen($file));
    header('Connection: close');

    echo (string)$file;
    die();
  }
  catch (Exception $e) {
    drupal_set_message(t('Error while trying to serve your Billing Document. Please try again later.'), 'error');
    Apigee\Util\Log::write('devconnect_monetization', Apigee\Util\Log::LOGLEVEL_ERROR, $e);
  }
}
