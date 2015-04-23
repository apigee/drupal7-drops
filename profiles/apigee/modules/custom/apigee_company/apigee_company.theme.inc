<?php

/**
 * @file
 * Theme functions
 */

/**
 * Returns HTML for the developer's invitation list.
 *
 * @param array $variables
 *   An associative array containing invitations. Each invitation is
 *   an associative array containing:
 *   -company_name: The name of the company which sent the invite.
 *   -links: Themed links to Accept / Deny the request.
 *
 * @return string
 *   HTML table of developer's invitations.
 */
function theme_apigee_company_developer_invitations($variables) {
  $invitations = $variables['invitations'];
  drupal_add_css(drupal_get_path('module', 'apigee_company') . '/css/apigee_company.css', 'file');

  $rows = array();
  foreach ($invitations as $invitation) {
    $rows[] = array(
      'data' => array(
        'company' => array(
          'data' => $invitation['company_name'],
        ),
        'actions' => array(
          'data' => $invitation['links'],
        ),
      ),
    );
  }

  $header = array(
    'invitations' => array(
      'data' => t('Invitations'),
      'colspan' => 2,
    ),
  );
  $attributes = array(
    'class' => array('apigee-company-tables', 'developers-invitation-list'),
  );
  return theme('table', array(
    'header' => $header,
    'rows' => $rows,
    'attributes' => $attributes,
    'empty' => t('You do not have any company invitations.'),
      )
  );
}

/**
 * Returns HTML for list of companies that the current user is part of.
 *
 * @param array $variables
 *   An associative array of company objects.
 *
 * @return string
 *   A HTML table of developer's companies.
 */
function theme_apigee_company_developer_companies($variables) {
  global $user;
  $companies = $variables['companies'];
  drupal_add_css(drupal_get_path('module', 'apigee_company') . '/css/apigee_company.css', 'file');

  $rows = array();
  $row_link_options = array(
    'attributes' => array(
      'class' => array('btn', 'btn-default', 'leave-company-btn'),
    ),
  );

  foreach ($companies as $company) {
    $leave_link = l(t('Leave'), 'api_company/company/developer/leave/' . $company->name, $row_link_options);
    // If the current developer is the owner of the company then don't show
    // the leave link.
    if ($company->attributes['ADMIN_EMAIL'] == $user->mail) {
      $leave_link = t("Owner");
    }

    // @TODO: This URL should be modified by monetization module.
    $company_link_options = array('query' => array('destination' => 'users/me/monetization/company/developers'));
    $company_cell = l($company->displayName, 'api_company/company/switch/' . $company->name, $company_link_options);

    $rows[] = array(
      'data' => array(
        'company' => array(
          'data' => $company_cell,
        ),
        'leave' => array(
          'data' => $leave_link,
        ),
      ),
    );
  }

  $header = array(
    'company' => array(
      'data' => t('Companies'),
      'colspan' => 2,
    ),
  );
  $attributes = array(
    'class' => array('apigee-company-tables', 'developers-company-list'),
  );

  $output = theme('table', array(
    'header' => $header,
    'rows' => $rows,
    'attributes' => $attributes,
    'empty' => t('You are not a member of any companies.'),
  ));

  $output .= '<div class="create-company-container">';
  $link_options = array(
    'attributes' => array(
      'class' => array('btn', 'btn-default', 'create-company-btn'),
    ),
  );
  $output .= l(t('Create company'), "api_company/developer/company/add", $link_options);
  $output .= '</div>';

  return $output;
}

/**
 * Returns HTML for the list of developers in a company.
 *
 * @param array $variables
 *   An associative array containing:
 *    -developers: an indexed array of Drupal\devconnect_user\DeveloperEntity
 *     objects.
 *    -company_name: The name of the company for which the developer list is
 *     being generated.
 *
 * @return string
 *   HTML table of developers.
 */
function theme_apigee_company_company_developers($variables) {
  global $user;
  $developers = $variables['developers'];
  $company_name = $variables['company_name'];
  drupal_add_css(drupal_get_path('module', 'apigee_company') . '/css/apigee_company.css', 'file');
  $rows = array();

  foreach ($developers as $developer_id => $developer_info) {
    $developer = $developer_info['developer'];
    $developer_roles = $developer_info['roles'];
    $row = array(
      'data' => array(
        'company' => array(
          'data' => $developer->firstName . " " . $developer->lastName,
        ),
      ),
    );
    $row['data'] += array(
      'roles' => array(
        'data' => $developer_roles,
      ),
    );
    // Only show actions if user has proper permissions.
    if (user_access('manage company users')) {
      $modify = l(t('Modify Role'), 'api_company/nojs/developer/' . $developer->email . '/modify_role', array(
        'attributes' => array(
          'class' => array(
            'ctools-use-modal',
            'ctools-modal-company-role-modal-style',
            'btn', 'btn-default',
            'modify-role-btn',
          ),
        ),
      ));
      $remove = l(t('Remove'), 'api_company/company/developer/' . $developer->email . '/remove/', array(
        'attributes' => array(
          'class' => array(
            'btn',
            'btn-default',
            'remove-developer-btn',
          ),
        ),
      ));
      // If the current developer is the owner of the company then don't
      // show the leave link.
      $company = apigee_company_get_current_context_company();
      if ($company->attributes['ADMIN_EMAIL'] == $developer->email) {
        $remove = '';
        $modify = t('Owner');
      }

      $row['data'] += array(
        'actions' => array(
          'data' => $modify . $remove,
        ),
      );
    }
    $rows[] = $row;
  }

  $header = array(
    'name' => array(
      'data' => $company_name,
      'colspan' => 1,
    ),
    'roles' => array(
      'data' => t('Roles'),
      'colspan' => 1,
    ),
    'actions' => array(
      'data' => t('Actions'),
      'colspan' => 1,
    ),
  );
  if (!user_access('manage company users')) {
    unset($header['actions']);
  }

  $attributes = array(
    'class' => array('apigee-company-tables', 'company-developers-list'),
  );
  $company_user_list = theme('table', array(
    'header' => $header,
    'rows' => $rows,
    'attributes' => $attributes,
      )
  );

  return $company_user_list;
}

/**
 * Returns HTML for the company context switcher.
 *
 * @param array $variables
 *   An associative array containing:
 *   - company_list: An array containing the company attributes, indexed by
 *     company id. Each element of the array has the following keys:
 *     - "id": The company's unique id.
 *     - "display_name": The company's display name.
 *     - 'url': The URL to call in order to switch to that company.
 *     Here's an example:
 *
 * @code
 *     $company_list = array(
 *       'apigee' => array(
 *         'id' => 'apigee',
 *         'display_name' => 'Apigee Corporation',
 *         'url' => 'api_company/company/switch/apigee',
 *       ),
 *     );
 * @endcode
 *
 * @return string
 *   HTML for the table switcher.
 */
function theme_apigee_company_switcher($variables) {
  $company_links = array();

  // Create link for each company.
  foreach ($variables['company_list'] as $company) {
    $company_links[] = array(
      'title' => $company['display_name'],
      'href' => $company['url'],
      'query' => drupal_get_destination(),
      'attributes' => array(
        'title' => $company['display_name'],
        'class' => array(
          'company-' . $company['name'],
          'views-ajax-link',
        ),
      ),
    );
  }

  // Add "Manage companies" link.
  $company_links[] = array(
    'title' => t('Manage Companies'),
    'href' => 'api_company/companies/list',
    'attributes' => array(
      'title' => t('Manage your companies'),
      'class' => array(
        'company-manage',
        'views-ajax-link',
      ),
    ),
  );

  // Theme using ctools dropbutton.
  drupal_add_css(drupal_get_path('module', 'views') . '/css/views-admin.ctools.css');
  $context_links = theme('links__ctools_dropbutton', array(
    'links' => $company_links,
      )
  );

  $output = '<label>' . t('Switch Company') . '</label>';
  $output .= '<div class="company-switcher">' . $context_links . '</div>';
  return $output;
}

/**
 * Returns HTML for the pending developer invitations for a company.
 *
 * @param array $variables
 *   An associative array containing array of invitation objects.
 *
 * @return string
 *   HTML table of pending developer invitations for a company.
 */
function theme_apigee_company_company_pending_developer_invitations($variables) {
  $invitations = $variables['invitations'];
  drupal_add_css(drupal_get_path('module', 'apigee_company') . '/css/apigee_company.css', 'file');

  $rows = array();
  $row_link_options = array(
    'query' => array('destination' => $_GET['q']),
    'attributes' => array(
      'class' => array('btn', 'btn-default', 'cancel-invite-btn')),
  );

  foreach ($invitations as $invitation) {
    $row_url = 'api_company/company_invitation/cancel/' . $invitation->id;
    $rows[] = array(
      'data' => array(
        'company' => array(
          'data' => $invitation->developerId,
        ),
        'actions' => array(
          'data' => l(t('Cancel'), $row_url, $row_link_options),
        ),
      ),
    );
  }

  $header = array(
    'invitations' => array(
      'data' => t('Pending Invitations'),
      'colspan' => 2,
    ),
  );
  $attributes = array(
    'class' => array('apigee-company-tables', 'company-developer-invitation-list'),
  );
  return theme('table', array(
    'header' => $header,
    'rows' => $rows,
    'attributes' => $attributes,
    'empty' => t('No pending company invitations.'),
      )
  );
}
