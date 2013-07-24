<?php

/**
 * @file
 * Represents a resource that can be accessed via a particular URI or URI
 * pattern.
 *
 * @author djohnson
 */

class WADL_Resource implements XMLJSONPersistable {

  private $path;
  private $methods;
  private $params;

  public function __construct($path) {
    $this->path = $path;
    $this->methods = array();
    $this->params = array();
  }

  /**
   * Adds a pre-populated method to the resource.
   *
   * @param WADL_Method $method
   */
  public function addMethod(WADL_Method $method) {
    $this->methods[] = $method;
  }

  /**
   * Adds a parameter to the resource.
   *
   * This type of parameter is used when all methods of a given resource have
   * parameters in common. It is implied that this is equivalent to duplicating
   * the parameter declaration in each method's descriptor.
   *
   * @param WADL_Param $param
   */
  public function addParam(WADL_Param $param) {
    $this->params[] = $param;
  }

  /**
   * Converts this object to a structured array.
   *
   * @return array
   */
  public function toObject() {
    $resources = array();
    foreach ($this->methods as $method) {
      $method_obj = $method->toObject;
      if (count($this->params) > 0) {
        $method_obj['params'] = array_merge($this->params, $method_obj['params']);
      }
      $resources[] = array(
        'path' => '/' . $this->path, // for some reason JSON paths start with slash
        'method' => $method_obj
      );
    }
    return $resources;
  }

  /**
   * Converts this object to an XML element.
   *
   * @param DOMDocument $doc
   * @return DOMElement
   */
  public function toXML(DOMDocument $doc) {
    $resource = $doc->createElement('resource');
    $resource->setAttribute('path', $this->path);

    foreach ($this->params as $param) {
      $resource->appendChild($param->toXML($doc));
    }

    foreach ($this->methods as $method) {
      $resource->appendChild($method->toXML($doc));
    }
    return $resource;
  }
}