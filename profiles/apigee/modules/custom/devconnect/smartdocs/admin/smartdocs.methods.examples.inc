<?php

/**
 * Provides JSON Example Structures for Methods in SmartDocs
 *
 * @param $which
 * @return string
 */
function smartdocs_method_examples($which) {
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
			return '<pre>' . $example . '</pre>';
			break;
		case 'parameters':
			$example = '[
    {
        "dataType": "string",
        "defaultValue": "",
        "description": "Set the weather location as a WOEID.",
        "name": "w",
        "required": true,
        "type": "QUERY"
    }
]';
			return '<pre>' . $example . '</pre>';
			break;
		case 'tags':
			$example = '[
    "Get Weather"
]';
			return '<pre>' . $example . '</pre>';
			break;
		case 'customAttributes':
			$example = '[
  "name":"someattr",
  "value":"somevalue"
]';
			return '<pre>' . $example . '</pre>';
			break;
		default:
			return null;
	}
}