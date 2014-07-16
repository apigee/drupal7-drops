<?php

/**
 * Provides JSON Example Structures for Methods in SmartDocs
 *
 * @param $which
 * @return string
 */
function smartdocs_method_examples($which) {
  $example = '';
  switch ($which) {
    case 'parameterGroups':
      $example = '{
  "minChoice":"1",
  "maxChoice":"2",
  "parameters":[
    {
      "dataType":"string",
      "type":"query",
      "name":"shirtsize",
      "default":"lower",
      "description":"Shirtsize.",
      "options":["s","m","l"]
    } ]
}';
      break;
    case 'parameters':
      $example = '[
  {
    "name":"Content-Type",
    "required":false,
    "dataType":"string",
    "type":"header",
    "defaultValue":"multipart/form-data",
    "description":"SpecifyContentType."
  }, {
    "name":"expand",
    "required":false,
    "dataType":"boolean",
    "type":"query",
    "default":"false",
    "description":"Expandedresponse."
  }
]';
      break;
    case 'tags':
      $example = '{
  "Tag1",
  "Tag2"
}';
      break;
    case 'samples':
      $example = '{
  "contentType":"application/json",
  "type":"inlinehar",
  "harValue":"SomeinlineHARcontent",
  "sampleUrl":"http://xyz/v1/4xc953ad-1e6f-46f4-a4b2-d1f73cfa9cfc"
}';
      break;
    case 'response':
      break;
    case 'customAttributes':
      $example = '{
  "name":"someattr",
  "value":"somevalue"
}';
      break;
  }
  return ($example == '') ? '' : '<pre>' . $example . '</pre>';
}