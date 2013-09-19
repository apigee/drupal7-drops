<?php
/**
 * @file
 * Pass HTMLTidy over some inputs to make them valid XHTML
 *
 * Uses either the PHP5 extension module or the commandline binary.
 *
 * Run htmltidy_test() to try and initialize the
 * htmltidy_apppath variable, which is needed to find the 'tidy' binary.
 *
 * In the commandline version, the xhtml_tidy.conf file
 * is read from the same directory as this library.
 *
 * @package coders
 * @author Dan Morrison http://coders.co.nz/
 */

@module_load_include('inc', 'import_html', 'coders_php_library/debug');

define("TIDY_OPTIONS", " -q -config " . foreslash( dirname(__FILE__) . "/xhtml_tidy.conf ") );

/**
 * Use the tidy extension (Whee!) to fix randomly downloaded 'html'
 *
 * The result should be valid XHTML that can now be converted into a DOM
 * if the extension is unavailable, try doing it on the commandline
 * if the executable is available.
 *
 * @param $data string Raw web page source
 * @param $xhtml If set, the correct namespace will be inserted into the
 * document, and later xhtml imports will work without bogus namespace tags.
 * @return $string Cooked web page source
 */
function xml_tidy_string($data, $xhtml = TRUE) {

  $tidy_conf = dirname(__FILE__) . '/xhtml_tidy.conf';
  $tidy_method = variable_get('htmltidy_method', 'commandline');

  // Tidy
  if ($tidy_method == 'extension' && extension_loaded('tidy') ) {
    // There are two conflicting tidy extensions. v2 is PHP5 and OO
    // v1 was PHP4 and broken
    // One reads from a conf file, one only takes params

    // Specify configuration
    // The DOCTYPE syntax seems to cause complaints
    // <!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">
    // is apparently illegal ? Need to omit it. Wants space inside the end of the tag ...
    // The xml declaration also causes trouble with browser rendering.
    //

    $config = array(
      'indent' => FALSE,
      'output-xhtml' => TRUE,
      'numeric-entities' => TRUE,
      'add-xml-decl' => FALSE,
      'doctype' => 'omit',
      'char-encoding' => 'utf8',
      'wrap' => 2000,
      'repeated-attributes' => 'keep-last',
      'alt-text' => '',
      #       'drop-font-tags'   => TRUE,
    );
    if ($xhtml) {
      $config['output-xhtml'] = TRUE;
    }

    // Finally a character entity decoder that does what I needed!
    // If the source file contains (illegal) high-order bytes, convert
    // them to numeric entities. Even tidy_html wasnt doing it right.
    if (function_exists('charset_decode_utf_8')) {
      $data = charset_decode_utf_8($data);
    }

    if (class_exists('tidy')) {
      import_html_debug('Using tidy Extension (v2 OO)', array(), WATCHDOG_INFO);

      // this doesnt work. How to get conf files into tidy v2?
      // ini_set('tidy.default_config', $tidy_conf);
      $tidy = new tidy();
      $tidy->parsestring($data, $config);
      $tidy->cleanrepair();
      return (string) tidy_get_output($tidy);
    }

    if (function_exists('tidy_parse_string')) {
      // Untested
      import_html_debug('Using tidy Extension (v1, PECL)', array(), WATCHDOG_INFO);
      if (function_exists('tidy_load_config')) {
        tidy_load_config($tidy_conf);
      }
      $tidy = tidy_parse_string($data, $config);
      $tidy->cleanrepair();
      return (string) $tidy;
    }
  }

  // No Tidy Module, go commando
  if ($tidy_method != 'commandline') {
    import_html_debug("No PHP tidy support, attempting to use the command-line way to process a string.");
  }

  // Need to write, pipe & read the result.
  $temp_path =  foreslash( tempnam(sys_get_temp_dir(), "htm") );

  file_put_contents($temp_path, $data);
  if (! file_exists($temp_path)) {
    trigger_error("Failed to create/write temp file '$temp_path'");
    return FALSE;
  }
  $out = xml_tidy_file($temp_path);
  #if (! DEBUGLEVEL) {
  # unlink($temp_path);
  #}

  if ($out) {
    return $out;
  }
  import_html_debug("No html tidy support at all. Trying to tidy returned null. You need to enable the PHP module to use this, or ensure the 'tidy' executable is present and defined as TIDY_PATH and use that", array(), WATCHDOG_ERROR);
  return $data;
}


/**
 * Tidy an xml file in place
 *
 * If we don't have the tidy module, we can still try to get the effect
 * using the command-line version.
 * Remote files are supported, they are fetched into a the temp dir
 * before being processed.
 *
 * @parameter $filepath string The full filepath of the file to be tidied
 * @returns string the content of the tidied file
 */
function xml_tidy_file($filepath) {
  import_html_debug("Tidying file '!filepath' ", array('!filepath' => $filepath), WATCHDOG_INFO);

  if (! is_local($filepath)) {
    // OK, so it's a remote file and I have to process it on the command-line ...
    // Copy it down
    $source = file_get_contents($filepath);
    if (! $source) {
      import_html_debug("No content from '!filepath'", array('!filepath' => $filepath), WATCHDOG_ERROR );
      return FALSE;
    }
    import_html_debug("Retrieved remote file:'!filepath' is !length big", array('!length' => strlen($source), '!filepath' => $filepath), WATCHDOG_DEBUG);
    // put it in a temp place
    $target_path =  tempnam($_ENV['TEMP'], "htm");
    file_put_contents(foreslash($target_path), $source );
  }
  else {
    $target_path = $filepath;
  }

  if (! file_exists($target_path) ) {
    import_html_debug("Attempted to tidy a file that doesn't exist. Looking for '!target_path' failed!", array('!target_path' => $target_path), WATCHDOG_ERROR);
    return;
  }

  if (! $htmltidy_apppath = variable_get("htmltidy_apppath", 'tidy')) {
    // First run?
    // Try to autoconfigure.
    module_load_include('inc', 'import_html', 'coders_php_library/install-htmltidy');
    if ( ! htmltidy_test($message) ) {
      // Test returned an error
      import_html_debug('Unable to tidy file - htmltidy was unavailable "$message". Returning the original source.', array(), WATCHDOG_WARNING);
      return file_get_contents($target_path );
    }
  }

  // Format it.
  $command = $htmltidy_apppath . " " . TIDY_OPTIONS . '"' . foreslash($target_path) . '" ';
  // Errors are compressed into the output so I can see them.

  // TODO - check for exploits here? What could happen?
  import_html_debug("Running \n$command", array(), WATCHDOG_INFO);
  $result = exec($command, $response, $return_code);
  $out = join("\n", $response);

  if (empty($out)) {
    // There was an insoluble problem.
    // Run the exact same command again, but collect the errors this time
    $result = exec($command . ' 2>&1', $response, $return_code);
    $out = join("\n", $response);

    import_html_debug(
      "HTMLTidy failed to parse the input at all!
      It's probably very problematic HTML.
      I ran <code>\n!command\n</code>
      and htmltidy returned: !return_code <pre>!out</pre>\n",
      array('!command' => $command, '!return_code' => $return_code, '!out' => htmlspecialchars($out)),
      WATCHDOG_ERROR
    );
    $out = FALSE; // file_get_contents($target_path );
  }

  import_html_debug_code("After command-line tidy of $filepath", $out, WATCHDOG_DEBUG);

  if ($target_path != $filepath) {
    // remove temp file we just made up.
    unlink($target_path);
  }

  return $out;
}

/**
 * Tidies an incomplete fragment of HTML
 *
 * by wrapping it in a document header/footer,
 * passing it through htmltidy full,
 * then stripping the 'body'
 *
 */
function xml_tidy_fragment($input) {
  if (!$input) {
    return '';
  }

  // Pretend it's a full document. This declaration just suppresses one of the warnings.
  $declaration = '<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">';
  $output = xml_tidy_string($declaration . '<html><head><title></title></head><body>' . $input . '</body></html>');

  if (preg_match('|<body[^>]*>([\s\S]*)</body>|', $output, $matches)) {
    $output = $matches[1];
  }
  return $output;
}

/**
 * A form definition for configuring htmltidy on the server.
 *
 * Include it in a system_settings_form()
 */
function html_tidy_settings(&$form_state) {
  module_load_include('inc', 'import_html', 'coders_php_library/install-htmltidy');
  $warning = ''; // a return container handle

  $form['HTMLTidy'] = array(
    '#type' => 'fieldset',
    '#title' => 'Install HTMLTidy',
  );

  if (!htmltidy_test($warning)) {
    drupal_set_message(t('HTMLTidy executable is not available. %warning ', array('%warning' => $warning)), 'error');
  }
  else {
    $form['HTMLTidy_status'] = array(
      '#value' => t("<p>HTMLTidy is present and correct: %message </p>", array('%message' => $warning) ),
    );
    $form['HTMLTidy']['#collapsible'] = TRUE;
    $form['HTMLTidy']['#collapsed'] = TRUE;
  }

  $form['htmltidy_method'] = array(
    '#type' => 'radios',
    '#title' => t("HTML Tidy method"),
    '#options' => array('extension', 'commandline'),
    '#default_value' => variable_get('htmltidy_method', 'extension'),
    '#description' => t("Some servers don't have a working version of the tidy extension. On others it's broken. Choose the method for tidying here. The commandline method is more tested, though if the extension works it should be more eficient."),
  );

  $form['HTMLTidy']['htmltidy_apppath'] = array(
    '#type' => 'textfield',
    '#title' => t('Path to HTMLTidy executable'),
    '#default_value' => variable_get('htmltidy_apppath', '/usr/local/bin/tidy'),
    '#size' => 40,
    '#maxlength' => 100,
    '#description' => t("
      If the distributed php tidy extension is not enabled,
      we require the HTML Binary to be available on the server.
      Please <a href='http://tidy.sourceforge.net/'>download and install it</a>
      wherever you can, then tell me where to find it.
    "),
  );
  $form['HTMLTidy'][] = array(
    '#value' => t("
      <p>If you are unable to install the htmltidy binary yourself,
      try this button to download and unpack it on the server
      (Linux only so far) .</p>
    "),
  );
  $form['HTMLTidy']['install_button'] = array(
    '#type' => 'submit',
    '#value' => t('install htmltidy binary'),
    '#submit' => 'htmltidy_install_binary',
  );
  $form['HTMLTidy'][] = array(
    '#value' => t("
      <p>There is a <i>small</i> chance it will work for you,
      but configurations are different everywhere.
      This will work only on Linux, Windows users can just
      <a href='http://tidy.sourceforge.net/'>get the tidy.exe</a>
      and drop it in the <code>import_html/coders_php_library/bin</code> directory.</p>
    "),
  );

  return $form;
}
