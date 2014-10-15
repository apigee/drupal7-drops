<?php

/**
 * @file
 * API documentation for Apigee Monetization.
 */

use Apigee\ManagementAPI\Developer;
use Apigee\Mint\TermAndCondition;
use Apigee\Mint\DeveloperRatePlan;

/**
 * Take actions right after a developer is upgraded to company developer.
 * @param $account
 */
function hook_devconnect_monetization_developer_upgraded_to_company($account) {}

/**
 * Take actions right after a developer is added to a company.
 * @param $account
 * @param $company_id
 */
function hook_devconnect_monetization_developer_added_to_company($account, $company_id) {}

/**
 * Take actions right after a developer is removed from a company.
 * @param $account
 * @param $company_id
 */
function hook_devconnect_monetization_developer_removed_from_company($account, $company_id) {}

/**
 * Take actions right after a developer is assigned/removed a Monetized Role.
 * @param $account
 */
function hook_devconnect_monetization_developer_assinged_mint_role($account) {}

/**
 * Take actions right after a plan is ended.
 * @param DeveloperRatePlan $dev_rate_plan
 */
function hook_devconnect_monetization_plan_ended(DeveloperRatePlan $dev_rate_plan) {}

/**
 * Take actions right after a plan is purchased.
 * @param DeveloperRatePlan $dev_rate_plan
 */
function hook_devconnect_monetization_plan_purchased(DeveloperRatePlan $dev_rate_plan) {}

/**
 * Take actions right after a plan is removed from a company.
 * @param DeveloperRatePlan $dev_rate_plan
 */
function hook_devconnect_monetization_plan_removed(DeveloperRatePlan $dev_rate_plan) {}

/**
 * Take actions right after a developer has accepted Organization's Terms and
 * Conditions.
 * @param TermAndCondition $terms_n_conditions
 */
function hook_devconnect_monetization_developer_accepted_tncs(TermAndCondition $terms_n_conditions) {}

/**
 * Take actions right after a developer has topped up balance.
 * @param \Apigee\Mint\Developer $developer
 * @param unknown $commerce_order
 */
function hook_devconnect_monetization_developer_topped_up(\Apigee\Mint\Developer $developer, $commerce_order) {}

/**
 * Must return an array of renderable elements, elements are usually of
 * markup type element with a link to a current requirement for a developer
 * to purchase a plan. Link can be embedded in an explanation string.
 *
 * Returning back to the purchase form is responsibility of the implemented
 * requirement.
 *
 * Only the element with the heaviest #weight will be rendered.
 *
 * @param \Apigee\Mint\Developer $developer
 * @param Apigee\Mint\MonetizationPackage $package
 *
 * @return array()
 */
function hook_devconnect_monetization_purchase_plan_requirements(\Apigee\Mint\Developer $developer, \Apigee\Mint\MonetizationPackage $package) {
  return array(
    'my_requirement' => array(
      '#markup' => t('This is my requirement ') . url('my/requirement'),
      '#weight' => 0,
      '#value'
    ),
  );
}

/**
 * Alter messages defined by
 * hook_devconnect_monetization_purchase_plan_requirement.
 * @param array $messages
 * @param \Apigee\Mint\Developer $developer
 * @param Apigee\Mint\MonetizationPackage $package
 */
function hook_devconnect_monetization_purchase_plan_requirements_alter(array $messages, \Apigee\Mint\Developer $developer, Apigee\Mint\MonetizationPackage $package) {
  unset($messages['my_requirement']);
}

/**
 * Must return a link to the destiny the user must be redirected when a plan
 * is accepted.
 */
function hook_redirect_after_purchase_plan() {
  return url('some/path');
}

/**
 * Must return a link to the destiny the user must be redirected when a plan is
 * overridden.
 */
function hook_redirect_after_override_plan() {
  return url('some/path');
}
