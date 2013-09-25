#!/bin/bash

###############################################################################
# standalone-bundle-installer.sh - This script is used to install Dev Portal on
# either CentOS or Redhat Enterprise Linux 6.x without a network connection.
###############################################################################

if [ "$(id -u)" != "0" ]; then
echo "This script must be run as root" 1>&2
   exit 1
fi

# Clean up function called if signal caught
function cleanup_error(){

  # Call function to remove tmp directory
  remove_tmp_dir

  display_error "

 ===> Exiting, ERROR!
 The actions of this installer are written to a log here: 
   ${LOGFILE}
 If you need support during this installation,
 please include the logfile in your communication.
 
 Here are the last few lines of the logfile for your convenience:

"
  tail -n 20 $LOGFILE
  
  display_header "
  "
  exit 1

}

function cleanup_noerror() {

  # Call function to remove tmp directory
  remove_tmp_dir

  display_header "

  GREAT SUCCESS! You're good to go.

  Install directory: ${DRUPAL_WEBAPP}
  Database connection settings: ${DRUPAL_WEBAPP}/sites/default/settings.php
  Database name: ${PORTAL_DB_NAME}
  Database user: ${PORTAL_DB_USERNAME}

  The actions of this installer are written to a log here:
    ${LOGFILE}
  If you need support during this installation, please include the logfile in
  your communication.
"

  exit 0
}

function cleanup_ctlc() {
  # Call function to remove tmp directory
  remove_tmp_dir
  exit 1
}

# Remove tmp directory when exiting
function remove_tmp_dir() {
  # Remove tmp directory
  if [[ -d $SCRIPT_TEMP_DIR ]]; then
    rm -rf $SCRIPT_TEMP_DIR
  fi
}

# Display a multiline string, because we can't rely on `echo` to do the right thing.
#
# Arguments:
# 1. Text to display.
display() {
  display_nonewline "${1?}\n"
}

# Display a multiline string without a trailing newline.
#
# Arguments:
# 1. Text to display.
display_nonewline() {
  printf -- "${1?}"
}

# Display a newline
display_newline() {
  display ''
}

display_header() {
  display_separator
  display "${1?}"
  display_separator
}

# Display an error message to STDERR, but do not exit.
#
# Arguments:
# 1. Message to display.
display_error() {
  printf "\e[31m !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n"
  printf "\e[31m ERROR: ${1?}\n\n"
  printf "\e[31m !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n"
  printf "\e[0m"
}

# Display a minor separator line.
display_separator() {
  display "*******************************************************************************"
}

# Invoke the exception_handler on CTRL-C or "set -e" errors.
register_exception_handler() {
  trap cleanup_error ERR
  trap cleanup_noerror TERM HUP
  trap cleanup_ctlc SIGINT
}

# Display a step in the installation process.
#
# Arguments:
# 1. Description of the step, e.g. "PERFORM INSTALLATION"
# 2. Display newline afterwards? Defaults to 'y'.
display_step() {
  t_display_step__description="${1?}"
  t_display_step__newline="${2:-"y"}"

  if [ -z "${DISPLAY_STEP__NUMBER:-""}" ]; then
    DISPLAY_STEP__NUMBER=1
  else
    DISPLAY_STEP__NUMBER=$(( 1 + ${DISPLAY_STEP__NUMBER?} ))
  fi

  display_newline
  display_separator
  display_newline
  display "STEP ${DISPLAY_STEP__NUMBER?}: ${t_display_step__description?}"

  if [ y = "${t_display_step__newline?}" ]; then
    display_newline
  fi
  display_separator
}

# Display a question, make the user answer it, and set a variable with their answer.
#
# Arguments:
# 1. Question text to display, e.g. "What's your favorite color?"
# 2. Name of the variable to export, e.g. "q_favorite_color"
# 3. Kind of question, e.g. "Yn" to show a 'Y/n' prompt that defaults to 'yes', "yN" to show a y/N prompt that defaults to 'no', "String" for a mandatory string response, "StringOrBlank" for an optional string response.
# 4. Default answer, optional. Supported for "String" questions.
question() {
  question_question="${1?}"
  question_name="${2?}"
  question_kind="${3?}"
  question_default="${4:-""}"

  question_message="?? ${question_question?} "
  case "${question_kind?}" in
    Yn)
      question_message="${question_message?}[Y/n] "
      ;;
    StringOrBlank)
      question_message="${question_message?}[Default: (blank)] "
      ;;
    String*)
      if [ ! -z "${question_default?}" ]; then
        question_message="${question_message?}[Default: ${question_default?}] "
      fi
      ;;
    Port)
      if [ ! -z "${question_default?}" ]; then
        question_message="${question_message?}[Default: ${question_default?}] "
      fi
      ;;
    *)
      display_failure "Invalid question kind: ${question_kind?}"
      ;;
  esac

  # Try to load the answer from an existing variable, e.g. given name "q" look at variable "$q".
  eval question_answered=\$"${question_name:-""}"
  question_defined=0
  question_success=n
  until [ y = "${question_success?}" ]; do
    echo "${question_message?}" || display 0
    display_nonewline " "

    read question_response

    case "${question_kind?}" in
      Yn)
        if [ -z "${question_response?}" -o y = "${question_response?}" -o Y = "${question_response?}" ]; then
          question_answer=y
          question_success=y
        elif [ n = "${question_response?}" -o N = "${question_response?}" ]; then
          question_answer=n
          question_success=y
        else
          display_error 'Answer must be either "y", "n" or <ENTER> for "y"'
        fi
        ;;
      String)
        if [ -z "${question_response?}" -a ! -z "${question_default?}" ]; then
          question_answer="${question_default?}"
          question_success=y
        elif [ ! -z ${question_response?} ]; then
          question_answer="${question_response?}"
          question_success=y
        else
          display_error 'Answer must be a string'
        fi
        ;;
      StringOrBlank)
        question_answer="${question_response?}"
        question_success=y
        ;;
      Port)
        if [ -z "${question_response?}" -a ! -z "${question_default?}" ]; then
          question_answer="${question_default?}"
          question_success=y
        else
          if [ ${question_response?} -gt 0 -a ${question_response?} -lt 65536 2>/dev/null ]; then
            question_answer="${question_response?}"
            question_success=y
          else
            display_error 'Answer must be a valid port number in the range 1-65535'
          fi
        fi
        ;;
      *)
        ;;
    esac

  done
  eval "${question_name?}='${question_answer?}'"
}

# Turn of case sensitive matching for our string compares
shopt -s nocasematch

# Get the date of script running
export SCRIPT_RUNDATE="$(date '+%Y-%m-%d-%H.%M.%S')"

DEVCONNECT_ENDPOINT="https://api.enterprise.apigee.com/v1"
DEVCONNECT_ORG="my-org"
DEVCONNECT_AUTH="<username>:<password>"
# For no-network install, we may not be able to verify.
question "What is the URI of the Apigee Management API Endpoint:" DEVCONNECT_ENDPOINT String $DEVCONNECT_ENDPOINT
question "What is the Apigee Organization name:" DEVCONNECT_ORG String $DEVCONNECT_ORG
question "What is the UN:PW for the management API Endpoint?" DEVCONNECT_AUTH String $DEVCONNECT_AUTH

# Get directory this script is running in
SOURCE="${BASH_SOURCE[0]}"
while [ -h "$SOURCE" ]; do # resolve $SOURCE until the file is no longer a symlink
  DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"
  SOURCE="$(readlink "$SOURCE")"
  [[ $SOURCE != /* ]] && SOURCE="$DIR/$SOURCE" # if $SOURCE was a relative symlink, we need to resolve it relative to the path where the symlink file was located
done

found_bundle=0
while [ $found_bundle -eq 0 ]; do
  question "In what directory did you untar the install bundle?" BUNDLE_ROOT String "./bundle"
  if [ -d ${BUNDLE_ROOT}/bundle/devportal-repo ] ; then
    found_bundle=1  
  fi
  if [ $found_bundle -eq 0 ]; then
    display "Bundle was not found in $BUNDLE_ROOT; please check and try again."
  fi
done

# Create tmp directory
SCRIPT_PATH="$( cd -P "$( dirname "$SOURCE" )" && pwd )"
SCRIPT_TEMP_DIR=${SCRIPT_PATH}/tmp
mkdir -p $SCRIPT_TEMP_DIR

export LOGFILE="${SCRIPT_TEMP_DIR}/apigee-drupal-install.log"
export DRUPAL_WEBAPP="/var/www/html"

export HTTPD_CONF_DIR="/etc/httpd"
export HTTPD_VHOST_DIR_NAME="vhosts"

export PORTAL_DB_PASSWORD=`openssl rand -base64 12 | sed -e "s/[^0-9a-zA-Z]//g"`
export PORTAL_DB_RANDUSER=`openssl rand -base64 8 | sed -e "s/[^0-9a-zA-Z]//g"`
export PORTAL_DB_USERNAME="user-${PORTAL_DB_RANDUSER}"
export PORTAL_DB_NAME="devportal"
export PORTAL_DB_HOSTNAME="localhost"
export PORTAL_DB_PORT=3306

echo -n "" > $LOGFILE

echo "*************************************************************************"
echo " This script will install a default Apigee Portal here:"
echo " $DRUPAL_WEBAPP"
echo "*************************************************************************"


cat <<EOF > /etc/yum.repos.d/devportal.repo
[devportal]
name=Apigee Dev Portal Installation CDROM
baseurl=file://${BUNDLE_ROOT}/bundle/devportal-repo
enabled=0
EOF

yum clean all >> $LOGFILE 2>&1

# base RPMs install
display_step "Installing required packages"
yum --disablerepo=\* --enablerepo=devportal -y --nogpgcheck install pcre-devel wget gcc make >> $LOGFILE 2>&1
yum --disablerepo=\* --enablerepo=devportal -y --nogpgcheck install php php-gd \
  php-mbstring php-pdo php-xml php-mysql php-devel php-mcrypt php-pear httpd \
  mysql php-pecl-apc >> $LOGFILE 2>&1
yum --disablerepo=\* --enablerepo=devportal -y --nogpgcheck apigee-drupal >> $LOGFILE 2>&1

if [ `egrep -c '^;date.timezone =$' /etc/php.ini` -eq 1 ] ; then
  display_step "Setting up the timezone in php.ini"
  php_timezone=`egrep '^ZONE=' /etc/sysconfig/clock | cut -d '"' -f2`
  sed -i -e "s:^;date.timezone =:date.timezone = ${php_timezone}:g" /etc/php.ini
fi

# -----------------------------------------------------
# STEP: Install MySQL Database
# -----------------------------------------------------
display_step "MySQL Server"
# Would you like the MYSQL Server installed on this server?
display_header "

The Dev Portal database can be installed in a local MySQL database or
connect to a remote server.  If you are installing the server on a remote
machine, you will need to supply the MySQL username, password, and the server
hostname later in this script.  Please set up the MySQL server on the database
server and have this information available before continuing.
"
question "Would you like to install MySQL Server on this system?" INSTALL_MYSQL_SERVER Yn

if [[ $INSTALL_MYSQL_SERVER == "Y" ]]; then
  display_step "Installing MySQL Server"
  yum --disablerepo=\* --enablerepo=devportal -y --nogpgcheck install mysql-server
fi

# -----------------------------------------------------
# STEP: Set MySQL and Apache to run at startup
# -----------------------------------------------------
if [[ $INSTALL_MYSQL_SERVER == "Y" ]]; then
  display_step "Making sure MySQL and Apache run at startup"
  chkconfig --levels 35 mysqld on >> $LOGFILE 2>&1
  chkconfig --levels 35 httpd on >> $LOGFILE 2>&1
else
  display_step "Making sure Apache runs at startup..."
  chkconfig --levels 35 httpd on >> $LOGFILE 2>&1
fi

display_step "Installing Drush"
pear channel-add ${BUNDLE_ROOT}/bundle/drush/channel.xml >> $LOGFILE 2>&1
pear install "drush/drush-5.8.0.tgz" >> $LOGFILE 2>&1
display_step "Installing Drush extensions"
mkdir -p ~/.drush
cd ~/.drush
tar xzf ${BUNDLE_ROOT}/bundle/drush/registry_rebuild*.tar.gz

HTTPD_VHOST_DIR_PATH=${HTTPD_CONF_DIR}/${HTTPD_VHOST_DIR_NAME}

# Create the vhost conf directory to add virtualhosts to
if [ ! -d $HTTPD_VHOST_DIR_PATH ] ; then
  mkdir $HTTPD_VHOST_DIR_PATH
fi

IS_VHOST_HTTPD_INCLUDE_INSTALLED=`cat /etc/httpd/conf/httpd.conf | grep "^Include" | grep -c $HTTPD_VHOST_DIR_NAME`
# Add an include directive to httpd.conf for the vhost dir
if [ $IS_VHOST_HTTPD_INCLUDE_INSTALLED -eq 0 ]; then
  display_step "Adding an Include directive for ${HTTPD_VHOST_DIR_NAME}/*.conf virtual host files to httpd.conf"
  cp ${HTTPD_CONF_DIR}/httpd.conf ${HTTPD_CONF_DIR}/httpd.conf.orig
  echo "# Include ${HTTPD_VHOST_DIR_NAME}/*.conf for Dev Portal virtual hosts" >> ${HTTPD_CONF_DIR}/httpd.conf
  echo "Include ${HTTPD_VHOST_DIR_NAME}/*.conf" >> ${HTTPD_CONF_DIR}/httpd.conf
fi

echo "Installing VHOST config file for Dev Portal at: ${HTTPD_VHOST_DIR_PATH}"
(
  echo "<VirtualHost *:80>"
  echo "  DocumentRoot \"${DRUPAL_WEBAPP}\""
  echo "  <Directory \"${DRUPAL_WEBAPP}\">"
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

# -----------------------------------------------------
# STEP: Reloading Apache HTTPD
# -----------------------------------------------------
display_step "Starting/Restarting Apache and MySQL"
if [ `service httpd status | grep -c 'stopped'` -eq 1 ] ; then
  display "Starting Apache"
  service httpd start
else
  display "Restarting Apache"
  service httpd restart
fi
if [ `service mysqld status | grep -c 'stopped'` -eq 1 ] ; then
  display "Starting MySQL"
  service mysqld start
else
  display "Restarting MySQL"
  service mysqld restart
fi
if [[ $INSTALL_MYSQL_SERVER == "Y" ]]; then

  question "What is the MySQL database name?" PORTAL_DB_NAME String "devportal"
  question "Enter a MySQL database username Dev Portal should connect with" PORTAL_DB_USERNAME String "devportal"
  question "Enter a MySQL password for the user ${PORTAL_DB_USERNAME}" PORTAL_DB_USER_PASSWORD String

  # Make sure database is properly empty. Note that user-to-db associations
  # remain in effect as long as the new db has the same name as the old one.
  mysql -u root -e "DROP DATABASE IF EXISTS ${PORTAL_DB_NAME}";
  mysql -u root -e "CREATE DATABASE ${PORTAL_DB_NAME}";

  # Check to see if user exists
  IS_USER_CREATED=`mysql -u root --skip-column-names  -e "select count(*) from mysql.user where user='devportal' and host='localhost'"`

  if [ $IS_USER_CREATED -eq 0 ]; then
    display "Creating MySQL user ${PORTAL_DB_USERNAME}..."
    mysql -u root  -e "CREATE USER '${PORTAL_DB_USERNAME}'@'localhost' IDENTIFIED BY '${PORTAL_DB_USER_PASSWORD}';"
    mysql -u root  -e "GRANT ALL ON ${PORTAL_DB_NAME}.* TO '${PORTAL_DB_USERNAME}'@'localhost';"
    mysql -u root  -e 'FLUSH PRIVILEGES;'
  else
    display "MySQL user ${PORTAL_DB_USERNAME} already exists, updating password."
    mysql -u root  -e "SET PASSWORD FOR '${PORTAL_DB_USERNAME}'@'localhost' = PASSWORD('${PORTAL_DB_USER_PASSWORD}');"
  fi
else
  display_header "

  MySQL server has not been installed, so you will need to supply the connection information. The
  database user and database must already be created. The database should also be empty.

  Please make sure the database user has the following rights:
    SELECT, INSERT, UPDATE, DELETE, CREATE, DROP, INDEX, ALTER, CREATE TEMPORARY TABLES, LOCK TABLES.

"

  # Keep looping until user can connect
  IS_VALID_MYSQL_USER=0
  while [ $IS_VALID_MYSQL_USER -eq 0 ]; do
    question "What is the MySQL database host?" PORTAL_DB_HOSTNAME String "localhost"
    question "What is the MySQL database port?" PORTAL_DB_PORT Port "3306"
    question "What is the MySQL database name?" PORTAL_DB_NAME String "devportal"
    question "What is the MySQL database username Dev Portal should connect with?" PORTAL_DB_USERNAME String "devportal"
    question "Enter the MySQL password for the user ${PORTAL_DB_USERNAME}" PORTAL_DB_USER_PASSWORD String

    # Turn off error handling
    trap '' ERR
    trap '' TERM HUP

    SQL_RESULT=`mysql -u ${PORTAL_DB_USERNAME} -p${PORTAL_DB_USER_PASSWORD} -h ${PORTAL_DB_HOSTNAME} -P ${PORTAL_DB_PORT} --skip-column-names -e "SHOW DATABASES LIKE '${PORTAL_DB_NAME}'"` >> $LOGFILE 2>&1

    # Turn error handling back on
    register_exception_handler

    if [ "${SQL_RESULT}" == "${PORTAL_DB_NAME}" ]; then
      display "User $PORTAL_DB_ROOT_USER_NAME can connect to MySQL server."
      IS_VALID_MYSQL_USER=1
    else
      display_error "User $PORTAL_DB_ROOT_USER_NAME cannot connect to MySQL server."
      IS_VALID_MYSQL_USER=0
    fi
  done
fi
display_step "Creating Dev Portal DB ${DRUPAL_WEBAPP}/sites/default/settings.php file..."

mkdir --mode=755 -p ${DRUPAL_WEBAPP}/sites/default/{public,tmp,private}

# Create drupal site config
(
echo '<?php'
echo '$databases = array('
echo "  'default' => array ("
echo "    'default' => array ("
echo "	    'database' => '${PORTAL_DB_NAME}',"
echo "	    'username' => '${PORTAL_DB_USERNAME}',"
echo "	    'password' => '${PORTAL_DB_PASSWORD}',"
echo "	    'host' => '${PORTAL_DB_HOSTNAME}',"
echo "	    'port' => '${PORTAL_DB_PORT}',"
echo "	    'driver' => 'mysql',"
echo "	    'prefix' => '',"
echo "	  ),"
echo "  ),"
echo ");"
echo '$update_free_access = FALSE;'
echo '$drupal_hash_salt = "";'
echo "ini_set('session.gc_probability', 1);"
echo "ini_set('session.gc_divisor', 100);"
echo "ini_set('session.gc_maxlifetime', 200000);"
echo "ini_set('session.cookie_lifetime', 2000000);"
) > "${DRUPAL_WEBAPP}/sites/default/settings.php"

## -----------------------------------------------------
## STEP: Run apigee profile installer
## -----------------------------------------------------
display_step "Installing Drupal portal"
cd ${DRUPAL_WEBAPP}/sites/default

drush site-install apigee  apigee_install_api_endpoint.devconnect_org="${DEVCONNECT_ORG}" \
  apigee_install_api_endpoint.devconnect_endpoint="${DEVCONNECT_ENDPOINT}" \
  apigee_install_api_endpoint.devconnect_curlauth="${DEVCONNECT_AUTH}"

cd $SCRIPT_PATH

cleanup_noerror

