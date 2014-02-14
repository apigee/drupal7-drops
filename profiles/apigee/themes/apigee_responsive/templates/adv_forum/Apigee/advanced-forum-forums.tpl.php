<?php

/**
 * @file
 * Default theme implementation to display a forum which may contain forum
 * containers as well as forum topics.
 *
 * Variables available:
 * - $forum_links: An array of links that allow a user to post new forum topics.
 *   It may also contain a string telling a user they must log in in order
 *   to post. Empty if there are no topics on the page. (ie: forum overview)
 *   This is no longer printed in the template by default because it was moved
 *   to the topic list section. The variable is still available for customizations.
 * - $forums: The forums to display (as processed by forum-list.tpl.php)
 * - $topics: The topics to display (as processed by forum-topic-list.tpl.php)
 * - $forums_defined: A flag to indicate that the forums are configured.
 * - $forum_legend: Legend to go with the forum graphics.
 * - $topic_legend: Legend to go with the topic graphics.
 * - $forum_tools: Drop down menu for various forum actions.
 * - $forum_description: Description that goes with forum term. Not printed by default.
 *
 * @see template_preprocess_forums()
 * @see advanced_forum_preprocess_forums()
 */
?>

<?php if ($forums_defined): ?>
  <div id="forum">
    <?php if (!empty($forum_tools)) { ?>
      <div id="forum-tools" class="row">
        <div class="col-md-12">
          <div class="forum-tools"><?php print $forum_tools; ?></div>
        </div>
      </div>
      <hr>
    <?php } ?>
    <div id="forum-forum" class="row">
      <div class="col-md-12">
        <?php print $forums; ?>
      </div>
    </div>
    <?php if (!empty($topics)) { ?>
      <div id="forum-topics" class="row">
        <div class="col-md-12">
          <?php print $topics; ?>
        </div>
      </div>
    <?php } ?>
    <?php if (!empty($topics)): ?>
      <div id="forum-topic-legend" class="row">
        <?php print $topic_legend; ?>
      </div>
    <?php endif; ?>
    <?php if (!empty($forum_legend)): ?>
      <div id="forum-legend" class="row">
        <?php print $forum_legend; ?>
      </div>
    <?php endif; ?>
    <?php if (!empty($forum_statistics)): ?>
      <div class="row">
        <div class="col-md-12">
          <?php print $forum_statistics; ?>
        </div>
      </div>
    <?php endif; ?>
  </div>
<?php endif; ?>
