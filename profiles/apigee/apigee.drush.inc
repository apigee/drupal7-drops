<?php

// avoid error where it's defined twice...

if (!function_exists("apigee_drush_command")) {
  /**
   * Implements hook_drush_command().
   */
  function apigee_drush_command() {
    $items = array();

    $items['apigee-migrate'] = array(
      'description' => 'Migrate a sites archive from Apigee internal to Pantheon distro',
      'callback' => 'apigee_migrate',
      'bootstrap' => DRUSH_BOOTSTRAP_DRUSH, 
      'examples' => array(
        'drush apm @site' => 'run queries against @site to update database from Apigee Hosting',
      ),
      'aliases' => array('apm'),
    );
  
    $items['apigee-restore'] = array(
      'description' => 'Migrate a sites archive from Apigee to Pantheon hosting',
      'callback' => 'apigee_restore',
      'bootstrap' => DRUSH_BOOTSTRAP_DRUSH, 
      'options' => array(
        'file' => 'Path to a file containing the SQL to be run.'
      ),
      'examples' => array(
        'drush [@site] apigee-restore' => 'run queries against @site to update database from Apigee Hosting',
      ),
      'aliases' => array('apr'),
    );

    $items['apigee-liveify'] = array(
      'description' => 'Update site settings in preparation for going live',
      'callback' => 'apigee_livify',
      'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_FULL, 
      'examples' => array(
        'drush @site apigee-liveify' => 'run queries against @site to update database from Apigee Hosting',
      ),
      'aliases' => array('apl'),
    );

    $items['apigee-environment'] = array(
      'description' => 'show the live environment $_ENV variables in .json format',
      'callback' => 'apigee_environment',
      'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_FULL, 
      'examples' => array(
        'drush @site apigee-environment' => 'run queries against @site to update database from Apigee Hosting',
      ),
      'aliases' => array('ape'),
      'arguments' => array(
        "variable" => "The name of the variable to retrieve from the server environment"
      )
    );

    $items['apigee-portal'] = array(
      'description' => 'Create apigee portal from Apigee orgname including Development, Test and Live environment',
      'callback' => 'apigee_portal',
      'bootstrap' => DRUSH_BOOTSTRAP_DRUSH, 
      'examples' => array(
        'drush apigee-portal sears' => 'Create a portal for the apigee production org SEARS including Development, Test and Live environment',
      ),
      'aliases' => array('aport'),
      'arguments' => array(
        "orgname" => "Apigee orgname of the site being created"
      )
    );

    return $items;
  }

  /**
   * run commands necessary for importing a database from apigee hosting to our pantheon distro
   *
   * @return void
   * @author Tom Stovall
   */

  function apigee_migrate() {
    ini_set('memory_limit', -1);
    if (!drush_bootstrap_to_phase(DRUSH_BOOTSTRAP_DRUPAL_DATABASE)) {
      return drush_set_error('DRUPAL_SITE_NOT_FOUND', dt('You need to specify an alias or run this command within a drupal site.'));
    }
    $tables = db_find_tables("%cache%");
    $tables += db_find_tables("%facet%");
    foreach ($tables as $table){
      db_query("truncate {$table}");
      drush_log(dt('Database table :table truncated.', array(":table" => $table)), 'ok');
    }
  
    $tables = db_find_tables("%apachesolr%");
    foreach ($tables as $table){
      db_drop_table($table);    
      drush_log(dt('Database table :table dropped.', array(":table" => $table)), 'ok');
    }
  
    db_query("update system set filename=REPLACE(filename, 'sites/all/modules', 'profiles/apigee/modules') where filename like 'sites/all/modules%';");
    db_query("update system set filename=REPLACE(filename, 'sites/all/themes', 'profiles/apigee/themes') where filename like 'sites/all/themes%';");
    db_query("update registry set filename=REPLACE(filename, 'sites/all/modules', 'profiles/apigee/modules') where filename like 'sites/all/modules%';");
    db_query("update registry set filename=REPLACE(filename, 'sites/all/themes', 'profiles/apigee/themes') where filename like 'sites/all/themes%';");
    db_query("update block_custom set body=replace(body, '/sites/all/themes/apigee_base', '/profiles/apigee/themes/apigee_base')");
    db_query("update block_custom set body=replace(body, '/sites/all/themes/apigee_devconnect', '/profiles/apigee/themes/apigee_devconnect');");
    db_query("update menu_router set include_file=replace(include_file, 'sites/all/modules/contrib', 'profiles/apigee/modules/contrib');");
    db_query("update menu_router set include_file=replace(include_file, 'sites/all/modules/custom', 'profiles/apigee/modules/custom');");
    db_query("update system set weight=-10 where filename like '%view%';");
    db_query("update system set weight=-10 where filename like '%rules%';");
    db_query("update system set status=0 where name like '%apachesolr%';");
    db_query("update system set status=0 where name like '%facet%';");
    db_query("update system set schema_version=-1 where name like '%apachesolr%';");
    db_query("delete from variable where name like 'apachesolr%'");
    db_query("drop table if exists temp_og_users_roles;");
    //TODO: disable vairable_realm, Rules, oAuth
    drush_log(dt('Database updated with distro file locations.'), 'ok');
    variable_set("install_profile", "apigee");
    $base = variable_get("theme_apigee_base_settings", array());
    if (array_key_exists("PRESSFLOW_SETTINGS", $_SERVER)) {
      $pressflow = json_decode($_SERVER['PRESSFLOW_SETTINGS'], true);
      $c = $pressflow['conf'];
    } else {
      $c = array(
        "file_public_path" => "sites/default/files",
        "file_private_path" => "sites/default/private",
        "file_temporary_path" => "sites/default/tmp"
      );
    }
    variable_set("file_public_path", $c['file_public_path']);
    variable_set("file_temporary_path", $c['file_temporary_path']);
    variable_set("file_private_path", $c['file_private_path']);
    drush_log(dt('Install Profile Set.'), 'ok');
    drush_invoke("rr");
    module_load_include("php", "devconnect", "lib/Apigee/Util/Log");
    module_load_include("module", "devconnect");
    
    try{
      drush_include_engine('drupal', 'update', drush_drupal_major_version());
      if (update_main() === FALSE) {
        return FALSE;
      }
    } catch(Exception $e) {
      drush_set_error("APIGEE_MIGRATE", dt("Migration failed. ". $e->getMessage()));
    }
    try {
      module_enable(array("apachesolr", "apachesolr_search"));
      module_load_include("profile", "apigee");
      apigee_install_pantheon_push_solr();
    } catch(Exception $e) {
      drush_set_error("APIGEE_MIGRATE", dt("Unable to turn on apachesolr modules. ". $e->getMessage()));
      exit($e->getMessage());
    }
    drush_cache_command_clear("menu");
    drush_drupal_cache_clear_all();
    drush_log(dt('The database has been imported and all the caches cleared.'), 'success');
  
  }

  /**
   * Rollback the database for re-doing the import. use option --file= to set the mysql db import file
   *
   * @return void
   * @author Tom Stovall
   */

  function apigee_restore() {
    ini_set('memory_limit', -1);
    if (!drush_bootstrap_to_phase(DRUSH_BOOTSTRAP_DRUPAL_DATABASE)) {
      return drush_set_error('DRUPAL_SITE_NOT_FOUND', dt('You need to specify an alias or run this command within a drupal site.'));
    }
    $tables = db_find_tables("%");
    if (count($tables)) {
      foreach ($tables as $table){
        db_drop_table($table);
        drush_log(dt('Database table :table dropped.', array(":table" => $table)), 'ok');
      }
    } else {
      drush_log(dt('No tables to drop.'), 'ok');
    } 
    $file = drush_get_option("file", null);
    if ($file) {
      try{
        $sql = file($file);
        foreach($file as $line) { db_query($line); }
      } catch(Exception $e) {
        drush_set_error("APIGEE_MIGRATE", dt("Unable to restore from :filename", array(":filename" => $file) ));
      }
    } 
  }
  
  function apigee_liveify() {
    ini_set('memory_limit', -1);
    
  }
  
  
  function apigee_environment($variable = "PRESSFLOW_SETTINGS") {
    
    $pf = json_decode($_SERVER['PRESSFLOW_SETTINGS'], true);
    
    if (array_key_exists($variable, $_SERVER)) {
      drush_print($_SERVER[$variable], 0, null, false);
    } else {
      foreach($pf as $key => $value) {
        if (@array_key_exists($variable, $value)) {
          drush_print($value[$variable], 0, null, false);
        }
      }
    }
    // drush_print(print_r(get_defined_vars()));

  }
  
  function apigee_portal($orgname = null) {
    
    // TODO:check for terminus
    
    if ($orgname == null) {
      return drush_set_error('DRUPAL_NO_ORGNAME', dt('You must specify an orgname to create a site.'));
    }
    drush_set_option("organization", "cb6b74dc-c0a7-4b9a-a1eb-309155c7476a");
    drush_set_option("product", "7ce141f4-c1b2-439b-a0ad-fc90363ece1e");
    drush_set_option("label", $orgname);
    drush_invoke("pauth");
    $session_data = terminus_bootstrap();
    if ($session_data === FALSE) {
      return drush_set_error('DRUPAL_NO_AUTH', dt('You were not authenticated via pantheon.'));
    }
    drush_invoke("psite-create", array($orgname));
    drush_invoke("pantheon-aliases");
    drush_invoke("cc", array("drush"));
    // get UUID of new site
    
    
    // get accounts.apigee.com UN/PW
    
    // create devconnect 4g user
    //curl https://api.enterprise.apigee.com/v1/o/{org}/users -X POST -d '{"emailId":"tomstovall@gail.com","firstName":"Tom","lastName":"Stovall","password":"4G26AsTwBK9T1tTO1Si1kpbA=="}' -H "content-type: application/json"
    
    
    //authorize devconnect 4g user
    //curl 'https://api.enterprise.apigee.com/v1/o/{org}/users/tomstovall@gail.com/userroles' -X POST -d '{"role":[{"name":"orgadmin","organization":"zygot"}],"user":{"emailId":"tomstovall@gail.com","firstName":"Tom","lastName":"Stovall","password":"4G26AsTwBK9T1tTO1Si1kpbA=="}}' -H "content-type: application/json"
    
    //drush_invoke_process("site-install", array("apigee"), "");
    //apigee_install_api_endpoint.devconnect_org=blah
    // apigee_install_api_endpoint.devconnect_endpoint='https://api.enterprise.apigee.com/v1'
    // apigee_install_api_endpoint.devconnect_curlauth='username:password'
    

    
    
  }

}
