<?php

// China

function location_province_list_cn() {
  return array(
    '11' => "北京",
    '12' => "天津",
    '13' => "河北",
    '14' => "山西",
    '15' => "内蒙古",
    '21' => "辽宁",
    '22' => "吉林",
    '23' => "黑龙江",
    '31' => "上海",
    '32' => "江苏",
    '33' => "浙江",
    '34' => "安徽",
    '35' => "福建",
    '36' => "江西",
    '37' => "山东",
    '41' => "河南",
    '42' => "湖北",
    '43' => "湖南",
    '44' => "广东",
    '45' => "广西",
    '46' => "海南",
    '51' => "四川",
    '52' => "贵州",
    '53' => "云南",
    '54' => "西藏",
    '61' => "陕西",
    '62' => "甘肃",
    '63' => "青海",
    '64' => "宁夏",
    '65' => "新疆",
    '71' => "台湾",
    '91' => "香港",
    '92' => "澳门",
    '97' => "重庆",
  );
}

function location_map_link_cn_providers() {
  return array(
    'google' => array(
      'name' => 'Google Maps',
      'url' => 'http://maps.google.com.hk',
      'tos' => 'http://www.google.com.hk/help/terms_maps.html',
    ),
  );
}

function location_map_link_cn_default_providers() {
  return array('google');
}

function location_map_link_cn_google($location = array()) {
  $query_params = array();

  foreach (array('street', 'city', 'postal_code', 'country') as $field) {
    if (isset($location[$field])) {
      $query_params[] = $location[$field];
    }
  }

  if (count($query_params)) {
    return ('http://maps.google.com.hk?q='. urlencode(implode(', ', $query_params)));
  }
  else {
    return NULL;
  }
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_cn() {
  return array(
    'minlng' => 73.5403,
    'minlat' => 16.0968,
    'maxlng' => 134.7658,
    'maxlat' => 53.608867,
  );
}
