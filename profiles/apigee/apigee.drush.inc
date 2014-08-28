<?php

// avoid error where it's defined twice...

if (!function_exists('apigee_drush_command')) {
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
      ),
      'options' => array(
        'portal-password' => 'Password the portal user will use to connect to the management API.'
      )
    );

    $items['apigee-homepage-fix'] = array(
      'description' => 'Fix the path of the homepage image',
      'callback' => 'apigee_homepage_fix',
      'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_FULL,
      'examples' => array(
        'drush @site apigee-homepage-fix' => 'no arguments',
      ),
      'aliases' => array('afx'),
    );

    $items['apigee-create-orgadmin'] = array(
      'description' => 'Create a new admin user for an org.',
      'callback' => 'apigee_create_org_user',
      'bootstrap' => DRUSH_BOOSTRAP_DRUSH,
      'examples' => array(
        'drush apigee-create-orgadmin neworg --user=user@apigee.com --password=mypass'
      ),
      'arguments' => array(
        'orgname' => 'Name of the Apigee org needing an admin'
      ),
      'options' => array(
        'user' => 'Email address of an existing orgadmin for this organization',
        'password' => 'Password of an existing orgadmin for this organization',
        'newuser-password' => 'Password to be used for the newly-created admin user. If none is specified, a random 20-char password will be generated.'
      ),
      'aliases' => array('aco')
    );


    $items['apigee-provision-portal'] = array(
      'description' => 'Create a new admin user for an org and spin up a new instance.',
      'callback' => 'apigee_provision_portal',
      'bootstrap' => DRUSH_BOOSTRAP_DRUSH,
      'examples' => array(
        'drush apigee-provision-portal neworg --user=user@apigee.com --password=mypass'
      ),
      'arguments' => array(
        'orgname' => 'Name of the Apigee org needing a new instance provisioned'
      ),
      'options' => array(
        'user' => 'Email address of an existing orgadmin for this organization',
        'password' => 'Password of an existing orgadmin for this organization',
        'newuser-password' => 'Password to be used for the newly-created admin user. If none is specified, a random 20-char password will be generated.'
      ),
      'aliases' => array('aprov')
    );

    return $items;
  }

  /**
   * run commands necessary for importing a database from apigee hosting to our pantheon distro
   *
   * @return bool
   * @author Tom Stovall
   */
  function apigee_migrate() {
    ini_set('memory_limit', -1);
    if (!drush_bootstrap_to_phase(DRUSH_BOOTSTRAP_DRUPAL_DATABASE)) {
      return drush_set_error('DRUPAL_SITE_NOT_FOUND', dt('You need to specify an alias or run this command within a drupal site.'));
    }
    $tables = db_find_tables("%cache%");
    $tables += db_find_tables("%facet%");
    $tables += array('registry', 'registry_file');
    foreach ($tables as $table) {
      db_query("TRUNCATE {$table}");
      drush_log(dt('Database table :table truncated.', array(":table" => $table)), 'ok');
    }

    $tables = db_find_tables("%apachesolr%");
    foreach ($tables as $table) {
      db_drop_table($table);
      drush_log(dt('Database table :table dropped.', array(":table" => $table)), 'ok');
    }

    db_query("update system set filename=REPLACE(filename, 'sites/all/modules', 'profiles/apigee/modules') where filename like 'sites/all/modules%';");
    db_query("update system set filename=REPLACE(filename, 'sites/all/themes', 'profiles/apigee/themes') where filename like 'sites/all/themes%';");
    //db_query("update registry set filename=REPLACE(filename, 'sites/all/modules', 'profiles/apigee/modules') where filename like 'sites/all/modules%';");
    //db_query("update registry set filename=REPLACE(filename, 'sites/all/themes', 'profiles/apigee/themes') where filename like 'sites/all/themes%';");
    db_query("update block_custom set body=replace(body, '/sites/all/themes/apigee_base', '/profiles/apigee/themes/apigee_base')");
    db_query("update block_custom set body=replace(body, '/sites/all/themes/apigee_devconnect', '/profiles/apigee/themes/apigee_devconnect');");
    db_query("update menu_router set include_file=replace(include_file, 'sites/all/modules/contrib', 'profiles/apigee/modules/contrib');");
    db_query("update menu_router set include_file=replace(include_file, 'sites/all/modules/custom', 'profiles/apigee/modules/custom');");
    db_query("update system set weight=-10 where filename like '%view%';");
    db_query("update system set weight=-10 where filename like '%rules%';");
    db_query("DELETE FROM system WHERE name LIKE '%apachesolr%';");
    db_query("update system set status=0 where name like '%facet%';");
    db_query("delete from variable where name like 'apachesolr%'");
    db_query("drop table if exists temp_og_users_roles;");

    //TODO: disable variable_realm, Rules, oAuth
    drush_log(dt('Database updated with distro file locations.'), 'ok');

    if (array_key_exists("PRESSFLOW_SETTINGS", $_SERVER)) {
      $pressflow = json_decode($_SERVER['PRESSFLOW_SETTINGS'], TRUE);
      $vars = array(
        'file_public_path' => $pressflow['conf']['file_public_path'],
        'file_private_path' => $pressflow['conf']['file_private_path'],
        'file_temporary_path' => $pressflow['conf']['file_temporary_path'],
      );
    }
    else {
      $vars = array(
        "file_public_path" => "sites/default/files",
        "file_private_path" => "sites/default/private",
        "file_temporary_path" => "sites/default/tmp"
      );
    }
    $vars['install_profile'] = 'apigee';
    // Avoid variable_set in case there are errors that drush rr or drush updb
    // needs to fix
    foreach ($vars as $name => $value) {
      db_update('variable')
        ->fields(array('value' => serialize($value)))
        ->condition('name', $name)
        ->execute();
      $GLOBALS['conf'][$name] = $value;
    }

    drush_log(dt('Install Profile Set.'), 'ok');
    drush_invoke("rr");
    /*
    module_load_include('module', 'libraries');
    $path = libraries_get_path('mgmt-api-php-sdk');
    require_once("$path/Apigee/Util/Log.php");
    module_load_include("module", "devconnect");
    */
    try {
      drush_include_engine('drupal', 'update', drush_drupal_major_version());
      if (update_main() === FALSE) {
        return FALSE;
      }
    } catch (Exception $e) {
      drush_set_error("APIGEE_MIGRATE", dt("Migration failed. " . $e->getMessage()));
    }
    drush_cache_command_clear("menu");
    drush_drupal_cache_clear_all();
    drush_log(dt('The database has been imported and all the caches cleared.'), 'success');

  }

  /**
   * Rollback the database for re-doing the import. use option --file= to set the mysql db import file
   *
   * @return bool
   * @author Tom Stovall
   */
  function apigee_restore() {
    ini_set('memory_limit', -1);
    if (!drush_bootstrap_to_phase(DRUSH_BOOTSTRAP_DRUPAL_DATABASE)) {
      return drush_set_error('DRUPAL_SITE_NOT_FOUND', dt('You need to specify an alias or run this command within a drupal site.'));
    }
    $tables = db_find_tables("%");
    if (count($tables)) {
      foreach ($tables as $table) {
        db_drop_table($table);
        drush_log(dt('Database table :table dropped.', array(":table" => $table)), 'ok');
      }
    }
    else {
      drush_log(dt('No tables to drop.'), 'ok');
    }
    $file = drush_get_option("file", NULL);
    if ($file) {
      try {
        $sql = file($file);
        foreach ($sql as $line) {
          db_query($line);
        }
      } catch (Exception $e) {
        return drush_set_error("APIGEE_MIGRATE", dt("Unable to restore from :filename", array(":filename" => $file)));
      }
    }
    return TRUE;
  }

  function apigee_liveify() {
    ini_set('memory_limit', -1);

  }


  function apigee_environment($variable = "PRESSFLOW_SETTINGS") {

    $pf = json_decode($_SERVER['PRESSFLOW_SETTINGS'], TRUE);

    if (array_key_exists($variable, $_SERVER)) {
      drush_print($_SERVER[$variable], 0, NULL, FALSE);
    }
    else {
      foreach ($pf as $value) {
        if (@array_key_exists($variable, $value)) {
          drush_print($value[$variable], 0, NULL, FALSE);
        }
      }
    }
    // drush_print(print_r(get_defined_vars()));

  }

  /**
   * Creates a portal instance on Pantheon.
   *
   * @param string|null $orgname
   * @return bool
   */
  function apigee_portal($orgname = NULL) {

    $portal_password = trim(drush_get_option('portal-password', NULL));
    if (empty($portal_password)) {
      $portal_password = drush_prompt(dt('Password for portal user'), NULL, FALSE, TRUE);
    }

    if ($orgname == NULL) {
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

    drush_invoke_process("site-install", array(
      "apigee",
      "apigee_install_api_endpoint.devconnect_org={$orgname}",
      "apigee_install_api_endpoint.devconnect_endpoint='https://api.enterprise.apigee.com/v1'",
      "apigee_install_api_endpoint.devconnect_curlauth='dc_devportal+{$orgname}@apigee.com:{$portal_password}'"
    ), "");

    return TRUE;
  }


  function apigee_homepage_fix() {
    variable_set("text_variable", str_replace('/sites/all', "/profiles/apigee", variable_get("text_variable", FALSE)));
    drush_log(dt('homepage variable set: %variable', array("%variable" => variable_get("text_variable", "text_variable set incorrectly"))), 'ok');
    return TRUE;
  }

  /**
   * Creates a randomized password.
   * 
   * @return string
   */
  function _apigee_create_password() {
    return base64_encode(drupal_random_bytes(15));
  }

  /**
   * Gets user/password for both orgadmin and to-be-created portal users.
   * 
   * @param string $orgname
   * @return array|bool
   */
  function _apigee_get_org_create_options($orgname) {
    // get user/password of already-existing orgadmin
    $user = drush_get_option('user', NULL);
    $password = drush_get_option('password', NULL);

    if (empty($user)) {
      $user = drush_prompt(dt('Email of existing orgadmin: '), NULL, TRUE, FALSE);
    }
    if (empty($password)) {
      $password = drush_prompt(dt('Password of existing orgadmin: '), NULL, TRUE, TRUE);
    }
    if (empty($user) || empty($password)) {
      return drush_set_error('APIGEE_NO_USER_PASS', dt('You must specify an orgadmin user and password to spin up an instance.'));
    }
    if (!valid_email_address($user)) {
      return drush_set_error('APIGEE_INVALID_USER', dt('The orgadmin user that was specified does not appear to be a valid email address.'));
    }

    $portal_pass = drush_get_option('newuser-password', NULL);
    if (empty($org_pass)) {
      $portal_pass = _apigee_create_password();
    }
    $portal_user = 'dc_devportal+' . $orgname . '@apigee.com';

    return array(
      'orgadmin_user' => $user,
      'orgadmin_pass' => $password,
      'portal_user' => $portal_user,
      'portal_pass' => $portal_pass
    );

  }

  /**
   * Drush task callback: creates an orgadmin user in Enterprise.
   * 
   * @param string $orgname
   * @return bool
   */
  function apigee_create_org_user($orgname = NULL) {
    if ($orgname == NULL) {
      return drush_set_error('APIGEE_NO_ORGNAME', dt('You must specify an orgname to create an orgadmin user.'));
    }

    $info = _apigee_get_org_create_options($orgname);
    if ($info === FALSE) {
      return FALSE;
    }
    return _apigee_create_org_user($orgname, $info['orgadmin_user'], $info['orgadmin_pass'], $info['portal_user'], $info['portal_pass']);
  }

 /**
   * Creates an org user and spins up a new portal for it.
   *
   * This is a combination of drush aco and drush aport.
   *
   * @param string $orgname
   * @return bool
   */
  function apigee_provision_portal($orgname = NULL) {
    if ($orgname == NULL) {
      return drush_set_error('APIGEE_NO_ORGNAME', dt('You must specify an orgname to spin up a site.'));
    }
    $info = _apigee_get_org_create_options($orgname);
    if ($info === FALSE) {
      return FALSE;
    }
    $status = _apigee_create_org_user($orgname, $info['orgadmin_user'], $info['orgadmin_pass'], $info['portal_user'], $info['portal_pass']);
    if (!$status) {
      return FALSE;
    }

    drush_set_option('portal-password', $info['portal_pass']);
    drush_invoke('aport', array($orgname));
    return TRUE;
  }

  /**
   * Heavy lifting: actually creates the orgadmin user.
   * 
   * @param string $orgname
   * @param string $orgadmin_user
   * @param string $orgadmin_pass
   * @param string $portal_user
   * @param string $portal_pass
   * @return bool
   */
  function _apigee_create_org_user($orgname, $orgadmin_user, $orgadmin_pass, $portal_user, $portal_pass) {
    $org_url = 'https://api.enterprise.apigee.com/v1/o/' . rawurlencode($orgname);

    $ch = curl_init();
    curl_setopt($ch, CURLOPT_HTTPGET, TRUE);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, TRUE);
    curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, TRUE);
    curl_setopt($ch, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
    curl_setopt($ch, CURLOPT_URL, $org_url);
    curl_setopt($ch, CURLOPT_USERPWD, "$orgadmin_user:$orgadmin_pass");
    curl_exec($ch);
    $code = curl_getinfo($ch, CURLINFO_HTTP_CODE);

    if ($code != 200) {
      return drush_set_error('DRUPAL_INVALID_ORG', dt('The org @org does not exist, or the user you supplied does not have credentials on that org.', array('@org' => $orgname)));
    }

    curl_setopt($ch, CURLOPT_URL, $org_url . '/users/' . rawurlencode($portal_user));
    curl_exec($ch);
    $code = curl_getinfo($ch, CURLINFO_HTTP_CODE);

    if ($code == 200) {
      // Org user already exists
      return drush_set_error('DRUPAL_USER_EXISTS', dt('The user @user already exists in org @org. Please find the correct password for this user, then try "drush aport @org --portal-password=SuperSecret".', array('@user' => $portal_user, '@org' => $orgname)));
    }

    $data = json_encode(array(
      'emailId' => $portal_user,
      'firstName' => 'DEVPORTAL_USER',
      'lastName' => 'DO_NOT_DELETE',
      'password' => $portal_pass
    ));

    curl_setopt($ch, CURLOPT_POST, TRUE);
    curl_setopt($ch, CURLOPT_URL, $org_url . '/users');
    curl_setopt($ch, CURLOPT_HTTPHEADER, array('Content-Type: application/json'));
    curl_setopt($ch, CURLOPT_POSTFIELDS, $data);
    curl_exec($ch);
    $code = curl_getinfo($ch, CURLINFO_HTTP_CODE);
    if (floor($code / 100) != 2) {
      return drush_set_error('DRUPAL_CANNOT_CREATE_USER', dt('The user @user could not be created in org @org. Are you sure you used the credentials of an orgadmin?', array('@user' => $portal_user, '@org' => $orgname)));
    }

    drush_log(dt('User @user created with password "@pass".', array('@user' => $portal_user, '@pass' => $portal_pass)), 'success');

    $data = json_encode(array(
      'role' => array(
        array('name' => 'orgadmin')
      )
    ));

    curl_setopt($ch, CURLOPT_URL, $org_url . '/users/' . rawurlencode($portal_user) . '/userroles');
    curl_setopt($ch, CURLOPT_POSTFIELDS, $data);
    curl_exec($ch);
    $code = curl_getinfo($ch, CURLINFO_HTTP_CODE);
    if (floor($code / 100) != 2) {
      return drush_set_error('DRUPAL_CANNOT_ASSIGN_ROLE', dt('The orgadmin role could not be assigned to user @user.', array('@user' => $portal_user)));
    }
    drush_log(dt('Orgadmin role assigned to user @user.', array('@user' => $portal_user)), 'success');
    return TRUE;
  }
}
