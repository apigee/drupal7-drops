<?php

class XHProfRunsFile implements XHProfRunsInterface {
  private $dir;
  private $suffix;

  public function __construct() {
    $this->dir = ini_get("xhprof.output_dir") ?: sys_get_temp_dir();
    $this->suffix = 'xhprof';
  }

  private function gen_run_id($type) {
    return uniqid();
  }

  private function fileName($run_id, $namespace) {
    $file = implode('.', array($run_id, $namespace, $this->suffix));

    if (!empty($this->dir)) {
      $file = $this->dir . "/" . $file;
    }
    return $file;
  }

  public function getRun($run_id, $namespace) {
    $file_name = $this->fileName($run_id, $namespace);

    if (!file_exists($file_name)) {
      throw new Exception("Could not find file $file_name");
    }

    $contents = file_get_contents($file_name);
    return unserialize($contents);
  }

  public function getRuns($namespace = NULL) {
    xdebug_break();
    $files = $this->scanXHProfDir($this->dir, $namespace);
    $files = array_map(function($f) {
        $f['date'] = strtotime($f['date']);
        return $f;
      }, $files);
    return $files;
  }

  public function scanXHProfDir($dir, $namespace = NULL) {
    if (is_dir($dir)) {
      $runs = array();
      foreach (glob("{$this->dir}/*.{$this->suffix}") as $file) {
        list($run, $source) = explode('.', basename($file));
        $runs[] = array(
          'run_id' => $run,
          'namespace' => $source,
          'basename' => htmlentities(basename($file)),
          'date' => date("Y-m-d H:i:s", filemtime($file)),
        );
      }
    }
    return array_reverse($runs);
  }
}

