<?php
/**
 * @file
 * Defines the top-level Application object in a WADL hierarchy. Most of the
 * heavy lifting is done by the child Resource objects and their child objects.
 *
 * @author djohnson
 */

class WADL_Application {

  private $resources;
  private $base;
  private $debug;

  /**
   * Class constructor. Sets basic object properties.
   *
   * @param string $format
   * @param string $base
   * @param bool $debug
   */
  public function __construct($format, $base, $debug = FALSE) {
    if ($format == 'json' || $format == 'xml') {
      $this->format = $format;
    }
    else {
      throw new Exception('Invalid format ' . $format);
    }
    $this->base = (string)$base;
    $this->resources = array();
    $this->debug = $debug;
  }

  /**
   * Allows a resource to be added to this application's hierarchy.
   *
   * @param WADL_Resource $resource
   */
  public function addResource(WADL_Resource $resource) {
    $this->resources[] = $resource;
  }

  /**
   * Magic method to turn the object hierarchy into an appropriately-formatted
   * string, depending upon the requested format (JSON or XML).
   *
   * @return string
   */
  public function __toString() {
    if ($this->format == 'json') {
      $base_object = array(
        'application' => array(
          'endpoints' => array(
            'resources'
          )
        )
      );
      foreach ($this->resources as $resource) {
        $resource_obj = $resource->toObject();
        // A resource may have more than one method. When that happens,
        // the JSON must display it as two separate resources. (Dumb, but
        // that's the way it is.)
        foreach ($resource_obj as $resource_item) {
          $base_object['application']['endpoints']['resources'][] = $resource_item;
        }
      }
      $json_options = 0;
      if ($this->debug) {
        $json_options |= JSON_PRETTY_PRINT;
      }
      return json_encode($base_object, $json_options);
    }
    else {
      $primary_namespace = 'http://wadl.dev.java.net/2009/02';
      $other_namespaces = array(
        'xsd' => 'http://www.w3.org/2001/XMLSchema',
        'apigee' => 'http://api.apigee.com/wadl/2010/07/',
        'xsi' => 'http://www.w3.org/2001/XMLSchema-instance'
      );

      $doc = new DOMDocument('1.0', 'UTF-8');
      $application = $doc->createElementNS($primary_namespace, 'application');
      foreach ($other_namespaces as $abbrev => $namespace) {
        $application->setAttribute("xmlns:$abbrev", $namespace);
      }
      $application->setAttribute('xsi:schemaLocation', 'http://wadl.dev.java.net/2009/02 http://apigee.com/schemas/wadl-schema.xsd http://api.apigee.com/wadl/2010/07/ http://apigee.com/schemas/apigee-wadl-extensions.xsd');
      $doc->appendChild($application);

      $resources = $doc->createElement('resources');
      $resources->setAttribute('base', $this->base);
      $application->appendChild($resources);

      foreach ($this->resources as $resource) {
        $resources->appendChild($resource->toXML($doc));
      }
      return $doc->saveXML();
    }
  }
}