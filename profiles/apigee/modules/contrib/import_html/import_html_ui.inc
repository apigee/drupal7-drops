<?php
/**
 * @file
 * Forms and UI functions for import_html.
 *
 * The bulk is made up of the multistep import_html_process_form.
 * This is made up of conditional includes of the per-stage subforms, and their
 * respective submit handlers.
 *
 * TODO ajaxify?
 *
 * The actual PROCESS starts at import_html_import_files_submit()
 * which triggers import_html_import_files() where the action happens.
 *
 * @ingroup import_html Import HTML
 * @author Dan Morrison http://coders.co.nz/
 */

module_load_include('inc', 'import_html', 'import_html_process');

///////////////////////////////////////////
// SETTINGS and PROFILES

/**
 * Display the global import_html options and requirements
 *
 * @return array FAPI form
 */
function import_html_setup($form, &$form_state) {

  $form = array();
  // Check htmltidy is present and correct
  module_load_include('inc', 'import_html', 'coders_php_library/tidy-functions');
  $form['HTMLTidy'] = html_tidy_settings($form_state);

  // Allow user to define the max items per page for the treeeview.
  // Replace IMPORT_HTML_MAX_FILE_LISTING_COUNT
  $form['import_html_max_file_listing_count'] = array(
    '#type' => 'textfield',
    '#size' => 4,
    '#title' => t('Maximum number of files to display in the import treeview wizard.'),
    '#default_value' => variable_get('import_html_max_file_listing_count', 500),
    '#description' => t("This value is limited for performance reasons - it's possible for your browser to break or timeout if trying to display all files in huge sites, especially sites that are extremely 'wide' with lots of items at the 'top level'. If you are running a fast machine and an efficient browser, you can increase this number, but step 2 of the import process <em>may</em> become sluggish if you bump it into the thousands of rows."),
  );

  // Warn about known problem with open_basedir
  if ($open_basedir = ini_get('open_basedir')) {
    $form['open_basedir'] = array(
      '#value' => t("
        <p><strong>Warning:</strong>
        This server has open_basedir restrictions set [%open_basedir].
        It's possible that external source directories may be inaccessible,
        or that access to htmlTidy may be restricted.
        <a href='%help_url'>Check the help for more details</a>.</p>
      ",
      array(
        '%open_basedir' => $open_basedir,
        '%help_url' => url('admin/help/import_html', array('fragment' => 'open_basedir')),
      )
      ),
    );
  }

  return system_settings_form($form);
}

/**
 * Small submit handler to redirect to the process form - just so we get a
 * button.
 */
function import_html_goto_process() {
  drupal_goto(IMPORT_HTML_ADMIN_PATH . '/import_site');
}


/**
 * Page to display a list of current profiles, or to edit a named one.
 *
 * As seen at /admin/structure/import_html/profiles .
 *
 * @return a rendered form for configuring the named profile.
 */
function import_html_profiles_page($profile_id = NULL) {
  if (!$profile_id) {
    // Assume the currently or last selected 'default' profile
    $profile_id = import_html_current_profile_id();
    if (empty($profile_id)) {
      $profile_id = reset(array_keys(import_html_profile_load_all()));
    }
    if (empty($profile_id)) {
      $profile_id = 'new';
    }
    drupal_goto(IMPORT_HTML_ADMIN_PATH . '/profiles/' . $profile_id);
  }
  return drupal_get_form('import_html_profile_settings', $profile_id);
}


/**
 * An import_html profile settings form.
 *
 * @return array FAPI form.
 */
function import_html_profile_settings($form, &$form_state, $profile_id) {
  // Allow individual modules to add even more settings to this form.

  // Ensure our modules dir with support for core is loaded.
  // Supporting modules can then implement {module}_form_{formname}_alter()
  // and add bits to this form.
  import_html_include_add_on_module_handlers();

  $profile = import_html_profile_load($profile_id);
  if (! $profile) {
    // Fill in defaults (should only be needed first time, if that.)
    $profile = import_html_profile_defaults();
  }
  #dpm($profile);

  $form = array();
  if ($profile_id != 'new') {
    $form['summary'] = array('#markup' => import_html_profile_summary($profile));
  }
  if ($profile_id == 'new') {
    $form['profile_id'] = array(
      '#title' => t("Profile ID"),
      '#type' => 'textfield',
      '#required' => TRUE,
      '#default_value' => $profile_id,
    );
  }
  else {
    $form['profile_id'] = array(
      '#type' => 'value',
      '#value' => $profile_id,
    );
  }
  // Save the raw data so form_alter hooks can find it.
  $form['#profile_id'] = $profile_id;
  $form['#profile'] = $profile;

  $form['extraction'] = array(
    '#type' => 'fieldset',
    '#title' => t('Import and Content Analysis Options'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['extraction'][] = array(
    '#value' => '<p>' . t("How the content is extracted from the source documents.") . '</p>',
  );

  // This is abstracted so it can be re-used in other places (wrapper.module)
  $form['extraction']['translation_template'] = import_html_template_selector($profile['translation_template']);

  $form['extraction']['content_tag_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Content Tag ID'),
    '#default_value' => $profile['content_tag_id'],
    '#size' => 20,
    '#maxlength' => 100,
    '#description' => t("
      If your source document is structured well enough to have the actual
      content in a named div or something (&lt;div id='main'&gt;...content )
      what is that name?
      <br/>
      The default <code>html2simplehtml</code> template will search for
      'main', 'copy', 'story', 'text', 'contentbody' and 'bodyCopy'.
      If your input uses another label, set it here.
      <br/>
      If you use a generic classname like 'content' you may retrieve <em>all</em>
      elements in the page with class 'content', which is probably not good.
      <br/>
      This only takes effect if the selected XSL template uses this parameter,
      and may be overridden or ignored by custom templates.
    "),
  );

  $form['extraction']['default_document'] = array(
    '#type' => 'textfield',
    '#title' => t("Default Document"),
    '#default_value' => $profile['default_document'],
    '#size' => 40,
    '#description' => t("
      What is the default document name for pages served in directories on
      the current site (index.htm, index.html, default.asp, index.php)?
      When documents of this name are imported, they can be given both
      the old name (my/topic/index.htm) and a friendly directory name (my/topic).
      When the navigation menu is built, this file is served when the
      directory-like link is clicked.
      This should end up emulating normal website navigation behaviour.
      <br/>
      May be a comma-separated list, the first option is the default.
      <small>If your source site has more than one default document candidate
      in a single folder ... well ... please don't do that.</small>
    "),
  );

  $form['extraction']['file_exclusions'] = array(
    '#type' => 'textarea',
    '#title' => t("File Exclusion Pattern"),
    '#default_value' => $profile['file_exclusions'],
    '#description' => t("
      When listing files to import, some sources should probably be left behind.
      Enter a set of regular expressions to check against the file <b>path</b>
      that should <em>not</em> show up on the import screen.
      <ul><li>
      <code>^_</code> = begins with '_'
      </li><li>
      <code>/_</code> = in a dir that begins with '_'
      </li><li>
      <code>CVS/?</code> = called CVS, or has CVS in the path
      </li></ul>"
      . 'Resulting regexp pattern: <code>'
      . import_html_file_exclusions_to_regexp($profile['file_exclusions'])
      . '</code>'
    ),
  );

  $form['extraction']['strip_tables'] = array(
    '#type' => 'checkbox',
    '#title' => t("Remove table markup"),
    '#return_value' => TRUE,
    '#default_value' => $profile['strip_tables'],
    '#description' => t("
      A common task when updating old sites is removing the
      legacy formatting tables. While we are going through the
      pages, this process can do it all for you.
      Beware - enabling this will remove ALL tables, so don't
      try it if there are genuine data tables you wish to retain!
    "),
  );

  $form['extraction']['cleanup_namespaces'] = array(
    '#type' => 'checkbox',
    '#title' => t("Clean up xmlns namespace declarations"),
    '#return_value' => TRUE,
    '#default_value' => $profile['cleanup_namespaces'],
    '#description' => t("
      This is rare, but some input (eg from MSWord) may have layers of
      inappropriate xmlns:o=\"urn:schemas-microsoft-com:office:office\"
      etc markup inserted into the HTML.
      This is technically actually good and accurate (!).
      However, it can turn out really cluttered markup.
      With this option on,
      <ul><li>xmlns declarations that are <em>not actually used</em> will
        be stripped. (good)</li>
        <li>repeated xmlns declarations that are identical will be shifted up
        in the document so they only have to happen once at the top,
        not on every element</li></ol>
      PHP DOM processing doesn't do this by default, so it's an extra small
      step. Leave it on unless you are encountering namespace problems and
      understand what's (not) happening.
    "),
  );

  $form['extraction']['force_tidy'] = array(
    '#type' => 'checkbox',
    '#title' => t("Force HTML Tidy-up"),
    '#return_value' => TRUE,
    '#default_value' => $profile['force_tidy'],
    '#description' => t("
      Run HTML Tidy with all strict options over the input before
      parsing it as XML. Normally you want this on BUT turn it off
      if trying to read from non-HTML sources (like arbitrary XML)
      or it will mutilate it.
      <br/>
      Instead of using HTML input, you can feed in any <b>valid</b> XML
      and use XSL template to massage it into simple XHTML.
      But in that case disable this option and do NOT use HTML tidy.
    "),
  );
  $form['extraction']['absorb_all_tagged_elements']  = array(
    '#type' => 'checkbox',
    '#title' => t("Absorb all elements with classes or IDs"),
    '#return_value' => TRUE,
    '#default_value' => $profile['absorb_all_tagged_elements'],
    '#description' => t("
      Greedily try to see if anything at all in the source document looks like
      it may be useful. Any element with an HTML class or id will be extracted
      into our data array and made available for processing.
      Default is on.
      Turn it off only if you are going to use a specially tuned set of cutom
      patterns of your own.
      A lot of other functionality may assume this is on and data from it is available.
    "),
  );

  $form['extraction']['spider'] = array(
    '#type' => 'checkbox',
    '#title' => t("EXPERIMENTAL spidering option"),
    '#return_value' => TRUE,
    '#default_value' => $profile['spider'],
    '#description' => t("
      This is unneccessary on local sites.
      It MAY trigger recursive loading when importing directly from
      a remote URL. It may also fall into an infinite loop.
      DO NOT USE.
    "),
  );

  ///////////////////////////
  // Replication options
  ///////////////////////////

  $form['replication'] = array(
    '#type' => 'fieldset',
    '#title' => t("Replication Options"),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['replication'][] = array(
    '#type' => 'markup',
    '#prefix' => '<p>',
    '#markup' => t("How and where the imported content is reproduced on this site."),
    '#suffix' => '</p>',
  );


  $form['replication']['content_type'] = array(
    '#type' => 'select',
    '#title' => t("Node Type for new pages"),
    '#options' => node_type_get_names(),
    '#default_value' => $profile['content_type'],
    '#description' => t("
      This setting can be overridden at runtime using logic that inspects
      the content itself - but that's custom code. @see hook_import_html().
      If importing into a CCK content type, or anything that doesn't
      store its body in the node 'body', some customization will have to be
      done on your XSL template.
      "),
  );

  // Just a select, the filter radios are much to bulky for here.
  $form['replication']['preferred_filter'] = array(
    '#type' => 'select',
    '#title' => t("Default Input Filter"),
    '#default_value' => $profile['preferred_filter'],
    '#options' => import_html_get_preferred_filter($profile, TRUE),
    '#description' => t("
      After importing HTML, the end result may still have a standard Drupal
      filter applied to it internally.
      Use 'Unfiltered HTML' if you want the content verbatim.
      'Full HTML' <em>may</em> be correct here, although it may create
      unwanted hard breaks if the input is already word-wrapped internally.
      Modify only if you want to strip the source right down, execute php,
      or do other filter magic on the input.
      "),
  );


  if (!module_exists('path')) {
    drupal_set_message(t("The <b>path</b> module is not enabled! If you try to import pages without paths, I can do it, but links won't work and you'll find it hard to find the pages again.'"), 'error');
  }

  $form['replication']['linking'] = array(
    '#type' => 'fieldset',
    '#title' => t('URLs and linking'),
  );

  $form['replication']['linking']['import_site_prefix'] = array(
    '#type' => 'textfield',
    '#title' => t("Import Site Prefix"),
    '#default_value' => ensure_trailing_slash($profile['import_site_prefix']),
    '#size' => 20,
    '#maxlength' => 100,
    '#description' => t("
      Where, beneath siteroot, should extra imported <strong>pages</strong>
      <em>appear to be</em> served from?
      This is a prefix that is applied to the generated URL-aliases.
      <br/>
      If you wish to keep the imported
      pages separate from the main site, setting this value to look like a
      subdirectory will make the imported site act like it's in a subsection.
      Otherwise leave it blank and new page URLs will be added under the top
      level navigation.
    "),
  );

  $form['replication']['linking']['file_storage_path'] = array(
    '#type' => 'textfield',
    '#title' => t("Extra File Storage Path"),
    '#default_value' => $profile['file_storage_path'],
    '#description' => t("
      Where, beneath siteroot, should extra imported <strong>files</strong>
      be stored. Images and suchlike will be put into a heirachy mirroring
      their old location, but all under this directory.
      <br/>
      Effectively, this moves file storage towards the Drupal CMS way of
      storing user files, although it does not enter them as 'attachments'.
      (See <a href=\"http://drupal.org/node/1110890\">additional post-process
      filters</a> that may help with that.)
      <br/>
      Enter '.' if you want files to be stored directly under siteroot.
      (Very risky, but may suit some legacy conversions)
      Take care they don't conflict with drupal files and folders.
    "),
  );

  $form['replication']['linking']['trim_suffixes'] = array(
    '#type' => 'checkbox',
    '#title' => t("Trim Suffixes"),
    '#return_value' => TRUE,
    '#default_value' => $profile['trim_suffixes'],
    '#description' => t("
      When importing files, nice new url_aliases will be generated based on the
      old path. Optionally, the new primary alias can have the suffix
      chopped off. So <code>old/path/file.htm</code> wil be accessed as just
      <code>old/path/file</code> and <code>old/section/index.htm</code>
      will become <code>old/section</code>.
      Uncheck this if you do NOT want this tidy behaviour.
      <br/>Note, this aliasing is needed to detect when an import is
      <em>replacing</em> a previously imported page. Disabling it may
      cause you to end up with duplicates if importing the same file again.
      <br/>This just defines the name given to the page, it does not
      automatically update any links <em>to</em> the page (see below) .
    "),
  );

  $form['replication']['linking']['legacy_aliases'] = array(
    '#type' => 'checkbox',
    '#title' => t("Legacy Aliases"),
    '#return_value' => TRUE,
    '#default_value' => $profile['legacy_aliases'],
    '#description' => t("
      Optionally, a secondary alias can be made that DOES have the old
      full suffix. This supports old links and hrefs. <em>You want this</em>
      unless the source URLS are entirely useless.
      <b>The new node may end up with two aliases</b> (thats OK) but the tidy
      one will be used internally by default.
      Uncheck this if you do NOT want old URL support.
      <br/>
      These two options are independant of each other. If both are off
      the new page will have no url_alias.
    "),
  );

  $form['replication']['linking']['relink_files'] = array(
    '#type' => 'checkbox',
    '#title' => t("Try to Relink Files by discarding suffixes"),
    '#return_value' => TRUE,
    '#default_value' => $profile['relink_files'],
    '#description' => t("
      <b>If</b> you choose not to support the Legacy Aliases suffixes
      on your new pages, <em>existing embedded links to those pages will break</em>.
      This includes inline links from within the content we are importing.
      <br/>If a page linked to /home/about.htm and you have renamed it
      to home/about, without legacy support, the old link will be broken.
      This tool is a batch job process, not an entire site spider and
      relinker.
      <br/>
      <b>But</b> if you understand exactly what is going on,
      you can choose to try and rewrite all local links to their shortname
      equivalent (basically dropping off the suffixes of linked files)
      during import.
      This will convert any existing <code>href=\"local/path/file.htm\"</code>
      into just <code>href=\"local/path/file\"</code>. Use with care, but
      this option plus \"Trim Suffixes\" should be able to work together to
      rename all files. You'll want to run a link checker afterwards!
    "),
  );

  $form['replication']['linking']['allow_bad_urls'] = array(
    '#type' => 'checkbox',
    '#title' => t("Allow bad URLs and filenames"),
    '#return_value' => TRUE,
    '#default_value' => $profile['allow_bad_urls'],
    '#description' => t("
      It's bad mojo to have spaces or non-alphanumeric characters
      in your filenames and in your URLs. Normally I try to fix this
      by replacing potentially damaging characters with an underscore.
      If, however you have to support legacy stuff like
      <code>'Random filename #3 - & about what?.pdf'</code>
      then we can try to leave them alone.
      May not always work the way you expect.
    "),
  );

  $form['replication']['linking']['rewrite_links'] = array(
    '#type' => 'checkbox',
    '#title' => t("Relink references to resources to the new file storage location"),
    '#return_value' => TRUE,
    '#default_value' => $profile['rewrite_links'],
    '#description' => t("
      As the actual files are being stored elsewhere in a nice Drupally way,
      the HTML links need to be rewritten to allow for that.
      This should be on by default.
      Turn off only if you are intending to re-create a messy site with files
      in old places and have set the file storage path to blank.
      See the transformation file
      <code>rewrite_href_and_src.xsl</code> for fine-tuning if there is trouble.
      <small>Only relative and site-relative links are rewritten.
      Hard-coded, fully-justified (external) URLS are not messed with,
      even if they used to resolve to the same host.</small>
    "),
  );

  // Publishing settings.
  $form['replication']['publishing'] = array(
    '#type' => 'fieldset',
    '#title' => t('Publishing settings'),
  );

  // Status selection.
  $form['replication']['publishing']['import_status'] = array(
    '#type' => 'checkbox',
    '#title' => t('Imported nodes are Published?'),
    '#default_value' => $profile['import_status'],
    '#description' => t("
      Sets the node status. Check this to have nodes published.
    "),
  );

  // Promote selection:  added by L0rne
  $form['replication']['publishing']['import_promote'] = array(
    '#type' => 'checkbox',
    '#title' => t('Imported Nodes are Promoted to front page?'),
    '#default_value' => $profile['import_promote'],
    '#description' => t("
      Sets whether or not imported nodes are promoted to the front page.
    "),
  );

  global $user;
  $form['replication']['publishing']['import_user'] = array(
    '#type' => 'textfield',
    '#title' => t('User to create nodes as'),
    '#maxlength' => 60,
    '#autocomplete_path' => 'user/autocomplete',
    '#default_value' => $profile['import_user'],
  );

  // If some content types are set to 'translateable', set the language.
  // Obviously this only works if the site has languages defined.
  if ($languages = module_invoke('locale', 'language_list')) {
    $languages = array('' => t('Language neutral')) + $languages;
    $transl_types = array();
    foreach (node_type_get_types() as $type => $val) {
      if (variable_get('language_content_type_' . $type, 0)) {
        $transl_types[] = $val->name;
      }
    }
    if (!$transl_types) {
      $transl_types[] = '<None>';
    }
    if ($transl_types) {
      $form['replication']['publishing']['import_lang'] = array(
        '#type' => 'select',
        '#title' => t('Language to assign to nodes'),
        '#default_value' => array_key_exists($profile['import_lang'], $languages) ? $profile['import_lang'] : '',
        '#options' => $languages,
        '#description' => t("
          Language to assign to imported nodes.
          This is only done if the Node Type (see above) has multilingual support enabled.
          Currently, multilingual types are: %typenames
          <br/>
          Note that if your site's 'Language negotiation' setting is set to 'path prefix',
          the original url_aliases may stop working unless you take special measures!
          This is because the node's url_aliases will have the 2-letter language code prepended.
          ", array('%typenames' => join(', ', $transl_types))
        ),
      );
    }
  }

  // Menu.
  if (module_exists('menu')) {
    $form['replication']['menu'] = array(
      '#type' => 'fieldset',
      '#title' => t('Menu settings'),
    );

    $form['replication']['menu']['create_menus'] = array(
      '#type' => 'checkbox',
      '#title' => t("Add each page to menu"),
      '#return_value' => TRUE,
      '#default_value' => $profile['create_menus'],
      '#description' => t('Requires menu.module. ' . (module_exists('menu') ? '(installed)' : '(Which is NOT available)')),
    );

    // Retrieve a select list of the entire menu tree.
    $menu_parent_options = menu_parent_options(menu_get_menus(), array('mlid' => 0));

    $form['replication']['menu']['menu_parent_id'] = array(
      '#type' => 'select',
      '#title' => t('Menu Parent'),
      '#default_value' => $profile['menu_parent_id'],
      '#options' => $menu_parent_options,
      '#description' => t("
        Where in the menu system should the new heirachy
        of pages be built?
        <br/>
        If you leave this as 'Navigation' the new pages
        may start to collide with your Admin menus,
        so consider making a placeholder menu first.
      "),
    );

    $form['replication']['menu']['hide_default_menu_placeholders'] = array(
      '#type' => 'checkbox',
      '#title' => t('Hide placeholder menu items'),
      '#return_value' => TRUE,
      '#default_value' => $profile['hide_default_menu_placeholders'],
      '#description' => t( '
        When creating menu items, sometimes we find that parent items are
        required in order to correctly position new pages into the menu tree.
        Initially these are created as "placeholder" nodes that stand in for
        index.htm sort of pages.
        These are always required, and are useful to see when prototyping,
        but you can choose to hide them from normal navigation menus by
        disabling the rendering of such pages using this setting.
      '),
    );
  }
  else {
    $form['replication']['menu'] = array(
      '#type' => 'markup',
      '#value' => t("
        Menu is not enabled, so the heirachical structure
        of the imported files will NOT be retained.
      "),
      '#description' => t("
        <p>If you enable the menu.module,
        the structure of the imported files can be imported as well.</p>
        <p>Note, that in order for this to work, all the apparent parents
        of a page must have menu items as well. If importing a subsection,
        Placeholders for the higher sections will be created, but they may be
        hollow shells.</p>
      "),
    );
  }

  ///////////////////////////
  // Advanced options
  ///////////////////////////

  $form['advanced'] = array(
    '#type' => 'fieldset',
    '#title' => t("Advanced Import Tuning"),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['advanced']["help"] = array(
    '#type' => 'markup',
    '#prefix' => '<p>',
    '#markup' => t("How to handle problems when importing"),
    '#suffix' => '</p>',
  );

  $form['advanced']['handle_duplicates'] = array(
    '#type' => 'select',
    '#title' => t("Duplicate Handling"),
    '#default_value' => $profile['handle_duplicates'],
    '#options' => array(
      IMPORT_HTML_MERGE => 'overwrite/merge',
      IMPORT_HTML_SKIP => 'skip',
      IMPORT_HTML_CAREFUL => 'overwrite unless modified',
    ),
    '#description' => t("
      If attempting to import a file into a path that already exists
      in the system (such as by repeating an import process), do what?
      \"overwrite/merge\" is normal and will re-import content over top of
      existing items.
      \"overwrite unless modified\" will check the last user that modified a
      page.
      If it differs from the user the script is running as,
      it will not be overwritten.
    "),
  );

  $form['advanced']['handle_no_title'] = array(
    '#type' => 'select',
    '#title' => t("No Title"),
    '#default_value' => $profile['handle_no_title'],
    '#options' => array(
      IMPORT_HTML_GUESS => 'use the document path',
      IMPORT_HTML_SKIP => 'skip, do not import',
      IMPORT_HTML_DEFAULT => 'set to a placeholder value',
    ),
    '#description' => t("
      If parsing the input document fails to return any title at all, do what?"
    ),
  );

  $form['advanced']['recursion_behaviour'] = array(
    '#type' => 'select',
    '#title' => t("Recursion behaviour"),
    '#default_value' => $profile['recursion_behaviour'],
    '#options' => array(
      IMPORT_HTML_GLOB_BEFORE => IMPORT_HTML_GLOB_BEFORE,
      IMPORT_HTML_RECURSE_AFTER => IMPORT_HTML_RECURSE_AFTER,
    ),
    '#description' => t("
      Very advanced option for mega-sites and batch processes.
      <br/>
      When selecting subdirectories, and there are too many items to display in
      the UI, you can choose to import everything under a given path.
      When using this option, we can either scan those directories
      <em>before</em> or <em>during</em> the process.
      <br/>
      Scanning and queuing everything first is more predictable, but may
      result in a huge list being loaded into memory first.
      <br/>
      Noting the directories to recurse into and only scanning them when
      it's time <em>may</em> be easier on the memory for gigantic sites.
      The batch job will keep extending until it is done.
      <br/>I don't know which is better.
      The first is easy, but I'd recommend the second for
      tens of thousands of pages.
    "),
  );

  $form['advanced']['import_html_debug_level'] = array(
    '#type' => 'select',
    '#title' => t("Debug Level"),
    '#default_value' => variable_get('import_html_debug_level', WATCHDOG_WARNING),
    '#options' => watchdog_severity_levels(),
    '#description' => t("
      To trace and test the import process, increasing the debug level here
      will produce a huge amount of process tracing,
      including dumps of the raw versions of the input files
      as it gets massaged into the end result.
      <br/>
      Note that enabling debug can itself introduce errors as the trace logs
      and pretty-printer can cause memory overages.
    "),
  );

  $form['advanced']['keep_temp_files'] = array(
    '#type' => 'checkbox',
    '#title' => t("Keep Temp Files"),
    '#return_value' => TRUE,
    '#default_value' => $profile['keep_temp_files'],
    '#description' => t("
      When processing, files are copied temporarily into a temp directory.
      These are usually deleted immediately after tidying and parsing,
      but if you want to trace problems, enable this option and check the
      files/import directory.
    "),
  );

  $form['advanced']['import_html_other_logic'] = array(
    '#value' => t("<p>
       Other logic, like whether to use the TITLE tag or the H1 tag found in a
       file is defined in the XSL template, which you can tune yourself.
       </p>"
    ),
  );

  $form['save'] = array(
    '#type' => 'submit',
    '#value' => t('Save Settings'),
  );
  $form['delete'] = array(
    '#type' => 'submit',
    '#value' => t('Delete Profile'),
    '#submit' => array('import_html_profile_settings_delete'),
  );
  $form['export'] = array(
    '#type' => 'submit',
    '#value' => t('Export Profile'),
    '#submit' => array('import_html_profile_export_button'),
  );

  return $form;
}

/**
 * Returns a text summary of all the current settings
 */
function import_html_profile_summary($profile) {
  $dest_file_root = url($profile['file_storage_path'], array('absolute' => TRUE));
  $dest_virtual_path = url($profile['import_site_prefix'], array('absolute' => TRUE));

  if (module_exists('menu')) {
    $menu_parent = menu_get_item($profile['menu_parent_id']);
    list($menu_name, $mlid) = explode(':', $profile['menu_parent_id'] );
    if ($mlid) {
      $menu = menu_link_load($mlid);
      // TODO
    }
    else {
      $menu = menu_load($menu_name);
      $menu_title = $menu['title'];
    }
  }
  else {
    // SRSLY? You don't have 'menu' on?
    $menu_title = '*DISABLED*';
  }

  if (function_exists('dpm') && (variable_get('import_html_debug_level', 0) > 0)) {
#    dpm($profile);
  }

  $import_categories = array();
  if (module_exists('taxonomy')) {
    foreach ((array) $profile['import_category'] as $tid) {
      if ($term = taxonomy_term_load($tid)) {
        $import_categories[$tid] = $term->name;
      }
    }
  }

  $substitutions = array(
    '%import_html_source_siteroot' => variable_get('import_html_source_siteroot', "/var/www/htdocs/"),
    '%import_html_current_subsection' => variable_get('import_html_current_subsection', ""),
    '!dest_virtual_path' => l($dest_virtual_path, $dest_virtual_path ),
    '!dest_file_root' => l($dest_file_root, $dest_file_root),
    '!settings_url' => url(IMPORT_HTML_ADMIN_PATH . '/settings'),
    '!translation_template' => l(basename($profile['translation_template']), $profile['translation_template']),
    '%default_document' => $profile['default_document'],
    '%create_menus' => $profile['create_menus'] ? 'will' : 'will not',
    '%menu_parent_name' => $menu_title,
    '%content_type' => $profile['content_type'],
    '%import_site_prefix' => $profile['import_site_prefix'],
    '%import_status' => $profile['import_status'] ? 'will' : 'will not',
    '%import_promote' => $profile['import_promote'] ? 'will' : 'will not',
    '%import_categories' => join($import_categories, ', '),
  );

  $summary = array();
  $summary[] = t("
      Source files will be scanned from
      <br/><code>%import_html_source_siteroot%import_html_current_subsection</code>
    ", $substitutions);
  $summary[] = t("
    Pages will be re-created underneath
    <br/><code>!dest_virtual_path%import_html_current_subsection</code>
    <br> as <strong>%content_type</strong> type nodes.", $substitutions);

  $summary[] = t("
    Import template for semantic data extraction is <code>!translation_template</code>
    ", $substitutions);

  $summary[] = $profile['rewrite_links']
    ? t("Links found within the sources <b>will</b> be rewritten to try and allow for the new paths")
    : t("Links from within the imported pages will <b>not</b> be rewritten, and may get lost.");

  $summary[] = t("Non-page files imported will be saved beneath <br/><code>!dest_file_root %import_html_current_subsection</code> ", $substitutions);

  $summary[] = t("Default document <code>%default_document</code> will be used to represent folders.", $substitutions);

  $summary[] = t("New pages %create_menus be added to the menu underneath %menu_parent_name.", $substitutions);
  if ($profile['menu_parent_id'] == 1) {
    $summary[] = t("<strong>Warning:</strong> The parent menu is set to the default 'Navigation' menu. This is OK, but will place new pages directly into the top of your menu which can be messy. You may instead wish to create a unique menu holder or menu item and use that as the import menu root instead. 'Primary Links' is also a good choice.", $substitutions);
  }

  $summary[] = t("Imported pages %import_status be published by default and %import_promote be promoted to the front page.", $substitutions);

  if (! empty($import_categories)) {
    $summary[] = t("ALL imported pages will be tagged with the terms: (%import_categories)", $substitutions);
  }

  $renderable = array(
    '#type' => 'fieldset',
    '#title' => t('%profile_label import profile:', array('%profile_label' => $profile['profile_id'])),
    '#value' => theme("item_list", array('items' => $summary)) ,

  );
  return drupal_render($renderable);
}



/**
 * Returns a widget for selecting an import template from the ones provided by
 * import_html
 */
function import_html_template_selector($current_template) {
  $templates = file_scan_directory(drupal_get_path('module', 'import_html') . '/templates', '/.xsl/');
  $options = array_map(
      create_function('$o', 'return $o->name;'),
      $templates
    );
  // Allow code set paths to be retained
  // TODO - find a way to expose contributed templates from features to be found better
  if ($current_template && file_exists($current_template) && ! isset($options[$current_template])) {
    $options[$current_template] = basename($current_template);
  }
  if (! $current_template) {
    $current_template = drupal_get_path('module', 'import_html') . "/templates/html2simplehtml.xsl";
  }

  return array(
    '#type' => 'select',
    '#title' => t('Import template to use'),
    '#options' => $options,
    '#default_value' => $current_template,
    '#prefix' => '<div id="import-template-selector" class="sub-option">',
    '#suffix' => '</div>',
    '#description' => t("
        This file must be a valid XSL Template that can convert from your source
        HTML to the simplified semantic XHTML schema (see docs and examples).
        Templates should be placed in the import_html module <code>/templates/</code> directory.
        <br/>
        Sample import templates can be found in %module_dir.
      ", array('%module_dir' => drupal_get_path('module', 'import_html') . '/templates')),
  );
}

/**
 * FAPI callback
 */
function import_html_profile_settings_validate($form, &$form_state) {
  // Initializing the XSL doc just to check is inefficient, but this is just a config screen.
  if (!_import_html_get_xsl_doc($form_state['values']['translation_template'])) {
    form_set_error("import_html_translation_template", "XSL File unable to be parsed");
  }
  // Check the $profile['file_storage_path']
  $file_storage_path = $form_state['values']['file_storage_path'];
  if (! is_writable($file_storage_path) || ! is_dir($file_storage_path)) {
    drupal_mkdir($file_storage_path);
  }
  if (! is_writable($file_storage_path) || ! is_dir($file_storage_path)) {
    form_set_error("file_storage_path", "Unable to write to or create file path. Please check permissions on the server.");
  }
}

/**
 * FAPI callback
 */
function import_html_profile_settings_submit($form, &$form_state) {
  if ($form_state['values']['profile_id'] == 'new') {
    drupal_set_message(t('Not saving details called "new", you must give your profile a name.', array()));
    return;
  }
  // Debug level is a global option
  variable_set('import_html_debug_level', $form_state['values']['import_html_debug_level']);
  // Ensure add-on helpers are loaded
  import_html_include_add_on_module_handlers();

  // Save it as a blob
  $profile = import_html_profile_save($form_state['values']);
  drupal_set_message(t('%profile_id Profile Updated', array('%profile_id' => $profile['profile_id'])));

  // Assume this is the one we want to work with now.
  import_html_current_profile_id($form_state['values']['profile_id']);

  menu_rebuild();
}

/**
 * Form button handler
 */
function import_html_profile_settings_delete($form, &$form_state) {
  import_html_profile_delete($form_state['values']['profile_id']);
  drupal_set_message(t('%profile_id Profile Deleted', array('%profile_id' => $form_state['values']['profile_id'])));
  menu_rebuild();
  drupal_goto(IMPORT_HTML_ADMIN_PATH . '/profile');
  return;
}

/**
 * Form button handler
 */
function import_html_profile_export_button($form, &$form_state) {
  $profile_id = $form_state['values']['profile_id'];
  drupal_goto(IMPORT_HTML_ADMIN_PATH . "/profiles/$profile_id/export");
}

/**
 * Export a profile and display it in a form.
 *
 * Menu callback.
 *
 * Support for ctools/exportable/features
 *
 * Returns a form containing text dump of PHP code.
 */
function import_html_profile_export_page($profile) {
  drupal_set_title($profile['profile_id']);
  $code = import_html_profile_export($profile);
  return drupal_get_form('ctools_export_form', $code, check_plain($profile['profile_id']));
}


/**
 * FAPI handler. Handle incidental settings when submitted from other pages -
 * like during the import process.
 */
function import_html_subsettings_submit($form, &$form_state) {
  $profile = import_html_profile_load($form_state['values']['profile_id']);
  $variables = array(
    'profile_id',
    'import_html_source_siteroot',
    'import_html_current_subsection',
    'import_site_prefix',
    'import_category',
    'import_status',
    'import_promote',
    'import_user',
    'create_menus',
    'menu_parent_id',
  );
  foreach ($variables as $v) {
    if (isset($form_state['values'][$v]) && ($profile[$v] != $form_state['values'][$v])) {
      $profile[$v] = $form_state['values'][$v];
    }
  }

  import_html_profile_save($profile);
}

/**
 * Handle the profile switcher (a button on phase 1 of import_html_importprocess_form)
 */
function import_html_profile_switcher_submit($form, &$form_state) {
  import_html_current_profile_id($form_state['values']['profile_id']);
}



///////////////////////////////////////////
// IMPORT PROCESS
///////////////////////////////////////////

/**
 * A multi-part 'wizard' style form.
 * Step 1 - intro and set parameters
 * Step 2 - display files and select them
 * - submit that to run the import
 * Step 3 - display summary of results.
 *
 * Parameters and return as per usual with FAPI
 */
function import_html_importprocess_form($form, &$form_state) {
  $form = array();
  $steps = 3;

  // Initialize multistep
  // D6 multistep example from
  // http://www.andrewyager.com/content/view/51/27/
  if (empty($form_state['storage']['step'])) {
    // we are coming in without a step, so default to step 1
    $form_state['storage']['step'] = 1;
  }

  $form['title'] = array(
    '#type' => 'markup',
    '#markup' => t('<h3>Step %step of %steps</h3>', array(
        '%step' => $form_state['storage']['step'],
        '%steps' => $steps
    ))
  );

  $profile_id = isset($form_state['values']['profile_id']) ? $form_state['values']['profile_id'] : import_html_current_profile_id();
  // Fetching an unknown profile will return default settings anyway.
  // However, it shouldn't happen
  $profile = import_html_profile_load($profile_id);
  if (! $profile) {
    // Fill in defaults (should only be needed first time, if that.)
    $profile = import_html_profile_defaults();
  }

  $import_html_profiles = variable_get('import_html_profiles', array());
  #if (count($import_html_profiles) > 1) {
    // Display this on every page - help knowing just what's happening
    $form['profile_info'] = array(
      '#type' => 'markup',
      '#prefix' => '<h4>', '#suffix' => '</h4>',
      '#markup' => t("Using %profile_id profile", array('%profile_id' => $profile['profile_id'])),
    );
  #}

  $form['profile_id'] = array(
    '#type' => 'hidden',
    '#default_value' => $profile_id,
  );
  // In fact, set this as a global preference so it will always be used from now on
  variable_set('import_html_current_profile', $profile_id);

  // Note the current context and state info as the rendering functions and submit process needs to know
  if (isset($form_state['values'])) {
    $form['source_siteroot'] = array(
      '#type' => 'hidden',
      '#default_value' => $form_state['values']['source_siteroot'],
    );
    $form['current_subsection'] = array(
      '#type' => 'hidden',
      '#default_value' => $form_state['values']['current_subsection'],
    );
  }

  // Retrieve the last submitted values from storage to re-populate this form
  if ( isset($form_state['storage']['values'][$form_state['storage']['step']])) {
    $form_state['values'] = array_merge($form_state['storage']['values'][$form_state['storage']['step']], $form_state['values']);
  }

  if (! isset($profile['source_siteroot']) ) {
    $profile['source_siteroot'] = variable_get('import_html_source_siteroot', '/var/www/old');
  }
  if (! isset($profile['current_subsection']) ) {
    $profile['current_subsection'] = variable_get('import_html_current_subsection', '');
  }

  switch ($form_state['storage']['step']) {

    case 1:
      // Display current options and path prior to filesystem scan
      $form['step1'] = import_html_select_source_form($form_state, $profile);

      // Don't confuse the issue with choices if there is only one profile.
      if (count($import_html_profiles) > 1) {
        $profile_options = array_combine(array_keys($import_html_profiles), array_keys($import_html_profiles));
        $form['profile_id'] = array(
          '#type' => 'select',
          '#title' => 'Import settings profile',
          '#options' => $profile_options,
          '#default_value' => $profile_id,
        );
        $form['#submit'][] = 'import_html_profile_switcher_submit';
      }

      $form['summary'] = array(
        '#type' => 'markup',
        '#markup' => import_html_profile_summary($profile)
      );

      if (!module_exists('path')) {
        drupal_set_message(t("The <b>path</b> module is not enabled! If you try to import pages without paths, I can do it, but links won't work and you'll find it hard to find the pages again.'"), 'warning');
      }
      break;

    case 2:

      // Create the file selection form - list all files.
      // The function will automatically limit itself to sane volumes on huge
      $form['step2'] = import_html_list_filesystem($form_state['values'], $profile);

      $form['action'] = array(
        '#type' => 'markup',
        '#prefix' => '<p>',
        '#suffix' => '</p>',
        '#markup' => t("Either run the import now, or queue the selected files for processing later - may be required for large jobs"),
      );

      // If these buttons are to do their thing and ALSO make an effect on the multistep
      // and proceed, they need to have both behaviours attached as #submit actions.
      $form['import'] = array(
        '#type' => 'submit',
        '#value' => t('Import Files'),
        '#submit' => array(
          'import_html_import_files_submit',
          'import_html_importprocess_form_next_button_submit',
        ),
      );
      $form['batch'] = array(
        '#type' => 'submit',
        '#value' => t('Batch Process Files'),
        '#submit' => array(
          'import_html_batch_import_files_submit',
          'import_html_importprocess_form_next_button_submit',
        ),
      );

      break;

    case 3:
      // Files have been selected and submitted.

      // To keep the state information fresh, (forward/back testing)
      // Make a quiet note of the recently selected checkboxes
      $import_files = (array) $form_state['values']['file_rel_path'];
      $selected_files = array_filter($import_files);
      $form['file_rel_path'] = array(
        '#type' => 'item',
        '#tree' => TRUE,
      );
      foreach ($selected_files as $selected) {
        $form['file_rel_path'][$selected] = array(
          '#type' => 'hidden',
          '#value' => $selected,
        );
      }

      // Display recent logged results
      $form['results'] = array(
        '#type' => '#markup',
        '#markup' => import_html_results(WATCHDOG_NOTICE),
      );

      break;
  }


  // Show step buttons.
  if ($form_state['storage']['step'] > 1) {
    $form['previous'] = array(
      '#type' => 'submit',
      '#value' => t('<< Previous'),
      '#submit' => array('import_html_importprocess_form_previous_button_submit'),
    );
  }
  if ($form_state['storage']['step'] < $steps -1) {
    $form['next'] = array(
      '#type' => 'submit',
      '#value' => t('Next >>'),
      '#submit' => array('import_html_importprocess_form_next_button_submit'),
    );
  }
  return $form;

}

/**
 * Handle each step of the import process
 * - by passing the form submit down to per-page handlers
 * , actually subforms with their own identity.
 */
function import_html_importprocess_form_validate($form, &$form_state) {
  // Massage the input to be forgiving and figure what we really mean
  // Note these transient settings as variables.
  // It's more convenient if they are remembered like this
  if ( isset($form_state['values']['source_siteroot']) ) {
    $form_state['values']['source_siteroot'] = ensure_trailing_slash(foreslash($form_state['values']['source_siteroot'])); // win32 safe.
    variable_set('import_html_source_siteroot', $form_state['values']['source_siteroot']);
  }
  if ( isset($form_state['values']['current_subsection']) ) {
    $current_subsection = ensure_trailing_slash($form_state['values']['current_subsection']);
    $current_subsection = ltrim($current_subsection, '/');
    $form_state['values']['current_subsection'] = $current_subsection;
    variable_set('import_html_current_subsection', $current_subsection);
  }
  // Other actual validation is handled by individual #element_validate calls
  // @see import_html_source_siteroot_validate()
}

/**
 * This function probably never runs. Remove it when all is well with the
 * multistep.
 *
 * Action buttons within import_html_importprocess_form have their own handlers!
 *
 * The actual process that runs at the end of this form is
 * @see import_html_import_files_submit()
 */
function import_html_importprocess_form_submit($form, &$form_state) {
  // I don't do anything useful.
}

/**
 * Multistep navigation button press handler function. Stand-alone way of
 * working with a multistep form.
 */
function import_html_importprocess_form_next_button_submit($form, &$form_state) {
  // Multistep: save the values for the current submitted step into the storage array
  $form_state['storage']['values'][$form_state['storage']['step']] = $form_state['values'];
  $form_state['storage']['step']++;
//  Multistep: Tell Drupal we are redrawing the same form
  $form_state['rebuild'] = TRUE;
  // do other things the form may have been expecting to happen
  foreach ((array) $form['#submit'] as $submit_func) {
    $submit_func($form, $form_state);
  }
}

/**
 * Multistep navigation button press handler function. Stand-alone way of
 * working with a multistep form.
 */
function import_html_importprocess_form_previous_button_submit($form, &$form_state) {
  // Multistep: save the values for the current submitted step into the storage array
  $form_state['storage']['values'][$form_state['storage']['step']] = $form_state['values'];
  $form_state['storage']['step']--;
//  Multistep: Tell Drupal we are redrawing the same form
  $form_state['rebuild'] = TRUE;
}

/**
 * Part 1 of the multistep import ptocess - a form where you can
 * choose where to get input from.
 */
function import_html_select_source_form(&$form_state, $profile) {
  $form = array();

  $form['help'] = array(
    '#type' => 'markup',
    '#markup' => t("<p>
      For an intro, try <a href='!demo_link'>a quick demo</a>.
    </p><p>
      The source website files must be directly available to this server.
      This process will NOT yet import the structure of a client website on a remote server,
      or spider all the resources of a remote site.
    </p><p>
      <b>Note</b> Big sites <i>will</i> take a long time to process,
      and processing may timeout.
      You can either increase the php timeout value and wait around,
      or just do sections at a time by using the 'subsection' parameter.
    </p>",
    array(
      '!demo_link' => url(IMPORT_HTML_ADMIN_PATH . '/demo'),
    )
  ),
  );


  $form['htmlsource'] = array(
    '#type' => 'fieldset',
    '#title' => t("Select HTML file source"),
  );

  $form['htmlsource']['source_siteroot'] = array(
    '#type' => 'textfield',
    '#title' => t("Site Root on the Server"),
    '#default_value' => $profile['source_siteroot'],
    '#description' => t("
      Where to read from.
      <br/><b>If the files are on the server</b> (local to Drupal)
      Enter the absolute or drupal-root-relative location of the site to import.
      You must have access permissions.
      Relative paths and aliases will be calculated from here.
      <br/>eg: <code>/var/www/old_site/htdocs</code>, or <code>sites/default/files/copy-of-old-site</code>
      <br/>You can also run it on: <code>!example_dir</code> for a test.
    ", array('!example_dir' => drupal_get_path('module', 'import_html') . '/tests/example_pages')
    ),
    '#element_validate' => array('import_html_validate_trailing_slash', 'import_html_source_siteroot_validate'),
  );

  $form['htmlsource']['current_subsection'] = array(
    '#type' => 'textfield',
    '#title' => t("Subsection to list"),
    '#default_value' => $profile['current_subsection'],
    #'#size' => 100,
    '#description' => t("
      For large sites, its more convenient to just list and process sections.
      If a subdirectory is specified, only that will be displayed."
    ),
    '#element_validate' => array('import_html_validate_trailing_slash'),
  );

  // Set a callback so these settings get saved generically, persistantly
  $form['#submit'][] = 'import_html_subsettings_submit';

  return $form;
}

/**
 * Check the submitted source path and subsection are valid
 *
 * A FAPI #element_validate callback invoked on $form['source_siteroot']
 *
 * http://api.drupal.org/api/file/developer/topics/forms_api_reference. html#element_validate
 *
 */
function import_html_source_siteroot_validate($element, &$form_state) {
  $working_path = ensure_trailing_slash($form_state['values']['source_siteroot']) . $form_state['values']['current_subsection'];
  if (! is_dir($working_path)) {
    form_error($element, t("Directory %dir does not exist or is unreadable. Check the path and permissions.", array('%dir' => $working_path)));
    return;
  }
}

/**
 * Form element validator. Ensure the value ends with /
 */
function import_html_validate_trailing_slash($element, &$form_state) {
  form_set_value($element, ensure_trailing_slash($element['#value']), $form_state);
}


/**
 * Given a local filepath, display all the files I can find in it.
 *
 * This will automatically give up and reduce depth if the scan starts getting
 * out of control.
 *
 * Exclude hidden files and directories ( starting with "." or "_" ) .
 * Guess what type of content they are
 * (html, resources like images and styles or other)
 * based on suffix.
 * Enable handy multi-selections to enhance the form
 *
 * A form api form def used in step 2 of import_html_page()
 *
 * TODO - need another algorithm if dealing with tens of thousands of folders
 *
 * @param  $form_values array including 'source_siteroot' and
 * 'current_subsection'
 *
 * @param $profile array including file exclusion patterns and scanning rules
 *
 * @param $max_depth int The number of folders it's permitted to recurse.
 *
 * @return Drupal form
 */
function import_html_list_filesystem($form_values, $profile, $max_depth = 0) {

  // Paranoia.
  if (empty($form_values['source_siteroot'])) {
    trigger_error('import_html_list_filesystem called without a valid source_siteroot path. args were: ' . print_r($form_values, 1), E_USER_WARNING);
    return NULL;
  }

  $source_siteroot = $form_values['source_siteroot'];
  $current_subsection = $form_values['current_subsection'];

  drupal_set_message(t("
    Listing contents of %source_siteroot %current_subsection ; %max_depth
    ", array(
      '%source_siteroot' => $source_siteroot,
      '%current_subsection' => empty($current_subsection) ? '' : '[' . $current_subsection . ']',
      '%max_depth' => empty($max_depth) ? 'recursive' : 'maximum recursion depth:' . $max_depth,
    )
  ));
  $working_path = $source_siteroot . $current_subsection;
  $form = array();

  if (! $max_depth) {
    $max_depth = 4;
  }

  $file_scan_options = array(
    'nomask' => import_html_file_exclusions_to_regexp($profile['file_exclusions']),
    'recurse' => TRUE,
    'max_depth' => $max_depth,
  );
  $dir_structure = import_html_file_scan_directory(trim_trailing_slash($working_path), "@.*@", $file_scan_options);
  $count_files = count($dir_structure);

  // If there are too many files than can reasonably be shown, need to reduce the recursion depth
  if ($count_files > variable_get('import_html_max_file_listing_count', 500)) {
    $max_depth--;
    if ($max_depth < 1) {
      // Way too many items to list
      drupal_set_message(t("
        Too many items to list in the folder structure at this level.
        You must just select the folder and hope it works.
      "), 'warning');

      $count_files = count(file_scan_directory(trim_trailing_slash($working_path), "/.*/"));
      $dir_structure = array($working_path => (object) array(
          'filename' => $working_path,
          'basename' => basename($working_path),
          'child_count' => $count_files,
        ));
    }
    else {
      // Too many items found, but depth isn't exhausted yet.
      // Call myself with a new depth limit.
      drupal_set_message(t("Too many items (%count_files) found at %max_depth.
        Reducing recursion depth and trying again", array(
          '%count_files' => $count_files,
          '%max_depth' => $max_depth,
        )));
      // RECURSION HERE
      return import_html_list_filesystem($form_values, $profile, $max_depth);
    }
  }
  // If I got here, then the self-recusion above didn't get to a satisfactory
  // result.
  if ($max_depth != 4) {
    // Only complain if we actually encountered the need to truncate the recursion.
    drupal_set_message(t("
      Because the full file listing contains more than %max files,
      it is unreasonable to show all files in a browser treeview.
      The recursion depth has therefore been reduced to %max_depth.
      Content below that level can be selected in bulk
      by selecting the appropriate parent directory.
      This limit
        (which is trying to prevent your browser from being overwhelmed)
      can be adjusted if needed in the !settings_link.
      You may also try changing the 'current subsection'
      to be more specific and just show files at that level.
    ", array(
      '%max' => variable_get('import_html_max_file_listing_count', 500),
      '!settings_link' => l('global import_html settings', 'admin/structure/import_html/setup'),
      '%count' => $count_files,
      '%max_depth' => $max_depth,
    )
    ));
  }
  // Ensure that parents and files get processed before children
  uksort($dir_structure, '_import_html_sort_files');

  // file_scan_directory returns a flat array. Convert it into a tree structure, then render it.
  $tree = import_html_sort_list_into_tree($dir_structure, $source_siteroot, $profile);

  // Note the current context and state info as the rendering functiona and submit process needs to know
  $form['file_count'] = array(
    '#type' => 'value',
    '#value' => count($dir_structure),
  );

  $dest_file_root = url($profile['file_storage_path'], array('absolute' => TRUE));
  $dest_virtual_path = url($profile['import_site_prefix'], array('absolute' => TRUE));

  // UI thingie. Actual goodies get added in script client-side
  // @see filetype_selectors.js
  $form['selectors'] = array(
    '#weight' => -2,
    '#markup' => "<p id='import-html-selectors'>Select files to import<br/></p>",
  );

  // Need to convert the sructured tree into a structured form
  $tree_form = _import_html_tree_to_form($tree, "", "/", $form_values);

  $form['filesystem'] = $tree_form;

  $form['#attached']['js'][] = drupal_get_path('module', 'import_html') . '/toggle_treeview.js';
  $form['#attached']['js'][] = drupal_get_path('module', 'import_html') . '/filetype_selectors.js';
  $form['#attached']['css'][] = drupal_get_path('module', 'import_html') . '/treeview.css';

  drupal_add_css(drupal_get_path('module', 'import_html') . '/treeview.css');;

  $form['#theme'] = 'import_html_list_filesystem_tree';

  return $form;
}



/**
 * The 'import files' button was pressed on the 'select files' form.
 * Go do it!
 */
function import_html_import_files_submit($form, $form_state) {
  variable_set('import_html_last_import_timestamp', REQUEST_TIME);

  $import_files = isset($form_state['values']['file_rel_path']) ? (array) $form_state['values']['file_rel_path'] : array();
  // All the checkboxes returned as keys, but only those selected need be processed.
  $selected_files = array_filter($import_files);

  // Glob out the recursive folders now.
  // This will create a huge list, but at least we know what we are doing.
  // globbing later may be lighter for batch jobs, but is unpredictable.
  $import_dirs = isset($form_state['values']['file_rel_dir']) ? (array) $form_state['values']['file_rel_dir'] : array();
  $selected_dirs = array_filter($import_dirs);
  $files_in_dirs = import_html_scan_rel_dir($selected_dirs, $form_state['values']);
  if (! empty($files_in_dirs)) {
    drupal_set_message("Globbed an additional " . count($files_in_dirs) . " files from " . count($selected_dirs) . " subdirectories");
    $selected_files = array_merge($selected_files, $files_in_dirs);
  }

  $results = array();
  $count = count($selected_files);
  import_html_debug("START: About to import ($count) items from the form submission");

  if (count($selected_files)) {
    $results = import_html_import_files($selected_files, $form_state['values']);
  }
  else {
    drupal_set_message(t("No files or folders selected for import"), 'warning');
  }

  drupal_set_message(t("Imported %count items", array('%count' => count($results))));
  return $results;
}

/**
 * A successor to import_html_import_files_submit.
 *
 * Does the same thing, using the Drupal batch process.
 */
function import_html_batch_import_files_submit($form, $form_state) {
  variable_set('import_html_last_import_timestamp', REQUEST_TIME);

  $profile_id = $form_state['values']['profile_id'];
  $profile = import_html_profile_load($profile_id);

  $import_files = (array) @$form_state['values']['file_rel_path'];
  $selected_files = array_filter($import_files);

  $import_dirs = (array) @$form_state['values']['file_rel_dir'];
  $selected_dirs = array_filter($import_dirs);

  // Data from this submission that the batch process will need to know
  // Keep it simple - this data gets passed around a bit
  $context = array(
    'profile_id' => $form_state['values']['profile_id'],
    'source_siteroot' => $form_state['values']['source_siteroot'],
    'form_id' => $form_state['values']['form_id'],
  );

  // Substitutions used for messages
  $info = array(
    '%count_files' => count($selected_files),
    '%count_dirs' => count($selected_dirs),
    '!context' => print_r($context, 1),
  );

  if ($profile['recursion_behaviour'] == IMPORT_HTML_GLOB_BEFORE) {
    // Glob out the recursive folders now.
    // This will create a huge list, but at least we know what we are doing.
    // globbing later may be lighter for batch jobs, but is unpredictable.
    // TODO look at queuing dirs
    $files_in_dirs = import_html_scan_rel_dir($selected_dirs, $form_state['values']);
    $info['%count_globbed_files'] = count($files_in_dirs);
    if (! empty($files_in_dirs)) {
      drupal_set_message(t("Globbed an aditional %count_globbed_files files from %count_dirs subdirectories", $info));
      $selected_files = array_merge($selected_files, $files_in_dirs);
    }
  }

  // Before adding things to the batch, ensure that parents and index pages get processed before children
  uasort($selected_files, '_import_html_sort_files');

  import_html_debug("Queuing Batch process of %count_files selected files, %count_dirs dirs", $info, WATCHDOG_INFO );

  import_html_batch_add_list_to_queue($selected_files, $context);

  if (($profile['recursion_behaviour'] == IMPORT_HTML_RECURSE_AFTER) && !empty($selected_dirs)) {
    // Also batch queue folders - individual batches
    $batch_instructions = import_html_import_files_batch_instructions($selected_dirs, $context, 'import_html_import_directory_batch_job');
    batch_set($batch_instructions);
  }

  import_html_debug("Batch process (%count_dirs dirs, %count_files files) items) is queued", $info, WATCHDOG_INFO);
}

/**
 * Ensure that parents and index pages get processed before children.
 * A uasort callback.
 *
 * @param unknown_type $a
 * @param unknown_type $b
 */
function _import_html_sort_files($a, $b) {
  $a_exploded = explode('/', $a);
  $b_exploded = explode('/', $b);
  if (count($a_exploded) == count($b_exploded)) {
    $a_filename = array_pop($a_exploded);
    $a_dirname = array_pop($a_exploded);
    $b_filename = array_pop($b_exploded);
    $b_dirname = array_pop($b_exploded);

    if ($a_dirname == $b_dirname) {
      if ($a_filename == 'index.html') {
        return -1;
      }
      else if ($b_filename == 'index.html') {
        return 1;
      }
      else {
        return strcmp($a_filename, $b_filename);
      }
    }
    else {
      return strcmp($a_dirname, $b_dirname);
    }
  }
  else {
    return (count($a_exploded) < count($b_exploded)) ? -1 : 1;
  }
}


/**
 * From a flat list of file defs, build a tree structure, and annotate it like
 * the forms API does.
 *
 * @param $dir_structure list of file names and file details, as returned from
 * file_scan_directory.
 *
 * @return an array structure that is almost, but not quite a FAPI form
 * definition.
 */
function import_html_sort_list_into_tree($dir_structure, $source_siteroot, $profile) {
  $file_exclusions = $profile['file_exclusions'];

  $tree = array();
  $count = 0;
  foreach ($dir_structure as $file_path => $file_info) {
    $rel_path = substr($file_path, strlen($source_siteroot));

    $ancestors = preg_split("/[\/\:]+/", $rel_path);
    $twig = & $tree; // find the current twig, starting from the top
    $path = '';
    while ($dad = array_shift($ancestors)) {
      // walk down the path
      if (!array_key_exists($dad, $twig)) {
        // add new branch
        $twig['#type'] = 'tree_branch';
        $twig['#value'] = $path;
        $twig['#description'] = "";
        $twig['#filename']   = $source_siteroot . $path;
        // dirs come below files
        $twig['#weight'] = 10;
        $twig[$dad] = array();
        $count++;
      }
      $path .= $dad . '/';
      $twig = & $twig[$dad];
    }
    $twig['#type']       = is_dir($file_path) ? 'tree_branch' : 'tree_item';
    $twig['#description'] = basename($file_path); // $file_info->basename;
    $twig['#filename']   = $file_path;
    $twig['#value']      = $rel_path;
    // Ensure default docs appear at the top
    if (in_array(basename($file_path), $profile['default_documents'])) {
      $twig['#is-default'] = TRUE;
      $twig['#weight'] = -10;
    }
    // Misc data that MAY have been set when we scanned.
    if (! empty($file_info->child_count)) {
      $twig['#description'] = $file_info->child_count . " items";
    }
  }
  return $tree;
}


/**
 * Recursive form construction function to format a tree-like structure
 * of heirachical filenames into a Form API nested tree of checkboxes.
 *
 * The function is self-referentially recursive - depth-first formatting of a tree menu.
 *
 * @param $tree array The tree layout so far;
 * @param $name string The human name of the item were are adding right now
 * @param $prefix string The path so far;
 *
 * @return form object
 */
function _import_html_tree_to_form($tree, $name, $prefix = "", $form_values ) {
  $path = (($prefix != "/") ? $prefix . "/" : "") . $name;
  $id = "ID_" . preg_replace("/[^\w]/", "", $path);
  // $id is a safe name - a unique id derived from the item path
  // -- OK, these are becoming stupidly long - any reason not to just use a counter?

  // Although the form is presented in a tree, the form values are to come back in a list.
  //
  // To sorta-but-not-really 'tree' and allow multiple values for identically named checkboxes,
  // we keep a running list of numbers.
  // We cheat by setting the '#parents' manually before rendering, and using a counter of our own
  static $checkbox_count;
  $element = array();

  if ($tree['#type'] == 'tree_branch') {
    // tree_branch is our own pseudo form element item
    // I wanted to re-use 'fieldset' and just render it specially myself
    // but that was not over-ridable.
    // This element uses theme_tree_branch() when it is encountered in drupal_render

    $element = array(
      '#type' => 'tree_branch',
      '#title' => $tree['#value'],
      '#attributes' => array('class' => array('tree-branch')),
      '#theme' => 'tree_branch',
      '#description' => isset($tree['#description']) ? $tree['#description'] : '',
    );

    // checkbox toggle for the children of this item
    //
    // If this was a folder that displays children,
    // the children will be submitted as individual entries in the UI
    // this means that we can ignore its state for the purposes of importing,
    // otherwise we would queue both the folder (recursive) and its children (individuals)
    // which would produce double-ups
    // Give the checkbox a different form element name.
    $checkbox_name = element_children($tree) ? 'file_rel_dir_ui_only' : 'file_rel_dir';
    $element['file_rel_dir'] = array(
      '#type' => 'checkbox',
      '#id' => "${id}-toggle",
      '#item-type' => 'container',
      '#return_value' => $tree['#value'],
      '#default_value' => @$form_values['file_rel_dir'][$tree['#value']],
      '#attributes' => array('class' => array('tree-branch-toggle')),
      // Need to allow multiples, but they are essentially flat, not tree-d
      '#parents' => array($checkbox_name, $tree['#value']),
      '#weight' => isset($tree['#weight']) ? $tree['#weight'] : 10,
    );
  }
  else {
    // Normal file
    $element = array(
      '#type' => 'checkbox',
      '#title' => $tree['#description'],
     # '#theme' => 'import_html_filesystem_tree_item',
      // I need to put a class around my items.
      // Normal ceckboxes get wrapped with theme form_element
      // Do my own instead, so I can insert class attributes
      '#theme_wrappers' => array('import_html_file_selector_wrapper'),
      '#id' => "${id}-checkbox",
      '#attributes' => array('class' => array('tree-leaf-toggle')),
      '#default_value' => @$form_values['file_rel_path'][$tree['#value']],
      '#return_value' => $tree['#value'],
      // Need to allow multiples, but they are essentially flat, not tree-d
      '#parents' => array('file_rel_path', $tree['#value']),
      '#weight' => isset($tree['#weight']) ? $tree['#weight'] : 0,
    );
  }
  $item_type = ($tree['#type'] == 'tree_branch') ? "container" : import_html_guess_file_class($tree['#filename']);
  // Sorta want to be able to have a select-all
  if (empty($item_type)) {
    $item_type = 'unknown';
  }

  // Special case:  if name is blank (we are at root) show the working dir.
  if (! $tree['#value']) {
    $item_type = 'tree-root';
    $element['#title'] = $form_values['source_siteroot'];
  }

  // I want to get a class into the label, but theme_form_element_label
  // won't let me.
  // This is forced by the use of our custom theme-wrapper func instead.
  $element['#attributes']['class'][] = $item_type . '-item';

  // Highlight detected 'default' docs
  if (!empty($tree['#is-default'])) {
    $element['#attributes']['class'][] = 'is-default';
  }

  // Construct the content

  if (element_children($tree)) {
    // This node has content to be recursed into. Add it as a child
    $element['children'] = array();
    // Just a container array. Themed later
    $element['children']['#id'] = "${id}-content";
    $element['children']['#attributes']['class'] = "tree-content";
    foreach ($tree as $twigname => $twig) {
      if (element_property($twigname)) {
        continue;
      }
      $element['children']['file_rel_path'][] = _import_html_tree_to_form($twig, $twigname, $path, $form_values);
    }
  }
  return $element;
}

/**
 * Main filesystem select container. Renders the treeview of the files found in
 * the filesystem, with form extras.
 */
function theme_import_html_list_filesystem_tree($variables) {
  $element = $variables['form'];
  return drupal_render_children($element);
}


/**
 * Replace theme_form_element (the wrapper around theme_checkbox) so I could
 * get my CLASSES into the div that goes around checkboxes.
 *
 * This is a very cut down version of
 * @see theme_form_element()
 * but ADDS a useful class
 *
 * @param unknown_type $variables
 * @return string
 */
function theme_import_html_file_selector_wrapper($variables) {
  $element = &$variables['element'];

  // Add element's #type and #name as class to aid with JS/CSS selectors.
  $attributes['class'] = array('form-item');
  if (!empty($element['#type'])) {
    $attributes['class'][] = 'form-type-' . strtr($element['#type'], '_', '-');
  }
  // Add a class for disabled elements to facilitate cross-browser styling.
  if (!empty($element['#attributes']['disabled'])) {
    $attributes['class'][] = 'form-disabled';
  }
  // MY BIT
  foreach ($element['#attributes']['class'] as $class) {
    $attributes['class'][] = $class . '-wrapper';
  }

  $output = '<div' . drupal_attributes($attributes) . '>' . "\n";

  $prefix = isset($element['#field_prefix']) ? '<span class="field-prefix">' . $element['#field_prefix'] . '</span> ' : '';
  $suffix = isset($element['#field_suffix']) ? ' <span class="field-suffix">' . $element['#field_suffix'] . '</span>' : '';

  $output .= ' ' . $prefix . $element['#children'] . $suffix;
  $output .= ' ' . theme('form_element_label', $variables) . "\n";

  // Also add a link to the old file
  $output .= ' ' . l('&seArr;', IMPORT_HTML_ADMIN_PATH . '/file_preview/' . $element['#return_value'], array('html' => TRUE, 'title' => 'View old file'));

  if (!empty($element['#description'])) {
    $output .= '<div class="description">' . $element['#description'] . "</div>\n";
  }

  $output .= "</div>\n";
  return $output;
}



/**
 * filesystem tree container is a fieldset, contains one toggle checkbox and a collection of 'children'
 * If you want the normal rendering back (description, value) put it in yourself.
 */
function theme_tree_branch($variables) {
  $element = $variables['element'];
  $toggle_button = $element['file_rel_dir'];
  $checked = $toggle_button['#value'] ? ' checked="checked" ' : ' ';
  $atts = drupal_attributes($toggle_button['#attributes']);
  $item_checkbox = "<input type='checkbox' name='{$toggle_button['#name']}' id='{$toggle_button['#id']}' value='{$toggle_button['#return_value']}' $checked $atts />";
  $item = "<label>$item_checkbox {$element['#title']}</label>";

  $children = empty($element['children']) ? '' :
    '<div' . drupal_attributes($element['children']['#attributes']) . '>'
    . drupal_render_children($element['children'] )
    . '</div>'
    ;

  return '<fieldset' . drupal_attributes($element['#attributes']) . '>'
  . "<legend class=\"container-label\">{$item}</legend>"
  . (isset($element['#description']) && $element['#description'] ? '<div class="description">' . $element['#description'] . '</div>' : '')
  . $children
  . (isset($element['#value']) ? $element['#value'] : '')
  . "</fieldset>\n";
}

/**
 * Return a table listing the results from the watchdog.
 *
 * By default it's quite noisy - at the info level.
 */
function import_html_results($severity = NULL) {
  if (! module_exists('dblog')) {
    drupal_set_message('Logging requires the core dblog module (Database logging) to be enabled', 'warning');
    return 'No logs';
  }

  if (empty($severity)) {
    $severity = variable_get('import_html_debug_level', WATCHDOG_INFO);
  }
  drupal_add_css(drupal_get_path('module', 'dblog') . '/dblog.css');

  $levels = watchdog_severity_levels();
  $last_import_timestamp = variable_get('import_html_last_import_timestamp', 0);

  $output = "Results from the last import process - " . format_date($last_import_timestamp);
  $output .= t( " (Messages up to %severity. !more | !less)", array(
    '%severity' => $levels[$severity],
    '!more' => l('more', IMPORT_HTML_ADMIN_PATH . '/results/' . min(($severity + 1), 7)),
    '!less' => l('less', IMPORT_HTML_ADMIN_PATH . '/results/' . max(($severity -1), 0)),
    )
  );

  // We will use the watchdog to do most of our logging - but then have to retrieve it ourself as the watchdog reports are tedious.
  // Code adapted from dblog.admin.inc
  $classes = array(
    WATCHDOG_DEBUG     => 'dblog-debug',
    WATCHDOG_INFO      => 'dblog-info',
    WATCHDOG_NOTICE    => 'dblog-notice',
    WATCHDOG_WARNING   => 'dblog-warning',
    WATCHDOG_ERROR     => 'dblog-error',
    WATCHDOG_CRITICAL  => 'dblog-critical',
    WATCHDOG_ALERT     => 'dblog-alert',
    WATCHDOG_EMERGENCY => 'dblog-emerg',
  );

  // Would be good to hijack dblog_build_filter_query()
  // by putting args into the $_SESSION, but can't insert a timestamp comparison
  // into its query builder. do it by hand.

  // @see dblog_overview()
  $header = array(
      '', // Icon column.
      array('data' => t('Type'), 'field' => 'w.type'),
      #array('data' => t('Date'), 'field' => 'w.wid', 'sort' => 'desc'),
      t('Message'),
      array('data' => t('User'), 'field' => 'u.name'),
      array('data' => t('Operations')),
  );


  $query = db_select('watchdog', 'w')->extend('PagerDefault')->extend('TableSort');
  $query->fields('w',
    array('wid', 'uid', 'severity', 'type', 'timestamp', 'message', 'variables', 'link')
  );
  $query->condition('w.type', 'Import HTML');
  $query->condition('w.timestamp', $last_import_timestamp, '>=');
  $query->condition('w.severity', $severity, '<=');

  $result = $query
    ->limit(50)
    ->orderByHeader($header)
    ->execute();
  $rows = array();
  foreach ($result as $dblog) {
    $rows[] = array('data' =>
      array(
        // Cells
        array('class' => 'icon'),
        t($dblog->type),
        #format_date($dblog->timestamp, 'short'),
        array(
          'class' => 'message',
          'data' => t($dblog->message, unserialize($dblog->variables))
        ),
        # theme('dblog_message', array('event' => $dblog, 'link' => TRUE)),
        theme('username', array('account' => $dblog)),
        filter_xss($dblog->link),
      ),
      // Attributes for tr
      'class' => array(drupal_html_class('dblog-' . $dblog->type), $classes[$dblog->severity]),
    );
  }
  $build['legend'] = array(
    '#markup' => $output,
  );
  $build['dblog_table'] = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
    '#attributes' => array('id' => 'admin-dblog'),
    '#empty' => t('No log messages available.'),
    );
  $build['dblog_pager'] = array('#theme' => 'pager');
  $build['#attached']['css'][] = drupal_get_path('module', 'import_html') . '/import_html-dblog.css';
  return $build;

}

/////////////////////////////////////////////
// DEMO
/////////////////////////////////////////////



/**
 * Show an interface to import just one file
 */
function import_html_demo_form($form, &$form_state) {
  $form = array();

  // Show extended help on the first page only.
  if (! $form_state['submitted']) {
    $form['introduction'] = array(
      '#type' => 'markup',
      '#markup' => file_get_contents(drupal_get_path("module", "import_html") . "/help/import_html_demo.html"),
    );
  }

  $form['Select'] = array(
    '#type' => 'fieldset',
    '#title' => t("Select HTML file source"),
  );
  $form['Select']['source_url'] = array(
    '#type' => 'textfield',
    '#title' => t("URL to import"),
    '#default_value' => '',
    '#maxlength' => 240,
    '#required' => TRUE,
    '#description' => t("
        Local file paths are OK, although you may have to use the
        <kbd>file://</kbd> syntax on them, depending on your system.
    "),
  );
  $import_html_profiles = variable_get('import_html_profiles', array());
  $profile_options = array_combine(array_keys($import_html_profiles), array_keys($import_html_profiles));
  $form['Select']['profile_id'] = array(
    '#type' => 'select',
    '#title' => "Import Profile",
    '#options' => $profile_options,
    '#default_value' => variable_get('import_html_current_profile', 'default'),
  );
  $form['Select']['actions'] = array('#type' => 'actions');
  $form['Select']['actions'][] = array(
    '#type' => 'submit',
    '#value' => t("Preview"),
  );

  // Show the results if it was submitted
  if ($form_state['submitted'] && $form_state['temporary']['node_form']) {
    if (function_exists('dpm') && (variable_get('import_html_debug_level', WATCHDOG_WARNING) > 0)) {
      $form['dump']['#markup'] = kpr($form_state['temporary']['node_form']['#node'], 1);
    }
    $form['results']['#type'] = 'fieldset';
    $form['results']['#title'] = 'Sample node edit form';

    // Trying to nest the form into another form will mess with it and
    // any of its logic.
    #$form['results']['node_form'] = $form_state['temporary']['node_form'];
    // Instead, pre-render it and dump the HTML inline.

    $node_edit_form = $form_state['temporary']['node_form'];
    unset($node_edit_form['actions']);
    $rendered = drupal_render($node_edit_form);
    $form['results']['node_form']['#markup'] = $rendered;

  }

  return $form;
}

/**
 * Submit handler for import_html_demo_form
 * @param $form_id
 *   a form_id
 * @param $form_values
 *   posted form values typically passed in by submit handler, but also can be from $_POST.
 *   should contain
 *     ('file_rel_path' array)
 *     relative to the submitted 'source_siteroot' (no trailing slash) .
 *     Rel paths here begin with slashes.
 */
function import_html_demo_form_submit($form, &$form_state) {
  switch ($form_state['values']['form_id']) {
    case 'import_html_demo_form':
      // Bulk imports come with context paths which are used for structure
      // Over-ride (fudge) those inputs for the demo
      $source_path = foreslash( $form_state['values']['source_url'] );

      $url_parts = parse_url($source_path);

      // Divide the path into two halves (avoiding glue_url())
      // to provide a dummy context and a rel_path
      $split_at = strrpos($source_path, $url_parts['path']) + 1;
      $form_state['values']['source_siteroot'] = $source_siteroot = substr($source_path, 0, $split_at);
      $rel_path = substr($source_path, $split_at);

      // Remmber the selected profile_id
      $profile_id = $form_state['values']['profile_id'];
      variable_set('import_html_current_profile', $profile_id);
      import_html_current_profile_id($profile_id);

      // Grab the current settings for this profile.
      $context = import_html_profile_load($profile_id);

      // Add a fake proxy directive in the 'imported' files folder that will
      // temporarily reflect the source images from the original site.
      import_html_add_fake_files_proxy($context['file_storage_path'], $source_siteroot);

      variable_set('import_html_last_import_timestamp', REQUEST_TIME);

      $import_files = array($rel_path);

      $files = import_html_import_files($import_files, $form_state['values']);

      // Take the resulting node definition from the processed array and display it in a node edit form
      // as if it were an existing node.

      if (is_array($files) && ($file = array_shift($files)) ) {
        if (empty($file['node'])) {
          drupal_set_message(t("Looks like processing %path didn't return any valid node.", array('%path' => $url_parts['path'])), 'error');
          break;
        }
        $node = $file['node'];

        // Dummy up a node edit form
        module_load_include('inc', 'node', 'node.pages');

        // Setting NODE_BUILD_PREVIEW should prevent the theme etc from borking
        // because the node has no nid. Maybe?
        # may have been d6 only problem. remove
        #$node->build_mode = NODE_BUILD_PREVIEW;

        // Previewing alters $node - so it needs to be cloned instead.
        #$cloned_node = clone $node;

        #if (function_exists('dpm') && (variable_get('import_html_debug_level', WATCHDOG_WARNING) > 0)) {
        #  dpm($node);
        #}

        #$output = theme('node_preview', array('node' => $cloned_node));

        $node_edit_form = drupal_get_form('page_node_form', $node);

        #dpm($node_edit_form);
        #$rendered = drupal_render($node_edit_form);
        // Tell the form to rebuild and therefore be able to insert my page
        // into it.
        $form_state['rebuild'] = TRUE;
        $form_state['temporary']['node_form'] = $node_edit_form;

        /*
        $form_id = $node->type . '_node_form';

        // Build form, almost like drupal_get_form, but with a fake destination
        // So it will WORK.
        // All this copied from form.inc:drupal_get_form()
        $form_state = array(
          'storage' => NULL,
          'submitted' => FALSE,
        );
        $args_temp = array($form_id, $form_state, $node);
        $form = call_user_func_array('drupal_retrieve_form', $args_temp);

        $form = drupal_rebuild_form($form_id, $form_state, array($form_id, $node));
        $form['#action'] = url('node/add/' . $node->type);
        $output .= drupal_render_form($form_id, $form);

        // Note there is also a hook in import_html_form_alter
        // that adjusts this page_node_form and makes it work.
        return $output;
        */
      }
      else {
        return t("For whatever reason, the request to %rel_path on %source_siteroot) failed to return a result. You'll have to enable debugging to find out why.", array('%rel_path' => $rel_path, '%source_siteroot' => $source_siteroot));
      }
      break;

    case 'page_node_form':
      drupal_form_submit('page_node_form', $form_state['values']);
      break;
  }
}

/**
 * Support file request for demo preview.
 *
 * This will cause any files EXPECTED to be in the imported files directory to
 * be retrieved from the real source URL on the fly.
 *
 * This should allow the demo preview to work transparently, but is not a
 * substitute for a real import, as it only redirects to the original source.
 *
 * For a real import, we may instead implement a real caching proxy that will
 * really copy the files to the import location.
 */
function import_html_add_fake_files_proxy($file_storage_path, $source_siteroot) {
  $rewrite_code = "
# Serve requests under this path as if they were from a remote source
# This file created by import_html 'demo'.
RewriteEngine On
RewriteCond %{REQUEST_FILENAME} !-f
RewriteCond %{REQUEST_FILENAME} !-d
RewriteRule ^(.*)\$ {$source_siteroot}\$1 [L]
  ";
  // Permissions on the server may be such that this write it prohibited
  // (Permission denied to write to .htaccess.
  // That's OK. Fail quietly.
  @file_put_contents($file_storage_path . '.htaccess', $rewrite_code);
}


/**
 * Menu callback to help the filebrowser UI link directly to files being
 * imported.
 * Ask for the path IMPORT_HTML_ADMIN_PATH/file_preview/{rel_path}
 *
 * Most stuff will break, this isn't a real working mirror -
 * is just to help some diagnostic previews to check you are looking at the
 * right HTML source.
 *
 * Potential for some pretty huge security hole if I don't verify a few things
 * are legal.
 */
function import_html_file_preview() {
  // We don't refer to the profile, just to the last active source_siteroot.
  $args = func_get_args();
  $source_siteroot = variable_get('import_html_source_siteroot', 'dummy');
  // Source_siteroot must always end in /
  $target_filepath = $source_siteroot . join('/', $args);

  // Beware directory traversal. It's probably hard,
  // but I won't bet it's not impossible.
  $realBase = realpath($source_siteroot);
  $realUserPath = realpath($target_filepath);
  if ($realUserPath === false || strpos($realUserPath, $realBase) !== 0) {
    // Bad path.
    return t("Bad path, %target_filepath is naughty ", array('%target_filepath' => $target_filepath));
  }
  // else Good path!


  if (file_exists($target_filepath)) {
    // Send it back to the browser. Sorry about the mime type, I don't care.
    print (file_get_contents($target_filepath));
    exit;
  }
  return t("Bad path, %target_filepath is inaccessible ", array('%target_filepath' => $target_filepath));
}

