<?php

interface XHProfRunsInterface {
  public function getRuns();
  public function getRun($run_id, $namespace);
}
