<?php


/**
 * Standard Template for Nodes after import
 */
function _devconnect_docgen_generate_default_node($mid) {
  return '<?php

$verb = "";
if (isset($content["field_' . $mid . '_model_verb"][0]["#title"])) {
  $verb = $content["field_' . $mid . '_model_verb"][0]["#title"];
}
$params = "";
if (isset($content["field_' . $mid . '_model_requestjson"][0]["#markup"])
  && is_string($content["field_' . $mid . '_model_requestjson"][0]["#markup"])) {
  $params = $content["field_' . $mid . '_model_requestjson"][0]["#markup"];
}
$path = "";
if (isset($content["field_' . $mid . '_model_path"][0]["#markup"])) {
  $path = $content["field_' . $mid . '_model_path"][0]["#markup"];
}
$auth_valid = FALSE;
if (isset($content["field_' . $mid . '_model_authschemes"][0]["#title"])) {
  $auth_valid = TRUE;
  $auth = render($content["field_' . $mid . '_model_authschemes"]);
  $auth = preg_replace(\'#<div class="field-label">(.*?)</div>#\', "", $auth);
} else {
  $auth = FALSE;
}
$body = "";
if (isset($content["body"][0]["#markup"])) {
  $body = $content["body"][0]["#markup"];
}
$tags = "";
if (isset($content["field_' . $mid . '_model_tags"])) {
  $tags = render($content["field_' . $mid . '_model_tags"]);
  $tags = preg_replace(\'#<div class="field-label">(.*?)</div>#\', "", $tags);
}

?>
<?php if (!$teaser) { ?>
<div class="row">
  <div class="span18">
    <div class="row">
      <div class="span3">
        <p data-role="verb" class="<?php print $verb; ?>"><?php print $verb; ?></p>
        <?php if ($auth_valid) { ?>
        <span class="icon_lock" data-original-title="" title=""></span>
        <?php } ?>
      </div>
      <div class="span15 pull-right">
        <?php print $body; ?>
      </div>
      <br/>
      <br/>
      <br/>
      <div class="span15 pull-right">
        <?php print $params; ?>
      </div>
    </div>
  </div>
  <div class="span5 well">
    <div class="row resource_summary">
      <h4>Resource Summary</h4>
      <div class="row">
        <div class="span2"><p class="title">Auth Type:</p></div>
        <div class="span3"><div class="pull-right"><?php print $auth; ?></div></div>
      </div>
      <div class="row">
        <div class="span2"><p class="title">Category:</p></div>
        <div class="span3"><div class="pull-right"><?php print $tags; ?></div></div>
      </div>
      <div class="row">
        <div class="span2"><p class="title">Updated:</p></div>
        <div class="span3"><div class="pull-right"><?php print date("F j, Y", $node->changed); ?></div></div>
      </div>
    </div>
  </div>
</div>
<?php } else {

  if (isset($node->field_' . $mid . '_model_path[LANGUAGE_NONE][0]["value"])) {
    $path = $node->field_' . $mid . '_model_path[LANGUAGE_NONE][0]["value"];
  } else {
    $path = "";
  }

  if (isset($node->field_' . $mid . '_model_verb[LANGUAGE_NONE][0]["tid"])) {
    $tid = $node->field_' . $mid . '_model_verb[LANGUAGE_NONE][0]["tid"];
    $term = taxonomy_term_load($tid);
    $verb = $term->name;
  } else {
    $verb = "";
  }

  ?>
<div class="row">
  <div class="method_details clearfix">
    <div class="method_data title">
      <div class="verb-auth">
        <p class="<?php print $verb; ?>" data-role="<?php print $verb; ?>"><?php print $verb; ?></p>
        <span class="icon_lock" title="This method needs authentication."></span>
      </div>
      <div class="title-description">
        <?php print l($title, "node/" . $node->nid); ?>
        <p data-role="resource_path" class="resource_path" title="<?php print $path; ?>">
          <?php print $path; ?>
        </p>
      </div>
    </div>
    <div class="method_data description">
      <?php print $node->body[LANGUAGE_NONE][0]["value"] ?>
    </div>
  </div>
</div>
<?php } ?>';
}