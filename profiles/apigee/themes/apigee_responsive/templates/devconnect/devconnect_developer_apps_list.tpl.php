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
$i = 0;

$label_callback = function ($status, $pull_right = FALSE) {
  if ($status == 'Revoked') {
    return '<span class="label label-danger' . ($pull_right ? ' pull-right' : '') . '">' . t('Revoked') . '</span>';
  }
  elseif ($status == 'Pending') {
    return '<span class="label label-default' . ($pull_right ? ' pull-right' : '') . '">' . t('Pending') . '</span>';
  }
  return '<span class="label label-success' . ($pull_right ? ' pull-right' : '') . '">' . t('Approved') . '</span>';
};

?>
<?php if ($add_app): ?>
  <div class="row">
    <div class="col-sm-12">
      <div class="add-app-button pull-right">
        <?php print $add_app; ?>
      </div>
    </div>
  </div>
<?php endif; ?>

<?php if ($application_count): ?>
  <div class="row">
    <div class="col-sm-12">
      <h2 class="featurette-heading"><?php print t("These are your $plural_downcase!"); ?>
        <span class="text-muted"><?php print t('Explore them!'); ?></span>
      </h2>
      <hr>
    </div>
  </div>
  <div class="row">
  <div class="col-sm-12">
  <?php // more than one application ?>
  <?php foreach ($applications as $app): ?>
    <?php $status = apigee_responsive_app_status($app); ?>
    <div class="panel-group" id="my-apps-accordion">
      <div class="panel panel-default">
        <div class="panel-heading">
          <h4 class="panel-title">
            <div class="truncate">
              <?php if ((bool) $app['new_status']) : ?><span class="badge">new</span>&nbsp;&nbsp;<?php endif; ?>
              <a data-toggle="collapse" data-parent="#my-apps-accordion" href="#my-apps-collapse<?php print $i; ?>"><strong><?php print $app['app_name']; ?></strong></a>
            </div>
            <div class="status-label"><?php print $label_callback($status, TRUE); ?></div>
          </h4>
        </div>
        <div id="my-apps-collapse<?php print $i; ?>" class="my-apps-panels panel-collapse collapse">
          <div class="panel-body">
            <ul class="nav nav-pills">
              <li class="active"><a data-toggle="tab" href="#keys<?php print $i; ?>"><?php print t('Keys'); ?></a></li>
    <?php if (!$app['noproducts']) : ?>
              <li><a data-toggle="tab" href="#profile<?php print $i; ?>"><?php print t('Products'); ?></a></li>
      <?php if (isset($app['smartdocs'])) : ?>
              <li><a data-toggle="tab" href="#docs<?php print $i; ?>"><?php print t('Docs'); ?></a></li>
      <?php endif; ?>
    <?php endif; ?>
              <li><a data-toggle="tab" href="#details<?php print $i; ?>"><?php print t('Details'); ?></a></li>
    <?php if ($show_analytics): ?>
              <li><?php print l(t('Analytics'), $app['detail_url']); ?></li>
    <?php endif; ?>
    <?php if (user_access("edit developer apps")): ?>
              <li class="hidden-xs apigee-modal-link-edit"><a href="<?php print base_path() . $app['edit_url']; ?>" data-toggle="modal" data-target="#<?php print $app['edit_url_id']; ?>"><?php print t('Edit "%n"', array('%n' => $app['app_name'])); ?></a></li>
              <li class="visible-xs apigee-modal-link-edit"><a href="<?php print base_path() . $app['edit_url']; ?>" data-toggle="modal" data-target="#<?php print $app['edit_url_id']; ?>"><?php print t('Edit'); ?></a></li>
    <?php endif; // edit developer apps ?>
    <?php if (user_access("delete developer apps")): ?>
              <li class="hidden-xs apigee-modal-link-delete"><a href="<?php print base_path() . $app['delete_url']; ?>" data-toggle="modal" data-target="#<?php print $app['delete_url_id']; ?>"><?php print t('Delete "%n"', array('%n' => $app['app_name'])); ?></a></li>
              <li class="visible-xs apigee-modal-link-delete"><a href="<?php print base_path() . $app['delete_url']; ?>" data-toggle="modal" data-target="#<?php print $app['delete_url_id']; ?>"><?php print t('Delete'); ?></a></li>
    <?php endif; // delete developer apps ?>
            </ul>
    <!-- Edit Modal -->
            <div class="modal fade" id="<?php print $app['edit_url_id']; ?>" tabindex="-1">
              <div class="modal-dialog">
                <div class="modal-content">
                  <div class="modal-header">
                    <button type="button" class="close" data-dismiss="modal" aria-hidden="true">&times;</button>
                    <h4 class="modal-title" id="<?php print $app['edit_url_id']; ?>"><?php print t('Edit @name', array('@name' => $app['app_name'])); ?></h4>
                  </div>
                  <div class="modal-body"></div>
                  <div class="modal-footer">
                    <button type="button" class="btn btn-default" data-dismiss="modal">Close</button>
                  </div>
                </div>
              </div>
            </div>
    <!-- Delete Modal -->
            <div class="modal fade" id="<?php print $app['delete_url_id']; ?>" tabindex="-1">
              <div class="modal-dialog">
                <div class="modal-content">
                  <div class="modal-header">
                    <button type="button" class="close" data-dismiss="modal" aria-hidden="true">&times;</button>
                    <h4 class="modal-title" id="<?php print $app['delete_url_id']; ?>"><?php print t('Delete @name', array('@name' => $app['app_name'])); ?></h4>
                  </div>
                  <div class="modal-body"></div>
                  <div class="modal-footer"></div>
                </div>
              </div>
            </div>
            <div class="tab-content" id="myTabContent">
              <div id="keys<?php print $i; ?>" class="tab-pane fade in active">
                <hr>
                <div class="panel panel-default">
                  <div class="panel-heading"><strong><?php print t('@name’s Keys', array('@name' => $app['app_name'])); ?></strong></div>
        <?php if ($app['new_status'] && !$app['noproducts']) : ?>
                  <div class="panel-body">
                    <p><?php print t('Below are keys you can use to access the API products associated with this application <em><span class="text-muted">(@name)</span></em>.
                      The actual keys need to be approved <em>and</em> approved for an <em>API product</em> to be capable of accessing any of the URIs defined in the API
                      product.', array('@name' => $app['app_name'])); ?></p>
                  </div>
        <?php endif; // new_status && !noproducts ?>
                  <div class="table-responsive">
                    <table class="table" style="border:0;">
                      <tbody>
                        <tr>
                          <td class="key"><strong><?php print t('Consumer Key'); ?></strong></td>
                          <td>
                            <span<?php print ($status == 'Revoked' || $status == 'Pending' ? ' class="striked"' : ''); ?>"><?php print $app['credential']['consumerKey']; ?></span>
        <?php if ($status == 'Pending'): ?>
                            <hr>
                            Some products associated with this application are in <span class="label label-default">pending</span> status.
                            <hr>
                            <ul style="margin:0;padding:0;">
                              <?php foreach ($app['credential']['apiProducts'] as $product): ?>
                                <?php if ($product['status'] == 'pending'): ?>
                                  <li style="margin:0;padding:0;list-style-type:none;"><?php print $product['displayName']; ?></li>
                                <?php endif; ?>
                              <?php endforeach; ?>
                            </ul>
        <?php endif; // status Pending ?>
        <?php if ($status == 'Revoked'): ?>
                            <hr>
                            Some products associated with this application are in <span class="label label-default">revoked</span> status.
                            <hr>
                            <ul style="margin:0;padding:0;">
                              <?php foreach ($app['credential']['apiProducts'] as $product): ?>
                                <?php if ($product['status'] == 'revoked'): ?>
                                  <li style="margin:0;padding:0;list-style-type:none;"><?php print $product['displayName']; ?></li>
                                <?php endif; ?>
                              <?php endforeach; ?>
                            </ul>
        <?php endif; // status Revoked ?>
                          </td>
                        </tr>
                        <tr>
                          <td class="key"><strong><?php print t('Consumer Secret'); ?></strong></td>
                          <td>
                            <span<?php print ($status == 'Revoked' || $status == 'Pending' ? ' class="striked"' : ''); ?>"><?php print $app['credential']['consumerSecret']; ?></span>
                          </td>
                        </tr>
                      </tbody>
                    </table>
                  </div><!-- /table-responsive -->
                </div><!-- /panel -->
              </div><!-- /keys<?php print $i; ?> -->
        <?php if (!$app['noproducts']): ?>
              <div id="profile<?php print $i; ?>" class="tab-pane fade">
                <hr>
        <?php foreach ($app['credential']['apiProducts'] as $product): ?>
                <div class="panel panel-default">
                  <div class="panel-heading"><?php print t('API Product:'); ?> <strong><?php print $product['displayName']; ?></strong></div>
                  <div class="table-responsive">
                    <table class="table" style="border:0;">
                      <tbody>
                        <tr>
                          <td class="key"><strong><?php print t('Status'); ?></strong></td>
                          <td><?php print $label_callback($status); ?></td>
                        </tr>
                      </tbody>
                    </table>
                  </div>
                </div>
                <br>
        <?php endforeach; // apiProducts ?>
              </div>
        <?php if (isset($app['smartdocs'])) : ?>
              <div id="docs<?php print $i; ?>" class="tab-pane fade">
                <hr>
                <div class="panel-heading"><?php print t('Quick Reference:'); ?></div>
          <?php foreach ($app['credential']['apiProducts'] as $product): ?>
            <?php foreach ($app['smartdocs'] as $docs): ?>
              <?php foreach ($docs as $link): ?>
                  <div class="panel panel-default">
                    <div class="panel-heading">
                      <strong><?php print $product['displayName']; ?></strong> <?php print t('Documentation:'); ?> <?php print $link; ?></div>
                  </div>
                  <br>
              <?php endforeach; // link ?>
            <?php endforeach; // docs ?>
          <?php endforeach; // product ?>
                </div>
      <?php endif; // smartdocs ?>
    <?php endif; // !noproducts ?>
                <div id="details<?php print $i; ?>" class="tab-pane fade">
                <hr>
                  <div class="panel panel-default">
                    <div class="panel-heading"><strong><?php print t('@name’s Details', array('@name' => $app['app_name'])); ?></strong></div>
                    <div class="table-responsive">
                      <table class="table" style="border:0;">
                        <tbody>
                          <tr>
                            <td class="key"><strong><?php print t('Application Name'); ?></strong></td>
                            <td><?php print check_plain($app['app_name']); ?></td>
                          </tr>
    <?php if (!$app['noproducts']): ?>
                          <tr>
                            <td class="key"><strong><?php print t('API Products'); ?></strong></td>
                            <td>
                              <ul style="margin:0;padding:0;">
                                <?php foreach ($app['credential']['apiProducts'] as $product): ?>
                                  <li style="margin:0;padding:0;list-style-type:none;"><?php print $product['displayName']; ?></li>
                                <?php endforeach; ?>
                              </ul>
                            </td>
                          </tr>
    <?php endif; // !noproducts ?>
    <?php if (!empty($app['attributes'])): ?>
      <?php foreach ($app['attributes'] as $name => $attr): ?>
                          <tr>
                            <td class="key"><strong><?php print check_plain($name); ?></strong></td>
                            <td><?php print check_plain($attr); ?></td>
                          </tr>
      <?php endforeach; // attributes ?>
    <?php endif; // attributes ?>
    <?php if (!empty($app['callback_url'])): ?>
                          <tr>
                            <td class="key"><strong><?php print t('Callback URL'); ?></strong></td>
                            <td><?php print $app['callback_url']; ?></td>
                          </tr>
    <?php endif; // callback_url ?>
                        </tbody>
                      </table>
                    </div>
                  </div>
                </div>
                <div id="analytics<?php print $i; ?>" class="tab-pane fade">
                  <hr>
                  <?php print render($app['analytics_filter_form']); ?>
                </div>
              </div>
            </div>
          </div>
        </div>
        <br>
      <?php $i++;
        endforeach; // applications ?>
      </div>
    </div>
  </div>
<?php else : ?>
  <?php // only one application ?>
  <div class="row">
    <div class="col-sm-12">
      <h2 class="featurette-heading"><?php print t("These are your $plural_downcase!"); ?>
        <span class="text-muted"><?php print t('Explore them!'); ?></span>
      </h2>
    </div>
  </div>
<?php endif; // application_count ?>