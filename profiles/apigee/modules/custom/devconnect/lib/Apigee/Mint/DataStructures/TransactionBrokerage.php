<?php

namespace Apigee\Mint\DataStructures;

use Apigee\Mint\Transaction;
use Apigee\Mint\Developer;

class TransactionBrokerage extends DataStructure {

  /**
   * @var string
   */
  private $transaction_id;

  /**
   * @var boolean
   */
  private $broker_id;

  /**
   * @var double
   */
  private $calculated_revenue_share;

  /**
   * @var double
   */
  private $fee;

  /**
   * @var \Apigee\Mint\Transaction
   */
  private $transaction;

  /**
   * @var \Apigee\Mint\Developer
   */
  private $broker;

  public function __construct($data = NULL) {
    $excluded_properties = array('transaction', 'broker');
    if (is_array($data)) {
      $this->loadFromRawData($data, $excluded_properties);
    }

    // @TODO Implement broker load

    // @TODO Implement transaction load
  }

  public function getTransactionId() {
    return $this->transaction_id;
  }
  public function setTransactionId($trans_id) {
    $this->transaction_id = $trans_id;
  }

  public function getBrokerId() {
    return $this->broker_id;
  }
  public function setBrokerId($broker_id) {
    $this->broker_id = $broker_id;
  }

  public function getCalculatedRevenueShare() {
    return $this->calculated_revenue_share;
  }
  public function setCalculatedRevenueShare($calculated_revenue_share) {
    $this->calculated_revenue_share = $calculated_revenue_share;
  }

  public function getFee() {
    return $this->fee;
  }
  public function setFee() {
    $this->fee = $fee;
  }

  /**
   * @return \Apigee\Mint\Transaction
   */
  public function getTransaction() {
    return $this->transaction;
  }
  /**
   * @param \Apigee\Mint\Transaction $trans
   */
  public function setTransaction(Transaction $trans) {
    $this->transaction = $trans;
  }

  /**
   * @return \Apigee\Mint\Developer
   */
  public function getBroker() {
    return $this->broker;
  }
  /**
   * @param \Apigee\Mint\Developer $broker
   */
  public function setBroker(Developer $broker) {
    $this->broker = $broker;
  }

}