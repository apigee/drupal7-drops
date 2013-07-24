<?php
/**
 * @file
 * Describes a method invocation of a resource. WADL Methods are always child
 * objects of WADL Resources.
 *
 * @author djohnson
 */

class WADL_Method implements XMLJSONPersistable {

  private $displayName;
  private $id;
  private $name;

  private $tags;
  private $authentication;
  private $example_url;
  private $doc_string;
  private $doc_url;
  private $params;
  private $representation;

  public function __construct($name, $id, $displayName) {
    if ($name != 'GET' && $name != 'POST' && $name != 'PUT' && $name != 'DELETE') {
      throw new Exception('Invalid method name ' . $name);
    }
    if (!self::is_id_unique($id)) {
      throw new Exception('Duplicate method id ' . $id);
    }
    $this->name = $name;
    $this->id = $id;
    $this->displayName = $displayName;
    $this->tags = array();
    $this->authentication = FALSE;
    $this->example_url = '';
    $this->doc_string = '';
    $this->doc_url = NULL;
    $this->params = array();
    $this->representation = array();
  }

  /**
   * Adds a tag to the method.
   *
   * @param string $name
   * @param bool $primary
   */
  public function addTag($name, $primary = FALSE) {
    $tag = new stdClass;
    $tag->name = $name;
    $tag->primary = $primary;
    $this->tags[] = $tag;
  }

  /**
   * Sets whether authentication is required when using the method.
   *
   * @param bool $required
   */
  public function setAuthentication($required) {
    $this->authentication = (bool)$required;
  }

  /**
   * Sets the example URL or URL pattern for this method's call.
   *
   * @param string $url
   */
  public function setExampleURL($url) {
    $this->example_url = $url;
  }

  /**
   * Sets the documentation string for the method.
   * @param string $doc
   * @param string $url
   */
  public function setDoc($doc, $url = NULL) {
    $this->doc_string = $doc;
    $this->doc_url = $url;
  }

  /**
   * Adds a method-level parameter.
   *
   * @param WADL_Param $param
   */
  public function addParam(WADL_Param $param) {
    $this->params[] = $param;
  }

  /**
   * Adds a representation block.
   *
   * @param WADL_Representation $rep
   */
  public function addRepresentation(WADL_Representation $rep) {
    $this->representation[] = $rep;
  }

  /**
   * Converts this object to a structured array.
   *
   * @return array
   */
  public function toObject() {
    $tags = array();
    foreach ($this->tags as $tag) {
      // What a ridiculous representation system!
      if ($tag->primary) {
        $tags['primary'] = $tag->name;
      }
      else {
        $tags[$tag->name] = NULL;
      }
    }

    $method_obj = array(
      'id' => $this->id,
      'apigee:displayName' => $this->displayName,
    );
    if (count($tags) > 0) {
      $method_obj['apigee:tags'] = $tags;
    }

    if (count($this->params) > 0 || count($this->representation) > 0) {
      $request = array();
      foreach ($this->params as $param) {
        if ($param->type == 'template') {
          if (!isset($request['templateParams'])) {
            $request['templateParams'] = array();
          }
          $request['templateParams'][] = $param->toObject();
        }
        elseif ($param->type == 'query') {
          if (!isset($request['queryParams'])) {
            $request['queryParams'] = array();
          }
          $request['queryParams'][] = $param->toObject();
        }
      }
      if (count($this->representation) > 0) {
        $request['representation'] = array();
        foreach ($this->representation as $representation) {
          $request['representation'][] = $representation->toObject();
        }
      }
      $method_obj['request'] = $request;
    }
    $method_obj['name'] = $this->name;
    if (strlen($this->doc_string) > 0) {
      $doc['content'] = $this->doc_string;
      if (strlen($this->doc_url) > 0) {
        $doc['apigee:url'] = $this->doc_url;
      }
      $method_obj['doc'] = $doc;
    }
    if (count($this->params) > 0) {
      $method_obj['params'] = array();
      foreach ($this->params as $param) {
        $method_obj['params'][] = $param->toObject();
      }
    }
    if (strlen($this->example_url) > 0) {
      $method_obj['apigee:example'] = array('url' => $this->example_url);
    }
    $method_obj['apigee:authentication'] = array('required' => $this->authentication);

    return $method_obj;
  }

  /**
   * Converts this object to an XML element.
   *
   * @param DOMDocument $doc
   * @return DOMElement
   */
  public function toXML(DOMDocument $doc) {
    $method = $doc->createElement('method');
    $method->setAttribute('id', $this->id);
    $method->setAttribute('name', $this->name);
    $method->setAttribute('apigee:displayName', $this->displayName);

    if (count($this->tags) > 0) {
      $tags = $doc->createElement('apigee:tags');
      foreach ($this->tags as $tag) {
        $tag_element = $doc->createElement('apigee:tag');
        if ($tag->primary) {
          $tag_element->setAttribute('primary', 'true');
        }
        $tag_element->appendChild($doc->createTextNode($tag->name));
        $tags->appendChild($tag_element);
      }
      $method->appendChild($tags);
    }

    $auth = $doc->createElement('apigee:authentication');
    $auth->setAttribute('required', ($this->authentication ? 'true' : 'false'));
    $method->appendChild($auth);

    if (!empty($this->example_url)) {
      $example = $doc->createElement('apigee:example');
      $example->setAttribute('url', $this->example_url);
      $method->appendChild($example);
    }

    if (!empty($this->doc_string)) {
      $doc_element = $doc->createElement('doc');
      if (!empty($this->doc_url)) {
        $doc_element->setAttribute('apigee:url', $this->doc_url);
      }
      $doc_element->appendChild($doc->createTextNode($this->doc_string));
      $method->appendChild($doc_element);
    }

    if (count($this->params) > 0 || count($this->representation) > 0) {
      $request = $doc->createElement('request');
      foreach ($this->params as $param) {
        $request->appendChild($param->getXML($doc));
      }
      foreach ($this->representation as $representation) {
        $request->appendChild($representation->getXML($doc));
      }
      $method->appendChild($request);
    }
    return $method;
  }

  /**
   * Validates that a given ID is unique within a document.
   *
   * @static
   * @param string $id
   * @return bool
   */
  private static function is_id_unique($id) {
    static $ids;

    if (!isset($ids)) {
      $ids = array();
      return TRUE;
    }
    if (in_array($id, $ids)) {
      return FALSE;
    }
    $ids[] = $id;
    return TRUE;
  }
}