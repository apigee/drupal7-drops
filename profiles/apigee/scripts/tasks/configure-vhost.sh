# -----------------------------------------------------
# STEP: Configure Virtual host
# -----------------------------------------------------
display_step "Installing Dev Portal Apache Virtualhost"

HTTPD_VHOST_DIR_PATH=${HTTPD_CONF_DIR}/${HTTPD_VHOST_DIR_NAME}

# Create the vhost conf directory to add virtualhosts to
if [[ -d $HTTPD_VHOST_DIR_PATH ]] ; then
  display "Virtual Host config directory already exists: ${HTTPD_VHOST_DIR_PATH}"
else
  echo "Creating ${HTTPD_VHOST_DIR_PATH}" >> $LOGFILE 2>&1
  mkdir $HTTPD_VHOST_DIR_PATH
fi

# Check to see if Include is already added to httpd.conf
if [[ `cat /etc/httpd/conf/httpd.conf | grep "^Include" | grep -c $HTTPD_VHOST_DIR_NAME` -gt 0 ]]; then
  display "The httpd.conf file already has an include directive for ${HTTPD_VHOST_DIR_NAME}/*.conf files."
  IS_VHOST_HTTPD_INCLUDE_INSTALLED=1
else
  IS_VHOST_HTTPD_INCLUDE_INSTALLED=0
fi

# Add an include directive to httpd.conf for the vhost dir
if [[ $IS_VHOST_HTTPD_INCLUDE_INSTALLED -eq 0 ]]; then
  display "Adding an Include directive for ${HTTPD_VHOST_DIR_NAME}/*.conf virtual host files to httpd.conf"
  if [ -f /etc/httpd/conf/httpd.conf.orig ] ; then
    # Make another backup of httpd.conf w/unique timestamp.
    cp /etc/httpd/conf/httpd.conf /etc/httpd/conf/httpd.conf.devportal-${SCRIPT_RUNDATE}
  else
    # If there isn't a orig backup of httpd, create it.
    cp /etc/httpd/conf/httpd.conf /etc/httpd/conf/httpd.conf.orig
  fi

  # Modify httpd.conf
  echo "
# Include ${HTTPD_VHOST_DIR_NAME}/*.conf for Dev Portal virtual hosts
Include ${HTTPD_VHOST_DIR_NAME}/*.conf" >> /etc/httpd/conf/httpd.conf

fi

# Install the vhost config file for Dev Portal
if [[ -e ${HTTPD_VHOST_DIR_PATH}/devportal.conf ]] ; then
  display "Virtual Host for Dev Portal already exists: ${HTTPD_VHOST_DIR_PATH}/devportal.conf"
  question "Would you like to reinstall the virtualhost configuration for Dev Portal?" INSTALL_VHOST Yn
else
  INSTALL_VHOST=Y
fi

if [[ $INSTALL_VHOST == "Y" ]]; then
  echo "Installing VHOST config file for Dev Portal at: ${HTTPD_VHOST_DIR_PATH}"
  (
    echo "<VirtualHost *:80>"
    echo "  DocumentRoot \"${DRUPAL_WEBROOT}\""
    echo "  <Directory \"${DRUPAL_WEBROOT}\">"
    echo "    Options Indexes FollowSymLinks MultiViews"
    echo "    AllowOverride All"
    echo "    Order allow,deny"
    echo "    Allow from all"
    echo "  </Directory>"
    echo "  ErrorLog /var/log/httpd/devportal_error.log"
    echo "  LogLevel warn"
    echo "  CustomLog /var/log/httpd/devportal_access.log combined"
    echo "</VirtualHost>"
  ) > ${HTTPD_VHOST_DIR_PATH}/devportal.conf
  HTTPD_RELOAD=1
fi

# -----------------------------------------------------
# STEP: Reloading Apache HTTPD
# -----------------------------------------------------
if [[ `service httpd status | grep -c 'stopped'` -eq 1 ]] ; then
  display_step "Starting Apache"
  service httpd start
else
  display_step "Restarting Apache"
  service httpd restart
fi

