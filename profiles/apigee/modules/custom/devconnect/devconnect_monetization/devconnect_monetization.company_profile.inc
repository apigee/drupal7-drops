<?php

/**
 * @file
 * Company methods.
 */
use Apigee\Mint\DeveloperBalance;
use Apigee\Mint\Provider;
use Apigee\Util\CacheFactory;
use Apigee\Mint\Organization;
use Apigee\Mint\DataStructures\DeveloperTnc;
use Apigee\Mint\TermAndCondition;
use Apigee\Mint\DataStructures\Address;
use Apigee\Mint\Types\BillingType;
use Apigee\Mint\Types\Country;

/**
 * Page callback for users/%devconnect_monetization_developer/monetization/company/developers
 *
 * @param \Apigee\Mint\Developer $developer
 * @return string
 */
function devconnect_monetization_company_users_forms(\Apigee\Mint\Developer $developer) {
  // Manage Users tab
  $company = apigee_company_get_current_context_company();
  if (!$company) {
    $output = '<div class="default-company-create-new">';
    $output .= t('If you want to collaborate with others, !create_company or if you have already created a company switch to that company.', array('!create_company' => l('Create a company', 'api_company/developer/company/add')));
    $output .= '</div>';
    return $output;
  }


  $output = apigee_company_company_developers();
  return $output;
}

/**
 * Form builder for page callback:
 *   users/%devconnect_monetization_developer/monetization/company
 *   users/%devconnect_monetization_developer/monetization/company/edit
 *
 * @param $form
 * @param $form_state
 * @param $developer
 * @return array
 */
function devconnect_monetization_company_details_form($form, &$form_state, $developer) {
  $form = array();

  // Load the company if there is a company in context.
  $company = apigee_company_get_current_context_company();
  if (!empty($company)) {
    $developer = $company;
  }

  // Load either the developer or company depending on context.
  if (($developer instanceof \Apigee\Mint\Developer)) {
    $entity_type = 'developer';
    $devs = entity_load('developer', array('mail' => $developer->getEmail()));
    if (count($devs)) {
      $dev = reset($devs);
    }
    else {
      $dev = new Drupal\devconnect_user\DeveloperEntity();
    }
    $contact_name = $dev->firstName . ' ' . $dev->lastName;
  }
  else {
    // This is a Company, not a developer and name attribute is the unique id.
    $entity_type = 'apigee_company';
    if ($developer) {
      $devs = entity_load('apigee_company', array($company->name));
      $dev = reset($devs);
    }
    else {
      $dev = new ApigeeCompanyEntity();
    }
    $form['company_name'] = array(
      '#type' => 'value',
      '#value' => $company->name,
    );
    $contact_name = isset($dev->attributes['MINT_DEVELOPER_LEGAL_NAME']) ? $dev->attributes['MINT_DEVELOPER_LEGAL_NAME'] : '';
  }

  $form['entity_type'] = array(
    '#type' => 'value',
    '#value' => $entity_type,
  );


  $form['company_details'] = array(
    '#type' => 'fieldset',
    '#title' => t('Company Details'),
    '#collapsible' => FALSE,
  );

  $form['company_details']['legal_company_name'] = array(
    '#title' => t('Legal Company Name'),
    '#type' => 'textfield',
    '#required' => TRUE,
    '#default_value' => $dev->getAttribute('MINT_DEVELOPER_LEGAL_NAME'),
  );

  $form['company_details']['company_reg_number'] = array(
    '#title' => t('Company Reg. Number'),
    '#type' => 'textfield',
    '#default_value' => $dev->getAttribute('MINT_REGISTRATION_ID'),
  );

  if (($developer instanceof \Apigee\Mint\Developer)) {
    foreach ($developer->getAddresses() as $address_item) {
      if ($address_item->isPrimary()) {
        $address = $address_item;
        break;
      }
    }
  }
  else {
    $address = new Address(json_decode($dev->attributes['MINT_DEVELOPER_ADDRESS'], TRUE));
  }

  if (!isset($address)) {
    $address = new Address(array('country' => 'US', 'isPrimary' => TRUE));
  }

  $form['billing_address'] = array(
    '#type' => 'fieldset',
    '#title' => t('Contact and Billing Details'),
    '#collapsible' => FALSE,
  );

  $form['billing_address']['contact_name'] = array(
    '#type' => 'item',
    '#title' => t('Contact Name'),
    '#markup' => t('@contact_name', array(
      '@contact_name' => $contact_name,
    )),
  );

  // If the object is a Developer, then use it's email, else it is a company, get
  // the email from the company admin.
  if (($developer instanceof \Apigee\Mint\Developer)) {
    $email = $developer->getEmail();
  }
  else {
    $email = $dev->attributes['ADMIN_EMAIL'];
  }

  $form['billing_address']['contact_email'] = array(
    '#type' => 'item',
    '#title' => t('Email Address'),
    '#markup' => $email,
  );

  $form['billing_address']['billing_type'] = array(
    '#title' => t('Billing Type'),
    '#type' => 'item',
    '#markup' => ucfirst(strtolower($dev->getAttribute('MINT_BILLING_TYPE'))),
  );

  $form['address_id'] = array(
    '#type' => 'value',
    '#value' => $address->getId(),
  );
  $form['billing_address']['street_address_1'] = array(
    '#title' => t('Street Address 1'),
    '#type' => 'textfield',
    '#default_value' => $address->getAddress1(),
    '#required' => TRUE,
  );

  $form['billing_address']['street_address_2'] = array(
    '#title' => t('Street Address 2'),
    '#type' => 'textfield',
    '#default_value' => $address->getAddress2(),
  );

  $form['billing_address']['country'] = array(
    '#title' => t('Country'),
    '#type' => 'select',
    '#options' => Country::getList(),
    '#default_value' => $address->getCountry(),
    '#empty_option' => t('Select Country'),
  );

  $form['billing_address']['state_province'] = array(
    '#title' => t('State/Province'),
    '#type' => 'textfield',
    '#default_value' => $address->getState(),
    '#empty_option' => t('N/A'),
    '#required' => TRUE,
  );

  $form['billing_address']['city'] = array(
    '#title' => t('City'),
    '#type' => 'textfield',
    '#default_value' => $address->getCity(),
    '#required' => TRUE,
  );

  $form['billing_address']['zip_code'] = array(
    '#title' => t('Zip/Postal Code'),
    '#type' => 'textfield',
    '#default_value' => $address->getZip(),
    '#required' => TRUE,
  );

  $form['billing_address']['contact_tel_number'] = array(
    '#title' => t('Telephone Number'),
    '#type' => 'textfield',
    '#default_value' => $dev->getAttribute('MINT_DEVELOPER_PHONE'),
  );

  $form['billing_address']['registered_for_tax_vat'] = array(
    '#title' => t('Are you registered for Tax/VAT?'),
    '#type' => 'radios',
    '#options' => array(1 => t('Yes'), 0 => t('No')),
    '#default_value' => strlen(trim($dev->getAttribute('MINT_TAX_EXEMPT_AUTH_NO'))) > 0 ? 1 : 0,
  );

  $form['billing_address']['vat_tax_number'] = array(
    '#type' => 'textfield',
    '#title' => t('VAT/Tax Number'),
    '#default_value' => trim($dev->getAttribute('MINT_TAX_EXEMPT_AUTH_NO')),
  );

  $form['developer_type'] = array(
    '#type' => 'value',
    '#value' => $dev->getAttribute('MINT_DEVELOPER_TYPE'),
  );

  $form['billing_type'] = array(
    '#type' => 'value',
    '#value' => $dev->getAttribute('MINT_BILLING_TYPE'),
  );

  $form['_billing_type_fieldset'] = array(
    '#title' => t('Billing Type'),
    '#type' => 'fieldset',
    '#collapsible' => FALSE,
  );

  $form['_billing_type_fieldset']['_billing_type'] = array(
    '#markup' => ucfirst(strtolower($dev->getAttribute('MINT_BILLING_TYPE'))),
  );

  $form['company_id'] = array(
    '#type' => 'value',
    '#value' => $dev->getAttribute('MINT_COMPANY_ID'),
  );

  $form['approx_tax_rate'] = array(
    '#type' => 'value',
    '#value' => $dev->getAttribute('MINT_APPROX_TAX_RATE'),
  );

  // Note that KMS does not return boolean values but it string representation
  // That will break custom fields if they are checkboxes
  $form['is_broker'] = array(
    '#type' => 'value',
    '#value' => $dev->getAttribute('MINT_IS_BROKER') == 'true' ? 1 : 0,
  );

  $form['has_self_billing'] = array(
    '#type' => 'value',
    '#value' => $dev->getAttribute('MINT_HAS_SELF_BILLING'),
  );

  $form['billing_profile'] = array(
    '#type' => 'value',
    '#value' => $dev->getAttribute('MINT_BILLING_PROFILE'),
  );

  $form['supported_currency'] = array(
    '#type' => 'value',
    '#value' => $dev->getAttribute('MINT_SUPPORTED_CURRENCY'),
  );

  $form['developer_category'] = array(
    '#type' => 'value',
    '#value' => $dev->getAttribute('MINT_DEVELOPER_CATEGORY'),
  );

  $form['developer_roles'] = array(
    '#type' => 'value',
    '#value' => $dev->getAttribute('MINT_ROLES'),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save Changes'),
  );

  $form['cancel'] = array(
    '#markup' => l(t('Cancel'), 'users/' . $GLOBALS['user']->uid . '/monetization/company/edit'),
  );

//  $form['#attached']['js'][] = array(
//    'data' => drupal_get_path('module', 'devconnect_monetization') . '/js/jquery.select-to-autocomplete.js',
//    'type' => 'file',
//  );

  $form['#validate'][] = 'devconnect_monetization_company_details_form_validate';
  $form['#submit'][] = 'devconnect_monetization_company_details_form_submit';
  return $form;
}

function devconnect_monetization_company_details_form_validate(&$form, &$form_state) {
  if ($form_state['values']['registered_for_tax_vat'] == 1 && strlen(trim($form_state['values']['vat_tax_number'])) == 0) {
    form_set_error('vat_tax_number', t('If you are registered for Tax/VAT, then you must enter your VAT/Tax number'));
  }
}

function devconnect_monetization_company_details_form_submit(&$form, &$form_state) {
  // Load the company if there is a company in context.
  $entity_type = $form_state['values']['entity_type'];
  if ($entity_type == 'apigee_company') {
    devconnect_monetization_company_details_form_submit_company($form, $form_state);
  }
  else {
    devconnect_monetization_company_details_form_submit_developer($form, $form_state);
  }
}

function devconnect_monetization_company_details_form_submit_company(&$form, &$form_state) {
  global $user;
  $company_name = $form_state['values']['company_name'];

  // Load the company from Edge to populate w/new values.
  try {
    $company_entities = entity_load('apigee_company', array($company_name));
    $company_entity = reset($company_entities);
  }
  catch (\Exception $e) {
    $message = (method_exists($e, '__toString') ? (string) $e : $e->getMessage());
    drupal_set_message(t('The website encountered an unexpected error: %message', array('message' => $message)), 'error');
    devconnect_default_org_config()->logger->critical($message);
  }

  // Update the company display name if changed.
  $company_entity->displayName = $form_state['values']['legal_company_name'];

  // Add all monetization fields to attributes.  Array key is form field,
  // value is Edge attribute.
  $company_attributes_map = array(
    'email' => 'ADMIN_EMAIL',
    'company_id' => 'MINT_COMPANY_ID',
    'company_reg_number' => 'MINT_REGISTRATION_ID',
    'vat_tax_number' => 'MINT_TAX_EXEMPT_AUTH_NO',
    'legal_company_name' => 'MINT_DEVELOPER_LEGAL_NAME',
    'developer_type' => 'MINT_DEVELOPER_TYPE',
    'billing_type' => 'MINT_BILLING_TYPE',
    'contact_tel_number' => 'MINT_DEVELOPER_PHONE',
    'approx_tax_rate' => 'MINT_APPROX_TAX_RATE',
    'is_broker' => 'MINT_IS_BROKER',
    'has_self_billing' => 'MINT_HAS_SELF_BILLING',
    'billing_profile' => 'MINT_BILLING_PROFILE',
    'supported_currency' => 'MINT_SUPPORTED_CURRENCY',
  );

  foreach ($company_attributes_map as $company_attribute_form_field => $company_attribute_edge) {
    $company_attributes[$company_attribute_edge] = $form_state['values'][$company_attribute_form_field];
  }

  // Save developer email to attributes.
  $company_attributes['ADMIN_EMAIL'] = $user->mail;

  // Save Address to attributes.
  $address = array(
    "address1" => $form_state['values']['street_address_1'],
    "address2" => $form_state['values']['street_address_2'],
    "city" => $form_state['values']['city'],
    "country" => $form_state['values']['country'],
    "isPrimary" => "true",
    "state" => $form_state['values']['state_province'],
    "zip" => $form_state['values']['zip_code'],
  );
  $company_attributes['MINT_DEVELOPER_ADDRESS'] = json_encode($address);

  $company_entity->attributes = $company_attributes;

  // Create the company entity.
  $saved = entity_save('apigee_company', $company_entity);
  if (!$saved) {
    $e = ApigeeCompanyController::getLastException();
    $code = $e->getCode();
    if ($code == 409) {
      drupal_set_message(t('A company with the machine name %company_id already exists.', array('%company_id' => $company_entity->name)), 'error');
      $form_state['rebuild'] = TRUE;
    }
    else {
      $response = @json_decode($e->getResponse());
      drupal_set_message(t('There was an error trying to create the company: %response_message', array('%response_message' => $response->message)), 'error');
      watchdog('apigee_company', 'Error creating company %company: %response_message', array('%company' => $company_entity->name, '%response_message' => $response->message), WATCHDOG_ERROR);
      $form_state['rebuild'] = TRUE;
    }
  }
  else {
    watchdog('apigee_company', 'User %name %email updated details for company %company', array(
      '%user' => $user->name,
      '%email' => '<' . $user->mail . '>',
      '%company' => $company_entity->displayName,
        ), WATCHDOG_NOTICE);
    drupal_set_message(t('The company %company has been updated.', array('%company' => $company_entity->displayName)));
    $form_state['redirect'] = 'users/me/monetization/company';
  }
}

function devconnect_monetization_company_details_form_submit_developer(&$form, &$form_state) {
  try {
    $developer_id = _devconnect_monetization_get_developer_id(TRUE);
    if ($GLOBALS['user']->mail != $developer_id) {
      $uid = user_load_by_mail($developer_id)->uid;
    }
    else {
      $uid = $GLOBALS['user']->uid;
    }

    $account = entity_load_single('user', $uid);

    $account->field_mint_developer_legal_name[LANGUAGE_NONE][0]['value'] = $form_state['values']['legal_company_name'];
    $account->field_mint_registration_id[LANGUAGE_NONE][0]['value'] = $form_state['values']['company_reg_number'];

    // Save VAT in system even if they do not select registered_for_tax_vat.
    $account->field_mint_vat_tax_number[LANGUAGE_NONE][0]['value'] = $form_state['values']['vat_tax_number'];
    $account->field_mint_developer_phone[LANGUAGE_NONE][0]['value'] = $form_state['values']['contact_tel_number'];
    $address = array(
      'address1' => $form_state['values']['street_address_1'],
      'address2' => $form_state['values']['street_address_2'],
      'city' => $form_state['values']['city'],
      'country' => $form_state['values']['country'],
      'isPrimary' => 'true',
      'state' => $form_state['values']['state_province'],
      'zip' => $form_state['values']['zip_code'],
    );
    if (isset($form_state['values']['address_id']) && !is_null($form_state['values']['address_id'])) {
      $address['id'] = $form_state['values']['address_id'];
    }
    $account->field_mint_developer_address[LANGUAGE_NONE][0]['value'] = json_encode($address, JSON_FORCE_OBJECT);
    $account->field_mint_developer_type[LANGUAGE_NONE][0]['value'] = $form_state['values']['developer_type'];
    $account->field_mint_billing_type[LANGUAGE_NONE][0]['value'] = $form_state['values']['billing_type'];
    $account->field_mint_company_id[LANGUAGE_NONE][0]['value'] = $form_state['values']['company_id'];
    $account->field_mint_approx_tax_rate[LANGUAGE_NONE][0]['value'] = $form_state['values']['approx_tax_rate'];
    $account->field_mint_is_broker[LANGUAGE_NONE][0]['value'] = $form_state['values']['is_broker'];
    $account->field_mint_has_self_billing[LANGUAGE_NONE][0]['value'] = $form_state['values']['has_self_billing'];
    $account->field_mint_billing_profile[LANGUAGE_NONE][0]['value'] = $form_state['values']['billing_profile'];
    $account->field_mint_supported_currency[LANGUAGE_NONE][0]['value'] = $form_state['values']['supported_currency'];
    $account->field_mint_developer_category[LANGUAGE_NONE][0]['value'] = $form_state['values']['developer_category'];
    $account->field_mint_developer_roles[LANGUAGE_NONE][0]['value'] = $form_state['values']['developer_roles'];

    entity_save('user', $account);
    drupal_set_message(t('Your Company Profile Info has been saved.'), 'status');
    module_invoke_all('devconnect_monetization_developer_upgraded_to_company', $account);
    if (isset($_SESSION['return_here'])) {
      if (strpos('#', $_SESSION['return_here']) !== FALSE) {
        list($url, $fragment) = explode("#", $_SESSION['return_here']);
      }
      else {
        $url = $_SESSION['return_here'];
        $fragment = '';
      }
      $form_state['redirect'] = array($url, array('fragment' => rawurldecode($fragment)));
      unset($_SESSION['return_here']);
    }
  }
  catch (\Exception $e) {
    drupal_set_message(t('The website encountered an unexpected error. Please try again later.'), 'error');
    $message = (method_exists($e, '__toString') ? (string) $e : $e->getMessage());
    devconnect_default_org_config()->logger->critical($message);
  }
}

/**
 * Form builder for page callback:
 *   users/%devconnect_monetization_developer/monetization/company/bank-details
 *
 * @param $form
 * @param $form_state
 * @param \Apigee\Mint\Developer $developer
 * @return mixed
 */
function devconnect_monetization_company_bank_details_form($form, $form_state, \Apigee\Mint\Developer $developer) {

  $config = devconnect_monetization_default_org_config();
  // If we have the company in the context then we set the developer email to company name
  // so that the bank details are fetched for the company.
  $company = apigee_company_get_current_context_company();
  if (!empty($company)) {
    $developer_or_company_id = $company->name;
  }
  else {
    $developer_or_company_id = _devconnect_monetization_get_developer_id(TRUE);
  }
  $developer->setEmail($developer_or_company_id);
  $bank_detail = $developer->getBankDetails();
  $organization = new Organization($config);
  $currencies = $organization->listSupportedCurrencies();

  $address = $bank_detail->getAddress();

  $form['bank_details'] = array(
    '#type' => 'fieldset',
    '#title' => t('Bank Details'),
    '#collapsible' => FALSE,
  );

  $form['#bank_details'] = $bank_detail;

  $form['bank_details']['street_address_1'] = array(
    '#title' => t('Street Address 1'),
    '#type' => 'textfield',
    '#default_value' => isset($address) ? $address->getAddress1() : NULL,
  );

  $form['bank_details']['street_address_2'] = array(
    '#title' => t('Street Address 2'),
    '#type' => 'textfield',
    '#default_value' => isset($address) ? $address->getAddress2() : NULL,
  );

  $form['bank_details']['country'] = array(
    '#title' => t('Country'),
    '#type' => 'select',
    '#options' => Country::getList(),
    '#empty_option' => t('Select Country'),
    '#required' => TRUE,
    '#default_value' => isset($address) ? $address->getCountry() : NULL,
  );

  $form['bank_details']['state_province'] = array(
    '#title' => t('State\Province'),
    '#type' => 'textfield',
    '#default_value' => isset($address) ? $address->getState() : NULL,
  );

  $form['bank_details']['city'] = array(
    '#title' => t('City'),
    '#type' => 'textfield',
    '#default_value' => isset($address) ? $address->getCity() : NULL,
  );

  $form['bank_details']['zip_code'] = array(
    '#title' => t('Zip/Postal Code'),
    '#type' => 'textfield',
    '#default_value' => isset($address) ? $address->getZip() : NULL,
  );

  $form['account_info'] = array(
    '#title' => t('Account Info'),
    '#type' => 'fieldset',
    '#collapsible' => FALSE,
  );

  $form['account_info']['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Bank Name'),
    '#required' => TRUE,
    '#default_value' => $bank_detail->getName(),
  );

  $form['account_info']['account_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Account Name'),
    '#required' => TRUE,
    '#default_value' => $bank_detail->getAccountName(),
  );

  $form['account_info']['account_number'] = array(
    '#type' => 'textfield',
    '#title' => t('Account Number'),
    '#required' => TRUE,
    '#default_value' => $bank_detail->getAccountNumber(),
  );

  $currency_opts = array();
  /** @var Apigee\Mint\DataStructures\SupportedCurrency $currency */
  foreach ($currencies as &$currency) {
    $currency_opts[$currency->getId()] = "{$currency->getDisplayName()} ({$currency->getName()})";
  }
  $form['account_info']['account_currency'] = array(
    '#type' => 'select',
    '#title' => t('Currency'),
    '#empty_option' => t('- Select currency -'),
    '#options' => $currency_opts,
    '#required' => TRUE,
    '#default_value' => $bank_detail->getCurrency(),
  );

  $form['account_info']['account_sort_code'] = array(
    '#type' => 'textfield',
    '#title' => t('Sort Code'),
    '#default_value' => $bank_detail->getSortCode(),
  );

  $form['account_info']['account_aban_routing_number'] = array(
    '#type' => 'textfield',
    '#title' => t('ABA/Routing Number'),
    '#default_value' => $bank_detail->getAban(),
  );

  $form['account_info']['account_bic_swiff_code'] = array(
    '#type' => 'textfield',
    '#title' => t('BIC/Swiff Code'),
    '#default_value' => $bank_detail->getBic(),
  );

  $form['account_info']['account_iban'] = array(
    '#type' => 'textfield',
    '#title' => t('IBAN'),
    '#default_value' => $bank_detail->getIbanNumber(),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save Changes'),
  );

  $me = (module_exists('me') ? 'me' : $GLOBALS['user']->uid);
  $form['cancel'] = array(
    '#markup' => l(t('Cancel'), 'users/' . $me . '/monetization/company/edit'),
  );

  $form['#validate'][] = 'devconnect_monetization_company_bank_details_form_validate';
  $form['#submit'][] = 'devconnect_monetization_company_bank_details_form_submit';
  return $form;
}

function devconnect_monetization_company_bank_details_form_validate(&$form, &$form_state) {
  $has_errors = FALSE;
  if ($form_state['values']['account_number'] != (string) ((int) $form_state['values']['account_number'])) {
    form_set_error('account_number', t('Acount Number must be a numeric value with no decimals and no trailing spaces either.'));
  }
}

function devconnect_monetization_company_bank_details_form_submit(&$form, &$form_state) {
  try {
    /** @var Apigee\Mint\BankDetail $bank_detail */
    $bank_detail = $form['#bank_details'];
    $address = $bank_detail->getAddress();
    $address = isset($address) ? $address : new Address();
    $address->setAddress1($form_state['values']['street_address_1']);
    $address->setAddress2($form_state['values']['street_address_2']);
    $address->setCountry($form_state['values']['country']);
    $address->setState($form_state['values']['state_province']);
    $address->setCity($form_state['values']['city']);
    $address->setZip($form_state['values']['zip_code']);

    $bank_detail->setAddress($address);
    $bank_detail->setName($form_state['values']['name']);
    $bank_detail->setAccountName($form_state['values']['account_name']);
    $bank_detail->setAccountNumber($form_state['values']['account_number']);
    $bank_detail->setCurrency($form_state['values']['account_currency']);
    $bank_detail->setSortCode($form_state['values']['account_sort_code']);
    $bank_detail->setAban($form_state['values']['account_aban_routing_number']);
    $bank_detail->setBic($form_state['values']['account_bic_swiff_code']);
    $bank_detail->setIbanNumber($form_state['values']['account_iban']);
    $bank_detail->save('save');
    drupal_set_message(t('Your Bank Details have been saved.'), 'status');
  }
  catch (Exception $e) {
    $message = (method_exists($e, '__toString') ? (string) $e : $e->getMessage());
    devconnect_default_org_config()->logger->error($message);
    drupal_set_message(t('The operation could not be completed. Please try again later.'), 'error');
  }
}

/**
 * Form builder for monetization roles
 *
 * @param $form
 * @param $form_state
 * @param \Apigee\Mint\Developer $developer
 * @return array
 */
function devconnect_monetization_roles_form($form, $form_state, \Apigee\Mint\Developer $developer) {

  $query = new EntityFieldQuery();
  $child_developers = $query
      ->entityCondition('entity_type', 'user')
      ->entityCondition('bundle', 'user')
      ->fieldCondition('field_mint_company_id', 'value', $developer->getEmail(), '=')
      ->execute();

  $child_ids = isset($child_developers['user']) && is_array($child_developers['user']) ? array_keys($child_developers['user']) : array();

  $users = count($child_ids) > 0 ? entity_load('user', $child_ids) : array();

  $admin_uid = $developer->getEmail() == $GLOBALS['user']->mail ? $GLOBALS['user']->uid : user_load_by_mail($developer->getEmail())->uid;
  $users = entity_load('user', array($admin_uid)) + $users;

  $original_roles = array();
  foreach (array(MONETIZATION_ADMIN_ROLE_NAME, MONETIZATION_FINANCE_ADMIN_ROLE_NAME, MONETIZATION_DEVELOPER_ROLE_NAME) as $role_name) {
    $rid = variable_get('devconnect_monetization_role_' . $role_name, NULL);
    if ($rid) {
      $original_roles[$role_name] = $rid;
    }
  }

  $roles = db_select('role', 'r')
      ->fields('r', array('rid', 'name', 'weight'))
      ->condition('rid', array_values($original_roles), 'IN')
      ->orderBy('weight')
      ->execute();

  $form = array();

  $role_values = array();
  while ($role = $roles->fetchAssoc()) {
    $role_values[$role['rid']] = $role['name'];
    $form['#roles'][$role['rid']] = (object) $role;
  }


  $form['#original_roles'] = $original_roles;
  $form['#table_headers'] = array(
    t('Username'),
    $role_values[$original_roles[MONETIZATION_ADMIN_ROLE_NAME]],
    $role_values[$original_roles[MONETIZATION_FINANCE_ADMIN_ROLE_NAME]],
    $role_values[$original_roles[MONETIZATION_DEVELOPER_ROLE_NAME]],
    t('Actions'),
  );
  $dev_list = array();
  foreach ($users as $uid => $user) {
    $dev_name = isset($user->field_first_name[LANGUAGE_NONE][0]['value']) ? $user->field_first_name[LANGUAGE_NONE][0]['value'] : '';
    $dev_name .= isset($user->field_last_name[LANGUAGE_NONE][0]['value']) ? ('&nbsp;' . $user->field_last_name[LANGUAGE_NONE][0]['value']) : '';
    $dev_list[$uid] = array(
      'name' => array(
        '#type' => 'item',
        '#markup' => $dev_name,
      ),
      "admin[$uid]" => array(
        '#type' => 'checkbox',
        '#default_value' => in_array($role_values[$original_roles[MONETIZATION_ADMIN_ROLE_NAME]], $user->roles),
      ),
      "finance[$uid]" => array(
        '#type' => 'checkbox',
        '#default_value' => in_array($role_values[$original_roles[MONETIZATION_FINANCE_ADMIN_ROLE_NAME]], $user->roles),
      ),
      "developer[$uid]" => array(
        '#type' => 'checkbox',
        '#default_value' => in_array($role_values[$original_roles[MONETIZATION_DEVELOPER_ROLE_NAME]], $user->roles),
      ),
      "action[$uid]" => array(
        '#markup' => l(t('Delete'), 'users/' . $GLOBALS['user']->uid . '/monetization/company/developers/' . rawurlencode($uid) . '/remove', array('attributes' => array('class' => array('btn'))))
      ),
    );

    // Keep company admin and self user from editing its roles
    if ($user->mail == $GLOBALS['user']->mail || $uid == $admin_uid) {
      if ($uid == $admin_uid) {
        $dev_list[$uid]['name'] += array(
          '#prefix' => '<strong>',
          '#suffix' => '</strong>',
        );
      }
      else {
        $dev_list[$uid]['name'] += array(
          '#prefix' => t('<div style="font-style:italic" title="This is you"><h4>'),
          '#suffix' => '</h4></div>',
        );
      }
      $dev_list[$uid]["admin[{$uid}]"]['#disabled'] = TRUE;
      $dev_list[$uid]["finance[{$uid}]"]['#disabled'] = TRUE;
      $dev_list[$uid]["developer[{$uid}]"]['#disabled'] = TRUE;
      $dev_list[$uid]["action[{$uid}]"]['#access'] = FALSE;
    }
  }

  $form['developers'] = $dev_list;

  $form['submit'][] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  $form['#theme'] = 'devconnect_monetization_roles_form';
  $form['#submit'][] = 'devconnect_monetization_roles_form_submit';
  return $form;
}

function devconnect_monetization_roles_form_submit($form, &$form_state) {
  try {

    $developer_id = _devconnect_monetization_get_developer_id(TRUE);

    $query = new EntityFieldQuery();
    $child_developers = $query
        ->entityCondition('entity_type', 'user')
        ->entityCondition('bundle', 'user')
        ->fieldCondition('field_mint_company_id', 'value', $developer_id, '=')
        ->execute();

    $child_ids = array_keys($child_developers['user']);

    $developers = !empty($child_ids) ? entity_load('user', $child_ids) : array();

    $pseudo_monetization_roles = array(
      'admin' => $form['#roles'][$form['#original_roles'][MONETIZATION_ADMIN_ROLE_NAME]],
      'finance' => $form['#roles'][$form['#original_roles'][MONETIZATION_FINANCE_ADMIN_ROLE_NAME]],
      'developer' => $form['#roles'][$form['#original_roles'][MONETIZATION_DEVELOPER_ROLE_NAME]],
    );

    $monetization_roles = array();
    foreach ($pseudo_monetization_roles as $role) {
      $monetization_roles[$role->rid] = $role->name;
    }

    foreach ($developers as $uid => $developer) {
      $new_assigned_roles = array();
      foreach ($pseudo_monetization_roles as $pseudo_role_name => $role) {
        if (isset($form_state['input'][$pseudo_role_name][$uid]) && $form_state['input'][$pseudo_role_name][$uid] == 1) {
          $new_assigned_roles[$pseudo_monetization_roles[$pseudo_role_name]->rid] = $pseudo_monetization_roles[$pseudo_role_name]->name;
        }
      }

      $roles_the_user_has = array_intersect($developer->roles, $monetization_roles);
      $roles_user_will_keep = array_intersect($roles_the_user_has, $new_assigned_roles);

      $save_new_assigments = !($roles_user_will_keep == $roles_the_user_has) || !($roles_user_will_keep == $new_assigned_roles);

      if ($save_new_assigments) {
        foreach (array_keys($monetization_roles) as $rid) {
          unset($developer->roles[$rid]);
        }

        foreach ($new_assigned_roles as $rid => $name) {
          $developer->roles[$rid] = $name;
        }
        entity_save('user', $developer);
        module_invoke_all('devconnect_monetization_developer_assinged_mint_role', $developer);
      }
    }
    drupal_set_message(t('Operation completed successfully.'), 'status');
  }
  catch (Exception $e) {
    drupal_set_message(t('Your operation could not complete. Please contact your administrator if problem persists.'), 'error');
  }
}

function theme_devconnect_monetization_roles_form($variables) {
  $form = $variables['form'];

  $rows = array();

  foreach (element_children($form['developers']) as $uid) {
    $row = array();
    foreach (element_children($form['developers'][$uid]) as $element) {
      $row[] = drupal_render($form['developers'][$uid][$element]);
    }
    $rows[] = array('data' => $row);
  }

  $output = theme_table(array(
        'header' => $form['#table_headers'],
        'rows' => $rows,
        'attributes' => array(),
        'caption' => '',
        'colgroups' => array(),
        'sticky' => TRUE,
        'empty' => t('Your company has no developers assigned.'),
      )) . drupal_render($form['submit']) . drupal_render_children($form);
  return $output;
}

/**
 * Form builder to add users to a company
 *
 * @param \Apigee\Mint\Developer $developer
 *
 * @return array
 */
function devconnect_monetization_users_add_to_company_form($form, $form_state, \Apigee\Mint\Developer $developer) {

  $form = array();
  $form['#attributes']['class'][] = 'form-inline';
  $form['user_email_address'] = array(
    '#type' => 'textfield',
    '#title' => t('Enter User Email Address'),
    '#required' => TRUE,
    '#size' => 20,
    '#suffix' => '<br/><br/>'
      //'#autocomplete_path' => 'users/' . $GLOBALS['user']->uid . '/monetization/developers',
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Add User'),
  );

  return $form;
}

function devconnect_monetization_users_add_to_company_form_validate(&$form, &$form_state) {
  $form_state['values']['user_email_address'] = trim($form_state['values']['user_email_address']);
  $admin_id = _devconnect_monetization_get_developer_id(TRUE);

  $admin = user_load_by_mail($admin_id);
  $admin = entity_load_single('user', $admin->uid);
  if (strlen($admin->field_mint_company_id[LANGUAGE_NONE][0]['value']) > 0) {
    form_set_error(NULL, t('You cannot add developers to your company since you are added to another company.'));
  }
  else {
    $form_state['admin_id'] = $admin_id;
  }

  if (trim($form_state['values']['user_email_address']) == $admin_id) {
    form_set_error('user_email_address', t('You cannot add your own user to your company.'));
  }

  $child_user = user_load_by_mail($form_state['values']['user_email_address']);
  if ($child_user == NULL) {
    form_set_error('user_email_address', t('User with email @mail does not exist.', array('@mail' => $form_state['values']['user_email_address'])));
  }
  else {
    $child_entity = entity_load_single('user', $child_user->uid);

    if (isset($child_entity->field_mint_company_id[LANGUAGE_NONE][0]['value']) && strlen($child_entity->field_mint_company_id[LANGUAGE_NONE][0]['value']) > 0
    ) {
      form_set_error('user_email_address', t('You can not add this developer since it is already assigned.'));
    }
    else {
      $form_state['child_entity'] = $child_entity;
    }
  }
}

function devconnect_monetization_users_add_to_company_form_submit($form, &$form_state) {
  try {
    $child_entity = $form_state['child_entity'];
    $child_entity->field_mint_company_id[LANGUAGE_NONE][0]['value'] = $form_state['admin_id'];
    entity_save('user', $child_entity);
    drupal_set_message(t('Developer @developer added to your company.', array('@developer' => $form_state['values']['user_email_address'])));
    module_invoke_all('devconnect_monetization_developer_added_to_company', $child_entity, $child_entity->field_mint_company_id[LANGUAGE_NONE][0]['value']);
  }
  catch (Exception $e) {
    drupal_set_message(t('There was an error trying to add @developer to your company', array('@developer' => $form_state['values']['user_email_address'])), 'error');
  }
}

/**
 * Page callback for
 *   users/%devconnect_monetization_developer
 *
 * @param $string
 * @param \Apigee\Mint\Developer $developer
 */
function devconnect_monetization_users_autocomplete($string, \Apigee\Mint\Developer $developer) {

  $developers = entity_load('developer');
  $emails = array();
  foreach ($developers as $developer_item) {
    if (stripos($developer_item->email, $string) !== FALSE) {
      $emails[$developer_item->email] = $developer_item->email;
    }
  }
  drupal_json_output($emails);
  return;
}

/**
 * Page callback for
 *   users/%devconnect_monetization_developer/monetization/company/developers/%/remove
 *
 * @param $uid
 * @param \Apigee\Mint\Developer $developer
 */
function devconnect_monetization_user_remove_from_company($uid, \Apigee\Mint\Developer $developer) {
  try {
    $child_entity = entity_load_single('user', $uid);
    $company_id = $child_entity->field_mint_company_id[LANGUAGE_NONE][0]['value'];
    $child_entity->field_mint_company_id = NULL;
    entity_save('user', $child_entity);
    drupal_set_message(t('Developer @developer removed from your company.', array('@developer' => $child_entity->mail)));
    module_invoke_all('devconnect_monetization_developer_removed_from_company', $child_entity, $company_id);
    $me = module_exists('me') ? 'me' : $GLOBALS['user']->uid;
    drupal_goto("users/$me/monetization/company/developers");
  }
  catch (Exception $e) {
    drupal_set_message(t("Error: %error", array('%error' => $e->getMessage())), 'error');
    if (!isset($child_entity)) {
      $child_entity = entity_load_single('user', $uid);
    }
    $developer = $child_entity['mail'];
    drupal_set_message(t('There was an error trying to remove @developer from your company', array('@developer' => $developer)), 'error');
  }
}

/**
 * Page callback for
 *   users/%devconnect_monetization_developer/monetization/company/tncs
 *
 * @param \Apigee\Mint\Developer $developer
 * @return string
 */
function devconnect_monetization_list_tncs(\Apigee\Mint\Developer $developer) {

  $config = devconnect_monetization_default_org_config();
  $tnc = new TermAndCondition($config);
  $tncs = $tnc->getList(TRUE);

  $org = new Organization($config);
  $org->load();

  $org_timezone = new DateTimeZone($org->getTimezone());
  $local_timezone = new DateTimeZone(date_default_timezone_get());

  // Sort TnCs by start date DESC
  uasort($tncs, function ($tnc1, $tnc2) {
        $date1 = date_create_from_format('Y-m-d', $tnc1->getStartDate());
        $date2 = date_create_from_format('Y-m-d', $tnc2->getStartDate());

        if ($date1 < $date2) {
          return 1;
        }
        if ($date1 > $date2) {
          return -1;
        }
        return 0;
      });

  // Get the company name if there is a company in context.
  $company = apigee_company_get_current_context_company();
  if (!empty($company)) {
    $developer_id = $company->name;
  }
  else {
    $developer_id = $developer->getEmail();
  }
  $dev_tncs = $tnc->getAcceptedDevTermsAndConditions($developer_id);

  $tncs_forms = array();
  foreach ($tncs as $tnc) {
    $form = drupal_get_form('devconnect_monetization_accept_tnc_form', $tnc);
    $tncs_form = array(
      'tnc' => $tnc,
      'form' => drupal_render($form),
    );

    /** @var Apigee\Mint\DataStructures\DeveloperTnc $dev_tnc */
    foreach ($dev_tncs as $dev_tnc) {
      if ($dev_tnc->getTnc()->getId() == $tnc->getId()) {
        $tncs_form['accepted'] = DateTime::createFromFormat('Y-m-d H:i:s', $dev_tnc->getAuditDate(), $org_timezone)
                ->setTimezone($local_timezone)->format('Y-m-d H:i:s');
        break;
      }
    }
    $tncs_forms[] = $tncs_form;
  }

  // If developers have not accepted TnC, they should only
  // see the current and future TnC. Not old expired ones
  // (unless they had accepted them).
  // Should only see the CURRENT (and next future) one.

  $remove = FALSE;
  for ($i = 0, $len = count($tncs_forms); $i < $len; $i++) {
    if (!$remove && isset($tncs_forms[$i]['accepted'])) {
      $remove = TRUE;
      continue;
    }
    if ($remove && !isset($tncs_forms[$i]['accepted'])) {
      unset($tncs_forms[$i]);
    }
  }
  $variables = array(
    'tncs_forms' => $tncs_forms,
  );
  return theme('devconnect_monetization_company_tncs', $variables);
}

function devconnect_monetization_accept_tnc_form($form, &$form_state) {
  $tnc = $form_state['build_info']['args'][0];

  $form = array();
  if (user_access('accept mint terms and conditions')) {
    $form['tnc_id'] = array(
      '#type' => 'hidden',
      '#value' => $tnc->getId(),
    );
    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Accept Terms & Conditions'),
    );
    $form['#submit'][] = 'devconnect_monetization_accept_tnc_form_submit';
  }
  return $form;
}

function devconnect_monetization_accept_tnc_form_submit($form, &$form_state) {

  try {
    // Get the company name if there is a company in context.
    $company = apigee_company_get_current_context_company();
    if (!empty($company)) {
      $developer_id = $company->name;
    }
    else {
      $developer_id = _devconnect_monetization_get_developer_id(TRUE);
    }
    $tnc_id = $form_state['input']['tnc_id'];

    $config = devconnect_monetization_default_org_config();
    $org = new Organization($config);
    $org->load();

    $org_timezone = new DateTimeZone($org->getTimezone());

    $dev_tnc = new DeveloperTnc();

    $audit_date = new DateTime('now');
    $dev_tnc->setAuditDate($audit_date->setTimezone($org_timezone)->format('Y-m-d H:i:s'));
    $dev_tnc->setAction('ACCEPTED');
    $tnc = new TermAndCondition($config);
    $tnc->acceptTermsAndConditions($dev_tnc, $developer_id, $tnc_id);
    CacheFactory::getCacheManager(NULL)->clear('developer_accepted_tncs:' . $developer_id);
    module_invoke_all('devconnect_monetization_developer_accepted_tncs', $tnc);
    drupal_set_message(t('You have accepted terms and conditions successfully.'));
    if (isset($_SESSION['return_here'])) {
      $return_url = isset($_GET['return']) ? rawurldecode($_GET['return']) : '';
      if ($return_url) {
        list($url, $fragment) = explode("#", $return_url);
        $form_state['redirect'] = array($url, array('fragment' => $fragment));
      }
      else {
        $form_state['redirect'] = array($_SESSION['return_here']);
      }
      unset($_SESSION['return_here']);
    }
  }
  catch (Exception $e) {
    drupal_set_message(t('Error while trying to accept TnCs. Please try again later.'));
    devconnect_default_org_config()->logger->error('Could not accept terms and conditions: ' . $e->getMessage());
  }
}

function devconnect_monetization_form_devconnect_monetization_company_details_form_alter(&$form, &$form_state) {

  if (!variable_get('devconnect_monetization_recurring_enabled', FALSE) || devconnect_monetization_developer_load()->getBillingType() == BillingType::POSTPAID
  ) {
    return;
  }
  $developer_id = _devconnect_monetization_get_developer_id(TRUE);
  $config = devconnect_monetization_default_org_config();

  $developerBalance = new DeveloperBalance($developer_id, $config);
  $balances = $developerBalance->getList();
  if (empty($balances)) {
    return array();
  }
  $provider = new Provider($config);
  $providers = $provider->getList();

  $provider_names = array();
  /** @var Apigee\Mint\Provider $provider */
  foreach ($providers as $provider) {
    $provider_names[$provider->getId()] = $provider->getName();
  }

  $weight = -10;
  foreach (array_keys($form) as $element) {
    if (array_key_exists('#type', $form[$element])) {
      $form[$element]['#weight'] = $weight++;
      if ($element == '_billing_type_fieldset') {
        $form['balances'] = array(
          '#tree' => TRUE,
          '#theme' => 'devconnect_monetization_recurring_balances',
          '#weight' => $weight++,
        );
      }
    }
  }

  if (!empty($balances)) {
    unset($form['_billing_type_fieldset']);
  }

  /** @var Apigee\Mint\DeveloperBalance $balance */
  foreach ($developerBalance->getList() as $balance) {
    if ($balance->getProviderId() != NULL) {
      $form['balances']['items'][$balance->getSupportedCurrency()->getId()] = array(
        '#provider' => $provider_names[$balance->getProviderId()],
        '#currency' => $balance->getSupportedCurrency()->getName(),
        '#minimum_recurring_amount' => $balance->getSupportedCurrency()->getMinimumRecurringAmount(),
        'provider' => array(
          '#type' => 'value',
          '#value' => $balance->getProviderId(),
        ),
        'charge_per_usage' => array(
          '#type' => 'checkbox',
          '#default_value' => (int) $balance->isChargePerUsage(),
          '#attributes' => array(
            'class' => array('charge-per-usage'),
          )
        ),
        'is_recurring' => array(
          '#type' => 'checkbox',
          '#default_value' => (int) $balance->isRecurring(),
          '#attributes' => array(
            'class' => array('recurring'),
          ),
        ),
        'recurring_amount' => array(
          '#type' => 'textfield',
          '#default_value' => $balance->getRecurringAmount(),
          '#required' => TRUE,
          '#size' => 10,
          '#attributes' => array(
            'class' => array(
              'numeric',
              'currency',
              'recurring',
            ),
            'title' => t('Recurring amount must be greater than Replenish amount.'),
            'currency' => $balance->getSupportedCurrency()->getName(),
            'minimum' => (float) $balance->getSupportedCurrency()->getMinimumRecurringAmount(),
          ),
          '#disabled' => !$balance->isRecurring(),
        ),
        'replenish_amount' => array(
          '#type' => 'textfield',
          '#default_value' => $balance->getReplenishAmount(),
          '#required' => TRUE,
          '#size' => 10,
          '#attributes' => array(
            'class' => array(
              'numeric',
              'currency',
              'replenish',
            ),
            'title' => t('Replenish amount cannot be greater than Recurring amount.'),
            'currency' => $balance->getSupportedCurrency()->getName(),
            'minimum' => (float) $balance->getSupportedCurrency()->getMinimumRecurringAmount(),
          ),
          '#disabled' => !$balance->isRecurring(),
        )
      );
    }
  }

  $form['#balances'] = $form['balances']['items'];

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  $form['#attached']['js'][] = array(
    'data' => drupal_get_path('module', 'devconnect_monetization') . '/js/recurring-table.js',
    'type' => 'file'
  );

  $form['#validate'][] = 'devconnect_monetization_recurring_payment_form_validate';
  $form['#submit'][] = 'devconnect_monetization_recurring_payment_form_submit';
}

function devconnect_monetization_recurring_payment_form_validate($form, $form_state) {

  foreach ($form_state['input']['balances']['items'] as $currency => $new_setup) {
    if ($new_setup['is_recurring']) {
      $recurring_amount = $new_setup['recurring_amount'] + 0;
      $replenish_amount = $new_setup['replenish_amount'] + 0;
      $text = $form['#balances'][$currency]['#provider'] . ' (' . $form['#balances'][$currency]['#currency'] . ')';
      if (((string) $recurring_amount) != $new_setup['recurring_amount'] || $recurring_amount < 0) {
        form_set_error("balances][items][{$currency}][recurring_amount", t('@holder Recurring Amount must be a valid number and greater or equals to 0.', array('@holder' => $text)));
      }
      if (((string) $replenish_amount) != $new_setup['replenish_amount'] || $replenish_amount < 0) {
        form_set_error("balances][items][{$currency}][replenish_amount", t('@holder Replenish Amount must be a valid number and greater or equals to 0.', array('@holder' => $text)));
      }
      if (!($recurring_amount > $replenish_amount)) {
        form_set_error("balances][items][{$currency}][recurring_amount", t('@holder Recurring Amount must be greater than Recurring Amount.', array('@holder' => $text)));
      }
    }
  }
}

function devconnect_monetization_recurring_payment_form_submit($form, $form_state) {
  $developer_id = _devconnect_monetization_get_developer_id(TRUE);
  $config = devconnect_monetization_default_org_config();
  $developerBalance = new DeveloperBalance($developer_id, $config);
  foreach ($form_state['input']['balances']['items'] as $currency => $new_setup) {
    $new_setup['provider'] = $form_state['values']['balances']['items'][$currency]['provider'];
    $recurring_choice = array(
      'isRecurring' => (bool) $new_setup['is_recurring'],
      'chargePerUsage' => (bool) $new_setup['charge_per_usage'],
    );
    $developerBalance->updateRecurringSetup($currency, $recurring_choice, $new_setup['replenish_amount'], $new_setup['recurring_amount'], $new_setup['provider']);
  }
}

function theme_devconnect_monetization_recurring_balances($vars) {
  $rows = array();
  foreach (element_children($vars['balances']['items']) as $currency) {
    $rows[] = array(
      array('data' => $vars['balances']['items'][$currency]['charge_per_usage']),
      array('data' => $vars['balances']['items'][$currency]['is_recurring']),
      array('data' => $vars['balances']['items'][$currency]['#provider']),
      array('data' => $vars['balances']['items'][$currency]['#currency']),
      array('data' => $vars['balances']['items'][$currency]['recurring_amount']),
      array('data' => $vars['balances']['items'][$currency]['replenish_amount']),
    );
  }

  $header = array(
    t('Charge per Usage'),
    t('Recurring'),
    t('Provider'),
    t('Currency'),
    t('Recurring Amount'),
    t('Replenish Amount')
  );
  return theme('table', array('header' => $header, 'rows' => $rows));
}

function _devconnect_monetization_access_recurring_payments() {
  return variable_get('devconnect_monetization_recurring_enabled', FALSE) &&
      user_access('edit mint company profile');
}
