<?php

/**
 * XHProfRuns_Default is the default implementation of the
 * iXHProfRuns interface for saving/fetching XHProf runs.
 *
 */
class XHProfRunsFile implements XHProfRunsInterface {
  private $dir = '';
  private $suffix = 'xhprof';

  public function getDir(){
    return $this->dir;
  }

  private function gen_run_id($type) {
    return uniqid();
  }

  private function file_name($run_id, $type) {

    $file = "$run_id.$type." . $this->suffix;

    if (!empty($this->dir)) {
      $file = $this->dir . "/" . $file;
    }
    return $file;
  }

  public function __construct($dir = null) {
    // if user hasn't passed a directory location,
    // we use the xhprof.output_dir ini setting
    // if specified, else we default to the directory
    // in which the error_log file resides.
    if (empty($dir)) {
      $dir = ini_get("xhprof.output_dir");
      if (empty($dir)) {
        // some default that at least works on unix...
        $dir = "/tmp";
        watchdog("xhprof", "Warning: Must specify directory location for XHProf runs. ".
                     "Trying {$dir} as default. You can either pass the " .
                     "directory location as an argument to the constructor ".
                     "for XHProfRuns_Default() or set xhprof.output_dir ".
                     "ini param.");
      }
    }
    $this->dir = $dir;
  }

  public function get_run($run_id, $type, &$run_desc) {
    $file_name = $this->file_name($run_id, $type);

    if (!file_exists($file_name)) {
      watchdog("xhprof", "Could not find file $file_name");
      $run_desc = "Invalid Run Id = $run_id";
      return null;
    }

    $contents = file_get_contents($file_name);
    $run_desc = "XHProf Run (Namespace=$type)";
    return unserialize($contents);
  }

  public function save_run($xhprof_data, $type, $run_id = null) {

    // Use PHP serialize function to store the XHProf's
    // raw profiler data.
    $xhprof_data = serialize($xhprof_data);

    if ($run_id === null) {
      $run_id = $this->gen_run_id($type);
    }

    $file_name = $this->file_name($run_id, $type);
    $file = fopen($file_name, 'w');

    if ($file) {
      fwrite($file, $xhprof_data);
      fclose($file);
    }
    else {
      watchdog("xhprof", "Could not open %filename.", array('%file_name' => $file_name));
    }

    // echo "Saved run in {$file_name}.\nRun id = {$run_id}.\n";
    return $run_id;
  }

  public function getRuns($stats, $limit = 50, $skip = 0) {
    $files = $this->scanXHProfDir($this->getDir(), variable_get('site_name', ''));
    foreach ($files as $i => $file) {
      $file['date'] = strtotime($file['date']);
      $files[$i] = $file;
    }
    return $files;
  }

  public function getCount(){}

  public function scanXHProfDir($dir, $source = NULL) {
    if (is_dir($dir)) {
      $runs = array();
      foreach (glob("$dir/*.$source.*") as $file) {
        list($run, $source) = explode('.', basename($file));
        $runs[] = array(
          'run_id' => $run,
          'source' => $source,
          'basename' => htmlentities(basename($file)),
          'date' => date("Y-m-d H:i:s", filemtime($file)),
        );
      }
    }
    return array_reverse($runs);
  }
}
