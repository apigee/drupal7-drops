<?php

class XHProfDiffParser {
  public $parser1;
  public $parser2;
  public $totals = array();
  public $symbol_totals = array();

  public function __construct($data1, $data2) {
    $this->data = $data;
    $this->parser1 = new XHProfParser($data1);
    $this->parser2 = new XHProfParser($data2);
    $this->parser1->getTotals();
    $this->parser2->getTotals();
  }

  public function getDiffTotals() {
    $diff_totals[0] = $this->parser1->getTotals();
    $diff_totals[1] = $this->parser2->getTotals();
    $diff_totals['diff'] = array();
    $diff_totals['diff%'] = array();
    foreach ($diff_totals[0] as $metric => $value) {
      $diff_totals['diff'][$metric] = $diff_totals[1][$metric] - $value;
      $diff_totals['diff%'][$metric] = (($diff_totals[1][$metric] / $value) - 1) * 100;
    }
    return $diff_totals;
  }
}
