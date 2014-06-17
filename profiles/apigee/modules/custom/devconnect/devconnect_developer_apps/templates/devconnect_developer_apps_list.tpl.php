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
 *  - credential (each member has apiproduct, status, displayName keys)
 *  - delete_url
 */

// Set Title
if ($user->uid == $GLOBALS['user']->uid) {
  if ((bool)variable_get('myapis')) {
    $title = t('My APIs');
  } else {
    $title = t('My Apps');
  }
}
else {
  if ((bool)variable_get('myapis')) {
    $title = t('@name’s APIs', array('@name' => $user->name));
  } else {
    $title = t('@name’s Apps', array('@name' => $user->name));
  }
}
drupal_set_title($title);

// Build Breadcrumbs
$breadcrumb = array();
$breadcrumb[] = l('Home', '<front>');

// Set Breadcrumbs
drupal_set_breadcrumb($breadcrumb);

?>
<?php if ((bool)variable_get('myapis')) { ?>
  <?php print l(t('Add a new API'), 'user/' . $user->uid . '/apps/add', array('attributes' => array('class' => array('add-app')))); ?>
<?php } else { ?>
  <?php print l(t('Add a new app'), 'user/' . $user->uid . '/apps/add', array('attributes' => array('class' => array('add-app')))); ?>
<?php } ?>

<form class="form-stacked">

  <?php if ($application_count) : ?>

    <h2>These are your apps!</h2>
    <h3>Add more, edit or delete them as you like.</h3>
    <hr>

    <?php
    foreach ($applications as $app) {
      print '<div class="app-delete">';
      if (!empty($app['delete_url'])) {
        print '<button class="btn primary action button-processed" title="' . t('Delete App') . '" data-url="' . $app['delete_url'] . '"></button>';
      }
      print '</div>';
      print '<div class="app-content"><h4 class="app-title">' . l($app['app_name'], $app['detail_url']) . '</h4>';
      if (!empty($app['attributes']['Description'])) {
        print '<div class="app-desc">' . check_plain($app['attributes']['Description']) . '</div>';
      }
      print '</div>';
      print '<br><hr>';
    }
    ?>
  <?php else: ?>
    <h2>Looks like you don’t have any apps</h2>
    <h3>Get started by adding one.</h3>
  <?php endif; ?>
</form>
