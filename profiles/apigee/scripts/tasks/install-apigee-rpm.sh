display_step "Installing Apigee Dev Portal"
if [[ $OPDK_STANDALONE -eq 1 ]]; then
  yum --disablerepo=\* --enablerepo=devportal -y --nogpgcheck install apigee-drupal >> $LOGFILE 2>&1
else
  rpm -ivh --replacepkgs ${RPM_LOCAL_PATH}/${RPM_FILENAME}
fi

mkdir --mode=755 -p ${DRUPAL_WEBROOT}/sites/default/{files,tmp,private}
if [[ $has_valid_settings = "Y" ]]; then
  display_step "Restoring ${DRUPAL_WEBROOT}/sites/default/settings.php file..."
  mv /tmp/settings.php ${DRUPAL_WEBROOT}/sites/default/
elif [[ -n $PORTAL_DB_NAME ]]; then
  display_step "Creating ${DRUPAL_WEBROOT}/sites/default/settings.php file..."

  # Create drupal site config
  (
  echo '<?php'
  echo '$databases = array('
  echo "  'default' => array ("
  echo "    'default' => array ("
  echo "      'database' => '${PORTAL_DB_NAME}',"
  echo "      'username' => '${PORTAL_DB_USERNAME}',"
  echo "      'password' => '${PORTAL_DB_PASSWORD}',"
  echo "      'host' => '${PORTAL_DB_HOSTNAME}',"
  echo "      'port' => '${PORTAL_DB_PORT}',"
  echo "      'driver' => 'mysql',"
  echo "      'prefix' => '',"
  echo "    ),"
  echo "  ),"
  echo ");"
  echo '$update_free_access = FALSE;'
  echo '$drupal_hash_salt = "";'
  echo "ini_set('session.gc_probability', 1);"
  echo "ini_set('session.gc_divisor', 100);"
  echo "ini_set('session.gc_maxlifetime', 200000);"
  echo "ini_set('session.cookie_lifetime', 2000000);"
  ) > "${DRUPAL_WEBROOT}/sites/default/settings.php"
fi

if [ ! -d /etc/drush ] ; then
  mkdir -p /etc/drush
fi
question "What is the fully-qualified hostname for this Dev Portal instance?:" DEV_PORTAL_HOSTNAME String "localhost"
if [ ! -f /etc/drush/devportal.aliases.drushrc.php ] ; then
  (
    echo "<?php"
    echo "\$aliases['devportal'] = array("
    echo "  'root' => '${DRUPAL_WEBROOT}',"
    echo "  'uri' => '${DEV_PORTAL_HOSTNAME}"
    echo ");"
  ) > /etc/drush/devportal.aliases.drushrc.php
  display "A drush alias of 'devportal' has been created for this Drupal instance."
fi
if [[ `grep -c "${DEV_PORTAL_HOSTNAME}" /etc/hosts` -eq 0 ]] ; then
  echo "127.0.0.1 ${DEV_PORTAL_HOSTNAME}" >> /etc/hosts
  echo "::1 ${DEV_PORTAL_HOSTNAME}" >> /etc/hosts
  display "Added ${DEV_PORTAL_HOSTNAME} to /etc/hosts"
fi
