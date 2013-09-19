<?php
// $Id$
/** @file
 * Implementation of translation overview module pages.
 *
 * This file contains code which controls how the translation_overview module content is displayed.
 *
 */
function translation_overview_node_page($node) {
  drupal_set_title(t('Translations of %title', array('%title' => $node->title)), PASS_THROUGH);

  return drupal_get_form('translation_overview_node_form', $node);
}

/**
 * Overview page for a node's translations.
 *
 * Based on the translation.module's translation_node_overview().
 *
 * @param $node
 *   Node object.
 */
function translation_overview_node_form($form, &$form_state, $node) {
  if ($node->tnid) {
    // Already part of a set, grab that set.
    $tnid = $node->tnid;
    $translations = translation_node_get_translations($node->tnid);
  }
  else {
    // We have no translation source nid, this could be a new set, emulate that.
    $tnid = $node->nid;
    $translations = array($node->language => $node);
  }
  $priorities = translation_overview_get_node_priority($node);

  $form['languages']['#tree'] = TRUE;

  foreach (locale_language_list() as $lang_code => $language) {
    $form['languages'][$lang_code]['priority'] = array(
      '#type' => 'radios',
      '#default_value' => $priorities[$lang_code],
      '#options' => array(
        TRANSLATION_OVERVIEW_HIGH => t('High'),
        TRANSLATION_OVERVIEW_NORMAL => t('Normal'),
        TRANSLATION_OVERVIEW_IGNORE => t('Ignore'),
      ),
      // If there's no manager role then everyone gets a chance. Otherwise
      // check that they're a manager for this language.
      '#access' => translation_overview_is_manager($lang_code),
    );

    if (isset($translations[$lang_code])) {
      // Existing translation in the translation set: display status.
      // We load the full node to check whether the user can edit it.
      $translation_node = node_load($translations[$lang_code]->nid);
      if ($translation_node->nid == $tnid) {
        // Original shouldn't have a priority.
        $form['languages'][$lang_code]['priority']['#access'] = FALSE;
        $form['languages'][$lang_code]['language'] = array(
          '#markup' =>  '<strong>' . $language . '</strong> (source)',
        );
      }
      else {
        $form['languages'][$lang_code]['language'] = array(
          '#markup' => $language,
        );
      }
      $form['languages'][$lang_code]['title'] = array(
        '#markup' => l($translation_node->title, 'node/' . $translation_node->nid),
      );
      $form['languages'][$lang_code]['status'] = array(
        '#markup' => translation_overview_translation_link($node, $translation_node, $lang_code, TRUE),
      );
    }
    else {
      // No such translation in the set yet: help user to create it.
      $form['languages'][$lang_code]['language'] = array(
        '#markup' => $language,
      );
      $form['languages'][$lang_code]['title'] = array(
        '#markup' => t('n/a'),
      );
      $form['languages'][$lang_code]['status'] = array(
        '#markup' => translation_overview_translation_link($node, NULL, $lang_code, TRUE),
      );
    }
  }

  $form['tnid'] = array(
    '#type' => 'value',
    '#value' => $tnid,
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit settings'),
  );

  return $form;
}

function translation_overview_node_form_submit($form, &$form_state) {
  $row = array('tnid' => (int) $form_state['values']['tnid']);
  foreach ($form_state['values']['languages'] as $language => $values) {
    $row[translation_overview_field_name($language)] = $values['priority'];
  }

  // We can assume that there will alwyas be a record with the given tnid
  // because translation_overview_get_node_priority() has been called and it
  // creates records if one is not present.
  if (drupal_write_record('translation_overview_priority', $row, 'tnid') !== FALSE) {
    drupal_set_message(t('The translation priorities have been saved.'));
  }
  else {
    drupal_set_message(t('There was a problem saving the translation priorities.'), 'error');
  }
}


function theme_translation_overview_node_form($form) {

  drupal_add_css(drupal_get_path('module', 'translation_overview') . '/translation_overview.css');

  $is_manager = translation_overview_is_manager();

  $header = array(
    array('data' => t('Language')),
    array('data' => t('Title')),
    array('data' => t('Status')),
  );

  if ($is_manager) {
    $header[] = array('data' => t('Priority'), 'colspan' => 3);
  }

  $form = $form['form'];
  $rows = array();
  foreach (element_children($form['languages']) as $key) {

    $row = array();
    $row[] = array('data' => drupal_render($form['languages'][$key]['language']));
    $row[] = array('data' => drupal_render($form['languages'][$key]['title']));
    $row[] = array('data' => drupal_render($form['languages'][$key]['status']), 'class' => array('status'));

    if ($is_manager) {
      foreach (element_children($form['languages'][$key]['priority']) as $priority) {
        $row[] = array('data' => drupal_render($form['languages'][$key]['priority'][$priority]));
      }
    }
    $rows[] = $row;
  }


  $themed = theme('table', array('header' => $header, 'rows' => $rows, 'attributes' => array('class' => array('trov'))));

  return $themed . drupal_render_children($form);

}


/**
 * List node administration filters that can be applied.
 *
 * This is a fork of node_filters().
 */
function translation_overview_node_filters() {
  // Regular filters
  $filters['status'] = array(
    'title' => t('Status'),
    'options' => array(
      '[any]' => t('any'),
      'status-1' => t('published'),
      'status-0' => t('not published'),
      'promote-1' => t('promoted'),
      'promote-0' => t('not promoted'),
      'sticky-1' => t('sticky'),
      'sticky-0' => t('not sticky'),
      'translate-0' => t('Up-to-date translation'),
      'translate-1' => t('Outdated translation'),
    ),
  );

  $filters['type'] = array(
    'title' => t('type'),
    'options' => array_merge(array('[any]' => t('Any')), translation_overview_node_types()),
  );

  // The taxonomy filter
  if ($taxonomy = module_invoke('taxonomy', 'form_all', 1)) {
    $filters['category'] = array('title' => t('category'), 'options' => $taxonomy);
  }

  //The domain access filter : list domains managed by the Domain Access module
  if (module_exists('domain')) {
    $domains = domain_domains();
    if (count($domains) > 1) {
      $filters['domain'] = array('title' => t('Domain'), 'options' => array());
      foreach ($domains as $domain) {
        $filters['domain']['options'][$domain['domain_id']] = $domain['sitename'];
      }
    }
  }

  return $filters;
}

/**
 * Return form for node administration filters.
 *
 * This is a fork of node_filter_form().
 */
function translation_overview_filter_form($form = array()) {
  // We reuse a bunch of the node.module's stuff.
  module_load_include('inc', 'node', 'node.admin');
  $filters = translation_overview_node_filters();

  $session = isset($_SESSION['translation_overview_filter']) ? $_SESSION['translation_overview_filter'] : array();

  $form['#submit'][] = 'translation_overview_filter_form_submit';

  $i = 0;
  $form['filters'] = array(
    '#type' => 'fieldset',
    '#title' => t('Show only items where'),
    '#theme' => 'exposed_filters__node',
  );
  foreach ($session as $filter) {
    list($type, $value) = $filter;
    if ($type == 'term') {
      // Load term name from DB rather than search and parse options array.
      $value = module_invoke('taxonomy', 'term_load', $value);
      $value = $value->name;
    }
    elseif ($type == 'language') {
      $value = $value == LANGUAGE_NONE ? t('Language neutral') : module_invoke('locale', 'language_name', $value);
    }
    else {
      $value = $filters[$type]['options'][$value];
    }
    $t_args = array('%property' => $filters[$type]['title'], '%value' => $value);
    if ($i++) {
      $form['filters']['current'][] = array('#markup' => t('and where %property is %value', $t_args));
    }
    else {
      $form['filters']['current'][] = array('#markup' => t('where %property is %value', $t_args));
    }
    if (in_array($type, array('type', 'language'))) {
      // Remove the option if it is already being filtered on.
      unset($filters[$type]);
    }
  }

  $form['filters']['status'] = array(
    '#type' => 'container',
    '#attributes' => array('class' => array('clearfix')),
    '#prefix' => ($i ? '<div class="additional-filters">' . t('and where') . '</div>' : ''),
  );
  $form['filters']['status']['filters'] = array(
    '#type' => 'container',
    '#attributes' => array('class' => array('filters')),
  );
  foreach ($filters as $key => $filter) {
    $form['filters']['status']['filters'][$key] = array(
      '#type' => 'select',
      '#options' => $filter['options'],
      '#title' => check_plain($filter['title']),
      '#default_value' => '[any]',
    );
  }

  $form['filters']['status']['actions'] = array(
    '#type' => 'actions',
    '#attributes' => array('class' => array('container-inline')),
  );
  $form['filters']['status']['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => count($session) ? t('Refine') : t('Filter'),
  );
  if (count($session)) {
    $form['filters']['status']['actions']['undo'] = array('#type' => 'submit', '#value' => t('Undo'));
    $form['filters']['status']['actions']['reset'] = array('#type' => 'submit', '#value' => t('Reset'));
  }

  drupal_add_js('misc/form.js');

  return $form;
}



/**
 * Process result from node administration filter form.
 *
 * This is a fork of node_filter_form().
 */

function translation_overview_filter_form_submit($form, &$form_state) {
  $filters = translation_overview_node_filters();
  switch ($form_state['values']['op']) {
    case t('Filter'):
    case t('Refine'):
      // Apply every filter that has a choice selected other than 'any'.
      foreach ($filters as $filter => $options) {//print_r($form_state['values'][$filter]);echo $filter;die();
        if (isset($form_state['values'][$filter]) && $form_state['values'][$filter] != '[any]') {
          // Flatten the options array to accommodate hierarchical/nested options.
          $flat_options = form_options_flatten($filters[$filter]['options']);
          // Only accept valid selections offered on the dropdown, block bad input.
          if (isset($flat_options[$form_state['values'][$filter]])) {
            $_SESSION['translation_overview_filter'][] = array($filter, $form_state['values'][$filter]);
          }
        }
      }
      break;
    case t('Undo'):
      array_pop($_SESSION['translation_overview_filter']);
      break;
    case t('Reset'):
      $_SESSION['translation_overview_filter'] = array();
      break;
  }
}

/**
 * Build the where clause for a filtered query.
 *
 * This is a fork of node_filter_form_submit().
 */
function translation_overview_build_filter_query(SelectQueryInterface $query = NULL) {
  $filters = translation_overview_node_filters();

  // Build query
  $type = NULL;
  if (isset($_SESSION['translation_overview_filter'])) {
    foreach ((array) $_SESSION['translation_overview_filter'] as $index => $filter) {
      list($key, $value) = $filter;
      switch ($key) {
        case 'status':
          // Note: no exploitable hole as $key/$value have already been checked when submitted
          list($key, $value) = explode('-', $value, 2);

          $query->condition('n.' . $key, $value);
          break;
        case 'category':
          $table = "tn$index";
          $query->condition("$table.tid" , $value);
          $query->join('term_node', $table, "n.nid = $table.nid");
          break;
        case 'type':
          $query->condition("n.type" , $value);
          $type = $value;
          break;
        case 'domain':
          // Perform query only on the selected domain managed by the Domain
          // Access module.
          $table = "da$index";

          $query->condition("$table.gid" , $value);
          $query->join('domain_access', $table, "n.nid = $table.nid");
          break;
      }
    }
  }

  // Make sure we limit it to translation enabled types.
  if (empty($type)) {
    $types = array_keys(translation_overview_node_types());
    $query->condition("n.type", $types);
  }


  return $query;
}

/**
 * Translation overview page.
 */
function translation_overview_manager_page() {
  drupal_add_css(drupal_get_path('module', 'translation_overview') . '/translation_overview.css');

  $rows_per_page = 30;

  // Get a list of the enabled languages that this user manages.
  $languages = array();
  foreach (locale_language_list() as $lang_code => $language) {
    if (translation_overview_is_manager($lang_code)) {
      $languages[$lang_code] = $language;
    }
  }

  // Bail if there are no translatable nodes
  if (count(translation_overview_node_types()) == 0) {
    drupal_set_message(t('There are no translatable node types on this site.'), 'error');
    return '';
  }

  $header = array(
    array('field' => 'n.title', 'data' => t('Title'), 'sort' => 'asc'),
    array('field' => 'n.type', 'data' => t('Type')),
    array('field' => 'n.created', 'data' => t('Created')),
  );
  foreach ($languages as $lang_code => $lang_name) {
    $header[] = array('data' => str_replace('-', '-<br />', $lang_code), 'class' => array('trov-lang'), 'title' => $lang_name);
  }

  $query = new SelectQuery('node', 'n', Database::getConnection());
  $query = translation_overview_build_filter_query($query);

  $query->addField('n', 'nid');
  $query->addField('n', 'title');
  $query->addField('n', 'type');
  $query->addField('n', 'created');
  $query->where("(n.nid = n.tnid OR n.tnid = 0) AND n.language <> '' AND n.language IS NOT NULL");

  $rows = array();
  $result = $query->execute();
  foreach ($result as $node) {
    $node = node_load($node->nid);

    $row = array(
      array('data' => l(translation_overview_trimmed_title($node), 'node/' . $node->nid . '/translate', array('attributes' => array('title' => $node->title), 'query' => array('destination' => $_GET['q'])))),
      array('data' => check_plain($node->type)),
      array('data' => format_date($node->created, 'custom', 'j M Y')),
    );

    // Load the node's translations and then fill in the table with the status.
    $translations = (array) translation_node_get_translations($node->tnid);
    foreach ($languages as $lang_code => $lang_name) {
      $translation = empty($translations[$lang_code]->nid) ? NULL : node_load($translations[$lang_code]->nid);
      $row[$lang_code] = array(
        'data' => translation_overview_translation_link($node, $translation, $lang_code, FALSE),
        'class' => array('status'),
      );
    }

    $rows[] = $row;
  }

  return drupal_render(drupal_get_form('translation_overview_filter_form'))
    . theme('table', array('header' => $header, 'rows' => $rows, 'attributes' => array('class' => array('trov-all'))))
    . theme('pager', array('tags' => NULL));
}

/**
 * Show a listing of the languages that translators can look at.
 */
function translation_overview_assignment_lang_page() {
  $items = array();
  foreach (locale_language_list() as $lang_code => $language) {
    $items[] = l($language, 'admin/config/content/translation-overview-assignments/' . $lang_code);
  }
  return theme('item_list', array('items' => $items));
}

function translation_overview_assignment_page($language) {
  drupal_add_css(drupal_get_path('module', 'translation_overview') . '/translation_overview.css');

  $rows_per_page = 30;
  $node_types = node_type_get_names();
  $languages = language_list();

  // Bail if there are no translatable nodes
  if (count(translation_overview_node_types()) == 0) {
    drupal_set_message(t('There are no translatable node types on this site.'), 'error');
    return '';
  }

  $header = array(
    array('field' => 'n.title', 'data' => t('Title')),
    array('field' => 'translation_status', 'data' => t('Status'), 'sort' => 'desc'),
    array('field' => 'n.type', 'data' => t('Type')),
    array('field' => 'n.language', 'data' => t('Source language')),
    array('field' => 'n.created', 'data' => t('Created')),
  );

  // We want to sort the nodes by the status so we have to resort to this SQL
  // CASE statement.
  $rows = array();
  $query = new SelectQuery('node', 'n', Database::getConnection());
  $query = translation_overview_build_filter_query($query);
  $language_field = translation_overview_field_name($language);

  $query->addField('n', 'nid');
  $query->addField('n', 'title');
  $query->addField('n', 'type');
  $query->addField('n', 'status');
  $query->addField('n', 'language');
  $query->addField('n', 'created');
  $query->addField('nt', 'nid', 'translation_nid');
  //$sql = "SELECT n.nid, n.title, n.type, n.status, n.language, n.created, nt.nid AS translation_nid,
  $query->addExpression("CASE
    WHEN n.language = ':lang' " /* ORIGINAL */ . " THEN 0
    WHEN nt.tnid IS NULL   " /* MISSING  */ . " THEN 3 + 2 * top.$language_field
    WHEN nt.translate = 0  " /* COMPLETE */ . " THEN 1
    WHEN n.nid = n.tnid    " /* OUTDATED */ . " THEN 2 + 2 * top.$language_field
    ELSE -1 END", "translation_status", array(':lang' => $language));

  $query->leftJoin('node', 'nt', 'n.nid=nt.tnid AND nt.language=:lang', array(':lang' => $language));
  $query->innerJoin('translation_overview_priority', 'top', "n.nid = top.tnid AND top.$language_field <> :lang", array(':lang' => $language));
  $query->where("(n.nid = n.tnid OR n.tnid = 0) AND n.language NOT IN ('', :lang) AND n.language IS NOT NULL", array(':lang' => $language));

  $query = $query->extend('PagerDefault');
  $query->limit($rows_per_page);

  $query = $query->extend('TableSort');
  $query->orderByHeader($header);

  $rows = array();
  $result = $query->execute(); // pager_query(db_rewrite_sql($sql), $rows_per_page, 0, db_rewrite_sql($count_sql), $args);

  while ($n = $result->fetchAssoc()) {
    $node = node_load($n['nid']);
    $translation = empty($n['translation_nid']) ? NULL : node_load($n['translation_nid']);
    $rows[] = array(
      array('data' => node_access('view', $node) ? l(translation_overview_trimmed_title($node, 30), 'node/' . $node->nid, array('attributes' => array('title' => $node->title))) : translation_overview_trimmed_title($node, 30)),
      array('data' => translation_overview_translation_link($node, $translation, $language, TRUE), 'class' => array('status')),
      array('data' => isset($node_types[$node->type]) ? $node_types[$node->type] : check_plain($node->type)),
      array('data' => isset($languages[$node->language]) ? $languages[$node->language]->name : check_plain($node->language)),
      array('data' => format_date($node->created, 'custom', 'j M Y')),
    );
  }

  return drupal_render(drupal_get_form('translation_overview_filter_form'))
    . theme('table', array('header' => $header, 'rows' => $rows, 'attributes' => array('class' => array('trov'))))
    . theme('pager', array('tags' => NULL));
}
