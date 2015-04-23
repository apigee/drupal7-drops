<?php
/**
 * @file
 * Default theme implementation to display list of developer apps.
 *
 * Available variables:
 * $add_app - link to add an app if user has permission, otherwise FALSE.
 * $application_count - number of applications registered to the user
 * $applications - array of arrays, each of which has the following keys:
 *  - app_name
 *  - callback_url
 *  - credential (each member has apiproduct, status, displayName keys)
 *  - delete_url
 *  - created (Unix timestamp)
 *  - new_status (TRUE if app created in last 24 hrs, FALSE otherwise)
 *  - noproducts (TRUE if there are no API Products for this app, else FALSE)
 * $user - fully-populated user object (stdClass)
 * $show_status - bool indicating whether APIProduct status should be shown.
 * $show_analytics - bool indicating whether analytics link should be shown.
 * $singular - label for an app. Usually App or API. First letter is uppercase.
 * $singular_downcase - label for an app, with first letter lowercased unless
 *                      it is an acronym.
 * $plural - label for more than one app. First letter uppercase.
 * $plural_downcase - label for more than one app, downcased as above.
 */

print $add_app;
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
