<?php


/**
 * Sends statistics to the stats daemon over UDP
 *
 **/

class StatsD {

  /**
   * Log timing information
   *
   * @param $stat
   *   A string of the metric to log timing info for.
   * @param $time
   *   The ellapsed time (ms) to log
   * @param $sample_rate
   *   A float between 0 and 1 representing the sampling rate.
   *
   */
  public static function timing($stat, $time, $sample_rate = NULL) {
    self::send(array($stat => "$time|ms"), $sample_rate);
  }


  /**
   * Sends a gauge, an arbitrary value to statsd
   *
   * @param $stat 
   *   The metric to send. 
   * @param $value 
   *   The value to send for this metric.
   * @param $sample_rate
   *   A float between 0 and 1 representing the sampling rate.
   *
   */
  public static function gauge($stat, $value, $sample_rate = NULL) {
    self::send(array($stat => "$value|g"), $sample_rate);
  }


  /**
   * Increments one or more stats counters
   *
   * @param $stats
   *   A string or an array of string representing the metric(s) to increment.
   * @param $sample_rate
   *   A float between 0 and 1 representing the sampling rate.
   * @return boolean
   *
   */
  public static function increment($stats, $sample_rate = NULL) {
    self::updateStats($stats, 1, $sample_rate);
  }


  /**
   * Decrements one or more stats counters.
   *
   * @param $stats
   *   A string or an array of string representing the metric(s) to decrement.
   * @param $sample_rate
   *   A float between 0 and 1 representing the sampling rate.
   * @return boolean
   *
   */
  public static function decrement($stats, $sample_rate = NULL) {
    self::updateStats($stats, -1, $sample_rate);
  }


  /**
   * Updates one or more stats counters by arbitrary amounts.
   *
   * @param $stats
   *   A string or an array of string representing the metric(s) to increment or decrement.
   * @param $delta
   *   The amount to increment/decrement each metric by.
   * @param $sample_rate
   *   A float between 0 and 1 representing the sampling rate.
   * @return boolean
   *
   */
  public static function updateStats($stats, $delta = 1, $sample_rate = NULL) {

    $data = array();

    if (!is_array($stats)) {
      $stats = array($stats);
    }

    foreach($stats as $stat) {
      $data[$stat] = "$delta|c";
    }

    self::send($data, $sample_rate);

  }


  /*
   * Squirt the metrics over UDP
   *
   */
  public static function send($data, $sample_rate = NULL) {

    if (!variable_get('statsd_enabled', FALSE) ) {
      return;
    }

    $sample_rate  = $sample_rate ? $sample_rate : variable_get('statsd_sample_rate', 1);
    $sampled_data = array();
    $data         = self::prefixData($data);

    if ($sample_rate < 1) {
      foreach ($data as $stat => $value) {
        if ((mt_rand() / mt_getrandmax()) <= $sample_rate) {
          $sampled_data[$stat] = "$value|@$sample_rate";
        }
      }
    } else {
      $sampled_data = $data;
    }

    if (empty($sampled_data) ) {
      return;
    }

    $host = variable_get('statsd_host', '127.0.0.1');
    $port = variable_get('statsd_port', 8125);
    $fp   = stream_socket_client("udp://$host:$port", $errno, $errstr);

    if ($fp) {
      stream_set_blocking($fp, 0);
      foreach ($sampled_data as $stat => $value) {
        fwrite($fp, "$stat:$value");
      }
      fclose($fp);
    }

  }


  /**
   * Create the data strings that will be passed into statsd
   * 
   * @param $data
   *   An array of key value pairs to prefix.
   * 
   */
  protected function prefixData($data) {

    $prefix = ($prefix = variable_get('statsd_prefix', NULL) ) ? $prefix . '.' : '';
    $suffix = ($suffix = variable_get('statsd_suffix', NULL) ) ? '.' . $suffix : '';
    $return = array();

    foreach ($data as $key => $value) {
      $name = sprintf('%s' . $key . '%s', $prefix, $suffix);
      $return[$name] = $value;
    }

    return $return;

  }

}