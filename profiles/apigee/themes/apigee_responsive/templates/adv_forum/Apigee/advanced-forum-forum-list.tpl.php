<?php
/**
 * @file
 * Default theme implementation to display a list of forums and containers.
 *
 * Available variables:
 * - $forums: An array of forums and containers to display. It is keyed to the
 *   numeric id's of all child forums and containers.
 * - $forum_id: Forum id for the current forum. Parent to all items within
 *   the $forums array.
 *
 * Each $forum in $forums contains:
 * - $forum->is_container: Is TRUE if the forum can contain other forums. Is
 *   FALSE if the forum can contain only topics.
 * - $forum->depth: How deep the forum is in the current hierarchy.
 * - $forum->zebra: 'even' or 'odd' string used for row class.
 * - $forum->name: The name of the forum.
 * - $forum->link: The URL to link to this forum.
 * - $forum->description: The description of this forum.
 * - $forum->new_topics: True if the forum contains unread posts.
 * - $forum->new_url: A URL to the forum's unread posts.
 * - $forum->new_text: Text for the above URL which tells how many new posts.
 * - $forum->old_topics: A count of posts that have already been read.
 * - $forum->total_posts: The total number of posts in the forum.
 * - $forum->last_reply: Text representing the last time a forum was posted or
 *   commented in.
 * - $forum->forum_image: If used, contains an image to display for the forum.
 *
 * @see template_preprocess_forum_list()
 * @see theme_forum_list()
 */
?>

<?php
/*
  The $tables variable holds the individual tables to be shown. A table is
  either created from a root level container or added as needed to hold root
  level forums. The following code will loop through each of the tables.
  In each table, it loops through the items in the table. These items may be
  subcontainers or forums. Subcontainers are printed simply with the name
  spanning the entire table. Forums are printed out in more detail. Subforums
  have already been attached to their parent forums in the preprocessing code
  and will display under their parents.
 */
?>

<?php foreach ($tables as $table_id => $table): ?>
  <?php $table_info = $table['table_info']; ?>

  <div class="row advanced-forum">
    <div class="col-md-12">
      <div class="panel panel-default">
        <div class="panel-heading">
          <?php if (empty($table_info->link)): ?>
            <h3><?php print $table_info->name; ?></h3>
          <?php else: ?>
            <a href="<?php print $table_info->link; ?>"><?php print $table_info->name; ?></a>
          <?php endif; ?>
        </div>
        <?php if (!empty($table_info->description)) { ?>
          <div class="panel-body">
            <?php print $table_info->description; ?>
          </div>
        <?php } ?>
        <div id="forum-table-<?php print $table_info->tid; ?>">
          <div class="table-responsive">
            <table class="table table-striped">
              <thead>
              <tr>
                <th>&nbsp;</th>
                <th><?php print t('Forum'); ?></th>
                <th><?php print t('Topics'); ?></th>
                <th><?php print t('Posts'); ?></th>
                <th><?php print t('Last post'); ?></th>
              </tr>
              </thead>
              <tbody id="forum-table-<?php print $table_info->tid; ?>-content">
              <?php foreach ($table['items'] as $item_id => $item): ?>
                <?php if ($item->is_container): ?>
                  <tr id="subcontainer-<?php print $item_id; ?>" class="forum-row <?php print $item->zebra; ?> container-<?php print $item_id; ?>-child">
                <?php else: ?>
                  <tr id="forum-<?php print $item_id; ?>" class="forum-row <?php print $item->zebra; ?> container-<?php print $item_id; ?>-child">
                <?php endif; ?>

                <?php if (!empty($item->forum_image)): ?>
                  <?php if (strpos($item->forum_image, 'new') !== false) { ?>
                    <td class="forum-image forum-image-<?php print $item_id; ?>">
                      <h3>
                        <div class="label label-success">
                          <span class="glyphicon glyphicon glyphicon-bookmark"></span>
                        </div>
                      </h3>
                    </td>
                  <?php } else { ?>
                    <td class="forum-image forum-image-<?php print $item_id; ?>">
                      <h3>
                        <div class="label label-info">
                          <span class="glyphicon glyphicon-bell"></span>
                        </div>
                      </h3>
                    </td>
                  <?php } ?>
                <?php else: ?>
                  <?php if (strpos($item->icon_classes, 'new') !== false) { ?>
                    <td class="<?php print $item->icon_classes ?>">
                      <h3>
                        <div class="label label-success">
                          <span class="glyphicon glyphicon glyphicon-bookmark"></span>
                        </div>
                      </h3>
                    </td>
                  <?php } else { ?>
                    <td class="<?php print $item->icon_classes ?>">
                      <h3>
                        <div class="label label-info">
                          <span class="glyphicon glyphicon-bell"></span>
                        </div>
                      </h3>
                    </td>
                  <?php } ?>
                <?php endif; ?>

                <?php $colspan = ($item->is_container) ? 4 : 1 ?>
                <td class="forum-details" colspan="<?php print $colspan ?>">
                  <div class="forum-name">
                    <a href="<?php print $item->link; ?>"><?php print $item->name; ?></a>
                  </div>
                  <?php if (!empty($item->description)): ?>
                    <div class="forum-description">
                      <hr>
                      <?php print $item->description; ?>
                    </div>
                  <?php endif; ?>

                  <?php if (!empty($item->subcontainers)): ?>
                    <div class="forum-subcontainers">
                      <hr>
                      <span class="label label-default">Subcontainers</span> <?php print $item->subcontainers; ?>
                    </div>
                  <?php endif; ?>

                  <?php if (!empty($item->subforums)): ?>
                    <div>
                      <hr>
                      <span class="label label-default">Subforums</span> <?php print $item->subforums; ?>
                    </div>
                  <?php endif; ?>
                </td>
                <?php if (!$item->is_container): ?>
                  <td class="forum-number-topics">
                    <div class="forum-number-topics">
                      <span class="label label-default">
                      <?php print $item->total_topics ?>
                      </span>
                      <br>
                      <?php if ($item->new_topics): ?>
                        <span class="label label-success">
                          <a href="<?php print $item->new_topics_link; ?>"><?php print $item->new_topics_text; ?></a>
                        </span>
                      <?php endif; ?>
                    </div>
                  </td>

                  <td class="forum-number-posts">
                      <span class="label label-default">
                        <?php print $item->total_posts ?>
                      </span>
                    <br>
                    <?php if ($item->new_posts): ?>
                      <br />
                      <span class="label label-info">
                        <a href="<?php print $item->new_posts_link; ?>"><?php print $item->new_posts_text; ?></a>
                      </span>
                    <?php endif; ?>
                  </td>
                  <td class="forum-last-reply">
                    <?php print $item->last_post ?>
                  </td>
                <?php endif; ?>
                </tr>
              <?php endforeach; ?>
              </tbody>
            </table>
          </div>
        </div>
        <div class="panel-footer"></div>
      </div>
    </div>
  </div>
<?php endforeach; ?>
