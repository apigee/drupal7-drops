<?php
/**
 * @file
 * Describes a parameter, either at the resource level (common to all method
 * invocations) or on the method level. Hence, Params may be child objects of
 * Resources or Methods, or possibly also Representations.
 *
 * @author djohnson
 */

class WADL_Param implements XMLJSONPersistable {

  private $name;
  private $default;
  private $required;
  private $style;
  private $type;
  private $docstring;
  private $options;

  public function __construct($name, $default, $required, $style, $type, $docstring = '') {
    if ($style != 'query' && $style != 'template') {
      throw new Exception('Invalid param style ' . $style);
    }
    if (!self::validate_xsd_type($type)) {
      throw new Exception('Invalid param type ' . $type);
    }
    $this->name = $name;
    $this->default = $default;
    $this->required = (bool)$required;
    $this->style = $style;
    $this->type = $type;
    $this->docstring = $docstring;
    $this->options = NULL;
  }

  /**
   * Adds an option to this object's options array.
   *
   * @param $opt
   */
  public function addOption($opt) {
    if (!isset($this->options)) {
      $this->options = array();
    }
    $this->options[] = $opt;
  }

  /**
   * Converts this object to a structured array.
   *
   * @return array
   */
  public function toObject() {
    $param = array(
      'default' => $this->default,
      'style' => $this->style,
      'count' => NULL, //WTF?
      'name' => $this->name,
    );
    if (strlen($this->docstring) > 0) {
      $param['doc'] = array('content' => $this->docstring);
    }
    $param['type'] = $this->type;
    $param['required'] = $this->required;

    if (isset($this->options)) {
      $options = array();
      foreach ($this->options as $opt) {
        $options[] = array('value' => $opt);
      }
      $param['options'] = $options;
    }

    return $param;
  }

  /**
   * Converts this object to an XML element.
   *
   * @param DOMDocument $doc
   * @return DOMElement
   */
  public function toXML(DOMDocument $doc) {
    $param = $doc->createElement('param');
    $param->setAttribute('default', $this->default);
    $param->setAttribute('name', $this->name);
    $param->setAttribute('required', ($this->required ? 'true' : 'false'));
    $param->setAttribute('style', $this->style);
    $param->setAttribute('type', 'xsd:' . $this->type);

    if (strlen($this->docstring) > 0) {
      $doc_element = $doc->createElement('doc');
      $doc_element->appendChild($doc->createTextNode($this->docstring));
      $param->appendChild($doc_element);
    }
    if (!empty($this->options)) {
      foreach ($this->options as $opt) {
        $option = $doc->createElement('option');
        $option->setAttribute('value', $opt);
        $param->appendChild($option);
      }
    }
    return $param;
  }

  /**
   * Validates that the given type is a valid basic xsd datatype.
   *
   * @static
   * @param string $type
   * @return bool
   */
  private static function validate_xsd_type($type) {
    static $valid_types = array(
      'anyURI', 'base64Binary', 'boolean', 'byte', 'date', 'dateTime',
      'decimal', 'double', 'duration', 'float', 'gDay', 'gMonth',
      'gMonthDay', 'gYear', 'gYearMonth', 'hexBinary', 'ID', 'IDREF',
      'IDREFS', 'int', 'integer', 'language', 'long', 'Name', 'NCName',
      'negativeInteger', 'NMTOKEN', 'NMTOKENS', 'nonNegativeInteger',
      'nonPositiveInteger', 'normalizedString', 'positiveInteger', 'QName',
      'short', 'string', 'time', 'token', 'unsignedByte', 'unsignedInt',
      'unsignedLong', 'unsignedShort'
    );
    return in_array($type, $valid_types);
  }
}