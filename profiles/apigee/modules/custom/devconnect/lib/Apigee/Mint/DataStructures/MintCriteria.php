<?php

namespace Apigee\Mint\DataStructures;

class MintCriteria extends DataStructure {

  /**
   * @var bool
   */
  private $as_xorg;

  /**
   * @var string
   */
  private $current_option;

  /**
   * @var string
   *   Date value formatted as YYYY-mm-dd
   */
  private $from_date;

  /**
   * @var array
   */
  private $group_by;

  /**
   * @var bool
   */
  private $show_rev_share_pct;

  /**
   * @var bool
   */
  private $show_summary;

  /**
   * @var bool
   */
  private $show_tx_detail;

  /**
   * @var bool
   */
  private $show_tx_type;

  /**
   * @var string
   *   Date value formatted as YYYY-mm-dd
   */
  private $to_date;

  public function __construct($data = NULL) {
    if (is_array($data)) {
      $this->loadFromRawData($data);
    }
  }

  // accessors/setters
  public function asXorg() {
    return $this->as_xorg;
  }
  public function setAsXorg($as_xorg) {
    $this->as_xorg = $as_xorg;
  }

  public function getCurrencyOption() {
    return $this->current_option;
  }
  public function setCurrencyOption($currency_option) {
    $this->current_option = $currency_option;
  }

  public function getFromDate() {
    return $this->from_date;
  }
  public function setFromDate($from_date) {
    $this->from_date = $from_date;
  }

  public function getGroupBy() {
    return $this->group_by;
  }
  public function setGroupBy($group_by) {
    $this->group_by = $group_by;
  }

  public function showRevSharePct() {
    return $this->show_rev_share_pct;
  }
  public function setShowRevSharePct($show_rev_share_pct) {
    $this->show_rev_share_pct = $show_rev_share_pct;
  }

  public function showSummary() {
    return $this->show_summary;
  }
  public function setShowSummary($show_summary) {
    $this->show_summary = $show_summary;
  }

  public function showTxDetail() {
    return $this->show_tx_detail;
  }
  public function setShowTxDetail($show_tx_detail) {
    $this->show_tx_detail = $show_tx_detail;
  }

  public function showTxType() {
    return $this->show_tx_type;
  }
  public function setSowTxType($show_tx_type) {
    $this->show_tx_type = $show_tx_type;
  }

  public function getToDate() {
    return $this->to_date;
  }
  public function setToDate($to_date) {
    $this->to_date = $to_date;
  }


}