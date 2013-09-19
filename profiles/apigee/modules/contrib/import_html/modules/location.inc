<?php
/**
 * @file read and absorb location microformats
 *

 <div class="location vcard">
 <div class="adr">
 <span class="fn">Aotea</span> <span class="geo"><abbr
 class="latitude" title="-41.127594">41&iexcl; 7'
 39.3384" S</abbr>, <abbr class="longitude"
 title="174.868020">174&iexcl; 52' 4.872"
 E</abbr></span>
 </div>
 </div>

 */


/**
 * A callback function invoked by import_html when it's analyzing page
 * semantics.
 *
 *
 * Scrape the microformats
 * - very rough, could be vastly improved.
 */
function location_import_html($profile, &$node, $datadoc = NULL) {
  $locations = xml_query($datadoc, './/*[contains(@class,"adr")]');
  if ($locations->length == 0) {
    return;
  }
  // microformat class => location module attribute
  $microformats = array(
    'latitude' => 'latitude',
    'longitude' => 'longitude',
    'fn' => 'name',
    'country' => 'country_name',
  );

  foreach ($locations as $location_element) {
    $location_data = array();
    foreach ($microformats as $class => $attribute) {
      $found = xml_query($location_element, './/*[contains(@class,"' . $class . '")]');
      foreach ($found as $element) {
        if (!$val = $element->getAttribute('title')) {
          $val = xml_textContent($element);
        }
        if (! empty($val)) {
          $location_data[$attribute] = $val;
        }
      }
    }
    $node->locations[] = $location_data;
  }
}
