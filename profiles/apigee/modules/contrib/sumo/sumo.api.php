<?php

/**
 * Alters the site_id that is sent in the X-Sumo-Name header for Sumo Logic
 * logging (both web access logs and watchdog).
 *
 * @param string $site_id
 */
function hook_sumo_site_id_alter(&$site_id) {
  $site_id .= '.' . time();
}