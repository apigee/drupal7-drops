<?php
namespace Apigee\Exceptions;

class ResponseException extends \Exception {

  private $uri;
  private $params;
  private $response_body;

  public function __construct($message, $code = 0, $uri = NULL, $params = NULL, $response_body = NULL) {
    parent::__construct($message, $code);
    $this->uri = $uri;
    $this->params = $params;
    $this->response_body = $response_body;
  }

  public function getUri() {
    return $this->uri;
  }
  public function getParams() {
    return $this->params;
  }
  public function getResponse() {
    return $this->response_body;
  }
}