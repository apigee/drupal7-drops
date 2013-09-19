<?php
/**
 * @file
 * Convenience class for holding responses from HTTP calls.
 *
 * @author Daniel Johnson <djohnson@apigee.com>
 * @since 26-Apr-2013
 */

namespace Apigee\Util;

class HTTPResponse {
  public $code;
  public $error;
  public $request;
  public $data;
  public $headers;
  public $statusMessage;

  public function __construct() {
    $this->error = FALSE;
    $this->code = 0;
    $this->request = '';
    $this->data = '';
    $this->headers = array();
    $this->statusMessage = '';
  }

  public function __toString() {
    $out = '';

    $out .= "HTTP Response: {$this->code} {$this->statusMessage}\n";
    $out .= "Request header: ";
    $request_headers = preg_split('!(\r|\n|\r\n)!', $this->request);
    foreach ($request_headers as $i => $header) {
      if ($i > 0) {
        $out .= "\t";
      }
      if (substr(strtolower($header), 0, 14) == 'authorization:') {
        $header = 'Authorization: [encrypted]';
      }
      $out .= "$header\n";
    }

    if (!empty($this->headers)) {
      $out .= "Response Headers: ";
      $i = 0;
      foreach ($this->headers as $name => $value) {
        if ($i > 0) {
          $out .= "\t";
        }
        $out .= "$name: $value\n";
      }
    }

    if (!empty($this->error)) {
      $out .= "Error: {$this->error}\n";
    }
    if (!empty($this->data)) {
      $out .= "Response Payload:\n{$this->data}\n";
    }

    return $out;
  }
}