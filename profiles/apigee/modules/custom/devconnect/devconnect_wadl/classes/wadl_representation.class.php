<?php
/**
 * @file
 * Represents a resource's state.
 *
 * @author djohnson
 */

class WADL_Representation implements XMLJSONPersistable {

  private $payload;
  private $media_type;
  private $params;
  private $doc;

  public function __construct($media_type = 'multipart/form-data') {
    $this->media_type = $media_type;
    $this->doc = NULL;
    $this->params = array();
    $this->payload = NULL;
  }

  public function setDocumentation($doc) {
    $this->doc = $doc;
  }

  /**
   * Sets the payload of a representation.
   *
   * @param string $content
   * @param bool $required
   */
  public function setPayload($content, $required = TRUE) {
    $payload = new stdClass;
    $payload->content = $content;
    $payload->required = (bool)$required;
    $this->payload = $payload;
  }

  public function addParam(WADL_Param $param) {
    $this->params[] = $param;
  }

  /**
   * Converts this object to a structured array.
   *
   * @return array
   */
  public function toObject() {
    $representation = array();
    if (isset($this->payload)) {
      $representation['apigee:payload'] = array(
        'content' => $this->payload->content,
        'required' => $this->payload->required
      );
    }
    $representation['mediaType'] = $this->media_type;
    $representation['profile'] = FALSE; //WTF?
    if (!empty($this->doc)) {
      $representation['doc'] = array('content' => $this->doc);
    }

    if (count($this->params) > 0) {
      $params = array();
      foreach ($this->params as $param) {
        $params[] = $param->toObject();
      }
      $representation['params'] = $params;
    }

    return $representation;
  }

  /**
   * Converts this object to an XML element.
   *
   * @param DOMDocument $doc
   * @return DOMElement
   */
  public function toXML(DOMDocument $doc) {
    $representation = $doc->createElement('representation');
    $representation->setAttribute('mediaType', $this->media_type);
    if (isset($this->payload)) {
      $payload = $doc->createElement('apigee:payload');
      $payload->setAttribute('required', ($this->payload->required ? 'true' : 'false'));
      $content = $doc->createElement('apigee:content');
      $content->appendChild($doc->createTextNode($this->payload->content));
      $payload->appendChild($content);
      $representation->appendChild($payload);
    }
    if (!empty($this->doc)) {
      $documentation = $doc->createElement('doc');
      $documentation->appendChild($doc->createTextNode($this->doc));
      $representation->appendChild($documentation);
    }
    if (!empty($this->params)) {
      foreach ($this->params as $param) {
        $representation->appendChild($param->toXML($doc));
      }
    }

    return $representation;
  }
}