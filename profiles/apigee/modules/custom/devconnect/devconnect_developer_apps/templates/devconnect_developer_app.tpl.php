<?php
/**
 * @file
 * Displays detail for a single app.
 *
 * Available vars:
 *   $account - stdClass: user owning this app.
 *   $access_type - string: read, write, read/write, none
 *   $callback_url - string
 *   $name - string
 *   $status - string (App status)
 *   $app_attributes - associative array: display-name => value.
 *   $credentials - array of associative arrays with the following keys:
 *     apiproducts - array of strings
 *     consumer_key
 *     consumer_secret
 *     status (Credential status)
 *
 * Each $credentials['apiproducts'] is an associative array with the following
 * keys:
 *   display_name
 *   description
 *   status (API Product status)
 */

// IMPORTANT: latest credential is last in array!
$credential = (isset($credentials) ? end($credentials) : NULL);
$show_analytics = ($analytics_chart !== FALSE);
$app_id = arg(3);
?>

<ul class="nav nav-tabs">
  <li class="active"><a data-toggle="tab" href="#keys">Keys</a></li>
  <li><a data-toggle="tab" href="#products">Products</a></li>
  <li><a data-toggle="tab" href="#details">App Details</a></li>
  <li><a href="/user/me/apps/<?php print $app_id; ?>/edit-app">Edit App</a></li>
<?php if ($show_analytics): ?>
  <li><a data-toggle="tab" href="#performance">App Performance</a></li>
<?php endif; ?>
</ul>

<div class="tab-content app-details">
  <div class="tab-pane active" id="keys">
    <h3>Keys</h3>
    <p>These are the keys to your app kingdom.</p>
<?php if (isset($credential)): ?>
    <div class="key">
      <span class="key-label">Consumer Key:</span>
      <?php print check_plain($credential['consumer_key']); ?>
    </div>
    <div class="key">
      <span class="key-label">Consumer Secret Key:</span>
      <?php print check_plain($credential['consumer_secret']); ?>
    </div>
<?php endif; ?>
<?php if (!empty($callback_url)): // certain client implementations may make callback_url optional ?>
    <div class="key">
      <span class="key-label">Callback URL:</span>
      <?php print check_plain($callback_url); ?>
    </div>
<?php endif; ?>
<?php foreach ($app_attributes as $label => $value): ?>
    <div class="key">
      <span class="key-label"><?php print check_plain($label); ?>:</span>
      <?php print check_plain($value); ?>
    </div>
<?php endforeach; ?>
  <!-- <button class="btn btn-success" href="#">Refresh Keys</button><br><br>  -->
  </div>
  <div class="tab-pane" id="products">
    <?php
      if (isset($credential)) {
        foreach ($credential['apiproducts'] as $apiproduct) {
          print '<div class="app-content"><div class="app-info-wrapper">';
          print '<h4 class="app-product-title">' . check_plain($apiproduct['display_name']) . '</h4>';
          if (strlen($apiproduct['description']) > 0) {
            print '<div class="app-desc">' . check_plain($apiproduct['description']) . '</div>';
          }
          print '</div>';
          if (!empty($apiproduct['status'])) {
            print '<div class="apiproduct-status">Approval Status: ' . check_plain(ucfirst($apiproduct['status'])) . '</div>';
          }
          print '</div><br>';
        }
      }
    ?>
  </div>
  <div class="tab-pane" id="details">
    <h3>App Details</h3>
    <div class="well">
    <?php print '<div class="control-group"><strong>App Name:</strong><div>' . check_plain($name) . '</div></div>'; ?>
    <?php print '<div class="control-group"><strong>Callback URL:</strong><div>' . check_plain($callback_url) . '</div></div>'; ?>
    <?php
    print '<div class="control-group"><strong>API Products:</strong>';
    if (isset($credential)) {
      foreach ($credential['apiproducts'] as $apiproduct) {
        print '<div>' . check_plain($apiproduct['display_name']) . '</div>';
        if (strlen($apiproduct['description']) > 0) {
          print '<div>' . check_plain($apiproduct['description']) . '</div><br>';
        }
      }
    }
    print '<div class="control-group"><strong>Status:</strong><div>' . check_plain($credential['status']) . '</div></div>';
    print '</div>';
    ?>
    </div>
  </div>
<?php if ($show_analytics): ?>
  <div class="tab-pane" id="performance">
    <?php if (empty($analytics_chart)): ?>
      <p>
        No performance data is available for the criteria you supplied.
      </p>
    <?php else: ?>
    <div class="btn-group pull-right">
      <a class="export btn dropdown-toggle" data-toggle="dropdown" href="#">Export<span class="caret"></span></a>
      <ul class="dropdown-menu">
        <li><a href="#">XML</a></li>
        <li><a href="#">CSV</a></li>
      </ul>
    </div>
    <div class="span24 well">
      <?php print drupal_render(drupal_get_form('devconnect_developer_apps_analytics_form')); ?>
    </div>
    <div style="clear: both; margin-top:20px">
      <?php print $analytics_chart; ?>
    </div>
    <?php endif; ?>
  </div>
<?php endif; ?>
</div>
