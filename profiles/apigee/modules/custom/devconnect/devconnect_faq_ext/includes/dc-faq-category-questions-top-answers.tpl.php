<?php

/**
 * @file
 * Template file for the answers section of the FAQ page if set to show
 * categorized questions at the top of the page.
 */

/**
 * Available variables:
 *
 * $display_answers
 *   Whether or not there should be any output.
 * $display_header
 *   Boolean value controlling whether a header should be displayed.
 * $header_title
 *   The category title.
 * $category_name
 *   The name of the category.
 * $answer_category_name
 *   Whether the category name should be displayed with the answers.
 * $group_questions_top
 *   Whether the questions and answers should be grouped together.
 * $category_depth
 *   The term or category depth.
 * $description
 *   The current page's description.
 * $term_image
 *   The HTML for the category image. This is empty if the taxonomy image module
 *   is not enabled or there is no image associated with the term.
 * $display_faq_count
 *   Boolean value controlling whether or not the number of faqs in a category
 *   should be displayed.
 * $question_count
 *   The number of questions in category.
 * $nodes
 *   An array of nodes to be displayed.
 *   Each node stored in the $nodes array has the following information:
 *     $node['question'] is the question text.
 *     $node['body'] is the answer text.
 *     $node['links'] represents the node links, e.g. "Read more".
 * $use_teaser
 *   Whether $node['body'] contains the full body or just the teaser text.
 * $container_class
 *   The class attribute of the element containing the sub-categories, either
 *   'faq-qa' or 'faq-qa-hide'. This is used by javascript to open/hide
 *   a category's faqs.
 * $subcat_body_list
 *   The sub-categories faqs, recursively themed (by this template).
 */

$hdr = ($category_depth ? 'h6' : 'h5');

$indent_start = $indent_end = '';
if ($display_answers && $answer_category_name && $category_depth > 0) {
  $indent_start = str_repeat('<div class="faq-category-indent">', $category_depth);
  $indent_end = str_repeat('</div>', $category_depth);
}

$header_start = $header_end = '';
if ($display_header) {
  $header_start = '<' . $hdr . ' class="faq-header">'
    . $term_image . $category_name
    . '</' . $hdr . '>'
    . '<div class="clear-block"></div>'
    . '<div class="faq-category_group"><div>';
  $header_end = '</div></div>';
}

$category_group_start = $category_group_end = '';
if ($display_header) {
  $category_group_start = '<div class="faq-category-group"><div>';
  $category_group_end = '</div></div>';
}

?>
<?php print $indent_start; ?>
<div class="faq-category-menu">
  <?php print $header_start; ?>

  <?php
  if (!$answer_category_name || $display_header) {
    if (count($subcat_body_list) > 0) {
      print join("\n", $subcat_body_list);
    }
    print $category_group_start;
    foreach ($nodes as $node) {
      print '<div class="verticals-wrap">';
      print '<h5>' . $node['question'] . '</h5>';
      print '<div class="faq-answer">' . $answer_label . ' ' . $node['body'] . '</div>';
      print "</div>\n"; // end verticals-wrap
    }
    print $category_group_end;
  }
  ?>

  <?php print $header_end; ?>
</div><!-- end faq-category-menu -->
<?php print $indent_end; ?>
<div class="backtotop"></div>
