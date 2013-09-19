<?php
/**
 * @file
 * Setup and test the availability of a binary htmltidy
 * executable on the host.
 *
 * These functions have been extracted into this inc, as
 * they are usually only run-once and they are not needed in
 * day-to-day running, or under PHP5 with the tidy extension.
 *
 * The htmltidy_test() function will set the drupal variable
 * htmltidy_apppath - which should be the full system path to the
 * executable.
 *
 * htmltidy_install() will FETCH THE PACKAGE from sourceforge,
 * and attempt to unpack it into the module directory.
 * This is a desperate measure, useful only if you dan't have access
 * to the server directly.
 *
 * The code is verbose and careful - disparate systems and installs
 * may have lots of differences.
 (
 * @TODO review code for exploits - this runs a system() call on the server.
 *
 *
 * @package coders
 * @author Dan Morrison http://coders.co.nz/
 */

/**
 * Returns true if ok, false on error.
 *
 * Assigns the passed-by-handle input $message an explanation. Sets the
 * htmltidy_apppath Drupal variable to a valid value as a side-effect.
 */
function htmltidy_test(&$message) {
  $message = "";
  if (extension_loaded("tidy")) {
    // Check it runs (PHP4 claims to, but doesn't)
    // There is both an older functional (PECL) version and
    // an official Object-Oriented implimentation out there in the wild
    // I only support the OO released version.
    @eval('$tidy = new tidy();');
    if ($tidy) {
      // looks like success
      $message = 'PHP Tidy Extension enabled OK.';
      #debug($message);
    }
    else {
      $message = 'PHP Tidy Extension claims to exist, but does not initialize as expected.';
      #debug($message);
    }
  }
  // Keep scanning for the binary, even if extension was found.
  // For informational purposes.

  $tidypath = variable_get("htmltidy_apppath", "tidy");
  if (is_file($tidypath)) {
    #debug("Tidy binary found OK at $tidypath");
  }
  else {
    drupal_set_message("Not sure where 'tidy' is. Scanning likely places the tidy executable may be.");

    $maybepaths = array(
      '/bin/tidy',
      '/usr/bin/tidy',
      '/usr/local/bin/tidy',
      foreslash(dirname(__FILE__)) . '/bin/tidy',
    );
    foreach ($maybepaths as $tidypath) {
      #debug("Is tidy at " . $tidypath);
      if (file_exists($tidypath)) {
        drupal_set_message("Found 'tidy' at $tidypath.");
        break;
      }
      $tidypath .= '.exe';
      if (file_exists($tidypath)) {
        drupal_set_message("Found 'tidy' at $tidypath.");
        break;
      }
    }

    if (!file_exists($tidypath)) {
      $message .= " Couldn't find tidy binary anywhere!";
      return FALSE;
    }
  }

  #debug("Tidy binary is available at " . $tidypath);

  // Now test it
  $command = escapeshellcmd("$tidypath -v");
  debug("Running command <br/>\n$command");
  $result = exec($command, $response);

  if (!$result) {
    $message .= " Found 'tidy' binary, but it didn't run right. \n$command\nfailed to respond correctly";
    variable_del("htmltidy_apppath");
    return FALSE;
  }

  // Probably success if we've got this far.
  variable_set("htmltidy_apppath", $tidypath);

  #debug("Looks like tidy is ready for use! " . $result);
  $message .= " Commandline tidy at $tidypath . \n" . $result;
  return TRUE;
}

/**
 * Attempt to download the tidy binary from the sourceforge repository.
 *
 * Flying blind, and with no introspection on the actual platform,
 * I just make some guesses about what should work.
 * Tested on two different locked-down commercial hosts so far.
 * @TODO other OS?
 */
function htmltidy_install_binary() {
  $wd = dirname(__FILE__) . '/';
  print("Installing HTMLTidy.<br/>\nWorking dir is $wd<br/>");

  $source = 'http://tidy.sourceforge.net/cf/tidy_linux_x86.tgz';
  $tgz = basename($source);
  $tar = basename($tgz, '.tgz') . '.tar';

  if (!file_exists($wd . $tar)) {
    print("Fetching from $source <br/>");
    system("GET $source > $wd$tgz");
  }
  if (!file_exists($wd . $tgz)) {
    print("Failed to fetch the remote package <br/>");
    return FALSE;
  }

  print("We have the package :) <br/>");

  $command = "gunzip $wd$tgz";
  print("running $command  <br/>");
  system($command);

  if (!file_exists($wd . $tar)) {
    print("Didn't unzip, $wd$tar not created<br/>");
    return FALSE;
  }

  print("unzipped OK<br/>");
  $command = "cd $wd; tar -xf {$wd}$tar";
  print("running $command  <br/>");
  system($command);

  $apppath = $wd . 'bin/tidy';
  if (file_exists($apppath)) {
    print("Unpacked the binary to '$apppath' OK! Tres cool<br/>");
    variable_set("htmltidy_apppath", $apppath);

    if (htmltidy_test($message)) {
      print("...and it seems to work! $message");
    }
    else {
      print("...but running it failed.<br/>\n$message\n<br/>\nMay have been the wrong OS");
    }

  }
  else {
    print("Failed to unpack binary, it should have been at '$apppath' by now");
  }
}
