<?php

use Apigee\ManagementAPI\Developer;
use Apigee\Mint\TermAndCondition;
use Apigee\Mint\DeveloperRatePlan;

/**
 * Take actions right after a developer is upgraded to company developer
 * @param $account
 */
function hook_devconnect_monetization_developer_upgraded_to_company($account) {}

/**
 * Take actions right after a developer is added to a company
 * @param $account
 * @param $company_id
 */
function hook_devconnect_monetization_developer_added_to_company($account, $company_id) {}

/**
 * Take actions right after a developer is removed from a company
 * @param $account
 * @param $company_id
 */
function hook_devconnect_monetization_developer_removed_from_company($account, $company_id) {}

/**
 * Take actions right after a developer is assigned/removed a Monetized Role
 * @param $account
 */
function hook_devconnect_monetization_developer_assinged_mint_role($account) {}

/**
 * Take actions right after a plan is ended
 * @param DeveloperRatePlan $dev_rate_plan
 */
function hook_devconnect_monetization_plan_ended(DeveloperRatePlan $dev_rate_plan) {}

/**
 * Take actions right after a plan is purchased
 * @param DeveloperRatePlan $dev_rate_plan
 */
function hook_devconnect_monetization_plan_purchased(DeveloperRatePlan $dev_rate_plan) {}

/**
 * Take actions right after a plan is removed from a company
 * @param DeveloperRatePlan $dev_rate_plan
 */
function hook_devconnect_monetization_plan_removed(DeveloperRatePlan $dev_rate_plan) {}

/**
 * Take actions right after a developer has accepted Organization's Terms and Conditions
 * @param TermAndCondition $tncs
 */
function hook_devconnect_monetization_developer_accepted_tncs(TermAndCondition $tncs) {}

/**
 * Take actions right after a developer has topped up balance
 * @param \Apigee\Mint\Developer $developer
 * @param unknown $commerce_order
 */
function hook_devconnect_monetization_developer_topped_up(\Apigee\Mint\Developer $developer, $commerce_order) {}