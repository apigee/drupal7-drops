<?php


class PrintGlobals extends Task {
  
  function main() {
    //$GLOBALS[__CLASS__."_extra_variable"] = "extra value";
    //print_r($GLOBALS);
    print_r($this->project);
  }
  
  
}