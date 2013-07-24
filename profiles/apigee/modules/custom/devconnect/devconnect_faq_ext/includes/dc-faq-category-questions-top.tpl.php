<?php
// $Id: faq-category-questions-top.tpl.php,v 1.1.2.6 2009/02/08 17:24:49 snpower Exp $

/**
 * @file
 * Template file for the questions section of the FAQ page if set to show
 * categorized questions at the top of the page.
 */

/**
 * Available variables:
 *
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
 * $question_list
 *   An array of question links.
 * $question_list_style
 *   The style of the question list, ul for unordered, ol for ordered.
 * $subcat_list
 *   An array of sub-categories.  Each sub-category stored in the $subcat_list
 *   array has the following information:
 *     $subcat['link'] is the link to the sub-category.
 *     $subcat['description'] is the sub-category description.
 *     $subcat['count'] is the number of questions in the sub-category.
 *     $subcat['term_image'] is the sub-category (taxonomy) image.
 * $subcat_list_style
 *   The style of the sub-category list, either ol or ul (ordered or unordered).
 * $subcat_body_list
 *   The sub-categories faqs, recursively themed (by this template).
 */


?>

 <div class="tabheading">
	<div class="inner">
		<div class="addforumpost-title"><?php print $category_name; ?></div> 
	</div>	
</div>
<?php // list question links ?>
  <?php if (!empty($question_list)): ?>
  <div class="faq-general-information" >
     <div class="item-list">
     <<?php print $question_list_style; ?> class="faq-ul-questions-top" >
    <?php foreach ($question_list as $i => $question_link): ?>
      <li>
      <?php print $question_link; ?>
      </li>
    <?php endforeach; ?>
    </<?php print $question_list_style; ?>>
    </div>
   </div> 
   <!-- Close div: item-list -->
  <?php endif; ?>
   
  
