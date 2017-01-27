<?php
/**
 * @file
 * Contains functions related to generation of default content.
 */

/**
 * Installs the default homepage.
 *
 * @param array $context
 *   Current state of installer.
 */
function apigee_install_create_homepage(array &$context) {
  watchdog('apigee_install', 'Generating homepage', array(), WATCHDOG_INFO);
  $homepage = (object) array(
    'title' => 'home',
    'body' => array(),
    'type' => 'page',
    'status' => TRUE,
    'comment' => FALSE,
    'promote' => FALSE,
    'sticky' => FALSE,
  );
  try {
    node_save($homepage);
    variable_set("site_frontpage", "node/{$homepage->nid}");
    $context['results'][] = 'homepage_created';
    $context['message'] = st('Default Homepage Generated!');
  }
  catch (Exception $e) {
    watchdog_exception('apigee_install', $e, 'Error generating home page: %message', array('%message' => $e->getMessage()), WATCHDOG_ERROR);
    $context['results'][] = 'homepage_created';
    $context['message'] = st('No need to generate default homepage...');
  }
}

/**
 * Creates dummy taxonomy terms.
 *
 * @param array $context
 *   Current state of installer.
 */
function apigee_install_create_taxonomy_terms(array &$context) {
  for ($i = 0; $i <= 5; $i++) {
    $term = new stdClass();
    $term->name = _apigee_install_generate_greek(1, TRUE);
    $term->vid = taxonomy_vocabulary_machine_name_load('forums')->vid;
    taxonomy_term_save($term);
  }
  $context['results'][] = 'content_created';
  $context['message'] = st('Example taxonomy terms created');
}

/**
 * Creates example tutorial content.
 *
 * @param array $context
 *   Current state of installer.
 */
function apigee_install_create_tutorial_content(array &$context) {
  $startup_guide_body = '<p>The&nbsp;<em>developer portal</em>&nbsp;is a template portal, designed as a base that you can easily' .
    ' customize to meet your specific requirements</p>' .
    '<p>Your customized developer portal should educate developers about your API—what it is and how it’s used. It' .
    ' should also enable you to manage developer use of your API. This could include authorizing developers to use your API,' .
    ' giving developers an easy way to create apps that use your API products, assigning developers specific roles and permissions' .
    ' related to the API, or revoking developer access to the API as necessary. Beyond that, your developer portal can serve' .
    ' as the focal point for community activity, where developers can contribute API-related content to social media repositories' .
    ' such as blogs and forums.</p>' .
    ' <p>View more information about developer portals at ' .
    l(t('apigee.com'), 'http://apigee.com/docs/developer-channel/content/what-developer-portal') . '</p>';

  $customizing_portal_body = '<p>You can customize the appearance of the developer portal to match your company theme, to add new content areas' .
    ' to the portal, or to change the layout of any page on the portal. Much of this configuration requires a working knowledge' .
    ' of ' . l(t('Drupal'), 'https://drupal.org') . '. However, there is documentation that describes some of the basic tasks that you might want to' .
    ' perform to customize your portal.</p>' .
    ' <p>View more information about customizing your developer portals at ' .
    l(t('www.apigee.com'), 'http://apigee.com/docs/developer-channel/content/customize-appearance') . '.</p>';

  $posts = array(
    array(
      'title' => 'Portal Start Up Guide',
      'body' => $startup_guide_body,
      'keyword' => 'Portal',
    ),
    array(
      'title' => 'Customizing your portal',
      'body' => $customizing_portal_body,
      'keyword' => 'Tutorials',
    ),
  );
  foreach ($posts as $post) {
    $body = array();
    $body['post'] = $post['body'];
    $body['title'] = $post['title'];
    $fields = array();
    $fields['type'] = 'tutorial';
    $fields['keyword'] = $post['keyword'];
    $fields['vid'] = taxonomy_vocabulary_machine_name_load('blog')->vid;
    _apigee_install_generate_node('article', $body, $fields);
  }
  $context['results'][] = 'content_created';
  $context['message'] = st('Tutorial content generated');
}

/**
 * Creates default forum content for the install.
 *
 * @param array $context
 *   Current state of installer.
 */
function apigee_install_create_forum_content(array &$context) {
  // Create 10 forum posts.
  for ($i = 0; $i <= 7; $i++) {
    $body = array();
    $body['title'] = _apigee_install_generate_greek(mt_rand(3, 10));
    $body['post'] = _apigee_install_generate_greek(mt_rand(2, 300), TRUE);
    $fields = array();
    $fields['type'] = 'forum';
    $fields['keyword'] = _apigee_install_generate_greek(mt_rand(2, 5), TRUE);
    $fields['vid'] = taxonomy_vocabulary_machine_name_load('forums')->vid;
    _apigee_install_generate_node('forum', $body, $fields);
  }
  user_role_grant_permissions(DRUPAL_AUTHENTICATED_RID, array('create forum content', 'edit own forum content'));
  $context['results'][] = 'content_created';
  $context['message'] = st('10 example forum posts created');
}

/**
 * Creates default page content for the install.
 *
 * @param array $context
 *   Current state of installer.
 */
function apigee_install_create_page_content(array &$context) {
  // Create five pages.
  for ($i = 0; $i <= 5; $i++) {
    $body = array();
    $body['title'] = _apigee_install_generate_greek(mt_rand(3, 10));
    $body['post'] = _apigee_install_generate_greek(mt_rand(2, 300), TRUE);
    _apigee_install_generate_node('page', $body);
  }
  $context['results'][] = 'content_created';
  $context['message'] = st('5 example pages created!');
}

/**
 * Creates default FAQ content for the install.
 *
 * @param array $context
 *   Current state of installer.
 */
function apigee_install_create_faq_content(array &$context) {
  for ($i = 0; $i <= 3; $i++) {
    _apigee_install_generate_node('faq');
  }
  $context['results'][] = 'content_created';
  $context['message'] = st('FAQ example content created');
}

/**
 * Helper function that sets the file usage.
 *
 * @param int $fid
 *   The file identifier.
 * @param int $nid
 *   The node identifier.
 */
function _apigee_content_types_set_file_usage($fid, $nid) {
  $fid_exists = db_select('file_usage', 'fu')
    ->fields('fu', array('fid'))
    ->condition('type', 'node')
    ->condition('id', $nid)
    ->condition('fid', $fid)
    ->execute()
    ->fetchField();
  if (!$fid_exists) {
    db_insert('file_usage')
      ->fields(array(
        'fid' => $fid,
        'module' => 'file',
        'type' => 'node',
        'id' => $nid,
        'count' => 1,
      ))
      ->execute();
  }
}

/**
 * Helper function to find (or create) an entry in the file_managed table.
 *
 * @param string $uri
 *   URI of the managed file.
 * @param string $filename
 *   Filename of the managed file.
 * @param string $filemime
 *   MIME type of the managed file.
 * @param string $type
 *   Content-type to which file is attached.
 *
 * @return int
 *   File ID of the managed file.
 */
function _apigee_content_types_get_fid($uri, $filename, $filemime, $type) {
  $fid = db_select('file_managed', 'fm')
    ->fields('fm', array('fid'))
    ->condition('uri', $uri)
    ->execute()
    ->fetchField();
  if (!$fid) {
    $file = (object) array(
      'uid' => 0,
      'filename' => $filename,
      'uri' => $uri,
      'filemime' => $filemime,
      'status' => 1,
      'type' => $type,
      'alt' => '',
      'title' => '',
    );
    $file = file_save($file);
    $fid = $file->fid;
  }
  return $fid;
}

/**
 * Gets the tid for the 'blog' term in the 'content_type_tag' vocabulary.
 *
 * @param int|null $blog_tid
 *   If $blog_tid is passed in, it is set as the tid for future use.
 *
 * @return int
 *   The tid of the blog term.
 *
 * @TODO Why aren't we using taxonomy_get_term_by_name()?
 */
function _apigee_get_blog_tid($blog_tid = NULL) {
  static $tid;
  if (isset($blog_tid)) {
    $tid = $blog_tid;
  }
  if (!isset($tid)) {
    $query = db_select('taxonomy_vocabulary', 'v');
    $query->innerJoin('taxonomy_term_data', 't', 'v.vid = t.vid');
    $tid = $query->condition('v.machine_name', 'content_type_tag')
      ->condition('t.name', 'blog')
      ->fields('t', array('tid'))
      ->execute()
      ->fetchField();
  }
  return $tid;
}

/**
 * Helper function to generate a node.
 *
 * @param string $type
 *   Node content-type to be created.
 * @param string|null $body
 *   Body of the node.
 * @param null|array $fields
 *   Abbreviated descriptors of fields to be created on the node.
 */
function _apigee_install_generate_node($type, $body = NULL, $fields = NULL) {
  $node = new stdClass();
  $node->nid = NULL;
  $node->type = $type;
  $users = array();

  $result = db_select('users', 'u')
    ->fields('u', array('uid'))
    ->range(0, 50)
    ->execute();
  foreach ($result as $record) {
    $users[] = $record->uid;
  }
  $users = array_merge($users, array('0'));
  node_object_prepare($node);
  $node->uid = $users[array_rand($users)];
  $node->title = (isset($body['title'])) ? $body['title'] : _apigee_install_generate_greek(mt_rand(2, 7), TRUE);
  $node->language = LANGUAGE_NONE;
  if (isset($body['post'])) {
    $node->body[LANGUAGE_NONE][0]['value'] = $body['post'];
    $node->body[LANGUAGE_NONE][0]['summary'] = $body['post'];
    $node->body[LANGUAGE_NONE][0]['format'] = 'filtered_html';
  }
  else {
    $nparas = mt_rand(1, 12);
    $output = '';
    for ($i = 1; $i <= $nparas; $i++) {
      $output .= "<p>" . _apigee_install_generate_greek(mt_rand(10, 60)) . "</p>" . "\n\n";
    }
    $node->body[LANGUAGE_NONE][0]['value'] = $output;
    $node->body[LANGUAGE_NONE][0]['summary'] = $output;
    $node->body[LANGUAGE_NONE][0]['format'] = 'filtered_html';
  }
  $node->comment = 1;
  $node->status = 1;
  $node->created = REQUEST_TIME - mt_rand(0, 604800);
  if (!is_null($fields)) {
    switch ($fields['type']) {
      case 'tutorial':
        if (isset($fields['vid'])) {
          $vid = $fields['vid'];
          $term = taxonomy_get_term_by_name($fields['keyword'], 'blog');
          if (empty($term)) {
            if ($vid) {
              taxonomy_term_save((object) array(
                'name' => $fields['keyword'],
                'vid' => $vid,
              ));
              $keyword = taxonomy_get_term_by_name($fields['keyword'], 'blog');
            }
          }
          else {
            if ($vid) {
              $keyword = taxonomy_get_term_by_name($fields['keyword'], 'blog');
            }
          }
          if (isset($keyword)) {
            foreach ($keyword as $obj) {
              $node->field_keywords[LANGUAGE_NONE][]['tid'] = $obj->tid;
            }
          }
          foreach (taxonomy_get_term_by_name('blog', 'content_type_tag') as $obj) {
            $node->field_content_tag[LANGUAGE_NONE][]['tid'] = $obj->tid;
          }
        }
        break;

      case 'forum':
        if (isset($fields['vid'])) {
          $node->comment = 2;
          $rand = array_rand(taxonomy_get_tree($fields['vid']));
          $tree = taxonomy_get_tree($fields['vid']);
          $node->taxonomy_forums[LANGUAGE_NONE][]['tid'] = $tree[$rand]->tid;
        }
        break;

      default:
        break;
    }
  }
  if ($node->type == 'faq') {
    $node->detailed_question = $node->title;
  }
  node_save($node);
}

/**
 * Generates filler content for generated nodes.
 *
 * Mimics Devel's devel_creating_greeking, but makes this profile not rely on
 * that module.
 *
 * @param int $word_count
 *   How many words are to be created?
 * @param bool $title
 *   Are we creating a node title rather than node body?
 *
 * @return string
 *   String of greeked "words".
 */
function _apigee_install_generate_greek($word_count, $title = FALSE) {
  static $greek_flipped = NULL;
  if (!isset($greek_flipped)) {
    $greek = file(dirname(__FILE__) . '/greek.txt', FILE_IGNORE_NEW_LINES | FILE_SKIP_EMPTY_LINES);
    $greek_flipped = array_flip($greek);
  }
  if ($word_count > count($greek_flipped)) {
    $word_count = count($greek_flipped);
  }

  $greek_text = '';
  if (!$title) {
    $words_remaining = $word_count;
    while ($words_remaining > 0) {
      $sentence_length = mt_rand(3, 10);
      $words = array_rand($greek_flipped, $sentence_length);
      $sentence = implode(' ', $words);
      $greek_text .= ucfirst($sentence) . '. ';
      $words_remaining -= $sentence_length;
    }
  }
  else {
    // Use slightly different method for titles.
    $words = array_rand($greek_flipped, $word_count);
    $words = is_array($words) ? implode(' ', $words) : $words;
    $greek_text = ucwords($words);
  }
  return trim($greek_text);
}
