<?php

/**
 * Standard Template for Views generation
 *
 * @param $mid
 * @param $smart_mid
 * @param $smart_field
 * @param $display
 * @return string
 */
function smartdocs_view($mid, $smart_mid, $smart_field, $display) {

  $actual = $mid;
  $long = $smart_mid;
  $mid = $smart_field;

  return '<?php

  $view = new view();
$view->name = "' . $long . '_listing";
$view->description = "' . $long . ' Listing Description";
$view->tag = "default";
$view->base_table = "node";
$view->human_name = "' . $display . ' Model Overview";
$view->core = 7;
$view->api_version = "3.0";
$view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

/* Display: Master */
$handler = $view->new_display("default", "Master", "default");
$handler->display->display_options["title"] = "' . $display . ' Documentation";
$handler->display->display_options["use_more_always"] = FALSE;
$handler->display->display_options["access"]["type"] = "perm";
$handler->display->display_options["cache"]["type"] = "none";
$handler->display->display_options["query"]["type"] = "views_query";
$handler->display->display_options["exposed_form"]["type"] = "basic";
$handler->display->display_options["pager"]["type"] = "none";
$handler->display->display_options["pager"]["options"]["offset"] = "0";
$handler->display->display_options["style_plugin"] = "default";
$handler->display->display_options["style_options"]["grouping"] = array(
  0 => array(
    "field" => "field_' . $mid . '_model_tags",
    "rendered" => 1,
    "rendered_strip" => 0,
  ),
);
$handler->display->display_options["row_plugin"] = "fields";
/* Field: Content: Title */
$handler->display->display_options["fields"]["title"]["id"] = "title";
$handler->display->display_options["fields"]["title"]["table"] = "node";
$handler->display->display_options["fields"]["title"]["field"] = "title";
$handler->display->display_options["fields"]["title"]["label"] = "";
$handler->display->display_options["fields"]["title"]["exclude"] = TRUE;
$handler->display->display_options["fields"]["title"]["alter"]["word_boundary"] = FALSE;
$handler->display->display_options["fields"]["title"]["alter"]["ellipsis"] = FALSE;
$handler->display->display_options["fields"]["title"]["element_label_colon"] = FALSE;
/* Field: Content: authschemes */
$handler->display->display_options["fields"]["field_' . $mid . '_model_authschemes"]["id"] = "field_' . $mid . '_model_authschemes";
$handler->display->display_options["fields"]["field_' . $mid . '_model_authschemes"]["table"] = "field_data_field_' . $mid . '_model_authschemes";
$handler->display->display_options["fields"]["field_' . $mid . '_model_authschemes"]["field"] = "field_' . $mid . '_model_authschemes";
$handler->display->display_options["fields"]["field_' . $mid . '_model_authschemes"]["label"] = "";
$handler->display->display_options["fields"]["field_' . $mid . '_model_authschemes"]["exclude"] = TRUE;
$handler->display->display_options["fields"]["field_' . $mid . '_model_authschemes"]["alter"]["alter_text"] = TRUE;
$handler->display->display_options["fields"]["field_' . $mid . '_model_authschemes"]["alter"]["text"] = "<span title=\'This method needs authentication.\' class=\'icon_lock\'></span>";
$handler->display->display_options["fields"]["field_' . $mid . '_model_authschemes"]["element_label_colon"] = FALSE;
$handler->display->display_options["fields"]["field_' . $mid . '_model_authschemes"]["type"] = "taxonomy_term_reference_plain";
$handler->display->display_options["fields"]["field_' . $mid . '_model_authschemes"]["delta_offset"] = "0";
/* Field: Content: Body */
$handler->display->display_options["fields"]["body"]["id"] = "body";
$handler->display->display_options["fields"]["body"]["table"] = "field_data_body";
$handler->display->display_options["fields"]["body"]["field"] = "body";
$handler->display->display_options["fields"]["body"]["label"] = "";
$handler->display->display_options["fields"]["body"]["exclude"] = TRUE;
$handler->display->display_options["fields"]["body"]["element_label_colon"] = FALSE;
$handler->display->display_options["fields"]["body"]["type"] = "text_plain";
/* Field: Content: Path */
$handler->display->display_options["fields"]["field_' . $mid . '_model_path"]["id"] = "field_' . $mid . '_model_path";
$handler->display->display_options["fields"]["field_' . $mid . '_model_path"]["table"] = "field_data_field_' . $mid . '_model_path";
$handler->display->display_options["fields"]["field_' . $mid . '_model_path"]["field"] = "field_' . $mid . '_model_path";
$handler->display->display_options["fields"]["field_' . $mid . '_model_path"]["label"] = "";
$handler->display->display_options["fields"]["field_' . $mid . '_model_path"]["exclude"] = TRUE;
$handler->display->display_options["fields"]["field_' . $mid . '_model_path"]["element_label_colon"] = FALSE;
/* Field: Content: tags */
$handler->display->display_options["fields"]["field_' . $mid . '_model_tags"]["id"] = "field_' . $mid . '_model_tags";
$handler->display->display_options["fields"]["field_' . $mid . '_model_tags"]["table"] = "field_data_field_' . $mid . '_model_tags";
$handler->display->display_options["fields"]["field_' . $mid . '_model_tags"]["field"] = "field_' . $mid . '_model_tags";
$handler->display->display_options["fields"]["field_' . $mid . '_model_tags"]["label"] = "";
$handler->display->display_options["fields"]["field_' . $mid . '_model_tags"]["exclude"] = TRUE;
$handler->display->display_options["fields"]["field_' . $mid . '_model_tags"]["alter"]["alter_text"] = TRUE;
$handler->display->display_options["fields"]["field_' . $mid . '_model_tags"]["alter"]["text"] = "<h3 class=\'method-tag-header\'>[field_' . $mid . '_model_tags]</h3>
<div class=\'clearfix method_details\'>
                        <div class=\'method_data title-header\'>Method</div>
                        <div class=\'method_data description-header\'>Description</div>
                    </div>";
$handler->display->display_options["fields"]["field_' . $mid . '_model_tags"]["element_label_colon"] = FALSE;
$handler->display->display_options["fields"]["field_' . $mid . '_model_tags"]["type"] = "taxonomy_term_reference_plain";
$handler->display->display_options["fields"]["field_' . $mid . '_model_tags"]["group_rows"] = FALSE;
$handler->display->display_options["fields"]["field_' . $mid . '_model_tags"]["delta_offset"] = "0";
/* Field: Content: verb */
$handler->display->display_options["fields"]["field_' . $mid . '_model_verb"]["id"] = "field_' . $mid . '_model_verb";
$handler->display->display_options["fields"]["field_' . $mid . '_model_verb"]["table"] = "field_data_field_' . $mid . '_model_verb";
$handler->display->display_options["fields"]["field_' . $mid . '_model_verb"]["field"] = "field_' . $mid . '_model_verb";
$handler->display->display_options["fields"]["field_' . $mid . '_model_verb"]["label"] = "";
$handler->display->display_options["fields"]["field_' . $mid . '_model_verb"]["exclude"] = TRUE;
$handler->display->display_options["fields"]["field_' . $mid . '_model_verb"]["element_label_colon"] = FALSE;
$handler->display->display_options["fields"]["field_' . $mid . '_model_verb"]["type"] = "taxonomy_term_reference_plain";
/* Field: Global: Custom text */
$handler->display->display_options["fields"]["nothing"]["id"] = "nothing";
$handler->display->display_options["fields"]["nothing"]["table"] = "views";
$handler->display->display_options["fields"]["nothing"]["field"] = "nothing";
$handler->display->display_options["fields"]["nothing"]["label"] = "";
$handler->display->display_options["fields"]["nothing"]["alter"]["text"] = "<div class=\'method_details clearfix\'>
                                <div class=\'method_data title\'>
<div class=\'verb-auth\'>
                                    <p data-role=\'verb\' class=\'[field_' . $mid . '_model_verb]\'>[field_' . $mid . '_model_verb]</p>
[field_' . $mid . '_model_authschemes]
</div>
<div class=\'title-description\'>
                                    [title]
<p title=\'[field_' . $mid . '_model_path]\' class=\'resource_path\' data-role=\'resource_path\'>[field_' . $mid . '_model_path]</p>
</div>
                                </div>
                                <div class=\'method_data description\'>
                                [body]
                                </div>
                            </div>";
$handler->display->display_options["fields"]["nothing"]["element_label_colon"] = FALSE;
/* Sort criterion: Content: Post date */
$handler->display->display_options["sorts"]["created"]["id"] = "created";
$handler->display->display_options["sorts"]["created"]["table"] = "node";
$handler->display->display_options["sorts"]["created"]["field"] = "created";
$handler->display->display_options["sorts"]["created"]["order"] = "DESC";
/* Filter criterion: Content: Published */
$handler->display->display_options["filters"]["status"]["id"] = "status";
$handler->display->display_options["filters"]["status"]["table"] = "node";
$handler->display->display_options["filters"]["status"]["field"] = "status";
$handler->display->display_options["filters"]["status"]["value"] = 1;
$handler->display->display_options["filters"]["status"]["group"] = 1;
$handler->display->display_options["filters"]["status"]["expose"]["operator"] = FALSE;
/* Filter criterion: Content: Type */
$handler->display->display_options["filters"]["type"]["id"] = "type";
$handler->display->display_options["filters"]["type"]["table"] = "node";
$handler->display->display_options["filters"]["type"]["field"] = "type";
$handler->display->display_options["filters"]["type"]["value"] = array(
  "' . $long . '" => "' . $long . '",
);

/* Display: Page */
$handler = $view->new_display("page", "Page", "page");
$handler->display->display_options["path"] = "' . $actual . '/apis";


/* Display: Block */
$handler = $view->new_display("block", "Block", "block_1");
$handler->display->display_options["defaults"]["title"] = FALSE;
$handler->display->display_options["title"] = "APIs";
$handler->display->display_options["defaults"]["pager"] = FALSE;
$handler->display->display_options["pager"]["type"] = "some";
$handler->display->display_options["pager"]["options"]["items_per_page"] = "5";
$handler->display->display_options["pager"]["options"]["offset"] = "0";
$handler->display->display_options["defaults"]["fields"] = FALSE;
/* Field: Content: Title */
$handler->display->display_options["fields"]["title"]["id"] = "title";
$handler->display->display_options["fields"]["title"]["table"] = "node";
$handler->display->display_options["fields"]["title"]["field"] = "title";
$handler->display->display_options["fields"]["title"]["label"] = "";
$handler->display->display_options["fields"]["title"]["exclude"] = TRUE;
$handler->display->display_options["fields"]["title"]["alter"]["word_boundary"] = FALSE;
$handler->display->display_options["fields"]["title"]["alter"]["ellipsis"] = FALSE;
$handler->display->display_options["fields"]["title"]["element_label_colon"] = FALSE;
/* Field: Content: authschemes */
$handler->display->display_options["fields"]["field_' . $mid . '_model_authschemes"]["id"] = "field_' . $mid . '_model_authschemes";
$handler->display->display_options["fields"]["field_' . $mid . '_model_authschemes"]["table"] = "field_data_field_' . $mid . '_model_authschemes";
$handler->display->display_options["fields"]["field_' . $mid . '_model_authschemes"]["field"] = "field_' . $mid . '_model_authschemes";
$handler->display->display_options["fields"]["field_' . $mid . '_model_authschemes"]["label"] = "";
$handler->display->display_options["fields"]["field_' . $mid . '_model_authschemes"]["exclude"] = TRUE;
$handler->display->display_options["fields"]["field_' . $mid . '_model_authschemes"]["alter"]["alter_text"] = TRUE;
$handler->display->display_options["fields"]["field_' . $mid . '_model_authschemes"]["alter"]["text"] = "<span title=\"This method needs authentication.\" class=\"icon_lock\"></span>";
$handler->display->display_options["fields"]["field_' . $mid . '_model_authschemes"]["element_label_colon"] = FALSE;
$handler->display->display_options["fields"]["field_' . $mid . '_model_authschemes"]["type"] = "taxonomy_term_reference_plain";
$handler->display->display_options["fields"]["field_' . $mid . '_model_authschemes"]["delta_offset"] = "0";
/* Field: Content: Body */
$handler->display->display_options["fields"]["body"]["id"] = "body";
$handler->display->display_options["fields"]["body"]["table"] = "field_data_body";
$handler->display->display_options["fields"]["body"]["field"] = "body";
$handler->display->display_options["fields"]["body"]["label"] = "";
$handler->display->display_options["fields"]["body"]["exclude"] = TRUE;
$handler->display->display_options["fields"]["body"]["element_label_colon"] = FALSE;
$handler->display->display_options["fields"]["body"]["type"] = "text_plain";
/* Field: Content: Path */
$handler->display->display_options["fields"]["field_' . $mid . '_model_path"]["id"] = "field_' . $mid . '_model_path";
$handler->display->display_options["fields"]["field_' . $mid . '_model_path"]["table"] = "field_data_field_' . $mid . '_model_path";
$handler->display->display_options["fields"]["field_' . $mid . '_model_path"]["field"] = "field_' . $mid . '_model_path";
$handler->display->display_options["fields"]["field_' . $mid . '_model_path"]["label"] = "";
$handler->display->display_options["fields"]["field_' . $mid . '_model_path"]["exclude"] = TRUE;
$handler->display->display_options["fields"]["field_' . $mid . '_model_path"]["element_label_colon"] = FALSE;
/* Field: Content: verb */
$handler->display->display_options["fields"]["field_' . $mid . '_model_verb"]["id"] = "field_' . $mid . '_model_verb";
$handler->display->display_options["fields"]["field_' . $mid . '_model_verb"]["table"] = "field_data_field_' . $mid . '_model_verb";
$handler->display->display_options["fields"]["field_' . $mid . '_model_verb"]["field"] = "field_' . $mid . '_model_verb";
$handler->display->display_options["fields"]["field_' . $mid . '_model_verb"]["label"] = "";
$handler->display->display_options["fields"]["field_' . $mid . '_model_verb"]["exclude"] = TRUE;
$handler->display->display_options["fields"]["field_' . $mid . '_model_verb"]["element_label_colon"] = FALSE;
$handler->display->display_options["fields"]["field_' . $mid . '_model_verb"]["type"] = "taxonomy_term_reference_plain";
/* Field: Global: Custom text */
$handler->display->display_options["fields"]["nothing"]["id"] = "nothing";
$handler->display->display_options["fields"]["nothing"]["table"] = "views";
$handler->display->display_options["fields"]["nothing"]["field"] = "nothing";
$handler->display->display_options["fields"]["nothing"]["label"] = "";
$handler->display->display_options["fields"]["nothing"]["alter"]["text"] = "<div class=\"method_details clearfix\">
                                <div class=\"method_data title\">
<div class=\"verb-auth\">
                                    <p data-role=\"verb\" class=\"[field_' . $mid . '_model_verb]\">[field_' . $mid . '_model_verb]</p>
[field_' . $mid . '_model_authschemes]
</div>
<div class=\"title-description\">
                                    [title]
<p title=\"[field_' . $mid . '_model_path]\" class=\"resource_path\" data-role=\"resource_path\">[field_' . $mid . '_model_path]</p>
</div>
                                </div>

                            </div>";
$handler->display->display_options["fields"]["nothing"]["element_label_colon"] = FALSE;


  ?>';
}
