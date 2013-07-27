<?php
/**
 * @file
 * devconnect_default_content.features.node_export_features.inc
 */

/**
 * Implements hook_node_export_features_default().
 */
function devconnect_default_content_node_export_features_default() {
  $node_export = array(
  'code_string' => 'array(
  (object) array(
      \'vid\' => \'960\',
      \'uid\' => \'2\',
      \'title\' => \'Terms of Use\',
      \'log\' => \'\',
      \'status\' => \'1\',
      \'comment\' => \'1\',
      \'promote\' => \'0\',
      \'sticky\' => \'0\',
      \'vuuid\' => \'db4a2210-1dd4-4cec-9813-73fc8dfcc401\',
      \'ds_switch\' => \'\',
      \'nid\' => \'955\',
      \'type\' => \'page\',
      \'language\' => \'und\',
      \'created\' => \'1369940243\',
      \'changed\' => \'1369940243\',
      \'tnid\' => \'0\',
      \'translate\' => \'0\',
      \'uuid\' => \'0360390f-8109-41f6-a5ff-6c3bda5b09fa\',
      \'revision_timestamp\' => \'1369940243\',
      \'revision_uid\' => \'2\',
      \'body\' => array(),
      \'metatags\' => array(),
      \'rdf_mapping\' => array(
        \'rdftype\' => array(
          \'foaf:Document\',
        ),
        \'title\' => array(
          \'predicates\' => array(
            \'dc:title\',
          ),
        ),
        \'created\' => array(
          \'predicates\' => array(
            \'dc:date\',
            \'dc:created\',
          ),
          \'datatype\' => \'xsd:dateTime\',
          \'callback\' => \'date_iso8601\',
        ),
        \'changed\' => array(
          \'predicates\' => array(
            \'dc:modified\',
          ),
          \'datatype\' => \'xsd:dateTime\',
          \'callback\' => \'date_iso8601\',
        ),
        \'body\' => array(
          \'predicates\' => array(
            \'content:encoded\',
          ),
        ),
        \'uid\' => array(
          \'predicates\' => array(
            \'sioc:has_creator\',
          ),
          \'type\' => \'rel\',
        ),
        \'name\' => array(
          \'predicates\' => array(
            \'foaf:name\',
          ),
        ),
        \'comment_count\' => array(
          \'predicates\' => array(
            \'sioc:num_replies\',
          ),
          \'datatype\' => \'xsd:integer\',
        ),
        \'last_activity\' => array(
          \'predicates\' => array(
            \'sioc:last_activity_date\',
          ),
          \'datatype\' => \'xsd:dateTime\',
          \'callback\' => \'date_iso8601\',
        ),
      ),
      \'cid\' => \'0\',
      \'last_comment_timestamp\' => \'1369940243\',
      \'last_comment_name\' => NULL,
      \'last_comment_uid\' => \'2\',
      \'comment_count\' => \'0\',
      \'name\' => \'morgan\',
      \'picture\' => \'0\',
      \'data\' => \'a:2:{s:8:"gravatar";i:1;s:7:"overlay";i:0;}\',
      \'path\' => array(
        \'pid\' => \'1125\',
        \'source\' => \'node/955\',
        \'alias\' => \'terms-use\',
        \'language\' => \'und\',
      ),
      \'menu\' => NULL,
      \'node_export_drupal_version\' => \'7\',
    ),
  (object) array(
      \'vid\' => \'54\',
      \'uid\' => \'1\',
      \'title\' => \'home\',
      \'log\' => \'\',
      \'status\' => \'1\',
      \'comment\' => \'1\',
      \'promote\' => \'0\',
      \'sticky\' => \'0\',
      \'vuuid\' => \'ba7e425e-5161-7214-79f6-065943709d9e\',
      \'ds_switch\' => \'\',
      \'nid\' => \'54\',
      \'type\' => \'page\',
      \'language\' => \'und\',
      \'created\' => \'1326919670\',
      \'changed\' => \'1374947534\',
      \'tnid\' => \'0\',
      \'translate\' => \'0\',
      \'uuid\' => \'499296b5-d2c1-9bd4-0952-ef99fdce9255\',
      \'revision_timestamp\' => \'1374947534\',
      \'revision_uid\' => \'186\',
      \'body\' => array(
        \'und\' => array(
          array(
            \'value\' => "\\t\\t<h2 style=\\"font-size: 3.6em; font-weight: normal; line-height: 1.2; color: #3c3c3c; font-family: helvetica, Arial, sans-serif;\\">\\r\\n\\t\\t\\tWelcome to Apigee-Drupal\\r\\n\\t\\t</h2>\\r\\n\\t\\t<h3 style=\\"font-size: 2.2em; font-weight: normal; line-height: 1.2; color: #ff4000; font-family: helvetica, Arial, sans-serif; text-align: left;\\">\\r\\n\\t\\t\\tProviding support for the API Development Community\\r\\n\\t\\t</h3>\\r\\n\\t\\t<p style=\\"padding-right: 275px; font-size: 1.2em;  line-height: 1.5; font-family: helvetica, Arial, sans-serif; text-align: left; border-width: 0px;\\">\\r\\n\\t\\t\\tNamasteCRM provides open API to access Namaste\'s product catalog, featuring full product description (with image), pricing and availability. One can also search for \'Discounts\' across range of product categories. These API can also be used to locate Acme stores in a town/city.\\r\\n\\t\\t</p>\\r\\n\\t\\t<p>\\r\\n\\t\\t\\t<a class=\\"btn primary\\" href=\\"/api-method/create-api-product-0\\">Learn More</a>\\r\\n\\t\\t</p>\\r\\n",
            \'summary\' => \'\',
            \'format\' => \'full_html\',
            \'safe_value\' => "<pre><code>    &lt;h2 style=\\"font-size: 3.6em; font-weight: normal; line-height: 1.2; color: #3c3c3c; font-family: helvetica, Arial, sans-serif;\\"&gt;\\n        Welcome to Apigee-Drupal\\n    &lt;/h2&gt;\\n    &lt;h3 style=\\"font-size: 2.2em; font-weight: normal; line-height: 1.2; color: #ff4000; font-family: helvetica, Arial, sans-serif; text-align: left;\\"&gt;\\n        Providing support for the API Development Community\\n    &lt;/h3&gt;\\n    &lt;p style=\\"padding-right: 275px; font-size: 1.2em;  line-height: 1.5; font-family: helvetica, Arial, sans-serif; text-align: left; border-width: 0px;\\"&gt;\\n        NamasteCRM provides open API to access Namaste\'s product catalog, featuring full product description (with image), pricing and availability. One can also search for \'Discounts\' across range of product categories. These API can also be used to locate Acme stores in a town/city.\\n    &lt;/p&gt;\\n    &lt;p&gt;\\n        &lt;a class=\\"btn primary\\" href=\\"/api-method/create-api-product-0\\"&gt;Learn More&lt;/a&gt;\\n    &lt;/p&gt;\\n</code></pre>",
            \'safe_summary\' => \'\',
          ),
        ),
      ),
      \'metatags\' => array(),
      \'rdf_mapping\' => array(
        \'rdftype\' => array(
          \'foaf:Document\',
        ),
        \'title\' => array(
          \'predicates\' => array(
            \'dc:title\',
          ),
        ),
        \'created\' => array(
          \'predicates\' => array(
            \'dc:date\',
            \'dc:created\',
          ),
          \'datatype\' => \'xsd:dateTime\',
          \'callback\' => \'date_iso8601\',
        ),
        \'changed\' => array(
          \'predicates\' => array(
            \'dc:modified\',
          ),
          \'datatype\' => \'xsd:dateTime\',
          \'callback\' => \'date_iso8601\',
        ),
        \'body\' => array(
          \'predicates\' => array(
            \'content:encoded\',
          ),
        ),
        \'uid\' => array(
          \'predicates\' => array(
            \'sioc:has_creator\',
          ),
          \'type\' => \'rel\',
        ),
        \'name\' => array(
          \'predicates\' => array(
            \'foaf:name\',
          ),
        ),
        \'comment_count\' => array(
          \'predicates\' => array(
            \'sioc:num_replies\',
          ),
          \'datatype\' => \'xsd:integer\',
        ),
        \'last_activity\' => array(
          \'predicates\' => array(
            \'sioc:last_activity_date\',
          ),
          \'datatype\' => \'xsd:dateTime\',
          \'callback\' => \'date_iso8601\',
        ),
      ),
      \'cid\' => \'0\',
      \'last_comment_timestamp\' => \'1326919670\',
      \'last_comment_name\' => NULL,
      \'last_comment_uid\' => \'1\',
      \'comment_count\' => \'0\',
      \'name\' => \'admin\',
      \'picture\' => \'0\',
      \'data\' => \'a:2:{s:8:"gravatar";i:1;s:7:"overlay";i:0;}\',
      \'path\' => array(
        \'pid\' => \'4\',
        \'source\' => \'node/54\',
        \'alias\' => \'home\',
        \'language\' => \'und\',
      ),
      \'menu\' => NULL,
      \'node_export_drupal_version\' => \'7\',
    ),
  (object) array(
      \'vid\' => \'959\',
      \'uid\' => \'1\',
      \'title\' => \'Documentation\',
      \'log\' => \'\',
      \'status\' => \'1\',
      \'comment\' => \'1\',
      \'promote\' => \'0\',
      \'sticky\' => \'0\',
      \'vuuid\' => \'bc23b69b-c23e-4acf-b379-515114dd7c2c\',
      \'ds_switch\' => \'\',
      \'nid\' => \'954\',
      \'type\' => \'page\',
      \'language\' => \'und\',
      \'created\' => \'1357663648\',
      \'changed\' => \'1357663664\',
      \'tnid\' => \'0\',
      \'translate\' => \'0\',
      \'uuid\' => \'65f54a9a-4a31-4118-91e3-e631e1ab6278\',
      \'revision_timestamp\' => \'1357663664\',
      \'revision_uid\' => \'1\',
      \'body\' => array(
        \'und\' => array(
          array(
            \'value\' => \'<div id="documentation"><div class="row"><div class="span12"><h2>Getting Started</h2><ul><li><a href="#">Overview</a></li><li><a href="#">Startup Guide</a></li></ul><h2>Tutorials</h2><ul><li><a href="#">Sample Tutorial</a></li></ul></div><div class="span12"><h2>API Reference</h2><ul><li><a href="#">Apigee API Product Reference</a></li></ul><h2>Exploring Apigee</h2><ul><li><a href="#">Sample API</a></li><li><a href="#">Introduction to the console</a></li></ul></div></div></div>\',
            \'summary\' => \'\',
            \'format\' => \'full_html\',
            \'safe_value\' => "<div id=\\"documentation\\">\\n<div class=\\"row\\">\\n<div class=\\"span12\\">\\n<h2>Getting Started</h2>\\n<ul><li><a href=\\"#\\">Overview</a></li>\\n<li><a href=\\"#\\">Startup Guide</a></li>\\n</ul><h2>Tutorials</h2>\\n<ul><li><a href=\\"#\\">Sample Tutorial</a></li>\\n</ul></div>\\n<div class=\\"span12\\">\\n<h2>API Reference</h2>\\n<ul><li><a href=\\"#\\">Apigee API Product Reference</a></li>\\n</ul><h2>Exploring Apigee</h2>\\n<ul><li><a href=\\"#\\">Sample API</a></li>\\n<li><a href=\\"#\\">Introduction to the console</a></li>\\n</ul></div>\\n</div>\\n</div>\\n",
            \'safe_summary\' => \'\',
          ),
        ),
      ),
      \'metatags\' => array(),
      \'rdf_mapping\' => array(
        \'rdftype\' => array(
          \'foaf:Document\',
        ),
        \'title\' => array(
          \'predicates\' => array(
            \'dc:title\',
          ),
        ),
        \'created\' => array(
          \'predicates\' => array(
            \'dc:date\',
            \'dc:created\',
          ),
          \'datatype\' => \'xsd:dateTime\',
          \'callback\' => \'date_iso8601\',
        ),
        \'changed\' => array(
          \'predicates\' => array(
            \'dc:modified\',
          ),
          \'datatype\' => \'xsd:dateTime\',
          \'callback\' => \'date_iso8601\',
        ),
        \'body\' => array(
          \'predicates\' => array(
            \'content:encoded\',
          ),
        ),
        \'uid\' => array(
          \'predicates\' => array(
            \'sioc:has_creator\',
          ),
          \'type\' => \'rel\',
        ),
        \'name\' => array(
          \'predicates\' => array(
            \'foaf:name\',
          ),
        ),
        \'comment_count\' => array(
          \'predicates\' => array(
            \'sioc:num_replies\',
          ),
          \'datatype\' => \'xsd:integer\',
        ),
        \'last_activity\' => array(
          \'predicates\' => array(
            \'sioc:last_activity_date\',
          ),
          \'datatype\' => \'xsd:dateTime\',
          \'callback\' => \'date_iso8601\',
        ),
      ),
      \'cid\' => \'0\',
      \'last_comment_timestamp\' => \'1357663648\',
      \'last_comment_name\' => NULL,
      \'last_comment_uid\' => \'1\',
      \'comment_count\' => \'0\',
      \'name\' => \'admin\',
      \'picture\' => \'0\',
      \'data\' => \'a:2:{s:8:"gravatar";i:1;s:7:"overlay";i:0;}\',
      \'path\' => array(
        \'pid\' => \'1109\',
        \'source\' => \'node/954\',
        \'alias\' => \'documentation\',
        \'language\' => \'und\',
      ),
      \'menu\' => array(
        \'link_title\' => \'Documentation\',
        \'mlid\' => 0,
        \'plid\' => \'0\',
        \'menu_name\' => \'main-menu\',
        \'weight\' => \'-50\',
        \'options\' => array(),
        \'module\' => \'menu\',
        \'expanded\' => \'0\',
        \'hidden\' => \'0\',
        \'has_children\' => \'0\',
        \'customized\' => 0,
        \'parent_depth_limit\' => 8,
        \'description\' => \'\',
        \'enabled\' => 1,
      ),
      \'node_export_drupal_version\' => \'7\',
    ),
  (object) array(
      \'vid\' => \'3\',
      \'uid\' => \'1\',
      \'title\' => \'API Console\',
      \'log\' => \'\',
      \'status\' => \'1\',
      \'comment\' => \'1\',
      \'promote\' => \'0\',
      \'sticky\' => \'0\',
      \'vuuid\' => \'8bd6e639-fbb8-6db4-8d12-c78e6a444e25\',
      \'ds_switch\' => \'\',
      \'nid\' => \'3\',
      \'type\' => \'page\',
      \'language\' => \'und\',
      \'created\' => \'1326917239\',
      \'changed\' => \'1349798324\',
      \'tnid\' => \'0\',
      \'translate\' => \'0\',
      \'uuid\' => \'978be690-09a8-fae4-71aa-3f8b230a83c0\',
      \'revision_timestamp\' => \'1349798324\',
      \'revision_uid\' => \'138\',
      \'body\' => array(
        \'und\' => array(
          array(
            \'value\' => \'<iframe src="https://apigee.com/enterprisestickers/embed/console/estickers?v=2" width="100%" height="600" scrolling="no"></iframe>\',
            \'summary\' => \'\',
            \'format\' => \'full_html\',
            \'safe_value\' => \'<iframe src="https://apigee.com/enterprisestickers/embed/console/estickers?v=2" width="100%" height="600" scrolling="no"></iframe>\',
            \'safe_summary\' => \'\',
          ),
        ),
      ),
      \'metatags\' => array(),
      \'rdf_mapping\' => array(
        \'rdftype\' => array(
          \'foaf:Document\',
        ),
        \'title\' => array(
          \'predicates\' => array(
            \'dc:title\',
          ),
        ),
        \'created\' => array(
          \'predicates\' => array(
            \'dc:date\',
            \'dc:created\',
          ),
          \'datatype\' => \'xsd:dateTime\',
          \'callback\' => \'date_iso8601\',
        ),
        \'changed\' => array(
          \'predicates\' => array(
            \'dc:modified\',
          ),
          \'datatype\' => \'xsd:dateTime\',
          \'callback\' => \'date_iso8601\',
        ),
        \'body\' => array(
          \'predicates\' => array(
            \'content:encoded\',
          ),
        ),
        \'uid\' => array(
          \'predicates\' => array(
            \'sioc:has_creator\',
          ),
          \'type\' => \'rel\',
        ),
        \'name\' => array(
          \'predicates\' => array(
            \'foaf:name\',
          ),
        ),
        \'comment_count\' => array(
          \'predicates\' => array(
            \'sioc:num_replies\',
          ),
          \'datatype\' => \'xsd:integer\',
        ),
        \'last_activity\' => array(
          \'predicates\' => array(
            \'sioc:last_activity_date\',
          ),
          \'datatype\' => \'xsd:dateTime\',
          \'callback\' => \'date_iso8601\',
        ),
      ),
      \'cid\' => \'0\',
      \'last_comment_timestamp\' => \'1326917239\',
      \'last_comment_name\' => NULL,
      \'last_comment_uid\' => \'1\',
      \'comment_count\' => \'0\',
      \'name\' => \'admin\',
      \'picture\' => \'0\',
      \'data\' => \'a:2:{s:8:"gravatar";i:1;s:7:"overlay";i:0;}\',
      \'path\' => array(
        \'pid\' => \'3\',
        \'source\' => \'node/3\',
        \'alias\' => \'api-console\',
        \'language\' => \'und\',
      ),
      \'menu\' => array(
        \'link_title\' => \'Console\',
        \'mlid\' => 0,
        \'plid\' => \'0\',
        \'menu_name\' => \'main-menu\',
        \'weight\' => \'-49\',
        \'options\' => array(),
        \'module\' => \'menu\',
        \'expanded\' => \'0\',
        \'hidden\' => \'0\',
        \'has_children\' => \'0\',
        \'customized\' => 0,
        \'parent_depth_limit\' => 8,
        \'description\' => \'\',
        \'enabled\' => 1,
      ),
      \'node_export_drupal_version\' => \'7\',
    ),
  (object) array(
      \'vid\' => \'188\',
      \'uid\' => \'2\',
      \'title\' => \'Getting Started With Developer Connect Blogs\',
      \'log\' => \'\',
      \'status\' => \'1\',
      \'comment\' => \'2\',
      \'promote\' => \'1\',
      \'sticky\' => \'0\',
      \'vuuid\' => \'10e283fe-440f-9e04-612d-371f0a5d8b01\',
      \'ds_switch\' => \'\',
      \'nid\' => \'188\',
      \'type\' => \'blog\',
      \'language\' => \'und\',
      \'created\' => \'1329251455\',
      \'changed\' => \'1341006303\',
      \'tnid\' => \'0\',
      \'translate\' => \'0\',
      \'uuid\' => \'add3cc14-a0b0-8784-0d84-08877973f6d5\',
      \'revision_timestamp\' => \'1341006303\',
      \'revision_uid\' => \'88\',
      \'body\' => array(
        \'und\' => array(
          array(
            \'value\' => "__Blogs__ are everywhere.  Use this as a vehicle to communicate with your development community.  \\r\\n\\r\\nPost information about current and upcoming releases, events, roadmaps, etc...\\r\\n\\r\\nEncourage Developer Participation",
            \'summary\' => \'\',
            \'format\' => \'markdown\',
            \'safe_value\' => "<p><strong>Blogs</strong> are everywhere.  Use this as a vehicle to communicate with your development community.</p>\\n\\n<p>Post information about current and upcoming releases, events, roadmaps, etc...</p>\\n\\n<p>Encourage Developer Participation</p>\\n",
            \'safe_summary\' => \'\',
          ),
        ),
      ),
      \'field_blog_tags\' => array(
        \'und\' => array(
          array(
            \'tid\' => \'11\',
            \'uuid\' => \'af4c7392-7889-9574-8126-cbd38c6dba87\',
          ),
          array(
            \'tid\' => \'13\',
            \'uuid\' => \'71694538-638d-acc4-9158-fe3e6d0c84f3\',
          ),
          array(
            \'tid\' => \'14\',
            \'uuid\' => \'7bcf2a47-2369-06a4-7525-c2600d6703f3\',
          ),
        ),
      ),
      \'metatags\' => array(),
      \'rdf_mapping\' => array(
        \'rdftype\' => array(
          \'sioc:Post\',
          \'sioct:BlogPost\',
        ),
        \'title\' => array(
          \'predicates\' => array(
            \'dc:title\',
          ),
        ),
        \'created\' => array(
          \'predicates\' => array(
            \'dc:date\',
            \'dc:created\',
          ),
          \'datatype\' => \'xsd:dateTime\',
          \'callback\' => \'date_iso8601\',
        ),
        \'changed\' => array(
          \'predicates\' => array(
            \'dc:modified\',
          ),
          \'datatype\' => \'xsd:dateTime\',
          \'callback\' => \'date_iso8601\',
        ),
        \'body\' => array(
          \'predicates\' => array(
            \'content:encoded\',
          ),
        ),
        \'uid\' => array(
          \'predicates\' => array(
            \'sioc:has_creator\',
          ),
          \'type\' => \'rel\',
        ),
        \'name\' => array(
          \'predicates\' => array(
            \'foaf:name\',
          ),
        ),
        \'comment_count\' => array(
          \'predicates\' => array(
            \'sioc:num_replies\',
          ),
          \'datatype\' => \'xsd:integer\',
        ),
        \'last_activity\' => array(
          \'predicates\' => array(
            \'sioc:last_activity_date\',
          ),
          \'datatype\' => \'xsd:dateTime\',
          \'callback\' => \'date_iso8601\',
        ),
      ),
      \'cid\' => \'0\',
      \'last_comment_timestamp\' => \'1332448255\',
      \'last_comment_name\' => NULL,
      \'last_comment_uid\' => \'2\',
      \'comment_count\' => \'0\',
      \'name\' => \'morgan\',
      \'picture\' => \'0\',
      \'data\' => \'a:2:{s:8:"gravatar";i:1;s:7:"overlay";i:0;}\',
      \'path\' => array(
        \'pid\' => \'165\',
        \'source\' => \'node/188\',
        \'alias\' => \'blog-entry/getting-started-developer-connect-blogs\',
        \'language\' => \'und\',
      ),
      \'menu\' => NULL,
      \'node_export_drupal_version\' => \'7\',
    ),
)',
);
  return $node_export;
}
