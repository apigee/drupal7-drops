<?php

$key_file = "/mnt/apigee/httpd/keys/ts-cloudfront-pk-APKAI7LG6XUA627NVSXQ.pem";
  if (file_exists($key_file)) {
    global $conf;
    $resource = $entity->field_uri[LANGUAGE_NONE][0]['value'];
    $aURI = parse_url($resource);
    //print_r($aURI);
    $bucket = str_replace(".s3.amazonaws.com", "", $aURI['host']);
    $object = $aURI['path'];
    $query['Key-Pair-Id'] = "APKAI7LG6XUA627NVSXQ";
    $query['AWSAccessKeyId'] = $conf['aws_key'];
    $query['Expires'] = time() + 3600; //Time out in seconds
    $query['Signature'] = base64_encode(hash_hmac("sha1", utf8_encode("GET\n\n\n{$query['Expires']}\n/{$bucket}{$object}"), $conf['aws_secret'], true));     
    echo l(t("Click To Download"), $resource, array("query" => $query));
  } else {
        echo "<p>Failed to load private key!</p>";
  }
