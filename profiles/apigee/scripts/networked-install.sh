#!/bin/bash

###############################################################################
# networked-install.sh - This script is used to install Dev Portal on either
# CentOS or Redhat Enterprise Linux 6.x.
###############################################################################


###############################################################################
#
# Non-Configurable Variables
#
###############################################################################
# Changing the values of any of the variables below could make your script 
# malfunction. Alter them at your own peril.
###############################################################################

export LOGFILE="/var/log/apigee-drupal-install.log"
export WHEREAMI=`pwd`

# Variables set in Determine System Step
export PLATFORM_NAME=
export PLATFORM_MAJOR_VERSION=
export PLATFORM_ARCHITECTURE=

export HTTPD_CONF_DIR="/etc/httpd"
export HTTPD_VHOST_DIR_NAME="vhosts"
export DRUPAL_WEBROOT="/var/www/html"


# Get the date of script running
export SCRIPT_RUNDATE="$(date '+%Y-%m-%d-%H.%M.%S')"

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

export RPM_LOCAL_PATH=${SCRIPT_PATH}/bundle/devportal-repo

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

  Install directory: ${DRUPAL_WEBROOT}
  Database connection settings: ${DRUPAL_WEBROOT}/sites/default/settings.php
  Database name: ${PORTAL_DB_NAME}
  Database user: ${PORTAL_DB_USER_NAME}

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

# ------------------------------------------------
# SCRIPT START
# ------------------------------------------------

# Turn of case sensitive matching for our string compares
shopt -s nocasematch

# Get the date of script running
export SCRIPT_RUNDATE="$(date '+%Y-%m-%d-%H.%M.%S')"


# Make sure script is run a root.
if [ "$(id -u)" != "0" ]; then
  display_error "This script must be run as root" 
  exit 1
fi

# initialize empty logfile
echo -n "" > $LOGFILE

display_header "

 This script will install a default Apigee Portal on this server.

"
question "Press ENTER to continue..." DISCARD_ME StringOrBlank

if [ ! -d $RPM_LOCAL_PATH ] ; then
  mkdir -p $RPM_LOCAL_PATH
fi
RPM_FILENAME="$( cd $RPM_LOCAL_PATH; shopt -s nullglob; echo apigee-drupal-*.rpm )"
if [ -z $RPM_FILENAME ]; then
  downloaded_rpm=0
  display "You need to download the Apigee Drupal RPM. Please ask your Apigee salesperson"
  display "for the correct download URL. Supported protocols include http, https, ftp,"
  display "ftps, sftp, scp, and file. If you have the RPM for the devportal, please put"
  display "it in the following folder:"
  display "  ${RPM_LOCAL_PATH}/apigee-drupal-*.rpm"
  display "and then re-run this script."
  
  # curl handles all of the above protocols.
  while [ $downloaded_rpm -eq 0 ]; do
    question "Enter the download URL:" RPM_DOWNLOAD_URL String
    question "Enter username, if necessary:" RPM_DOWNLOAD_USER StringOrBlank
    if [ ! -z $RPM_DOWNLOAD_USER ] ; then
      question "Enter password:" RPM_DOWNLOAD_PASS String
    else
      RPM_DOWNLOAD_PASS=
    fi
    RPM_FILENAME="$( basename $RPM_DOWNLOAD_URL )"
    
    if [ -z $RPM_DOWNLOAD_PASS ] ; then
      curl -L -k -o ${RPM_LOCAL_PATH}/${RPM_FILENAME} $RPM_DOWNLOAD_URL && downloaded_rpm=1    
    else
      curl -L -k -u "${RPM_DOWNLOAD_USER}:${RPM_DOWNLOAD_PASS}" -o ${RPM_LOCAL_PATH}/${RPM_FILENAME} $RPM_DOWNLOAD_URL && downloaded_rpm=1
    fi
    if [ $downloaded_rpm -eq 0 ] ; then
      display "Sorry, the URL and/or credentials you gave were not correct; please try again."    	    
    fi
  done
fi

#register_exception_handler
register_exception_handler


# -----------------------------------------------------
# STEP: Make sure IUS Community location is available
# -----------------------------------------------------
display_step "Validating network is available"

# we don't know yet if wget is installed.
until curl -f -s -X HEAD -H "Connection: Close" http://www.google.com/ ; do
  display_header "
Could not access the network.
Please make sure this computer is properly connected to the internet.
"
  question "Press ENTER to try again..." DISCARD_ME StringOrBlank
done

# -----------------------------------------------------
# STEP: Determine System
# -----------------------------------------------------
display_step "Determining OS Details"

# Make sure we can check if system is RHEL or CENTOS
if [[ ! -f /etc/redhat-release ]] ; then
  display_error "The server does not have a /etc/redhat-release file; cannot determine OS type."
  exit 1
fi

# Determine RHEL or CentOS
if [[ -f /etc/system-release && `cat /etc/system-release | cut -d " " -f1` == 'CentOS' ]] ; then
  PLATFORM_NAME="CentOS"
  PLATFORM_MAJOR_VERSION=`cat /etc/system-release | cut -d " " -f3 | cut -d. -f1`
elif [[ -f /etc/redhat-release && `cat /etc/redhat-release | cut -d " " -f1` == "Red" ]] ; then
  PLATFORM_NAME="Redhat"
  PLATFORM_MAJOR_VERSION=`cat /etc/redhat-release | cut -d " " -f7 | cut -d. -f1`
else
  display_error "The Server is not running Redhat or CentOS Linux. Only Redhat and CentOS Linux is supported at this time."
  exit 1
fi

display "Platform Name: ${PLATFORM_NAME}"
display "Platform Major Version: ${PLATFORM_MAJOR_VERSION}"

# Determine Archtecture
PLATFORM_ARCHITECTURE=`uname -i`
display "Platform Architecture: ${PLATFORM_ARCHITECTURE}"

# -----------------------------------------------------
# STEP: IS CentOS Base Repo or RHN available?
# -----------------------------------------------------
display_step "Validating Yum Repositories"

# If RHEL, check to make sure system is registered with RHN
if [[ $PLATFORM_NAME == "Redhat" ]]; then
  # If rhn_check comes back w/error, the system is not registered.
  if [ ! -f /etc/sysconfig/rhn/systemid ] ; then
    display_error "The server is not registered with the RedHat network. Please register your system with RHN using the rhn_register command and restart this script."
    exit 1
  else
    display "System is registered on RHN."
  fi

  # Make sure the RHN server-optional channel is registered
  if [ `rhn-channel -l | grep -c 'server-optional'` -eq 1 ] ; then
    display "System is registered to RHN channel server-optional."
  else
    display_error "The server is not registered to the server-optional channel. Please register the server-optional RHN channel restart this script."
    display_header "

You can register the server-optional channel by using the following command:

  rhn-channel --add --channel=<channel-name> --user=<rhn-username> --password=<rhn-password>

To find the <channel-name>, use the following command:

  rhn-channel -L -u <rhn-username> -p <rhn-password> | grep server-optional

"
    exit 1
  fi
fi

# -----------------------------------------------------
# STEP: Install wget
# -----------------------------------------------------
display_step "Installing wget"
yum -y install wget

# Install EPEL Repo if needed. This is required for php-mcrypt
if [ `rpm -qa  | grep -c 'epel-release'` -eq 0 ] ; then
  display_step "Installing EPEL Repo"
  wget --directory-prefix=${SCRIPT_TEMP_DIR}  --quiet -r -A "epel-release-*.rpm" --level=1 --no-directories --no-parent http://dl.iuscommunity.org/pub/ius/stable/${PLATFORM_NAME}/${PLATFORM_MAJOR_VERSION}/${PLATFORM_ARCHITECTURE} >> $LOGFILE 2>&1
  rpm -ivh ${SCRIPT_TEMP_DIR}/epel-release*.rpm >> $LOGFILE 2>&1
  yum clean all >> $LOGFILE 2>&1
fi

# -----------------------------------------------------
# STEP: Install required packages
# -----------------------------------------------------
display_step "Installing required packages"
yum install -y php php-gd php-mbstring php-pdo php-xml php-mysql php-devel \
  php-mcrypt php-pear gcc make httpd mysql pcre-devel php-pecl-apc

# -----------------------------------------------------
# STEP: Set PHP's timezone
# -----------------------------------------------------
if [ `egrep -c '^;date.timezone =$' /etc/php.ini` -eq 1 ] ; then
  display_step "Setting up the timezone in php.ini"
  php_timezone=`egrep '^ZONE=' /etc/sysconfig/clock | cut -d '"' -f2`
  sed -i -e "s:^;date.timezone =:date.timezone = ${php_timezone}:g" /etc/php.ini
fi

has_valid_settings="n";
db_on_localhost="n";
if [ -f ${DRUPAL_WEBROOT}/sites/default/settings.php ] ; then
  PORTAL_DB_NAME=`grep "'database'" ${DRUPAL_WEBROOT}/sites/default/settings.php | cut -d "'" -f4`
  PORTAL_DB_USER_NAME=`grep "'username'" ${DRUPAL_WEBROOT}/sites/default/settings.php | cut -d "'" -f4`
  PORTAL_DB_USER_PASSWORD=`grep "'password'" ${DRUPAL_WEBROOT}/sites/default/settings.php | cut -d "'" -f4`
  PORTAL_DB_HOSTNAME=`grep "'host'" ${DRUPAL_WEBROOT}/sites/default/settings.php | cut -d "'" -f4`
  PORTAL_DB_PORT=`grep "'port'" ${DRUPAL_WEBROOT}/sites/default/settings.php | cut -d "'" -f4`
  if [[ -n $PORTAL_DB_NAME && -n $PORTAL_DB_USER_NAME && -n $PORTAL_DB_USER_PASSWORD && -n $PORTAL_DB_HOSTNAME && -n $PORTAL_DB_PORT ]] ; then
    mysql -u $PORTAL_DB_USER_NAME -p${PORTAL_DB_USER_PASSWORD} -h $PORTAL_DB_HOSTNAME -P $PORTAL_DB_PORT -D $PORTAL_DB_NAME -e 'SHOW TABLES' > /dev/null 2>&1 && has_valid_settings="y"
  fi
  if [ $has_valid_settings = "y" ]; then
    question "A valid settings.php file was already found. Do you want to keep these settings?" has_valid_settings Yn
    if [ $has_valid_settings = "Y" ] ; then
      has_valid_settings='y';
    fi
    if [ $has_valid_settings = 'y' ]; then
      # Make sure apigee-drupal-*.rpm doesn't trample on this
      cp ${DRUPAL_WEBROOT}/sites/default/settings.php /tmp
      if [[ $PORTAL_DB_HOSTNAME = 'localhost' || $PORTAL_DB_HOSTNAME = '127.0.0.1' ]]; then
        db_on_localhost="y"
      fi
    fi
  fi
fi

if [ $has_valid_settings == 'y' ] ; then
  INSTALL_MYSQL_SERVER='N'
else
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
    yum install -y mysql-server
  fi
fi

# -----------------------------------------------------
# STEP: Set MySQL and Apache to run at startup
# -----------------------------------------------------
if [[ $INSTALL_MYSQL_SERVER == "Y" || $db_on_localhost == 'y' ]]; then
  display_step "Making sure MySQL and Apache run at startup"
  chkconfig --levels 35 mysqld on >> $LOGFILE 2>&1
  chkconfig --levels 35 httpd on >> $LOGFILE 2>&1
else
  display_step "Making sure Apache runs at startup..."
  chkconfig --levels 35 httpd on >> $LOGFILE 2>&1
fi

# -----------------------------------------------------
# STEP: Upgrade PEAR
# -----------------------------------------------------
display_step "Upgrading PEAR"
pear upgrade -f -a pear >> $LOGFILE 2>&1
display "If APC asks you for options, you can safely choose the default."
sleep 2
pear upgrade-all | tee $LOGFILE 2>&1

# -----------------------------------------------------
# STEP: Install Drush
# -----------------------------------------------------
display_step "Installing Drush"

# Install drush channel if needed
if [ `pear list-channels  | grep -c 'pear.drush.org'` -eq 1 ] ; then
  display "Pear channel pear.drush.org is already installed."
else
  pear config-set auto_discover 1 >> $LOGFILE 2>&1
  pear channel-discover pear.drush.org >> $LOGFILE 2>&1
  pear config-set preferred_state stable >> $LOGFILE 2>&1
fi

if [ `pear info drush/drush | grep -c 'No information found'` -eq 0 ] ; then
  display "Drush is already installed."
else
  pear install -a drush/drush >> $LOGFILE 2>&1
fi

drush -y dl registry_rebuild >> $LOGFILE 2>&1


# -----------------------------------------------------
# STEP: Install Apigee from RPM
# -----------------------------------------------------
display_step "Installing Apigee Dev Portal"
rpm -ivh --replacepkgs ${RPM_LOCAL_PATH}/${RPM_FILENAME}

# -----------------------------------------------------
# STEP: Configure SELinux policy if necessary
# -----------------------------------------------------
display_step "Configuring SELinux policies"
if [[ -f /usr/sbin/getsebool && `getsebool httpd_can_network_connect | cut -d " " -f3` = 'off' ]] ; then
  display "Setting SELinux policy for outgoing httpd network connections..."
  display "(This can take quite a few seconds; please be patient.)"
  setsebool -P httpd_can_network_connect 1
else
  display "Apache can already make outgoing network connections"
fi

# -----------------------------------------------------
# STEP: Configure Virtual host
# -----------------------------------------------------
display_step "Installing Dev Portal Apache Virtualhost"

HTTPD_VHOST_DIR_PATH=${HTTPD_CONF_DIR}/${HTTPD_VHOST_DIR_NAME}

# Create the vhost conf directory to add virtualhosts to
if [ -d $HTTPD_VHOST_DIR_PATH ] ; then
  display "Virtual Host config directory already exists: ${HTTPD_VHOST_DIR_PATH}"
else
  echo "Creating ${HTTPD_VHOST_DIR_PATH}" >> $LOGFILE 2>&1
  mkdir $HTTPD_VHOST_DIR_PATH
fi

# Check to see if Include is already added to httpd.conf
if [ `cat /etc/httpd/conf/httpd.conf | grep "^Include" | grep -c $HTTPD_VHOST_DIR_NAME` -gt 0 ]; then
  display "The httpd.conf file already has an include directive for ${HTTPD_VHOST_DIR_NAME}/*.conf files."
  IS_VHOST_HTTPD_INCLUDE_INSTALLED=1
else
  IS_VHOST_HTTPD_INCLUDE_INSTALLED=0
fi

# Add an include directive to httpd.conf for the vhost dir
if [ $IS_VHOST_HTTPD_INCLUDE_INSTALLED -eq 0 ]; then
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
if [ -e ${HTTPD_VHOST_DIR_PATH}/devportal.conf ] ; then
  display "Virtual Host for Dev Portal already exists: ${HTTPD_VHOST_DIR_PATH}/devportal.conf"
  question "Would you like to reinstall the virtualhost configuration for Dev Portal?" INSTALL_VHOST Yn
else
  INSTALL_VHOST=y
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

# -----------------------------------------------------
# STEP: Database Configuration
# -----------------------------------------------------
display_step "Database Configuration"

if [[ $INSTALL_MYSQL_SERVER == "Y" ]]; then

  question "What is the MySQL database name?" PORTAL_DB_NAME String "devportal"
  question "Enter a MySQL database username Dev Portal should connect with" PORTAL_DB_USER_NAME String "devportal"
  question "Enter a MySQL password for the user ${PORTAL_DB_USER_NAME}" PORTAL_DB_USER_PASSWORD String

  # Make sure database is properly empty. Note that user-to-db associations
  # remain in effect as long as the new db has the same name as the old one.
  mysql -u root -e "DROP DATABASE IF EXISTS ${PORTAL_DB_NAME}";
  mysql -u root -e "CREATE DATABASE ${PORTAL_DB_NAME}";

  # Check to see if user exists
  IS_USER_CREATED=`mysql -u root --skip-column-names  -e "select count(*) from mysql.user where user='devportal' and host='localhost'"`

  if [ $IS_USER_CREATED -eq 0 ]; then
    display "Creating MySQL user ${PORTAL_DB_USER_NAME}..."
    mysql -u root  -e "CREATE USER '${PORTAL_DB_USER_NAME}'@'localhost' IDENTIFIED BY '${PORTAL_DB_USER_PASSWORD}';"
    mysql -u root  -e "GRANT ALL ON ${PORTAL_DB_NAME}.* TO '${PORTAL_DB_USER_NAME}'@'localhost';"
    mysql -u root  -e 'FLUSH PRIVILEGES;'
  else
    display "MySQL user ${PORTAL_DB_USER_NAME} already exists, updating password."
    mysql -u root  -e "SET PASSWORD FOR '${PORTAL_DB_USER_NAME}'@'localhost' = PASSWORD('${PORTAL_DB_USER_PASSWORD}');"
  fi
elif [[ $has_valid_settings != 'y' ]]; then
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
    question "What is the MySQL database username Dev Portal should connect with?" PORTAL_DB_USER_NAME String "devportal"
    question "Enter the MySQL password for the user ${PORTAL_DB_USER_NAME}" PORTAL_DB_USER_PASSWORD String

    # Turn off error handling
    trap '' ERR
    trap '' TERM HUP

    SQL_RESULT=`mysql -u ${PORTAL_DB_USER_NAME} -p${PORTAL_DB_USER_PASSWORD} -h ${PORTAL_DB_HOSTNAME} -P ${PORTAL_DB_PORT} --skip-column-names -e "SHOW DATABASES LIKE '${PORTAL_DB_NAME}'"` >> $LOGFILE 2>&1

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

## -----------------------------------------------------
## STEP: Creating Dev Portal DB settings.php file
## -----------------------------------------------------
if [ $has_valid_settings = 'y' ]; then
  mkdir --mode=755 -p ${DRUPAL_WEBROOT}/sites/default/{public,tmp,private}
  mv /tmp/settings.php ${DRUPAL_WEBROOT}/sites/default/
else
  display_step "Creating Dev Portal DB ${DRUPAL_WEBROOT}/sites/default/settings.php file..."

  mkdir --mode=755 -p ${DRUPAL_WEBROOT}/sites/default/{public,tmp,private}

  # Create drupal site config
  (
  echo '<?php'
  echo '$databases = array('
  echo "  'default' => array ("
  echo "    'default' => array ("
  echo "      'database' => '${PORTAL_DB_NAME}',"
  echo "      'username' => '${PORTAL_DB_USER_NAME}',"
  echo "      'password' => '${PORTAL_DB_USER_PASSWORD}',"
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

## -----------------------------------------------------
## STEP: Configure Dev Portal to connect to KMS.
## -----------------------------------------------------
display_step "Dev Portal Drupal configuration"

endpoint_connect_success=0
DEVCONNECT_ENDPOINT="https://api.enterprise.apigee.com/v1"
DEVCONNECT_ORG="my-org"
DEVCONNECT_AUTH="<username>:<password>"
until [ $endpoint_connect_success -eq 1 ]; do
  question "What is the URI of the Apigee Management API Endpoint:" DEVCONNECT_ENDPOINT String $DEVCONNECT_ENDPOINT
  question "What is the Apigee Organization name:" DEVCONNECT_ORG String $DEVCONNECT_ORG
  question "What is the UN:PW for the management API Endpoint?" DEVCONNECT_AUTH String $DEVCONNECT_AUTH
  
  # k = allow insecure SSL, s = silent, f = fail silently on server error
  curl -k -s -f -X HEAD -u $DEVCONNECT_AUTH "${DEVCONNECT_ENDPOINT}/o/${DEVCONNECT_ORG}" 2>&1 > /dev/null && endpoint_connect_success=1 || endpoint_connect_success=0
  if [ $endpoint_connect_success -eq 0 ]; then
    display "Could not connect to endpoint. Please check your parameters and try again."
  fi
done

## -----------------------------------------------------
## STEP: Run apigee profile installer
## -----------------------------------------------------
display_step "Drupal installation"
cd ${DRUPAL_WEBROOT}/sites/default

## TODO: disable Pantheon-specific modules in the apigee profile.

drush site-install apigee  apigee_install_api_endpoint.devconnect_org="${DEVCONNECT_ORG}" \
  apigee_install_api_endpoint.devconnect_endpoint="${DEVCONNECT_ENDPOINT}" \
  apigee_install_api_endpoint.devconnect_curlauth="${DEVCONNECT_AUTH}"

cd $SCRIPT_PATH

cleanup_noerror
