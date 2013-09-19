#!/usr/bin/env php
<?php

function display_step($step) {
  echo "\n****************************************************************************************************\n";
  echo "\n{$step}\n";
  echo "\n****************************************************************************************************\n";
}

function rsync_command($source, $destination){
  display_step("Syncing: {$source} to {$destination}");
  return passthru("rsync -rlvz --size-only --exclude='.*' --ipv4 --progress -e 'ssh -p 2222' {$source} {$destination}");
}


display_step("IF SYNCING DEV SITE, PLEASE BE SURE TO PUT IN SFTP MODE! DO THIS NOW! (you have a few minutes while the db script is running)");

$drushexe = '/usr/local/bin/drush';
$distro_dir = 'devconnect-111.8';
$site_alias = null;
$script_name = $argv[0];
if (count($argc) >= 2) {
  $site_alias = $argv[1];
}

$manifest = getcwd()."/MANIFEST.ini";

if (!file_exists($manifest)) {
  exit("You must execute this file from an uncompressed drush site archive.\n");
}

if (!$ini = parse_ini_file($manifest, TRUE)) {
   exit("The site archive is unreadable.\n");
} 
$instance_name = @array_pop(array_keys($ini));
$orgname = @array_pop(explode("-", $instance_name));
$instance = $ini[$instance_name];
$sqlfile = dirname($manifest)."/".$instance['database-default-file'];

if (empty($site_alias) || trim($site_alias) == "") {
  $site_alias = "@{$orgname}.dev";
  echo "No valid site alias. Assuming {$site_alias}.\n";
} else {
  echo "Running distro-ize on {$site_alias}\n";
}

$aliases = shell_exec("{$drushexe} site-alias");
$aliases = str_replace("pantheon.", "", $aliases);
$aliases = explode("\n", $aliases);

if (in_array($site_alias, $aliases) === false) {
  print_r(get_defined_vars());
  exit("Site alias not installed in ~/.drush. Exiting.");
}


$restore_command = "`{$drushexe}  {$site_alias} sql-connect` < {$sqlfile}";
$upgrade_command = "{$drushexe}  {$site_alias} apigee-migrate --yes";
$remote_host = str_replace(PHP_EOL, "", shell_exec("drush site-alias --component=remote-host {$site_alias}"));
$remote_user = str_replace(PHP_EOL, "", shell_exec("drush site-alias --component=remote-user {$site_alias}"));
$remote_prefix = "{$remote_user}@{$remote_host}";
$drush_command_source = dirname(__FILE__)."/apigee.drush.inc";
$drush_command_destination = "{$remote_prefix}:.drush";
$rsync_destination = "{$remote_prefix}:code/sites/default";
$files_source = dirname($manifest)."/{$distro_dir}/{$instance['files-public']}";
$rsync_source = "{$files_source}/*";

try {
  rsync_command($drush_command_source, $drush_command_destination);
  display_step("Clearing Drush command cache.");  
  passthru("{$drushexe} {$site_alias} cc drush", $output);
  display_step("Emptying Database.");
  passthru("{$drushexe} {$site_alias} apigee-restore", $output);
  display_step("Running: {$restore_command}.");
  passthru($restore_command, $output);
  display_step("Running: {$upgrade_command}.");
  passthru($upgrade_command, $output);
  display_step("Removing files CSS and JS files.");
  passthru("rm -Rfv {$files_source}/js", $output);
  passthru("rm -Rfv {$files_source}/css", $output);
  passthru("rm -Rfv {$files_source}/cdn", $output);
  passthru("rm -Rfv {$files_source}/ctools", $output);  
  rsync_command($rsync_source, $rsync_destination);
  if (strpos(".dev", $site_alias)) {
    rsync_command(dirname($manifest)."/{$distro_dir}/sites/{$instance_name}/modules", "{$remote_prefix}:code/sites/all");
    rsync_command(dirname($manifest)."/{$distro_dir}/sites/{$instance_name}/libraries", "{$remote_prefix}:code/sites/all");
    rsync_command(dirname($manifest)."/{$distro_dir}/sites/{$instance_name}/themes", "{$remote_prefix}:code/sites/all");
  }
  display_step("Distro-ficiation complete.");
} catch(Exception $e) {
  print_r(get_defined_vars());
  exit("Exiting: ".$e->getMessage());
}




