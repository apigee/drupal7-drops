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
?>
<?php if($add_app):?>
	<div class="row">
		<div class="col-sm-12">
			<div class="add-app-button pull-right">
				<?php print $add_app; ?>
			</div>
		</div>
	</div>
<?php endif; ?>
<?php if ($application_count) { ?>
	<div class="row">
		<div class="col-sm-12">
			<?php if ((bool)variable_get('myapis')) { ?>
				<h2 class="featurette-heading">These are your APIs!
					<span class="text-muted">Explore them!</span>
				</h2>
			<?php } else { ?>
				<h2 class="featurette-heading">These are your apps!
					<span class="text-muted">Explore them!</span>
				</h2>
			<?php } ?>
			<hr>
		</div>
	</div>
	<div class="row">
	<div class="col-sm-12">
	<?php // more than one application ?>
	<?php foreach ($applications as $app) { ?>
	<?php $pending_status = 0; ?>
	<?php $revoked_status = 0; ?>
	<?php foreach($app['credential']['apiProducts'] as $product) { ?>
		<?php foreach($app['credential']['apiProducts'] as $product) { ?>
			<?php
			switch ($product['status']) {
				case 'pending':
					$pending_status = 1;
					break;
				case 'revoked':
					$revoked_status = 1;
					break;
				default:
					break;
			}
			?>
		<?php } ?>
	<?php } ?>
	<div class="panel-group" id="my-apps-accordion">
	<div class="panel panel-default">
	<div class="panel-heading">
		<h4 class="panel-title">
			<div class="truncate">
				<?php if ((bool)$app['new_status']) { ?>
					<span class="badge">new</span>&nbsp;&nbsp;
				<?php } ?>
				<a data-toggle="collapse" data-parent="#my-apps-accordion" href="#my-apps-collapse<?php print $i; ?>">
					<strong><?php print $app['app_name']; ?></strong>
				</a>
			</div>
			<div class="status-label">
				<?php if ((bool) $revoked_status) { ?>
					<span class="label label-danger pull-right">Revoked</span>
				<?php } else if ((bool) $pending_status) { ?>
					<span class="label label-default pull-right">Pending</span>
				<?php } else { ?>
					<span class="label label-success pull-right">Approved</span>
				<?php } ?>
			</div>
		</h4>
	</div>
	<div id="my-apps-collapse<?php print $i; ?>" class="my-apps-panels panel-collapse collapse">
	<div class="panel-body">
	<ul class="nav nav-pills">
		<li class="active"><a data-toggle="tab" href="#keys<?php print $i; ?>">Keys</a></li>
		<?php if (!$app['noproducts']) { ?>
			<li><a data-toggle="tab" href="#profile<?php print $i; ?>">Products</a></li>
			<?php if (isset($app['smartdocs'])) { ?>
				<li><a data-toggle="tab" href="#docs<?php print $i; ?>">Docs</a></li>
			<?php } ?>
		<?php } ?>
		<li><a data-toggle="tab" href="#details<?php print $i; ?>">Details</a></li>
		<?php if ($show_analytics) { ?>
			<li>
				<?php print l('Analytics', $app['detail_url']); ?>
			</li>
		<?php } ?>
    <?php if(user_access("edit developer apps")){ ?>
		<li class="hidden-xs apigee-modal-link-edit">
			<a href="/<?php print $app['edit_url']; ?>" data-toggle="modal" data-target="#<?php print $app['edit_url_id']; ?>">
				<?php print t('Edit "%n"', array('%n' => $app['app_name'])); ?></a></li>
		<li class="visible-xs apigee-modal-link-edit">
			<a href="/<?php print $app['edit_url']; ?>" data-toggle="modal" data-target="#<?php print $app['edit_url_id']; ?>">
				<?php print t('Edit'); ?></a></li>
    <?php }?>
    <?php if(user_access("delete developer apps")){ ?>
		<li class="hidden-xs apigee-modal-link-delete">
			<a href="/<?php print $app['delete_url']; ?>" data-toggle="modal" data-target="#<?php print $app['delete_url_id']; ?>">
				<?php print t('Delete "%n"', array('%n' => $app['app_name'])); ?></a></li>
		<li class="visible-xs apigee-modal-link-delete">
			<a href="/<?php print $app['delete_url']; ?>" data-toggle="modal" data-target="#<?php print $app['delete_url_id']; ?>">
				<?php print t('Delete'); ?></a></li>
    <?php }?>
	</ul>
	<!-- Delete Modal -->
	<div class="modal fade" id="<?php print $app['edit_url_id']; ?>" tabindex="-1">
		<div class="modal-dialog">
			<div class="modal-content">
				<div class="modal-header">
					<button type="button" class="close" data-dismiss="modal" aria-hidden="true">&times;</button>
					<h4 class="modal-title" id="<?php print $app['edit_url_id']; ?>">Edit <?php print $app['app_name']; ?></h4>
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
					<h4 class="modal-title" id="<?php print $app['delete_url_id']; ?>">Delete <?php print $app['app_name']; ?></h4>
				</div>
				<div class="modal-body"></div>
				<div class="modal-footer">
					
				</div>
			</div>
		</div>
	</div>
	<div class="tab-content" id="myTabContent">
		<div id="keys<?php print $i; ?>" class="tab-pane fade in active">
			<hr/>
			<div class="panel panel-default">
				<div class="panel-heading"><strong><?php print $app['app_name']; ?>'s Keys</strong></div>
				<?php if ((bool) $app['new_status']) { ?>
					<?php if (!$app['noproducts']) { ?>
						<div class="panel-body">
							<p>Below are keys you can use to access the API products associated with this application
								<em><span class="text-muted">(<?php print $app['app_name']; ?>)</span></em>.
								The actual keys need to be approved <em>and</em> approved for an <em>API product</em>
								to be capable of accessing any of the URIs defined in the API product.</p>
						</div>
					<?php } ?>
				<?php } ?>
				<div class="table-responsive">
					<table class="table" style="border:0;">
						<tbody>
						<tr>
							<td class="key"><strong>Consumer Key</strong></td>
							<td>
								<?php $striked = 0; ?>
								<?php if ((bool) $pending_status) { ?>
									<?php $striked = 1; ?>
								<?php } else if ((bool) $revoked_status) { ?>
									<?php $striked = 1; ?>
								<?php } ?>
								<span class="<?php if ((bool) $striked): print 'striked'; else: print ''; endif; ?>">
                          <?php print $app['credential']['consumerKey']; ?></span>
								<?php if ((bool) $pending_status) { ?>
									<hr/>
									Some products associated with this application are in <span class="label label-default">pending</span> status.
									<hr/>
									<ul style="margin:0;padding:0;">
										<?php foreach($app['credential']['apiProducts'] as $product) { ?>
											<?php if ($product['status'] == 'pending') { ?>
												<li style="margin:0;padding:0;list-style-type:none;"><?php print $product['displayName']; ?></li>
											<?php } ?>
										<?php } ?>
									</ul>
								<?php } ?>
								<?php if ((bool) $revoked_status) { ?>
									<hr/>
									Some products associated with this application are in <span class="label label-danger">revoked</span> status.
									<hr/>
									<ul style="margin:0;padding:0;">
										<?php foreach($app['credential']['apiProducts'] as $product) { ?>
											<?php if ($product['status'] == 'revoked') { ?>
												<li style="margin:0;padding:0;list-style-type:none;"><?php print $product['displayName']; ?></li>
											<?php } ?>
										<?php } ?>
									</ul>
								<?php } ?>
							</td>
						</tr>
						<tr>
							<td class="key"><strong>Consumer Secret</strong></td>
							<td>
								<?php $striked = 0; ?>
								<?php if ((bool) $pending_status) { ?>
									<?php $striked = 1; ?>
								<?php } else if ((bool) $revoked_status) { ?>
									<?php $striked = 1; ?>
								<?php } ?>
								<span class="<?php if ((bool) $striked): print 'striked'; else: print ''; endif; ?>">
                          <?php print $app['credential']['consumerSecret']; ?>
                                                      </span>
							</td>
						</tr>
						</tbody>
					</table>
				</div>
			</div>
		</div>
		<?php if (!$app['noproducts']) { ?>
			<div id="profile<?php print $i; ?>" class="tab-pane fade">
				<hr/>
				<?php foreach($app['credential']['apiProducts'] as $product) { ?>
					<div class="panel panel-default">
						<div class="panel-heading">API Product: <strong><?php print $product['displayName']; ?></strong></div>
						<div class="table-responsive">
							<table class="table" style="border:0;">
								<tbody>
								<tr>
									<td class="key"><strong>Status</strong></td>
									<td>
										<?php if ($product['status'] == 'pending') { ?>
											<span class="label label-default">Pending</span>
										<?php } else if ($product['status'] == 'revoked') { ?>
											<span class="label label-danger">Revoked</span>
										<?php } else { ?>
											<span class="label label-success">Approved</span>
										<?php } ?>
									</td>
								</tr>
								</tbody>
							</table>
						</div>
					</div>
					<br/>
				<?php } ?>
			</div>
			<?php if (isset($app['smartdocs'])) { ?>
				<div id="docs<?php print $i; ?>" class="tab-pane fade">
					<hr/>
					<div class="panel-heading">Quick Reference:</div>

					<?php foreach($app['credential']['apiProducts'] as $product) { ?>
						<?php foreach($app['smartdocs'] as $docs) { ?>
							<?php foreach($docs as $link) { ?>
								<div class="panel panel-default">
									<div class="panel-heading"><strong><?php print $product['displayName']; ?></strong> Documentation: <?php print $link; ?></div>
								</div>
								<br/>
							<?php } ?>
						<?php } ?>
					<?php } ?>
				</div>
			<?php } ?>
		<?php } ?>
		<div id="details<?php print $i; ?>" class="tab-pane fade">
			<hr/>
			<div class="panel panel-default">
				<div class="panel-heading"><strong><?php print $app['app_name']; ?>'s Details</strong></div>
				<div class="table-responsive">
					<table class="table" style="border:0;">
						<tbody>
						<tr>
							<td class="key"><strong>Application Name</strong></td>
							<td><?php print $app['app_name']; ?></td>
						</tr>
						<?php if (!$app['noproducts']) { ?>
							<tr>
								<td class="key"><strong>API Products</strong></td>
								<td>
									<ul style="margin:0;padding:0;">
										<?php foreach($app['credential']['apiProducts'] as $product) { ?>
											<li style="margin:0;padding:0;list-style-type:none;"><?php print $product['displayName']; ?></li>
										<?php } ?>
									</ul>
								</td>
							</tr>
						<?php } ?>
						<?php if (!empty($app['attributes'])) { ?>
							<?php foreach($app['attributes'] as $name => $attr) { ?>
								<tr>
									<td class="key"><strong><?php print $name; ?></strong></td>
									<td><?php print $attr; ?></td>
								</tr>
							<?php } ?>
						<?php } ?>
						<?php if (!empty($app['callback_url'])) { ?>
							<tr>
								<td class="key"><strong>Callback URL</strong></td>
								<td><?php print $app['callback_url']; ?></td>
							</tr>
						<?php } ?>
						</tbody>
					</table>
				</div>
			</div>
		</div>
		<div id="analytics<?php print $i; ?>" class="tab-pane fade">
			<hr/>
			<?php print render($app['analytics_filter_form']); ?>
		</div>
	</div>
	</div>
	</div>
	</div>
	<br/>
	<?php $i++;} ?>
	</div>
	</div>
	</div>

<?php } else { ?>
	<?php // only one application ?>
	<div class="row">
		<div class="col-sm-12">
			<?php if ((bool)variable_get('myapis')) { ?>
				<h2 class="featurette-heading">These are your APIs!
					<span class="text-muted">Explore them!</span>
				</h2>
			<?php } else { ?>
				<h2 class="featurette-heading">These are your apps!
					<span class="text-muted">Explore them!</span>
				</h2>
			<?php } ?>
		</div>
	</div>
<?php } ?>