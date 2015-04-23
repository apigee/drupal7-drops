<?php

/**
 * @file
 * Code to manage monetization purchased plans.
 *
 */

use Apigee\Mint\DeveloperRatePlan;

/**
 * Page callback for:
 *    users/%devconnect_monetization_developer/monetization/packages/purchased
 *
 * @param \Apigee\Mint\Developer $developer
 * @return string
 */
function devconnect_monetization_purchased_plans(\Apigee\Mint\Developer $developer) {

  // Create table headers.
  $header = array(
    array('data' => t('Status'), 'field' => 'status'),
    array('data' => t('Package'), 'field' => 'package'),
    array('data' => t('Products'), 'field' => 'products'),
    array('data' => t('Plan'), 'field' => 'rate_plan'),
    array('data' => t('Start Date'), 'field' => 'start_date'),
    array('data' => t('End Date'), 'field' => 'end_date'),
    array('data' => t('Plan End Date'), 'field' => 'plan_end_date'),
    array('data' => t('Renewal Date'), 'field' => 'renewal_date'),
    array('data' => t('Actions')),
  );

  $purchased_plans = devconnect_monetization_get_purchased_plans($developer);

  if(count($purchased_plans) > 1) {
    // Get column and it's sort order information.
    $table_sort = devconnect_monetization_purchased_plans_get_sort_info($header);

    devconnect_monetization_purchased_plans_do_table_sort($table_sort, $purchased_plans);
  }

  $rows = array();
  foreach ($purchased_plans as $plan) {
    // Add the $plan data to the row.
    $row = array();
    $row[] = $plan['status'];
    $row[] = $plan['package'];
    $row[] = $plan['products'];
    $row[] = $plan['rate_plan'];
    $row[] = $plan['start_date']->format(variable_get('devconnect_monetization_date_format', NORTH_AMERICAN_DATE_FORMAT));
    $row[] = (empty($plan['end_date'])) ? '--' : $plan['end_date']->format(variable_get('devconnect_monetization_date_format', NORTH_AMERICAN_DATE_FORMAT));
    $row[] = (empty($plan['plan_end_date'])) ? '--' : $plan['plan_end_date']->format(variable_get('devconnect_monetization_date_format', NORTH_AMERICAN_DATE_FORMAT));
    $row[] = (empty($plan['renewal_date'])) ? '--' : $plan['renewal_date']->format(variable_get('devconnect_monetization_date_format', NORTH_AMERICAN_DATE_FORMAT));
    $row[] = $plan['action'];

    $rows[] = array('data' => $row);
  }

  // Build the table for the nice output.
  $build['tablesort_table'] = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
  );

  return $build;

}

function devconnect_monetization_purchased_plans_do_table_sort($table_sort_info, &$purchased_plans) {
  usort($purchased_plans, devconnect_monetization_purchased_plans_table_sorter($table_sort_info['field'],$table_sort_info['sort']));
}

function devconnect_monetization_purchased_plans_table_sorter($key, $order) {
  switch ($key) {
    case 'rate_plan':
      // Do a natural string compare on hyperlink text.
      return function ($a, $b) use ($key, $order) {
        $re = "/<a href=\\\"([^\\\"]*)\\\">(.*)<\\/a>/";
        preg_match($re, $a[$key], $matches_a);
        if(!empty($matches_a[2])) {
          $a_plan_name = $matches_a[2];
        }
        else {
          $a_plan_name = $a[$key];
        }
        preg_match($re, $b[$key], $matches_b);
        if(!empty($matches_b[2])) {
          $b_plan_name = $matches_b[2];
        }
        else {
          $b_plan_name = $b[$key];
        }


        if($order == 'asc') {
          return strnatcasecmp($a_plan_name, $b_plan_name);
        }
        else {
          return -strnatcasecmp($a_plan_name, $b_plan_name);
        }
      };
    case 'package':
    case 'products':
      // Do a natural string compare.
      return function ($a, $b) use ($key, $order) {
        if($order == 'asc') {
          return strnatcasecmp($a[$key], $b[$key]);
        }
        else {
          return -strnatcasecmp($a[$key], $b[$key]);
        }
      };
    case 'start_date':
    case 'end_date':
    case 'plan_end_date':
    case 'renewal_date':
      // Sort by date.
      return function ($a, $b) use ($key, $order) {
        // Date diffs only work in PHP version >= 5.3.
        if(empty($a[$key])) {
          $aEpoch = 0;
        }
        else {
          $aEpoch = $a[$key]->format('U');
        }

        if(empty($b[$key])) {
          $bEpoch = 0;
        }
        else {
          $bEpoch = $b[$key]->format('U');
        }

        if($order == 'asc') {
          $result =  ($aEpoch > $bEpoch)? 1 : -1;
        }
        else {
          $result =  ($aEpoch < $bEpoch)? 1 : -1;
        }
        return $result;
      };
    case 'status':
      // Sort by: ended, then active, then future.
      return function ($a, $b) use ($key, $order) {
        if ($a[$key] == $b[$key]) {
          return 0;
        }
        // Logic table for asc order (ended active future):
        //   a:future b:active =  1
        //   a:future b:ended  =  1
        //   a:ended  b:active = -1
        //   a:ended  b:future = -1
        //   a:active b:ended  =  1
        //   a:active b:future = -1
        if($a[$key] == DeveloperRatePlan::STATUS_FUTURE) {
          return ($order == 'asc')? 1 : -1;
        }
        if($a[$key] == DeveloperRatePlan::STATUS_ENDED) {
          return ($order == 'asc')? -1 : 1;
        }
        if($b[$key] == DeveloperRatePlan::STATUS_ENDED) {
          return ($order == 'asc')? 1 : -1;
        }
        else {
          return ($order == 'asc')? -1 : 1;
        }

      };
  }

}


/**
 * Order the query based on a header array.
 *
 * @see theme_table()
 * @param $header
 *   Table header array.
 * @return SelectQueryInterface
 *   The called object.
 */
function devconnect_monetization_purchased_plans_get_sort_info(Array $headers) {
  // Get order from $GET.
    $order = isset($_GET['order']) ? $_GET['order'] : '';
  foreach ($headers as $header) {
    if (is_array($header)) {
      if (isset($header['data']) && $order == $header['data']) {
        $default = $header;
        break;
      }

      if (empty($default) && isset($header['sort']) && ($header['sort'] == 'asc' || $header['sort'] == 'desc')) {
        $default = $header;
      }
    }
  }

  if (!isset($default)) {
    // Make default header the "Status" column.
    $default = reset($headers);
    if (!is_array($default)) {
      $default = array('data' => $default);
    }
  }

  $default += array('data' => NULL, 'field' => NULL);
  $header_sort_info = array('name' => $default['data'], 'field' => $default['field']);

  // Order by asc or desc?
  if (isset($_GET['sort'])) {
    $header_sort_info['sort'] = (strtolower($_GET['sort']) == 'desc') ? 'desc' : 'asc';
  }
  // The user has not specified a sort. Use the default for the currently sorted
  // header if specified; otherwise use "asc".
  else {
    $header_sort_info['sort'] = 'asc';

  }

  return $header_sort_info;
}


function sort_purchased_plans() {
  usort($sorted_purchased_plans, function ($v1, $v2) {
    $v1_start_date = date_create_from_format(variable_get('devconnect_monetization_date_format', NORTH_AMERICAN_DATE_FORMAT), $v1['start_date']);
    $v2_start_date = date_create_from_format(variable_get('devconnect_monetization_date_format', NORTH_AMERICAN_DATE_FORMAT), $v2['start_date']);

    if ($v1_start_date < $v2_start_date) {
      return 1;
    }
    else {
      if ($v1_start_date > $v2_start_date) {
        return -1;
      }
      else {
        $v1_end_date = date_create_from_format(variable_get('devconnect_monetization_date_format', NORTH_AMERICAN_DATE_FORMAT), $v1['end_date']);
        $v2_end_date = date_create_from_format(variable_get('devconnect_monetization_date_format', NORTH_AMERICAN_DATE_FORMAT), $v2['end_date']);
        if ($v1_end_date === FALSE && $v2_end_date === FALSE) {
          return 0;
        }
        else {
          if ($v1_end_date === FALSE && $v2_end_date !== FALSE) {
            return 1;
          }
          else {
            if ($v1_end_date !== FALSE && $v2_end_date != FALSE) {
              return -1;
            }
            else {
              if ($v1_end_date < $v2_end_date) {
                return 1;
              }
              else {
                if ($v1_end_date > $v2_end_date) {
                  return -1;
                }
              }
              return 0;
            }
          }
        }
      }
    }
  });
}

/**
 * @param \Apigee\Mint\Developer $developer
 * @return array
 */
function devconnect_monetization_get_purchased_plans(\Apigee\Mint\Developer $developer) {
  $company = apigee_company_get_current_context_company();
  // If a user is in the company context then load the plans purchased by the company.
  if (!empty($company)) {
    $developer_or_company_id = $company->name;
  }
  else {
    $developer_or_company_id = _devconnect_monetization_get_developer_id(TRUE);
  }
  $config = devconnect_monetization_default_org_config();
  $developer_rate_plan = new DeveloperRatePlan($developer_or_company_id, $config);
  $purchased_package_plans = array();
  $developer_rate_plans = $developer_rate_plan->getList();
  foreach ($developer_rate_plans as $developer_rate_plan) {
    $rate_plan = $developer_rate_plan->getRatePlan();
    $package_id = $rate_plan->getMonetizationPackage()->getId();
    $rate_plan_id = $developer_rate_plan->getId();

    if ($developer_rate_plan->isCancelable()) {
      $btn_cancel_url = 'users/' . $GLOBALS['user']->uid . '/monetization/packages/' . rawurlencode($rate_plan->getMonetizationPackage()->getId()) . '/delete/' . rawurlencode($rate_plan_id);
      $action = l(t('Cancel'), $btn_cancel_url,
        array('attributes' => array('class' => array('btn'))));
    }
    else {
      $action = '&nbsp;';
    }

    if($rate_plan->hasEnded()) {
      // Plan has ended, do not link to plan page.
      $rate_plan_display = $rate_plan->getDisplayName();
    }
    else {
      // Plan has not ended, link to plan page.
      global $user;
      $rate_plan_url = 'users/' . $user->uid . '/monetization/packages/' . rawurlencode($rate_plan->getMonetizationPackage()->getId()) . '/view';
      $rate_plan_url_fragment = 'tab_' . preg_replace('/[^a-z0-9_-]/i', '_', $rate_plan->getId());
      $rate_plan_display = l($rate_plan->getDisplayName(), $rate_plan_url, array('fragment' => $rate_plan_url_fragment));
    }

    $purchased_package_plans[$package_id][$rate_plan_id] = array(
      'package' => $rate_plan->getMonetizationPackage()->getDisplayName(),
      'package_id' => $rate_plan->getMonetizationPackage()->getId(),
      'rate_plan' => $rate_plan_display,
      'rate_plan_id' => $rate_plan->getId(),
      'action' => $action,
      'start_date' => $developer_rate_plan->getStartDateTime(),
      'end_date' => $developer_rate_plan->getEndDateTime(),
      'plan_end_date' => $rate_plan->getEndDateTime(),
      'renewal_date' => $developer_rate_plan->getRenewalDateTime(),
      'products' => array(),
      'status' => $developer_rate_plan->getStatus(),
    );

    foreach ($rate_plan->getMonetizationPackage()->getProducts() as $product) {
      $purchased_package_plans[$package_id][$rate_plan_id]['products'][] = $product->getDisplayName();
    }
    $purchased_package_plans[$package_id][$rate_plan_id]['products'] = implode(', ', array_unique($purchased_package_plans[$package_id][$rate_plan_id]['products']));
  }

  $purchased_plans = array();
  foreach ($purchased_package_plans as $package_id => $plans) {
    foreach ($plans as $plan) {
      $purchased_plans[] = $plan;
    }
  }
  return $purchased_plans;
}
