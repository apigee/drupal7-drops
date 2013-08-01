<?php
/**
 * @file
 * Default theme implementation to display list of developer apps.
 *
 * Available variables:
 * $user - fully-populated user object (stdClass)
 * $application_count - number of applications registered to the user
 * $applications - array of arrays, each of which has the following keys:
 *  - app_name
 *  - callback_url
 *  - credential
 *  - delete_url
 */

?>
<?php
// Set Title
drupal_set_title('My Apps');

// Build Breadcrumbs
$breadcrumb = array();
$breadcrumb[] = l('Home', '<front>');

// Set Breadcrumbs
drupal_set_breadcrumb($breadcrumb);

$show_status = variable_get('devconnect_show_apiproduct_status', FALSE);

?>
<?php print l('Add a new app', 'user/' . $user->uid . '/apps/add', array('attributes' => array('class' => array('add-app')))); ?>

<form class="form-stacked">

<?php if ($application_count) : ?>

<h2>These are your apps!</h2>
<h3>Add more, edit or delete them as you like.</h3>
<hr>

<?php
  foreach ($applications as $app) {

    if ($show_status) {
      if (isset($app['credential'])) {
        $statuses = array();
        foreach ($app['credential']['apiProducts'] as $product) {
          $statuses[$product['apiproduct']] = (empty($product['status']) ? 'unknown' : $product['status']);
        }
      }
      else {
        $statuses = NULL;
      }
    }
    print '<div class="app-delete">';
    print '<button class="btn primary action button-processed" title="Delete App" data-url="' . $app['delete_url'] . '"></button>';
    print '</div>';
    print '<div class="app-content"><h4 class="app-title">' . l($app['app_name'], 'user/' . $user->uid . '/app-detail/' . $app['app_name']) . '</h4>';
    if (!empty($app['attributes']['Description'])) {
      print '<div class="app-desc">' . check_plain($app['attributes']['Description']) . '</div>';
    }
    print '</div>';

    if ($show_status && !empty($statuses)) {
      print '<div class="app-status"><strong>Status:</strong><br>';
      foreach ($statuses as $apiproduct => $status) {
        print '<div><span class="api-product-name"><strong>' . check_plain($apiproduct) . '</strong></span>&nbsp;-&nbsp;<span class="api-product-status">' . check_plain($status) . '</span></div>';
      }
      print '</div>';
    }
    print '<br><hr>';
  }
?>
<?php else: ?>
	<h2>Looks like you don’t have any apps</h2>
  <h3>Get started by adding one.</h3>
<?php endif; ?>
</form>
