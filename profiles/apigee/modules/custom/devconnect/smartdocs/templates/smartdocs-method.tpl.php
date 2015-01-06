<?php
/*
 * Required as passed via preprocess function
 */
extract($variables);
$type = $variables['type'];
$path = 'field_' . smartdocs_field_name($type) . '_model_path';
$path = $node->$path;
$path = $path[LANGUAGE_NONE][0]['value'];
$verb = 'field_' . smartdocs_field_name($type) . '_model_verb';
$verb = $node->$verb;
$verb = taxonomy_term_load($verb[LANGUAGE_NONE][0]['tid']);
$verb = $verb->name;

// Multiples need to be rendered
$auth = render($content['field_' . smartdocs_field_name($type) . '_model_authschemes']);
$types = render($content['field_' . smartdocs_field_name($type) . '_model_types']);
$tags = render($content['field_' . smartdocs_field_name($type) . '_model_tags']);

$tags = preg_replace('#<div class="field-label">(.*?)</div>#', "", $tags);
$auth = preg_replace('#<div class="field-label">(.*?)</div>#', "", $auth);
$types = preg_replace('#<div class="field-label">(.*?)</div>#', "", $types);

// TODO: determine if $teaser is ever true.

// TODO: move these inline stylesheets to a proper drupal_add_css invocation.
?>
<?php if (!$teaser): ?>
  <link href="https://apigee.com/git/docs/css/main.css" rel="stylesheet" type="text/css" media="screen" charset="utf-8"/>
  <link href="https://apigee.com/git/docs/css/cms.css" rel="stylesheet" type="text/css" media="screen" charset="utf-8"/>
  <style type="text/css">
    #container #content {
      display:block;
    }
  </style><?php // TODO: the following two divs are unclosed! ?>
  <div id="container">
  <div id="content">
  <div>
    <div class="resource_summary">
      <h4>Resource Summary</h4>
      <?php if ($schemes): ?>
        <div><p class="title">Auth Type</p><p class="data auth_type" data-role="auth-type"><?php print $schemes; ?></p></div>
      <?php endif; // $schemes ?>
      <?php if ($types): ?>
        <div><p class="title">Content Types</p><p class="data" data-role="content-type"><?php print $types; ?></p></div>
      <?php endif; // $types ?>
      <?php if ($terms): ?>
        <div><p class="title">Category</p><p class="data" data-role="content-type"><?php print $terms; ?></p></div>
      <?php endif; // $terms ?>
      <?php if ($date): ?>
        <div><p class="title">Updated</p><p class="data" data-role="modified-time"><?php print $date; ?></p></div>
      <?php endif; // $date ?>
    </div><?php // end resource_summary ?>
    <div class="resource_details">
      <span id="method_name" data-role="method-name" class="hide"><?php print $title; ?></span>
      <div>
        <div class="verb_container">
          <?php if ($verb): ?>
            <p class="<?php print $verb; ?>" data-role="verb"><?php print $verb; ?></p>
          <?php endif; // $verb ?>
          <?php if ($schemes): ?>
            <span class="icon_lock"></span>
          <?php endif; // $schemes ?>
        </div><?php // end verb_container ?>
        <h2 data-role="method-title" data-allow-edit="true"><?php print $title; ?></h2>
      </div>
      <div class="description_and_url_container">
        <div class="description_container">
          <?php if ($description): ?>
            <div class="resource_description" data-allow-edit="true" data-role="method-description"><?php print $description; ?></div>
          <?php endif; // $description ?>
        </div><?php // end description_container ?>
        <?php if ($path): ?>
          <h3>Resource URL</h3>
          <div class="url_container">
            <p id="resource_URL" data-role="method-url"><span data-role="host"><?php print $path; ?></span></p>
          </div>
        <?php endif; // $path ?>
      </div><?php // end description_and_url_container ?>
    </div><?php // end resource_details ?>
  </div>
<?php else: // $teaser ?>
  <div class="row method_details clearfix">
    <div class="col-sm-4">
      <div class="row method_data title">
        <div class="col-sm-2">
          <div class="verb-auth">
            <p class="<?php print $verb; ?>" data-role="<?php print $verb; ?>"><?php print $verb; ?></p>
            <span class="icon_lock" title="This method needs authentication."></span>
          </div>
        </div>
        <div class="col-sm-10">
          <div class="title-description">
            <?php print l($node->title, 'node/' . $node->nid); ?>
            <p data-role="resource_path" class="resource_path" title="<?php print $path; ?>">
              <?php print $path; ?>
            </p>
          </div>
        </div>
      </div>
    </div>
    <div class="col-sm-8">
      <div class="method_data description">
        <p><?php print $content['body']['#items'][0]['value']; ?></p>
      </div>
    </div>
  </div>
<?php endif; ?>