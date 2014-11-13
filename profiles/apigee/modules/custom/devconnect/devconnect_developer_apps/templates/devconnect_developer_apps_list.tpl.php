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

$singular = _devconnect_developer_apps_get_app_label(FALSE);
$plural = _devconnect_developer_apps_get_app_label(TRUE);
if ($singular == 'API') {
  $singular_downcase = $singular;
  $plural_downcase = $plural;
}
else {
  $singular_downcase = strtolower($singular);
  $plural_downcase = strtolower($plural);
}

// Set Title
if ($user->uid == $GLOBALS['user']->uid) {
  $title = t("My $plural");
}
else {
  $title = t("@name’s $plural", array('@name' => $user->name));
}
drupal_set_title($title);

// Build Breadcrumbs
$breadcrumb = array();
$breadcrumb[] = l(t('Home'), '<front>');

// Set Breadcrumbs
drupal_set_breadcrumb($breadcrumb);

print l(t("Add a new $singular_downcase"), 'user/' . $user->uid . '/apps/add', array('attributes' => array('class' => array('add-app'))));
?>

<form class="form-stacked">

  <?php if ($application_count) : ?>

    <h2><?php print t("These are your $plural_downcase!"); ?></h2>
    <h3><?php print t('Add more, edit or delete them as you like.'); ?></h3>
    <hr>

    <?php
    foreach ($applications as $app) {
      print '<div class="app-delete">';
      if (!empty($app['delete_url'])) {
        print '<button class="btn primary action button-processed" title="' . t("Delete $singular") . '" data-url="' . $app['delete_url'] . '"></button>';
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
    <h2><?php print t("Looks like you don’t have any $plural_downcase"); ?></h2>
    <h3><?php print t('Get started by adding one.'); ?></h3>
  <?php endif; ?>
</form>
