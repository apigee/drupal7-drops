#!/bin/bash

###############################################################################
# standalone-bundle-installer.sh - This script is used to install Dev Portal on
# either CentOS or Redhat Enterprise Linux 6.x without a network connection.
###############################################################################

export OPDK_STANDALONE=1

# Get directory this script is running in
SOURCE="${BASH_SOURCE[0]}"
while [ -h "$SOURCE" ]; do # resolve $SOURCE until the file is no longer a symlink
  DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"
  SOURCE="$(readlink "$SOURCE")"
  [[ $SOURCE != /* ]] && SOURCE="$DIR/$SOURCE" # if $SOURCE was a relative symlink, we need to resolve it relative to the path where the symlink file was located
done

export SCRIPT_PATH="$( cd -P "$( dirname "$SOURCE" )" && pwd )"

source ${SCRIPT_PATH}/common-functions.sh

found_bundle=0
while [[ $found_bundle -ne 1 ]]; do
  question "In what directory did you untar the install bundle?" BUNDLE_ROOT String "`pwd`"
  top_dir="$( echo $BUNDLE_ROOT | cut -d '/' -f1 )"
  if [[ "${top_dir}" = "." || "${top_dir}" = ".." ]] ; then
    echo "Relative paths are not allowed; please enter the absolute path."
    found_bundle=2;
  elif [[ -d ${BUNDLE_ROOT}/bundle/devportal-repo ]] ; then
    found_bundle=1  
  fi
  if [[ $found_bundle -eq 0 ]]; then
    display "Bundle was not found in $BUNDLE_ROOT; please check and try again."
  fi
done

if [[ ! -f ${BUNDLE_ROOT}/system-architecture ]] ; then
  display_error "The bundle was created with an older version of standalone-bundle-creator.sh.\
    Please re-create the bundle with the latest version and try again."
  exit 1
fi
BUNDLE_ARCHITECTURE="$( cat ${BUNDLE_ROOT}/system-architecture )"
SYSTEM_ARCHITECTURE="$( echo "${PLATFORM_NAME}-${PLATFORM_MAJOR_VERSION}-${PLATFORM_ARCHITECTURE}" | tr '[A-Z]' '[a-z]' )"
if [[ "$BUNDLE_ARCHITECTURE" != "$SYSTEM_ARCHITECTURE" ]]; then
  display_error "The bundle was created for a different architecture.\
  Bundle architecture: ${BUNDLE_ARCHITECTURE}\
  System architecture: ${SYSTEM_ARCHITECTURE}"
  exit 1
fi

# Make sure Red Hat is properly registered
#source ${SCRIPT_PATH}/tasks/validate-rhn-repos.sh

echo "*************************************************************************"
echo " This script will install a default Apigee Portal here:"
echo " $DRUPAL_WEBROOT"
echo "*************************************************************************"

source ${SCRIPT_PATH}/tasks/configure-devportal-repo.sh
source ${SCRIPT_PATH}/tasks/install-required-packages.sh
source ${SCRIPT_PATH}/tasks/install-apigee-rpm.sh
source ${SCRIPT_PATH}/tasks/set-php-timezone.sh
source ${SCRIPT_PATH}/tasks/configure-selinux.sh
source ${SCRIPT_PATH}/tasks/configure-vhost.sh
source ${SCRIPT_PATH}/tasks/install-mysql.sh
source ${SCRIPT_PATH}/tasks/install-pear-bundle.sh

## -----------------------------------------------------
## STEP: Run apigee profile installer
## -----------------------------------------------------
display_step "Dev Portal Drupal configuration"

question "Would you like to run the install/configure process for your portal now?
(If you have already installed and configured a portal for this database server,
choose No.)" INSTALL_PROFILE Yn

if [[ $INSTALL_PROFILE = "Y" ]]; then
  export SKIP_CONNECTION_TEST="Y"
  source ${SCRIPT_PATH}/tasks/configure-apigee-profile.sh
fi

cleanup_noerror

