<?php

class XHProfParser {
  public $data = array();
  public $totals = array();
  public $symbol_totals = array();

  public function __construct($data) {
    $this->data = $data;
    $this->getTotals();
  }

  public function getTotals() {
    $this->totals = $this->data['main()'];
    $this->totals['ct'] = $this->getCallCount();
    return $this->totals;
  }

  public function toXML($totals) {
    $xml = new SimpleXMLElement('<xhprof_data/>');
    array_walk_recursive(array_flip($totals), array ($xml, 'addChild'));
    return $xml->asXML();
  }

  public function getCallCount() {
    $call_count = 0;
    foreach ($this->data as $symbol) {
      $call_count += $symbol['ct'];
    }
    return $call_count;
  }

  public function getMetrics($symbol) {
    if (!isset($this->symbol_totals[$symbol])) {
      $this->symbol_totals[$symbol] = array(
        'ct' => 0,
        'wt' => 0,
        'cpu' => 0,
        'mu' => 0,
        'pmu' => 0,
      );
    }
    foreach ($this->data as $key => $symbol_data) {
      if ($key !== 'main()') {
        list($caller, $cur_symbol) = explode('==>', $key);
        if ($cur_symbol == $symbol) {
          foreach ($symbol_data as $metric => $value) {
            $this->symbol_totals[$symbol][$metric] += $value;
          }
          $this->symbol_totals[$symbol] = $this->calculatePercentages($this->symbol_totals[$symbol]);
          $this->symbol_totals[$symbol] = $this->symbol_totals[$symbol];
          return $this->symbol_totals[$symbol];
        }
      }
    }
  }
  protected function calculatePercentages($symbol_metrics) {
    foreach ($symbol_metrics as $metric => $value) {
      if ($this->totals[$metric] !== 0) {
        $symbol_metrics[$metric . '%'] = $value / $this->totals[$metric];
      }
    }
    return $symbol_metrics;
  }
}
