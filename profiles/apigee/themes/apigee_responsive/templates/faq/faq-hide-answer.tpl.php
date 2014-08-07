<?php

/**
 * @file
 * Template file for the FAQ page if set to show/hide the answers when the
 * question is clicked.
 */

/**
 * Available variables:
 *
 * $nodes
 *   The array of nodes to be displayed.
 *   Each $node array contains the following information:
 *     $node['question'] is the question text.
 *     $node['body'] is the answer text.
 *     $node['links'] represents the node links, e.g. "Read more".
 * $use_teaser
 *   Is true if $node['body'] is a teaser.
 */
?>
<div class="panel-group accordion" id="faq_accordion" style="margin-top:60px;">
  <?php if (count($nodes)): ?>
    <?php $index = 0; ?>
    <?php foreach ($nodes as $node): ?>
      <?php // Cycle through each of the nodes. We now have the variable $node to work with. ?>
      <div class="panel panel-default">
        <div class="panel-heading">
          <h4 class="panel-title">
            <a data-toggle="collapse" data-parent="#faq_accordion" href="#collapse_<?php print $index; ?>">
              <?php print (string) (simplexml_load_string($node['question'])->a); ?>
            </a>
          </h4>
        </div>
        <div id="collapse_<?php print $index++; ?>" class="panel-collapse collapse">
          <div class="panel-body">
            <?php print $node['body']; ?>
            <?php if (isset($node['links'])): ?>
              <?php print $node['links']; ?>
            <?php endif; ?>
          </div>
        </div>
      </div>
    <?php endforeach; ?>
  <?php endif; ?>
</div>
