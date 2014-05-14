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

export DRUSH_FILE="php-drush-drush-6.2.0-1.el6.noarch.rpm"

##################### load common functions ####################
source ${SCRIPT_PATH}/tasks/common-functions.sh
source ${SCRIPT_PATH}/tasks/checks.sh

# --------------------------------------------------------------------------------------------------
# STEP: BEGAN INSTALLATION
# --------------------------------------------------------------------------------------------------

# Check RPM bundle directory
if [ ! -d ${SCRIPT_PATH}/bundle/devportal-repo ] ; then
  echo "ERROR: bundle ${SCRIPT_PATH}/bundle does not exist."
  exit 1

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

# -----------------------------------------------------
# STEP: Install Drush
# -----------------------------------------------------
if drush --version | grep -v grep | grep 'Drush Version' > /dev/null  ; then
  display "Drush is already installed!"
  exit 1
fi

display_step "Install Drush"

if [[ ! $HAS_NETWORK == 1 ]] ; then
  display "The following rpm's are required to install Drush:"
  display "${DRUSH_FILE}"
fi

question "Would you like to install Drush on this system?" INSTALL_DRUSH Yn

if [[ $INSTALL_DRUSH = "Y" ]]; then

  if [[ $HAS_NETWORK == 1 ]] ; then
    display_step "Installing Drush ... "
    yum install -y php-drush-drush >> $LOGFILE 2>&1
  else
    found_drush=0
    while [ $found_drush -ne 1 ]; do
      question "In what directory did you download the Drush rpm too?" DRUSH_ROOT String "`pwd`"

      if [[ $DRUSH_ROOT == "Q" || $DRUSH_ROOT == "q" ]]; then
        exit 1
      fi

      top_dir="$( echo $DRUSH_ROOT | cut -d '/' -f1 )"
      if [[ "${top_dir}" = "." || "${top_dir}" = ".." ]] ; then
        echo "Relative paths are not allowed; please enter the absolute path."
        found_drush=2;
      elif [ -d ${DRUSH_ROOT}/ ] ; then
        found_drush=1

        if [ ! -f $DRUSH_ROOT/$DRUSH_FILE ] ; then
            display_error "The required rpm ${DRUSH_FILE} does not exist in ${DRUSH_ROOT}.";
            found_drush=0
        else
            cp $DRUSH_ROOT/$DRUSH_FILE ${BUNDLE_ROOT}/bundle/devportal-repo
        fi

      else
        display_error "${DRUSH_ROOT} is not a valid directory path"
      fi
    done

    display_step "Installing Drush
    Please be patient, this could take several minutes..."
    yum --disablerepo=\* --enablerepo=devportal -y --nogpgcheck install php-drush-drush >> $LOGFILE 2>&1
  fi

  if drush --version | grep -v grep | grep 'Drush Version' > /dev/null  ; then
    display "Drush successfully installed!"
  else
    display_error "Drush installation failed.  Please refer to logs at:
    ${LOGFILE}"
  fi
  
fi

# backup log file
cp $LOGFILE $LOGFILE.$SCRIPT_RUNDATE

