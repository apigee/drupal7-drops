<?php

/**
 * @file example of how to deploy content with import_html as a feature.
 *
 * Implements hook_import_html_content_defaults().
 *
 * Provide some basic configurations to use as import preferences, and a list of
 * files or directories relative to the source_siteroot.
 *
 * These paths will be imported when the feature is enabled.
 *
 * Deleting imported content will trigger a feature rebuild, but updating them
 * will not.
 *
 * Deleting content found within folders (recursed) will not be noticed, only
 * the existence of the named files will be checked for.
 *
 * Any profile settings can be added here as overrides to the named profile_id.
 *
 * If you want to trigger a larger job, set
 *   $profile['recursion_method'] = 'import_html_batch';
 *
 */
function sample_content_import_html_content_defaults() {
  $settings = array();

  // You can import directly from remote URLs
  $settings['drupal_project'] = array(
    'name' => 'Import the import_html homepage directly from drupal.org',
    'source_siteroot' => 'http://drupal.org/',
    'profile_id' => 'default',
    'content_tag_id' => 'node-content',
    'paths' => array(
      '0' => 'project/import_html',
      '1' => 'files/images/import_html-logo.thumbnail.png',
    ),
    'site_root' => 'http://drupal.org/',
  );

  // You can define more than one section of content to import
  // This selects a file and two folders that were distributed as part of the
  // import_html 'tests' folder. But not all of it.
  $settings['sample_content_data'] = array(
    'name' => 'Import some content from the sample directory',
    'source_siteroot' => '!module/../../tests/example_pages/',
    'profile_id' => 'sample_content_profile',
    'paths' => array(
      '0' => 'input_example_01.htm',
      '1' => 'example_subfolder',
      '2' => 'images',
    ),
    #'recursion_method' => 'import_html_batch',
  );


  // A normal way to deploy content would be to provide the nodes in the feature itself.
  $settings['sample_feature_data'] = array(
    'name' => 'Import some pages deployed with the feature',
    'source_siteroot' => '!module/html/',
    'profile_id' => 'default',
    'content_tag_id' => 'content',
    // import_site_prefix, if set, should usually end with a /,
    // though it's not required if you want to concatenate filenames.
    'import_site_prefix' => 'sample_feature_data/',
    'paths' => array(
      'imported_feature_content' => 'index.htm',
      'imported_feature_content/page1' => 'page1.htm',
    ),
  );
  return $settings;
}

/**
 * Implements hook_import_html_profile_defaults().
 */
function sample_content_import_html_profile_defaults() {
  $import_html_profiles = array();

  // Exported setting (only the non-default values): sample_content_profile
  $import_html_profiles['sample_content_profile'] = array(
    'profile_id' => 'sample_content_profile',
    'translation_template' => '!module/sample2simplehtml.xsl',
    'content_tag_id' => 'main',
  );

  return $import_html_profiles;
}
