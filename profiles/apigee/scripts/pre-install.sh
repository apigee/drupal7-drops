#!/bin/bash

###############################################################################
# pre-install.sh - This script is used to install Dev Portal on either CentOS
# or Redhat Enterprise Linux 6.x.
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

# Variables set in Determine System Step
export PLATFORM_NAME=
export PLATFORM_MAJOR_VERSION=
export PLATFORM_ARCHITECTURE=

export IUS_BASE_URL="dl.iuscommunity.org/pub/ius/stable"

export HTTPD_CONF_DIR="/etc/httpd"
export HTTPD_VHOST_DIR_NAME="vhosts"

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

  To set up your Drupal instance, go to http://${PORTAL_HOSTNAME}/install.php in
  a browser. Your site will not be operative until you do so.

  The actions of this installer are written to a log here: ${LOGFILE}
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

install_or_update() {
  packages=$@
  updates=''
  installs=''
  for pkg in $packages; do
    rpm -q $pkg 2>&1 > /dev/null && update=1 || update=0
    if [ "$update" = "1" ] ; then
      updates="$updates $pkg"    
    else
      installs="$installs $pkg" 
    fi
  done
  if [ "${updates}" != "" ] ; then
    display "Updating ${updates}"
    yum update -y ${updates} 2>&1 >> $LOGFILE
  fi
  if [ "${installs}" != "" ] ; then
    display "Installing ${installs}"
    yum install -y ${installs} 2>&1 >> $LOGFILE
  fi
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
        yN)
            question_message="${question_message?}[y/N] "
            ;;
        yn)
            question_message="${question_message?}[y/n] "
            ;;
        cr)
            question_message="${question_message?}[c/r] "
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
        Password*)
            if [ ! -z "${question_default?}" ]; then
                question_message="${question_message?}[Default: ${question_default?}] "
            fi
            ;;
        Email)
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

            if [ "${question_kind?}" = "Password4" -o "${question_kind?}" = "Password8" ]; then
                read -q question_response; echo
            else
                #statements
                read question_response
            fi

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
            yN)
                if [ y = "${question_response?}" -o Y = "${question_response?}" ]; then
                    question_answer=y
                    question_success=y
                elif [ -z "${question_response?}" -o n = "${question_response?}" -o N = "${question_response?}" ]; then
                    question_answer=n
                    question_success=y
                else
                    display_error 'Answer must be either "y", "n" or <ENTER> for "n"'
                fi
                ;;
            yn)
                if [ y = "${question_response?}" -o Y = "${question_response?}" ]; then
                    question_answer=y
                    question_success=y
                elif [ n = "${question_response?}" -o N = "${question_response?}" ]; then
                    question_answer=n
                    question_success=y
                else
                    display_error 'Answer must be either "y", "n"'
                fi
                ;;
            cr)
                if [ c = "${question_response?}" -o C = "${question_response?}" ]; then
                    question_answer=c
                    question_success=y
                elif [ r = "${question_response?}" -o R = "${question_response?}" ]; then
                    question_answer=r
                    question_success=y
                else
                    display_error 'Answer must be either "c", "r"'
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
            StringForceLowerCase)
                if [ -z "${question_response?}" -a ! -z "${question_default?}" ]; then
                    question_answer="$(echo "${question_default?}" | tr '[A-Z]' '[a-z]')"
                    question_success=y
                elif [ ! -z ${question_response?} ]; then
                    question_answer="$(echo "${question_response?}" | tr '[A-Z]' '[a-z]')"
                    question_success=y
                else
                    display_error 'Answer must be a string'
                fi
                ;;
            StringDNSName)
                if [ -z "${question_response?}" -a ! -z "${question_default?}" ]; then
                    question_answer="${question_default?}"
                    question_success=y
                elif [ ! -z ${question_response?} ] && echo "${question_response?}" | ${PLATFORM_EGREP?} -v '[:;()_`\"\\ ]' | ${PLATFORM_EGREP?} -qv "[']"; then
                    question_answer="${question_response?}"
                    question_success=y
                else
                    display_error 'Answer must be a valid string of DNS names (use , to separate names)'
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
            Password4)
                if [ 1 = "${question_defined?}" ]; then
                    read -s -r -p "Confirm Password: " question_response_confirm; echo
                    if [ "${question_response?}" = "${question_response_confirm?}" ]; then
                        question_answer="${question_response?}"
                        LEN=${#question_answer}
                        if [ $LEN -lt 4 ]; then
                            question_success=n
                            display_error 'Password must be a minimum of 4 characters'
                        else
                            question_success=y
                        fi
                    else
                        display_error 'Password mismatch: Please try again'
                    fi
                else
                    question_answer="${question_response?}"
                    question_success=y
                fi
                ;;
            Password8)
                if [ 1 = "${question_defined?}" ]; then
                    read -s -r -p "Confirm Password: " question_response_confirm; echo
                    if [ "${question_response?}" = "${question_response_confirm?}" ]; then
                        question_answer="${question_response?}"
                        LEN=${#question_answer}
                        if [ $LEN -lt 8 ]; then
                            question_success=n
                            display_error 'Password must be a minimum of 8 characters'
                        else
                            question_success=y
                        fi
                    else
                        display_error 'Password mismatch: Please try again'
                    fi
                else
                    question_answer="${question_response?}"
                    question_success=y
                fi
                ;;
            Email)
                if echo "${question_response}" | ${PLATFORM_EGREP?} -q '^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]+$' ; then
                    question_success=y
                    question_answer="${question_response?}"
                else
                    display_error 'Answer must be a valid email address that matches username@example.com'
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
mkdir -p ${SCRIPT_PATH}/tmp
DRUPAL_WEBROOT="$( cd -P ${SCRIPT_PATH}/../../.. && pwd )"

#register_exception_handler
register_exception_handler

# Make sure script is run a root.
if [ "$(id -u)" != "0" ]; then
   display_error "This script must be run as root" 
   exit 1
fi

echo -n "" > $LOGFILE

display_header "

 This script will install a default Apigee Portal on this server.

"

question "Press RETURN to continue..." DISCARD_ME StringOrBlank

# ---------------------------------------------------------
# STEP: Make sure install script can get to install files
# ---------------------------------------------------------
display_step "Validating Dev Portal install files exist"

if [[ ! -f ${DRUPAL_WEBROOT}/buildInfo ]] ; then
    display_error "This script expects the Dev Portal files to exist in the
      ${DRUPAL_WEBROOT} directory."
    exit 1
fi

# -----------------------------------------------------
# STEP: Make sure IUS Community location is available
# -----------------------------------------------------
display_step "Validating network is available"

# we don't know yet if wget is installed.
until curl -f -s -X HEAD http://${IUS_BASE_URL}; do
    display_header "
Could not reach EPEL repo at: http://${IUS_BASE_URL}

Please make sure this computer is properly connected to the internet.
"
    question "Press RETURN to try again..." DISCARD_ME StringOrBlank
done

# -----------------------------------------------------
# STEP: Determine System
# -----------------------------------------------------
display_step "Determining OS Details"

# Make sure we can check if system is RHEL or CENTOS
if [[ ! -f /etc/redhat-release ]] ; then
    display_error "The Server does not have a /etc/redhat-release file, cannot determine OS type."
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
# STEP: Install wget if not installed already
# -----------------------------------------------------
display_step "Installing wget"
if [ ! -f /usr/bin/wget ] ; then
  display "wget not found; installing."
  yum install -y wget >> $LOGFILE 2>&1
else
  display "wget is already installed."
fi

# -----------------------------------------------------
# STEP: Install IUS Community & EPEL Repo
# -----------------------------------------------------
display_step "Installing IUS Community & EPEL Repo"

# Install EPEL Repo if needed.
if [ `rpm -qa  | grep -c 'epel-release'` -eq 1 ] ; then
    display "EPEL Repo is already installed."
else
    display "Installing EPEL Repo..."
    wget --directory-prefix=${SCRIPT_TEMP_DIR}  --quiet -r -A "epel-release-*.rpm" --level=1 --no-directories --no-parent http://dl.iuscommunity.org/pub/ius/stable/${PLATFORM_NAME}/${PLATFORM_MAJOR_VERSION}/${PLATFORM_ARCHITECTURE} >> $LOGFILE 2>&1
    rpm -ivh ${SCRIPT_TEMP_DIR}/epel-release*.rpm >> $LOGFILE 2>&1
    yum clean all >> $LOGFILE 2>&1
fi

# Install IUS Community Repo if needed.
if [ `rpm -qa  | grep -c 'ius-release'` -eq 1 ] ; then
    display "IUS Community Repo is already installed."
else
    display "Installing IUS Community Repo..."
    wget --directory-prefix=${SCRIPT_TEMP_DIR} --quiet -r -A "ius-release-*.rpm" --level=1 --no-directories --no-parent http://dl.iuscommunity.org/pub/ius/stable/${PLATFORM_NAME}/${PLATFORM_MAJOR_VERSION}/${PLATFORM_ARCHITECTURE} >> $LOGFILE 2>&1
    rpm -ivh ${SCRIPT_TEMP_DIR}/ius-release*.rpm >> $LOGFILE 2>&1
    yum clean all >> $LOGFILE 2>&1
fi

# -----------------------------------------------------
# STEP: Install RPM Packages
# -----------------------------------------------------
display_step "Installing MySQL client libraries and Apache Web Server"
install_or_update httpd mysql

display_step "Installing PHP and PHP modules..."
install_or_update php54 php54-devel php54-mysql php54-pdo php54-mcrypt php54-mbstring php54-process php54-tidy php54-xmlrpc php54-xml php54-gd php54-pear

# -----------------------------------------------------
# STEP: Set PHP's timezone
# -----------------------------------------------------
if [ `egrep -c '^;date.timezone =$' /etc/php.ini` -eq 1 ] ; then
  display_step "Setting up the timezone in php.ini"
  php_timezone=`egrep '^ZONE=' /etc/sysconfig/clock | cut -d '"' -f2`
  sed -i -e "s:^;date.timezone =:date.timezone = ${php_timezone}:g" /etc/php.ini
fi

display_step "Installing dependency modules..."
install_or_update  gd gd-devel openssl openssl-devel ImageMagick ImageMagick-devel

display_step "Installing tools needed for building PECL modules..."
install_or_update gcc make

#TODO: do we need to install augeas and git?
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
    install_or_update mysql-server
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


# -----------------------------------------------------
# STEP: Upgrade PEAR
# -----------------------------------------------------
display_step "Upgrading PEAR"
pear upgrade -f -a pear >> $LOGFILE 2>&1
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
# STEP: Install PECL RPMs
# -----------------------------------------------------
display_step "Installing PECL RPMs"
if [[ $PLATFORM_NAME == "Redhat" ]]; then
  # There is no php54-pecl-ssh2 package for Red Hat (yet).
  install_or_update php54-pecl-apc php54-pecl-imagick
else
  install_or_update php54-pecl-apc php54-pecl-imagick php54-pecl-ssh2
fi

# -----------------------------------------------------
# STEP: Restarting MySQL
# -----------------------------------------------------
if [[ $INSTALL_MYSQL_SERVER == "Y" ]]; then
  display_step " Starting/Restarting MySQL"
  service mysqld start >> $LOGFILE 2>&1
fi

# -----------------------------------------------------
# STEP: Configure SELinux policy if necessary
# -----------------------------------------------------
display_step "Configuring SELinux policies"
if [[ -f /usr/sbin/getsebool && `getsebool httpd_can_network_connect | cut -d " " -f3` = 'off' ]] ; then
  display "Setting SELinux policy for outgoing httpd network connections..."
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
  cp ${SCRIPT_PATH}/resources/devportal.conf ${HTTPD_VHOST_DIR_PATH}
  sed -i "s:_DEVPORTAL_INSTALL_DIR_:${DRUPAL_WEBROOT}:g" ${HTTPD_VHOST_DIR_PATH}/devportal.conf
  HTTPD_RELOAD=1
fi

# -----------------------------------------------------
# STEP: Reloading Apache HTTPD
# -----------------------------------------------------
if [ "$HTTPD_RELOAD" = 1 ]; then
  echo "Restarting Apache"
  if [ `service httpd status | grep -c 'stopped'` -eq 1 ] ; then
    service httpd start
  else
    service httpd restart
  fi
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
else
  display_header "

  MySQL server has not been installed, so you will need to supply the connection information. The
  database user and database must already be created. The database should also be empty.

  Please make sure the database user has the following rights:
    SELECT, INSERT, UPDATE, DELETE, CREATE, DROP, INDEX, ALTER,CREATE TEMPORARY TABLES, LOCK TABLES.

"

  # Keep looping until user can connect
  IS_VALID_MYSQL_USER=0
  while [ $IS_VALID_MYSQL_USER -eq 0 ]; do

  question "What is the MySQL database host?" PORTAL_DB_HOSTNAME String "localhost"
  question "What is the MySQL database port?" PORTAL_DB_PORT String "3306"
  question "What is the MySQL database name?" PORTAL_DB_NAME String "devportal"
  question "What is the MySQL database username Dev Portal should connect with?" PORTAL_DB_USER_NAME String "devportal"
  question "Enter the MySQL password for the user ${PORTAL_DB_USER_NAME}" PORTAL_DB_USER_PASSWORD String

  # Turn off error handling
  trap '' ERR
  trap '' TERM HUP

  SQL_RESULT=`mysql -u ${PORTAL_DB_USER_NAME} -p${PORTAL_DB_USER_PASSWORD} -h ${PORTAL_DB_HOSTNAME} -P ${PORTAL_DB_PORT} --skip-column-names -e "SHOW DATABASES LIKE '${PORTAL_DB_NAME}'"`   >> $LOGFILE 2>&1

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
display_step "Creating Dev Portal DB ${DRUPAL_WEBROOT}/sites/default/settings.php file..."

mkdir --mode=755 -p ${DRUPAL_WEBROOT}/sites/default/{public,tmp,private}

# Create drupal site config
(
echo '<?php'
echo '$databases = array('
echo "  'default' => array ("
echo "    'default' => array ("
echo "	    'database' => '${PORTAL_DB_NAME}',"
echo "	    'username' => '${PORTAL_DB_USER_NAME}',"
echo "	    'password' => '${PORTAL_DB_USER_PASSWORD}',"
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
) > "${DRUPAL_WEBROOT}/sites/default/settings.php"

## -----------------------------------------------------
## STEP: Dev Portal hostname configuration
## -----------------------------------------------------
display_step "Dev Portal hostname configuration"

SERVER_HOSTNAME=$(hostname)
question "Enter the hostname or IP that Dev Portal should use:" PORTAL_HOSTNAME String $SERVER_HOSTNAME

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

drush site-install apigee  apigee_install_api_endpoint.devconnect_org="${DEVCONNECT_ORG}" \
    apigee_install_api_endpoint.devconnect_endpoint="${DEVCONNECT_ENDPOINT}" \
    apigee_install_api_endpoint.devconnect_curlauth="${DEVCONNECT_AUTH}"

cd $SCRIPT_PATH

cleanup_noerror
