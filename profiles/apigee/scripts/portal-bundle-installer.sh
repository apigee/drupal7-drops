#!/bin/bash

###############################################################################
# standalone-bundle-installer.sh - This script is used to install Dev Portal on
# either CentOS or Redhat Enterprise Linux 6.x without a network connection.
###############################################################################

# Get the date of script running
export SCRIPT_RUNDATE="$(date '+%Y%m%d%H%M%S')"

# Get directory this script is running in
SOURCE="${BASH_SOURCE[0]}"
while [ -h "$SOURCE" ]; do # resolve $SOURCE until the file is no longer a symlink
  DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"
  SOURCE="$(readlink "$SOURCE")"
  [[ $SOURCE != /* ]] && SOURCE="$DIR/$SOURCE" # if $SOURCE was a relative symlink, we need to resolve it relative to the path where the symlink file was located
done

# Create tmp directory
SCRIPT_PATH="$( cd -P "$( dirname "$SOURCE" )" && pwd )"
SCRIPT_TEMP_DIR=${SCRIPT_PATH}/tmp
mkdir -p $SCRIPT_TEMP_DIR

export LOGFILE="${SCRIPT_TEMP_DIR}/apigee-drupal-install.log"
export DRUPAL_WEBAPP="/var/www/html"

# Set default values for portal DB -- FORCE USER TO ENTER NAME AND PASSWORD
export PORTAL_DB_NAME="devportal"

##################### load common functions ####################
source ${SCRIPT_PATH}/tasks/common-functions.sh
source ${SCRIPT_PATH}/tasks/checks.sh

# Uncomment to force installation from supplied repositories - bypass network
#HAS_NETWORK=0

# --------------------------------------------------------------------------------------------------
# STEP: BEGAN INSTALLATION
# --------------------------------------------------------------------------------------------------
# Create RPM bundle directory
if [ ! -d ${SCRIPT_PATH}/bundle/devportal-repo ] ; then

  if [ ! -d ${SCRIPT_PATH}/bundle.${PLATFORM_NAME} ] ; then
    echo "ERROR: bundle ${SCRIPT_PATH}/bundle.${PLATFORM_NAME} does not exist."
    exit 1
  fi

  # Move over correct Platform bundle RPMs
  mv ${SCRIPT_PATH}/bundle.${PLATFORM_NAME} ${SCRIPT_PATH}/bundle >> $LOGFILE 2>&1

  # Copy over Apigee RPM to bundle dir
  cp -rp ${SCRIPT_PATH}/bundle.Apigee/* ${SCRIPT_PATH}/bundle/devportal-repo >> $LOGFILE 2>&1

#  # Copy Redhat Bundle Version files
#  if [[ $PLATFORM_NAME == "Redhat" ]] ; then
#    cp -rp ${SCRIPT_PATH}/bundle.${PLATFORM_NAME}.${PLATFORM_MAJOR_VERSION}.${PLATFORM_MAJOR_RELEASE}/* ${SCRIPT_PATH}/bundle/devportal-repo >> $LOGFILE 2>&1
#  fi
fi

found_bundle=0
RPM_FILENAME="$( cd bundle/devportal-repo; shopt -s nullglob; echo apigee-drupal-*.rpm )"
if [ -z $RPM_FILENAME ] ; then
    while [ $found_bundle -ne 1 ]; do
      question "In what directory did you untar the install bundle?" BUNDLE_ROOT String "`pwd`"
      top_dir="$( echo $BUNDLE_ROOT | cut -d '/' -f1 )"
      if [[ "${top_dir}" = "." || "${top_dir}" = ".." ]] ; then
        echo "Relative paths are not allowed; please enter the absolute path."
        found_bundle=2;
      elif [ -d ${BUNDLE_ROOT}/bundle/devportal-repo ] ; then
        found_bundle=1
      fi
      if [ $found_bundle -eq 0 ]; then
        display "Bundle was not found in $BUNDLE_ROOT; please check and try again."
      fi
    done
    RPM_FILENAME="$( cd ${BUNDLE_ROOT}/bundle/devportal-repo; shopt -s nullglob; echo apigee-drupal-*.rpm )"
else
    BUNDLE_ROOT="${SCRIPT_PATH}"
fi

cat <<EOF > /etc/yum.repos.d/devportal.repo
[devportal]
name=Apigee Dev Portal Installation CDROM
baseurl=file://${BUNDLE_ROOT}/bundle/devportal-repo
enabled=0
EOF

yum clean all >> $LOGFILE 2>&1

display "
Apigee Developer Channel Services Installation"

display_step "Install Apigee Developer Web Portal"
question "Would you like to install Web Portal (PHP, Apache, Drupal) on this system?" INSTALL_WEBPORTAL Yn
if [[ $INSTALL_WEBPORTAL == "Y" || $INSTALL_WEBPORTAL == "y" ]]; then

   if [ -d ${DRUPAL_WEBAPP}/sites ] ; then
     display_error "An Apigee Portal installation already exist here: $DRUPAL_WEBAPP
       Please refer to OPDK-Dev-Portal-Upgrade-Guide.pdf for the upgrade procedure."
     exit 1
   fi

  display_step "Installing a default Apigee Portal here: $DRUPAL_WEBAPP
  Please be patient, this could take several minutes..."

  if [[ $HAS_NETWORK == 1 ]] ; then
    # Add the server optional channel by using the commands:
    yum install -y yum-utils >> $LOGFILE 2>&1
    yum-config-manager --enable rhel-6-server-optional-rpms >> $LOGFILE 2>&1

    #enable the EPEL (Extra Packages for Enterprise Linux) repo:
    rpm -Uvh http://dl.iuscommunity.org/pub/ius/stable/${PLATFORM_NAME}/6/x86_64/epel-release-6-5.noarch.rpm >> $LOGFILE 2>&1
    yum install -y ${BUNDLE_ROOT}/bundle/devportal-repo/$RPM_FILENAME >> $LOGFILE 2>&1
  else
    # Add the server optional channel by using the commands:
    yum --disablerepo=\* --enablerepo=devportal -y --nogpgcheck install yum-utils >> $LOGFILE 2>&1
    yum-config-manager --enable rhel-6-server-optional-rpms >> $LOGFILE 2>&1

    #enable the EPEL (Extra Packages for Enterprise Linux) repo:
    yum --disablerepo=\* --enablerepo=devportal -y --nogpgcheck install epel-release-6-5.noarch.rpm  >> $LOGFILE 2>&1
    yum --disablerepo=\* --enablerepo=devportal -y --nogpgcheck install apigee-drupal >> $LOGFILE 2>&1
  fi

  # Ensure latest OpenSSL is installed - Heartblead fix
  yum --disablerepo=\* --enablerepo=devportal -y --nogpgcheck update openssl >> $LOGFILE 2>&1

  if [ `egrep -c '^;date.timezone =$' /etc/php.ini` -eq 1 ] ; then
    display "Setting up the timezone in php.ini"
    php_timezone=`egrep '^ZONE=' /etc/sysconfig/clock | cut -d '"' -f2`  >> $LOGFILE 2>&1
    sed -i -e "s:^;date.timezone =:date.timezone = ${php_timezone}:g" /etc/php.ini  >> $LOGFILE 2>&1
  fi

  display "Making sure Apache runs at startup..."
  chkconfig --levels 35 httpd on >> $LOGFILE 2>&1

  if php < /dev/null > /dev/null 2>&1  ; then
    display "PHP successfully installed!"
  else
    display_error "PHP installation failed.  Please refer to logs at:
    $LOGFILE"
    exit 1
  fi

  if ps ax | grep -v grep | grep 'httpd' > /dev/null ; then
    display "Apache (httpd) successfully installed!"
  else
    display_error "Apache installation failed.  Please refer to logs at:
    $LOGFILE"
    exit 1
  fi

  display "Enabling SELinux outgoing connection policy"
  setsebool -P httpd_can_network_connect 1 >> $LOGFILE 2>&1
fi

# -----------------------------------------------------
# STEP: Install MySQL Database
# -----------------------------------------------------
display_step "MySQL Server"

# Would you like the MYSQL Server installed on this server?
display "
The Dev Portal database can be installed in a local MySQL database or
connect to a remote server.  If you are installing the MySQL database
on a remote machine, select 'n' to bypass MySQL server installation.
You will need to supply the MySQL username, password, and the server hostname.
"

if [[ ! $HAS_NETWORK == 1 ]] ; then
  display "The following rpm's are required to install MySQL:"
  while read line; do
          echo "$line"
  done < "${SCRIPT_PATH}"/bundle/devportal-repo/required_rpms.txt
  display ""
fi

question "Would you like to install MySQL Server on this system?" INSTALL_MYSQL_SERVER Yn

if [[ $INSTALL_MYSQL_SERVER = "Y" ]]; then

  if [[ $HAS_NETWORK == 1 ]] ; then
    display_step "Installing MySQL Server"
    yum install -y mysql mysql-server mysql-libs >> $LOGFILE 2>&1
  else
    found_mysql=0
    while [ $found_mysql -ne 1 ]; do
      question "In what directory did you download the MYSQL RPMs too?" MYSQL_ROOT String "`pwd`"

      if [[ $MYSQL_ROOT == "Q" || $MYSQL_ROOT == "q" ]]; then
        exit 1
      fi

      top_dir="$( echo $MYSQL_ROOT | cut -d '/' -f1 )"
      if [[ "${top_dir}" = "." || "${top_dir}" = ".." ]] ; then
        echo "Relative paths are not allowed; please enter the absolute path."
        found_mysql=2;
      elif [ -d ${MYSQL_ROOT}/ ] ; then
        found_mysql=1
        while read line; do
            if [ ! -f $MYSQL_ROOT/$line ] ; then
                display_error "The required rpm ${line} does not exist in ${MYSQL_ROOT}.";
                found_mysql=0
            else
                cp $MYSQL_ROOT/$line ${BUNDLE_ROOT}/bundle/devportal-repo
            fi
        done < "${BUNDLE_ROOT}"/bundle/devportal-repo/required_rpms.txt
      else
        display_error "${MYSQL_ROOT} is not a valid directory path"
      fi
    done

    display_step "Installing MySQL Server
    Please be patient, this could take several minutes..."
    yum --disablerepo=\* --enablerepo=devportal -y --nogpgcheck install mysql mysql-server mysql-libs >> $LOGFILE 2>&1
  fi

  display_step "Enter Developer Channel Services MySQL connection information"
  question "What is the MySQL database name?" PORTAL_DB_NAME String $PORTAL_DB_NAME
  question "Enter a MySQL database username Dev Portal should connect with" PORTAL_DB_USERNAME String $PORTAL_DB_USERNAME
  question "Enter a MySQL password for the user ${PORTAL_DB_USERNAME}" PORTAL_DB_PASSWORD String $PORTAL_DB_PASSWORD

  if [ `service mysqld status | grep -c 'stopped'` -eq 1 ] ; then
    display "Starting MySQL"
    service mysqld start >> $LOGFILE 2>&1
  else
    display "Restarting MySQL"
    service mysqld restart >> $LOGFILE 2>&1
  fi

  display "Making sure MySQL runs at startup"
  chkconfig --levels 35 mysqld on >> $LOGFILE 2>&1y

  # Make sure database is properly empty. Note that user-to-db associations
  # remain in effect as long as the new db has the same name as the old one.
  mysql -u root -e "DROP DATABASE IF EXISTS ${PORTAL_DB_NAME}"; >> $LOGFILE 2>&1
  mysql -u root -e "CREATE DATABASE ${PORTAL_DB_NAME}"; >> $LOGFILE 2>&1

  # Check to see if user exists
  IS_USER_CREATED=`mysql -u root --skip-column-names  -e "select count(*) from mysql.user where user='devportal' and host='localhost'"` >> $LOGFILE 2>&1

  if [ $IS_USER_CREATED -eq 0 ]; then
    display "Creating MySQL user ${PORTAL_DB_USERNAME}..."
    mysql -u root  -e "CREATE USER '${PORTAL_DB_USERNAME}'@'localhost' IDENTIFIED BY '${PORTAL_DB_PASSWORD}';"
    mysql -u root  -e "GRANT ALL ON ${PORTAL_DB_NAME}.* TO '${PORTAL_DB_USERNAME}'@'localhost';"
    mysql -u root  -e 'FLUSH PRIVILEGES;'
  else
    display "MySQL user ${PORTAL_DB_USERNAME} already exists, updating password."
    mysql -u root  -e "SET PASSWORD FOR '${PORTAL_DB_USERNAME}'@'localhost' = PASSWORD('${PORTAL_DB_PASSWORD}');"
  fi

  mysql_check=$(which mysql)
  if [[ $mysql_check == */mysql ]]
  then
    display "
    MySQL successfully installed!
    Please record your MySQL connection information. You will require it in the next installation phase:
    MySQL Username:       ${PORTAL_DB_USERNAME}
    MySQL Password:       ${PORTAL_DB_PASSWORD}
    MySQL Database name:  ${PORTAL_DB_NAME}
    "
  else
    display_error "MySQL installation failed.  Please refer to logs at:
    $LOGFILE"
  fi
fi

# backup log file
cp $LOGFILE $LOGFILE.$SCRIPT_RUNDATE

display_step "
Navigate to the server URL in a browser: http://localhost. Typically, you will
have already configured a hostname and registered it with your DNS server so
that you do not have to use http://localhost.

The remaining installations steps will be completed from a browser:
1) Configure Developer Portal database
2) Connect Developer Portal to Apigee Edge Management server
3) Create Developer Portal admin user
"
